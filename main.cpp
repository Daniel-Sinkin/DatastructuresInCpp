#include <print>

#include <cstddef>
#include <cstdlib>
#include <cstring>
#include <stdexcept>

class List
{
public:
    explicit List(size_t n_elements)
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

    List(List &&other) noexcept
        : m_start(other.m_start),
          m_capacity(other.m_capacity),
          m_end(other.m_end)
    {
        other.m_start = nullptr;
        other.m_capacity = nullptr;
        other.m_end = nullptr;
    }

    List &operator=(List &&other) noexcept
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

    List(const List &other)
    {
        size_t n_elements = other.get_length();
        size_t capacity = static_cast<size_t>(other.m_capacity - other.m_start);
        m_start = static_cast<double *>(std::malloc(capacity * sizeof(double)));
        if (m_start == nullptr)
        {
            throw std::runtime_error("Failed to allocate memory for List.");
        }
        m_capacity = m_start + capacity;
        m_end = m_start + n_elements;

        std::memcpy(m_start, other.m_start, n_elements * sizeof(double));
    }

    List &operator=(const List &other)
    {
        if (this == &other) return *this;

        free(m_start);

        size_t n_elements = other.get_length();
        size_t capacity = static_cast<size_t>(other.m_capacity - other.m_start);
        m_start = static_cast<double *>(std::malloc(capacity * sizeof(double)));
        if (m_start == nullptr)
        {
            throw std::runtime_error("Failed to allocate memory for List.");
        }
        m_capacity = m_start + capacity;
        m_end = m_start + n_elements;
        std::memcpy(m_start, other.m_start, n_elements * sizeof(double));
        return *this;
    }

    ~List()
    {
        std::free(m_start);
    }

    size_t get_length() const noexcept { return static_cast<size_t>(m_end - m_start); }
    size_t get_capacity_length() const noexcept { return static_cast<size_t>(m_capacity - m_start); }

private:
    double *m_start = nullptr;
    double *m_end = nullptr;
    double *m_capacity = nullptr;
};

int main()
{
    std::println("Hello, World!");
}