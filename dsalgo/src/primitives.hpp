// dsalgo/src/primitives.hpp
#pragma once
#include <type_traits>

#include "types.hpp"

namespace dsalgo
{
struct alignas(16) Vec3f
{
    float x, y, z;
    friend constexpr bool operator==(const Vec3f &lhs, const Vec3f &rhs) noexcept
    {
        return lhs.x == rhs.x && lhs.y == rhs.y && lhs.z == rhs.z;
    }
};

struct alignas(16) Vec3d
{
    double x, y, z;
    friend constexpr bool operator==(const Vec3d &lhs, const Vec3d &rhs) noexcept
    {
        return lhs.x == rhs.x && lhs.y == rhs.y && lhs.z == rhs.z;
    }
};

struct alignas(16) Vec4f
{
    float x, y, z, w;
    friend constexpr bool operator==(const Vec4f &lhs, const Vec4f &rhs) noexcept
    {
        return lhs.x == rhs.x && lhs.y == rhs.y && lhs.z == rhs.z && lhs.w == rhs.w;
    }
};

struct alignas(16) Vec4d
{
    double x, y, z, w;
    friend constexpr bool operator==(const Vec4d &lhs, const Vec4d &rhs) noexcept
    {
        return lhs.x == rhs.x && lhs.y == rhs.y && lhs.z == rhs.z && lhs.w == rhs.w;
    }
};

struct alignas(4) ColorRGB
{
    u8 r, g, b;
    friend constexpr bool operator==(const ColorRGB &lhs, const ColorRGB &rhs) noexcept
    {
        return lhs.r == rhs.r && lhs.g == rhs.g && lhs.b == rhs.b;
    }
};

struct alignas(4) ColorRGBA
{
    u8 r, g, b, a;
    friend constexpr bool operator==(const ColorRGBA &lhs, const ColorRGBA &rhs) noexcept
    {
        return lhs.r == rhs.r && lhs.g == rhs.g && lhs.b == rhs.b && lhs.a == rhs.a;
    }
};

static_assert(std::is_trivially_copyable_v<Vec3f>);
static_assert(std::is_trivially_copyable_v<Vec4f>);
static_assert(std::is_trivially_copyable_v<ColorRGB>);
static_assert(std::is_trivially_copyable_v<ColorRGBA>);
}