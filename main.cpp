#include <cstddef>
#include <exception>
#include <new>
#include <type_traits>
#include <utility>

struct BadOptionalAccess : std::exception {};

struct Nulloptional {};

template <typename T>
class Optional {
public:
    Optional() = default;
    explicit Optional(const T &val) {
        new (reinterpret_cast<T *>(buffer_)) T{val};
        has_value_ = true;
    }
    explicit Optional(T &&val) {
        new (reinterpret_cast<T *>(buffer_)) T{std::move(val)};
        has_value_ = true;
    }
    ~Optional() { reset(); }

    Optional(Nulloptional) noexcept {}

    Optional(const Optional &other) noexcept(std::is_nothrow_copy_constructible_v<T>) {
        copy_from(other);
    }
    Optional &operator=(const Optional &other) noexcept(std::is_nothrow_copy_constructible_v<T>) {
        if (this != &other) {
            reset();
            copy_from(other);
        }
        return *this;
    }
    Optional(Optional &&other) noexcept(std::is_nothrow_move_constructible_v<T>) {
        steal_from(std::move(other));
    }
    Optional &operator=(Optional &&other) noexcept(std::is_nothrow_move_constructible_v<T>) {
        if (this != &other) {
            reset();
            steal_from(std::move(other));
        }
        return *this;
    }

    [[nodiscard]] friend bool operator==(const Optional &left, const Optional &right) {
        if (left.has_value_ and right.has_value_) {
            return *left.ptr_() == *right.ptr_();
        } else {
            return left.has_value_ == right.has_value_;
        }
    }
    [[nodiscard]] friend bool operator==(const Optional &left, [[maybe_unused]] const Nulloptional &) {
        return !left.has_value_;
    }
    [[nodiscard]] friend bool operator==([[maybe_unused]] const Nulloptional &, const Optional &right) {
        return !right.has_value_;
    }
    [[nodiscard]] friend bool operator==(const Optional &left, const T &right) {
        return left.has_value_ && (*left.ptr_() == right);
    }
    [[nodiscard]] friend bool operator==(const T &left, const Optional &right) {
        return right.has_value_ && (left == *right.ptr_());
    }

    template <typename... Ts>
    auto emplace(Ts &&...ts) -> T & {
        reset();
        new (reinterpret_cast<T *>(buffer_)) T{std::forward<Ts>(ts)...};
        has_value_ = true;
        return *ptr_();
    }

    [[nodiscard]] auto has_value() const noexcept -> bool { return has_value_; }
    [[nodiscard]] explicit operator bool() const noexcept { return has_value_; }

    [[nodiscard]] auto value() -> T & {
        if (!has_value_)
            throw BadOptionalAccess{};
        return *ptr_();
    }
    [[nodiscard]] auto value() const -> const T & {
        if (!has_value_)
            throw BadOptionalAccess{};
        return *ptr_();
    }
    [[nodiscard]] auto operator*() -> T & { return *ptr_(); }
    [[nodiscard]] auto operator*() const -> const T & { return *ptr_(); }
    [[nodiscard]] auto operator->() -> T * { return ptr_(); }
    [[nodiscard]] auto operator->() const -> const T * { return ptr_(); }

    template <typename U>
    [[nodiscard]] auto value_or(U &&u) -> T {
        if (has_value_) {
            return *ptr_();
        } else {
            return static_cast<T>(std::forward<U>(u));
        }
    }

    auto reset() noexcept -> void {
        if (has_value_) {
            ptr_()->~T();
            has_value_ = false;
        }
    }

private:
    alignas(T) std::byte buffer_[sizeof(T)];
    bool has_value_{false};

    auto copy_from(const Optional &other) -> void {
        if (other.has_value_) {
            new (reinterpret_cast<T *>(buffer_)) T{*other.ptr_()};
            has_value_ = true;
        }
    }
    auto steal_from(Optional &&other) -> void {
        if (other.has_value_) {
            new (reinterpret_cast<T *>(buffer_)) T{std::move(*other.ptr_())};
            other.reset();
            has_value_ = true;
        }
    }

    [[nodiscard]] auto ptr_() noexcept -> T * {
        return std::launder(reinterpret_cast<T *>(buffer_));
    }
    [[nodiscard]] auto ptr_() const noexcept -> const T * {
        return std::launder(reinterpret_cast<const T *>(buffer_));
    }
};

template <typename T, typename... Args>
auto make_optional(Args &&...args) -> Optional<T> {
    Optional<T> out{};
    out.emplace(std::forward<Args>(args)...);
    return out;
}
