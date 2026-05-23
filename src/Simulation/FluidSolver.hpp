#pragma once

#include <cstdint>
#include <memory>

#include "../Core/PhysicsTypes.hpp"

struct SDL_GPUCommandBuffer;
struct SDL_GPUTexture;

namespace Graphics
{
class GPUContext;
class TextureRegistry;
class ShaderLibrary;
} // namespace Graphics

namespace Simulation
{
class FluidSolver
{
public:
    FluidSolver(Graphics::GPUContext* context, uint32_t width, uint32_t height);
    ~FluidSolver();

    FluidSolver(const FluidSolver&) = delete;
    FluidSolver& operator=(const FluidSolver&) = delete;
    FluidSolver(FluidSolver&&) noexcept = default;
    FluidSolver& operator=(FluidSolver&&) noexcept = default;

    void Dispatch(SDL_GPUCommandBuffer* cmd, const Core::FluidPhysicsParameters& params);

    [[nodiscard]] SDL_GPUTexture* GetOutputTexture() const;

private:
    Graphics::GPUContext* m_context;

    std::unique_ptr<Graphics::TextureRegistry> m_textureRegistry;
    std::unique_ptr<Graphics::ShaderLibrary> m_shaderLibrary;
};
} // namespace Simulation
