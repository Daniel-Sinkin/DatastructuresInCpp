// dsalgo/src/binary_tree_node.hpp
#pragma once

#include <format>
#include <memory>
#include <string>
#include <string_view>
#include <utility>

namespace dsalgo {
template <typename T>
struct BinaryTreeNode
{
    explicit BinaryTreeNode(T value) : m_value(std::move(value)) {}

    T m_value;

    void set_left(T value)
    {
        if (m_left) m_left->m_value = std::move(value);
        else m_left = std::make_unique<BinaryTreeNode>(std::move(value));
    }

    void set_right(T value)
    {
        if (m_right) m_right->m_value = std::move(value);
        else m_right = std::make_unique<BinaryTreeNode>(std::move(value));
    }

    BinaryTreeNode* left() noexcept { return m_left.get(); }
    const BinaryTreeNode* left() const noexcept { return m_left.get(); }
    BinaryTreeNode* right() noexcept { return m_right.get(); }
    const BinaryTreeNode* right() const noexcept { return m_right.get(); }

private:
    std::unique_ptr<BinaryTreeNode> m_left = nullptr;
    std::unique_ptr<BinaryTreeNode> m_right = nullptr;
};
}

template <typename T>
struct std::formatter<dsalgo::BinaryTreeNode<T>>
{
    constexpr auto parse(std::format_parse_context& ctx)
    {
        auto it = ctx.begin();
        if (it != ctx.end() && *it != '}') {
            throw std::format_error("invalid format for BinaryTreeNode");
        }
        return it;
    }

    auto format(const BinaryTreeNode<T>& node, std::format_context& ctx) const
    {
        auto out = ctx.out();
        out = std::format_to(out, "root: {}\n", node.m_value);

        const auto* l = node.left();
        const auto* r = node.right();
        const bool has_l = (l != nullptr);
        const bool has_r = (r != nullptr);

        if (has_l) out = format_child(*l, out, "", "L", !has_r);
        if (has_r) out = format_child(*r, out, "", "R", true);

        return out;
    }

private:
    static auto format_child(
        const BinaryTreeNode<T>& node,
        std::format_context::iterator out,
        const std::string& prefix,
        std::string_view label,
        bool is_last
    ) -> std::format_context::iterator
    {
        out = std::format_to(
            out,
            "{}{}── {}: {}\n",
            prefix,
            (is_last ? "└" : "├"),
            label,
            node.m_value
        );

        std::string next_prefix = prefix + (is_last ? "    " : "│   ");

        const auto* l = node.left();
        const auto* r = node.right();
        const bool has_l = (l != nullptr);
        const bool has_r = (r != nullptr);

        if (has_l) out = format_child(*l, out, next_prefix, "L", !has_r);
        if (has_r) out = format_child(*r, out, next_prefix, "R", true);

        return out;
    }
};