#pragma once

#include "waveform.hpp"
#include "port.hpp"

struct OscillatorModule
{
	std::unique_ptr<Waveform> waveform;
	double t = 0;
	const unsigned int samplerate;

	Port<PortDirection::output, double> signalOut;
	Port<PortDirection::input> sampleRequested{[this]{sampleRequestedCallback();}};

	void sampleRequestedCallback()
	{
		if (!waveform) return;
		signalOut.send(waveform->getSample(t));
		t += 1./samplerate;
	}

	OscillatorModule(unsigned int samplerate_) : samplerate{samplerate_}
	{
	}
};
