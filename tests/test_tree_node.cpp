// tests/test_tree_node.cpp
#include "common.hpp"
#include "tree_node.hpp"

#include <format>
#include <string>
#include <type_traits>

namespace dsalgo::Test
{

static void test_basic_construction_and_children()
{
    TreeNode<int> root{0};
    EXPECT_EQ(root.value(), 0);
    EXPECT_EQ(root.child_count(), 0zu);

    root.create_new_child(10);
    root.create_new_child(20);
    root.create_new_child(30);

    EXPECT_EQ(root.child_count(), 3zu);
    EXPECT_TRUE(root.child_ptr(0) != nullptr);
    EXPECT_TRUE(root.child_ptr(1) != nullptr);
    EXPECT_TRUE(root.child_ptr(2) != nullptr);

    EXPECT_EQ(root.child_ptr(0)->value(), 10);
    EXPECT_EQ(root.child_ptr(1)->value(), 20);
    EXPECT_EQ(root.child_ptr(2)->value(), 30);

    // nested
    root.child_ptr(1)->create_new_child(42);
    EXPECT_EQ(root.child_ptr(1)->child_count(), 1zu);
    EXPECT_EQ(root.child_ptr(1)->child_ptr(0)->value(), 42);
}

static void test_child_ptr_bounds_throws()
{
    TreeNode<int> root{0};
    EXPECT_THROW(root.child_ptr(0)); // out_of_range from vector::at

    root.create_new_child(1);
    EXPECT_NO_THROW(root.child_ptr(0));
    EXPECT_THROW(root.child_ptr(1));
}

static void test_const_accessors()
{
    TreeNode<int> root{7};
    root.create_new_child(8);

    const TreeNode<int>& croot = root;
    EXPECT_EQ(croot.value(), 7);
    EXPECT_EQ(croot.child_count(), 1zu);
    EXPECT_TRUE(croot.child_ptr(0) != nullptr);
    EXPECT_EQ(croot.child_ptr(0)->value(), 8);
    EXPECT_THROW(croot.child_ptr(1));
}

static void test_move_only_type_supported()
{
    // Ensures the API works with move-only payloads.
    TreeNode<std::unique_ptr<int>> root{std::make_unique<int>(1)};
    root.create_new_child(std::make_unique<int>(2));

    EXPECT_TRUE(root.value() != nullptr);
    EXPECT_EQ(*root.value(), 1);
    EXPECT_EQ(root.child_count(), 1zu);
    EXPECT_TRUE(root.child_ptr(0)->value() != nullptr);
    EXPECT_EQ(*root.child_ptr(0)->value(), 2);
}

static void test_formatter_smoke()
{
    TreeNode<int> root{0};
    root.create_new_child(1);
    root.create_new_child(2);
    root.child_ptr(1)->create_new_child(42);

    const std::string s = std::format("{}", root);

    // Basic smoke checks: contains root line and at least one child line with index labels.
    EXPECT_TRUE(s.find("root: 0") != std::string::npos);
    EXPECT_TRUE(s.find("0: 1") != std::string::npos || s.find("0: 1\n") != std::string::npos);
    EXPECT_TRUE(s.find("1: 2") != std::string::npos || s.find("1: 2\n") != std::string::npos);
    EXPECT_TRUE(s.find("0: 42") != std::string::npos); // nested child index at that level
}

static void test_type_traits_move_only_node()
{
    static_assert(std::is_move_constructible_v<TreeNode<int>>);
    static_assert(std::is_move_assignable_v<TreeNode<int>>);
}

} // namespace dsalgo::Test

int main()
{
    using namespace dsalgo::Test;
    test_basic_construction_and_children();
    test_child_ptr_bounds_throws();
    test_const_accessors();
    test_move_only_type_supported();
    test_formatter_smoke();
    test_type_traits_move_only_node();
    return 0;
}