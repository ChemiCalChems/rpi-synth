#pragma once

class Waveform {
protected:
	long double t();
public:
	virtual double getSample(float f, long double t) = 0;
};

template<int T>
class WaveformBase : public Waveform {
};

template<>
class WaveformBase<0> : public Waveform { //square
	double getSample(float f, long double t) override;
};

template<>
class WaveformBase<1> : public Waveform { //sine
	double getSample(float f, long double t) override;
};

template<>
class WaveformBase<2> : public Waveform { //sawtooth
	double getSample(float f, long double t) override;
};

template<>
class WaveformBase<3> : public Waveform { //triangle
	double getSample(float f, long double t) override;
};


