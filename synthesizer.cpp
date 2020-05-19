#include "synthesizer.hpp"
#include "utils.hpp"
#include <queue>
//#include <Vc/Vc>
#include <circle/logger.h>

unsigned char nibble(unsigned char byte, bool first = true) {
	return first ? byte >> 4 : byte % 16;
}


Synthesizer::Synthesizer(std::unique_ptr<Waveform> waveform_)
	: Stream("synth"),
	  waveform(std::move(waveform_)) {
	MidiManager::get().add_listener(this);
}



void Synthesizer::set_patch(std::function<double(int)> f) {
	for (unsigned i=0; i<harmonics.size(); i++) {
		harmonics[i] = f(i+1);
	}
}

u32 Synthesizer::getSample(long double t) {
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
		

	u32 result = nullLevel();
	
	for (unsigned i = 0; i < key_velocities.size(); i++) {
		if (key_velocities.at(i) == 0) continue;
		float f = utils::midi_freqs[i];
	    result += utils::mapToRange(waveform->getSample(f,t), lowLevel(), highLevel());
	}
	
	return result;
}																								 

void Synthesizer::set_key_velocity(unsigned int key, unsigned char velocity) {
	key_velocities.at(key) = velocity;
}

unsigned char Synthesizer::get_key_velocity(unsigned int key) {
	return key_velocities[key];
}

void Synthesizer::midi_callback(MidiEvent event) {
	switch(event.type) {
	case MidiEvent::Type::noteoff:
		Synthesizer::set_key_velocity(event.note.key, 0);
		break;
	case MidiEvent::Type::noteon:
		Synthesizer::set_key_velocity(event.note.key, 128);
		break;
	default:
		break;
	}	
}

