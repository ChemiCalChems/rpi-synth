#pragma once

#include <cmath>
#include <array>
#include <algorithm>
#include <string_view>
#include <ranges>

namespace utils
{

[[nodiscard]] static constexpr double midi_to_note_freq(int note) {
    //Calculate difference in semitones to A4 (note number 69) and use equal temperament to find pitch.
    return 440 * std::pow(2, ((double)note - 69) / 12);
}

static constexpr auto midi_freqs = []() {
    std::array<double, 128> res={0};
    for (unsigned i=0; i<res.size(); i++) res[i]=midi_to_note_freq(i);
    return res;
}();

template<typename T1, typename T2>
[[nodiscard]] constexpr T2 mapToRange(T1 value, T2 outputRangeMin, T2 outputRangeMax, T1 inputRangeMin = -1., T1 inputRangeMax = 1.)
{
    return outputRangeMin + (std::clamp(value, inputRangeMin, inputRangeMax) - inputRangeMin) / (inputRangeMax - inputRangeMin) * (outputRangeMax - outputRangeMin);
}

template<std::input_iterator Iterator>
struct RolloverIterator
{
    using difference_type = typename std::iterator_traits<Iterator>::difference_type;
    using value_type = typename std::iterator_traits<Iterator>::value_type;
    using pointer = typename std::iterator_traits<Iterator>::pointer;
    using reference = typename std::iterator_traits<Iterator>::reference;
    using iterator_category = std::bidirectional_iterator_tag;

private:
    Iterator rangeBegin;
    Iterator rangeEnd;
    Iterator current;

public:
    constexpr RolloverIterator() = default;
    constexpr RolloverIterator(const Iterator& rangeBegin_, const Iterator& rangeEnd_)
    : rangeBegin{rangeBegin_}, rangeEnd{rangeEnd_}, current{rangeBegin_}
    {
    }

    constexpr RolloverIterator& operator++()
    {
        ++current;
        if (current == rangeEnd) current = rangeBegin;

        return *this;
    }

    constexpr RolloverIterator operator++(int)
    {
        auto result = *this;
        ++*this;

        return result;
    }

    constexpr RolloverIterator& operator--() requires std::bidirectional_iterator<Iterator>
    {
        if (current == rangeBegin) current = std::prev(rangeEnd);
        else --current;

        return *this;
    }

    constexpr RolloverIterator operator--(int) requires std::bidirectional_iterator<Iterator>
    {
        auto result = *this;
        --*this;

        return result;
    }

    [[nodiscard]] constexpr auto& operator*() &
    {
        return *current;
    }

    [[nodiscard]] constexpr auto operator*() &&
    {
        return *current;
    }

    [[nodiscard]] constexpr auto operator->()
    {
        return &(*current);
    }
};

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

template<typename T, std::size_t N>
struct Last
{
    using value_type = T;
    static constexpr std::size_t size = N;
private:
    std::array<value_type, size> data{};
public:
    using IteratorType = RolloverIterator<typename decltype(data)::iterator>;

    IteratorType writeIt{data.begin(), data.end()};
public:
    constexpr Last() = default;
    constexpr Last(std::initializer_list<T> list)
    {
        if (list.size() > size) throw;

        std::copy_n(std::begin(list), list.size(), data.begin());
        std::advance(writeIt, list.size());
    }

    template<typename U>
    constexpr void push(U&& u)
    {
        if constexpr (size == 0) return;

        *writeIt = std::forward<U>(u);
        writeIt++;
    }

    [[nodiscard]] constexpr T operator[](int i)
    {
        return *std::prev(writeIt, -i + 1);
    }

    [[nodiscard]] constexpr auto begin() const
    {
        return std::begin(std::views::counted(std::reverse_iterator(writeIt), size));
    }

    [[nodiscard]] constexpr auto end() const
    {
        return std::end(std::views::counted(std::reverse_iterator(writeIt), size));
    }
};

}
