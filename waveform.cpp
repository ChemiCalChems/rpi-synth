#include "waveform.hpp"
#include <cmath>
#include <circle/timer.h>

double WaveformBase<0>::getSample(double t) {
	double temp;
	if (std::modf(f*t, &temp) < 0.5) return 1;
	return -1;
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

