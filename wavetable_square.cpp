#include "wavetable_gen.hpp"

template<std::size_t N>
constexpr std::array<double, N> squareWaveCoefficients()
{
	std::array<double, N> result;
	std::ranges::generate(result, [n = 1] mutable {double result; if (n % 2 == 0) result = 0; else result = 4./(pi * n); n++; return result;});
	return result;
}

static constexpr auto squareWaveTableTemp{generateWaveTables(squareWaveCoefficients<200>())};

extern const WavetableArrayType squareWaveTables{squareWaveTableTemp};
