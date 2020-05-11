#pragma once

#include "synthesizer.hpp"
#include "midi.hpp"
#include <vector>
#include <array>
#include <utility>


class Sequencer;

class Sequencer : public MidiListener {
public:
	class Loop {
		std::vector<std::vector<MidiEvent>> data = {{}};
		std::size_t counter = 0;
	public:
		enum class Status {stopped, playing, recording} status = Status::stopped;
		
		void restart() {
			counter = 0;
		}
		std::vector<MidiEvent>& advance() {
			if (++counter == data.size()) {
				if (status == Status::playing) counter = 0;
				else if (status == Status::recording) {
					data.push_back({});
				}
			}
			return current();
		}
		std::vector<MidiEvent>& current() {
			return data.at(counter);
		}
		
	};
private:
	Sequencer() {
		MidiManager::get().add_listener(this);
	}
	
	
	unsigned long long currentFrame = 0;
	
	void tick();
public:
	float BPM = 120;
	const float PPQ = 240;

	Loop loop;
	
	Sequencer(Sequencer const&) = delete;
	void operator=(MidiManager const&) = delete;
	
	static Sequencer& get() {
		static Sequencer instance;
		return instance;
	}

	void cycle();

	void midi_callback(MidiEvent e) override;
};
