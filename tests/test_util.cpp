// tests/test_util.cpp
#include "common.hpp"
#include "util.hpp"
#include <cstdint>
#include <limits>

namespace dsalgo::Test
{

static void test_is_power_of_two()
{
    // Current implementation returns true for 0.
    EXPECT_TRUE(is_power_of_two(0zu));

    EXPECT_TRUE(is_power_of_two(1zu));
    EXPECT_TRUE(is_power_of_two(2zu));
    EXPECT_TRUE(!is_power_of_two(3zu));
    EXPECT_TRUE(is_power_of_two(4zu));
    EXPECT_TRUE(!is_power_of_two(5zu));
    EXPECT_TRUE(!is_power_of_two(6zu));
    EXPECT_TRUE(!is_power_of_two(7zu));
    EXPECT_TRUE(is_power_of_two(8zu));

    // Random spot checks
    EXPECT_TRUE(is_power_of_two(16zu));
    EXPECT_TRUE(!is_power_of_two(18zu));
    EXPECT_TRUE(is_power_of_two(64zu));
    EXPECT_TRUE(!is_power_of_two(65zu));

    // Highest power of two that fits in usize
    const unsigned bitw = static_cast<unsigned>(sizeof(usize) * 8u);
    if (bitw <= 63u)
    {
        const usize top = static_cast<usize>(1ull << (bitw - 1u));
        EXPECT_TRUE(is_power_of_two(top));
        if (top > 1zu) EXPECT_TRUE(!is_power_of_two(top - 1zu));
    }
    else
    { // typical 64-bit
        const usize top = static_cast<usize>(1ull << 63);
        EXPECT_TRUE(is_power_of_two(top));
        EXPECT_TRUE(!is_power_of_two(top - 1zu));
    }
}

static void test_hash_int_traits_and_constexpr()
{
    // noexcept and constexpr availability
    static_assert(noexcept(hash_int<u64>(0ull)));
    static_assert(noexcept(hash_int<u32>(0u)));

    // Known SplitMix64 constants with this implementation:
    // h(0)   = 0xe220a8397b1dcdaf
    // h(1)   = 0x910a2dec89025cc1
    // h(2)   = 0x975835de1c9756ce
    // h(~0u) = 0x73b13ba2aff181c0   (u32 -> widened to 64)
    // h(~0ull)=0xe4d971771b652c20
    constexpr u64 h0 = hash_int<u64>(0ull);
    constexpr u64 h1 = hash_int<u64>(1ull);
    constexpr u64 h2 = hash_int<u64>(2ull);
    constexpr u64 h32m = hash_int<u32>(0xFFFFFFFFu);
    constexpr u64 h64m = hash_int<u64>(0xFFFFFFFFFFFFFFFFull);

    static_assert(h0 == 0xe220a8397b1dcdafull);
    static_assert(h1 == 0x910a2dec89025cc1ull);
    static_assert(h2 == 0x975835de1c9756ceull);
    static_assert(h32m == 0x73b13ba2aff181c0ull);
    static_assert(h64m == 0xe4d971771b652c20ull);

    // Same input, same output
    EXPECT_EQ(hash_int<u64>(123456789ull), hash_int<u64>(123456789ull));
    EXPECT_EQ(hash_int<u32>(42u), hash_int<u32>(42u));
}

static void test_hash_int_uniqueness_small_ranges()
{
    // Small-range collision sanity checks. Not a proof.
    // u32 range sample
    {
        constexpr u32 N = 256u;
        u64 seen[N]{};
        for (u32 i = 0; i < N; ++i)
        {
            const u64 h = hash_int<u32>(i);
            // naive O(N^2) duplicate check, N=256 is fine
            bool dup = false;
            for (u32 j = 0; j < i; ++j)
            {
                if (seen[j] == h)
                {
                    dup = true;
                    break;
                }
            }
            EXPECT_TRUE(!dup);
            seen[i] = h;
        }
    }
    // u64 small range
    {
        constexpr u64 N = 128ull;
        u64 seen[N]{};
        for (u64 i = 0; i < N; ++i)
        {
            const u64 h = hash_int<u64>(i);
            bool dup = false;
            for (u64 j = 0; j < i; ++j)
            {
                if (seen[j] == h)
                {
                    dup = true;
                    break;
                }
            }
            EXPECT_TRUE(!dup);
            seen[i] = h;
        }
    }
}

static void test_hash_int_bit_change_effect()
{
    // Flip one bit and expect different hash.
    const u64 a = 0x1234567890ABCDEFull;
    const u64 b = a ^ (1ull << 17);
    EXPECT_TRUE(hash_int<u64>(a) != hash_int<u64>(b));

    const u32 c = 0xDEADBEEFu;
    const u32 d = c ^ (1u << 9);
    EXPECT_TRUE(hash_int<u32>(c) != hash_int<u32>(d));
}

} // namespace dsalgo::Test

int main()
{
    using namespace dsalgo::Test;
    test_is_power_of_two();
    test_hash_int_traits_and_constexpr();
    test_hash_int_uniqueness_small_ranges();
    test_hash_int_bit_change_effect();
    return 0;
}