#include "midi.hpp"
#include <circle/logger.h>

void MidiManager::run_callbacks(MidiEvent event) {
	for (auto listener : listeners) listener->midi_callback(event);
}

void MidiManager::add_listener(MidiListener* listener) {
	listeners.push_back(listener);
}

void MidiManager::init() {
	
}

void MidiManager::broadcast(const MidiEvent& ev) {
	eventQueue.push(ev);
}

void MidiManager::run() {
	while (!eventQueue.empty()) {
		run_callbacks(eventQueue.front());
		eventQueue.pop();
	}
}
