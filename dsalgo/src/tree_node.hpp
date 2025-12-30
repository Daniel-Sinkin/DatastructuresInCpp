// dsalgo/src/tree_node.hpp
#pragma once

#include <format>
#include <memory>
#include <string>
#include <utility>
#include <vector>

namespace dsalgo{

template <typename T>
struct TreeNode
{
    explicit TreeNode(T value) : m_value(std::move(value)) {}

    void create_new_child(T value)
    {
        m_children.emplace_back(std::make_unique<TreeNode>(std::move(value)));
    }

    [[nodiscard]] TreeNode* child_ptr(size_t idx) noexcept { return m_children.at(idx).get(); }
    [[nodiscard]] const TreeNode* child_ptr(size_t idx) const noexcept { return m_children.at(idx).get(); }

    [[nodiscard]] const T& value() const noexcept { return m_value; }
    [[nodiscard]] size_t child_count() const noexcept { return m_children.size(); }
    [[nodiscard]] const auto& children() const noexcept { return m_children; }

private:
    T m_value;
    std::vector<std::unique_ptr<TreeNode>> m_children;
};
}

template <typename T>
struct std::formatter<dsalgo::TreeNode<T>>
{
    constexpr auto parse(std::format_parse_context& ctx)
    {
        auto it = ctx.begin();
        if (it != ctx.end() && *it != '}') {
            throw std::format_error("invalid format for TreeNode");
        }
        return it;
    }

    auto format(const TreeNode<T>& node, std::format_context& ctx) const
    {
        auto out = ctx.out();
        out = std::format_to(out, "root: {}\n", node.value());

        const auto& ch = node.children();
        for (size_t i = 0; i < ch.size(); ++i) {
            const bool is_last = (i + 1 == ch.size());
            out = format_child(*ch[i], out, "", i, is_last);
        }
        return out;
    }

private:
    static auto format_child(
        const TreeNode<T>& node,
        std::format_context::iterator out,
        const std::string& prefix,
        size_t idx,
        bool is_last
    ) -> std::format_context::iterator
    {
        out = std::format_to(
            out,
            "{}{}── {}: {}\n",
            prefix,
            (is_last ? "└" : "├"),
            idx,
            node.value()
        );

        std::string next_prefix = prefix + (is_last ? "    " : "│   ");

        const auto& ch = node.children();
        for (size_t i = 0; i < ch.size(); ++i) {
            const bool child_is_last = (i + 1 == ch.size());
            out = format_child(*ch[i], out, next_prefix, i, child_is_last);
        }
        return out;
    }
};