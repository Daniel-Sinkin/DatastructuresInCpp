// tests/main.cpp
#include "common.hpp"
#include <print>

extern void test_list();

int main()
{
    test_list();
    std::println("All tests passed.");
}