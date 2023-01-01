#pragma once

#include "midi.hpp"
#include <circle/serial.h>
#include <queue>
#include <circle/interrupt.h>

class MidiInput {
	CSerialDevice serial;
	std::queue<unsigned char> bytes_read;
public:
	MidiInput(CInterruptSystem*);
	
	void read();
};
