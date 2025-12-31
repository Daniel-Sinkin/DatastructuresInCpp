// graph.hpp
#include <algorithm>
#include <expected>
#include <print>
#include <ranges>
#include <span>
#include <utility>
#include <vector>
#include <optional>

#include "types.hpp"

namespace dsalgo
{
using GraphIndex = u64;

template <typename T>
class GraphNode
{
public:
    explicit GraphNode(T value, GraphIndex idx) : m_value(std::move(value)), m_idx(idx) {}

    const T &value() const noexcept { return m_value; }
    T value_copy() const { return m_value; }
    void set_value(T value) noexcept { m_value = std::move(value); }

    GraphIndex get_idx() const noexcept { return m_idx; }

    std::span<const GraphIndex> neighbors() const noexcept { return m_neighbors; }

    bool contains_edge(GraphIndex idx) const noexcept {
        return std::ranges::binary_search(m_neighbors, idx);
    }

    enum class AddNeighborError
    {
        SelfLoop,
        DuplicateNeighbor,
    };
    
    [[nodiscard]]
    std::expected<void, AddNeighborError>
    add_neighbor(GraphIndex idx)
    {
        if (idx == m_idx)
        {
            return std::unexpected(AddNeighborError::SelfLoop);
        }
        auto insertion_slot = std::ranges::lower_bound(m_neighbors, idx);
        if (insertion_slot != m_neighbors.end() && *insertion_slot == idx)
        {
            return std::unexpected(AddNeighborError::DuplicateNeighbor);
        }
        m_neighbors.insert(insertion_slot, idx);
        return {};
    }

    enum class ValidationError
    {
        UnsortedNeighbors,
        SelfLoop,
        DuplicateNeighbor,
    };
    [[nodiscard]]
    std::expected<void, ValidationError>
    validate() const
    {
        if (!std::ranges::is_sorted(m_neighbors))
        {
            return std::unexpected(ValidationError::UnsortedNeighbors);
        }
        if (std::ranges::binary_search(m_neighbors, m_idx))
        {
            return std::unexpected(ValidationError::SelfLoop);
        }
        if (std::ranges::adjacent_find(m_neighbors) != m_neighbors.end())
        {
            return std::unexpected(ValidationError::DuplicateNeighbor);
        }
        return {};
    }

private:
    T m_value;
    GraphIndex m_idx;
    std::vector<GraphIndex> m_neighbors;
};

template <typename T>
class Graph
{
public:
    const T* value_by_idx(GraphIndex idx) const noexcept {
        auto it = m_nodes.find(idx);
        if (it == m_nodes.end()) return nullptr;
        return &it->second->value();
    }

    
    enum class AddEdgeError
    {
        NodeFromMissing,
        NodeToMissing,
        EdgeExists,
        SelfLoop,
    };
    [[nodiscard]]
    std::expected<void, AddEdgeError>
    add_edge(GraphIndex idx0, GraphIndex idx1) {
        using E = AddEdgeError;

        if(idx0 == idx1) return std::unexpected(E::SelfLoop);

        auto it0 = m_nodes.find(idx0);
        if (it0 == m_nodes.end()) return std::unexpected(E::NodeFromMissing);
        GraphNode<T>& node0 = *it0->second;

        if(!m_nodes.contains(idx1)) return std::unexpected(E::NodeToMissing);

        auto res = node0.add_neighbor(idx1);
        if(!res) {
            using EE = GraphNode<T>::AddNeighborError;
            switch(res.error()) {
                case EE::SelfLoop:
                    assert(false && "Graph::add_edge prechecked idx0==idx1 but node reported SelfLoop");
                    return std::unexpected(E::SelfLoop);
                case EE::DuplicateNeighbor:
                    return std::unexpected(E::EdgeExists);
            }
        }
        return {};
    }


    enum class CreateNodeError
    {
        IndexExists,
        IndexOverflowing,
        CreationFailed
    };
    [[nodiscard]]
    std::expected<GraphNode<T> *, CreateNodeError>
    create_node(T value, GraphIndex idx)
    {
        if (m_nodes.contains(idx))
        {
            return std::unexpected(CreateNodeError::IndexExists);
        }

        auto ptr = std::unique_ptr<GraphNode<T>>(new (std::nothrow) GraphNode<T>(value, idx));
        if (!ptr)
        {
            return std::unexpected(CreateNodeError::CreationFailed);
        }

        GraphNode<T> *raw = ptr.get();
        m_nodes.emplace(idx, std::move(ptr));
        return raw;
    }

    [[nodiscard]]
    std::expected<GraphNode<T> *, CreateNodeError>
    create_node(T value)
    {
        const auto idx_or_err = next_index();
        if (!idx_or_err)
        {
            return std::unexpected(idx_or_err.error());
        }
        return create_node(value, *idx_or_err);
    }

    void print() const
    {
        std::println("Graph:");

        for (const auto &[idx, node_ptr] : m_nodes)
        {
            const GraphNode<T> &node = *node_ptr;

            std::print("  Node {}: value={}, neighbors=[",
                node.get_idx(),
                node.value());

            auto neighbors = node.neighbors();
            for (std::size_t i = 0; i < neighbors.size(); ++i)
            {
                std::print("{}", neighbors[i]);
                if (i + 1 < neighbors.size())
                {
                    std::print(", ");
                }
            }
            std::println("]");
        }
    }

    [[nodiscard]]
    std::expected<void, typename GraphNode<T>::ValidationError> validate_all() const
    {
        for (const auto& [_, node_ptr] : m_nodes) {
            if (auto r = node_ptr->validate(); !r) {
                return std::unexpected(r.error());
            }
        }
        return {};
    }

private:
    [[nodiscard]]
    std::expected<GraphIndex, CreateNodeError>
    next_index() const
    {
        if (m_nodes.empty())
        {
            return GraphIndex{0};
        }

        GraphIndex max_idx = 0;
        for (const auto &[idx, _] : m_nodes)
        {
            max_idx = std::max(max_idx, idx);
        }

        if (max_idx == std::numeric_limits<GraphIndex>::max())
        {
            return std::unexpected(CreateNodeError::IndexOverflowing);
        }

        return static_cast<GraphIndex>(max_idx + 1);
    }
    std::unordered_map<GraphIndex, std::unique_ptr<GraphNode<T>>> m_nodes;
};
} // namespace dsalgo