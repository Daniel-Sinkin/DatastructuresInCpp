// dsalgo/src/linked_list_double.hpp
#pragma once
#include <cstddef>

namespace dsalgo
{

struct LinkedListDoubleNode
{
    int value{0};
    LinkedListDoubleNode *prev{nullptr};
    LinkedListDoubleNode *next{nullptr};
};

class LinkedListDouble
{
public:
    LinkedListDouble() = default;
    LinkedListDouble(const LinkedListDouble &) = delete;
    LinkedListDouble &operator=(const LinkedListDouble &) = delete;
    LinkedListDouble(LinkedListDouble &&) noexcept = delete;
    LinkedListDouble &operator=(LinkedListDouble &&) noexcept = delete;

    ~LinkedListDouble() { clear(); }

    [[nodiscard]] bool is_empty() const noexcept { return m_head == nullptr; }

    void push_front(int v)
    {
        if (!m_head)
        {
            m_head = new LinkedListDoubleNode{v, nullptr, nullptr};
            return;
        }
        auto *new_head = new LinkedListDoubleNode{v, nullptr, m_head};
        m_head->prev = new_head;
        m_head = new_head;
    }

    void pop_front() noexcept
    {
        if (!m_head) return;
        auto *old_head = m_head;
        m_head = old_head->next;
        if (m_head) m_head->prev = nullptr;
        delete old_head;
    }

    void push_back(int v)
    {
        if (!m_head)
        {
            m_head = new LinkedListDoubleNode{v, nullptr, nullptr};
            return;
        }
        auto *end = get_tail();
        end->next = new LinkedListDoubleNode{v, end, nullptr};
    }

    void pop_back() noexcept
    {
        if (!m_head) return;
        if (!m_head->next)
        {
            delete m_head;
            m_head = nullptr;
            return;
        }
        auto *end = get_tail();
        end->prev->next = nullptr;
        delete end;
    }

    void clear() noexcept
    {
        while (m_head)
            pop_front();
    }

    [[nodiscard]] LinkedListDoubleNode *front() const noexcept { return m_head; }
    [[nodiscard]] LinkedListDoubleNode *back() const noexcept { return get_tail(); }

private:
    LinkedListDoubleNode *m_head = nullptr;

    [[nodiscard]] LinkedListDoubleNode *get_tail() const noexcept
    {
        if (!m_head) return nullptr;
        auto *tmp = m_head;
        while (tmp->next)
            tmp = tmp->next;
        return tmp;
    }
};

} // namespace dsalgo