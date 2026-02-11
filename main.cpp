// main.cpp

#include <cassert>     // IWYU pragma: keep
#include <cstddef>     // IWYU pragma: keep
#include <cstdint>     // IWYU pragma: keep
#include <cstdlib>     // IWYU pragma: keep
#include <exception>   // IWYU pragma: keep
#include <functional>  // IWYU pragma: keep
#include <iterator>    // IWYU pragma: keep
#include <limits>      // IWYU pragma: keep
#include <map>         // IWYU pragma: keep
#include <print>       // IWYU pragma: keep
#include <random>      // IWYU pragma: keep
#include <stdexcept>   // IWYU pragma: keep
#include <stdlib.h>    // IWYU pragma: keep
#include <type_traits> // IWYU pragma: keep
#include <vector>      // IWYU pragma: keep

using f32 = float;
using f64 = double;

static_assert(sizeof(f32) == 4);
static_assert(sizeof(f64) == 8);

using u8 = std::uint8_t;
using u16 = std::uint16_t;
using u32 = std::uint32_t;
using u64 = std::uint64_t;

using i8 = std::int8_t;
using i16 = std::int16_t;
using i32 = std::int32_t;
using i64 = std::int64_t;

using Byte = std::byte;

using usize = std::size_t;

using MatrixI32 = std::vector<std::vector<i32>>;

using Matrix = MatrixI32;

auto print(const Matrix &matrix) {
    for (const auto &row : matrix) {
        std::println("{}", row);
    }
}

int main() {
}
