#pragma once

#include <cmath>
#include <circle/timer.h>
#include <array>
#include <algorithm>
#include <numbers>
#include <cstddef>
#include <ranges>

static constexpr auto pi{std::numbers::pi_v<double>};

// We will be generating many wavetables for each waveform
// in order to avoid aliasing issues caused by pushing harmonics
// above the Nyquist frequency when requesting waveforms of higher
// and higher frequency. This is similar to mipmapping.
//
// The range of fundamental frequencies we wish to appropriately
// handle is [30, 4200] Hz, slightly below B0 and slightly above C8
// respectly. We want all harmonics under 20kHz to be represented.
//
// The plan is to have a wavetable for everything octave,
// such that the highest harmonic for the highest pitch that uses
// the given wavetables is less than 20kHz * 2 = 40kHz.
// This is obviously above the Nyquist frequency for 44.1kHz and
// 48kHz sample rates, but this can be fixed by oversampling,
// low-pass filtering and undersampling.
//
// The number of tables needed is N such that 30 * 2^N > 4200,
// which turns out to be N = 8. In case anything changes in the
// future, we will parameterize everything below...

inline constexpr double minimumWantedFundamentalFrequency = 30.;
inline constexpr double maximumWantedFundamentalFrequency = 4200.;
inline constexpr double maxOvertoneFrequency = 20000.;

inline constexpr std::size_t SampleCount = 2048;

inline constexpr std::size_t numberOfWaveTables = static_cast<std::size_t>(
		std::ceil(std::log2(4200/30)));
// static_assert(numberOfWaveTables == 8);

using WavetableArrayType = std::array<std::array<double, SampleCount>, numberOfWaveTables>;

constexpr std::size_t mapFreqToWaveTableNum(double f)
{
	// The appropriate table number N satisfies min * 2^N ≤ f ≤ min * 2^(N+1),
	// thus N = std::floor(std::log2(f/min))
	double tempTableNum = std::floor(std::log2(f/minimumWantedFundamentalFrequency));
	return static_cast<std::size_t>(std::clamp(tempTableNum, 0., static_cast<double>(numberOfWaveTables - 1)));
}

static_assert(mapFreqToWaveTableNum(minimumWantedFundamentalFrequency) == 0);
static_assert(mapFreqToWaveTableNum(minimumWantedFundamentalFrequency - 1) == 0);
static_assert(mapFreqToWaveTableNum(maximumWantedFundamentalFrequency) == numberOfWaveTables - 1);

template<std::size_t HarmonicCount>
constexpr auto generateNthWaveTable(std::size_t N,
                                    const std::array<double, HarmonicCount>& coefficients)
{
	double fundamentalFreq{minimumWantedFundamentalFrequency * std::pow(2, N)};

	std::array<double, SampleCount> result;
	for (std::size_t i = 0; i < SampleCount; ++i)
	{
		double sample = 0;
		for (const auto& [n, c] : coefficients | std::views::enumerate)
		{
			std::size_t harmonicNumber{n + 1uz};
			double harmonicFreq{fundamentalFreq * harmonicNumber};
			if (harmonicFreq > maxOvertoneFrequency)
			{
				break;
			}
			if (c == 0)
			{
				continue;
			}
			sample += c * std::sin(2.*pi*harmonicNumber*(static_cast<double>(i) / double {SampleCount}));
		}
		result[i] = sample;
	}
	return result;
}

template<std::size_t HarmonicCount>
constexpr auto generateWaveTables(const std::array<double, HarmonicCount>& coefficients)
{
	std::array<std::array<double, SampleCount>, numberOfWaveTables> result{};
	std::ranges::generate(result, [&, i = 0] mutable {return generateNthWaveTable(i++, coefficients);});
	return result;
}
