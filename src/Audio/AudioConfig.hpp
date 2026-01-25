#pragma once

#include <cstddef>
#include <cstdint>

namespace Audio
{
struct Config
{
    static constexpr uint32_t kSampleRate = 48000;
    static constexpr uint32_t kFFTSize = 2048;
    static constexpr size_t kRingBufferSize = 1 << 14;
};
} // namespace Audio
