#pragma once

#include "port.hpp"

struct ADSRModule
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

	/* To be connected to the patch object
	 * or whatever logically controls ADSR */
	Port<PortDirection::input> pressed{[this]{t = 0; lastAdsrValue = 0; duringRelease = false;}};
	Port<PortDirection::input> released {[this]{releasedCallback();}};
	Port<PortDirection::output> done;

	/* Audio ports */
	Port<PortDirection::input, double> signalIn {[this](double signal){signalInCallback(signal);}};
	Port<PortDirection::output, double> signalOut;

	double adsrValue(double t)
	{
		if (duringRelease)
		{
			if (t >= releaseTime + release)
			{
				done.send();
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

		signalOut.send(signal * lastAdsrValue);
		t += 1./samplerate;
	}

	void releasedCallback()
	{
		valueAtRelease = lastAdsrValue;
		releaseTime = t;
		duringRelease = true;
	}
	ADSRModule(unsigned int _samplerate) : samplerate{_samplerate} {};
};
