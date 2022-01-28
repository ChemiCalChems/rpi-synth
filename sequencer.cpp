#include "sequencer.hpp"
#include <chrono>
#include <circle/logger.h>
#include <charconv>
#include "clock.hpp"

void Sequencer::ppq() {
	static int i=0;
	if (i == 0) {
		MidiEvent e;
		e.type = MidiEvent::Type::noteon;
		e.note.key = 60;
		e.note.velocity = 128;
		MidiManager::get().broadcast(e);
	}
	if (i == Clock::get().PPQ/4) {
		MidiEvent e;
		e.type = MidiEvent::Type::noteoff;
		e.note.key = 60;
		e.note.velocity = 128;
		MidiManager::get().broadcast(e);
	}
	if (++i == Clock::get().PPQ) {
		i = 0;
	}
	
	
	if (loop.status != Loop::Status::stopped) loop.advance();

	if (loop.status == Loop::Status::playing) 
		for (auto& e : loop.current()) MidiManager::get().broadcast(e);
}

void Sequencer::midi_callback(MidiEvent e) {
	if (loop.status != Loop::Status::recording) return;

	loop.current().push_back(e);
}

