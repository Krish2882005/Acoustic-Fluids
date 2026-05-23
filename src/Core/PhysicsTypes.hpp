#pragma once

namespace Core
{
// NOLINTNEXTLINE(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)
struct alignas(16) FluidPhysicsParameters
{
    float SubBass = 0.0F;
    float Bass = 0.0F;
    float Mids = 0.0F;
    float Treble = 0.0F;

    float DeltaTime = 0.0F;
    float GridResolutionX = 0.0F;
    float GridResolutionY = 0.0F;
    float Dissipation = 0.0F;
};
} // namespace Core
