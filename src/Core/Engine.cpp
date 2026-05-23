#include "Engine.hpp"

#include <SDL3/SDL.h>

#include <algorithm>
#include <cstdint>
#include <memory>
#include <stdexcept>

#include "../Audio/AudioDriver.hpp"
#include "../Audio/AudioRingBuffer.hpp"
#include "../Audio/FFTAnalyzer.hpp"
#include "../Graphics/GPUContext.hpp"
#include "../Graphics/Renderer.hpp"
#include "../Simulation/FluidSolver.hpp"
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
constexpr float kDefaultDissipation = 0.99F;
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

    m_audioRingBuffer = std::make_unique<Audio::AudioRingBuffer>();
    m_audioDriver = std::make_unique<Audio::AudioDriver>(*m_audioRingBuffer);
    m_fftAnalyzer = std::make_unique<Audio::FFTAnalyzer>(*m_audioRingBuffer);

    m_physicsParams.GridResolutionX = static_cast<float>(config.WindowWidth);
    m_physicsParams.GridResolutionY = static_cast<float>(config.WindowHeight);
    m_physicsParams.Dissipation = kDefaultDissipation;

    m_window = std::make_unique<Window>(config.WindowTitle, config.WindowWidth, config.WindowHeight);

    m_gpuContext = std::make_unique<Graphics::GPUContext>(m_window->GetNativeHandle(), config.EnableGPUDebug);
    m_gpuContext->SetVSync(config.VSync);

    m_fluidSolver =
        std::make_unique<Simulation::FluidSolver>(m_gpuContext.get(), config.WindowWidth, config.WindowHeight);
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
    const Audio::FrequencyBands bands = m_fftAnalyzer->ProcessFrame();

    m_physicsParams.SubBass = bands.SubBass;
    m_physicsParams.Bass = bands.Bass;
    m_physicsParams.Mids = bands.Mids;
    m_physicsParams.Treble = bands.Treble;
    m_physicsParams.DeltaTime = static_cast<float>(dt);
}

void Engine::Render(double alpha)
{
    (void)alpha;

    m_gpuContext->BeginFrame();

    SDL_GPUCommandBuffer* command = m_gpuContext->GetCurrentCommandBuffer();
    if (command)
    {
        m_fluidSolver->Dispatch(command, m_physicsParams);
        m_renderer->Draw(command, m_fluidSolver->GetOutputTexture());
    }

    m_gpuContext->EndFrame();
}
} // namespace Core
