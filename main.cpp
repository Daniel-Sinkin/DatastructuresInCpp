// main.cpp
/*
Implement a fixed-capacity SPSCRingBuffer<T, N> with:
- push(T value) -> bool - returns false if full
- pop() -> std::optional<T> - returns empty if empty
- Lock-free using std::atomic with acquire/release ordering
- N must be power of 2
Write a main() with one producer thread and one consumer thread.
*/

//                v-- head
// [ 1, 2, 3, 4][ 1, 2, 3, 4]
//         ^-- tail
// push(7)
//                   v-- head
// [ 7, 2, 3, 4][ 7, 2, 3, 4]
//         ^-- tail
// pop()
//                   v-- head
// [ 7, 2, 3, 4][ 7, 2, 3, 4]
//            ^-- tail
// pop()
//                   v-- head
// [ 7, 2, 3, 4][ 7, 2, 3, 4]
//                ^-- tail
// Empty <=> 0 = Number of elements = head - tail
// pop()
//                   v-- head
// [ 7, 2, 3, 4][ 7, 2, 3, 4]
//                   ^-- tail
// Empty <=> 0 = Number of elements = head - tail
// Full: N = number of elements = head - tail
//                   v-- head
// [ 7, 2, 3, 4][ 7, 2, 3, 4]
//         ^-- tail

#include <algorithm>
#include <array>
#include <atomic>
#include <cstddef>
#include <cstdint>
#include <optional>
#include <stdexcept>
#include <type_traits>
#include <utility>

using usize = std::size_t;
using isize = std::ptrdiff_t;
using uptr = std::uintptr_t;
using iptr = std::intptr_t;

using u8 = std::uint8_t;
using u16 = std::uint16_t;
using u32 = std::uint32_t;
using u64 = std::uint64_t;

using i8 = std::int8_t;
using i16 = std::int16_t;
using i32 = std::int32_t;
using i64 = std::int64_t;

#if defined(__STDCPP_FLOAT32_T__) and defined(__STDCPP_FLOAT64_T__)
using f32 = std::float32_t;
using f64 = std::float64_t;
#else
using f32 = float;
using f64 = double;
#endif

/*
Implement a templated Vector<T> with:
push_back(const T&) and push_back(T&&)
emplace_back(Args&&...)
operator[], at(), size(), capacity()
Destructor, move constructor, move assignment
Amortized doubling with ::operator new, placement new, manual destroy, ::operator delete
*/

template <typename T>
class Vector {
public:
    Vector() = default;
    ~Vector() {
        reset_();
    }

    Vector(Vector &&other) noexcept
        : start_(std::exchange(other.start_, nullptr)),
          end_(std::exchange(other.end_, nullptr)),
          cap_(std::exchange(other.cap_, nullptr)) {}
    Vector &operator=(Vector &&other) noexcept {
        if (this != &other) {
            reset_();
            start_ = std::exchange(other.start_, nullptr);
            end_ = std::exchange(other.end_, nullptr);
            cap_ = std::exchange(other.cap_, nullptr);
        }
        return *this;
    }
    Vector(const Vector &) = delete;
    Vector &operator=(const Vector &) = delete;

    auto push_back(const T &x) -> void {
        if (end_ == cap_) {
            resize_();
        }
        new (end_) T(x);
        ++end_;
    }
    auto push_back(T &&x) -> void {
        if (end_ == cap_) {
            resize_();
        }
        new (end_) T(std::move(x));
        ++end_;
    }
    template <typename... Ts>
    auto emplace_back(Ts &&...ts) -> void {
        if (end_ == cap_) {
            resize_();
        }
        new (end_) T(std::forward<Ts>(ts)...);
        ++end_;
    }

    [[nodiscard]] auto operator[](usize i) -> T & { return start_[i]; }
    [[nodiscard]] auto operator[](usize i) const -> const T & { return start_[i]; }
    [[nodiscard]] auto at(usize i) -> T & {
        if (i >= size()) {
            throw std::out_of_range("Vector::at() OOB");
        }
        return start_[i];
    }
    [[nodiscard]] auto at(usize i) const -> const T & {
        if (i >= size()) {
            throw std::out_of_range("Vector::at() OOB");
        }
        return start_[i];
    }
    [[nodiscard]] auto size() const noexcept -> usize { return end_ - start_; }
    [[nodiscard]] auto capacity() const noexcept -> usize { return cap_ - start_; }

private:
    static constexpr usize k_initial_cap{1};
    T *start_{};
    T *end_{};
    T *cap_{};

    auto resize_() -> void {
        if (!start_) {
            start_ = static_cast<T *>(::operator new(sizeof(T) * k_initial_cap));
            end_ = start_;
            cap_ = start_ + k_initial_cap;
            return;
        }
        const auto s = size();
        const auto c = 2zu * capacity();

        const auto new_start = static_cast<T *>(::operator new(sizeof(T) * c));
        auto nptr = new_start;
        auto optr = start_;
        while (optr != end_) {
            new (nptr) T{std::move(*optr)};
            optr->~T();
            ++nptr;
            ++optr;
        }
        ::operator delete(start_);
        start_ = new_start;
        end_ = new_start + s;
        cap_ = new_start + c;
    }

    auto reset_() -> void {
        if (!start_) {
            return;
        }
        for (auto p = start_; p != end_; ++p) {
            p->~T();
        }
        ::operator delete(start_);
    }
};
static_assert(sizeof(Vector<int>) == 3 * sizeof(int *));
