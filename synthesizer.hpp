#pragma once

#include <array>
#include <functional>
#include <circle/pwmsounddevice.h>
#include <circle/logger.h>
#include "midi.hpp"

const unsigned short nh = 32;

class Synthesizer : public CPWMSoundBaseDevice, public MidiListener {
	std::array<unsigned char, 128> key_velocities = {0};
	std::array<double, 16384> harmonics = {0};
public:
	double t = 0;

	Synthesizer(CInterruptSystem* interrupt_system);
	
	double pitch_bend_semitones = 0;
	void set_patch(std::function<double(int)> f);
	u32 get_sample(double t);
	void set_key_velocity(unsigned int key, unsigned char velocity);
	unsigned char get_key_velocity(unsigned int key);
	void start();

	void midi_callback(MidiEvent event);
	unsigned GetChunk (u32* buf, unsigned chunk_size);
};
