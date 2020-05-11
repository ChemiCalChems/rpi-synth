#include "sequencer.hpp"
#include <chrono>

void Sequencer::cycle() {
	static decltype(std::chrono::high_resolution_clock::now()) lastTick;
	if (status != playing) return;

	auto now = std::chrono::high_resolution_clock::now();
	auto usSinceLastTick = std::chrono::duration<float, std::ratio<1,1000000>>(now - lastTick).count();

	if (usSinceLastTick < 60000000.f/(BPM*PPQ)) return;

	lastTick = now;
	tick();
}

void Sequencer::tick() {
	const MidiEvent ev = loop.advance();
	
}

