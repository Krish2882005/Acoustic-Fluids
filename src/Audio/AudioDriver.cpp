#include "AudioDriver.hpp"

#define MINIAUDIO_IMPLEMENTATION
#include <miniaudio.h>

#include <array>
#include <memory>
#include <span>
#include <stdexcept>

#include "../Core/Logger.hpp"
#include "AudioConfig.hpp"
#include "AudioRingBuffer.hpp"

namespace Audio
{
namespace
{
void DataCallback(ma_device* pDevice, [[maybe_unused]] void* pOutput, const void* pInput, ma_uint32 frameCount)
{
    if (!pDevice || !pDevice->pUserData || !pInput)
    {
        return;
    }

    auto* ringBuffer = static_cast<AudioRingBuffer*>(pDevice->pUserData);
    const auto* inputFloats = static_cast<const float*>(pInput);

    ringBuffer->Write(std::span<const float>(inputFloats, frameCount));
}
} // namespace

void AudioDriver::MaDeviceDestroyer::operator()(ma_device* device) const
{
    if (device)
    {
        ma_device_uninit(device);
        delete device; // NOLINT(cppcoreguidelines-owning-memory)
    }
}

AudioDriver::AudioDriver(AudioRingBuffer& ringBuffer) : m_device(nullptr), m_ringBuffer(ringBuffer)
{
    ma_device_config config = ma_device_config_init(ma_device_type_loopback);
    config.capture.format = ma_format_f32;
    config.capture.channels = 1;
    config.sampleRate = Config::kSampleRate;
    config.dataCallback = DataCallback;
    config.pUserData = &m_ringBuffer;

    LOG_INFO("AudioDriver: Initializing Loopback device...");

    auto rawDevice = std::make_unique<ma_device>();

    if (ma_device_init(nullptr, &config, rawDevice.get()) != MA_SUCCESS)
    {
        LOG_ERROR("AudioDriver: Failed to initialize Loopback device.");
        throw std::runtime_error("Audio Backend Init Failed");
    }

    m_device.reset(rawDevice.release());

    if (ma_device_start(m_device.get()) != MA_SUCCESS)
    {
        LOG_ERROR("AudioDriver: Failed to start loopback device.");
        throw std::runtime_error("Audio Backend Start Failed");
    }

    std::array<char, MA_MAX_DEVICE_NAME_LENGTH + 1> deviceName{};
    const char* backendName = ma_get_backend_name(m_device->pContext->backend);

    if (ma_device_get_name(m_device.get(), ma_device_type_loopback, deviceName.data(), deviceName.size(), nullptr) ==
        MA_SUCCESS)
    {
        LOG_INFO("AudioDriver: Connected to '{}' ({})", deviceName.data(), backendName);
    }
}

AudioDriver::~AudioDriver() = default;
} // namespace Audio
