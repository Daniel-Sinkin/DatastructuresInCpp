// tests/common.hpp
#pragma once
#include "types.hpp"

#include <cmath>
#include <stdexcept>
#include <type_traits>
#include <utility>

namespace dsalgo::Test
{
using namespace dsalgo;

inline void expect(bool ok, const char *msg)
{
    if (!ok) throw std::runtime_error(msg);
}

template <class F>
inline void expect_exception(F &&fn, const char *msg)
{
    static_assert(std::is_invocable_v<F>);
    bool thrown = false;
    try
    {
        std::forward<F>(fn)();
    }
    catch (...)
    {
        thrown = true;
    }
    if (!thrown) throw std::runtime_error(msg);
}

template <typename T>
constexpr bool nearly_equal(T a, T b, T rel_eps, T abs_eps) noexcept
{
    static_assert(std::is_floating_point_v<T>);
    const T diff = std::abs(a - b);
    if (diff <= abs_eps) return true;
    return diff <= rel_eps * std::max(std::abs(a), std::abs(b));
}

#define EXPECT_TRUE(expr)                                             \
    do                                                                \
    {                                                                 \
        if (!(expr)) throw std::runtime_error("EXPECT_TRUE: " #expr); \
    } while (0)

#define EXPECT_EQ(a, b)                                                          \
    do                                                                           \
    {                                                                            \
        if (!((a) == (b))) throw std::runtime_error("EXPECT_EQ: " #a " == " #b); \
    } while (0)

/* Float comparison (absolute + relative tolerance) */
#define EXPECT_NEAR(a, b)                                                            \
    do                                                                               \
    {                                                                                \
        using _T = std::decay_t<decltype(a)>;                                        \
        static_assert(std::is_floating_point_v<_T>, "EXPECT_NEAR requires floats"); \
        if (!::dsalgo::Test::nearly_equal(                                           \
                static_cast<_T>(a),                                                  \
                static_cast<_T>(b),                                                  \
                static_cast<_T>(1e-6),                                               \
                static_cast<_T>(1e-12)))                                             \
        {                                                                            \
            throw std::runtime_error("EXPECT_NEAR: " #a " ~= " #b);                  \
        }                                                                            \
    } while (0)

#define EXPECT_THROW(expr)                                         \
    do                                                             \
    {                                                              \
        bool _t = false;                                           \
        try                                                        \
        {                                                          \
            (void)(expr);                                          \
        }                                                          \
        catch (...)                                                \
        {                                                          \
            _t = true;                                             \
        }                                                          \
        if (!_t) throw std::runtime_error("EXPECT_THROW: " #expr); \
    } while (0)

#define EXPECT_NO_THROW(expr)                                    \
    do                                                           \
    {                                                            \
        try                                                      \
        {                                                        \
            (void)(expr);                                        \
        }                                                        \
        catch (...)                                              \
        {                                                        \
            throw std::runtime_error("EXPECT_NO_THROW: " #expr); \
        }                                                        \
    } while (0)

} // namespace dsalgo::Test