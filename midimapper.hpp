#pragma once

#include "midi.hpp"
#include <map>

class Voice;

class MidiMapper : public MidiListener
{
	std::map<unsigned char /*midi note*/, Voice*> mappedKeys;
public:
	void midi_callback(MidiEvent) override;
};
