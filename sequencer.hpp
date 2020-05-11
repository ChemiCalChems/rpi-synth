#pragma once

#include "synthesizer.hpp"
#include "midi.hpp"
#include <vector>
#include <array>
#include <utility>


class Sequencer;

class Sequencer {
	class Loop {
		std::vector<MidiEvent> data;
		std::size_t counter = 0;
	public:
		void restart() {
			counter = 0;
		}
		const MidiEvent& advance() {
			if (++counter >= data.size()) counter = 0;
			return current();
		}
		const MidiEvent& current() const {
			return data.at(counter);
		}
		
	};
	
	Sequencer() {}
	
	Loop loop;
	unsigned long long currentFrame = 0;
	
	void tick();
public:
	float BPM = 120;
	const float PPQ = 240;

	enum Status {stopped, paused, playing} status = stopped;
	
	Sequencer(Sequencer const&) = delete;
	void operator=(MidiManager const&) = delete;
	
	static Sequencer& get() {
		static Sequencer instance;
		return instance;
	}

	void cycle();
};
