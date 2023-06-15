#include "testpatch.hpp"
#include "midi.hpp"

void TestPatch::onKeyPress(const MidiEvent& event)
{
	oscillator.waveform = std::make_unique<WaveformBase<0>>(utils::midi_freqs[event.note.key]);
	adsr = ADSREnvelopeGenerator{};
}

void TestPatch::onKeyRelease()
{
	adsr.released(t);
}

void TestPatch::onKeyRepress()
{
	t = 0;
	adsr = ADSREnvelopeGenerator{};
}

bool TestPatch::isDone() const
{
	return done;
}

double TestPatch::getSample()
{
	requestSample.send();

	double result = adsr(t)*lastSample.value();
	lastSample.reset();

	if (adsr.done) {done = true;}

	t += 1./double(samplerate);
	return result;
}
