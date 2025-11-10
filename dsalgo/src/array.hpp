// dsalgo/src/array.hpp
#pragma once
#include "types.hpp"

namespace dsalgo
{
template <class T, usize N>
class Array
{
public:
    [[nodiscard]] static constexpr usize get_size_static() noexcept { return N; }
    [[nodiscard]] constexpr usize get_size() const noexcept { return N; }

    [[nodiscard]] constexpr T &operator[](usize i) noexcept { return data[i]; }
    [[nodiscard]] constexpr const T &operator[](usize i) const noexcept { return data[i]; }

    [[nodiscard]] constexpr T *begin() noexcept { return data; }
    [[nodiscard]] constexpr T *end() noexcept { return data + N; }
    [[nodiscard]] constexpr const T *begin() const noexcept { return data; }
    [[nodiscard]] constexpr const T *end() const noexcept { return data + N; }

    constexpr void fill(const T &v) noexcept
    {
        for (auto &x : data)
        {
            x = v;
        }
    }
    [[nodiscard]] constexpr T *raw() noexcept { return data; }
    [[nodiscard]] constexpr const T *raw() const noexcept { return data; }

private:
    T data[N]{};
};
} // namespace dsalgo