#include <cstdio>
#include <exception>
#include <print>

#include "Core/Config.hpp"
#include "Core/Engine.hpp"

int main([[maybe_unused]] int argc, [[maybe_unused]] char** argv)
{
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
        std::println(stderr, "Fatal Error: {}", e.what());
        return -1;
    }
    catch (...)
    {
        std::println(stderr, "Fatal Error: Unknown exception occurred.");
        return -1;
    }

    return 0;
}
