// main.cpp

#include <cassert>            // IWYU pragma: keep
#include <chrono>             // IWYU pragma: keep
#include <condition_variable> // IWYU pragma: keep
#include <cstddef>            // IWYU pragma: keep
#include <cstdint>            // IWYU pragma: keep
#include <cstdlib>            // IWYU pragma: keep
#include <exception>          // IWYU pragma: keep
#include <functional>         // IWYU pragma: keep
#include <iostream>           // IWYU pragma: keep
#include <iterator>           // IWYU pragma: keep
#include <limits>             // IWYU pragma: keep
#include <map>                // IWYU pragma: keep
#include <memory>             // IWYU pragma: keep
#include <mutex>              // IWYU pragma: keep
#include <numeric>            // IWYU pragma: keep
#include <optional>           // IWYU pragma: keep
#include <print>              // IWYU pragma: keep
#include <queue>              // IWYU pragma: keep
#include <random>             // IWYU pragma: keep
#include <span>               // IWYU pragma: keep
#include <stdexcept>          // IWYU pragma: keep
#include <stdlib.h>           // IWYU pragma: keep
#include <string>             // IWYU pragma: keep
#include <thread>             // IWYU pragma: keep
#include <type_traits>        // IWYU pragma: keep
#include <vector>             // IWYU pragma: keep

using usize = std::size_t;
using u32 = std::uint32_t;
using u16 = std::uint16_t;
using u8 = std::uint8_t;

[[maybe_unused]] constexpr bool k_do_print{true};

// clang-format off
#define DS_DBG(x) if constexpr (k_do_print) std::cout << #x << " = " << x << "\n";
// clang-format on

template <typename T>
concept Streamable = requires(T a, std::ostream &ostream) {
    { ostream << a };
};

template <Streamable T>
auto print(const std::vector<T> &xs, std::string_view name) -> void {
    std::cout << name << " = [";
    for (usize i{0zu}; i < xs.size(); ++i) {
        if (i > 0)
            std::cout << ", ";
        std::cout << xs[i];
    }
    std::cout << "]\n";
}

// clang-format off
#define DS_DBG_ARR(xs) if constexpr (k_do_print) ::print((xs), #xs);
// clang-format on

enum class ESlotMetadata : u8 {
    empty = 0,
    filled = 1,
    tombstone = 2
};

template <typename T>
concept Hashable = requires(T a) {
    { std::hash<T>{}(a) } -> std::convertible_to<std::size_t>;
};

template <Hashable Key, typename Value>
class OpenAddressingHashMap {
public:
    using SizeT = std::uint32_t;

    OpenAddressingHashMap() {
        data_.resize(k_initial_capacity);
        keys_.resize(k_initial_capacity);
        metadata_.assign(k_initial_capacity, ESlotMetadata::empty);
    }

    auto insert(const Key &key, const Value &value) -> void {
        if (num_elements_ + 1 > data_.size() / 2) {
            resize_and_rehash();
        }
        insert_no_resize(key, value);
    }

    auto erase(const Key &key) -> bool {
        auto idx = static_cast<SizeT>(std::hash<Key>{}(key));
        while (metadata_[(idx & mask())] != ESlotMetadata::empty) {
            if (filled_with_key(key, idx)) {
                metadata_[idx & mask()] = ESlotMetadata::tombstone;
                --num_elements_;
                return true;
            }
            ++idx;
        }
        return false;
    }

    auto get(const Key &key) -> Value & {
        auto idx = static_cast<SizeT>(std::hash<Key>{}(key));
        while (metadata_[idx & mask()] != ESlotMetadata::empty) {
            if (filled_with_key(key, idx)) {
                return data_[idx & mask()];
            }
            ++idx;
        }
        throw std::out_of_range("Key does not exist");
    }

    auto get(const Key &key) const -> const Value & {
        auto idx = static_cast<SizeT>(std::hash<Key>{}(key));
        while (metadata_[idx & mask()] != ESlotMetadata::empty) {
            if (filled_with_key(key, idx)) {
                return data_[idx & mask()];
            }
            ++idx;
        }
        throw std::out_of_range("Key does not exist");
    }

    auto contains(const Key &key) const -> bool {
        auto idx = static_cast<SizeT>(std::hash<Key>{}(key));
        while (metadata_[idx & mask()] != ESlotMetadata::empty) {
            if (filled_with_key(key, idx)) {
                return true;
            }
            ++idx;
        }
        return false;
    }

    auto get_size() const -> SizeT {
        return num_elements_;
    }

    auto get_capacity() const -> usize {
        return data_.size();
    }

    auto print() const -> void {
        std::cout << "mdata = [";
        for (usize i{0zu}; i < metadata_.size(); ++i) {
            if (i > 0) {
                std::cout << ", ";
            }
            switch (metadata_[i]) {
            case ESlotMetadata::empty:
                std::cout << ' ';
                break;
            case ESlotMetadata::filled:
                std::cout << '*';
                break;
            case ESlotMetadata::tombstone:
                std::cout << 't';
                break;
            }
        }
        std::cout << "]\n";
        DS_DBG_ARR(data_);
    }

    auto is_validate() const -> bool {
        const auto cap = get_capacity();
        return (cap & (cap - 1)) == 0zu;
    }

private:
    // We start with 16 and only double so we always have power of 2 capacity
    static constexpr usize k_initial_capacity = 16zu;
    std::vector<Value> data_;
    std::vector<ESlotMetadata> metadata_;
    std::vector<Key> keys_;
    SizeT num_elements_{0zu};

    static constexpr SizeT k_max_size = std::numeric_limits<SizeT>::max() / 2;

    [[nodiscard]] inline auto mask() const noexcept -> SizeT {
        // Power of two has bit pattern 100...00, if you subtract 1 you get
        // 011...11 if you bitwise and with this you get the same as % size
        // i.e. if x = 2^k then (a % x) == a & (x - 1), this saves the division
        // that would be done in module (which is too expensive for a cheap op like idx access
        return static_cast<SizeT>(get_capacity() - 1);
    }

    [[nodiscard]] auto filled_with_key(const Key &key, SizeT idx) const noexcept -> bool {
        return (metadata_[idx & mask()] == ESlotMetadata::filled && keys_[idx & mask()] == key);
    }

    auto insert_no_resize(const Key &key, const Value &value) -> void {
        auto idx = static_cast<SizeT>(std::hash<Key>{}(key));
        // Skip tombstone and filled
        while (metadata_[(idx & mask())] == ESlotMetadata::filled && keys_[idx & mask()] != key) {
            ++idx;
        }
        const auto was_already_filled = filled_with_key(key, idx);
        metadata_[idx & mask()] = ESlotMetadata::filled;
        data_[idx & mask()] = value;
        keys_[idx & mask()] = key;
        if (!was_already_filled) {
            ++num_elements_;
        }
    }

    auto resize_and_rehash() -> void {
        if (data_.size() >= k_max_size) {
            throw std::runtime_error("Max Size overflow");
        }
        const auto old_size = data_.size();
        const auto new_size = 2 * old_size;
        // Make copy before resizing so we can re-insert
        const auto data_old = data_;
        const auto keys_old = keys_;
        const auto metadata_old = metadata_;

        data_.resize(new_size);
        keys_.resize(new_size);
        metadata_.assign(new_size, ESlotMetadata::empty);

        num_elements_ = SizeT{0};

        for (usize i{0zu}; i < old_size; ++i) {
            if (metadata_old[i] == ESlotMetadata::filled) {
                insert_no_resize(keys_old[i], data_old[i]);
            }
        }
    }
};

int main() {
    OpenAddressingHashMap<std::uint32_t, int> map{};
    map.print();
    std::cout << "\n";

    map.insert(3u, 3);
    map.print();
    std::cout << "\n";

    assert(map.contains(3u));
    assert(!map.contains(4u));

    map.insert(7u, 7);
    map.print();
    std::cout << "\n";

    map.insert(3u, 4);
    map.print();
    std::cout << "\n";

    std::cout << map.get(3u) << "\n";
}
