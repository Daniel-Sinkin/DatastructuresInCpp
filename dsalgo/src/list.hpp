// dsalgo/src/list.hpp
#pragma once
#include <cstddef>
#include <cstdlib>
#include <cstring>
#include <stdexcept>
#include <type_traits>

namespace dsalgo
{
template <class T>
    requires std::is_trivially_copyable_v<T>
class List
{
public:
    explicit List(size_t n_elements)
    {
        if (n_elements == 0) return;
        m_start = static_cast<T *>(std::malloc(n_elements * sizeof(T)));
        if (!m_start) throw std::runtime_error("Failed to allocate memory for List.");
        m_end = m_start;
        m_capacity = m_start + n_elements;
    }

    List(List &&other) noexcept
        : m_start(other.m_start),
          m_end(other.m_end),
          m_capacity(other.m_capacity)
    {
        other.m_start = other.m_end = other.m_capacity = nullptr;
    }

    List &operator=(List &&other) noexcept
    {
        if (this != &other)
        {
            std::free(m_start);
            m_start = other.m_start;
            m_end = other.m_end;
            m_capacity = other.m_capacity;

            other.m_start = other.m_end = other.m_capacity = nullptr;
        }
        return *this;
    }

    List(const List &other)
    {
        const size_t n_elements = other.get_length();
        const size_t capacity = other.get_capacity();

        if (capacity == 0) return;

        m_start = static_cast<T *>(std::malloc(capacity * sizeof(T)));
        if (!m_start) throw std::runtime_error("Failed to allocate memory for List.");

        if (n_elements > 0) std::memcpy(m_start, other.m_start, n_elements * sizeof(T));
        m_capacity = m_start + capacity;
        m_end = m_start + n_elements;
    }

    List &operator=(const List &other)
    {
        if (this == &other) return *this;

        const size_t n_elements = other.get_length();
        const size_t capacity = other.get_capacity();

        T *new_start = nullptr;
        if (capacity > 0)
        {
            new_start = static_cast<T *>(std::malloc(capacity * sizeof(T)));
            if (!new_start) throw std::runtime_error("Failed to allocate memory for List.");
            if (n_elements > 0)
            {
                std::memcpy(new_start, other.m_start, n_elements * sizeof(T));
            }
        }

        std::free(m_start);

        if (new_start)
        {
            m_start = new_start;
            m_end = new_start + n_elements;
            m_capacity = new_start + capacity;
        }
        else
        {
            m_start = nullptr;
            m_end = nullptr;
            m_capacity = nullptr;
        }

        return *this;
    }

    ~List() { std::free(m_start); }

    void push_back(T value)
    {
        const size_t n_elements = get_length();
        const size_t capacity = get_capacity();

        if (n_elements < capacity)
        {
            *m_end = value;
            ++m_end;
            return;
        }

        size_t increment = capacity / 2;
        if (increment < 4) increment = 4;
        const size_t new_capacity = capacity + increment;

        T *new_start = static_cast<T *>(std::realloc(m_start, new_capacity * sizeof(T)));
        if (!new_start) throw std::runtime_error("Failed to allocate memory during reallocation.");

        m_start = new_start;
        m_end = m_start + n_elements;
        m_capacity = m_start + new_capacity;

        *m_end = value;
        ++m_end;
    }

    [[nodiscard]] T pop_back_return()
    {
        if (is_empty()) throw std::runtime_error("pop_back on empty!");
        const T value = *(m_end - 1);
        --m_end;
        return value;
    }

    void pop_back()
    {
        if (is_empty()) throw std::runtime_error("pop_back on empty!");
        --m_end;
    }

    [[nodiscard]] size_t get_length() const noexcept
    {
        return m_start ? static_cast<size_t>(m_end - m_start) : 0zu;
    }

    [[nodiscard]] size_t get_capacity() const noexcept
    {
        return m_start ? static_cast<size_t>(m_capacity - m_start) : 0zu;
    }

    [[nodiscard]] bool is_empty() const noexcept { return m_end == m_start; }
    [[nodiscard]] bool is_full() const noexcept { return m_end == m_capacity; }

private:
    T *m_start{};
    T *m_end{};
    T *m_capacity{};
};
}