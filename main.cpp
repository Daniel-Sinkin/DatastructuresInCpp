// main.cpp
#include "array.hpp"
#include "hashmap.hpp"
#include "list.hpp"
#include "util.hpp"

#include <bit>
#include <cstdint>
#include <print>

using namespace dsalgo;

template <Hashable K, typename V, usize N>
class HashmapOA
{
    static_assert(is_power_of_two(N), "N must be a power of two");
    static_assert(std::is_trivially_copyable_v<K>, "K must be trivially copyable");
    static_assert(std::is_trivially_copyable_v<V>, "V must be trivially copyable");

public:
    static constexpr u8 ctrl_empty = 0x80;
    static constexpr u8 ctrl_tombstone = 0xFE;
    static constexpr usize tomb_not_set = std::numeric_limits<usize>::max();

    HashmapOA() : m_ctrl_block(ctrl_empty) {}

    [[nodiscard]] usize key_to_idx(const K &key) const { return static_cast<usize>(hash_int(key)) & (N - 1); }

    bool insert(K key, V value)
    {
        const u64 hash = hash_int(key);
        const u8 hash_ctrl = static_cast<u8>(hash & 0x7F);
        const usize mask = N - 1;
        usize idx = static_cast<usize>(key) & mask;

        usize first_tomb = tomb_not_set;

        while (true)
        {
            const u8 ctrl = m_ctrl_block[idx];
            if (ctrl == ctrl_empty)
            {
                const usize tomb_idx = (first_tomb != tomb_not_set) ? first_tomb : idx;
                m_ctrl_block[tomb_idx] = hash_ctrl;
                m_keys[tomb_idx] = key;
                m_values[tomb_idx] = value;
                ++m_size;
                if (tomb_idx == first_tomb) --m_tombstones;
                return true;
            }
            if (ctrl == hash_ctrl && m_keys[idx] == key)
            {
                m_values[idx] = value;
                return false;
            }
            if (ctrl == ctrl_tombstone && (first_tomb == tomb_not_set)) first_tomb = idx;

            idx = (idx + 1) & mask;
        }
    }

private:
    Array<u8, N> m_ctrl_block;
    Array<K, N> m_keys;
    Array<V, N> m_values;

    usize m_size = 0;
    usize m_tombstones = 0;
};

int main()
{
}