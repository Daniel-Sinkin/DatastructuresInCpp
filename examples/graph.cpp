// main.cpp
#include <algorithm>
#include <expected>
#include <print>
#include <ranges>
#include <span>
#include <utility>
#include <vector>

#include "graph.hpp"

int main() {
    using dsalgo::GraphNode;
    using dsalgo::Graph;
    using dsalgo::GraphIndex;

    Graph<int> g;

    auto n0 = g.create_node(42);
    auto n1 = g.create_node(7);
    auto n2 = g.create_node(99, std::numeric_limits<GraphIndex>::max());

    if (!n0 || !n1 || !n2) {
        std::println("Failed to create nodes");
        return 1;
    }

    auto add_edge = [](GraphNode<int>* from, GraphIndex to) -> bool {
        auto r = from->add_neighbor(to);
        if (!r) {
            std::println(
                "add_neighbor(from={}, to={}) failed: {}",
                from->get_idx(),
                to,
                static_cast<int>(r.error())
            );
            return false;
        }
        return true;
    };

    if (!add_edge(n0.value(), n1.value()->get_idx())) return 1;
    if (!add_edge(n0.value(), n2.value()->get_idx())) return 1;
    if (!add_edge(n1.value(), n2.value()->get_idx())) return 1;
    if (!add_edge(n2.value(), n0.value()->get_idx())) return 1;

    for (GraphIndex i = 0; i < 3; ++i) {
        (void)g.create_node(static_cast<int>(i));
    }

    g.print();

    return 0;
}