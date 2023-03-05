#pragma once

#include "midi.hpp"
#include <map>
#include "voice.hpp"
#include <array>

#define NUM_VOICES 16

class MidiMapper : public MidiListener
{
	std::array<Voice, NUM_VOICES> voices;
public:
	MidiMapper();
	void midi_callback(MidiEvent) override;
};
