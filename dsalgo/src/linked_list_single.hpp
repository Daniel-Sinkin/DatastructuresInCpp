// dsalgo/src/linked_list_single.hpp
#pragma once
#include "types.hpp"

namespace dsalgo
{

struct LinkedListSingleNode
{
    double value{};
    LinkedListSingleNode *next{};
};

class LinkedListSingle
{
public:
    LinkedListSingle() = default;
    LinkedListSingle(const LinkedListSingle &) = delete;
    LinkedListSingle &operator=(const LinkedListSingle &) = delete;
    LinkedListSingle(LinkedListSingle &&) noexcept = delete;
    LinkedListSingle &operator=(LinkedListSingle &&) noexcept = delete;

    ~LinkedListSingle()
    {
        LinkedListSingleNode *current = m_head;
        while (current)
        {
            LinkedListSingleNode *tmp = current;
            current = current->next;
            delete tmp;
        }
    }

    void push_front(double v) { m_head = new LinkedListSingleNode{v, m_head}; }

    void pop_front()
    {
        if (!m_head) return;
        LinkedListSingleNode *old_head = m_head;
        m_head = m_head->next;
        delete old_head;
    }

    [[nodiscard]] bool is_empty() const noexcept { return m_head == nullptr; }

private:
    LinkedListSingleNode *m_head = nullptr;
};

} // namespace dsalgo