// main.cpp

#include <_stdlib.h>
#include <cassert>
#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <exception>
#include <functional>
#include <limits>
#include <print>
#include <random>
#include <stdexcept>
#include <type_traits>

using f32 = float;
using f64 = double;

static_assert(sizeof(f32) == 4);
static_assert(sizeof(f64) == 8);

struct alignas(16) f32x4 {
    f32 x;
    f32 y;
    f32 z;
    f32 w;
};

[[nodiscard]] inline f32x4 add(f32x4 a, f32x4 b) noexcept {
    return f32x4{
        .x = a.x + b.x,
        .y = a.y + b.y,
        .z = a.z + b.z,
        .w = a.w + b.w,
    };
}

[[nodiscard]] inline f32x4 sub(f32x4 a, f32x4 b) noexcept {
    return f32x4{
        .x = a.x - b.x,
        .y = a.y - b.y,
        .z = a.z - b.z,
        .w = a.w - b.w,
    };
}

[[nodiscard]] inline f32x4 mul(f32x4 a, f32x4 b) noexcept {
    return f32x4{
        .x = a.x * b.x,
        .y = a.y * b.y,
        .z = a.z * b.z,
        .w = a.w * b.w,
    };
}

[[nodiscard]] inline f32x4 div(f32x4 a, f32x4 b) noexcept {
    return f32x4{
        .x = a.x / b.x,
        .y = a.y / b.y,
        .z = a.z / b.z,
        .w = a.w / b.w,
    };
}

using u8 = std::uint8_t;
using u16 = std::uint16_t;
using u32 = std::uint32_t;
using u64 = std::uint64_t;

using i8 = std::int8_t;
using i16 = std::int16_t;
using i32 = std::int32_t;
using i64 = std::int64_t;

using Byte = std::byte;

using usize = std::size_t;

constexpr usize initial_n_elems{16};

[[nodiscard]] int random_int() noexcept {
    static thread_local std::mt19937 rng{std::random_device{}()};
    static thread_local std::uniform_int_distribution<int> dist{
        std::numeric_limits<int>::min(),
        std::numeric_limits<int>::max(),
    };
    return dist(rng);
}

usize hash(int x, usize n_buckets) {
    assert(n_buckets > 0);
    return static_cast<usize>(x) % n_buckets;
}

template <class K, class V>
struct HashNode {
    K key;
    V value;
    HashNode *next;
};

template <class K, class V, class Hash = std::hash<K>, class Eq = std::equal_to<K>>
struct HashMap {
    static_assert(std::is_same_v<K, usize>);
    static_assert(std::is_same_v<V, int>);
    using Node = HashNode<K, V>;

    HashMap() {
        auto tmp = std::malloc(initial_n_elems * sizeof(Node *));
        if (!tmp) {
            std::abort();
        }
        buckets_ = static_cast<Node **>(tmp);
    }

    HashMap(usize n_buckets) {
        auto tmp = std::malloc(n_buckets * sizeof(Node *));
        if (!tmp) {
            std::abort();
        }
        buckets_ = static_cast<Node **>(tmp);
    }

    ~HashMap() {
        for (Node *bucket : buckets_) {
            std::free(bucket);
        }
        std::free(buckets_);
    }

    void insert_or_assign(K key, V value) {
        const usize idx = hash(key, bucket_count_);
        Node **link = &buckets_[idx];
        while (*link) {
            Node *n = *link;
            if (eq_(n->key, key)) {
                n->value = value;
                return;
            }
            link = &n->next;
        }
        buckets_[idx] = new Node{.key = key, .value = value, .next = buckets_[idx]};
        ++size_;
    }

    [[no_unique_address]] Hash hash_{};
    [[no_unique_address]] Eq eq_{};

    Node **buckets_{};
    usize size_{};
    usize bucket_count_{32};
};
namespace ansi {
inline constexpr const char *reset = "\x1b[0m";
inline constexpr const char *red = "\x1b[31m";
inline constexpr const char *orange = "\x1b[38;5;208m";
} // namespace ansi

[[maybe_unused]] static void dump_bytes(const std::byte *p, usize n, usize alignment) {
    if (alignment == 0) {
        alignment = 1;
    }

    const auto base = reinterpret_cast<std::uintptr_t>(p);

    for (usize i{0}; i < n; ++i) {
        if ((i & 15u) == 0u) {
            std::print("{:04x}: ", static_cast<unsigned>(i));
        }

        const std::uintptr_t addr = base + i;
        const bool boundary = (addr % alignment) == 0u;

        const char *color = nullptr;
        if (alignment >= 16 && boundary) {
            color = ansi::red;
        } else if (alignment >= 2 && boundary) {
            color = ansi::orange;
        }

        if (color) {
            std::print("{}{:02x}{} ", color, std::to_integer<unsigned>(p[i]), ansi::reset);
        } else {
            std::print("{:02x} ", std::to_integer<unsigned>(p[i]));
        }

        if (((i & 15u) == 15u) || (i + 1u == n)) {
            std::println();
        }
    }
}

using Data = std::array<int, 32>;

struct TrackMemory {
    TrackMemory() {
        std::println("Empty Constructor");
    }
    TrackMemory(const TrackMemory &other) : data_(other.data_) {
        std::println("Copy constructor");
    }

    TrackMemory(TrackMemory &&other) noexcept : data_(std::move(other.data_)) {
        std::println("Move constructor");
    }
    Data data_{};
};

auto no_nrvo(bool return_a) -> TrackMemory {
    TrackMemory a{};
    TrackMemory b{};
    if (return_a) {
        return a;
    }
    return b;
}

auto yes_nrvo() -> TrackMemory {
    TrackMemory a{};
    return a;
}

auto forced_nrvo() -> TrackMemory {
    return TrackMemory{};
}

class RAII {
public:
    RAII(std::string_view name) : name_(name), data_(static_cast<int *>(std::malloc(8 * sizeof(int)))) {
        std::println("Allocated Memory '{}'", name_);
    }
    ~RAII() {
        std::free(data_);
        std::println("Deallocated Memory '{}'", name_);
    }

private:
    std::string name_{};
    int *data_{};
};

auto func() -> void {
    std::println("Starting Function");
    RAII func_scope("Function Scope");
    std::println("Starting inner scope");
    {
        RAII inner_scope("Inner Scope");
    }
    std::println("Finished inner scope");
    std::println("Throwing exception");
    throw std::runtime_error("");

    std::println("Finished function");
}

int main() {
    std::println("Starting Program");
    try {
        func();
    } catch (...) {
        std::println("Caught exception");
    }
    std::println("Finishing Program");
}
