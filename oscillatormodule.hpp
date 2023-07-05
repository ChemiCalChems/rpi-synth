#pragma once

#include "waveform.hpp"
#include "port.hpp"
#include "module.hpp"

struct OscillatorModule :
	Module<
		PortID<"signalOut", PortDirection::output, double>,
		PortID<"sampleRequested", PortDirection::input>
	>
{
	std::unique_ptr<Waveform> waveform;
	double t = 0;
	const unsigned int samplerate;

	void sampleRequestedCallback()
	{
		if (!waveform) return;
		getPort<"signalOut">().send(waveform->getSample(t));
		t += 1./samplerate;
	}

	OscillatorModule(unsigned int samplerate_) : samplerate{samplerate_}
	{
		getPort<"sampleRequested">().setCallback([this]{sampleRequestedCallback();});
	}
};
