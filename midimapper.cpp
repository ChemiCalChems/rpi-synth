#include "midimapper.hpp"
#include "mixer.hpp"
#include "waveform.hpp"
#include "utils.hpp"
#include <circle/logger.h>

void MidiMapper::midi_callback(MidiEvent e)
{
	if (e.type == MidiEvent::Type::noteon)
	{
		if (auto voice = Mixer::get().requestVoice())
		{
			mappedKeys[e.note.key] = voice;		
			voice->setWaveform(std::make_unique<WaveformBase<0>>(utils::midi_freqs[e.note.key]));
			voice->resetTime();
			voice->turnOn();
		}
	}
	if (e.type == MidiEvent::Type::noteoff)
	{
		if (auto& voice = mappedKeys[e.note.key])
		{
			voice->turnOff();
			Mixer::get().returnVoice(voice);
			voice = nullptr;
		}
	}
}
