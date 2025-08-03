#pragma once

#include "filtermath.hpp"

class Waveform {
protected:
	double f;
public:
	Waveform(double _f) : f{_f} {}
	virtual ~Waveform() = default;

	virtual double getSample(double t) = 0;
};

template<int T>
class WaveformBase : public Waveform {
};

template<>
class WaveformBase<0> : public Waveform { //square
	DiscreteFilter<6,6> filter;
	double getSample(double t) override;
public:
	WaveformBase(double _f);
};

template<>
class WaveformBase<1> : public Waveform { //sine
	using Waveform::Waveform;
	double getSample(double t) override;
};

template<>
class WaveformBase<2> : public Waveform { //sawtooth
	using Waveform::Waveform;
	double getSample(double t) override;
};

template<>
class WaveformBase<3> : public Waveform { //triangle
	using Waveform::Waveform;
	double getSample(double t) override;
};
