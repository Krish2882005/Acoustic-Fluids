#include "GPUContext.hpp"

#include <SDL3/SDL.h>

#include <cstdio>
#include <print>
#include <stdexcept>

namespace Graphics
{
void GPUContext::GPUDeviceDestroyer::operator()(SDL_GPUDevice* device) const
{
    if (device)
    {
        SDL_DestroyGPUDevice(device);
    }
}

GPUContext::GPUContext(SDL_Window* window, bool debugMode) : m_windowHandle(window)
{
    m_device.reset(SDL_CreateGPUDevice(
        SDL_GPU_SHADERFORMAT_SPIRV | SDL_GPU_SHADERFORMAT_DXIL | SDL_GPU_SHADERFORMAT_METALLIB, debugMode, nullptr));

    if (!m_device)
    {
        throw std::runtime_error(SDL_GetError());
    }

    const char* backend = SDL_GetGPUDeviceDriver(m_device.get());
    std::println("GPU: Device created using backend: {}", backend ? backend : "Unknown");

    if (m_windowHandle)
    {
        if (!SDL_ClaimWindowForGPUDevice(m_device.get(), m_windowHandle))
        {
            throw std::runtime_error(SDL_GetError());
        }
    }
}

GPUContext::~GPUContext()
{
    if (m_device && m_windowHandle)
    {
        SDL_ReleaseWindowFromGPUDevice(m_device.get(), m_windowHandle);
    }
}

GPUContext::GPUContext(GPUContext&&) noexcept = default;
GPUContext& GPUContext::operator=(GPUContext&&) noexcept = default;

void GPUContext::BeginFrame()
{
    if (!m_windowHandle)
    {
        return;
    }

    m_currentCmdBuffer = SDL_AcquireGPUCommandBuffer(m_device.get());

    if (!m_currentCmdBuffer)
    {
        std::println(stderr, "GPU Error: Failed to acquire command buffer: {}", SDL_GetError());
        return;
    }

    if (!SDL_WaitAndAcquireGPUSwapchainTexture(
            m_currentCmdBuffer, m_windowHandle, &m_swapchainTexture, nullptr, nullptr))
    {
        m_swapchainTexture = nullptr;
        std::println(stderr, "GPU Error: Failed to acquire swapchain: {}", SDL_GetError());
    }
}

void GPUContext::EndFrame()
{
    if (m_currentCmdBuffer)
    {
        if (!SDL_SubmitGPUCommandBuffer(m_currentCmdBuffer))
        {
            std::println(stderr, "GPU Error: Submission failed: {}", SDL_GetError());
        }

        m_currentCmdBuffer = nullptr;
        m_swapchainTexture = nullptr;
    }
}

void GPUContext::SetVSync(bool enabled)
{
    if (!m_windowHandle)
    {
        return;
    }

    if (!SDL_SetGPUSwapchainParameters(m_device.get(),
                                       m_windowHandle,
                                       SDL_GPU_SWAPCHAINCOMPOSITION_SDR,
                                       enabled ? SDL_GPU_PRESENTMODE_VSYNC : SDL_GPU_PRESENTMODE_IMMEDIATE))
    {
        std::println(stderr, "GPU Warning: Failed to set VSync: {}", SDL_GetError());
    }
}

SDL_GPUDevice* GPUContext::GetDevice() const
{
    return m_device.get();
}

SDL_GPUCommandBuffer* GPUContext::GetCurrentCommandBuffer() const
{
    return m_currentCmdBuffer;
}

SDL_GPUTexture* GPUContext::GetSwapchainTexture() const
{
    return m_swapchainTexture;
}
} // namespace Graphics
