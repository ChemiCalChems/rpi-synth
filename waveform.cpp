#include "waveform.hpp"
#include <cmath>
#include <circle/timer.h>
#include "config.hpp"
#include "wavetable_gen.hpp"

WaveformBase<0>::WaveformBase(double _f) : Waveform{_f}
{
	filter.SetTransferFunction(BilinearTransform(ButterworthLowPass(maxOvertoneFrequency), Config::SAMPLERATE * 2).InvertVariable());
}

extern const WavetableArrayType squareWaveTables;

double WaveformBase<0>::getSample(double t) {
	const auto& wantedWaveTable{squareWaveTables[mapFreqToWaveTableNum(f)]};
	auto sample{[&](double x)
		{
			double temp;
			return wantedWaveTable[std::size_t(std::modf(f*t, &temp) * wantedWaveTable.size())];
		}};
	return filter(sample(t)), filter(sample(t + 1./(2. * Config::SAMPLERATE)));
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
