// main.cpp

/*
Implement a templated Vector<T> with:

Default constructor, destructor
push_back(const T&) and push_back(T&&)
emplace_back(Args&&...)
x operator[]
x size(), capacity()
Move constructor and move assignment
Amortized doubling with placement new and manual destroy
No std::vector usage
*/

//           v-- cap
//        v-- end
//  v-- start
// [1, 2, *, *] -push_back(3)-> [1, 2, 3, *]
//              -push_back(4)-> [1, 2, 3, 4]
//              -push_back(5)-> [1, 2, 3, 4, 5, *, *, *]
// [1, 2, *, *] -shrink_to_fit()-> [1, 2]

// size(vs) == end - start
// capacity(vs) == cap - start
// empty(vs) == (start == end)

#include <algorithm>
#include <cstddef>
#include <stdexcept>
#include <type_traits>
#include <utility>

using usize = std::size_t;

template <typename T>
    requires(std::is_nothrow_move_constructible_v<T>)
class Vector {
public:
    using SizeT = usize;

    Vector() = default;
    ~Vector() {
        for (auto p = start_; p != end_; ++p) {
            p->~T();
        }
        ::operator delete(start_);
    }

    Vector(const Vector &) = delete;
    Vector &operator=(const Vector &) = delete;
    Vector(Vector &&other) noexcept
        : start_(std::exchange(other.start_, nullptr)),
          end_(std::exchange(other.end_, nullptr)),
          cap_(std::exchange(other.cap_, nullptr)) {}
    Vector &operator=(Vector &&other) noexcept {
        if (this != &other) {
            start_ = std::exchange(other.start_, nullptr);
            end_ = std::exchange(other.end_, nullptr);
            cap_ = std::exchange(other.cap_, nullptr);
        }
        return *this;
    }

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
    auto emplace_back(Ts &&...ts) {
        if (end_ == cap_) {
            resize_();
        }
        new (end_) T(std::forward<Ts>(ts)...);
        ++end_;
    }

    [[nodiscard]] auto operator[](SizeT idx) const noexcept -> const T & {
        return start_[idx];
    }
    [[nodiscard]] auto operator[](SizeT idx) noexcept -> T & {
        return start_[idx];
    }

    [[nodiscard]] auto at(SizeT idx) -> T & {
        if (idx >= size()) {
            throw std::runtime_error("Vector::operator[] OOB!");
        }
        if (idx >= size()) {
            throw std::out_of_range("Vector::at() OOB");
        }
        return start_[idx];
    }
    [[nodiscard]] auto at(SizeT idx) const -> const T & {
        if (idx >= size()) {
            throw std::out_of_range("Vector::at() OOB");
        }
        return start_[idx];
    }

    [[nodiscard]] auto size() const noexcept -> SizeT { return static_cast<SizeT>(end_ - start_); }
    [[nodiscard]] auto capacity() const noexcept -> SizeT { return static_cast<SizeT>(cap_ - start_); }
    [[nodiscard]] auto empty() const noexcept -> bool { return start_ == end_; }

private:
    static constexpr SizeT k_initial_capacity{1};

    T *start_{};
    T *end_{};
    T *cap_{};

    auto resize_() -> void {
        if (!start_) {
            start_ = static_cast<T *>(::operator new(sizeof(T) * k_initial_capacity));
            end_ = start_;
            cap_ = start_ + k_initial_capacity;
            return;
        }
        const auto old_size = size();
        const auto old_cap = capacity();
        const auto new_cap = SizeT{2} * old_cap;

        const auto new_start_ptr = static_cast<T *>(::operator new(sizeof(T) * new_cap));
        { // Move over all existing elements using move constructor
            auto new_ptr = new_start_ptr;
            auto old_ptr = start_;
            while (old_ptr != end_) {
                new (new_ptr) T(std::move(*old_ptr));
                old_ptr->~T();
                ++new_ptr;
                ++old_ptr;
            }
        }
        ::operator delete(start_);
        start_ = new_start_ptr;
        end_ = start_ + old_size;
        cap_ = start_ + new_cap;
    }
};
static_assert(sizeof(Vector<int>) == 3 * sizeof(int *));
