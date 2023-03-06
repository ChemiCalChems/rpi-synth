#pragma once

struct MidiEvent;

struct Patch
{
	virtual void onKeyPress(const MidiEvent&) = 0;
	virtual void onKeyRelease() = 0;
	virtual void onKeyRepress() = 0;

	virtual bool isDone() const = 0;

	virtual double getSample() = 0;

	virtual ~Patch() = default;
};
