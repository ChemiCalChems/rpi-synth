#pragma once

#include <circle/usertimer.h>
#include <circle/interrupt.h>

struct TimeSignature {
	unsigned int top;
	unsigned int bottom;
};

struct Clock {
	CUserTimer timer;
	unsigned lastTick;
	float BPM = 120;
	const float PPQ = 480;
	TimeSignature timeSignature;
	
	Clock() : timer(CInterruptSystem::Get(), tick, 0, true) {
		timer.Initialize();
		timer.Start(60*1000000/(BPM*PPQ));
	}

	Clock(Clock const&) = delete;
	void operator=(Clock const&) = delete;

	static Clock& get() {
		static Clock instance;
		return instance;
	}

	static void tick(CUserTimer*, void* param);
};
