#include "mixer.hpp"
#include <circle/synchronize.h>
#include <string>
#include <circle/logger.h>
#include <algorithm>

Mixer::Mixer(CInterruptSystem* interrupt_system, unsigned int samplerate_)
	: CPWMSoundBaseDevice(interrupt_system, samplerate_, 384), samplerate{samplerate_}
{
}

std::pair<u32, u32> Mixer::requestSample() {
	u32 sample = (GetRangeMin() + GetRangeMax())/2;

	for (auto v : registeredVoices) {
		sample += 0.05 * v->getSample() * (double(GetRangeMax()) - double(GetRangeMin()));
	}

	return {sample, sample};
}

void Mixer::fillBuffer() {
	if (!buffer.full()) {
		buffer.push(requestSample());
	}
}
void Mixer::registerVoice(Voice* const voice)
{
	registeredVoices.push_back(voice);
}

void Mixer::unregisterVoice(Voice* const voice)
{
	registeredVoices.erase(std::remove(registeredVoices.begin(), registeredVoices.end(), voice),
		registeredVoices.end());
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

