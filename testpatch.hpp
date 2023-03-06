#pragma once

#include "patch.hpp"
#include <memory>
#include "waveform.hpp"

struct ADSREnvelopeGenerator
{
	double attack = 0.005; //s
	double decay = 0.1; //s
	double sustain = 0.5;
	double release = 0.5; //s

	double duringRelease = false;
	double releaseTime;
	double valueAtRelease;

	bool done = false;

	double operator()(double t)
	{
		if (duringRelease)
		{
			if (t >= releaseTime + release)
			{
				done = true;
				return 0;
			}
			else return valueAtRelease*(1. - (t - releaseTime)/release);
		}
		if (t <= attack) return t / attack;
		if (t <= attack + decay) return 1. + (t - attack)/decay * (sustain - 1.);
		return sustain;
	}
	void released(double t)
	{
		valueAtRelease = operator()(t);
		releaseTime = t;
		duringRelease = t;
	}
};

struct MidiEvent;

class TestPatch : public Patch
{
	bool done = false;
	double t = 0;
	const unsigned int samplerate;

	std::unique_ptr<Waveform> waveform;
	ADSREnvelopeGenerator adsr;
public:
	TestPatch(unsigned int _samplerate) : samplerate{_samplerate}
	{
	}

	void onKeyPress(const MidiEvent&) override;
	void onKeyRelease() override;
	void onKeyRepress() override;

	bool isDone() const override;

	double getSample() override;
};
