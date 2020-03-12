#include <cmath>

namespace utils {
	constexpr double midi_to_note_freq(int note) {
		//Calculate difference in semitones to A4 (note number 69) and use equal temperament to find pitch.
		return 440 * std::pow(2, ((double)note - 69) / 12);
	}

	std::array<double, 128> midi_freqs = []() constexpr {
											 std::array<double, 128> res={0};
											 for (unsigned i=0; i<res.size(); i++) res[i]=midi_to_note_freq(i);
											 return res;
										 }();
}
