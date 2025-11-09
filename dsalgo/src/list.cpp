// dsalgo/src/list.cpp
#include "list.hpp"

#include <cstdlib>
#include <cstring>
#include <stdexcept>

List::List(std::size_t n_elements)
{
    if (n_elements == 0)
    {
        m_start = nullptr;
        m_end = nullptr;
        m_capacity = nullptr;
        return;
    }

    m_start = static_cast<double *>(std::malloc(n_elements * sizeof(double)));
    if (m_start == nullptr)
    {
        throw std::runtime_error("Failed to allocate memory for List.");
    }
    m_capacity = m_start + n_elements;
    m_end = m_start;
}

List::List(List &&other) noexcept
    : m_start(other.m_start),
      m_end(other.m_end),
      m_capacity(other.m_capacity)
{
    other.m_start = nullptr;
    other.m_capacity = nullptr;
    other.m_end = nullptr;
}

List &List::operator=(List &&other) noexcept
{
    if (this != &other)
    {
        std::free(m_start);
        m_start = other.m_start;
        m_end = other.m_end;
        m_capacity = other.m_capacity;

        other.m_start = nullptr;
        other.m_capacity = nullptr;
        other.m_end = nullptr;
    }
    return *this;
}

List::List(const List &other)
{
    const std::size_t n_elements = other.get_length();
    const std::size_t capacity = other.get_capacity();

    if (capacity == 0)
    {
        m_start = nullptr;
        m_end = nullptr;
        m_capacity = nullptr;
        return;
    }

    m_start = static_cast<double *>(std::malloc(capacity * sizeof(double)));
    if (!m_start)
    {
        throw std::runtime_error("Failed to allocate memory for List.");
    }

    if (n_elements > 0)
    {
        std::memcpy(m_start, other.m_start, n_elements * sizeof(double));
    }
    m_capacity = m_start + capacity;
    m_end = m_start + n_elements;
}

List &List::operator=(const List &other)
{
    if (this == &other) return *this;

    const std::size_t n_elements = other.get_length();
    const std::size_t capacity = other.get_capacity();

    double *new_start = nullptr;
    if (capacity > 0)
    {
        new_start = static_cast<double *>(std::malloc(capacity * sizeof(double)));
        if (!new_start)
        {
            throw std::runtime_error("Failed to allocate memory for List.");
        }
        if (n_elements > 0)
        {
            std::memcpy(new_start, other.m_start, n_elements * sizeof(double));
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

List::~List()
{
    std::free(m_start);
}

void List::push_back(double value)
{
    const std::size_t n_elements = get_length();
    const std::size_t capacity = get_capacity();

    if (n_elements < capacity)
    {
        *m_end = value;
        ++m_end;
        return;
    }

    // grow: +50%, minimum +4
    std::size_t increment = capacity / 2;
    if (increment < 4) increment = 4;
    const std::size_t new_capacity = capacity + increment;

    double *new_start = static_cast<double *>(std::realloc(m_start, new_capacity * sizeof(double)));
    if (!new_start)
    {
        throw std::runtime_error("Failed to allocate memory during reallocation.");
    }

    m_start = new_start;
    m_end = m_start + n_elements;
    m_capacity = m_start + new_capacity;

    *m_end = value;
    ++m_end;
}

double List::pop_back_return()
{
    if (is_empty())
    {
        throw std::runtime_error("pop_back on empty!");
    }
    const double value = *(m_end - 1);
    --m_end;
    return value;
}

void List::pop_back()
{
    if (is_empty())
    {
        throw std::runtime_error("pop_back on empty!");
    }
    --m_end;
}

std::size_t List::get_length() const noexcept
{
    if (m_start)
    {
        return static_cast<std::size_t>(m_end - m_start);
    }
    return 0u;
}

std::size_t List::get_capacity() const noexcept
{
    if (m_start)
    {
        return static_cast<std::size_t>(m_capacity - m_start);
    }
    return 0u;
}

bool List::is_empty() const noexcept
{
    return m_end == m_start;
}

bool List::is_full() const noexcept
{
    return m_end == m_capacity;
}