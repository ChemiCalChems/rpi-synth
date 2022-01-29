#pragma once

#include <vector>
#include <circle/pwmsounddevice.h>
#include <memory>
#include <circle/logger.h>
#include "utils.hpp"
#include <utility> //pair

class Mixer : public CPWMSoundBaseDevice {
public:
	class Stream {
	public:
		Stream(std::string name) {
			streamName = name;
		}
		
		std::string streamName;
		float leftrightFade = 0.1f;
		float volume = 0.05f;

		int lowLevel(), highLevel(), nullLevel();
		
		virtual u32 getSample(long double) = 0;
		
	};
private:	
	Mixer (CInterruptSystem* interrupt_system, unsigned samplerate = 44100);
	utils::Buffer<std::pair<u32, u32>, 512> buffer; //Used to preprocess data to be sent on GetChunk call
public:
	long double t = 0;
	unsigned samplerate;
	std::vector<std::unique_ptr<Stream>> streams;
	
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

	unsigned GetChunk (u32* buffer, unsigned chunk_size) override;
};
