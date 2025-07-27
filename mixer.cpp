#include "mixer.hpp"
#include "midi.hpp"
#include <circle/synchronize.h>
#include <numeric>
#include <string>
#include <circle/logger.h>
#include <algorithm>
#include <ranges>
#include <circle/logger.h>
#include <format>
#include "config.hpp"

Mixer::Mixer(unsigned int samplerate_)
	: CPWMSoundBaseDevice(CInterruptSystem::Get(), samplerate_, 384), samplerate{samplerate_},
	voices{utils::emplaceArray<std::pair<Voice, bool>, Config::NUM_VOICES>(std::make_pair(samplerate, false))}
{
}

double Mixer::requestSample() {
	auto assignedVoicesRange{voices | std::ranges::views::filter([](const auto& v){return v.second;})};
	return std::accumulate(assignedVoicesRange.begin(), assignedVoicesRange.end(), 0.0, [](const double result, auto& voice) {return result + 0.05 * voice.first.getSample();});
}

void Mixer::fillBuffer() {
	if (!buffer.full()) {
		u32 sample = double(GetRangeMin() + GetRangeMax()) / 2. + requestSample() * (double(GetRangeMax()) - double(GetRangeMin()));

		buffer.push(std::make_pair(sample, sample));
	}
}

Voice* Mixer::requestVoice()
{
	for (auto& voice : voices)
	{
		bool& assigned = voice.second;
		if (!assigned)
		{
			assigned = true;
			return &voice.first;
		}
	}
	return nullptr;
}

void Mixer::returnVoice(Voice* v)
{
	for (auto& voice : voices)
	{
		if (v == &voice.first)
		{
			voice.second = false;
			return;
		}
	}
}

unsigned Mixer::GetChunk (u32* buf, unsigned chunk_size) {
	sampleCountBeforeCallback = buffer.size;
	debugReady = true;

	for (unsigned i = 0; i<chunk_size; i+=2) {
		std::pair<u32, u32> sample;
		if (buffer.empty()) [[unlikely]] {
			sample = {(GetRangeMin() + GetRangeMax())/2, (GetRangeMin() + GetRangeMax())/2};
		} else [[likely]] {
			sample = buffer.pop();
		}
		
		*buf++ = sample.first;
		*buf++ = sample.second;
	}
	//assert(buffer.empty());
	return chunk_size;
}

