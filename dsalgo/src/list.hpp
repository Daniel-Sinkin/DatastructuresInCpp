// dsalgo/src/list.hpp
#pragma once
#include <cstddef>

class List
{
public:
    explicit List(std::size_t n_elements);
    List(List &&other) noexcept;
    List &operator=(List &&other) noexcept;
    List(const List &other);
    List &operator=(const List &other);
    ~List();

    void push_back(double value);
    [[nodiscard]] double pop_back_return();
    void pop_back();

    [[nodiscard]] std::size_t get_length() const noexcept;
    [[nodiscard]] std::size_t get_capacity() const noexcept;
    [[nodiscard]] bool is_empty() const noexcept;
    [[nodiscard]] bool is_full() const noexcept;

private:
    double *m_start = nullptr;
    double *m_end = nullptr;
    double *m_capacity = nullptr;
};