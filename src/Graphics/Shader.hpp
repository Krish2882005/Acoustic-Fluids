#pragma once

#include <cstdint>
#include <string>
#include <string_view>
#include <variant>

struct SDL_GPUDevice;
struct SDL_GPUShader;
struct SDL_GPUComputePipeline;

namespace Graphics
{
enum class ShaderStage : std::uint8_t
{
    Vertex,
    Fragment,
    Compute
};

struct ShaderMetadata
{
    uint32_t ThreadCountX = 0;
    uint32_t ThreadCountY = 0;
    uint32_t ThreadCountZ = 0;
};

class Shader
{
public:
    Shader(SDL_GPUDevice* device, SDL_GPUShader* handle, ShaderStage stage, std::string_view name);
    Shader(SDL_GPUDevice* device, SDL_GPUComputePipeline* handle, const ShaderMetadata& meta, std::string_view name);
    ~Shader();

    Shader(const Shader&) = delete;
    Shader& operator=(const Shader&) = delete;
    Shader(Shader&& other) noexcept;
    Shader& operator=(Shader&& other) noexcept;

    [[nodiscard]] SDL_GPUShader* GetGraphicsHandle() const;
    [[nodiscard]] SDL_GPUComputePipeline* GetComputeHandle() const;

    [[nodiscard]] ShaderStage GetStage() const;
    [[nodiscard]] const std::string& GetName() const;
    [[nodiscard]] const ShaderMetadata& GetMetadata() const;

private:
    void Release();

    SDL_GPUDevice* m_device;
    std::variant<std::monostate, SDL_GPUShader*, SDL_GPUComputePipeline*> m_handle;
    ShaderStage m_stage;
    std::string m_name;
    ShaderMetadata m_metadata;
};
} // namespace Graphics
