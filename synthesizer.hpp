#pragma once

#include <array>
#include <functional>
#include "mixer.hpp"
#include <circle/logger.h>
#include "midi.hpp"
#include <memory>
#include "waveform.hpp"

const unsigned short nh = 32;

class Synthesizer : public Mixer::Stream, public MidiListener {
	std::array<unsigned char, 128> key_velocities = {0};
	std::array<double, 16384> harmonics = {0};
	std::unique_ptr<Waveform> waveform;
public:
	Synthesizer(std::unique_ptr<Waveform> waveform_);
	
	double pitch_bend_semitones = 0;
	void set_patch(std::function<double(int)> f);
	u32 get_sample(double t);
	void set_key_velocity(unsigned int key, unsigned char velocity);
	unsigned char get_key_velocity(unsigned int key);

	void midi_callback(MidiEvent event);
	u32 getSample(long double t) override;
};
