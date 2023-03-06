#pragma once

#include "waveform.hpp"
#include <memory>
#include "midi.hpp"
#include "patch.hpp"

class Voice 
{
	const unsigned int samplerate;
	std::unique_ptr<Patch> patch;
	mutable bool on = false;

	MidiEvent reason;

	void turnOn();
	void turnOff();

	void onDone();
public:
	Voice(unsigned int _samplerate);

	Voice(const Voice&) = delete;
	Voice& operator=(const Voice&) = delete;
	Voice(Voice&&) = delete;
	Voice& operator=(Voice&&) = delete;

	bool isOn() const;

	MidiEvent businessReason() const;

	double getSample();

	void onKeyPress(const MidiEvent&, std::unique_ptr<Patch>&& _patch);
	void onKeyRelease();
	void onKeyRepress();
};
