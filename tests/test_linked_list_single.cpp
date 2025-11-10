// tests/test_linked_list_single.cpp
#include "common.hpp"
#include "linked_list_single.hpp"

namespace dsalgo::Test
{
static void test_empty_pop_idempotent()
{
    LinkedListSingle s;
    EXPECT_TRUE(s.is_empty());
    s.pop_front();
    EXPECT_TRUE(s.is_empty());
    s.pop_front();
    s.pop_front();
    EXPECT_TRUE(s.is_empty());
}

static void test_single_push_pop()
{
    LinkedListSingle s;
    EXPECT_TRUE(s.is_empty());
    s.push_front(1.0);
    EXPECT_TRUE(!s.is_empty());
    s.pop_front();
    EXPECT_TRUE(s.is_empty());
}

static void test_multi_push_then_pop_all()
{
    LinkedListSingle s;
    for (int i = 0; i < 10; ++i)
        s.push_front(static_cast<double>(i));
    EXPECT_TRUE(!s.is_empty());
    for (int i = 0; i < 10; ++i)
        s.pop_front();
    EXPECT_TRUE(s.is_empty());
    s.pop_front();
    s.pop_front();
    EXPECT_TRUE(s.is_empty());
}

static void test_alternating_push_pop()
{
    LinkedListSingle s;
    for (int i = 0; i < 1000; ++i)
    {
        EXPECT_TRUE(s.is_empty());
        s.push_front(static_cast<double>(i));
        EXPECT_TRUE(!s.is_empty());
        s.pop_front();
        EXPECT_TRUE(s.is_empty());
    }
}

static void test_stress_large_n()
{
    LinkedListSingle s;
    constexpr int N = 200000;
    for (int i = 0; i < N; ++i)
        s.push_front(static_cast<double>(i));
    EXPECT_TRUE(!s.is_empty());
    for (int i = 0; i < N; ++i)
        s.pop_front();
    EXPECT_TRUE(s.is_empty());
}

static void test_destructor_smoke()
{
    {
        LinkedListSingle s;
        for (int i = 0; i < 1000; ++i)
            s.push_front(static_cast<double>(i));
        EXPECT_TRUE(!s.is_empty());
    }
}

} // namespace dsalgo::Test

int main()
{
    using namespace dsalgo::Test;
    test_empty_pop_idempotent();
    test_single_push_pop();
    test_multi_push_then_pop_all();
    test_alternating_push_pop();
    test_stress_large_n();
    test_destructor_smoke();
    return 0;
}