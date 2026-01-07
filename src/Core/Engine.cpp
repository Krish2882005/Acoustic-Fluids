#include "Engine.hpp"

#include <SDL3/SDL.h>

#include <algorithm>
#include <cstdint>
#include <memory>
#include <stdexcept>

#include "../Graphics/GPUContext.hpp"
#include "../Graphics/Renderer.hpp"
#include "Clock.hpp"
#include "Config.hpp"
#include "Logger.hpp"
#include "Window.hpp"

namespace Core
{
namespace
{
constexpr double kMaxFrameTime = 0.25;
constexpr double kMillisecondsPerSecond = 1000.0;
constexpr double kNanosecondsPerSecond = 1.0e9;
} // namespace

SDLContext::SDLContext()
{
    LOG_INFO("Engine: Initializing SDL...");

    if (!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO))
    {
        throw std::runtime_error(SDL_GetError());
    }
}

SDLContext::~SDLContext()
{
    LOG_INFO("Engine: Shutting down SDL...");
    SDL_Quit();
}

Engine::Engine(const Config& config) : m_config(config), m_isRunning(true)
{
    LOG_INFO("Engine: Initializing Subsystems...");

    m_window = std::make_unique<Window>(config.WindowTitle, config.WindowWidth, config.WindowHeight);

    m_gpuContext = std::make_unique<Graphics::GPUContext>(m_window->GetNativeHandle(), config.EnableGPUDebug);
    m_gpuContext->SetVSync(config.VSync);

    m_renderer = std::make_unique<Graphics::Renderer>(m_gpuContext.get());

    LOG_INFO("Engine: Initialized subsystems!");
}

Engine::~Engine() = default;

void Engine::Run()
{
    const Clock clock;
    double accumulator = 0.0;
    double currentTime = clock.GetTotalSeconds();

    const double maxFrameTime = kMaxFrameTime;

    while (m_isRunning)
    {
        const double newTime = clock.GetTotalSeconds();
        double frameTime = newTime - currentTime;
        currentTime = newTime;

        frameTime = std::min(frameTime, maxFrameTime);

        accumulator += frameTime;

        SDL_Event event;
        while (SDL_PollEvent(&event))
        {
            switch (event.type)
            {
                case SDL_EVENT_QUIT:
                    m_isRunning = false;
                    break;
                case SDL_EVENT_WINDOW_RESIZED:
                    m_window->OnResize(event.window.data1, event.window.data2);
                    break;
                default:
                    break;
            }
        }

        while (accumulator >= Config::kPhysicsTimeStep)
        {
            Update(Config::kPhysicsTimeStep);
            accumulator -= Config::kPhysicsTimeStep;
        }

        const double alpha = accumulator / Config::kPhysicsTimeStep;
        Render(alpha);

        if (!m_config.VSync && m_config.TargetRenderFPS > 0)
        {
            const double targetDuration = 1.0 / static_cast<double>(m_config.TargetRenderFPS);
            const double elapsed = clock.GetTotalSeconds() - newTime;

            if (elapsed < targetDuration)
            {
                auto waitNS = static_cast<uint64_t>((targetDuration - elapsed) * kNanosecondsPerSecond);
                SDL_DelayNS(waitNS);
            }
        }
    }
}

void Engine::Update(double dt)
{
    (void)dt;
}

void Engine::Render(double alpha)
{
    m_gpuContext->BeginFrame();

    m_renderer->Draw(alpha);

    m_gpuContext->EndFrame();
}
} // namespace Core
