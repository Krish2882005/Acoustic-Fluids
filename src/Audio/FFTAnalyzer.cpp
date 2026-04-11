#include "FFTAnalyzer.hpp"

#define POCKETFFT_CACHE_SIZE 1      // NOLINT(cppcoreguidelines-macro-usage)
#define POCKETFFT_NO_MULTITHREADING // NOLINT(cppcoreguidelines-macro-usage)
#include <pocketfft_hdronly.h>

#include <algorithm>
#include <cmath>
#include <complex>
#include <cstddef>
#include <functional>
#include <numbers>
#include <ranges>
#include <span>
#include <vector>

#include "AudioConfig.hpp"
#include "AudioRingBuffer.hpp"

namespace Audio
{
namespace
{
constexpr float kEmaAlpha = 0.3F;
constexpr float kNoiseFloorDb = -80.0F;
constexpr float kMaxSignalDb = 0.0F;
constexpr float kSpectralTiltPerOctaveDb = 3.0F;
constexpr float kReferenceFrequency = 20.0F;
constexpr float kFrequencyResolution = static_cast<float>(Config::kSampleRate) / static_cast<float>(Config::kFFTSize);

constexpr size_t FrequencyToBin(float frequency)
{
    // MSVC's <cmath> lacks constexpr support for std::lround
    // NOLINTNEXTLINE(bugprone-incorrect-roundings, cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)
    return static_cast<size_t>((frequency / kFrequencyResolution) + 0.5F);
}

constexpr size_t kSubBassStart = std::max<size_t>(1, FrequencyToBin(20.0F));
constexpr size_t kSubBassEnd = FrequencyToBin(60.0F);
constexpr size_t kBassStart = FrequencyToBin(60.0F);
constexpr size_t kBassEnd = FrequencyToBin(250.0F);
constexpr size_t kMidsStart = FrequencyToBin(250.0F);
constexpr size_t kMidsEnd = FrequencyToBin(2000.0F);
constexpr size_t kTrebleStart = FrequencyToBin(2000.0F);
constexpr size_t kTrebleEnd = FrequencyToBin(16000.0F);

float CalculateTiltGain(size_t startBin, size_t endBin)
{
    constexpr size_t kMaxBin = Config::kFFTSize / 2;
    const size_t safeEnd = std::min(endBin, kMaxBin);

    const float startFreq = static_cast<float>(startBin) * kFrequencyResolution;
    const float endFreq = static_cast<float>(safeEnd) * kFrequencyResolution;
    const float centerFreq = std::sqrt(std::max(1.0F, startFreq) * endFreq);

    if (const float octaves = std::log2(centerFreq / kReferenceFrequency); octaves > 0.0F)
    {
        const float tiltDb = kSpectralTiltPerOctaveDb * octaves;
        // NOLINTNEXTLINE(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)
        return std::pow(10.0F, tiltDb / 10.0F);
    }
    return 1.0F;
}

const float kSubBassGain = CalculateTiltGain(kSubBassStart, kSubBassEnd);
const float kBassGain = CalculateTiltGain(kBassStart, kBassEnd);
const float kMidsGain = CalculateTiltGain(kMidsStart, kMidsEnd);
const float kTrebleGain = CalculateTiltGain(kTrebleStart, kTrebleEnd);

} // namespace

FFTAnalyzer::FFTAnalyzer(const AudioRingBuffer& ringBuffer) : m_ringBuffer(ringBuffer)
{
    m_timeData.resize(Config::kFFTSize);

    const size_t freqSize = (Config::kFFTSize / 2) + 1;
    m_freqData.resize(freqSize);
    m_power.resize(freqSize);
    m_window.resize(Config::kFFTSize);

    for (auto [i, windowCoefficient] : std::views::enumerate(m_window))
    {
        const float phase =
            2.0F * std::numbers::pi_v<float> * static_cast<float>(i) / static_cast<float>(Config::kFFTSize - 1);
        // NOLINTNEXTLINE(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)
        windowCoefficient = 0.5F * (1.0F - std::cos(phase));
    }

    m_shape = {Config::kFFTSize};
    m_strideIn = {sizeof(float)};
    m_strideOut = {sizeof(std::complex<float>)};
}

void FFTAnalyzer::ApplyHannWindow(std::span<float> data) const
{
    for (auto [sample, windowCoefficient] : std::views::zip(data, m_window))
    {
        sample *= windowCoefficient;
    }
}

float FFTAnalyzer::CalculateSemanticBand(size_t startBin, size_t endBin, float tiltLinearGain) const
{
    if (startBin > endBin || startBin >= m_power.size())
    {
        return 0.0F;
    }

    const size_t safeEnd = std::min(endBin, m_power.size() - 1);
    const size_t count = safeEnd - startBin + 1;

    const auto band = std::span{m_power}.subspan(startBin, count);
    const float sumPower = std::ranges::fold_left(band, 0.0F, std::plus<>{});

    const float meanPower = sumPower / static_cast<float>(count);
    const float adjustedPower = meanPower * tiltLinearGain;

    constexpr float kEpsilon = 1e-8F;
    const float decibels = 10.0F * std::log10(adjustedPower + kEpsilon);

    const float normalized = (decibels - kNoiseFloorDb) / (kMaxSignalDb - kNoiseFloorDb);

    return std::clamp(normalized, 0.0F, 1.0F);
}

FrequencyBands FFTAnalyzer::ProcessFrame()
{
    m_ringBuffer.ReadLatest(m_timeData);

    ApplyHannWindow(m_timeData);

    pocketfft::r2c(m_shape, m_strideIn, m_strideOut, 0, true, m_timeData.data(), m_freqData.data(), 1.0F);

    constexpr float kNormalization = 4.0F / static_cast<float>(Config::kFFTSize);
    constexpr float kPowerNorm = kNormalization * kNormalization;

    for (auto [freq, power] : std::views::zip(m_freqData, m_power))
    {
        power = std::norm(freq) * kPowerNorm;
    }

    const float rawSubBass = CalculateSemanticBand(kSubBassStart, kSubBassEnd, kSubBassGain);
    const float rawBass = CalculateSemanticBand(kBassStart, kBassEnd, kBassGain);
    const float rawMids = CalculateSemanticBand(kMidsStart, kMidsEnd, kMidsGain);
    const float rawTreble = CalculateSemanticBand(kTrebleStart, kTrebleEnd, kTrebleGain);

    m_smoothedBands.SubBass = std::lerp(m_smoothedBands.SubBass, rawSubBass, kEmaAlpha);
    m_smoothedBands.Bass = std::lerp(m_smoothedBands.Bass, rawBass, kEmaAlpha);
    m_smoothedBands.Mids = std::lerp(m_smoothedBands.Mids, rawMids, kEmaAlpha);
    m_smoothedBands.Treble = std::lerp(m_smoothedBands.Treble, rawTreble, kEmaAlpha);

    return m_smoothedBands;
}
} // namespace Audio
