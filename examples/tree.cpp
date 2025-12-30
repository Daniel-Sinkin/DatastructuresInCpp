#include <print>

#include "tree_node.hpp"

int main()
{
    TreeNode<int> g{0};
    g.create_new_child(1);
    g.create_new_child(2);
    g.create_new_child(3);

    auto* child_1 = g.child_ptr(1);
    child_1->create_new_child(42);
    child_1->create_new_child(43);
    child_1->child_ptr(0)->create_new_child(99);

    std::println("General tree:\n{}", g);
}