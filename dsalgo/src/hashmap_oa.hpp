// dsalgo/src/hashmap_oa.hpp
#pragma once
#include "array.hpp"
#include "hashmap_chained.hpp"
#include "list.hpp"
#include "util.hpp"

#include <bit>
#include <cstdint>
#include <limits>
#include <print>

using namespace dsalgo;

template <Hashable K, typename V, usize N>
class HashmapOA
{
    // TODO: Consider using backward shift to clean up tombstone accum as we don't do resizing
    static_assert(is_power_of_two(N), "N must be a power of two");
    static_assert(std::is_trivially_copyable_v<K>, "K must be trivially copyable");
    static_assert(std::is_trivially_copyable_v<V>, "V must be trivially copyable");

public:
    using key_type = K;
    using mapped_type = V;

    static constexpr u8 ctrl_empty = 0x80;
    static constexpr u8 ctrl_tombstone = 0xFE;
    static constexpr usize tomb_not_set = std::numeric_limits<usize>::max();

    HashmapOA() : m_ctrl_block(ctrl_empty) {}

    bool insert(const K &key, const V &value)
    {
        const u64 hash = hash_int(key);
        const u8 hash_ctrl = static_cast<u8>(hash & 0x7F);
        usize idx = static_cast<usize>(hash) & mask;
        const usize idx_start = idx;

        usize first_tomb_idx = tomb_not_set;

        do
        {
            const u8 ctrl = m_ctrl_block[idx];
            if (ctrl == ctrl_empty)
            {
                const usize tomb_idx = (first_tomb_idx != tomb_not_set) ? first_tomb_idx : idx;
                insert_at_idx_(key, value, hash_ctrl, tomb_idx, tomb_idx == first_tomb_idx);
                return true;
            }
            if (ctrl == hash_ctrl && m_keys[idx] == key)
            {
                m_values[idx] = value;
                return false;
            }
            if (ctrl == ctrl_tombstone && (first_tomb_idx == tomb_not_set)) first_tomb_idx = idx;

            idx = (idx + 1) & mask;
        } while (idx != idx_start);
        if (first_tomb_idx != tomb_not_set)
        {
            insert_at_idx_(key, value, hash_ctrl, first_tomb_idx, true);
            return true;
        }
        return false;
    }

    [[nodiscard]] V *find(const K &key)
    {
        const usize idx = find_index_(key);
        return (idx == tomb_not_set) ? nullptr : &m_values[idx];
    }

    [[nodiscard]] const V *find(const K &key) const
    {
        const usize idx = find_index_(key);
        return (idx == tomb_not_set) ? nullptr : &m_values[idx];
    }

    bool erase(const K &key)
    {
        const usize idx = find_index_(key);
        if (idx == tomb_not_set) return false;
        m_ctrl_block[idx] = ctrl_tombstone;
        --m_size;
        ++m_tombstones;
        return true;
    }

    [[nodiscard]] bool contains(const K &key) const { return find(key) != nullptr; }

    [[nodiscard]] double get_occupancy() const
    {
        return static_cast<double>(m_size) / static_cast<double>(m_ctrl_block.get_size());
    }

private:
    Array<u8, N> m_ctrl_block;
    Array<K, N> m_keys;
    Array<V, N> m_values;

    static constexpr usize mask = N - 1;

    usize m_size = 0;
    usize m_tombstones = 0;

    [[nodiscard]] usize find_index_(const K &key) const noexcept
    {
        const u64 hash = hash_int(key);
        const u8 hash_ctrl = static_cast<u8>(hash & 0x7F);
        usize idx = static_cast<usize>(hash) & mask;
        const usize idx_start = idx;

        do
        {
            const u8 ctrl = m_ctrl_block[idx];
            if (ctrl == ctrl_empty) return tomb_not_set;
            if (ctrl == hash_ctrl && m_keys[idx] == key) return idx;
            idx = (idx + 1) & mask;
        } while (idx != idx_start);

        return tomb_not_set;
    }

    void insert_at_idx_(const K &key, const V &value, u8 hash_ctrl, usize idx, bool remove_tomb)
    {
        m_ctrl_block[idx] = hash_ctrl;
        m_keys[idx] = key;
        m_values[idx] = value;
        ++m_size;
        if (remove_tomb) --m_tombstones;
    }
};