#include "FluidSolver.hpp"

#include <SDL3/SDL.h>

#include <cstdint>
#include <memory>

#include "../Core/Logger.hpp"
#include "../Core/PhysicsTypes.hpp"
#include "../Graphics/GPUContext.hpp"
#include "../Graphics/ShaderLibrary.hpp"
#include "../Graphics/TextureRegistry.hpp"

namespace Simulation
{
FluidSolver::FluidSolver(Graphics::GPUContext* context, uint32_t width, uint32_t height) : m_context(context)
{
    m_textureRegistry = std::make_unique<Graphics::TextureRegistry>(m_context);
    m_shaderLibrary = std::make_unique<Graphics::ShaderLibrary>(m_context);

    m_textureRegistry->CreatePingPong("Dye", width, height, SDL_GPU_TEXTUREFORMAT_R16G16B16A16_FLOAT);

    LOG_INFO("FluidSolver: System initialized");
}

FluidSolver::~FluidSolver() = default;

void FluidSolver::Dispatch(SDL_GPUCommandBuffer* cmd, const Core::FluidPhysicsParameters& params)
{
    (void)cmd;
    (void)params;
}

SDL_GPUTexture* FluidSolver::GetOutputTexture() const
{
    Graphics::PingPongBuffer* dyeBuf = m_textureRegistry->GetBuffer("Dye");
    return dyeBuf ? dyeBuf->GetRead().Handle : nullptr;
}
} // namespace Simulation
