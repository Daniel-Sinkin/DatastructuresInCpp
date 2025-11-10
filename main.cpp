// main.cpp
#include "util.hpp"
#include <bit>
#include <cstdint>
#include <print>
#include <vector>

using namespace dsalgo;

constexpr size_t round_power_of_2_up(size_t v) { return std::bit_ceil(v == 0 ? 1zu : v); }

template <Hashable K, typename V>
struct HashMapNode
{
    K key;
    V value;
};

template <Hashable K, typename V>
class HashMap
{
public:
    HashMap(size_t n_buckets) : m_buckets(round_power_of_2_up(n_buckets)) {}
    HashMap() : m_buckets(64zu) {}
    std::vector<std::vector<HashMapNode<K, V>>> m_buckets;

    size_t get_length() { return m_buckets.size(); }

    void pprint() const
    {
        std::println("HashMap with {} buckets:", m_buckets.size());
        for (size_t i = 0; i < m_buckets.size(); ++i)
        {
            std::print("  [{}] ", i);
            if (m_buckets[i].empty())
            {
                std::println("<empty>");
                continue;
            }
            for (const auto &node : m_buckets[i])
            {
                std::print("({}: {:.3f}) ", node.key, node.value);
            }
            std::println("");
        }
    }

    void insert(K key, V value)
    {
        std::println("Inserting [{}] := {}", key, value);
        std::println("Hash of key is {}", hash_int(key));
    }

private:
};

int main()
{
    HashMap<uint32_t, double> hm{5};
    hm.pprint();
    hm.insert(5, 3);
}