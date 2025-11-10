// main.cpp
#include "util.hpp"
#include <print>

class HashMap
{
};

int main()
{
    std::println("{}", static_cast<uint32_t>(hash_int64(static_cast<uint64_t>(12312))));
}