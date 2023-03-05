#include "voice.hpp"
#include "mixer.hpp"

Voice::Voice(unsigned int _samplerate) : samplerate{_samplerate}
{
}

void Voice::turnOn()
{
	Mixer::get().registerVoice(this);
	on = true;
}

void Voice::turnOff()
{
	Mixer::get().unregisterVoice(this);
	on = false;
}

bool Voice::isOn() const
{
	return on;
}

MidiEvent Voice::businessReason() const
{
	return reason;
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

void Voice::onKeyPress(const MidiEvent& _reason)
{
	reason = _reason;
	waveform = std::make_unique<WaveformBase<0>>(utils::midi_freqs[reason.note.key]);
	resetTime();
	if (!on) turnOn();
}

void Voice::onKeyRelease()
{
	if (on) turnOff();
	reason = MidiEvent{};
}
