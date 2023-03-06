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

double Voice::getSample()
{
	double result = patch->getSample();
	if (patch->isDone()) onDone();

	return result;
}

void Voice::onKeyPress(const MidiEvent& _reason, std::unique_ptr<Patch>&& _patch)
{
	reason = _reason;

	patch = std::move(_patch);

	turnOn();

	patch->onKeyPress(_reason);
}

void Voice::onKeyRelease()
{
	patch->onKeyRelease();
}

void Voice::onKeyRepress()
{
	patch->onKeyRepress();
}

void Voice::onDone()
{
	if (on) turnOff();
	reason = MidiEvent{};

	patch.reset();
}
