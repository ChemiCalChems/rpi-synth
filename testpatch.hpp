#pragma once

#include "patch.hpp"
#include <memory>
#include "waveform.hpp"
#include "oscillatormodule.hpp"
#include "port.hpp"
#include <optional>
#include "adsrmodule.hpp"

struct MidiEvent;

class TestPatch : public Patch
{
	bool done = false;
	double t = 0;
	const unsigned int samplerate;

	OscillatorModule oscillator;
	Port<PortDirection::output> requestSample;
	Port<PortDirection::input, double> signalIn{[this](double x){lastSample = x;}};
	std::optional<double> lastSample;

	ADSRModule adsr;
	Port<PortDirection::output> adsrKeyPressed;
	Port<PortDirection::output> adsrKeyReleased;
	Port<PortDirection::input> adsrDone{[this]{done = true;}};
public:
	TestPatch(unsigned int _samplerate) : samplerate{_samplerate}, oscillator{samplerate}, adsr{samplerate}
	{
		requestSample.connectTo(oscillator.sampleRequested);
		oscillator.signalOut.connectTo(adsr.signalIn);
		adsr.signalOut.connectTo(signalIn);

		adsrKeyPressed.connectTo(adsr.pressed);
		adsrKeyReleased.connectTo(adsr.released);
		adsr.done.connectTo(adsrDone);
	}

	void onKeyPress(const MidiEvent&) override;
	void onKeyRelease() override;
	void onKeyRepress() override;

	bool isDone() const override;

	double getSample() override;
};
