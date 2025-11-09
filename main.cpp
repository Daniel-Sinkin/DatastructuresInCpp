#include <print>

#include <cstddef>
#include <cstdlib>
#include <cstring>
#include <stdexcept>

#include "list.hpp"

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