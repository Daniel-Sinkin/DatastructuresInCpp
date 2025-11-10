// main.cpp
#include "list.hpp"
#include "util.hpp"
#include <array>
#include <bit>
#include <cstdint>
#include <print>

using namespace dsalgo;

template <Hashable K, typename V>
struct HashMapNode
{
    K key;
    V value;
};
template <Hashable K, typename V, size_t N>
class HashMap
{
    static_assert((N & (N - 1)) == 0, "Bucket count N must be a power of two");
    using Node = HashMapNode<K, V>;

public:
    HashMap() = default;

    void insert(K key, V value)
    {
        const std::uint64_t hashed = hash_int(key);
        const std::size_t bucket_id = static_cast<std::size_t>(hashed) & (N - 1);
        m_buckets[bucket_id].emplace_back(Node{key, value});
    }

    void pprint() const
    {
        std::println("HashMap with {} buckets:", N);
        for (std::size_t i = 0; i < N; ++i)
        {
            const auto &bucket = m_buckets[i];
            std::print("  [{}] ", i);
            if (bucket.is_empty())
            {
                std::println("<empty>");
                continue;
            }
            for (std::size_t j = 0; j < bucket.get_length(); ++j)
            {
                const auto &node = bucket[j];
                std::print("({}: {:.3f}) ", node.key, node.value);
            }
            std::println("");
        }
    }

    [[nodiscard]] constexpr std::size_t bucket_count() const noexcept { return N; }

private:
    std::array<List<Node>, N> m_buckets;
};

int main()
{
    HashMap<std::uint32_t, double, 32> hm;
    hm.insert(5, 3.0);
    hm.insert(5, 6.0);
    hm.insert(37, 9.0);
    hm.pprint();
}