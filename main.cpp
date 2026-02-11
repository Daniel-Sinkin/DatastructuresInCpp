// main.cpp

#include <cassert>    // IWYU pragma: keep
#include <cstddef>    // IWYU pragma: keep
#include <cstdint>    // IWYU pragma: keep
#include <cstdlib>    // IWYU pragma: keep
#include <exception>  // IWYU pragma: keep
#include <functional> // IWYU pragma: keep
#include <iostream>
#include <iterator> // IWYU pragma: keep
#include <limits>   // IWYU pragma: keep
#include <locale>
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

struct StackEntry {
    int value{};
    bool is_removed{false};
};

class BRStack {
public:
    auto push(int value) -> void {
        const usize idx = data_.size();
        data_.push_back({value, false});
        value_to_idx_[value].push_back(idx);
    }

    auto pop() -> void {
        while (!data_.empty()) {
            const auto top = data_.back();

            const auto entry = data_.back();
            data_.pop_back();
            value_to_idx_[entry.value].pop_back();

            if (!top.is_removed) {
                // break on first alive object deleted
                break;
            }
        }
    }

    auto remove_lower(int value) -> void {
        auto it = value_to_idx_.begin();
        const auto end = value_to_idx_.lower_bound(value);
        while (it != end) {
            for (auto idx : it->second) {
                data_[idx].is_removed = true;
            }
            ++it;
        }
    }

    auto remove_upper(int value) -> void {
        auto it = value_to_idx_.upper_bound(value);
        while (it != value_to_idx_.end()) {
            for (auto &idx : it->second) {
                std::println("remove_upper inner");
                data_[idx].is_removed = true;
            }
            ++it;
        }
    }

    auto print() const -> void {
        for (auto i = data_.size(); i-- > 0;) {
            const auto entry = data_[i];
            if (!entry.is_removed) {
                std::println("<{:3}>", entry.value);
            }
        }
    }

private:
    std::vector<StackEntry> data_{};
    std::map<int, std::vector<usize>> value_to_idx_{};
};

int main() {
    BRStack stack{};
}
