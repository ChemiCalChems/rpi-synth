#include "clock.hpp"
#include "sequencer.hpp"

void Clock::tick(CUserTimer* t, void* param) {
		Sequencer::get().ppq();
		t->Start(60*1000000/(get().BPM*get().PPQ));
}
