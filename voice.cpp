#include "voice.hpp"
#include "mixer.hpp"

Voice::Voice(unsigned int _samplerate) : samplerate{_samplerate}
{
}

double Voice::getSample()
{
	double result = patch->getSample();
	if (patch->isDone()) onDone();

	return result;
}

void Voice::onKeyPress(const MidiEvent& _reason, std::unique_ptr<Patch>&& _patch)
{
	patch = std::move(_patch);

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
    if (onVoiceDoneCallback)
    {
        onVoiceDoneCallback();
    }

	patch.reset();
}
