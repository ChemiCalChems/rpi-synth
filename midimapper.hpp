#pragma once

#include "midi.hpp"
#include "voice.hpp"
#include <unordered_map>

class MidiMapper : public MidiListener
{
	std::unordered_map<decltype(MidiEvent::NoteEvent::key), Voice*> mappedVoices;
public:
	void midiCallback(MidiEvent) override;
	void onVoiceDone(Voice*);
};
