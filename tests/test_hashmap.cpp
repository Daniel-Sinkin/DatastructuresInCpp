// tests/test_hashmap.cpp
#include "common.hpp"
#include "hashmap.hpp"
#include "primitives.hpp"
#include <type_traits>

namespace dsalgo::Test
{

// Helper to probe internal find paths and invariants without UB.
template <typename M>
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

static void test_basic_insert_find_overwrite_clear()
{
    using K = u32;
    using V = u64;
    constexpr usize N = 16zu;
    dsalgo::HashMap<K, V, N> m;

    // API sanity
    EXPECT_EQ(m.get_bucket_count(), N);
    EXPECT_EQ(m.get_total_count(), 0zu);
    EXPECT_TRUE(m.get_occupancy() == 0.0);

    // insert and find
    expect_missing_then_present(m, K{1u}, V{11u});
    expect_missing_then_present(m, K{2u}, V{22u});
    expect_missing_then_present(m, K{3u}, V{33u});
    EXPECT_EQ(m.get_total_count(), 3zu);

    // overwrite same key -> returns false, value updated, count unchanged
    EXPECT_TRUE(!m.insert(K{2u}, V{222u}));
    auto *p = m.find(K{2u});
    EXPECT_TRUE(p != nullptr);
    EXPECT_EQ(*p, V{222u});
    EXPECT_EQ(m.get_total_count(), 3zu);

    // remove present
    EXPECT_TRUE(m.remove(K{1u}));
    EXPECT_EQ(m.get_total_count(), 2zu);
    EXPECT_TRUE(m.find(K{1u}) == nullptr);
    EXPECT_TRUE(!m.contains(K{1u}));

    // remove missing
    EXPECT_TRUE(!m.remove(K{999u}));
    EXPECT_EQ(m.get_total_count(), 2zu);

    // clear
    m.clear();
    EXPECT_EQ(m.get_total_count(), 0zu);
    EXPECT_TRUE(m.get_occupancy() == 0.0);
}

static void test_all_keys_same_bucket_via_N_eq_1()
{
    // Force collisions deterministically with N == 1.
    using K = u64;
    using V = u32;
    constexpr usize N = 1zu;
    dsalgo::HashMap<K, V, N> m;

    // Every key maps to bucket 0. Exercise linear scan paths.
    for (K k = 10u; k < 20u; ++k)
        EXPECT_TRUE(m.insert(k, static_cast<V>(k + 100u)));

    EXPECT_EQ(m.get_total_count(), 10zu);
    // Overwrite a middle element to exercise "hit and update" branch
    EXPECT_TRUE(!m.insert(15u, static_cast<V>(999u)));
    auto *pv = m.find(15u);
    EXPECT_TRUE(pv && *pv == static_cast<V>(999u));

    // Remove head, tail, and a middle element to cover pop(idx) branches
    EXPECT_TRUE(m.remove(10u)); // head of bucket list at time of call
    EXPECT_TRUE(m.remove(19u)); // tail at some point
    EXPECT_TRUE(m.remove(16u)); // middle
    EXPECT_EQ(m.get_total_count(), 7zu);

    // Remove missing
    EXPECT_TRUE(!m.remove(42u));

    // Find all remaining to touch positive and negative scans
    for (K k = 10u; k < 20u; ++k)
    {
        const bool should_exist = (k != 10u && k != 19u && k != 16u);
        auto *p = m.find(k);
        if (should_exist)
            EXPECT_TRUE(p != nullptr);
        else
            EXPECT_TRUE(p == nullptr);
    }
}

static void test_key_to_idx_bounds_and_occupancy()
{
    using K = usize;
    using V = usize;
    constexpr usize N = 8zu;
    dsalgo::HashMap<K, V, N> m;

    // key_to_idx must map into [0, N-1]
    for (K k = 0zu; k < 100zu; ++k)
    {
        auto idx = m.key_to_idx(k);
        EXPECT_TRUE(idx < N);
    }

    // Occupancy counts non-empty buckets only
    // With N=8 insert exactly into 4 distinct buckets if possible.
    // We cannot control hash distribution exactly, so just insert many and assert bounds.
    for (K k = 0zu; k < 32zu; ++k)
        (void)m.insert(k, k + 1zu);

    const double occ = m.get_occupancy();
    EXPECT_TRUE(occ >= 0.0);
    EXPECT_TRUE(occ <= 1.0);
    EXPECT_TRUE(m.get_total_count() >= 32zu - 0zu); // no overwrites for distinct keys
}

} // namespace dsalgo::Test

int main()
{
    using namespace dsalgo::Test;
    test_basic_insert_find_overwrite_clear();
    test_all_keys_same_bucket_via_N_eq_1();
    test_key_to_idx_bounds_and_occupancy();
    return 0;
}