#pragma once

#include "waveform.hpp"
#include <memory>

class Voice 
{
	const unsigned int samplerate;
	mutable double t = 0;
	std::unique_ptr<Waveform> waveform;
	bool on = false;
public:
	Voice(unsigned int _samplerate);

	Voice(const Voice&) = delete;
	Voice& operator=(const Voice&) = delete;
	Voice(Voice&&) = delete;
	Voice& operator=(Voice&&) = delete;
	
	void turnOn();
	void turnOff();
	bool isOn() const;

	void setWaveform(std::unique_ptr<Waveform>&&);
	void resetTime() const;
	double getSample() const;
};
