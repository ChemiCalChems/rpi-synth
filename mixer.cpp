#include "mixer.hpp"

int Mixer::Stream::lowLevel() {
	return volume * Mixer::get().GetRangeMin();
}

int Mixer::Stream::highLevel() {
	return volume * Mixer::get().GetRangeMax();
}

int Mixer::Stream::nullLevel() {
	return (lowLevel()+highLevel())/2;
}

Mixer::Mixer(CInterruptSystem* interrupt_system)
	: CPWMSoundBaseDevice(interrupt_system, 44100, 512) {
}

unsigned Mixer::GetChunk (u32* buf, unsigned chunk_size) {
	for (unsigned i = 0; i<chunk_size; i+=2) {
		u32 sample = (GetRangeMin() + GetRangeMax())/2;

		for (auto& stream : streams) {
			sample += stream->getSample();
		}
		
		*buf++ = sample;
		*buf++ = sample;
	}

	return chunk_size;
}
