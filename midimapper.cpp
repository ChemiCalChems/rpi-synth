#include "midimapper.hpp"
#include "mixer.hpp"
#include "waveform.hpp"
#include "utils.hpp"
#include <circle/logger.h>
#include "midi.hpp"
#include <algorithm>
#include "mixer.hpp"
#include "testpatch.hpp"

void MidiMapper::midiCallback(MidiEvent e)
{
	if (e.type == MidiEvent::Type::noteon)
	{
		if (auto voice = mappedVoices.find(e.note.key); voice != mappedVoices.end())
		{
			voice->second->onKeyRepress();
		}
		else
		{
			if (auto voice = Mixer::get().requestVoice())
			{
				mappedVoices.insert(std::make_pair(e.note.key, voice));
                voice->onVoiceDoneCallback = [this, voice]{
                    std::erase_if(mappedVoices, [voice](const auto& elem){return voice == elem.second;});
                    Mixer::get().returnVoice(voice);
                };

				voice->onKeyPress(e, std::make_unique<TestPatch>(Mixer::get().samplerate));
			}
		}
	}
	if (e.type == MidiEvent::Type::noteoff)
	{
		if (auto voice = mappedVoices.find(e.note.key); voice != mappedVoices.end())
		{
			voice->second->onKeyRelease();
		}
	}
}

void MidiMapper::onVoiceDone(Voice* voice)
{
    std::erase_if(mappedVoices, [voice](const auto& elem){return elem.second == voice;});
}
