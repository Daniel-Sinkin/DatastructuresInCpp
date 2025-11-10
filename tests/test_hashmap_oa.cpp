// tests/test_hashmap_oa.cpp
#include "common.hpp"
#include "hashmap_oa.hpp"
#include "util.hpp"

#include <type_traits>

namespace dsalgo::Test
{

// Helpers
template <class M>
static void expect_missing_then_present(M &m, typename M::key_type key, typename M::mapped_type val)
{
    EXPECT_TRUE(m.find(key) == nullptr);
    EXPECT_TRUE(!m.contains(key));
    EXPECT_TRUE(m.insert(key, val)); // new insert
    EXPECT_TRUE(m.contains(key));
    auto *p = m.find(key);
    EXPECT_TRUE(p != nullptr);
    EXPECT_EQ(*p, val);
    const auto &cm = m;
    const auto *cp = cm.find(key);
    EXPECT_TRUE(cp != nullptr);
    EXPECT_EQ(*cp, val);
}

template <typename K, usize N>
static K find_key_with_bucket(usize target_bucket, K start = K{1})
{
    static_assert(std::is_same_v<K, u32> || std::is_same_v<K, u64> || std::is_same_v<K, usize>);
    constexpr usize mask = N - 1;
    for (K k = start;; ++k)
    {
        const u64 h = hash_int(k);
        const usize b = static_cast<usize>(h) & mask;
        if (b == target_bucket) return k;
    }
}

template <typename K>
static u8 ctrl_byte(K k) // lower 7 bits used by HashmapOA
{
    return static_cast<u8>(hash_int(k) & 0x7F);
}

// 1) Basic API and empty invariants
static void test_empty_api_and_occupancy()
{
    using K = u64;
    using V = u32;
    constexpr usize N = 8zu;
    HashmapOA<K, V, N> m;

    EXPECT_TRUE(m.get_occupancy() == 0.0);
    EXPECT_TRUE(m.find(K{0}) == nullptr);
    EXPECT_TRUE(!m.contains(K{0}));
    EXPECT_TRUE(!m.erase(K{0}));
}

// 2) Insert, find, overwrite same key, const find
static void test_insert_find_overwrite()
{
    using K = u32;
    using V = u64;
    constexpr usize N = 16zu;
    HashmapOA<K, V, N> m;

    expect_missing_then_present(m, K{10}, V{111});
    expect_missing_then_present(m, K{77}, V{222});
    expect_missing_then_present(m, K{1234}, V{333});

    // overwrite existing key returns false, value updated, occupancy unchanged
    EXPECT_TRUE(!m.insert(K{77}, V{999}));
    auto *p = m.find(K{77});
    EXPECT_TRUE(p && *p == V{999});
}

// 3) Fill to capacity, fail on extra insert, erase and reuse tombstones
static void test_fill_capacity_then_reuse()
{
    using K = usize;
    using V = usize;
    constexpr usize N = 8zu;
    HashmapOA<K, V, N> m;

    // Insert distinct keys until insertion fails. Should succeed exactly N times.
    usize inserted = 0zu;
    for (K k = 1;; ++k)
    {
        const bool ok = m.insert(k, k + 1);
        if (!ok) break;
        ++inserted;
        EXPECT_TRUE(m.get_occupancy() <= 1.0);
    }
    EXPECT_EQ(inserted, N);
    EXPECT_TRUE(m.get_occupancy() == 1.0);

    // All inserted keys must be findable
    for (K k = 1; k <= inserted; ++k)
    {
        auto *p = m.find(k);
        EXPECT_TRUE(p && *p == k + 1);
        EXPECT_TRUE(m.contains(k));
    }

    // Extra insert should fail when table has no empties or tombstones
    EXPECT_TRUE(!m.insert(K{99999}, V{123}));

    // Erase three existing keys to create tombstones
    EXPECT_TRUE(m.erase(K{2}));
    EXPECT_TRUE(m.erase(K{4}));
    EXPECT_TRUE(m.erase(K{6}));

    // Occupancy reflects live entries only
    const double occ = m.get_occupancy();
    EXPECT_TRUE(occ > 0.0);
    EXPECT_TRUE(occ < 1.0);

    // Re-insert three new distinct keys. Must succeed by reusing tombstones.
    EXPECT_TRUE(m.insert(K{100001}, V{1}));
    EXPECT_TRUE(m.insert(K{100003}, V{3}));
    EXPECT_TRUE(m.insert(K{100005}, V{5}));

    // Table should be full again. Next insert fails.
    EXPECT_TRUE(!m.insert(K{100007}, V{7}));
}

// 4) N == 1 stress: enforce maximal collisions, tombstone reuse path
static void test_n_eq_1_tombstone_reuse()
{
    using K = u64;
    using V = u32;
    constexpr usize N = 1zu; // mask = 0, every key probes the same slot
    HashmapOA<K, V, N> m;

    EXPECT_TRUE(m.insert(K{11}, V{111}));
    EXPECT_TRUE(m.contains(K{11}));
    EXPECT_TRUE(!m.insert(K{22}, V{222})); // cannot insert when slot occupied and no tombstone
    EXPECT_TRUE(m.erase(K{11}));           // create tombstone
    EXPECT_TRUE(!m.erase(K{11}));          // second erase is no-op
    EXPECT_TRUE(m.insert(K{22}, V{222}));  // must reuse tombstone
    EXPECT_TRUE(m.find(K{22}) && *m.find(K{22}) == V{222});
    EXPECT_TRUE(m.find(K{11}) == nullptr);
}

// 5) Probe and wrap-around: cluster starting near end of table
static void test_wraparound_and_find_through_tombstones()
{
    using K = usize;
    using V = u32;
    constexpr usize N = 8zu;
    HashmapOA<K, V, N> m;

    const usize bucket = N - 1; // 7
    // Find three distinct keys that start at bucket 7. Prefer different ctrl bytes to avoid overwrite.
    K k1 = find_key_with_bucket<K, N>(bucket, K{1});
    K k2 = find_key_with_bucket<K, N>(bucket, k1 + 1);
    while (ctrl_byte(k2) == ctrl_byte(k1))
        k2 = find_key_with_bucket<K, N>(bucket, k2 + 1);
    K k3 = find_key_with_bucket<K, N>(bucket, k2 + 1);
    while (ctrl_byte(k3) == ctrl_byte(k2) || ctrl_byte(k3) == ctrl_byte(k1))
        k3 = find_key_with_bucket<K, N>(bucket, k3 + 1);

    // Insert in order. Expected placement:
    // k1 -> idx 7
    // k2 -> wraps to idx 0
    // k3 -> idx 1
    EXPECT_TRUE(m.insert(k1, V{1}));
    EXPECT_TRUE(m.insert(k2, V{2}));
    EXPECT_TRUE(m.insert(k3, V{3}));

    // All findable
    EXPECT_TRUE(m.find(k1) && *m.find(k1) == V{1});
    EXPECT_TRUE(m.find(k2) && *m.find(k2) == V{2});
    EXPECT_TRUE(m.find(k3) && *m.find(k3) == V{3});

    // Erase middle of probe chain (k2). Leaves a tombstone between k1 and k3.
    EXPECT_TRUE(m.erase(k2));
    EXPECT_TRUE(!m.contains(k2));
    // k3 must still be findable across tombstone
    EXPECT_TRUE(m.find(k3) && *m.find(k3) == V{3});

    // Reinsert a new key that starts at bucket 7. Should reuse the tombstone slot.
    K k4 = find_key_with_bucket<K, N>(bucket, k3 + 1);
    while (ctrl_byte(k4) == ctrl_byte(k1) || ctrl_byte(k4) == ctrl_byte(k3))
        k4 = find_key_with_bucket<K, N>(bucket, k4 + 1);
    EXPECT_TRUE(m.insert(k4, V{4}));
    EXPECT_TRUE(m.find(k4) && *m.find(k4) == V{4});
}

// 6) Negative lookups stop at empty slot
static void test_find_negative_stops_at_empty()
{
    using K = u32;
    using V = u32;
    constexpr usize N = 32zu;
    HashmapOA<K, V, N> m;

    // Insert a handful of keys
    for (K k = 1; k <= 10; ++k)
        EXPECT_TRUE(m.insert(k, k + 100));

    // Probe for many non-inserted keys. All should be missing.
    for (K k = 1000; k < 1100; ++k)
        EXPECT_TRUE(m.find(k) == nullptr);
}

// 7) Overwrite does not change occupancy
static void test_overwrite_keeps_occupancy()
{
    using K = usize;
    using V = u64;
    constexpr usize N = 64zu;
    HashmapOA<K, V, N> m;

    EXPECT_TRUE(m.insert(123, 1));
    const double occ_before = m.get_occupancy();
    EXPECT_TRUE(!m.insert(123, 2)); // overwrite
    EXPECT_TRUE(m.find(123) && *m.find(123) == 2);
    const double occ_after = m.get_occupancy();
    EXPECT_TRUE(occ_before == occ_after);
}

} // namespace dsalgo::Test

int main()
{
    using namespace dsalgo::Test;
    test_empty_api_and_occupancy();
    test_insert_find_overwrite();
    test_fill_capacity_then_reuse();
    test_n_eq_1_tombstone_reuse();
    test_wraparound_and_find_through_tombstones();
    test_find_negative_stops_at_empty();
    test_overwrite_keeps_occupancy();
    return 0;
}