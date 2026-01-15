#include "ShaderLibrary.hpp"

#include <SDL3/SDL.h>
#include <SDL3_shadercross/SDL_shadercross.h>

#include <cstddef>
#include <filesystem>
#include <fstream>
#include <ios>
#include <memory>
#include <stdexcept>
#include <string>
#include <utility>

#include "../Core/Logger.hpp"
#include "GPUContext.hpp"
#include "Shader.hpp"

namespace Graphics
{
namespace
{
struct MetadataDestroyer
{
    void operator()(void* ptr) const { SDL_free(ptr); }
};

using SdlGraphicsMetadataPtr = std::unique_ptr<SDL_ShaderCross_GraphicsShaderMetadata, MetadataDestroyer>;
using SdlComputeMetadataPtr = std::unique_ptr<SDL_ShaderCross_ComputePipelineMetadata, MetadataDestroyer>;
} // namespace

void ShaderLibrary::BytecodeDestroyer::operator()(void* ptr) const
{
    SDL_free(ptr);
}

ShaderLibrary::ShaderLibrary(GPUContext* context) : m_context(context)
{
    if (!SDL_ShaderCross_Init())
    {
        throw std::runtime_error("Failed to initialize SDL_ShaderCross");
    }
    LOG_INFO("ShaderLibrary: System initialized");
}

ShaderLibrary::~ShaderLibrary()
{
    m_shaders.clear();
    SDL_ShaderCross_Quit();
    LOG_INFO("ShaderLibrary: System shutdown");
}

Shader* ShaderLibrary::LoadGraphics(const std::string& name, const std::string& path, ShaderStage stage)
{
    if (const auto it = m_shaders.find(name); it != m_shaders.end())
    {
        return it->second.get();
    }

    const std::string source = ReadFile(path);
    auto spirv = CompileToSPIRV(source, stage, path);

    const SdlGraphicsMetadataPtr metadata(
        SDL_ShaderCross_ReflectGraphicsSPIRV(static_cast<Uint8*>(spirv.Bytecode.get()), spirv.Size, 0));

    if (!metadata)
    {
        LOG_ERROR("ShaderLibrary: Reflection failed for '{}'", path);
        throw std::runtime_error("Shader Reflection Failed");
    }

    const SDL_ShaderCross_SPIRV_Info spirvInfo{.bytecode = static_cast<const Uint8*>(spirv.Bytecode.get()),
                                               .bytecode_size = spirv.Size,
                                               .entrypoint = "main",
                                               .shader_stage = (stage == ShaderStage::Vertex)
                                                                   ? SDL_SHADERCROSS_SHADERSTAGE_VERTEX
                                                                   : SDL_SHADERCROSS_SHADERSTAGE_FRAGMENT};

    SDL_GPUShader* handle =
        SDL_ShaderCross_CompileGraphicsShaderFromSPIRV(m_context->GetDevice(), &spirvInfo, &metadata->resource_info, 0);

    if (!handle)
    {
        LOG_ERROR("ShaderLibrary: Creation failed for '{}': {}", path, SDL_GetError());
        throw std::runtime_error("Shader Creation Failed");
    }

    auto shader = std::make_unique<Shader>(m_context->GetDevice(), handle, stage, name);
    Shader* ptr = shader.get();
    m_shaders[name] = std::move(shader);

    LOG_INFO("ShaderLibrary: Loaded Graphics Shader '{}'", name);
    return ptr;
}

Shader* ShaderLibrary::LoadCompute(const std::string& name, const std::string& path)
{
    if (const auto it = m_shaders.find(name); it != m_shaders.end())
    {
        return it->second.get();
    }

    const std::string source = ReadFile(path);
    auto spirv = CompileToSPIRV(source, ShaderStage::Compute, path);

    const SdlComputeMetadataPtr metadata(
        SDL_ShaderCross_ReflectComputeSPIRV(static_cast<Uint8*>(spirv.Bytecode.get()), spirv.Size, 0));

    if (!metadata)
    {
        LOG_ERROR("ShaderLibrary: Reflection failed for Compute '{}'", path);
        throw std::runtime_error("Compute Reflection Failed");
    }

    const SDL_ShaderCross_SPIRV_Info spirvInfo{.bytecode = static_cast<const Uint8*>(spirv.Bytecode.get()),
                                               .bytecode_size = spirv.Size,
                                               .entrypoint = "main",
                                               .shader_stage = SDL_SHADERCROSS_SHADERSTAGE_COMPUTE};

    SDL_GPUComputePipeline* pipeline =
        SDL_ShaderCross_CompileComputePipelineFromSPIRV(m_context->GetDevice(), &spirvInfo, metadata.get(), 0);

    if (!pipeline)
    {
        LOG_ERROR("ShaderLibrary: Pipeline creation failed for '{}': {}", path, SDL_GetError());
        throw std::runtime_error("Compute Pipeline Creation Failed");
    }

    ShaderMetadata meta{.ThreadCountX = metadata->threadcount_x,
                        .ThreadCountY = metadata->threadcount_y,
                        .ThreadCountZ = metadata->threadcount_z};

    auto shader = std::make_unique<Shader>(m_context->GetDevice(), pipeline, meta, name);
    Shader* ptr = shader.get();
    m_shaders[name] = std::move(shader);

    LOG_INFO("ShaderLibrary: Loaded Compute Shader '{}' [Threads: {}x{}x{}]",
             name,
             meta.ThreadCountX,
             meta.ThreadCountY,
             meta.ThreadCountZ);
    return ptr;
}

Shader* ShaderLibrary::Get(const std::string& name)
{
    if (const auto it = m_shaders.find(name); it != m_shaders.end())
    {
        return it->second.get();
    }
    LOG_WARN("ShaderLibrary: Shader '{}' not found", name);
    return nullptr;
}

std::string ShaderLibrary::ReadFile(const std::string& path)
{
    const std::filesystem::path filePath(path);
    if (!std::filesystem::exists(filePath))
    {
        LOG_ERROR("ShaderLibrary: File not found '{}'", path);
        throw std::runtime_error("Shader File Not Found");
    }

    const auto fileSize = std::filesystem::file_size(filePath);
    std::string buffer;
    buffer.resize(fileSize);

    std::ifstream file(filePath, std::ios::binary);
    if (!file.is_open())
    {
        LOG_ERROR("ShaderLibrary: Failed to open file '{}'", path);
        throw std::runtime_error("Shader File Open Failed");
    }

    file.read(buffer.data(), static_cast<std::streamsize>(fileSize));
    return buffer;
}

ShaderLibrary::SpirvResult
ShaderLibrary::CompileToSPIRV(const std::string& source, ShaderStage stage, const std::string& path)
{
    SDL_ShaderCross_HLSL_Info hlslInfo{.source = source.c_str(), .entrypoint = "main"};

    switch (stage)
    {
        case ShaderStage::Vertex:
            hlslInfo.shader_stage = SDL_SHADERCROSS_SHADERSTAGE_VERTEX;
            break;
        case ShaderStage::Fragment:
            hlslInfo.shader_stage = SDL_SHADERCROSS_SHADERSTAGE_FRAGMENT;
            break;
        case ShaderStage::Compute:
            hlslInfo.shader_stage = SDL_SHADERCROSS_SHADERSTAGE_COMPUTE;
            break;
    }

    size_t size = 0;
    void* bytecode = SDL_ShaderCross_CompileSPIRVFromHLSL(&hlslInfo, &size);

    if (!bytecode)
    {
        LOG_ERROR("ShaderLibrary: HLSL compilation failed for '{}'", path);
        throw std::runtime_error("Shader Compilation Failed");
    }

    return SpirvResult{.Bytecode = std::unique_ptr<void, BytecodeDestroyer>(bytecode), .Size = size};
}
} // namespace Graphics
