// dsalgo/src/hashmap_chained.hpp
#pragma once
#include "array.hpp"
#include "list.hpp"
#include "types.hpp"
#include "util.hpp"

namespace dsalgo
{
template <Hashable K, typename V>
struct HashMapChainedNode
{
    K key;
    V value;
};
template <Hashable K, typename V, usize N>
class HashMapChained
{
public:
    using key_type = K;
    using mapped_type = V;

    HashMapChained() = default;

    [[nodiscard]] usize key_to_idx(const K &key) const { return static_cast<usize>(hash_int(key)) & (N - 1); }

    bool insert(const K &key, const V &value)
    {
        Bucket &bucket = m_buckets[key_to_idx(key)];
        for (usize i = 0; i < bucket.get_length(); ++i)
        { // Traverse bucket linearly to check if key already exists
            if (bucket[i].key == key)
            { // overwrite on same key
                bucket[i].value = value;
                return false;
            }
        }
        // Emplace back if key not already availiable
        bucket.emplace_back(Node{key, value});
        return true;
    }

    [[nodiscard]] V *find(const K &key)
    {
        Bucket &bucket = m_buckets[key_to_idx(key)];
        for (usize i = 0; i < bucket.get_length(); ++i)
        {
            if (bucket[i].key == key) return &bucket[i].value;
        }
        return nullptr;
    }

    [[nodiscard]] const V *find(const K &key) const
    {
        const Bucket &bucket = m_buckets[key_to_idx(key)];
        for (usize i = 0; i < bucket.get_length(); ++i)
        {
            if (bucket[i].key == key) return &bucket[i].value;
        }
        return nullptr;
    }

    [[nodiscard]] bool contains(const K &key) const { return find(key) != nullptr; }

    bool remove(const K &key)
    {
        Bucket &bucket = m_buckets[key_to_idx(key)];
        for (usize i = 0; i < bucket.get_length(); ++i)
        { // Traverse bucket linearly to check if the key exists
            if (bucket[i].key == key)
            {
                if (bucket[i].key == key)
                {
                    bucket.pop(i);
                    return true;
                }
            }
        }
        return false;
    }

    void clear()
    {
        for (Bucket &bucket : m_buckets)
        {
            bucket.clear();
        }
    }

    [[nodiscard]] constexpr usize get_bucket_count() const noexcept { return N; }
    [[nodiscard]] usize get_total_count() const noexcept
    {
        usize counter = 0zu;
        for (const Bucket &bucket : m_buckets)
        {
            counter += bucket.get_length();
        }
        return counter;
    }

    [[nodiscard]] usize get_n_empty() const noexcept
    {
        usize n_empty = 0zu;
        for (const Bucket &bucket : m_buckets)
        {
            if (bucket.is_empty())
            {
                ++n_empty;
            }
        }
        return n_empty;
    }
    // Occupancy should be <= 0.75
    [[nodiscard]] double get_occupancy() const noexcept { return 1.0 - static_cast<double>(get_n_empty()) / static_cast<double>(N); }

private:
    static_assert(is_power_of_two(N), "Bucket count N must be a power of two");
    static_assert(std::is_trivially_copyable_v<HashMapChainedNode<K, V>>,
        "K and V must be trivially copyable.");
    using Node = HashMapChainedNode<K, V>;
    using Bucket = List<Node>;

    Array<Bucket, N> m_buckets;
};
} // namespace dsalgo