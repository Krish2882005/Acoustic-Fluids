#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>

#include <cmath>
#include <cstddef>
#include <numbers>
#include <vector>

#include "Audio/AudioConfig.hpp"
#include "Audio/AudioRingBuffer.hpp"
#include "Audio/FFTAnalyzer.hpp"

using namespace Audio;

// NOLINTNEXTLINE(readability-function-cognitive-complexity)
TEST_CASE("FFTAnalyzer", "[audio][dsp]")
{
    AudioRingBuffer ringBuffer;
    FFTAnalyzer analyzer(ringBuffer);

    constexpr float kTolerance = 1e-5F;
    constexpr float kTwoPi = 2.0F * std::numbers::pi_v<float>;

    SECTION("Zero Input")
    {
        const std::vector<float> silenceData(Config::kRingBufferSize, 0.0F);
        FrequencyBands bands{};

        // NOLINTNEXTLINE(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)
        for (size_t i = 0; i < 5; ++i)
        {
            ringBuffer.Write(silenceData);
            bands = analyzer.ProcessFrame();
        }

        using Catch::Matchers::WithinAbs;
        REQUIRE_THAT(bands.SubBass, WithinAbs(0.0F, kTolerance));
        REQUIRE_THAT(bands.Bass, WithinAbs(0.0F, kTolerance));
        REQUIRE_THAT(bands.Mids, WithinAbs(0.0F, kTolerance));
        REQUIRE_THAT(bands.Treble, WithinAbs(0.0F, kTolerance));
    }

    SECTION("Sine-Wave Isolation")
    {
        auto processContinuousSine = [&](float frequency, size_t frames)
        {
            FrequencyBands finalBands{};
            for (size_t i = 0; i < frames; ++i)
            {
                std::vector<float> buffer(Config::kFFTSize);
                for (size_t j = 0; j < buffer.size(); ++j)
                {
                    const float time =
                        static_cast<float>((i * Config::kFFTSize) + j) / static_cast<float>(Config::kSampleRate);
                    buffer[j] = std::sin(kTwoPi * frequency * time);
                }
                ringBuffer.Write(buffer);
                finalBands = analyzer.ProcessFrame();
            }
            return finalBands;
        };

        SECTION("Sub-Bass Isolation")
        {
            const auto bands = processContinuousSine(40.0F, 20);
            // NOLINTBEGIN(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)
            REQUIRE(bands.SubBass > 0.5F);
            REQUIRE(bands.Mids < 0.15F);
            REQUIRE(bands.Treble < 0.15F);
            // NOLINTEND(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)
        }

        SECTION("Bass Isolation")
        {
            const auto bands = processContinuousSine(120.0F, 20);
            // NOLINTBEGIN(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)
            REQUIRE(bands.Bass > 0.5F);
            REQUIRE(bands.Treble < 0.15F);
            // NOLINTEND(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)
        }

        SECTION("Mids Isolation")
        {
            const auto bands = processContinuousSine(1000.0F, 20);
            // NOLINTBEGIN(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)
            REQUIRE(bands.Mids > 0.5F);
            REQUIRE(bands.SubBass < 0.15F);
            REQUIRE(bands.Treble < 0.2F);
            // NOLINTEND(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)
        }

        SECTION("Treble Isolation")
        {
            const auto bands = processContinuousSine(8000.0F, 20);
            // NOLINTBEGIN(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)
            REQUIRE(bands.Treble > 0.5F);
            REQUIRE(bands.SubBass < 0.15F);
            REQUIRE(bands.Bass < 0.15F);
            // NOLINTEND(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)
        }
    }

    SECTION("Dual-Tone Isolation")
    {
        FrequencyBands bands{};
        constexpr size_t kWarmupFrames = 20;
        constexpr float kTwoPi = 2.0F * std::numbers::pi_v<float>;

        for (size_t i = 0; i < kWarmupFrames; ++i)
        {
            std::vector<float> buffer(Config::kFFTSize);
            for (size_t j = 0; j < buffer.size(); ++j)
            {
                const float time =
                    static_cast<float>((i * Config::kFFTSize) + j) / static_cast<float>(Config::kSampleRate);

                const float subBassWave = 0.5F * std::sin(kTwoPi * 40.0F * time);
                const float trebleWave = 0.5F * std::sin(kTwoPi * 5000.0F * time);

                buffer[j] = subBassWave + trebleWave;
            }
            ringBuffer.Write(buffer);
            bands = analyzer.ProcessFrame();
        }

        // NOLINTBEGIN(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)
        REQUIRE(bands.SubBass > 0.3F);
        REQUIRE(bands.Treble > 0.3F);
        REQUIRE(bands.Mids < 0.15F);
        // NOLINTEND(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)
    }

    SECTION("Impulse Response Tilt")
    {
        std::vector<float> audioData(Config::kRingBufferSize, 0.0F);

        audioData[Config::kRingBufferSize - (Config::kFFTSize / 2)] = 1.0F;
        ringBuffer.Write(audioData);

        const FrequencyBands bands = analyzer.ProcessFrame();

        REQUIRE(bands.Treble > bands.Mids);
        REQUIRE(bands.Mids > bands.Bass);
        REQUIRE(bands.Bass > bands.SubBass);
        REQUIRE(bands.Treble > 0.0F);
    }
}
