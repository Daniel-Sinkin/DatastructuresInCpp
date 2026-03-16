// main.cpp
/*
Implement a templated SharedPtr<T> with:

x Control block containing std::atomic<int> strong reference count
x Constructor from raw pointer SharedPtr(T* ptr)
x Destructor (decrements count, deletes when zero)
x Copy constructor and copy assignment (increment count)
x Move constructor and move assignment (transfer ownership, no count change)
x operator*, operator->, get()
x use_count() -> int
x Default constructor (null state)
x reset() to release ownership

Don't implement WeakPtr or custom deleters. Focus on getting the reference counting and copy/move semantics right.
*/

#include <atomic>
#include <memory>

template <typename T, typename Deleter = std::default_delete<T>>
class SharedPtr {
public:
    SharedPtr() : ptr_(nullptr), ctrl_(nullptr) {}
    SharedPtr(T *ptr) : ptr_(ptr), ctrl_(new ControlBlock{1, 0, Deleter{}}) {}
    ~SharedPtr() { reset(); }
    SharedPtr(const SharedPtr &other) {
        copy_from_(other);
    }
    SharedPtr &operator=(const SharedPtr &other) {
        if (this != &other) {
            copy_from_(other);
        }
        return *this;
    }
    SharedPtr(SharedPtr &&other) { steal_from_(std::move(other)); }
    SharedPtr &operator=(SharedPtr &&other) {
        if (this != &other) {
            steal_from_(std::move(other));
        }
        return *this;
    }

    [[nodiscard]] auto get() const noexcept -> const T * { return ptr_; }
    [[nodiscard]] auto get() noexcept -> T * { return ptr_; }

    [[nodiscard]] auto operator*() const -> const T & { return *ptr_; }
    [[nodiscard]] auto operator*() -> T & { return *ptr_; }

    [[nodiscard]] auto operator->() const -> const T * { return ptr_; }
    [[nodiscard]] auto operator->() -> T * { return ptr_; }

    [[nodiscard]] auto use_count() const noexcept -> int { return (ctrl_) ? ctrl_->strong_ref : 0; }

    auto reset() -> void {
        if (!ptr_) {
            return;
        }
        if (--ctrl_->strong_ref == 0) {
            ctrl_->deleter(ptr_);
            delete ctrl_;
        }
        ptr_ = nullptr;
        ctrl_ = nullptr;
    }

private:
    struct ControlBlock {
        std::atomic<int> strong_ref;
        std::atomic<int> weak_ref; // Out of Scope
        Deleter deleter;
    };
    ControlBlock *ctrl_{};
    T *ptr_{};

    auto copy_from_(const SharedPtr &other) -> void {
        reset();
        ptr_ = other.ptr_;
        ++other.ctrl_->strong_ref;
        ctrl_ = other.ctrl_;
    }
    auto steal_from_(SharedPtr &&other) -> void {
        reset();
        ctrl_ = std::exchange(other.ctrl_, nullptr);
        ptr_ = std::exchange(other.ptr_, nullptr);
    }
};
