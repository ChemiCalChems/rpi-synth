#include "mixer.hpp"
#include <circle/synchronize.h>
#include <string>

int Mixer::Stream::lowLevel() {
	return volume * Mixer::get().GetRangeMin();
}

int Mixer::Stream::highLevel() {
	return volume * Mixer::get().GetRangeMax();
}

int Mixer::Stream::nullLevel() {
	return (lowLevel()+highLevel())/2;
}

Mixer::Mixer(CInterruptSystem* interrupt_system, unsigned samplerate_)
	: CPWMSoundBaseDevice(interrupt_system, samplerate_, 512) {
	samplerate = samplerate_;
}

std::pair<u32, u32> Mixer::requestSample() {
	u32 sample = (GetRangeMin() + GetRangeMax())/2;

	for (auto& stream : streams) {
		sample += stream->getSample(t);
	}

	t += 1.L/samplerate;
	if (t > 60) t = 0;
	
	return {sample, sample};
}

void Mixer::fillBuffer() {
	if (!buffer.full()) {
		auto sample = requestSample();
		buffer.push(sample);
	}
}

unsigned Mixer::GetChunk (u32* buf, unsigned chunk_size) {
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

