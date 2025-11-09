// tests/test_list.cpp
#include <cstddef>
#include <stdexcept>

#include "common.hpp"
#include "list.hpp"

static List *g_ptr = nullptr;

void test_list()
{
    List a{4};
    EXPECT_EQ(a.get_length(), std::size_t{0});
    EXPECT_EQ(a.get_capacity(), std::size_t{4});
    EXPECT_TRUE(a.is_empty());
    EXPECT_TRUE(!a.is_full());

    a.push_back(1.0);
    a.push_back(2.0);
    EXPECT_EQ(a.get_length(), std::size_t{2});
    EXPECT_EQ(a.get_capacity(), std::size_t{4});
    EXPECT_TRUE(!a.is_empty());
    EXPECT_TRUE(!a.is_full());

    a.push_back(3.0);
    a.push_back(4.0);
    EXPECT_EQ(a.get_length(), std::size_t{4});
    EXPECT_TRUE(a.is_full());

    const std::size_t cap_before = a.get_capacity();
    a.push_back(5.0);
    EXPECT_EQ(a.get_length(), std::size_t{5});
    EXPECT_TRUE(a.get_capacity() > cap_before);
    EXPECT_TRUE(!a.is_full());

    double v1 = a.pop_back_return();
    double v2 = a.pop_back_return();
    EXPECT_EQ(v1, 5.0);
    EXPECT_EQ(v2, 4.0);
    EXPECT_EQ(a.get_length(), std::size_t{3});
    EXPECT_TRUE(!a.is_empty());

    List b = a;
    EXPECT_EQ(b.get_length(), a.get_length());
    EXPECT_EQ(b.get_capacity(), a.get_capacity());

    const std::size_t b_len = b.get_length();
    const std::size_t b_cap = b.get_capacity();
    List c = std::move(b);
    EXPECT_EQ(c.get_length(), b_len);
    EXPECT_EQ(c.get_capacity(), b_cap);
    EXPECT_EQ(b.get_length(), std::size_t{0});
    EXPECT_EQ(b.get_capacity(), std::size_t{0});

    List d{2};
    d = c;
    EXPECT_EQ(d.get_length(), c.get_length());
    EXPECT_EQ(d.get_capacity(), c.get_capacity());

    List e{1};
    const std::size_t d_len = d.get_length();
    const std::size_t d_cap = d.get_capacity();
    e = std::move(d);
    EXPECT_EQ(e.get_length(), d_len);
    EXPECT_EQ(e.get_capacity(), d_cap);
    EXPECT_EQ(d.get_length(), std::size_t{0});
    EXPECT_EQ(d.get_capacity(), std::size_t{0});

    const std::size_t e_len = e.get_length();
    const std::size_t e_cap = e.get_capacity();
    e = e; // self-assign
    EXPECT_EQ(e.get_length(), e_len);
    EXPECT_EQ(e.get_capacity(), e_cap);

    List z{0};
    EXPECT_EQ(z.get_length(), std::size_t{0});
    EXPECT_EQ(z.get_capacity(), std::size_t{0});
    List zc = z;
    EXPECT_EQ(zc.get_length(), std::size_t{0});
    EXPECT_EQ(zc.get_capacity(), std::size_t{0});
    List zm = std::move(zc);
    EXPECT_EQ(zm.get_length(), std::size_t{0});
    EXPECT_EQ(zm.get_capacity(), std::size_t{0});

    z.push_back(42.0);
    EXPECT_EQ(z.get_length(), std::size_t{1});
    EXPECT_TRUE(z.get_capacity() >= std::size_t{4});
    EXPECT_EQ(z.pop_back_return(), 42.0);
    EXPECT_TRUE(z.is_empty());

    List y{1};
    EXPECT_THROW(y.pop_back()); // empty -> throws
    y.push_back(11.0);
    EXPECT_NO_THROW(y.pop_back()); // now not empty -> ok
    EXPECT_THROW(y.pop_back());    // empty again -> throws

    g_ptr = &zm;
    expect_exception([]
        { g_ptr->pop_back(); }, "global pop on empty did not throw");
}