#include "sequencer.hpp"
#include <chrono>
#include <circle/logger.h>
void Sequencer::cycle() {
	static decltype(std::chrono::high_resolution_clock::now()) lastTick;

	auto now = std::chrono::high_resolution_clock::now();
	auto usSinceLastTick = std::chrono::duration<float, std::ratio<1,1000000>>(now - lastTick).count();

	if (usSinceLastTick < 60000000.f/(BPM*PPQ)) return;

	lastTick = now;
	tick();
}

void Sequencer::tick() {
	//CLogger::Get()->Write ("", LogNotice, "tick");
	if (loop.status != Loop::Status::stopped) loop.advance();

	if (loop.status == Loop::Status::playing) 
		for (auto& e : loop.current()) MidiManager::get().broadcast(e);
}

void Sequencer::midi_callback(MidiEvent e) {
	if (loop.status != Loop::Status::recording) return;

	loop.current().push_back(e);
}

