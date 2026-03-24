#include <concepts>
#include <cstddef>
#include <cstdint>
#include <memory>
#include <print>
#include <type_traits>

using usize = std::size_t;

using f32 = float;
using f64 = double;

template <std::floating_point T, usize N, usize M>
    requires(std::is_trivially_copyable_v<T>)
class Matrix {
public:
    Matrix() {
        data_ = static_cast<T *>(::operator new(N * M * sizeof(T)));
    }
    auto row(usize i) noexcept -> T * {
        return (data_ + i * M);
    }
    auto row(usize i) const noexcept -> const T * {
        return (data_ + i * M);
    }
    auto operator()(usize i, usize j) const -> T {
        return row(i)[j];
    }
    auto operator()(usize i, usize j) -> T & {
        return row(i)[j];
    }

private:
    T *data_{};
};
template <usize N, usize M>
using Matrix32 = Matrix<f32, N, M>;
template <usize N, usize M>
using Matrix64 = Matrix<f64, N, M>;

template <std::floating_point T, usize N, usize K, usize M>
auto GEMM(const Matrix<T, N, K> &A, const Matrix<T, K, M> &B, Matrix<T, N, M> &C) -> void {
    // Naive implementation
    for (usize n{0}; n < N; ++n) {
        for (usize m{0}; m < M; ++m) {
            T accum{0};
            for (usize k{0}; k < K; ++k) {
                // Row * column <- column is strided == bad memory access pattern
                accum += A(n, k) * B(k, m);
            }
            C(n, m) = accum;
        }
    }
}

/*
Suppose Cache line is 64 Byte, computing with fp32 (= 4 Byte)
we have (tightly packed) 16 fp32 per CL

GEMM(
X X X X
5 7 8 9
A B C D
E F G H
,
X 2 3 4
X 7 8 9
X B C D
X F G H
,
X * * *
* * * *
* * * *
* * * *
)
*/

int main() {
    std::println("Hello, World!");
}
