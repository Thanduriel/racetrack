#pragma once

namespace math
{
	template <typename T>
	struct Vec
	{
		T x;
		T y;

		Vec &operator+=(Vec<T> rhs)
		{
			x += rhs.x;
			y += rhs.y;
		}
	};

	template <typename T>
	Vec<T> operator+(const Vec<T> lhs, const Vec<T> rhs)
	{
		return {lhs.x + rhs.x, lhs.y + rhs.y};
	}

	template <typename T>
	Vec<T> operator-(const Vec<T> lhs, const Vec<T> rhs)
	{
		return {lhs.x - rhs.x, lhs.y - rhs.y};
	}

	template <typename T>
	T distSq(const Vec<T> a, const Vec<T> b)
	{
		const Vec<T> diff = a - b;
		return diff.x * diff.x + diff.y * diff.y;
	}
}
