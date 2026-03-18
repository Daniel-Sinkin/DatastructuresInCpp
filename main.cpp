/*
Implement a templated SharedPtr<T> with:
- Control block with atomic strong count
- Constructor from raw pointer, default constructor
- Destructor, copy constructor/assignment, move constructor/assignment
- operator*, operator->, get()
- use_count(), reset()
*/

#include <atomic>
#include <cstdint>
#include <memory>

using u32 = std::uint32_t;

template <typename Deleter, typename T>
concept IsDeleter = requires(Deleter d, T *t) {
    { d(t) } noexcept -> std::same_as<void>; // Deleter must not throw
};

template <typename T, IsDeleter<T> Deleter = std::default_delete<T>>
class SharedPtr {
public:
    SharedPtr() = default;
    SharedPtr(T *ptr) : ptr_(ptr), cb_(new ControlBlock{1, 0, Deleter{}}) {}
    ~SharedPtr() { release(); }
    SharedPtr(const SharedPtr &other) noexcept {
        copy_from(other);
    }
    SharedPtr &operator=(const SharedPtr &other) noexcept {
        if (this != &other) {
            release();
            copy_from(other);
        }
        return *this;
    }
    SharedPtr(SharedPtr &&other) noexcept
        : ptr_(std::exchange(other.ptr_, nullptr)), cb_(std::exchange(other.cb_, nullptr)) {}
    SharedPtr &operator=(SharedPtr &&other) noexcept {
        if (this != &other) {
            release();
            ptr_ = std::exchange(other.ptr_, nullptr);
            cb_ = std::exchange(other.cb_, nullptr);
        }
        return *this;
    }

    auto get() noexcept -> T * { return ptr_; }
    auto get() const noexcept -> const T * { return ptr_; }
    auto operator->() noexcept -> T * { return ptr_; }
    auto operator->() const noexcept -> const T * { return ptr_; }
    auto operator*() -> T & { return *ptr_; }
    auto operator*() const -> const T & { return *ptr_; }

    auto use_count() const noexcept -> u32 {
        return (cb_) ? cb_->strong_ref.load(std::memory_order_relaxed) : 0;
    }
    auto release() noexcept -> void {
        if (!cb_)
            return;
        if (cb_->strong_ref.fetch_sub(1, std::memory_order_acq_rel) == 1) {
            cb_->deleter(ptr_);
            delete (cb_);
        }
        cb_ = nullptr;
        ptr_ = nullptr;
    }

private:
    struct ControlBlock {
        std::atomic<u32> strong_ref;
        std::atomic<u32> weak_ref;
        [[no_unique_address]] Deleter deleter; // EBO Optimisation
    };
    T *ptr_{};
    ControlBlock *cb_{};

    auto copy_from(const SharedPtr &other) noexcept -> void {
        if (!other.cb_)
            return;
        cb_ = other.cb_;
        cb_->strong_ref.fetch_add(1, std::memory_order_acq_rel);
        ptr_ = other.ptr_;
    }
};
