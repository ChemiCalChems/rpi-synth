#include "sequencer.hpp"
#include <chrono>
#include <circle/logger.h>
#include <charconv>

void Sequencer::cycle() {
	static unsigned usTillNextTick = 60*1000000/(BPM*PPQ);
	unsigned now = CTimer::GetClockTicks();
	auto usSinceLastTick = now - lastTick;
	//CLogger::Get()->Write ("", LogNotice, std::to_string(usSinceLastTick).c_str());
	if (usSinceLastTick >= usTillNextTick) {
		usTillNextTick = 2*60*1000000/(BPM*PPQ) - usSinceLastTick;
		lastTick = now;
		tick();
	}
}

void Sequencer::tick() {
	/*static int i=0;
	if (i == 0) {
		MidiEvent e;
		e.type = MidiEvent::Type::noteon;
		e.note.key = 60;
		e.note.velocity = 128;
		MidiManager::get().broadcast(e);
	}
	if (i == 4) {
		MidiEvent e;
		e.type = MidiEvent::Type::noteoff;
		e.note.key = 60;
		e.note.velocity = 128;
		MidiManager::get().broadcast(e);
	}
	if (++i == PPQ) {
		i = 0;
		CLogger::Get()->Write("", LogNotice, "q");
	}*/
	
	
	if (loop.status != Loop::Status::stopped) loop.advance();

	if (loop.status == Loop::Status::playing) 
		for (auto& e : loop.current()) MidiManager::get().broadcast(e);
}

void Sequencer::midi_callback(MidiEvent e) {
	if (loop.status != Loop::Status::recording) return;

	loop.current().push_back(e);
}

