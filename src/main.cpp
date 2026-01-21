#include <exception>

#include "Core/Config.hpp"
#include "Core/Engine.hpp"
#include "Core/Logger.hpp"

int main([[maybe_unused]] int argc, [[maybe_unused]] char** argv)
{
    const Core::Logger::Scoped loggerScope;

    try
    {
        Core::Config config{};
        config.WindowTitle = "Acoustic Fluids";
        config.VSync = true;
#ifdef NDEBUG
        config.EnableGPUDebug = false;
#else
        config.EnableGPUDebug = true;
#endif

        Core::Engine app(config);
        app.Run();
    }
    catch (const std::exception& e)
    {
        LOG_ERROR("Fatal Error: {}", e.what());
        return -1;
    }
    catch (...)
    {
        LOG_ERROR("Fatal Error: Unknown exception occurred.");
        return -1;
    }

    return 0;
}
