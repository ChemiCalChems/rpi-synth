#pragma once

#include <vector>
#include <circle/pwmsounddevice.h>
#include <memory>
#include <circle/logger.h>

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
	long double t = 0;
public:
	unsigned samplerate = 44100;
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

	unsigned GetChunk (u32* buffer, unsigned chunk_size);
};
