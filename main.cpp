// main.cpp
#include "array.hpp"
#include "hashmap.hpp"
#include "list.hpp"
#include "util.hpp"

#include <bit>
#include <cstdint>
#include <print>

using namespace dsalgo;

int main()
{
    HashMap<std::uint32_t, double, 32> hm;
    hm.insert(5, 3.0);
    hm.insert(5, 6.0);
    hm.insert(37, 9.0);
}