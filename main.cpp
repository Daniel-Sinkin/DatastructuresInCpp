// main.cpp
#include "util.hpp"
#include <bit>
#include <cstdint>
#include <print>
#include <vector>

#include "list.hpp"

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
    HashMap(size_t n_buckets)
    {
        const std::size_t buckets = round_power_of_2_up(n_buckets);
        m_buckets.reserve(buckets);
        for (std::size_t i = 0; i < buckets; ++i)
            m_buckets.emplace_back(List<HashMapNode<K, V>>{});
    }
    HashMap() : m_buckets(64zu) {}

    void pprint() const
    {
        std::println("HashMap with {} buckets:", m_buckets.get_length());
        for (size_t i = 0; i < m_buckets.get_length(); ++i)
        {
            const auto &bucket = m_buckets[i];
            std::print("  [{}] ", i);
            if (m_buckets[i].is_empty())
            {
                std::println("<empty>");
                continue;
            }
            for (size_t j = 0; j < bucket.get_length(); ++j)
            {
                const auto &node = bucket[j];
                std::print("({}: {:.3f}) ", node.key, node.value);
            }
            std::println("");
        }
    }

    size_t get_length() const noexcept { return m_buckets.get_length(); }
    size_t get_mask() const noexcept { return static_cast<size_t>(get_length() - 1); }

    void insert(K key, V value)
    {
        u64 hashed = hash_int(key);
        size_t bucket_id = hashed & get_mask();
        m_buckets[bucket_id].emplace_back(key, value);
    }

private:
    List<List<HashMapNode<K, V>>> m_buckets;
};

int main()
{
}