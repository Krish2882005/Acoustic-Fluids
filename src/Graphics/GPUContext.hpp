#pragma once

#include <memory>

struct SDL_Window;
struct SDL_GPUDevice;
struct SDL_GPUCommandBuffer;
struct SDL_GPUTexture;

namespace Graphics
{
class GPUContext
{
public:
    explicit GPUContext(SDL_Window* window, bool debugMode = false);
    ~GPUContext();

    GPUContext(const GPUContext&) = delete;
    GPUContext& operator=(const GPUContext&) = delete;
    GPUContext(GPUContext&&) noexcept;
    GPUContext& operator=(GPUContext&&) noexcept;

    void BeginFrame();
    void EndFrame();

    void SetVSync(bool enabled);

    [[nodiscard]] SDL_GPUDevice* GetDevice() const;
    [[nodiscard]] SDL_GPUCommandBuffer* GetCurrentCommandBuffer() const;
    [[nodiscard]] SDL_GPUTexture* GetSwapchainTexture() const;

private:
    struct GPUDeviceDestroyer
    {
        void operator()(SDL_GPUDevice* device) const;
    };

    using GPUDevicePtr = std::unique_ptr<SDL_GPUDevice, GPUDeviceDestroyer>;

    GPUDevicePtr m_device;
    SDL_Window* m_windowHandle = nullptr;
    SDL_GPUCommandBuffer* m_currentCmdBuffer = nullptr;
    SDL_GPUTexture* m_swapchainTexture = nullptr;
};
} // namespace Graphics
