#pragma once

namespace Core
{
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
