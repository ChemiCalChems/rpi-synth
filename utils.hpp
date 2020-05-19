#include <cmath>

namespace utils {
	double midi_to_note_freq(int note) {
		//Calculate difference in semitones to A4 (note number 69) and use equal temperament to find pitch.
		return 440 * std::pow(2, ((double)note - 69) / 12);
	}

	const std::array<double, 128> midi_freqs = []() {
												   std::array<double, 128> res={0};
												   for (unsigned i=0; i<res.size(); i++) res[i]=midi_to_note_freq(i);
												   return res;
											   }();
	template<typename T1, typename T2>
	T2 mapToRange(T1 value, T2 rangeMin, T2 rangeMax) {
		T2 center = (rangeMin + rangeMax)/2;
		T2 halfRange = std::abs(rangeMax - center);
		return center + value * halfRange;
	}
}
