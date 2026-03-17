// main.cpp
/*
Implement a templated Vector<T> with:
- push_back(const T&) and push_back(T&&)
- emplace_back(Args&&...)
- operator[], at(), size(), capacity()
- Destructor, move constructor, move assignment
- Amortized doubling with ::operator new, placement new, manual destroy, ::operator delete
*/

#include <algorithm>
#include <cassert>
#include <cstddef>
#include <cstdint>
#include <stdexcept>
#include <type_traits>
#include <utility>

// (start, end, cap) = (0, 0, 0)
// []
// push_back(1)
// -resize_()-> [*] -push_back_(1)-> [1]
// push_back(2)
// -> [1, *] -> [1, 2]
// push_back(3)
// -resize_()-> [1, 2, *, *] -push_back_(3)-> [1, 2, 3, *]

//                v-- end_
// *  * 1 2 3 4 5 * * * * *
//      ^--head         ^
//                      +-- cap_

using usize = std::size_t;
using u64 = std::uint64_t;

template <typename T>
    requires(std::is_nothrow_move_constructible_v<T> and std::is_nothrow_copy_constructible_v<T>)
class Vector {
public:
    using SizeT = usize;
    static constexpr SizeT k_initial_capacity{1};

    Vector() = default;
    ~Vector() { release(); }
    Vector(Vector &&other) noexcept
        : start_(std::exchange(other.start_, nullptr)),
          end_(std::exchange(other.end_, nullptr)),
          cap_(std::exchange(other.cap_, nullptr)) {}
    Vector &operator=(Vector &&other) noexcept {
        if (this == &other) {
            return *this;
        }
        release();
        start_ = std::exchange(other.start_, nullptr);
        end_ = std::exchange(other.end_, nullptr);
        cap_ = std::exchange(other.cap_, nullptr);
        return *this;
    }
    Vector(const Vector &other) {
        if (!other.start_) {
            // Other vector doesn't hold anything so we stick to default nullptr's
            return;
        }
        const auto other_capacity = other.capacity();
        const auto new_start = static_cast<T *>(::operator new(sizeof(T) * other_capacity));
        auto new_ptr = new_start;
        auto old_ptr = other.start_;
        for (; old_ptr != other.end_; ++new_ptr, ++old_ptr) {
            new (new_ptr) T{*old_ptr}; // Copy over elements
        }
        start_ = new_start;
        end_ = new_start + other.size();
        cap_ = new_start + other_capacity;
    }
    Vector &operator=(const Vector &other) {
        if (this == &other) {
            return *this;
        }
        release();
        if (!other.start_) {
            start_ = nullptr;
            end_ = nullptr;
            cap_ = nullptr;
        } else {
            const auto other_capacity = other.capacity();
            const auto new_start = static_cast<T *>(::operator new(sizeof(T) * other_capacity));
            auto new_ptr = new_start;
            auto old_ptr = other.start_;
            for (; old_ptr != other.end_; ++new_ptr, ++old_ptr) {
                new (new_ptr) T{*old_ptr}; // Copy over elements
            }
            start_ = new_start;
            end_ = new_start + other.size();
            cap_ = new_start + other_capacity;
        }
        return *this;
    }

    auto push_back(const T &x) -> void {
        maybe_resize_();
        new (end_) T{x};
        ++end_;
    }
    auto push_back(T &&x) -> void {
        maybe_resize_();
        new (end_) T{std::move(x)};
        ++end_;
    }
    template <typename... Args>
    auto emplace_back(Args &&...args) -> void {
        maybe_resize_();
        new (end_) T{std::forward<Args>(args)...};
        ++end_;
    }

    [[nodiscard]] auto size() const noexcept -> SizeT { return (start_) ? end_ - start_ : SizeT{0}; }
    [[nodiscard]] auto capacity() const noexcept -> SizeT { return (start_) ? cap_ - start_ : SizeT{0}; }
    [[nodiscard]] auto empty() const noexcept -> bool { return end_ == start_; }

    auto at(SizeT i) -> T & {
        if (i >= size()) {
            throw std::out_of_range("Vector::at() OOB");
        }
        return start_[i];
    }
    auto at(SizeT i) const -> const T & {
        if (i >= size()) {
            throw std::out_of_range("Vector::at() OOB");
        }
        return start_[i];
    }
    auto operator[](SizeT i) -> T & { return start_[i]; }
    auto operator[](SizeT i) const -> const T & { return start_[i]; }

private:
    T *start_{}; // Points to the first SLOT
    T *end_{};   // Points to ONE AFTER the last ELEMENT
    T *cap_{};   // Points to ONE AFTER the last allocated SLOT

    auto release() -> void {
        if (!start_) {
            assert(!end_ and !cap_);
            return;
        }
        for (auto p = start_; p != end_; ++p) {
            p->~T();
        }
        ::operator delete(start_);
    }

    auto maybe_resize_() -> void {
        if (end_ != cap_) {
            return;
        }
        if (!start_) {
            assert(!end_ and !cap_);
            start_ = static_cast<T *>(::operator new(sizeof(T) * k_initial_capacity));
            end_ = start_;
            cap_ = start_ + k_initial_capacity;
            //    end_ --v
            //          [*] * * * *
            //  start_ --^  ^-- cap_
            return;
        }
        const auto new_capacity = 2 * capacity();
        const auto num_elems = size();
        const auto new_start = static_cast<T *>(::operator new(sizeof(T) * new_capacity));
        auto new_ptr = new_start;
        for (auto old_ptr = start_; old_ptr != end_; ++old_ptr, ++new_ptr) {
            new (new_ptr) T{std::move(*old_ptr)};
            old_ptr->~T();
        }
        ::operator delete(start_);
        start_ = new_start;
        end_ = new_start + num_elems;
        cap_ = new_start + new_capacity;
    }
};
static_assert(sizeof(Vector<int>) == 3 * sizeof(int *));
