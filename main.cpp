#include <concepts>
#include <cstddef>
#include <cstdint>
#include <iostream>
#include <memory>
#include <print>
#include <type_traits>

using usize = std::size_t;

using f32 = float;
using f64 = double;

template <std::floating_point T, usize N, usize M>
    requires(N > 0 and M > 0)
class Matrix {
public:
    Matrix() {
        data_ = static_cast<T *>(::operator new(N * M * sizeof(T)));
        for (usize i{0}; i < N * M; ++i) {
            data_[i] = T{0};
        }
    }
    ~Matrix() {
        ::operator delete(data_);
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
    template <typename F>
        requires(std::is_convertible_v<F, T>)
    static auto Diagonal(F value = static_cast<F>(1.0)) -> Matrix {
        Matrix out{};
        const auto val_t = static_cast<T>(value);
        for (usize i = 0; i < std::min(N, M); ++i) {
            out(i, i) = val_t;
        }
        return out;
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

template <std::floating_point T, usize N, usize K, usize M>
auto GEMM(const Matrix<T, N, K> &A, const Matrix<T, K, M> &B) -> Matrix<T, N, M> {
    Matrix<T, N, M> C{};
    GEMM(A, B, C);
    return C;
}

template <std::floating_point T, usize N, usize M>
auto print(const Matrix<T, N, M> &A) {
    std::print("[");
    for (usize i{0}; i < N; ++i) {
        if (i > 0) {
            std::print(" ");
        }
        std::print("[");
        for (usize j{0}; j < M; ++j) {
            if (j > 0) {
                std::print(", ");
            }
            std::print("{:8.4f}", A(i, j));
        }
        std::cout << "]";
        if (i < N - 1) {
            std::cout << "\n";
        }
    }
    std::cout << "]\n";
}

int main() {
    using MatT = Matrix<f32, 4, 4>;
    auto m1 = MatT::Diagonal(-3.2f);
    m1(3, 2) = 7.2f;
    m1(1, 4) = 2.3f;
    auto m2 = MatT::Diagonal(-3.2f);
    const auto m3 = GEMM(m1, m2);
    print(m3);
}
