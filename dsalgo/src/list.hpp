// dsalgo/src/list.hpp
#pragma once
#include <cstddef>
#include <cstring>
#include <memory>
#include <new>
#include <stdexcept>
#include <type_traits>
#include <utility>

namespace dsalgo
{

template <class T>
// requires(std::is_trivially_copyable_v<T> || std::is_trivially_move_constructible_v<T>)
class List
{
public:
    explicit List(std::size_t n_elements)
    {
        if (n_elements == 0) return;
        allocate_(n_elements);
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
        const std::size_t n = other.get_length();
        const std::size_t cap = other.get_capacity();
        if (cap == 0) return;
        allocate_(cap);
        if (n > 0) std::memcpy(m_start, other.m_start, n * sizeof(T));
        m_end = m_start + n;
    }

    List &operator=(const List &other)
    {
        if (this == &other) return *this;
        const std::size_t n = other.get_length();
        const std::size_t cap = other.get_capacity();

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

    [[nodiscard]] T pop_back_return()
    {
        if (is_empty()) throw std::runtime_error("pop_back on empty!");
        --m_end;
        return *m_end;
    }

    void pop_back()
    {
        if (is_empty()) throw std::runtime_error("pop_back on empty!");
        --m_end;
    }

    void clear() noexcept { m_end = m_start; }

    void reserve(std::size_t new_capacity)
    {
        const std::size_t current_capacity = get_capacity();
        if (new_capacity <= current_capacity) return;

        const std::size_t n = get_length();
        T *new_start = static_cast<T *>(raw_alloc_(new_capacity * sizeof(T)));
        if (!new_start) throw std::runtime_error("Failed to allocate memory in reserve().");
        if (n > 0) std::memcpy(new_start, m_start, n * sizeof(T));

        deallocate_(m_start);
        m_start = new_start;
        m_end = m_start + n;
        m_capacity = m_start + new_capacity;
    }

    [[nodiscard]] std::size_t get_length() const noexcept { return m_start ? static_cast<std::size_t>(m_end - m_start) : 0zu; }
    [[nodiscard]] std::size_t get_capacity() const noexcept { return m_start ? static_cast<std::size_t>(m_capacity - m_start) : 0zu; }
    [[nodiscard]] bool is_empty() const noexcept { return m_end == m_start; }
    [[nodiscard]] bool is_full() const noexcept { return m_end == m_capacity; }
    T *get_start() noexcept { return m_start; }
    T *get_end() noexcept { return m_end; }
    const T *get_start() const noexcept { return m_start; }
    const T *get_end() const noexcept { return m_end; }

    [[nodiscard]] T &operator[](std::size_t index) noexcept { return m_start[index]; }
    [[nodiscard]] const T &operator[](std::size_t index) const noexcept { return m_start[index]; }

    [[nodiscard]] T &at(std::size_t index)
    {
        if (index >= get_length())
            throw std::out_of_range("List::at index out of range");
        return m_start[index];
    }

    [[nodiscard]] const T &at(std::size_t index) const
    {
        if (index >= get_length())
            throw std::out_of_range("List::at index out of range");
        return m_start[index];
    }

private:
    T *m_start{};
    T *m_end{};
    T *m_capacity{};

    void *raw_alloc_(std::size_t bytes)
    {
        return ::operator new(bytes, std::align_val_t{alignof(T)});
    }

    void deallocate_(void *ptr) noexcept
    {
        if (!ptr) return;
        ::operator delete(ptr, std::align_val_t{alignof(T)});
    }

    void allocate_(std::size_t n)
    {
        m_start = static_cast<T *>(raw_alloc_(n * sizeof(T)));
        if (!m_start) throw std::runtime_error("Failed to allocate memory for List.");
        m_end = m_start;
        m_capacity = m_start + n;
    }

    void grow_()
    {
        const std::size_t cap = get_capacity();
        std::size_t inc = cap / 2;
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