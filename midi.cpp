#include "midi.hpp"
#include <circle/logger.h>

void MidiManager::runCallbacks(MidiEvent event) {
	for (auto listener : listeners) listener->midiCallback(event);
}

MidiListener::MidiListener()
{
	CLogger::Get()->Write("MidiManager", LogNotice, "Added listener");
	MidiManager::get().listeners.push_back(this);
}

MidiListener::~MidiListener()
{
	CLogger::Get()->Write("MidiManager", LogNotice, "Removed listener");
	std::erase(MidiManager::get().listeners, this);
}

void MidiManager::init() {
	
}

void MidiManager::broadcast(const MidiEvent& ev) {
	eventBuffer.push(ev);
}

void MidiManager::run() {
	while (!eventBuffer.empty()) {
		runCallbacks(eventBuffer.pop());
	}
}
