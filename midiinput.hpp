#pragma once

#include "midi.hpp"
#include <circle/serial.h>
#include <queue>

class MidiInput {
	CSerialDevice serial;
	std::queue<unsigned char> bytes_read;
public:
	MidiInput();
	
	void read();
};
