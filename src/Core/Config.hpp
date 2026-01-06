#pragma once

#include <cstdint>
#include <string>

namespace Core
{
struct Config
{
    static constexpr uint32_t kDefaultWidth = 1280;
    static constexpr uint32_t kDefaultHeight = 720;

    // Window Settings
    std::string WindowTitle = "Acoustic Fluids";
    uint32_t WindowWidth = kDefaultWidth;
    uint32_t WindowHeight = kDefaultHeight;

    // Render Settings
    bool VSync = true;
    uint32_t TargetRenderFPS = 0; // 0 for uncapped (if VSync is off)

    // Physics Settings
    static constexpr double kPhysicsTimeStep = 1.0 / 60.0;

    // Debug Settings
    bool EnableGPUDebug = false;
};
} // namespace Core
