#pragma once

#include <cstdint>

namespace Core
{
class Clock
{
public:
    Clock();

    [[nodiscard]] double GetTotalSeconds() const;

private:
    uint64_t m_startTick;
};
} // namespace Core
