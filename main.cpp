// main.cpp
#include <print>
#include <string>

#include "graph.hpp"

bool fill_nodes(dsalgo::Graph<int> &g)
{
    using namespace dsalgo;
    for(int i : {0, 1, 2, 3}) {
        if (!g.create_node(i)) return false;
    }
    return true;
}

int main()
{
    using namespace dsalgo;
    Graph<int> g{};
    fill_nodes(g);

    g.print();
}