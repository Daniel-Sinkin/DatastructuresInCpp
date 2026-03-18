// main.cpp
#include <array>
#include <atomic>
#include <cstddef>
#include <cstdint>
#include <optional>

using usize = std::size_t;

using u64 = std::uint64_t;

template <usize N>
concept IsPower2 = (N > 0) and ((N & (N - 1)) == 0);

// [1, 2, 3, 4] fixed size buffer with N = 2^k elements
// We "unwrap Z/NZ forward" <- identify numbers (mod N)
// so for example N = 2^2 = 4, 15 = 3 + 3 * N ===_N 3
// [1, 2, 3, 4] [1, 2, 3, 4]
//  ^            ^
//  +-same value-+

// Objects live in [tail, head) (not inclusive)
// Size = head - tail
// Empty: head - tail = 0 <=> tail == head
// Invariants: tail <= head <= tail + N, 0 <= Size <= N

// 0 Elements
//       head --v
// [1, 2, 3, 4][1, 2, 3, 4]
//              ^-- tail
// 3 Elements:
//       head --v
// [1, 2, 3, 4][1, 2, 3, 4]
//     ^-- tail
// After Pushing value 5
//          head --v
// [5, 2, 3, 4][5, 2, 3, 4]
//     ^-- tail
// Values are [2, 3, 4, 5]
// After poping value (from back)
//          head --v
// [5, 2, 3, 4][5, 2, 3, 4]
//        ^-- tail
// Values are [3, 4, 5]

// SPSC = (S)ingle (P)roducer (S)ingle (C)onsumer
// At most one Producer Thread that does push() <- moves head
// At most one Consumer Thread that does pop()  <- moves tail

template <typename T, usize N>
    requires(IsPower2<N> and std::is_default_constructible_v<T>)
class SPSCRingBuffer {
public:
    auto push(T value) -> bool {
        // This is the only thread that WRITES to head so we don't need any sync with other threads
        const auto h = h_.load(std::memory_order_relaxed);
        // The consumer thread writes to tail so we need to sync with it
        const auto t = t_.load(std::memory_order_acquire);
        if (h - t == N)
            return false; // Full
        set(h, std::move(value));
        h_.store(h + 1, std::memory_order_release);
        return true;
    }
    auto pop() -> std::optional<T> {
        const auto h = h_.load(std::memory_order_acquire);
        const auto t = t_.load(std::memory_order_relaxed);
        if (h == t)
            return std::nullopt; // Empty
        const auto out = std::move(get(t));
        t_.store(t + 1, std::memory_order_release);
        return out;
    }
    // These can fall into race conditions and therefore should be seen as approximations only

    auto size() const noexcept -> usize {
        const auto h = h_.load(std::memory_order_relaxed);
        const auto t = t_.load(std::memory_order_relaxed);
        return static_cast<usize>(h - t);
    }
    auto empty() const noexcept -> bool {
        const auto h = h_.load(std::memory_order_relaxed);
        const auto t = t_.load(std::memory_order_relaxed);
        return (h == t);
    }

private:
    // In principle there is std::hardware_destructive_interference_size for this
    // but this is not really a well defined quantity as the compilation triple
    // (OS, Vendor, ISA) does not tell you the CL size (for example on macos perf cores
    // the cache lines are 128 byte not 64 byte). STDLIB usually just hardcodes it as
    // 64 anyways.
    // [h_ p64 p64 p64 p64 p64 p64 p64][t_ p64 p64 p64 p64 p64 p64 p64] <- 56 Bytes of Padding
    alignas(64) std::atomic<u64> h_{0}; // Both indices take up an entire Cache Line to avoid False Sharing
    alignas(64) std::atomic<u64> t_{0};

    // Takes up N * sizeof(T) bytes, so total size is 2 * 64 + N * sizeof(T) bytes
    std::array<T, N> d_{};
    // More correct alternative which does not require objects to be default constructibel
    // as both std::array and std::vector default construct objects in the N slots
    // We could just allocate raw memory and work on that; this avoid having to default
    // construct garbage objects before we have done the first N insertions
    // alignas(T) std::byte buffer_[N * sizeof(T)];

    auto get(usize i) noexcept -> T & { return d_[i & (N - 1)]; }
    auto get(usize i) const noexcept -> const T & { return d_[i & (N - 1)]; }
    auto set(usize i, T &&value) noexcept -> void { d_[i & (N - 1)] = std::move(value); }
};
