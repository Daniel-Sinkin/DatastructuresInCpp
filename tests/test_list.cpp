// tests/test_list_pod.cpp
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <type_traits>

#include "common.hpp"
#include "list.hpp"

struct Vec2f
{
    float x, y;
    friend constexpr bool operator==(const Vec2f &a, const Vec2f &b)
    {
        return a.x == b.x && a.y == b.y;
    }
};

struct Vec3f
{
    float x, y, z;
    friend constexpr bool operator==(const Vec3f &a, const Vec3f &b)
    {
        return a.x == b.x && a.y == b.y && a.z == b.z;
    }
};

struct ColorRGBA8
{
    std::uint8_t r, g, b, a;
    friend constexpr bool operator==(const ColorRGBA8 &a, const ColorRGBA8 &b)
    {
        return a.r == b.r && a.g == b.g && a.b == b.b && a.a == b.a;
    }
};

// Negative sanity: not trivially copyable -> List should be ill-formed for this.
// Do NOT instantiate List<NonTrivial>.
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

    // push n values
    for (std::size_t i = 0; i < n; ++i)
        a.push_back(vals[i]);
    EXPECT_EQ(a.get_length(), n);
    EXPECT_TRUE(a.get_capacity() >= std::size_t{4});

    // copy
    L b = a;
    EXPECT_EQ(b.get_length(), a.get_length());
    EXPECT_EQ(b.get_capacity(), a.get_capacity());
    for (std::size_t i = 0; i < n; ++i)
        EXPECT_EQ(b.pop_back_return(), vals[n - 1 - i]);
    EXPECT_TRUE(b.is_empty());

    // move
    L c = std::move(a);
    EXPECT_EQ(c.get_length(), n);
    EXPECT_EQ(a.get_length(), std::size_t{0});
    EXPECT_EQ(a.get_capacity(), std::size_t{0});

    // pop validates LIFO and element integrity
    for (std::size_t i = 0; i < n; ++i)
    {
        T got = c.pop_back_return();
        EXPECT_EQ(got, vals[n - 1 - i]);
    }
    EXPECT_TRUE(c.is_empty());

    // self-assign checks
    L d{2};
    for (std::size_t i = 0; i < n; ++i)
        d.push_back(vals[i]);
    const std::size_t d_len = d.get_length();
    const std::size_t d_cap = d.get_capacity();
    d = d;
    EXPECT_EQ(d.get_length(), d_len);
    EXPECT_EQ(d.get_capacity(), d_cap);
}

int main()
{
    // float
    {
        float vals[] = {0.0f, 1.5f, -2.25f, 100.0f, 3.25f};
        run_basic_tests(vals, std::size_t{5});
    }

    // double
    {
        double vals[] = {0.0, 1.5, -2.25, 100.0, 3.25};
        run_basic_tests(vals, std::size_t{5});
    }

    // integers commonly used in gamedev
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

    // small POD structs used in gamedev
    {
        Vec2f vals[] = {{0.f, 0.f}, {1.f, 2.f}, {-3.f, 4.f}, {100.f, -5.f}};
        run_basic_tests(vals, std::size_t{4});
    }
    {
        Vec3f vals[] = {{0.f, 0.f, 0.f}, {1.f, 2.f, 3.f}, {-3.f, 4.f, -5.f}, {100.f, -5.f, 6.f}};
        run_basic_tests(vals, std::size_t{4});
    }
    {
        ColorRGBA8 vals[] = {
            {0, 0, 0, 255}, {255, 0, 0, 255}, {0, 255, 0, 255}, {0, 0, 255, 255}, {128, 128, 128, 128}};
        run_basic_tests(vals, std::size_t{5});
    }

    // push/pop exception behavior for POD types
    {
        List<Vec3f> v{0};
        EXPECT_THROW(v.pop_back());
        v.push_back({1.f, 2.f, 3.f});
        EXPECT_NO_THROW(v.pop_back());
        EXPECT_THROW(v.pop_back());
    }

    return 0;
}