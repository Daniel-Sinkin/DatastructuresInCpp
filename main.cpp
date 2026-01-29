// main.cpp
#include <cassert>
#include <type_traits>
#include <utility>

struct Foo {
    explicit Foo() : x_(0) {};
    explicit Foo(int x) : x_(x) {};
    // x = 1, 2 constructors can't be disambiguated
    // explicit Foo(Bar) : x_(1) {};
    Foo(Foo &) : x_(2) {};
    Foo(const Foo &) : x_(3) {};
    Foo(Foo &&) : x_(4) {};
    Foo &operator=(const Foo &) {
        x_ = 5;
        return *this;
    }
    Foo &operator=(Foo &&) {
        x_ = 6;
        return *this;
    }

    int x() const {
        return x_;
    }

    int x_;
};

inline int kind(Foo &) { return 0; }
inline int kind(const Foo &) { return 1; }
inline int kind(Foo &&) { return 2; }

template <class T>
int forward_bad(T &&arg) {
    return kind(arg);
}

template <class T>
int forward_good(T &&arg) {
    return kind(std::forward<T>(arg));
}

void demo_decltype() {
    // x is a L value of type T
    int x{0};
    static_assert(std::is_same_v<decltype(x), int>);
    static_assert(!std::is_same_v<decltype(x), decltype((x))>);
    // decltype behaves differenty on id expression 'x' and
    // other types of expresions (id expression is just a name / identifier)
    static_assert(std::is_same_v<decltype((x)), int &>);

    int &x_ref{x};
    static_assert(std::is_same_v<decltype(x_ref), int &>);
    static_assert(std::is_same_v<decltype((x_ref)), int &>);

    static_assert(std::is_same_v<decltype(std::move(x)), int &&>);
    auto z = std::move(x);
    static_assert(std::is_same_v<decltype(z), int>);
    static_assert(std::is_same_v<decltype((std::move(x))), int &&>);
    static_assert(std::is_same_v<decltype(std::move(x)), decltype((std::move(x)))>);
}

void demo_constructors() {
    Foo foo{};
    const Foo foo_const{};
    Foo foo_moved{};
    Foo foo2{};
    Foo &&foo_lvalue_of_move{std::move(foo2)};
    Foo foo3{};
    const Foo &&foo_lvalue_of_move2{std::move(foo3)};

    assert(Foo{foo}.x() == 2);
    assert(Foo{foo_const}.x() == 3);
    assert(Foo{std::move(foo_moved)}.x() == 4);
    assert(Foo{foo_lvalue_of_move}.x() == 2);
    assert(Foo{foo_lvalue_of_move2}.x() == 3);

    Foo a{};
    a = foo;
    assert(a.x() == 5);
    Foo b{};
    b = std::move(foo);
    assert(b.x() == 6);
    Foo c{};
    c = std::move(foo_const); // Can't move const objects so this is an implicit copy
    assert(c.x() == 5);
}

void demo_perfect_forwarding() {
    Foo foo{};
    const Foo foo_const{};

    assert(forward_bad(foo) == 0);
    assert(forward_bad(std::move(foo)) == 0); // losing R value
    assert(forward_bad(foo_const) == 1);
    assert(forward_bad(std::move(foo_const)) == 1); // losing R value

    Foo bar{};
    const Foo bar_const{};

    assert(forward_good(bar) == 0);
    assert(forward_good(std::move(bar)) == 2); // Preserved R value
    assert(forward_good(bar_const) == 1);
    assert(forward_good(std::move(bar_const)) == 1); // const Rvalue doesn't bind to Foo&&
}

int main() {
    demo_decltype();
    demo_constructors();
    demo_perfect_forwarding();
}
