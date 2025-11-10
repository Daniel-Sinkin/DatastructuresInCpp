// dsalgo/src/util.hpp
#pragma once

#include <cstdint>

#include "types.hpp"

namespace dsalgo
{
template <Hashable T>
constexpr uint64_t hash_int(T x) noexcept
{
    uint64_t v = static_cast<uint64_t>(x);
    // SplitMix64 variant
    v += 0x9e3779b97f4a7c15ull;
    v = (v ^ (v >> 30)) * 0xbf58476d1ce4e5b9ull;
    v = (v ^ (v >> 27)) * 0x94d049bb133111ebull;
    v ^= (v >> 31);
    return v;
}
} // namespace dsalgo