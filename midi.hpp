#pragma once

#include <vector>
#include <queue>
#include <circle/serial.h>
#include <map>

struct MidiEvent {
	enum class Type {null, noteon, noteoff} type = Type::null;

	struct NoteEvent {
		unsigned char key, velocity;
	};

	union {
		NoteEvent note;
	};
};

struct MidiListener {
	virtual void midi_callback(MidiEvent) = 0;
};

class MidiManager;

class MidiManager {
	MidiManager() {}
	std::vector<MidiListener*> listeners;
	std::queue<MidiEvent> eventQueue;
	
	void run_callbacks(MidiEvent event);
public:
	MidiManager(MidiManager const&) = delete;
	void operator=(MidiManager const&) = delete;

	
	static MidiManager& get() {
		static MidiManager instance;

		return instance;
	}

	void add_listener(MidiListener* listener);
	void init();
	void run();
	void broadcast(const MidiEvent& ev);
};

