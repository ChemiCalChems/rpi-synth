#pragma once

#include <cmath>
#include <array>
#include <algorithm>
#include <string_view>

namespace utils {
	[[nodiscard]] static double midi_to_note_freq(int note) {
		//Calculate difference in semitones to A4 (note number 69) and use equal temperament to find pitch.
		return 440 * std::pow(2, ((double)note - 69) / 12);
	}

	const std::array<double, 128> midi_freqs = []() {
		std::array<double, 128> res={0};
		for (unsigned i=0; i<res.size(); i++) res[i]=midi_to_note_freq(i);
		return res;
	}();
	template<typename T1, typename T2>
	[[nodiscard]] T2 mapToRange(T1 value, T2 rangeMin, T2 rangeMax) {
		T2 center = (rangeMin + rangeMax)/2;
		T2 halfRange = std::abs(rangeMax - center);
		return center + value * halfRange;
	}

	template<typename T, std::size_t N>
	struct Buffer {
		using IteratorType = typename std::array<T,N>::iterator;
	private:
		std::array<T, N> arr;
		IteratorType inputIterator = arr.begin(), outputIterator = arr.begin();
		[[nodiscard]] IteratorType next(IteratorType it) {
			if (++it == arr.end()) it = arr.begin();
			return it;
		}
	public:
		std::size_t size = 0;
		const std::size_t capacity = N;

		[[nodiscard]] inline bool empty() {
			return size == 0;
		}

		[[nodiscard]] inline bool full() {
			return size == N;
		}

		template<typename U>
		void push(U&& u)
		{
			assert(!full());
			*inputIterator = std::forward<U>(u);
			inputIterator = next(inputIterator);
			size++;
		}

		T pop() {
			assert(!empty());
			T result = *outputIterator;
			outputIterator = next(outputIterator);
			size--;
			return result;
		}	

	};

	template<typename T, std::size_t N, typename ... Args, std::size_t ... Is>
	std::array<T, N> emplaceArrayHelper(Args&&... args, std::index_sequence<Is...>)
	{
		return std::array<T, N>{(static_cast<void>(Is), T{std::forward<Args>(args)...})...};
	}


	template<typename T, std::size_t N, typename ... Args>
	std::array<T, N> emplaceArray(Args&&... args)
	{
		return emplaceArrayHelper<T, N, Args...>(std::forward<Args>(args)...,
			std::make_index_sequence<N>());
	}

	template<std::size_t N>
	struct ConstexprString
	{
		char data[N];

		constexpr ConstexprString(const char (& _data)[N+1])
		{
			std::copy_n(_data, N, data);
		}

		constexpr operator std::string_view() const
		{
			return std::string_view(data, data + N);
		}

		template<std::size_t M>
		constexpr auto operator<=>(const ConstexprString<M>& other) const
		{
			return std::string_view{*this} <=> std::string_view{other};
		}

		template<std::size_t M>
		constexpr bool operator==(const ConstexprString<M>& other) const
		{
			return std::string_view{*this} == std::string_view{other};
		}
	};

	template<std::size_t M>
	ConstexprString(const char(&)[M]) -> ConstexprString<M-1>;
}
