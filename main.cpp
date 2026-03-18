/*
Implement a templated HashMap<K, V> using separate chaining with the following requirements:

x put(const K& key, const V& value) -> void inserts or updates
x get(const K& key) -> V* returns pointer to value, nullptr if not found
x get(const K& key) const -> const V* const overload
x remove(const K& key) -> bool returns true if found and removed
x contains(const K& key) const -> bool
x size() const -> std::size_t number of stored key-value pairs
x empty() const -> bool
x Buckets are a dynamically allocated array of std::forward_list<std::pair<K, V>>
x Use std::hash<K> for hashing, operator== for key comparison
x Initial bucket count 8, must always be a power of 2
x Automatic rehash when load factor (size / bucket_count) exceeds 0.75
x On rehash, double the bucket count and reinsert all elements
x Mask-based indexing (hash & (bucket_count - 1)) instead of modulo
x Destructor, default constructor
x Copy constructor, copy assignment, move constructor, move assignment
x load_factor() const -> float
x bucket_count() const -> std::size_t
Iterator support: a forward iterator that traverses all key-value pairs across all buckets. Must support begin(), end(), operator++, operator*, operator->, operator==/!=. Iteration order does not need to be deterministic.
*/

#include <cassert>
#include <cstddef>
#include <forward_list>
#include <functional>

using usize = std::size_t;

template <typename K>
concept DefaultHashable = requires(K h) {
    { std::hash<K>{}(h) } noexcept -> std::same_as<usize>;
};

template <DefaultHashable K, typename V>
class HashMap {
public:
    using BucketElement = std::pair<K, V>;
    using Bucket = std::forward_list<BucketElement>;

    HashMap() { buckets_.resize(k_initial_bucket_count); }
    ~HashMap() = default;
    HashMap(const HashMap &other) {
        copy_from(other);
    }
    HashMap &operator=(const HashMap &other) {
        if (this != &other) {
            reset();
            copy_from(other);
        }
        return *this;
    }
    HashMap(HashMap &&other) {
        steal_from(std::move(other));
    }
    HashMap &operator=(HashMap &&other) {
        if (this != &other) {
            reset();
            steal_from(std::move(other));
        }
        return *this;
    }

    auto reset() -> void {
        buckets_.clear();
        size_ = 0;
    }

    auto put(K key, V value) -> void {
        auto &bucket = buckets_[idx_(key)];
        auto it = bucket.begin();
        auto it_prev = bucket.before_begin();
        for (; it != bucket.end(); ++it, ++it_prev) {
            if (it->first == key) {
                it->second = std::move(value);
                return;
            }
        }
        bucket.insert_after(it_prev, {std::move(key), std::move(value)});
        ++size_;
        if (load_factor() > k_max_load_factor) {
            rehash();
        }
    }
    [[nodiscard]] auto load_factor() const noexcept -> float {
        return static_cast<float>(size_) / static_cast<float>(buckets_.size());
    }
    auto rehash() -> void {
        [[maybe_unused]] const auto old_count = size_;
        std::vector<Bucket> old_buckets{};
        std::swap(buckets_, old_buckets);
        buckets_.resize(old_buckets.size() * 2zu);
        size_ = 0;
        for (auto &bucket : old_buckets) {
            for (auto &[k, v] : bucket) {
                put(std::move(k), std::move(v));
            }
        }
        assert(old_count == size_);
    }
    [[nodiscard]] auto get(const K &key) -> V * {
        auto &bucket = buckets_[idx_(key)];
        for (auto it = bucket.begin(); it != bucket.end(); ++it) {
            if (it->first == key) {
                return &it->second;
            }
        }
        return nullptr;
    }
    [[nodiscard]] auto get(const K &key) const -> const V * {
        const auto &bucket = buckets_[idx_(key)];
        for (auto it = bucket.begin(); it != bucket.end(); ++it) {
            if (it->first == key) {
                return &it->second;
            }
        }
        return nullptr;
    }
    auto remove(const K &key) -> bool {
        auto &bucket = buckets_[idx_(key)];
        auto it = bucket.begin();
        auto it_prev = bucket.before_begin();
        for (; it != bucket.end(); ++it, ++it_prev) {
            if (it->first == key) {
                bucket.erase_after(it_prev);
                --size_;
                return true;
            }
        }
        return false;
    }
    [[nodiscard]] auto contains(const K &key) const -> bool {
        auto &bucket = buckets_[idx_(key)];
        for (auto it = bucket.begin(); it != bucket.end(); ++it) {
            if (it->first == key) {
                return true;
            }
        }
        return false;
    }
    [[nodiscard]] auto size() const noexcept -> usize { return size_; }
    [[nodiscard]] auto empty() const noexcept -> bool { return size_ == 0; }
    [[nodiscard]] auto bucket_count() const noexcept -> usize { return buckets_.size(); }
    /*
    Iterator support: a forward iterator that traverses all key-value pairs across all buckets. Must support begin(), end(), operator++, operator*, operator->, operator==/!=. Iteration order does not need to be deterministic.

    begin() would be something like &buckets_[0].begin() and we iterate through that list, once we reach the end of it we jump to the next bucket
    end is then the last bucket end of that forward list

    I have no idea about the syntax on how to define iterators
    */

private:
    std::vector<Bucket> buckets_{};
    static constexpr usize k_initial_bucket_count{8};
    static constexpr float k_max_load_factor{0.75f};
    static_assert(k_initial_bucket_count > 0 and ((k_initial_bucket_count & (k_initial_bucket_count - 1)) == 0));
    usize size_{0};

    [[nodiscard]] auto idx_(const K &key) const noexcept -> usize { return std::hash<K>{}(key) & (buckets_.size() - 1); }

    auto steal_from(HashMap &&other) -> void {
        buckets_ = std::move(other.buckets_);
        size_ = other.size_;
        other.reset();
    }
    auto copy_from(const HashMap &other) -> void {
        buckets_ = other.buckets_;
        size_ = other.size_;
    }
};
