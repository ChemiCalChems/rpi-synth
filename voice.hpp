#pragma once

#include "waveform.hpp"
#include <memory>
#include "midi.hpp"
#include "patch.hpp"
#include <functional>

class Voice 
{
	friend class Mixer;

	const unsigned int samplerate;
	std::unique_ptr<Patch> patch;
	mutable bool on = false;

	void onDone();
public:
	Voice(unsigned int _samplerate);

	Voice(const Voice&) = delete;
	Voice& operator=(const Voice&) = delete;
	Voice(Voice&&) = delete;
	Voice& operator=(Voice&&) = delete;

	bool isOn() const;

	double getSample();

	void onKeyPress(const MidiEvent&, std::unique_ptr<Patch>&& _patch);
	void onKeyRelease();
	void onKeyRepress();

    std::function<void()> onVoiceDoneCallback;
};
