#include "Renderer.hpp"

#include <SDL3/SDL.h>

#include <cstdio>
#include <print>

#include "GPUContext.hpp"

namespace Graphics
{
namespace
{
constexpr float kDefaultClearColor = 0.1F;
constexpr float kOpaqueAlpha = 1.0F;
} // namespace

Renderer::Renderer(GPUContext* context)
    : m_context(context), m_clearColor(kDefaultClearColor, kDefaultClearColor, kDefaultClearColor, kOpaqueAlpha)
{
    std::println("Renderer: System initialized");
}

void Renderer::Draw(double alpha)
{
    (void)alpha;

    SDL_GPUCommandBuffer* cmd = m_context->GetCurrentCommandBuffer();
    SDL_GPUTexture* swapchainTarget = m_context->GetSwapchainTexture();

    if (!cmd || !swapchainTarget)
    {
        return;
    }

    SDL_GPUColorTargetInfo colorInfo{};
    colorInfo.texture = swapchainTarget;
    colorInfo.clear_color = SDL_FColor{m_clearColor.R, m_clearColor.G, m_clearColor.B, m_clearColor.A};
    colorInfo.load_op = SDL_GPU_LOADOP_CLEAR;
    colorInfo.store_op = SDL_GPU_STOREOP_STORE;

    SDL_GPURenderPass* pass = SDL_BeginGPURenderPass(cmd, &colorInfo, 1, nullptr);
    if (pass)
    {
        SDL_EndGPURenderPass(pass);
    }
    else
    {
        std::println(stderr, "Renderer Error: Failed to begin render pass: {}", SDL_GetError());
    }
}

void Renderer::SetClearColor(const Color& color)
{
    m_clearColor = color;
}

void Renderer::SetClearColor(float r, float g, float b, float a)
{
    m_clearColor = {r, g, b, a};
}
} // namespace Graphics
