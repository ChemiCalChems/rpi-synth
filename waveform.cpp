#include "waveform.hpp"
#include <cmath>
#include <circle/timer.h>
#include <array>
#include <algorithm>
#include <ranges>
#include <numbers>

static constexpr auto pi{std::numbers::pi_v<double>};

template<std::size_t N>
constexpr std::array<double, N> squareWaveCoefficients()
{
	std::array<double, N> result;
	std::ranges::generate(result, [n = 0] mutable {double result; if (n % 2 == 0) result = 0; else result = 4./(pi * n); n++; return result;});
	return result;
}

template<std::size_t N>
constexpr std::array<double, N> squareWaveTable()
{
	static constexpr auto squareWaveCoeff{squareWaveCoefficients<10>()};
	std::array<double, N> result{};
	double freq{1.};
	for (std::size_t i = 0; i < N; ++i)
	{
		double sample = 0;
		for (const auto& [n, c] : squareWaveCoeff | std::views::enumerate)
		{
			double harmonicFreq{freq * n};
			sample += c * std::sin(2.*pi*harmonicFreq*(static_cast<double>(i) / double {N}));
		}
		result[i] = sample;
	}
	return result;
}

double WaveformBase<0>::getSample(double t) {
	// TODO: this still aliases at high enough fundamental frequencies
	// Max harmonic = f * 10 = 1 * 10 = 10
	// When downsampled to f = 2205 => max harmonic = 22050 = nyquist for 44100Hz sample rate
	// One go of oversampling should be enough
	static constexpr auto squareWaveTab{squareWaveTable<4096>()};
	return squareWaveTab[std::size_t(f*t * squareWaveTab.size()) % squareWaveTab.size()];
}

double WaveformBase<1>::getSample(double t) {
	return std::sqrt(2)*std::sin(2*3.1415926535*f*t);
}

double WaveformBase<2>::getSample(double t) {
	double temp;
	return sqrt(3)*(2*std::modf(f*t, &temp) - 1);
}

double WaveformBase<3>::getSample(double t) {
	double temp;
	return sqrt(3)*(2*std::abs(2*std::modf(f*t, &temp) - 1) - 1);
}
