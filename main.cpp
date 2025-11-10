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

    size_t get_length() const noexcept { return m_buckets.size(); }
    size_t get_mask() const noexcept { return static_cast<size_t>(get_length() - 1); }

    void insert(K key, V value)
    {
        std::println("Inserting [{}] := {}", key, value);
        u64 hashed = hash_int(key);
        std::println("Hash of key is {}", hashed);
        size_t bucket_id = hashed & get_mask();
        std::println("BucketID is {}", bucket_id);
        m_buckets[bucket_id].emplace_back(key, value);
        std::println("Pushed node [{},{}] into bucket {}", key, value, bucket_id);
    }

private:
    std::vector<std::vector<HashMapNode<K, V>>> m_buckets;
};

int main()
{
    HashMap<uint32_t, double> hm{5};
    hm.pprint();
    hm.insert(5, 3);
    hm.pprint();
}