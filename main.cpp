
/*
Implement a LRUCache<K, V> with O(1) get and put:

x LRUCache(std::size_t capacity) constructor
get(const K& key) -> V* returns pointer to value if present, nullptr if not. Marks the entry as most recently used.
put(const K& key, const V& value) -> void inserts or updates. If inserting would exceed capacity, evict the least recently used entry first.
All operations must be O(1) amortized
No std::unordered_map restriction -- you may use STL containers freely for this one
Move/copy semantics not required, focus on the core logic
*/

// size_ = 0, cap_ = 3
//      *, {}
// put(0, 'a')
//      *<-[{0, 'a'}]->*, {0: 'a'}
// put(1, 'b')
//      *<-[{0, 'a'}]=[{1, 'b'}]->*, {0: &0, 1: &1}
// put(2, 'c')
//      *<-[{0, 'a'}]=[{1, 'b'}]=[{2, 'c'}]->*, {0: &0, 1: &1, 2: &2}
// get(0)
//      hm_[0] = &0 ~> splice &[{0, 'a'}] to the back
//      *<-[{1, 'b'}]=[{2, 'c'}]=[{0, 'a'}]->*, {0: &2, 1: &0, 2: &1}
//              Fact: Iterators are stable in std::list
// put(3, 'd')
//      Find LeastRecentlyUsed: [{1, 'b'}], evict from LRUCache
//      *<-[{2, 'c'}]=[{0, 'a'}]=[{3, 'd'}]->*, {0: &1, 2: &0, 3: &2}

#include <cstddef>
#include <list>
#include <unordered_map>

using usize = std::size_t;

template <typename K>
concept Hashable = requires(K k) {
    { std::hash<K>{}(k) } noexcept -> std::same_as<usize>;
};

template <Hashable K, typename V>
class LRUCache {
public:
    using KeyT = K;
    using ValueT = V;
    using NodeT = std::pair<KeyT, ValueT>;
    using ListT = std::list<NodeT>;
    using SizeT = usize;

    LRUCache(SizeT capacity) : cap_(capacity) {}

    auto get(const K &key) -> V * {
        if (auto it = map_.find(key); it != map_.end()) {
            // Found the key
            auto it_node = it->second;
            // First move to the end of the list, as it's most recently used
            lst_.splice(lst_.end(), lst_, it_node);
            // Return reference to the value
            return &it_node->second;
        }
        // Didn't find the key
        return nullptr;
    }

    auto put(const K &key, const V &value) -> void {
        if (auto it = map_.find(key); it != map_.end()) {
            // Found the element to overwrite
            auto it_node = it->second;
            // Overwriting value,
            it_node->second = value;
            lst_.splice(lst_.end(), lst_, it_node);
            return;
        }
        // Didn't find element, so we have to insert a new one
        if (lst_.size() == cap_) {
            auto key_to_erase = std::move(lst_.front().first);
            map_.erase(key_to_erase);
            // Remove least recently used
            lst_.pop_front();
        }
        // Pushing new element to the end of the list
        lst_.emplace_back(key, value);
        map_.emplace(key, std::prev(lst_.end()));
    }
    [[nodiscard]] auto size() const noexcept -> SizeT { return SizeT{lst_.size()}; }

private:
    ListT lst_{};
    std::unordered_map<K, typename ListT::iterator> map_{};
    SizeT cap_{0};
};
