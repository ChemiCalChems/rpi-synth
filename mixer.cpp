#include "mixer.hpp"
#include <circle/synchronize.h>
#include <string>
#include <circle/logger.h>

Mixer::Mixer(CInterruptSystem* interrupt_system, unsigned int samplerate_)
	: CPWMSoundBaseDevice(interrupt_system, samplerate_, 384),
	  voices{utils::emplaceArray<Voice, NUM_VOICES>(samplerate_)},
	  isVoiceLent{false} {
	samplerate = samplerate_;
}

std::pair<u32, u32> Mixer::requestSample() {
	u32 sample = (GetRangeMin() + GetRangeMax())/2;

	for (std::size_t i = 0; i<voices.size(); i++) {
		if (isVoiceLent[i] && voices[i].isOn())
		{
			sample += utils::mapToRange(voices[i].getSample(), GetRangeMin(), GetRangeMax());
		}
	}

	return {0.05*sample, 0.05*sample};
}

void Mixer::fillBuffer() {
	if (!buffer.full()) {
		buffer.push(requestSample());
	}
}
Voice* Mixer::requestVoice()
{
	//CLogger::Get()->Write("Mixer", LogNotice, "Voice requested");
	for (std::size_t i = 0; i < isVoiceLent.size(); i++)
	{
		if (!isVoiceLent[i]) 
		{
			//CLogger::Get()->Write("Mixer", LogNotice, std::to_string(i).c_str());
			isVoiceLent[i] = true;
			return &voices[i];
		}
	}
	return nullptr;
}

void Mixer::returnVoice(Voice* const voice)
{
	for (std::size_t i = 0; i < voices.size(); i++)
	{
		if (&voices[i] == voice) isVoiceLent[i] = false;
	}
}

unsigned Mixer::GetChunk (u32* buf, unsigned chunk_size) {
	sampleCountBeforeCallback = buffer.size;
	debugReady = true;

	for (unsigned i = 0; i<chunk_size; i+=2) {
		std::pair<u32, u32> sample;
		if (buffer.empty()) [[unlikely]] {
			sample = requestSample();
		} else [[likely]] {
			sample = buffer.pop();
		}
		
		*buf++ = sample.first;
		*buf++ = sample.second;
	}
	//assert(buffer.empty());
	return chunk_size;
}

