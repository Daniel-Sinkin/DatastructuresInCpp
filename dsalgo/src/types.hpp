// dsalgo/src/types.hpp
#pragma once
#include <concepts>
#include <cstddef>
#include <cstdint>

#include <glm/glm.hpp>

#if __has_include(<stdfloat>)
#include <stdfloat>
#endif

namespace dsalgo
{
using u8 = std::uint8_t;
using u16 = std::uint16_t;
using u32 = std::uint32_t;
using u64 = std::uint64_t;

#if defined(__cpp_lib_stdfloat)
using f32 = std::float32_t;
using f64 = std::float64_t;
#else
using f32 = float;
using f64 = double;
#endif

using usize = size_t;
using isize = std::ptrdiff_t;

inline constexpr bool use_f64 = true;
using real = std::conditional_t<use_f64, f64, f32>;

using Vec3f = glm::vec3;
using Vec3d = glm::dvec3;
using Vec4f = glm::vec4;
using Vec4d = glm::dvec4;

template <typename T>
concept Hashable = std::same_as<T, u32> || std::same_as<T, u64> || std::same_as<T, usize>;
} // namespace dsalgo