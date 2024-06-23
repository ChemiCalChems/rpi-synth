#pragma once

#include <vector>
#include <circle/serial.h>
#include <map>
#include "utils.hpp"

struct MidiEvent {
	enum class Type {null, noteon, noteoff, timingClock} type = Type::null;

	struct NoteEvent {
		unsigned char key, velocity;
	};

	union {
		NoteEvent note;
	};
};

class MidiListener;

class MidiManager {
	friend class MidiListener;

	MidiManager() = default;

	std::vector<MidiListener*> listeners;
	utils::Buffer<MidiEvent, 1024> eventBuffer;
	
	void runCallbacks(MidiEvent event);
public:
	MidiManager(MidiManager const&) = delete;
	void operator=(MidiManager const&) = delete;

	
	static MidiManager& get() {
		static MidiManager instance;

		return instance;
	}

	void init();
	void run();
	void broadcast(const MidiEvent& ev);
};

struct MidiListener {
	virtual void midiCallback(MidiEvent) = 0;
	MidiListener();

	MidiListener(const MidiListener&) = delete;
	MidiListener& operator=(const MidiListener&) = delete;

	MidiListener(MidiListener&& other) = delete;
	MidiListener& operator=(MidiListener&&) = delete;

	virtual ~MidiListener();
};
