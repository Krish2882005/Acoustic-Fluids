#include "Clock.hpp"

#include <SDL3/SDL.h>

#include <cstdint>

namespace Core
{
Clock::Clock() : m_startTick(SDL_GetPerformanceCounter()), m_lastTick(m_startTick) {}

double Clock::GetDeltaSeconds() const
{
    const uint64_t currentTick = SDL_GetPerformanceCounter();
    const uint64_t diff = currentTick - m_lastTick;
    return static_cast<double>(diff) / static_cast<double>(SDL_GetPerformanceFrequency());
}

double Clock::GetTotalSeconds() const
{
    const uint64_t currentTick = SDL_GetPerformanceCounter();
    const uint64_t diff = currentTick - m_startTick;
    return static_cast<double>(diff) / static_cast<double>(SDL_GetPerformanceFrequency());
}
} // namespace Core
