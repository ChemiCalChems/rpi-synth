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

void Mixer::requestSamples() {
	bool wasCalledFromInterrupt = (CurrentExecutionLevel() == IRQ_LEVEL);
	if (!wasCalledFromInterrupt) EnterCritical();

	if (!buffer.full()) {
		u32 sample = (GetRangeMin() + GetRangeMax())/2;

		for (auto& stream : streams) {
			sample += stream->getSample(t);
		}

		buffer.push({sample, sample});

		t += 1.L/samplerate;
		if (t > 60) t = 0;
	}
	if (!wasCalledFromInterrupt) LeaveCritical();
}

unsigned Mixer::GetChunk (u32* buf, unsigned chunk_size) {
	for (unsigned i = 0; i<chunk_size; i+=2) {
		if (buffer.empty()) {
			requestSamples();
		}

		auto sample = buffer.pop();
		*buf++ = sample.first;
		*buf++ = sample.second;
	}
	//assert(buffer.empty());
	return chunk_size;
}

