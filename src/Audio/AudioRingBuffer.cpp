#include "AudioRingBuffer.hpp"

#include <atomic>
#include <bit>
#include <cstddef>
#include <span>
#include <stdexcept>

#include "AudioConfig.hpp"

namespace Audio
{
AudioRingBuffer::AudioRingBuffer()
{
    if (!std::has_single_bit(Config::kRingBufferSize))
    {
        throw std::logic_error("AudioRingBuffer: Buffer size must be a power of two");
    }

    m_buffer.resize(Config::kRingBufferSize, 0.0F);
    m_mask = Config::kRingBufferSize - 1;
}

void AudioRingBuffer::Write(std::span<const float> data)
{
    size_t head = m_writeIndex.load(std::memory_order::relaxed);

    for (const float sample : data)
    {
        m_buffer[head++ & m_mask] = sample;
    }

    m_writeIndex.store(head, std::memory_order::release);
}

void AudioRingBuffer::ReadLatest(std::span<float> outData) const
{
    const size_t head = m_writeIndex.load(std::memory_order::acquire);
    size_t readPos = head - outData.size();

    for (float& sample : outData)
    {
        sample = m_buffer[readPos++ & m_mask];
    }
}
} // namespace Audio
