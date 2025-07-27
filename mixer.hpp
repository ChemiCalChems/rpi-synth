#pragma once

#include <array>
#include <string>
#include <circle/sound/pwmsoundbasedevice.h>
#include <memory>
#include <circle/logger.h>
#include "utils.hpp"
#include <utility> //pair
#include "voice.hpp"
#include "config.hpp"

class Mixer : public CPWMSoundBaseDevice {
	Mixer(unsigned samplerate = Config::SAMPLERATE);
	utils::Buffer<std::pair<u32, u32>, 384> buffer; //Used to preprocess data to be sent on GetChunk call
public:
	unsigned samplerate;
private:
	// TODO: Voices for sure don't belong to the MidiMapper, but
	// are they really owned by Mixer or are they standalone?
	std::array<std::pair<Voice, bool /*assigned*/>, Config::NUM_VOICES> voices;
public:
	
	Mixer(Mixer const&) = delete;
	void operator=(Mixer const&) = delete;

	static Mixer& get() {
		static Mixer instance;
		return instance;
	}

	static void init() {
		if (!get().Start()) {
			CLogger::Get()->Write("", LogError, "Couldn't start mixer!");
		}
	}
	
	double requestSample();
	void fillBuffer();

	Voice* requestVoice();
	void returnVoice(Voice*);

	unsigned GetChunk (u32* buffer, unsigned chunk_size) override;
	
	/*DEBUG INFO*/
	bool debugReady = false;
	unsigned int sampleCountBeforeCallback = 0;
};
