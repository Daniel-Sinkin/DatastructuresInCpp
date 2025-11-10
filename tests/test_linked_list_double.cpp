// tests/test_linked_list_double.cpp
#include "common.hpp"
#include "linked_list_double.hpp"
#include <cstddef>

namespace dsalgo::Test
{
static std::size_t fwd_len(const LinkedListDoubleNode *h)
{
    std::size_t n = 0;
    for (auto *p = h; p; p = p->next)
    {
        ++n;
    }
    return n;
}

static std::size_t back_len(const LinkedListDoubleNode *t)
{
    std::size_t n = 0;
    for (auto *p = t; p; p = p->prev)
    {
        ++n;
    }
    return n;
}

static void check_links(const LinkedListDouble &lst)
{
    auto *head = lst.front();
    auto *tail = lst.back();
    if (!head)
    {
        EXPECT_TRUE(tail == nullptr);
        return;
    }
    EXPECT_TRUE(head->prev == nullptr);
    EXPECT_TRUE(tail->next == nullptr);
    std::size_t n1 = fwd_len(head);
    std::size_t n2 = back_len(tail);
    EXPECT_TRUE(n1 == n2);
    for (auto *p = head; p && p->next; p = p->next)
    {
        EXPECT_TRUE(p->next->prev == p);
    }
    for (auto *p = tail; p && p->prev; p = p->prev)
    {
        EXPECT_TRUE(p->prev->next == p);
    }
}

static void test_empty_invariants()
{
    LinkedListDouble s;
    EXPECT_TRUE(s.is_empty());
    EXPECT_TRUE(s.front() == nullptr);
    EXPECT_TRUE(s.back() == nullptr);
    EXPECT_NO_THROW(s.pop_front());
    EXPECT_NO_THROW(s.pop_back());
    check_links(s);
}

static void test_single_push_front_pop_front()
{
    LinkedListDouble s;
    s.push_front(7);
    EXPECT_TRUE(!s.is_empty());
    EXPECT_TRUE(s.front() && s.front()->value == 7);
    EXPECT_TRUE(s.back() && s.back()->value == 7);
    check_links(s);
    s.pop_front();
    EXPECT_TRUE(s.is_empty());
    EXPECT_TRUE(s.front() == nullptr && s.back() == nullptr);
    check_links(s);
}

static void test_single_push_back_pop_back()
{
    LinkedListDouble s;
    s.push_back(9);
    EXPECT_TRUE(!s.is_empty());
    EXPECT_TRUE(s.front() && s.front()->value == 9);
    EXPECT_TRUE(s.back() && s.back()->value == 9);
    check_links(s);
    s.pop_back();
    EXPECT_TRUE(s.is_empty());
    EXPECT_TRUE(s.front() == nullptr && s.back() == nullptr);
    check_links(s);
}

static void test_multiple_push_front()
{
    LinkedListDouble s;
    s.push_front(1);
    s.push_front(2);
    s.push_front(3);
    EXPECT_TRUE(s.front()->value == 3);
    EXPECT_TRUE(s.back()->value == 1);
    check_links(s);
    s.pop_front();
    EXPECT_TRUE(s.front()->value == 2);
    s.pop_front();
    EXPECT_TRUE(s.front()->value == 1);
    s.pop_front();
    EXPECT_TRUE(s.is_empty());
    check_links(s);
}

static void test_multiple_push_back()
{
    LinkedListDouble s;
    s.push_back(1);
    s.push_back(2);
    s.push_back(3);
    EXPECT_TRUE(s.front()->value == 1);
    EXPECT_TRUE(s.back()->value == 3);
    check_links(s);
    s.pop_back();
    EXPECT_TRUE(s.back() && s.back()->value == 2);
    s.pop_back();
    EXPECT_TRUE(s.back() && s.back()->value == 1);
    s.pop_back();
    EXPECT_TRUE(s.is_empty());
    check_links(s);
}

static void test_mixed_operations()
{
    LinkedListDouble s;
    s.push_front(1); // [1]
    s.push_back(2);  // [1,2]
    s.push_front(3); // [3,1,2]
    EXPECT_TRUE(s.front()->value == 3);
    EXPECT_TRUE(s.back()->value == 2);
    check_links(s);
    s.pop_back(); // [3,1]
    EXPECT_TRUE(s.back()->value == 1);
    s.pop_front(); // [1]
    EXPECT_TRUE(s.front()->value == 1 && s.back()->value == 1);
    s.pop_back(); // []
    EXPECT_TRUE(s.is_empty());
    check_links(s);
}

static void test_clear()
{
    LinkedListDouble s;
    for (int i = 0; i < 20; ++i)
        s.push_back(i);
    EXPECT_TRUE(!s.is_empty());
    s.clear();
    EXPECT_TRUE(s.is_empty());
    EXPECT_TRUE(s.front() == nullptr && s.back() == nullptr);
    check_links(s);
    s.clear();
    EXPECT_TRUE(s.is_empty());
}

static void test_forward_backward_lengths_small()
{
    LinkedListDouble s;
    for (int i = 0; i < 10; ++i)
        s.push_back(i);
    EXPECT_TRUE(fwd_len(s.front()) == 10);
    EXPECT_TRUE(back_len(s.back()) == 10);
    check_links(s);
}

} // namespace dsalgo::Test
int main()
{
    using namespace dsalgo::Test;
    test_empty_invariants();
    test_single_push_front_pop_front();
    test_single_push_back_pop_back();
    test_multiple_push_front();
    test_multiple_push_back();
    test_mixed_operations();
    test_clear();
    test_forward_backward_lengths_small();
    return 0;
}