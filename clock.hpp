#pragma once

struct TimeSignature {
	unsigned int top;
	unsigned int bottom;
};

struct Clock {
	unsigned lastTick;
	float BPM = 120;
	const float PPQ = 24;
	TimeSignature timeSignature;
	
	Clock() = default;	
	Clock(Clock const&) = delete;
	void operator=(Clock const&) = delete;

	static Clock& get() {
		static Clock instance;
		return instance;
	}
};
