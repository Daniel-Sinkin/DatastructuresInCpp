// dsalgo/src/util.hpp
#pragma once

#include "types.hpp"

namespace dsalgo
{
constexpr bool is_power_of_two(usize n) { return (n & (n - 1)) == 0; }

template <Hashable T>
constexpr u64 hash_int(T x) noexcept
{
    u64 v = static_cast<u64>(x);
    // SplitMix64 variant
    v += 0x9e3779b97f4a7c15ull;
    v = (v ^ (v >> 30)) * 0xbf58476d1ce4e5b9ull;
    v = (v ^ (v >> 27)) * 0x94d049bb133111ebull;
    v ^= (v >> 31);
    return v;
}
} // namespace dsalgo