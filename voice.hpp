#pragma once

#include "waveform.hpp"
#include <memory>
#include "midi.hpp"

class Voice 
{
	const unsigned int samplerate;
	mutable double t = 0;
	std::unique_ptr<Waveform> waveform;
	bool on = false;

	MidiEvent reason;

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
	double getSample() const;

	void onKeyPress(const MidiEvent&);
	void onKeyRelease();
};
