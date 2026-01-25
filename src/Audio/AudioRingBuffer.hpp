#pragma once

#include <atomic>
#include <cstddef>
#include <span>
#include <vector>

namespace Audio
{
class AudioRingBuffer
{
public:
    AudioRingBuffer();
    ~AudioRingBuffer() = default;

    AudioRingBuffer(const AudioRingBuffer&) = delete;
    AudioRingBuffer& operator=(const AudioRingBuffer&) = delete;
    AudioRingBuffer(AudioRingBuffer&&) = delete;
    AudioRingBuffer& operator=(AudioRingBuffer&&) = delete;

    void Write(std::span<const float> data);
    void ReadLatest(std::span<float> outData) const;

private:
    std::vector<float> m_buffer;
    std::atomic<size_t> m_writeIndex = 0;
    size_t m_mask = 0;
};
} // namespace Audio
