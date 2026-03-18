
/*
Implement a templated Vector<T> with:
- push_back(const T&) and push_back(T&&)
- emplace_back(Args&&...)
- operator[], at(), size(), capacity()
- Destructor, move constructor, move assignment
- Amortized doubling with raw memory management
*/

#include <algorithm>
#include <cassert>
#include <cstddef>
#include <cstdint>

using usize = std::size_t;
using f32 = float;
using f64 = double;

// Vector is dynamic heap allocated memory
// Design:
//+  (1) Holds 3 Pointers on the Stack (start_, end_, cap_)
//+  (2) Starts with 0 capacity (all pointers nullptr)
//+  (3) MAYBE RESIZE
//+      (3.1) Resizing factor is 2.0
//+      (3.2) Initial resize has size 1
//+       (3.3) Only resize when uninitialised or full
//+  (4) PUSHBACK
//+      (4.1) push_back(const &)
//+      (4.2) push_back(T&&)
//+      (4.3) Maybe Resizes
//+      (4.4) Constructs with placement new at end_ location
//+  (5) EMPLACEBACK
//+      (5.1) empalce_back(Args&&...)
//+      (5.2) Does perfect forwarding
//+      (5.3) Constructs with placement new at end_ location
//+  (6) Getters and Operator Overloads

//+ Invariants
//+  (Inv1) Either all or none of the pointers are nullptr
//+  (Inv2) There is always a free slot after maybe_resize
//+      (Inv2.1) If we're not full nothing is done
//+      (Inv2.2) If we're full then we grow at least one slot
//+  (Inv3) Vector takes up exactly 3 pointers of size on the stack

template <typename T>
class Vector {
public:
    using SizeT = usize;
    // (3.2)
    static constexpr SizeT k_initial_size{1};
    // (3.1)
    static constexpr f64 k_growth_factor{2.0};

    Vector() = default;
    Vector(const Vector &other) noexcept(std::is_nothrow_copy_constructible_v<T>) {
        copy_from(other);
    }
    Vector &operator=(const Vector &other) noexcept(std::is_nothrow_copy_constructible_v<T>) {
        if (this != &other) {
            release();
            copy_from(other);
        }
        return *this;
    }
    Vector(Vector &&other) noexcept {
        steal_from(std::move(other));
    }
    Vector &operator=(Vector &&other) noexcept {
        if (this != &other) {
            release();
            steal_from(std::move(other));
        }
        return *this;
    }
    ~Vector() { release(); }

    auto push_back(const T &value) -> void {
        maybe_resize_();
        new (end_) T{value};
        ++end_;
    }
    auto push_back(T &&value) -> void {
        maybe_resize_();
        new (end_) T{std::move(value)};
        ++end_;
    }
    template <typename... Args>
    auto emplace_back(Args &&...args) -> void {
        maybe_resize_();
        new (end_) T{std::forward<Args>(args)...};
        ++end_;
    }

    // (6)
    auto empty() const noexcept -> bool { return start_ == end_; }
    auto size() const noexcept -> usize { return (start_) ? (end_ - start_) : 0; }
    auto capacity() const noexcept -> usize { return (start_) ? (cap_ - start_) : 0; }
    auto operator[](usize i) noexcept -> T & { return start_[i]; }
    auto operator[](usize i) const noexcept -> const T & { return start_[i]; }
    auto at(usize i) -> T & {
        if (i >= size())
            throw std::out_of_range("Vector::at() OOB");
        return start_[i];
    }
    auto at(usize i) const -> const T & {
        if (i >= size())
            throw std::out_of_range("Vector::at() OOB");
        return start_[i];
    }

private:
    // Probably shouldn't be below 1.5, but we force at least one extra slot per resize
    static_assert(k_growth_factor > 1.0);
    // (1)
    T *start_{}; // (2)
    T *end_{};   // (2)
    T *cap_{};   // (2)

    auto release() noexcept -> void {
        if (!start_)
            return;
        for (auto p = start_; p != end_; ++p) {
            p->~T();
        }
        ::operator delete(start_);
        start_ = nullptr;
        end_ = nullptr;
        cap_ = nullptr;
    }

    auto copy_from(const Vector &other) -> void {
        const auto other_cap = other.capacity();
        start_ = static_cast<T *>(::operator new(other_cap * sizeof(T)));
        end_ = start_;
        cap_ = start_ + other_cap;
        for (auto other_ptr = other.start_; other_ptr != other.end_; ++other_ptr, ++end_) {
            new (end_) T{*other_ptr};
        }
    }
    auto steal_from(Vector &&other) noexcept -> void {
        start_ = std::exchange(other.start_, nullptr);
        end_ = std::exchange(other.end_, nullptr);
        cap_ = std::exchange(other.cap_, nullptr);
    }

    auto maybe_resize_() -> void {
        // (3.3), (Inv2.1)
        if (end_ != cap_)
            return;
        if (!start_) { // Initial Resize
            // (Inv1)
            assert(!end_ and !cap_);
            // (3.2)
            start_ = static_cast<T *>(::operator new(sizeof(T) * k_initial_size));
            end_ = start_;
            cap_ = start_ + k_initial_size;
            return;
        }

        const auto old_cap_f64 = static_cast<f64>(capacity());
        // (Inv2.2)
        const auto new_cap = static_cast<usize>(std::max(old_cap_f64 + 1, k_growth_factor * old_cap_f64));
        const auto num_elems = size();
        const auto new_start = static_cast<T *>(::operator new(new_cap * sizeof(T)));
        auto new_ptr = new_start;
        for (auto old_ptr = start_; old_ptr != end_; ++old_ptr, ++new_ptr) {
            new (new_ptr) T{std::move(*old_ptr)};
            old_ptr->~T();
        }
        ::operator delete(start_);
        start_ = new_start;
        end_ = new_start + num_elems;
        cap_ = new_start + new_cap;
    }
};
// (Inv3)
static_assert(sizeof(Vector<int>) == 3 * sizeof(int *))
