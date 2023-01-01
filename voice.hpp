#pragma once

#include "waveform.hpp"
#include <memory>

class Voice 
{
	const unsigned int samplerate;
	mutable double t = 0;
	std::unique_ptr<Waveform> waveform;
public:
	Voice(unsigned int _samplerate);
	
	void setWaveform(std::unique_ptr<Waveform>&&);
	void resetTime() const;
	double getSample() const;
};
