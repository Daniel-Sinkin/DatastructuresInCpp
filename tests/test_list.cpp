// tests/test_list.cpp
#include <cstddef>
#include <cstdint>
#include <type_traits>
#include <utility>

#include "common.hpp"
#include "list.hpp"
#include "primitives.hpp"

namespace dsalgo::Test
{

struct NonTrivial
{
    NonTrivial() {}
    ~NonTrivial() {}
};
static_assert(!std::is_trivially_copyable_v<NonTrivial>);

template <class T>
static void check_sequence_eq(const List<T> &lst, const T *vals, std::size_t n)
{
    EXPECT_EQ(lst.get_length(), n);
    List<T> copy = lst;
    for (std::size_t i = 0; i < n; ++i)
    {
        T got = copy.pop_back_return();
        const T &expect = vals[n - 1 - i];
        EXPECT_TRUE(got == expect);
    }
    EXPECT_TRUE(copy.is_empty());
}

template <class T>
static void run_basic_push_copy_move(const T *vals, std::size_t n)
{
    static_assert(std::is_trivially_copyable_v<T>);
    using L = List<T>;

    L a{0};
    EXPECT_EQ(a.get_length(), std::size_t{0});
    EXPECT_EQ(a.get_capacity(), std::size_t{0});
    EXPECT_TRUE(a.is_empty());

    for (std::size_t i = 0; i < n; ++i)
        a.push_back(vals[i]);
    EXPECT_EQ(a.get_length(), n);
    EXPECT_TRUE(a.get_capacity() >= std::size_t{4});
    check_sequence_eq(a, vals, n);

    L b = a;
    EXPECT_EQ(b.get_length(), a.get_length());
    EXPECT_EQ(b.get_capacity(), a.get_capacity());
    check_sequence_eq(b, vals, n);

    L c = std::move(a);
    EXPECT_EQ(c.get_length(), n);
    EXPECT_EQ(a.get_length(), std::size_t{0});
    EXPECT_EQ(a.get_capacity(), std::size_t{0});
    check_sequence_eq(c, vals, n);

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

static void run_emplace_tests()
{
    List<std::uint32_t> li{0};
    for (std::uint32_t i = 0; i < 10; ++i)
    {
        auto &ref = li.emplace_back(i);
        EXPECT_TRUE(ref == i);
        EXPECT_EQ(li.get_length(), static_cast<std::size_t>(i + 1));
        EXPECT_TRUE(li.get_capacity() >= li.get_length());
    }
    for (int i = 9; i >= 0; --i)
    {
        auto v = li.pop_back_return();
        EXPECT_TRUE(v == static_cast<std::uint32_t>(i));
    }
    EXPECT_TRUE(li.is_empty());

    List<Vec3f> lv{0};
    lv.emplace_back(Vec3f{1.0f, 2.0f, 3.0f});
    lv.emplace_back(Vec3f{0.0f, 0.0f, 0.0f});
    EXPECT_EQ(lv.get_length(), std::size_t{2});
    {
        Vec3f v = lv.pop_back_return();
        EXPECT_TRUE((v == Vec3f{0.0f, 0.0f, 0.0f}));
        v = lv.pop_back_return();
        EXPECT_TRUE((v == Vec3f{1.0f, 2.0f, 3.0f}));
        EXPECT_TRUE(lv.is_empty());
    }
}

static void run_growth_reserve_clear()
{
    List<std::size_t> l{0};
    std::size_t last_cap = l.get_capacity();
    l.emplace_back(42zu);
    EXPECT_TRUE(l.get_capacity() >= std::size_t{4});
    EXPECT_TRUE(l.get_capacity() >= l.get_length());
    EXPECT_TRUE(l.get_capacity() >= last_cap);
    last_cap = l.get_capacity();

    for (int i = 0; i < 100; ++i)
    {
        l.emplace_back(static_cast<std::size_t>(i));
        EXPECT_TRUE(l.get_capacity() >= l.get_length());
        EXPECT_TRUE(l.get_capacity() >= last_cap);
        last_cap = l.get_capacity();
    }

    const std::size_t len_before = l.get_length();
    const std::size_t cap_before = l.get_capacity();
    l.reserve(cap_before + 50);
    EXPECT_EQ(l.get_length(), len_before);
    EXPECT_TRUE(l.get_capacity() >= cap_before + 50);

    l.clear();
    EXPECT_EQ(l.get_length(), std::size_t{0});
    EXPECT_EQ(l.get_capacity(), last_cap = l.get_capacity());
    EXPECT_TRUE(l.is_empty());
}

static void run_exception_cases()
{
    List<int> v{0};
    EXPECT_THROW(v.pop_back());
    EXPECT_THROW(v.pop_back_return());
}

static void run_reserve_edge_cases()
{
    List<int> v{0};
    v.reserve(0);
    EXPECT_EQ(v.get_capacity(), 0zu);
    v.reserve(1);
    EXPECT_TRUE(v.get_capacity() >= 1zu);
    v.emplace_back(7);
    EXPECT_EQ(v.get_length(), 1zu);
}

static void run_reserve_exact_fill()
{
    List<int> v{0};
    v.reserve(32);
    for (int i = 0; i < 32; ++i)
        v.emplace_back(i);
    EXPECT_EQ(v.get_length(), 32zu);
    EXPECT_EQ(v.get_capacity(), 32zu);
}

static void run_clear_reuse()
{
    List<int> v{0};
    for (int i = 0; i < 10; ++i)
        v.emplace_back(i);
    const auto cap = v.get_capacity();
    v.clear();
    EXPECT_EQ(v.get_length(), 0zu);
    EXPECT_EQ(v.get_capacity(), cap);
    v.emplace_back(42);
    EXPECT_EQ(v.pop_back_return(), 42);
}

static void run_move_from_reuse()
{
    List<int> a{0};
    a.emplace_back(1);
    List<int> b = std::move(a);
    EXPECT_EQ(a.get_length(), 0zu);
    EXPECT_EQ(a.get_capacity(), 0zu);
    a.emplace_back(9);
    EXPECT_EQ(a.pop_back_return(), 9);
}

static void run_reserve_realloc_null()
{
    List<int> v{0};
    v.reserve(8);
    EXPECT_TRUE(v.get_capacity() >= 8zu);
}

} // namespace dsalgo::Test

int main()
{
    using namespace dsalgo::Test;
    {
        float vals[] = {0.0f, 1.5f, -2.25f, 100.0f, 3.25f};
        run_basic_push_copy_move(vals, std::size_t{5});
    }
    {
        double vals[] = {0.0, 1.5, -2.25, 100.0, 3.25};
        run_basic_push_copy_move(vals, std::size_t{5});
    }
    {
        std::uint32_t vals[] = {0u, 1u, 42u, 0xDEADBEEFu, 7u};
        run_basic_push_copy_move(vals, std::size_t{5});
    }
    {
        std::int32_t vals[] = {0, -1, 42, -100000, 7};
        run_basic_push_copy_move(vals, std::size_t{5});
    }
    {
        std::size_t vals[] = {0zu, 1zu, 4096zu, 65535zu, 7zu};
        run_basic_push_copy_move(vals, std::size_t{5});
    }
    {
        Vec3f vals[] = {{0.0f, 0.0f, 0.0f}, {1.0f, 2.0f, 3.0f}, {-3.0f, 4.0f, -5.0f}, {100.0f, -5.0f, 6.0f}};
        run_basic_push_copy_move(vals, std::size_t{4});
    }
    {
        Vec3d vals[] = {{0.0, 0.0, 0.0}, {1.0, 2.0, 3.0}, {-3.0, 4.0, -5.0}, {100.0, -5.0, 6.0}};
        run_basic_push_copy_move(vals, std::size_t{4});
    }
    {
        Vec4f vals[] = {
            {0.0f, 0.0f, 0.0f, 0.0f},
            {1.0f, 2.0f, 3.0f, 4.0f},
            {-3.0f, 4.0f, -5.0f, 6.0f},
            {100.0f, -5.0f, 6.0f, 7.0f}};
        run_basic_push_copy_move(vals, std::size_t{4});
    }
    {
        Vec4d vals[] = {
            {0.0, 0.0, 0.0, 0.0},
            {1.0, 2.0, 3.0, 4.0},
            {-3.0, 4.0, -5.0, 6.0},
            {100.0, -5.0, 6.0, 7.0}};
        run_basic_push_copy_move(vals, std::size_t{4});
    }
    {
        ColorRGB vals[] = {
            {0, 0, 0},
            {255, 0, 0},
            {0, 255, 0},
            {0, 0, 255}};
        run_basic_push_copy_move(vals, std::size_t{4});
    }
    {
        ColorRGBA vals[] = {
            {0, 0, 0, 255},
            {255, 0, 0, 255},
            {0, 255, 0, 255},
            {0, 0, 255, 255},
            {128, 128, 128, 128}};
        run_basic_push_copy_move(vals, std::size_t{5});
    }

    run_emplace_tests();
    run_growth_reserve_clear();
    run_exception_cases();
    run_reserve_edge_cases();
    run_reserve_exact_fill();
    run_clear_reuse();
    run_move_from_reuse();
    run_reserve_realloc_null();
    return 0;
}