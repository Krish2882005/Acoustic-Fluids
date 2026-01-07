#pragma once

#include <memory>

#include "Config.hpp"

namespace Graphics
{
class GPUContext;
class Renderer;
} // namespace Graphics

namespace Core
{
class Window;

struct SDLContext
{
    SDLContext();
    ~SDLContext();

    SDLContext(const SDLContext&) = delete;
    SDLContext& operator=(const SDLContext&) = delete;
    SDLContext(SDLContext&&) noexcept = delete;
    SDLContext& operator=(SDLContext&&) noexcept = delete;
};

class Engine
{
public:
    explicit Engine(const Config& config);
    ~Engine();

    Engine(const Engine&) = delete;
    Engine& operator=(const Engine&) = delete;
    Engine(Engine&&) noexcept = delete;
    Engine& operator=(Engine&&) noexcept = delete;

    void Run();

private:
    void Update(double dt);
    void Render(double alpha);

    SDLContext m_sdlContext;
    Config m_config;
    bool m_isRunning = false;

    std::unique_ptr<Window> m_window;
    std::unique_ptr<Graphics::GPUContext> m_gpuContext;
    std::unique_ptr<Graphics::Renderer> m_renderer;
};
} // namespace Core
