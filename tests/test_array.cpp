// tests/test_array.cpp
#include "array.hpp"
#include "common.hpp"
#include "primitives.hpp"
#include <type_traits>

namespace dsalgo::Test
{

static void test_sizes_and_iter_int()
{
    Array<int, 8> a;
    EXPECT_EQ(a.get_size_static(), 8zu);
    EXPECT_EQ(a.get_size(), 8zu);

    // default zero-initialized via T data[N]{}
    for (usize i = 0zu; i < a.get_size(); ++i)
        EXPECT_EQ(a[i], 0);

    // write via operator[]
    for (usize i = 0zu; i < a.get_size(); ++i)
        a[i] = static_cast<int>(i * 3);

    // iterate non-const
    int acc = 0;
    for (auto &v : a)
        acc += v;
    EXPECT_EQ(acc, 3 * (0 + 1 + 2 + 3 + 4 + 5 + 6 + 7)); // 3 * 28 = 84
}

static void test_fill_and_raw_vec()
{
    Array<Vec3f, 4> a;
    Vec3f c{1.0f, 2.0f, 3.0f};
    a.fill(c);
    for (usize i = 0zu; i < a.get_size(); ++i)
        EXPECT_TRUE(a[i] == c);

    // raw pointers and pointer math
    auto *p = a.raw();
    auto *e = a.raw() + a.get_size();
    EXPECT_TRUE(p + 4 == e);

    // const access pathways
    const auto &cref = a;
    EXPECT_TRUE(cref.begin() == a.raw());
    EXPECT_TRUE(cref.end() == a.raw() + a.get_size());
    for (auto it = cref.begin(); it != cref.end(); ++it)
        EXPECT_TRUE(*it == c);
}

static void test_mutation_through_iteration()
{
    Array<u32, 5> a;
    a.fill(7u);
    usize i = 0zu;
    for (auto &v : a)
    {
        v = static_cast<u32>(i++);
    }
    for (usize j = 0zu; j < a.get_size(); ++j)
        EXPECT_EQ(a[j], static_cast<u32>(j));
}

} // namespace dsalgo::Test

int main()
{
    using namespace dsalgo::Test;
    test_sizes_and_iter_int();
    test_fill_and_raw_vec();
    test_mutation_through_iteration();
    return 0;
}