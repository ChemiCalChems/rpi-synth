#pragma once

#include <array>
#include <string>
#include <circle/sound/pwmsoundbasedevice.h>
#include <memory>
#include <circle/logger.h>
#include "utils.hpp"
#include <utility> //pair
#include "voice.hpp"

#define NUM_VOICES 16

class Mixer : public CPWMSoundBaseDevice {
	Mixer (CInterruptSystem* interrupt_system, unsigned samplerate = 44100);
	utils::Buffer<std::pair<u32, u32>, 384> buffer; //Used to preprocess data to be sent on GetChunk call
public:
	unsigned samplerate;
	std::vector<Voice*> registeredVoices;
	
	Mixer(Mixer const&) = delete;
	void operator=(Mixer const&) = delete;

	static Mixer& get() {
		static Mixer instance (CInterruptSystem::Get());
		return instance;
	}

	static void init() {
		if (!get().Start()) {
			CLogger::Get()->Write("", LogError, "Couldn't start mixer!");
		}
	}
	
	std::pair<u32, u32> requestSample(); //Requests samples from associated streams
	void fillBuffer();

	void registerVoice(Voice* const);
	void unregisterVoice(Voice* const);

	unsigned GetChunk (u32* buffer, unsigned chunk_size) override;
	
	/*DEBUG INFO*/
	bool debugReady = false;
	unsigned int sampleCountBeforeCallback = 0;
};
