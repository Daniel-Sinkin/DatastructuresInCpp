// main.cpp

/*
Implement a templated ThreadSafeQueue<T> with:
- push(T value) - adds element to back
- pop() -> T - blocks until element available, returns front element
- try_pop() -> std::optional<T> - returns immediately, empty optional if queue is empty
- empty() -> bool
- size() -> std::size_t
Use std::mutex and std::condition_variable. Write a main() that tests it.
*/

#include <condition_variable> // Condition variable, notify_one(), wait()
#include <mutex>              // Mutual Exclusion, lock(), unlock()
#include <optional>           // std::nullopt
#include <queue>              // front(), pop(), empty(), size();
#include <utility>            // std::move(...) === static_cast<std::remove_reference<T>&&>(...)

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
        std::scoped_lock sl{m_};
        q_.push(std::move(value)); // If we copied value don't re-copy
        cv_.notify_one();
    }

    auto pop() -> T {
        // Can lock and relock, if locked on scope leave it unlocks -> more expensive than scoped_lock
        std::unique_lock _ul{m_};
        cv_.wait(_ul, [this] { return !q_.empty(); });
        return pop_();
    }

    auto try_pop() -> std::optional<T> {
        std::scoped_lock _sl{m_}; // RAII wrapper, lock on constructor, unlock on destructor
        if (q_.empty()) {
            return std::nullopt; // [*garbage*, false]
        }
        return pop_();
    }
    [[nodiscard]] auto empty() const -> bool {
        std::scoped_lock _sl{m_};
        return q_.empty();
    }
    [[nodiscard]] auto size() const -> usize {
        std::scoped_lock _sl{m_};
        return q_.size();
    }

private:
    mutable std::condition_variable cv_{}; // Condition variable, blocks and allows notifying blocked threads
    mutable std::mutex m_{};               // mutable, as this does not affect the classes' invariants
    std::queue<T> q_{};                    // FIFO [] -> [1] -> [1, 2] -> [1, 2, 3] -> [2, 3]

    auto pop_() -> T {
        auto out = std::move(q_.front()); // static_cast<std::remove_reference_t<T>&&>(q_);
        q_.pop();                         // Removes from front as Queues have FIFO semantics
        return out;                       // [q_.front(), true]
    }
};
