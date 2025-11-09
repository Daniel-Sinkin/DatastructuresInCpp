// dsalgo/src/linked_list_single.cpp
#include "linked_list_single.hpp"

void LinkedListSingle::push_front(double v) { m_head = new LinkedListSingleNode{v, m_head}; };

void LinkedListSingle::pop_front()
{
    if (!m_head) return;
    LinkedListSingleNode *old_head = m_head;
    m_head = m_head->next;
    delete old_head;
}

bool LinkedListSingle::empty() const noexcept { return m_head == nullptr; }

LinkedListSingle::~LinkedListSingle()
{
    LinkedListSingleNode *current = m_head;
    while (current)
    {
        LinkedListSingleNode *tmp = current;
        current = current->next;
        delete tmp;
    }
}