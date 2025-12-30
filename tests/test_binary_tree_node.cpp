// tests/test_binary_tree_node.cpp
#include "binary_tree_node.hpp"
#include "common.hpp"

#include <format>
#include <string>
#include <type_traits>

namespace dsalgo::Test
{

static void test_empty_children_invariants()
{
    BinaryTreeNode<int> r{10};
    EXPECT_TRUE(r.left() == nullptr);
    EXPECT_TRUE(r.right() == nullptr);
}

static void test_set_left_right_create_and_overwrite()
{
    BinaryTreeNode<int> r{10};

    r.set_left(5);
    EXPECT_TRUE(r.left() != nullptr);
    EXPECT_EQ(r.left()->m_value, 5);
    EXPECT_TRUE(r.right() == nullptr);

    r.set_right(15);
    EXPECT_TRUE(r.right() != nullptr);
    EXPECT_EQ(r.right()->m_value, 15);

    // overwrite existing nodes (should not replace the pointer, just update value)
    auto* lptr_before = r.left();
    auto* rptr_before = r.right();

    r.set_left(6);
    r.set_right(16);

    EXPECT_TRUE(r.left() == lptr_before);
    EXPECT_TRUE(r.right() == rptr_before);
    EXPECT_EQ(r.left()->m_value, 6);
    EXPECT_EQ(r.right()->m_value, 16);
}

static void test_nested_setters()
{
    BinaryTreeNode<int> r{10};
    r.set_left(5);
    r.set_right(15);

    r.left()->set_left(2);
    r.left()->set_right(7);

    EXPECT_TRUE(r.left()->left() != nullptr);
    EXPECT_TRUE(r.left()->right() != nullptr);
    EXPECT_EQ(r.left()->left()->m_value, 2);
    EXPECT_EQ(r.left()->right()->m_value, 7);
}

static void test_const_overloads()
{
    BinaryTreeNode<int> r{10};
    r.set_left(5);

    const BinaryTreeNode<int>& cr = r;
    EXPECT_TRUE(cr.left() != nullptr);
    EXPECT_TRUE(cr.right() == nullptr);
    EXPECT_EQ(cr.left()->m_value, 5);
}

static void test_move_only_payload_supported()
{
    BinaryTreeNode<std::unique_ptr<int>> r{std::make_unique<int>(1)};
    EXPECT_TRUE(r.m_value != nullptr);
    EXPECT_EQ(*r.m_value, 1);

    r.set_left(std::make_unique<int>(2));
    EXPECT_TRUE(r.left() != nullptr);
    EXPECT_TRUE(r.left()->m_value != nullptr);
    EXPECT_EQ(*r.left()->m_value, 2);
}

static void test_formatter_smoke()
{
    BinaryTreeNode<int> r{10};
    r.set_left(5);
    r.set_right(15);
    r.left()->set_left(2);
    r.left()->set_right(7);

    const std::string s = std::format("{}", r);

    EXPECT_TRUE(s.find("root: 10") != std::string::npos);
    EXPECT_TRUE(s.find("L: 5") != std::string::npos);
    EXPECT_TRUE(s.find("R: 15") != std::string::npos);
    EXPECT_TRUE(s.find("L: 2") != std::string::npos);
    EXPECT_TRUE(s.find("R: 7") != std::string::npos);
}

static void test_type_traits_move_only_node()
{
    static_assert(!std::is_copy_constructible_v<BinaryTreeNode<int>>);
    static_assert(std::is_move_constructible_v<BinaryTreeNode<int>>);
    static_assert(!std::is_copy_assignable_v<BinaryTreeNode<int>>);
    static_assert(std::is_move_assignable_v<BinaryTreeNode<int>>);
}

} // namespace dsalgo::Test

int main()
{
    using namespace dsalgo::Test;
    test_empty_children_invariants();
    test_set_left_right_create_and_overwrite();
    test_nested_setters();
    test_const_overloads();
    test_move_only_payload_supported();
    test_formatter_smoke();
    test_type_traits_move_only_node();
    return 0;
}