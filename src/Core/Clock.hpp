#pragma once

#include <cstdint>

namespace Core
{
class Clock
{
public:
    Clock();

    [[nodiscard]] double GetDeltaSeconds() const;
    [[nodiscard]] double GetTotalSeconds() const;

private:
    uint64_t m_startTick;
    uint64_t m_lastTick;
};
} // namespace Core
