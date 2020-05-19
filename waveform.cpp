#include "waveform.hpp"
#include <cmath>
#include <circle/timer.h>

long double Waveform::t() {
	return CTimer::GetClockTicks() / 1000000.L;
}

double WaveformBase<0>::getSample(float f, long double t) {
	double temp;
	if (std::modf(f*t, &temp) < 0.7) return 1;
	return -1;
}

double WaveformBase<1>::getSample(float f, long double t) {
	return std::sqrt(2)*std::sin(2*3.1415926535*f*t);
}
