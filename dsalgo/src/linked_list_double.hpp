#pragma once

struct LinkedListDoubleNode
{
    int value = 0;
    LinkedListDoubleNode *prev = nullptr;
    LinkedListDoubleNode *next = nullptr;
};

class LinkedListDouble
{
public:
    LinkedListDouble() : m_head(nullptr) {}
    LinkedListDouble(const LinkedListDouble &) = delete;
    LinkedListDouble &operator=(const LinkedListDouble &) = delete;
    LinkedListDouble(LinkedListDouble &&) noexcept = delete;
    LinkedListDouble &operator=(LinkedListDouble &&) noexcept = delete;

    ~LinkedListDouble();

    bool is_empty() const noexcept;
    void push_front(int v);
    void pop_front() noexcept;
    void push_back(int v);
    void pop_back() noexcept;
    void clear() noexcept;

    LinkedListDoubleNode *front() const noexcept;
    LinkedListDoubleNode *back() const noexcept;

private:
    LinkedListDoubleNode *m_head = nullptr;

    LinkedListDoubleNode *get_tail() const noexcept;
};