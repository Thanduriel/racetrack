#pragma once

#include <format>
#include <cmath>

namespace math {
template <typename T>
struct Vec {
    T x;
    T y;

    constexpr Vec& operator+=(Vec<T> rhs)
    {
        x += rhs.x;
        y += rhs.y;

		return *this;
    }

    constexpr T lenSq() const
    {
        return x * x + y * y;
    }

    constexpr float len() const
    {
        return std::sqrt(static_cast<float>(lenSq()));
    }
};

template <typename T>
constexpr Vec<T> operator+(const Vec<T> lhs, const Vec<T> rhs)
{
    return { lhs.x + rhs.x, lhs.y + rhs.y };
}

template <typename T>
constexpr Vec<T> operator-(const Vec<T> lhs, const Vec<T> rhs)
{
    return { lhs.x - rhs.x, lhs.y - rhs.y };
}

template <typename T>
constexpr Vec<T> operator/(const Vec<T> lhs, T rhs)
{
    return { lhs.x / rhs, lhs.y / rhs };
}

template <typename T>
T distSq(const Vec<T> a, const Vec<T> b)
{
    const Vec<T> diff = a - b;
    return diff.lenSq();
}

template <typename T>
float dist(const Vec<T> a, const Vec<T> b)
{
    const Vec<T> diff = a - b;
    return diff.len();
}

}

template <typename T>
struct std::formatter<math::Vec<T>> {
    constexpr auto parse(std::format_parse_context& ctx) {
        return ctx.begin();
    }

    auto format(const math::Vec<T>& v, std::format_context& ctx) const {
        return std::format_to(ctx.out(), "({},{})", v.x, v.y);
    }
};

