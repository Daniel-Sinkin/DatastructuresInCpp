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

auto running_average(double running, int new_value, usize size) -> double {

    // Let (a_i : 1 <= i <= n) be a sequence of numbers and let
    // avg(k) = sum_{i = 1}^k a_i
    // denote the running average.
    // Then
    //  (Base Case)          avg(1) = a_1 (Base Case)
    //  (Induction Step) avg(k + 1) = (sum_{i = 1}^{k + 1} a_i) / (k + 1)
    //                              = (sum_{i = 1}^{k} a_i + a_{k + 1}) / (k + 1)
    //                              = (sum_{i = 1}^{k} a_i / k * k / (k + 1) + a_{k + 1} / (k + 1)
    //                              = (avg(k) * k + a_{k + 1}) / (k + 1)
    // As such the identity holds inductively
    // Examples:
    // avg(1) = a_1
    // avg(2) = (a_1 + a_2) / 2
    //        = (a_1 / 1 * 1 + a_2) / 2
    //        = (avg(1) * 1 + a_2) / 2
    // avg(3) = (a_1 + a_2 + a_3) / 3
    //        = ((a_1 + a_2) / 2 * 2 + a_3) / 3
    //        = (avg(2) * 2 + a_3) / 3
    auto size_d = static_cast<double>(size);
    auto new_value_d = static_cast<double>(new_value);
    return (running * size_d + new_value_d) / (size_d + 1.0);
}

auto countResponseTimeRegressions(std::vector<int> responseTimes) -> int {
    auto count = 0;
    auto current_average = 0.0;
    for (usize i{0}; i < responseTimes.size(); ++i) {
        const auto curr = responseTimes[i];
        current_average = running_average(current_average, curr, i);
        if (curr > current_average) {
            ++count;
        }
    }
    return count;
}

int main() {
}
