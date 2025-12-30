#include <print>

#include "binary_tree_node.hpp"

int main()
{
    using namespace dsalgo;
    BinaryTreeNode<int> r{10};
    r.set_left(5);
    r.set_right(15);

    auto* left = r.left();
    left->set_left(2);
    left->set_right(7);

    std::println("Binary tree:\n{}", r);
}