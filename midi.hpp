#pragma once

#include <vector>
#include <queue>
#include <circle/serial.h>

struct MidiEvent {
	enum class Type {null, noteon, noteoff};
	Type type;

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
	MidiManager();
	 
	
	CSerialDevice serial;
	std::vector<MidiListener*> listeners;
	
	void run_callbacks(MidiEvent event);
public:
	MidiManager(MidiManager const&) = delete;
	void operator=(MidiManager const&) = delete;

	
	static MidiManager& get() {
		static MidiManager instance;

		return instance;
	}

	void add_listener(MidiListener* listener);

	void run();
};

