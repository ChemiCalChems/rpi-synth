#include "testpatch.hpp"
#include "midi.hpp"

void TestPatch::onKeyPress(const MidiEvent& event)
{
	double freq{utils::midi_freqs[event.note.key]};
	filter.SetTransferFunction(BilinearTransform(ButterworthLowPass(freq*2.), samplerate).InvertVariable());
	oscillator.waveform = std::make_unique<WaveformBase<0>>(freq);
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
