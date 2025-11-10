// tests/test_list.cpp
#include <cstddef>
#include <cstdint>
#include <type_traits>

#include "common.hpp"
#include "list.hpp"
#include "primitives.hpp"

namespace dsalgo::Test
{

// Negative sanity: not trivially copyable -> List should be ill-formed for this.
struct NonTrivial
{
    NonTrivial() {}
    ~NonTrivial() {}
};
static_assert(!std::is_trivially_copyable_v<NonTrivial>);

template <class T>
static void run_basic_tests(const T *vals, std::size_t n)
{
    static_assert(std::is_trivially_copyable_v<T>, "T must be trivially copyable");
    using L = List<T>;

    L a{0};
    EXPECT_EQ(a.get_length(), std::size_t{0});
    EXPECT_EQ(a.get_capacity(), std::size_t{0});
    EXPECT_TRUE(a.is_empty());

    for (std::size_t i = 0; i < n; ++i)
        a.push_back(vals[i]);
    EXPECT_EQ(a.get_length(), n);
    EXPECT_TRUE(a.get_capacity() >= std::size_t{4});

    L b = a;
    EXPECT_EQ(b.get_length(), a.get_length());
    EXPECT_EQ(b.get_capacity(), a.get_capacity());
    for (std::size_t i = 0; i < n; ++i)
        EXPECT_EQ(b.pop_back_return(), vals[n - 1 - i]);
    EXPECT_TRUE(b.is_empty());

    L c = std::move(a);
    EXPECT_EQ(c.get_length(), n);
    EXPECT_EQ(a.get_length(), std::size_t{0});
    EXPECT_EQ(a.get_capacity(), std::size_t{0});

    for (std::size_t i = 0; i < n; ++i)
    {
        T got = c.pop_back_return();
        EXPECT_EQ(got, vals[n - 1 - i]);
    }
    EXPECT_TRUE(c.is_empty());

    L d{2};
    for (std::size_t i = 0; i < n; ++i)
        d.push_back(vals[i]);
    const std::size_t d_len = d.get_length();
    const std::size_t d_cap = d.get_capacity();
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wself-assign-overloaded"
    d = d;
#pragma clang diagnostic pop
    EXPECT_EQ(d.get_length(), d_len);
    EXPECT_EQ(d.get_capacity(), d_cap);
}
} // namespace dsalgo::Test
int main()
{
    using namespace dsalgo::Test;
    {
        float vals[] = {0.0f, 1.5f, -2.25f, 100.0f, 3.25f};
        run_basic_tests(vals, std::size_t{5});
    }

    {
        double vals[] = {0.0, 1.5, -2.25, 100.0, 3.25};
        run_basic_tests(vals, std::size_t{5});
    }

    {
        std::uint32_t vals[] = {0u, 1u, 42u, 0xDEADBEEFu, 7u};
        run_basic_tests(vals, std::size_t{5});
    }
    {
        std::int32_t vals[] = {0, -1, 42, -100000, 7};
        run_basic_tests(vals, std::size_t{5});
    }
    {
        std::size_t vals[] = {0zu, 1zu, 4096zu, 65535zu, 7zu};
        run_basic_tests(vals, std::size_t{5});
    }

    {
        Vec3f vals[] = {{0.f, 0.f, 0.f}, {1.f, 2.f, 3.f}, {-3.f, 4.f, -5.f}, {100.f, -5.f, 6.f}};
        run_basic_tests(vals, std::size_t{4});
    }
    {
        Vec3d vals[] = {{0.0, 0.0, 0.0}, {1.0, 2.0, 3.0}, {-3.0, 4.0, -5.0}, {100.0, -5.0, 6.0}};
        run_basic_tests(vals, std::size_t{4});
    }
    {
        Vec4f vals[] = {
            {0.f, 0.f, 0.f, 0.f},
            {1.f, 2.f, 3.f, 4.f},
            {-3.f, 4.f, -5.f, 6.f},
            {100.f, -5.f, 6.f, 7.f}};
        run_basic_tests(vals, std::size_t{4});
    }
    {
        Vec4d vals[] = {
            {0.0, 0.0, 0.0, 0.0},
            {1.0, 2.0, 3.0, 4.0},
            {-3.0, 4.0, -5.0, 6.0},
            {100.0, -5.0, 6.0, 7.0}};
        run_basic_tests(vals, std::size_t{4});
    }
    {
        ColorRGB vals[] = {
            {0, 0, 0},
            {255, 0, 0},
            {0, 255, 0},
            {0, 0, 255}};
        run_basic_tests(vals, std::size_t{4});
    }
    {
        ColorRGBA vals[] = {
            {0, 0, 0, 255},
            {255, 0, 0, 255},
            {0, 255, 0, 255},
            {0, 0, 255, 255},
            {128, 128, 128, 128}};
        run_basic_tests(vals, std::size_t{5});
    }

    {
        List<Vec3f> v{0};
        EXPECT_THROW(v.pop_back());
        v.push_back({1.f, 2.f, 3.f});
        EXPECT_NO_THROW(v.pop_back());
        EXPECT_THROW(v.pop_back());
    }

    return 0;
}