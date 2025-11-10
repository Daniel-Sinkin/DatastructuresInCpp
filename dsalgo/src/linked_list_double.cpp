// dsalgo/src/linked_list_double.cpp
#include "linked_list_double.hpp"

bool LinkedListDouble::is_empty() const noexcept
{
    return m_head == nullptr;
}

void LinkedListDouble::push_front(int v)
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

void LinkedListDouble::pop_front() noexcept
{
    if (!m_head) return;
    auto *old_head = m_head;
    m_head = old_head->next;
    if (m_head)
        m_head->prev = nullptr;
    delete old_head;
}

void LinkedListDouble::push_back(int v)
{
    if (!m_head)
    {
        m_head = new LinkedListDoubleNode{v, nullptr, nullptr};
        return;
    }
    auto *end = get_tail();
    end->next = new LinkedListDoubleNode{v, end, nullptr};
}

void LinkedListDouble::pop_back() noexcept
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

void LinkedListDouble::clear() noexcept
{
    while (m_head)
        pop_front();
}

LinkedListDoubleNode *LinkedListDouble::front() const noexcept
{
    return m_head;
}

LinkedListDoubleNode *LinkedListDouble::back() const noexcept
{
    return get_tail();
}

LinkedListDoubleNode *LinkedListDouble::get_tail() const noexcept
{
    if (!m_head) return nullptr;
    auto *tmp = m_head;
    while (tmp->next)
        tmp = tmp->next;
    return tmp;
}

LinkedListDouble::~LinkedListDouble()
{
    clear();
}