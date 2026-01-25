#pragma once

#include <memory>

struct ma_device; // NOLINT(readability-identifier-naming)

namespace Audio
{
class AudioRingBuffer;

class AudioDriver
{
public:
    explicit AudioDriver(AudioRingBuffer& ringBuffer);
    ~AudioDriver();

    AudioDriver(const AudioDriver&) = delete;
    AudioDriver& operator=(const AudioDriver&) = delete;
    AudioDriver(AudioDriver&&) = delete;
    AudioDriver& operator=(AudioDriver&&) = delete;

private:
    struct MaDeviceDestroyer
    {
        void operator()(ma_device* device) const;
    };

    using MaDevicePtr = std::unique_ptr<ma_device, MaDeviceDestroyer>;

    MaDevicePtr m_device;
    AudioRingBuffer& m_ringBuffer;
};
} // namespace Audio
