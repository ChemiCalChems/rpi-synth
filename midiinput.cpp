#include "midiinput.hpp"
#include <circle/logger.h>
#include <string>
#include <sstream>
#include <iomanip>

MidiInput::MidiInput(CInterruptSystem* interruptSystem)
	: serial(interruptSystem, true) {
	if (!serial.Initialize(115200)) CLogger::Get()->Write("a", LogError, "couldn't init serial");
	CLogger::Get()->Write("a", LogNotice, "started serial");
}

void MidiInput::read() {
	unsigned char buffer [128];
	auto byte_count = serial.Read(buffer, sizeof buffer);

	if (byte_count <= 0) return;

	for (unsigned int i = 0; i<byte_count; i++)
		bytes_read.push(buffer[i]);
		 	
	bool keepReading = true;	
	while (keepReading && !bytes_read.empty()) {
		auto byte = bytes_read.front();
		MidiEvent e;
		
		switch(byte>>4) {
		case 8: //1000 : noteOff
			{
				if (bytes_read.size() < 3) {
					keepReading = false;
					break;
				}
				bytes_read.pop();

				unsigned char key, velocity;
				key = bytes_read.front(); bytes_read.pop();
				velocity = bytes_read.front(); bytes_read.pop();
				e.type = MidiEvent::Type::noteoff;
				e.note.key = key;
				e.note.velocity = velocity;
				break;
			}
		case 9: //1001 : noteOn
			{
				if (bytes_read.size() < 3) {
					keepReading = false;
					break;
				}
				bytes_read.pop();
					
				unsigned char key, velocity;
				key = bytes_read.front(); bytes_read.pop();
				velocity = bytes_read.front(); bytes_read.pop();

				e.type = MidiEvent::Type::noteon;
				e.note.key = key;
				e.note.velocity = velocity;
				break;
			}
			/*
			  case 12: //1100 : programChange
			  {
			  if (bytes_read.size() < 2) {
			  keepReading = false;
			  break;
			  }
			  bytes_read.pop();

			  unsigned char program = bytes_read.front();
			  bytes_read.pop();
			  std::cout << "setting" << (int)program << std::endl;
			  Synthesizer::set_patch(patches.at(program));
			  break;
			  }
				
			  case 14: //1110 : pitchbend
			  {
			  if (bytes_read.size() < 3) {
			  keepReading = false;
			  break;
			  }
			  bytes_read.pop();
			  unsigned char lsb = bytes_read.front(); bytes_read.pop();
			  unsigned char msb = bytes_read.front(); bytes_read.pop();

			  short pitchbend = ((short)msb << 7) + lsb;
			  double semitones = (double)(pitchbend - 8192) / 16834.f;
			  Synthesizer::pitch_bend_semitones = semitones;
			  break;
			  }
			*/
		case 15: //1111: system message
			{
				if (byte == 0b11111000)
				{
					e.type = MidiEvent::Type::timingClock;
				}
				bytes_read.pop();
				break;
			}
		default: 
			CLogger::Get()->Write("a", LogNotice, "Bullshit byte");
			CLogger::Get()->Write("a", LogNotice, std::to_string(byte).c_str());
			bytes_read.pop();
		}
		if (e.type != MidiEvent::Type::null) MidiManager::get().broadcast(e);
	}
}
