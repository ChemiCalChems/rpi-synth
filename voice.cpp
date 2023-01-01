#include "voice.hpp"

Voice::Voice(unsigned int _samplerate) : samplerate{_samplerate}
{
}

void Voice::setWaveform(std::unique_ptr<Waveform>&& _waveform)
{
	waveform = std::move(_waveform);
}

void Voice::resetTime() const
{
	t = 0;
}

double Voice::getSample() const
{
	double result = waveform->getSample(t);

	t += 1./double(samplerate);
	return result;
}
