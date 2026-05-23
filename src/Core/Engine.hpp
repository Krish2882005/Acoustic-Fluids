#pragma once

#include <memory>

#include "Config.hpp"
#include "PhysicsTypes.hpp"

namespace Audio
{
class AudioRingBuffer;
class AudioDriver;
class FFTAnalyzer;
} // namespace Audio

namespace Simulation
{
class FluidSolver;
} // namespace Simulation

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

    FluidPhysicsParameters m_physicsParams{};

    SDLContext m_sdlContext;
    Config m_config;
    bool m_isRunning = false;

    std::unique_ptr<Window> m_window;
    std::unique_ptr<Graphics::GPUContext> m_gpuContext;
    std::unique_ptr<Graphics::Renderer> m_renderer;

    std::unique_ptr<Simulation::FluidSolver> m_fluidSolver;
    std::unique_ptr<Audio::AudioRingBuffer> m_audioRingBuffer;
    std::unique_ptr<Audio::AudioDriver> m_audioDriver;
    std::unique_ptr<Audio::FFTAnalyzer> m_fftAnalyzer;
};
} // namespace Core
