#include "Shader.hpp"

#include <SDL3/SDL_gpu.h>

#include <string_view>
#include <utility>
#include <variant>

namespace Graphics
{
Shader::Shader(SDL_GPUDevice* device, SDL_GPUShader* handle, ShaderStage stage, std::string_view name)
    : m_device(device), m_handle(handle), m_stage(stage), m_name(name)
{
}

Shader::Shader(SDL_GPUDevice* device, SDL_GPUComputePipeline* handle, const ShaderMetadata& meta, std::string_view name)
    : m_device(device), m_handle(handle), m_stage(ShaderStage::Compute), m_name(name), m_metadata(meta)
{
}

Shader::~Shader()
{
    Release();
}

Shader::Shader(Shader&& other) noexcept
    : m_device(other.m_device),
      m_handle(other.m_handle),
      m_stage(other.m_stage),
      m_name(std::move(other.m_name)),
      m_metadata(other.m_metadata)
{
    other.m_handle = std::monostate{};
}

Shader& Shader::operator=(Shader&& other) noexcept
{
    if (this != &other)
    {
        Release();
        m_device = other.m_device;
        m_handle = other.m_handle;
        m_stage = other.m_stage;
        m_name = std::move(other.m_name);
        m_metadata = other.m_metadata;
        other.m_handle = std::monostate{};
    }
    return *this;
}

SDL_GPUShader* Shader::GetGraphicsHandle() const
{
    if (const auto* ptr = std::get_if<SDL_GPUShader*>(&m_handle))
    {
        return *ptr;
    }
    return nullptr;
}

SDL_GPUComputePipeline* Shader::GetComputeHandle() const
{
    if (const auto* ptr = std::get_if<SDL_GPUComputePipeline*>(&m_handle))
    {
        return *ptr;
    }
    return nullptr;
}

ShaderStage Shader::GetStage() const
{
    return m_stage;
}

const std::string& Shader::GetName() const
{
    return m_name;
}

const ShaderMetadata& Shader::GetMetadata() const
{
    return m_metadata;
}

void Shader::Release()
{
    if (const auto* graphicsPtr = std::get_if<SDL_GPUShader*>(&m_handle))
    {
        if (*graphicsPtr)
        {
            SDL_ReleaseGPUShader(m_device, *graphicsPtr);
        }
    }
    else if (const auto* computePtr = std::get_if<SDL_GPUComputePipeline*>(&m_handle))
    {
        if (*computePtr)
        {
            SDL_ReleaseGPUComputePipeline(m_device, *computePtr);
        }
    }
    m_handle = std::monostate{};
}
} // namespace Graphics
