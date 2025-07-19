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

	DiscreteFilterModule<1, 1> filter;

	ADSRModule adsr;
	Port<PortDirection::output> adsrKeyPressed;
	Port<PortDirection::output> adsrKeyReleased;
	Port<PortDirection::input> adsrDone{[this]{done = true;}};
public:
	TestPatch(unsigned int _samplerate) : samplerate{_samplerate}, oscillator{samplerate}, adsr{samplerate}
	{
		double cutoff_frequency = 1000.;
		double RC = 1./(cutoff_frequency * 2. * 3.14159265);
		double RCK = 2. * RC * double(samplerate);

		// low pass filter coefficients
		filter.feedforward_coeff[0] = 1./(1. + RCK);
		filter.feedforward_coeff[1] = 1./(1. + RCK);
		filter.feedback_coeff[0] = (1. - RCK)/(1. + RCK);

		// high pass filter coefficients
		filter.feedforward_coeff[0] = RCK / (RCK + 1.);
		filter.feedforward_coeff[1] = - RCK / (RCK + 1.);
		filter.feedback_coeff[0] = (1. - RCK)/(1. + RCK);

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
