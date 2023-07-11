#include "testpatch.hpp"
#include "midi.hpp"

void TestPatch::onKeyPress(const MidiEvent& event)
{
	oscillator.waveform = std::make_unique<WaveformBase<0>>(utils::midi_freqs[event.note.key]);
	adsrKeyPressed.send();
}

void TestPatch::onKeyRelease()
{
	adsrKeyReleased.send();
}

void TestPatch::onKeyRepress()
{
	adsrKeyPressed.send();
}

bool TestPatch::isDone() const
{
	return done;
}

double TestPatch::getSample()
{
	requestSample.send();

	double result = lastSample.value();
	lastSample.reset();

	t += 1./double(samplerate);
	return result;
}
