#pragma once

#include "waveform.hpp"
#include <memory>
#include "midi.hpp"

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

class Voice 
{
	const unsigned int samplerate;
	mutable double t = 0;
	std::unique_ptr<Waveform> waveform;
	mutable bool on = false;

	MidiEvent reason;
	mutable ADSREnvelopeGenerator adsr;

public:
	Voice(unsigned int _samplerate);

	Voice(const Voice&) = delete;
	Voice& operator=(const Voice&) = delete;
	Voice(Voice&&) = delete;
	Voice& operator=(Voice&&) = delete;
	
	void turnOn();
	void turnOff();
	bool isOn() const;

	MidiEvent businessReason() const;

	void resetTime() const;
	double getSample();

	void onKeyPress(const MidiEvent&);
	void onKeyRelease();

	void onDone();
};
