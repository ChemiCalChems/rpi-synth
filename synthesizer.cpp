#include "synthesizer.hpp"
#include "utils.hpp"
#include <queue>
//#include <Vc/Vc>
#include <circle/logger.h>

unsigned char nibble(unsigned char byte, bool first = true) {
	return first ? byte >> 4 : byte % 16;
}


Synthesizer::Synthesizer(CInterruptSystem* interrupt_system)
	: CPWMSoundBaseDevice(interrupt_system, 44100, 512),
	  serial(interrupt_system, true) {
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
		float f = utils::midi_to_note_freq(i);
		double intpart;
		result += ((std::modf(f*t, &intpart) <= 0.5)?high_level:low_level);
		
	}
	
	return result;
}																								 

void Synthesizer::set_key_velocity(unsigned int key, unsigned char velocity) {
	key_velocities.at(key) = velocity;
}

unsigned char Synthesizer::get_key_velocity(unsigned int key) {
	return key_velocities[key];
}

void Synthesizer::process(CLogger& logger) {
	if (!Start()) {
		CLogger::Get()->Write("Synthesizer::process()", LogWarning, "Couldn't start SoundDevice");
	}

	serial.Initialize(38400);
	
	int n = 0;
	std::queue<unsigned char> bytes_read;

	while(true) {		
		//CLogger::Get()->Write("Synthesizer::process()", LogNotice, std::to_string(bytes_read.size()).c_str());
		unsigned char buffer [64];
		int byte_count = serial.Read(buffer, sizeof buffer);
		if (byte_count <= 0) continue;

		//CLogger::Get()->Write("process()", LogNotice, std::to_string(byte_count).c_str());
		
		for (unsigned int i = 0; i<byte_count; i++)
			bytes_read.push(buffer[i]);
		 	
		bool keepReading = true;	
		while (keepReading && !bytes_read.empty()) {
			auto byte = bytes_read.front();
			
			switch(nibble(byte)) {
			case 8: //1000 : noteOff
				{
					if (bytes_read.size() < 3) {
						keepReading = false;
						break;
					}
					bytes_read.pop();

					unsigned char key;
					key = bytes_read.front(); bytes_read.pop();
					/*velocity = bytes_read.front();*/ bytes_read.pop();
					Synthesizer::set_key_velocity(key, 0);
					//logger.Write("e", LogNotice, "OOOOOO");
					break;
				}
			case 9: //1001 : noteOn
				{
					if (bytes_read.size() < 3) {
						keepReading = false;
						break;
					}
					bytes_read.pop();
					
					unsigned char key, velocity;
					key = bytes_read.front(); bytes_read.pop();
					velocity = bytes_read.front(); bytes_read.pop();
					Synthesizer::set_key_velocity(key, 128);
					//logger.Write("e", LogNotice, "EEEEE");
					break;
				}
				/*
			case 12: //1100 : programChange
				{
					if (bytes_read.size() < 2) {
						keepReading = false;
						break;
					}
					bytes_read.pop();

					unsigned char program = bytes_read.front();
					bytes_read.pop();
					std::cout << "setting" << (int)program << std::endl;
					Synthesizer::set_patch(patches.at(program));
					break;
					}
				*/
			case 14: //1110 : pitchbend
				{
					if (bytes_read.size() < 3) {
						keepReading = false;
						break;
					}
					bytes_read.pop();
					unsigned char lsb = bytes_read.front(); bytes_read.pop();
					unsigned char msb = bytes_read.front(); bytes_read.pop();

					short pitchbend = ((short)msb << 7) + lsb;
					double semitones = (double)(pitchbend - 8192) / 16834.f;
					Synthesizer::pitch_bend_semitones = semitones;
					break;
				}
			default: bytes_read.pop();
			}
		}
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
