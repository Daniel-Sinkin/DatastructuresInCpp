// dsalgo/src/linked_list_single.hpp
#pragma once

struct LinkedListSingleNode
{
    double value;
    LinkedListSingleNode *next;
};

class LinkedListSingle
{
public:
    void push_front(double v);
    void pop_front();
    bool is_empty() const noexcept;
    ~LinkedListSingle();

private:
    LinkedListSingleNode *m_head = nullptr;
};