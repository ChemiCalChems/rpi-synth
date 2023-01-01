#include "midi.hpp"
#include <circle/logger.h>

void MidiManager::run_callbacks(MidiEvent event) {
	for (auto listener : listeners) listener->midi_callback(event);
}

void MidiManager::add_listener(MidiListener* listener) {
	CLogger::Get()->Write("MidiManager", LogNotice, "Added listener");
	listeners.push_back(listener);
}

void MidiManager::init() {
	
}

void MidiManager::broadcast(const MidiEvent& ev) {
	eventBuffer.push(ev);
}

void MidiManager::run() {
	while (!eventBuffer.empty()) {
		run_callbacks(eventBuffer.pop());
	}
}
