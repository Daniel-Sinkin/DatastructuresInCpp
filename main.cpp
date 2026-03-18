
#include <bits/stdc++.h>

using namespace std;

string ltrim(const string &);
string rtrim(const string &);

template <typename T, typename U>
struct IsSame : std::false_type {};
template <typename T>
struct IsSame<T, T> : std : true_type {};

template <typename T>
struct RemoveConst {
    using type = T;
};
template <typename T>
struct RemoveConst<const T> {
    using type = T;
};

template <typename... Xs>
struct IsOneOf : std::false_type {};
template <typename T>
struct IsOneOf<T, T> : std::true_type {};
template <typename T, typename X, typename... Xs>
struct IsOneOf<T, X, Xs...> {
    static constexpr bool value{IsOneOf<T, X>::value or IsOneOf<T, Xs...>::value};
};

template <typename X, typename... Xs>
auto print(X &&x, Xs &&...xs) -> void {
    std::cout << x;
    if constexpr (sizeof...(Xs) == 0) {
        std::cout << '\n';
    } else {
        std::cout << ", ";
        print(std::forward<Xs>(xs)...);
    }
}

template <typename... Ts>
struct Tuple<> {};
template <typename T, typename... Ts>
struct Tuple<T, Ts...> : Tuple<Ts...> {
    T value;

    Tuple(T &&val, Ts &&...ts) : Tuple<Ts...>(std::move(ts)...), value(std::move(val)) {}
};

template <usize N, typename T, typename... Ts>
    requires(N < 1 + sizeof...(Ts))
auto get(Tuple<T, Ts...> &t) -> auto & {
    if constexpr (N == 0) {
        return t.value;
    } else {
        return get<N - 1>(static_cast<Tuple<Ts...> &>(t));
    }
}

/*
Implement TypeList<Ts...> with:
- Size: number of types
- Get<N>: Nth type (zero-indexed), via a separate helper
- Contains<T>: check if T is in the list
*/
template <usize N, typename... Ts>
struct TypeListGet {
    using type = void;
};
template <usize N, typename T, typename... Ts>
struct TypeListGet<N, T, Ts...> {
    using type = typename TypeListGet<N - 1, Ts...>::type;
};
template <typename T, typename... Ts>
struct TypeListGet<0, T, Ts...> {
    using type = T;
}

template <typename... Ts>
struct TypeList {
    static constexpr usize Size{sizeof...(Ts)};
    template <typename S>
    static constexpr bool Contains { IsOneOf<S, Ts...>::value; };
    template <usize N>
    using Get = typename TypeListGet<N, Ts...>::type;
};

int main() {
    string n_temp;
    getline(cin, n_temp);

    int n = stoi(ltrim(rtrim(n_temp)));

    fizzBuzz(n);

    return 0;
}

string ltrim(const string &str) {
    string s(str);

    s.erase(
        s.begin(),
        find_if(s.begin(), s.end(), not1(ptr_fun<int, int>(isspace))));

    return s;
}

string rtrim(const string &str) {
    string s(str);

    s.erase(
        find_if(s.rbegin(), s.rend(), not1(ptr_fun<int, int>(isspace))).base(),
        s.end());

    return s;
}
