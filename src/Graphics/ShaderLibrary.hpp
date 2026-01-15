#pragma once

#include <memory>
#include <string>
#include <unordered_map>

#include "Shader.hpp"

namespace Graphics
{
class GPUContext;

class ShaderLibrary
{
public:
    explicit ShaderLibrary(GPUContext* context);
    ~ShaderLibrary();

    ShaderLibrary(const ShaderLibrary&) = delete;
    ShaderLibrary& operator=(const ShaderLibrary&) = delete;
    ShaderLibrary(ShaderLibrary&&) noexcept = default;
    ShaderLibrary& operator=(ShaderLibrary&&) noexcept = default;

    Shader* LoadGraphics(const std::string& name, const std::string& path, ShaderStage stage);
    Shader* LoadCompute(const std::string& name, const std::string& path);

    [[nodiscard]] Shader* Get(const std::string& name);

private:
    [[nodiscard]] static std::string ReadFile(const std::string& path);

    struct BytecodeDestroyer
    {
        void operator()(void* ptr) const;
    };

    struct SpirvResult
    {
        std::unique_ptr<void, BytecodeDestroyer> Bytecode;
        size_t Size;
    };

    [[nodiscard]] static SpirvResult
    CompileToSPIRV(const std::string& source, ShaderStage stage, const std::string& path);

    GPUContext* m_context;
    std::unordered_map<std::string, std::unique_ptr<Shader>> m_shaders;
};
} // namespace Graphics
