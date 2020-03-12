#include "synthesizer.hpp"
#include "utils.hpp"
#include <queue>
//#include <Vc/Vc>
#include <circle/logger.h>

unsigned char nibble(unsigned char byte, bool first = true) {
	return first ? byte >> 4 : byte % 16;
}


Synthesizer::Synthesizer(CInterruptSystem* interrupt_system)
	: CPWMSoundBaseDevice(interrupt_system, 44100, 512) {
	MidiManager::get().add_listener(this);
}



void Synthesizer::set_patch(std::function<double(int)> f) {
	for (int i=0; i<harmonics.size(); i++) {
		harmonics[i] = f(i+1);
	}
}

u32 Synthesizer::get_sample(double t) {
	/* ADDITIVE SYNTHESIS */
	/*
	Vc::float_v vector_result = Vc::float_v::Zero();
	float result = 0;
	for (int i = 0; i<key_velocities.size(); i++) {
		if (key_velocities.at(i) == 0) continue;
		//std::cout << "key" << i << std::endl;
		auto v = get_key_velocity(i);
		float f = utils::midi_to_note_freq(i) * std::pow(2, pitch_bend_semitones/12.f);
		int rendered_harmonics = 1;
		for (; rendered_harmonics*f < 48000 && rendered_harmonics <= 64; rendered_harmonics++);
		int j = 0;
		for (;j + Vc::float_v::size() <= rendered_harmonics; j+=Vc::float_v::size()) {
			Vc::float_v twopift = Vc::float_v::generate([f,t,j](int n){return 2*3.14159268*(j+n+1)*f*t;});
			Vc::float_v harms = Vc::float_v::generate([j](int n){return harmonics[n+j];});
			vector_result += v*harms*Vc::sin(twopift); 
			}
		for (; j < rendered_harmonics; j++) {
			result += (float)v*harmonics[j]*std::sin(2*3.14159268*(j+1)*f*t);
		}
	}
	return (result + vector_result.sum())/512;
	*/
		
	int low_level = GetRangeMin()/5, high_level = GetRangeMax()/5, null_level = (low_level + high_level)/2;

	u32 result = null_level;

	for (unsigned i = 0; i < key_velocities.size(); i++) {
		if (key_velocities.at(i) == 0) continue;
		float f = utils::midi_freqs[i];
		double intpart;
		result += ((std::modf(f*t, &intpart) <= (0.7f /*+ 0.4 * std::sin(3.1415*t) */) ) ? high_level : low_level);		 
	}
	
	return result;
}																								 

void Synthesizer::set_key_velocity(unsigned int key, unsigned char velocity) {
	key_velocities.at(key) = velocity;
}

unsigned char Synthesizer::get_key_velocity(unsigned int key) {
	return key_velocities[key];
}

void Synthesizer::start() {
	if (!Start()) {
		CLogger::Get()->Write("Synthesizer::process()", LogWarning, "Couldn't start SoundDevice");
	}
}

void Synthesizer::midi_callback(MidiEvent event) {
	switch(event.type) {
	case MidiEvent::Type::noteoff:
		Synthesizer::set_key_velocity(event.note.key, 0);
		break;
	case MidiEvent::Type::noteon:
		Synthesizer::set_key_velocity(event.note.key, 128);
		break;
	}
	
}

unsigned Synthesizer::GetChunk(u32* buf, unsigned chunk_size) {
	//CLogger::Get()->Write("a", LogNotice, "AAAAA");
	for (int i = 0; i<chunk_size; i+=2) {
		u32 sample = Synthesizer::get_sample(Synthesizer::t);
		*buf++ = (u32) sample;
		*buf++ = (u32) sample;
				
		Synthesizer::t += (double)1/(double)44100;
		if (Synthesizer::t > 60) Synthesizer::t = 0;
	}
	//CLogger::Get()->Write("a", LogNotice, "A");
	return chunk_size;
}
