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
        size_t capacity = other.get_capacity();

        if (capacity == 0)
        {
            m_start = nullptr;
            m_end = nullptr;
            m_capacity = nullptr;
            return;
        }

        m_start = static_cast<double *>(std::malloc(capacity * sizeof(double)));
        if (!m_start) throw std::runtime_error("Failed to allocate memory for List.");

        if (n_elements > 0) std::memcpy(m_start, other.m_start, n_elements * sizeof(double));
        m_capacity = m_start + capacity;
        m_end = m_start + n_elements;
    }

    List &operator=(const List &other)
    {
        if (this == &other) return *this;

        size_t n_elements = other.get_length();
        size_t capacity = other.get_capacity();
        double *new_start = nullptr;
        if (capacity > 0)
        {
            new_start = static_cast<double *>(std::malloc(capacity * sizeof(double)));
            if (!new_start) throw std::runtime_error("Failed to allocate memory for List.");
            if (n_elements > 0) std::memcpy(new_start, other.m_start, n_elements * sizeof(double));
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

    ~List()
    {
        std::free(m_start);
    }

    void push_back(double value)
    {
        size_t n_elements = get_length();
        size_t capacity = get_capacity();
        if (n_elements < capacity)
        {
            *m_end = value;
            ++m_end;
            return;
        }

        size_t increment = capacity / 2;
        if (increment < 4zu) increment = 4zu;
        size_t new_capacity = capacity + increment;

        double *new_start = static_cast<double *>(std::realloc(m_start, new_capacity * sizeof(double)));
        if (!new_start) throw std::runtime_error("Failed to allocate memory of reallocation");

        m_start = new_start;
        m_end = m_start + n_elements;
        m_capacity = m_start + new_capacity;

        *m_end = value;
        ++m_end;
    }

    [[nodiscard]] double pop_back_return()
    {
        if (is_empty()) throw std::runtime_error("pop_back on empty!");
        double value = *(m_end - 1);
        pop_back();
        return value;
    }

    void pop_back()
    {
        if (is_empty()) throw std::runtime_error("pop_back on empty!");
        --m_end;
    }

    [[nodiscard]] size_t get_length() const noexcept
    {
        if (m_start)
        {
            return static_cast<size_t>(m_end - m_start);
        }
        else
        {
            return 0zu;
        }
    }
    [[nodiscard]] size_t get_capacity() const noexcept
    {
        if (m_start)
        {
            return static_cast<size_t>(m_capacity - m_start);
        }
        else
        {
            return 0zu;
        }
    }
    [[nodiscard]] inline bool is_empty() noexcept { return m_end == m_start; }
    [[nodiscard]] inline bool is_full() noexcept { return m_end == m_capacity; }

private:
    double *m_start = nullptr;
    double *m_end = nullptr;
    double *m_capacity = nullptr;
};

static void expect(bool ok, const char *msg)
{
    if (!ok) throw std::runtime_error(msg);
}

static void expect_exception(void (*fn)(), const char *msg)
{
    bool thrown = false;
    try
    {
        fn();
    }
    catch (const std::exception &)
    {
        thrown = true;
    }
    if (!thrown) throw std::runtime_error(msg);
}

#define EXPECT_THROW(expr, msg)                 \
    do                                          \
    {                                           \
        bool _t = false;                        \
        try                                     \
        {                                       \
            expr;                               \
        }                                       \
        catch (const std::exception &)          \
        {                                       \
            _t = true;                          \
        }                                       \
        if (!_t) throw std::runtime_error(msg); \
    } while (0)

static List *_g_list_ptr = nullptr;
static void _call_pop_back_on_global() { _g_list_ptr->pop_back(); }

static void test_list()
{
    std::println("section: construct");
    List a{4};
    expect(a.get_length() == 0zu, "a length not zero after construct");
    expect(a.get_capacity() == 4zu, "a capacity not 4 after construct");
    expect(a.is_empty(), "a not empty after construct");
    expect(!a.is_full(), "a unexpectedly full after construct");

    std::println("section: push within capacity");
    a.push_back(1.0);
    a.push_back(2.0);
    expect(a.get_length() == 2zu, "a length not 2 after pushes");
    expect(a.get_capacity() == 4zu, "a capacity changed without growth");
    expect(!a.is_empty(), "a empty after pushes");
    expect(!a.is_full(), "a full prematurely");

    std::println("section: fill to capacity");
    a.push_back(3.0);
    a.push_back(4.0);
    expect(a.get_length() == 4zu, "a length not 4 at capacity");
    expect(a.is_full(), "a not full at capacity");

    std::println("section: growth on push");
    const std::size_t cap_before = a.get_capacity();
    a.push_back(5.0);
    expect(a.get_length() == 5zu, "a length not 5 after growth");
    expect(a.get_capacity() > cap_before, "a did not grow");
    expect(!a.is_full(), "a still full after growth");

    std::println("section: LIFO pop");
    double v1 = a.pop_back_return();
    double v2 = a.pop_back_return();
    expect(v1 == 5.0, "pop_back_return value mismatch 1");
    expect(v2 == 4.0, "pop_back_return value mismatch 2");
    expect(a.get_length() == 3zu, "a length not 3 after two pops");
    expect(!a.is_empty(), "a empty after partial pops");

    std::println("section: copy constructor");
    List b = a;
    expect(b.get_length() == a.get_length(), "copy ctor length mismatch");
    expect(b.get_capacity() == a.get_capacity(), "copy ctor capacity mismatch");

    std::println("section: move constructor");
    const std::size_t b_len = b.get_length();
    const std::size_t b_cap = b.get_capacity();
    List c = std::move(b);
    expect(c.get_length() == b_len, "move ctor length mismatch");
    expect(c.get_capacity() == b_cap, "move ctor capacity mismatch");
    expect(b.get_length() == 0zu && b.get_capacity() == 0zu, "moved-from b not empty");

    std::println("section: copy assignment");
    List d{2};
    d = c;
    expect(d.get_length() == c.get_length(), "copy assign length mismatch");
    expect(d.get_capacity() == c.get_capacity(), "copy assign capacity mismatch");

    std::println("section: move assignment");
    List e{1};
    const std::size_t d_len = d.get_length();
    const std::size_t d_cap = d.get_capacity();
    e = std::move(d);
    expect(e.get_length() == d_len, "move assign length mismatch");
    expect(e.get_capacity() == d_cap, "move assign capacity mismatch");
    expect(d.get_length() == 0zu && d.get_capacity() == 0zu, "moved-from d not empty");

    std::println("section: self-assignment");
    const std::size_t e_len = e.get_length();
    const std::size_t e_cap = e.get_capacity();
    e = e;
    expect(e.get_length() == e_len, "self-assign changed length");
    expect(e.get_capacity() == e_cap, "self-assign changed capacity");

    std::println("section: empty list scenarios");
    List z{0};
    expect(z.get_length() == 0zu && z.get_capacity() == 0zu, "empty construct wrong");
    List zc = z;
    expect(zc.get_length() == 0zu && zc.get_capacity() == 0zu, "copy empty wrong");
    List zm = std::move(zc);
    expect(zm.get_length() == 0zu && zm.get_capacity() == 0zu, "move empty wrong");

    std::println("section: push into zero-capacity");
    z.push_back(42.0);
    expect(z.get_length() == 1zu, "z length not 1 after first push");
    expect(z.get_capacity() >= 4zu, "z capacity did not meet minimum growth");
    expect(z.pop_back_return() == 42.0, "z pop_back_return value mismatch");
    expect(z.is_empty(), "z not empty after pop");

    std::println("section: exception cases");
    List y{1};
    EXPECT_THROW(y.pop_back(), "y pop_back on single element failed to pop");
    EXPECT_THROW(y.pop_back(), "y pop_back on empty did not throw");

    _g_list_ptr = &zm;
    expect_exception(_call_pop_back_on_global, "global pop on empty did not throw");
}

int main()
{
    test_list();
    std::println("All tests passed.");
}