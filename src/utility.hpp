#ifndef UTILITY_H
#define UTILITY_H

#include <numeric>
#include <random>


std::random_device rd;
std::mt19937 rng_gen { rd() };
std::uniform_int_distribution<std::uint8_t> two_five_six_rng { 0, 255 };


template <typename Container>
inline typename Container::value_type fold_plus(Container c)
{
    return std::accumulate(std::begin(c), std::end(c), 0);
}

template <typename Container>
inline typename Container::value_type fold_plus_and(Container c, typename Container::value_type v)
{
    return std::accumulate(std::begin(c), std::end(c), 0, [&](typename Container::value_type a, typename Container::value_type b) {
        return a + (b & v);
    });
}

#endif