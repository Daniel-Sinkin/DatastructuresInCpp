// main.cpp

#include <condition_variable>
#include <cstddef>
#include <cstdint>
#include <mutex>
#include <optional>
#include <queue>
#include <type_traits>

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

template <typename T>
class ThreadSafeQueue {
public:
    auto push(T value) -> void {
        std::scoped_lock _sl{m_};
        q_.push(std::move(value));
        // If there are any threads waiting on cv_ wake up one to have it check the !q_.empty() condition
        cv_.notify_one();
    }
    auto pop() noexcept(std::is_nothrow_move_constructible_v<T>) -> T {
        // unique_lock can be locked and unlocked, locks on creation
        std::unique_lock _ul{m_};
        // Check the condition once, if it's true we acquire the lock (of not already holding it)
        // If wrong we release the lock and block here until notified to re-check the condition
        cv_.wait(_ul, [this] { return !q_.empty(); });
        return pop_();
    }
    auto try_pop() noexcept(std::is_nothrow_move_constructible_v<T>) -> std::optional<T> {
        std::scoped_lock _sl{m_};
        // Optional models, similiar to T* and nullptr the existance or
        // absence of a value via the std::nullopt primitive.
        if (q_.empty())
            return std::nullopt;
        return pop_();
    }
    [[nodiscard]] auto empty() const noexcept -> bool {
        // RAII = (R)esource (A)cquisition (I)s (I)nitialisation
        // Personally I'd prefer the Acronym CADR = (C)onstructor (A)cquires (D)estructor (R)eleases
        // std::lock_guard<std::mutex> _lg{m_}; // C++14, there was no CTAD = (C)lass (T)template (A)rgument (D)eduction at that point
        std::scoped_lock _sl{m_}; // C++17, lock() on Constructor, unlock() on destruction
        return q_.empty();
    }
    [[nodiscard]] auto size() const noexcept -> usize {
        std::scoped_lock _sl{m_};
        return q_.size();
    }

private:
    mutable std::condition_variable cv_{};
    mutable std::mutex m_{}; // MutEx = (Mut)ual (Ex)clusion, creates critical sectiosn to avoid race conditions, lock(), unlock()
    std::queue<T> q_{};      // FIFO semantic, Implemented on top of a std::deque
    auto pop_() noexcept(std::is_nothrow_move_constructible_v<T>) -> T {
        const auto out = std::move(q_.front());
        q_.pop();
        return out;
    }
};

int main() {
}
