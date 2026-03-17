// main.cpp
/*
Implement a fixed-capacity SPSCRingBuffer<T, N> with:
- push(T value) -> bool - returns false if full
- pop() -> std::optional<T> - returns empty if empty
- Lock-free using std::atomic with acquire/release ordering
- N must be power of 2
*/

#include <array>
#include <atomic>
#include <cstddef>
#include <cstdint>
#include <optional>

using usize = std::size_t;

using u32 = std::uint32_t;
using u64 = std::uint64_t;

template <usize N>
concept IsPower2 = (N > 0 and (N & (N - 1)) == 0);

template <typename T, usize N>
    requires(IsPower2<N>)
class SPSCRingBuffer {
public:
    auto push(T value) -> bool {
        const auto h = h_.load(std::memory_order_relaxed);
        const auto t = t_.load(std::memory_order_acquire);
        if (h - t == N) { // full
            return false;
        }
        d_[h & (N - 1)] = std::move(value);
        h_.store(h + 1, std::memory_order_release);
        return true;
    }
    auto pop() -> std::optional<T> {
        const auto h = h_.load(std::memory_order_acquire);
        const auto t = t_.load(std::memory_order_relaxed);
        if (h == t) { // empty
            return std::nullopt;
        }
        auto out = std::move(d_[t & (N - 1)]);
        t_.store(t + 1, std::memory_order_release);
        return out;
    }

private:
    alignas(64) std::atomic<u64> h_{};
    alignas(64) std::atomic<u64> t_{};
    std::array<T, N> d_{};
};
