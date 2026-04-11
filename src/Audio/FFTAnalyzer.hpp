#pragma once

#include <complex>
#include <cstddef>
#include <span>
#include <vector>

namespace Audio
{
class AudioRingBuffer;

struct FrequencyBands
{
    float SubBass = 0.0F;
    float Bass = 0.0F;
    float Mids = 0.0F;
    float Treble = 0.0F;
};

class FFTAnalyzer
{
public:
    explicit FFTAnalyzer(const AudioRingBuffer& ringBuffer);
    ~FFTAnalyzer() = default;

    FFTAnalyzer(const FFTAnalyzer&) = delete;
    FFTAnalyzer& operator=(const FFTAnalyzer&) = delete;
    FFTAnalyzer(FFTAnalyzer&&) = delete;
    FFTAnalyzer& operator=(FFTAnalyzer&&) = delete;

    [[nodiscard]] FrequencyBands ProcessFrame();

private:
    void ApplyHannWindow(std::span<float> data) const;
    [[nodiscard]] float CalculateSemanticBand(size_t startBin, size_t endBin, float tiltLinearGain) const;

    // NOLINTNEXTLINE(cppcoreguidelines-avoid-const-or-ref-data-members)
    const AudioRingBuffer& m_ringBuffer;

    std::vector<float> m_timeData;
    std::vector<std::complex<float>> m_freqData;
    std::vector<float> m_power;
    std::vector<float> m_window;

    FrequencyBands m_smoothedBands;

    std::vector<std::size_t> m_shape;
    std::vector<std::ptrdiff_t> m_strideIn;
    std::vector<std::ptrdiff_t> m_strideOut;
};
} // namespace Audio
