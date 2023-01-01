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
	template <typename T, std::size_t N>
	using Buffer = utils::Buffer<T,N>;
	
	MidiManager() {}
	std::vector<MidiListener*> listeners;
	Buffer<MidiEvent, 1024> eventBuffer;
	
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

struct MidiListener {
	virtual void midi_callback(MidiEvent) = 0;
	MidiListener()
	{
		MidiManager::get().add_listener(this);
	}
};

