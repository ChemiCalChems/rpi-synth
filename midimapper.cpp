#include "midimapper.hpp"
#include "mixer.hpp"
#include "waveform.hpp"
#include "utils.hpp"
#include <circle/logger.h>
#include "midi.hpp"
#include <algorithm>
#include "mixer.hpp"
#include "testpatch.hpp"

MidiMapper::MidiMapper() : voices{utils::emplaceArray<Voice, NUM_VOICES>(Mixer::get().samplerate)}
{
}

void MidiMapper::midiCallback(MidiEvent e)
{
	if (e.type == MidiEvent::Type::noteon)
	{
		if (auto voice = std::find_if(voices.begin(), voices.end(),
			[e](const auto& v)
			{
				return (v.businessReason().type == MidiEvent::Type::noteon &&
						v.businessReason().note.key == e.note.key);
			}); voice != voices.end())
		{
			voice->onKeyRepress();
		}
		else if (auto voice = std::find_if(voices.begin(), voices.end(),
			[e](const auto& v)
			{
				return v.businessReason().type == MidiEvent::Type::null;
			}); voice != voices.end())
		{
			voice->onKeyPress(e, std::make_unique<TestPatch>(Mixer::get().samplerate));
		}
	}
	if (e.type == MidiEvent::Type::noteoff)
	{
		if (auto voice = std::find_if(voices.begin(), voices.end(),
			[e](const auto& v)
			{
				return v.businessReason().type == MidiEvent::Type::noteon &&
					v.businessReason().note.key == e.note.key;
			}); voice != voices.end())
		{
			voice->onKeyRelease();
		}
	}
}
