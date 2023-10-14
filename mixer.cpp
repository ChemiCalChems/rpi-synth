#include "mixer.hpp"
#include <circle/synchronize.h>
#include <numeric>
#include <string>
#include <circle/logger.h>
#include <algorithm>

Mixer::Mixer(CInterruptSystem* interrupt_system, unsigned int samplerate_)
	: CPWMSoundBaseDevice(interrupt_system, samplerate_, 384), samplerate{samplerate_}
{
}

double Mixer::requestSample() {
	return std::accumulate(registeredVoices.begin(), registeredVoices.end(), 0.0, [](const double result, auto& voice) {return result + 0.05 * voice->getSample();});
}

void Mixer::fillBuffer() {
	if (!buffer.full()) {
		u32 sample = double(GetRangeMin() + GetRangeMax()) / 2. + requestSample() * (double(GetRangeMax()) - double(GetRangeMin()));

		buffer.push(std::make_pair(sample, sample));
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
			sample = {(GetRangeMin() + GetRangeMax())/2, (GetRangeMin() + GetRangeMax())/2};
		} else [[likely]] {
			sample = buffer.pop();
		}
		
		*buf++ = sample.first;
		*buf++ = sample.second;
	}
	//assert(buffer.empty());
	return chunk_size;
}

