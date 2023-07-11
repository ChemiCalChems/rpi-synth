#pragma once

#include "port.hpp"
#include "module.hpp"

struct ADSRModule :
	Module<
		PortID<"pressed", PortDirection::input>,
		PortID<"released", PortDirection::input>,
		PortID<"done", PortDirection::output>,
		PortID<"signalIn", PortDirection::input, double>,
		PortID<"signalOut", PortDirection::output, double>
	>
{
	const unsigned int samplerate;

	double attack = 0.005; //s
	double decay = 0.1; //s
	double sustain = 0.5;
	double release = 0.5; //s

	bool duringRelease = false;
	double releaseTime;
	double valueAtRelease;

	double t = 0;
	double lastAdsrValue = 0;

	double adsrValue(double t)
	{
		if (duringRelease)
		{
			if (t >= releaseTime + release)
			{
				getPort<"done">().send();
				return 0;
			}
			else return valueAtRelease*(1. - (t - releaseTime)/release);
		}
		if (t <= attack) return t / attack;
		if (t <= attack + decay) return 1. + (t - attack)/decay * (sustain - 1.);
		return sustain;
	}

	void signalInCallback(double signal)
	{
		lastAdsrValue = adsrValue(t);

		getPort<"signalOut">().send(signal * lastAdsrValue);
		t += 1./samplerate;
	}

	void releasedCallback()
	{
		valueAtRelease = lastAdsrValue;
		releaseTime = t;
		duringRelease = true;
	}
	ADSRModule(unsigned int _samplerate) : samplerate{_samplerate}
	{
		getPort<"pressed">().setCallback([this]{t = 0; lastAdsrValue = 0; duringRelease = false;});
		getPort<"released">().setCallback([this]{releasedCallback();});
		getPort<"signalIn">().setCallback([this](double signal){signalInCallback(signal);});
	}
};
