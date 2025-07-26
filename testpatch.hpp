#pragma once

#include "patch.hpp"
#include "oscillatormodule.hpp"
#include "port.hpp"
#include <optional>
#include "adsrmodule.hpp"
#include "filtermodule.hpp"

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

	DiscreteFilterModule<6, 6> filter;

	ADSRModule adsr;
	Port<PortDirection::output> adsrKeyPressed;
	Port<PortDirection::output> adsrKeyReleased;
	Port<PortDirection::input> adsrDone{[this]{done = true;}};
public:
	TestPatch(unsigned int _samplerate) : samplerate{_samplerate}, oscillator{samplerate}, adsr{samplerate}
	{
		requestSample.connectTo(oscillator.getPort<"sampleRequested">());
		oscillator.getPort<"signalOut">().connectTo(filter.getPort<"signalIn">());
		filter.getPort<"signalOut">().connectTo(adsr.getPort<"signalIn">());
		// oscillator.getPort<"signalOut">().connectTo(adsr.getPort<"signalIn">());
		adsr.getPort<"signalOut">().connectTo(signalIn);

		adsrKeyPressed.connectTo(adsr.getPort<"pressed">());
		adsrKeyReleased.connectTo(adsr.getPort<"released">());
		adsr.getPort<"done">().connectTo(adsrDone);
	}

	void onKeyPress(const MidiEvent&) override;
	void onKeyRelease() override;
	void onKeyRepress() override;

	bool isDone() const override;

	double getSample() override;
};
