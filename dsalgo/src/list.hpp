// dsalgo/src/list.hpp
#pragma once
#include <cstring>
#include <memory>
#include <new>
#include <stdexcept>
#include <type_traits>
#include <utility>

#include "types.hpp"

namespace dsalgo
{

template <class T>
    requires(std::is_trivially_copyable_v<T>)
class List
{
public:
    List() = default;
    explicit List(usize n_elements)
    {
        if (n_elements > 0) allocate_(n_elements);
    }

    List(List &&other) noexcept
        : m_start(other.m_start), m_end(other.m_end), m_capacity(other.m_capacity)
    {
        other.m_start = other.m_end = other.m_capacity = nullptr;
    }

    List &operator=(List &&other) noexcept
    {
        if (this != &other)
        {
            deallocate_(m_start);
            m_start = other.m_start;
            m_end = other.m_end;
            m_capacity = other.m_capacity;
            other.m_start = other.m_end = other.m_capacity = nullptr;
        }
        return *this;
    }

    List(const List &other)
    {
        const usize n = other.get_length();
        const usize cap = other.get_capacity();
        if (cap == 0) return;
        allocate_(cap);
        if (n > 0) std::memcpy(m_start, other.m_start, n * sizeof(T));
        m_end = m_start + n;
    }

    List &operator=(const List &other)
    {
        if (this == &other) return *this;
        const usize n = other.get_length();
        const usize cap = other.get_capacity();

        T *new_start = nullptr;
        if (cap > 0)
        {
            new_start = static_cast<T *>(raw_alloc_(cap * sizeof(T)));
            if (!new_start) throw std::runtime_error("Failed to allocate memory for List.");
            if (n > 0) std::memcpy(new_start, other.m_start, n * sizeof(T));
        }
        deallocate_(m_start);
        m_start = new_start;
        m_end = new_start ? new_start + n : nullptr;
        m_capacity = new_start ? new_start + cap : nullptr;
        return *this;
    }

    ~List() { deallocate_(m_start); }

    template <class... Args>
        requires std::is_constructible_v<T, Args...>
    T &emplace_back(Args &&...args)
    {
        ensure_capacity_for_one_();
        T *slot = m_end;
        std::construct_at(slot, std::forward<Args>(args)...);
        ++m_end;
        return *slot;
    }

    void push_back(T value) { (void)emplace_back(value); }

    void pop(usize idx)
    {
        const usize n = get_length();
        if (idx >= n) throw std::runtime_error("Out of bounds on pop(idx).");

        if (idx < n - 1)
        {
            std::memmove(m_start + idx, m_start + idx + 1, (n - idx - 1) * sizeof(T));
        }
        --m_end;
    }

    [[nodiscard]] T pop_back_return()
    {
        if (is_empty()) throw std::runtime_error("pop_back_return on empty!");
        T out = *(m_end - 1);
        --m_end;
        return out;
    }

    void pop_back()
    {
        if (is_empty()) throw std::runtime_error("pop_back on empty!");
        --m_end;
    }

    void clear() noexcept { m_end = m_start; }

    void reserve(usize new_capacity)
    {
        const usize current_capacity = get_capacity();
        if (new_capacity <= current_capacity) return;

        const usize n = get_length();
        T *new_start = static_cast<T *>(raw_alloc_(new_capacity * sizeof(T)));
        if (!new_start) throw std::runtime_error("Failed to allocate memory in reserve().");
        if (n > 0) std::memcpy(new_start, m_start, n * sizeof(T));

        deallocate_(m_start);
        m_start = new_start;
        m_end = m_start + n;
        m_capacity = m_start + new_capacity;
    }

    [[nodiscard]] usize get_length() const noexcept { return m_start ? static_cast<usize>(m_end - m_start) : 0zu; }
    [[nodiscard]] usize get_capacity() const noexcept { return m_start ? static_cast<usize>(m_capacity - m_start) : 0zu; }
    [[nodiscard]] bool is_empty() const noexcept { return m_end == m_start; }
    [[nodiscard]] bool is_full() const noexcept { return m_end == m_capacity; }
    constexpr T *begin() noexcept { return m_start; }
    constexpr T *end() noexcept { return m_end; }
    constexpr const T *begin() const noexcept { return m_start; }
    constexpr const T *end() const noexcept { return m_end; }

    [[nodiscard]] T &operator[](usize index) noexcept { return m_start[index]; }
    [[nodiscard]] const T &operator[](usize index) const noexcept { return m_start[index]; }
    [[nodiscard]] T &at(usize index)
    {
        if (index >= get_length()) throw std::out_of_range("List::at index out of range");
        return m_start[index];
    }
    [[nodiscard]] const T &at(usize index) const
    {
        if (index >= get_length()) throw std::out_of_range("List::at index out of range");
        return m_start[index];
    }

private:
    T *m_start{};
    T *m_end{};
    T *m_capacity{};

    void *raw_alloc_(usize bytes)
    {
        return ::operator new(bytes, std::align_val_t{alignof(T)});
    }

    void deallocate_(void *ptr) noexcept
    {
        if (!ptr) return;
        ::operator delete(ptr, std::align_val_t{alignof(T)});
    }

    void allocate_(usize n)
    {
        m_start = static_cast<T *>(raw_alloc_(n * sizeof(T)));
        if (!m_start) throw std::runtime_error("Failed to allocate memory for List.");
        m_end = m_start;
        m_capacity = m_start + n;
    }

    void grow_()
    {
        const usize cap = get_capacity();
        usize inc = cap / 2;
        if (inc < 4) inc = 4;
        reserve(cap + inc);
    }

    void ensure_capacity_for_one_()
    {
        if (!m_start)
        {
            allocate_(4);
            return;
        }
        if (is_full()) grow_();
    }
};

} // namespace dsalgo