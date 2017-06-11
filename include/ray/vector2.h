// +----------------------------------------------------------------------
// | Project : ray.
// | All rights reserved.
// +----------------------------------------------------------------------
// | Copyright (c) 2013-2017.
// +----------------------------------------------------------------------
// | * Redistribution and use of this software in source and binary forms,
// |   with or without modification, are permitted provided that the following
// |   conditions are met:
// |
// | * Redistributions of source code must retain the above
// |   copyright notice, this list of conditions and the
// |   following disclaimer.
// |
// | * Redistributions in binary form must reproduce the above
// |   copyright notice, this list of conditions and the
// |   following disclaimer in the documentation and/or other
// |   materials provided with the distribution.
// |
// | * Neither the name of the ray team, nor the names of its
// |   contributors may be used to endorse or promote products
// |   derived from this software without specific prior
// |   written permission of the ray team.
// |
// | THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
// | "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
// | LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
// | A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
// | OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
// | SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
// | LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
// | DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
// | THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// | (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// | OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
// +----------------------------------------------------------------------
#ifndef _H_VECTOR2_H_
#define _H_VECTOR2_H_

#include <ray/mathfwd.h>
#include <ray/trait.h>
#include <ray/gdicmn.h>

_NAME_BEGIN

template<typename T>
class Vector2t
{
public:
	typedef typename trait::_typeaddition<T>::value_type value_type;
	typedef typename trait::_typeaddition<T>::pointer pointer;
	typedef typename trait::_typeaddition<T>::const_pointer const_pointer;
	typedef typename trait::_typeaddition<T>::reference reference;
	typedef typename trait::_typeaddition<T>::const_reference const_reference;

	T x, y;

	static const Vector2t<T> Zero;
	static const Vector2t<T> One;
	static const Vector2t<T> UnitX;
	static const Vector2t<T> UnitY;
	static const Vector2t<T> Right;
	static const Vector2t<T> Forward;

	Vector2t() noexcept {}
	Vector2t(T xx, T yy) noexcept : x(xx), y(yy) {}
	explicit Vector2t(T xy) noexcept : x(xy), y(xy) {}

	template<typename S, typename = std::enable_if<std::is_pointer<S>::value>>
	explicit Vector2t(S xy[2]) noexcept
		: x(static_cast<typename trait::_typeaddition<S>::value_type>(xy[0]))
		, y(static_cast<typename trait::_typeaddition<S>::value_type>(xy[1]))
	{
		assert(xy[0] < std::numeric_limits<typename trait::_typeaddition<S>::value_type>::max());
		assert(xy[1] < std::numeric_limits<typename trait::_typeaddition<S>::value_type>::max());
	}

	Vector2t<T>& operator+=(T scale) noexcept { x += scale; y += scale; return *this; }
	Vector2t<T>& operator-=(T scale) noexcept { x -= scale; y -= scale; return *this; }
	Vector2t<T>& operator*=(T scale) noexcept { x *= scale; y *= scale; return *this; }
	Vector2t<T>& operator/=(T scale) noexcept { x /= scale; y /= scale; return *this; }

	Vector2t<T>& operator+=(const Vector2t<T>& v) noexcept { x += v.x; y += v.y; return *this; }
	Vector2t<T>& operator-=(const Vector2t<T>& v) noexcept { x -= v.x; y -= v.y; return *this; }
	Vector2t<T>& operator*=(const Vector2t<T>& v) noexcept { x *= v.x; y *= v.y; return *this; }
	Vector2t<T>& operator/=(const Vector2t<T>& v) noexcept { x /= v.x; y /= v.y; return *this; }

	template<typename S, typename = std::enable_if_t<!std::is_pointer<S>::value>>
	explicit operator Vector2t<S>() const noexcept
	{
		assert(x <= std::numeric_limits<typename trait::_typeaddition<S>::value_type>::max());
		assert(y <= std::numeric_limits<typename trait::_typeaddition<S>::value_type>::max());

		return Vector2t<S>(static_cast<S>(x), static_cast<S>(y));
	}

	T operator[](std::uint8_t i) const noexcept { return *(&x + i); }
	reference operator[](std::uint8_t i) noexcept { return *(&x + i); }

	void set(T xx, T yy) noexcept { x = xx; y = yy; }
	void set(const Vector2t<T>& v) noexcept { x = v.x; y = v.y; }

	Vector2t<T> xx() const noexcept { return Vector2t<T>(x, x); }
	Vector2t<T> xy() const noexcept { return Vector2t<T>(x, y); }
	Vector2t<T> yx() const noexcept { return Vector2t<T>(y, x); }
	Vector2t<T> yy() const noexcept { return Vector2t<T>(y, y); }
	Vector3t<T> xxx() const noexcept { return Vector3t<T>(x, x, x); }
	Vector3t<T> xxy() const noexcept { return Vector3t<T>(x, x, y); }
	Vector3t<T> xyx() const noexcept { return Vector3t<T>(x, y, x); }
	Vector3t<T> xyy() const noexcept { return Vector3t<T>(x, y, y); }
	Vector3t<T> yxx() const noexcept { return Vector3t<T>(y, x, x); }
	Vector3t<T> yxy() const noexcept { return Vector3t<T>(y, x, y); }
	Vector3t<T> yyx() const noexcept { return Vector3t<T>(y, y, x); }
	Vector3t<T> yyy() const noexcept { return Vector3t<T>(y, y, y); }
	Vector4t<T> xxxx() const noexcept { return Vector4t<T>(x, x, x, x); }
	Vector4t<T> xxyx() const noexcept { return Vector4t<T>(x, x, y, x); }
	Vector4t<T> xyxx() const noexcept { return Vector4t<T>(x, y, x, x); }
	Vector4t<T> xyyx() const noexcept { return Vector4t<T>(x, y, y, x); }
	Vector4t<T> xxxy() const noexcept { return Vector4t<T>(x, x, x, y); }
	Vector4t<T> xxyy() const noexcept { return Vector4t<T>(x, x, y, y); }
	Vector4t<T> xyxy() const noexcept { return Vector4t<T>(x, y, x, y); }
	Vector4t<T> xyyy() const noexcept { return Vector4t<T>(x, y, y, y); }
	Vector4t<T> yxxx() const noexcept { return Vector4t<T>(y, x, x, x); }
	Vector4t<T> yxyx() const noexcept { return Vector4t<T>(y, x, y, x); }
	Vector4t<T> yyxx() const noexcept { return Vector4t<T>(y, y, x, x); }
	Vector4t<T> yyyx() const noexcept { return Vector4t<T>(y, y, y, x); }
	Vector4t<T> yxxy() const noexcept { return Vector4t<T>(y, x, x, y); }
	Vector4t<T> yxyy() const noexcept { return Vector4t<T>(y, x, y, y); }
	Vector4t<T> yyxy() const noexcept { return Vector4t<T>(y, y, x, y); }
	Vector4t<T> yyyy() const noexcept { return Vector4t<T>(y, y, y, y); }

	reference getComponent(std::uint8_t index) noexcept
	{
		switch (index)
		{
		case 0: return x;
		case 1: return y;
		case 'x': return x;
		case 'y': return y;
		default:
			assert(false);
		}
	}

	pointer ptr() noexcept { return (pointer)this; }
	const_pointer ptr() const noexcept { return (const_pointer)this; }
	pointer data() noexcept { return (pointer)this; }
	const_pointer data() const noexcept { return (const_pointer)this; }
};

template<typename T> const Vector2t<T> Vector2t<T>::Zero = Vector2t<T>((T)0, (T)0);
template<typename T> const Vector2t<T> Vector2t<T>::One = Vector2t<T>((T)1, (T)1);
template<typename T> const Vector2t<T> Vector2t<T>::UnitX = Vector2t<T>((T)1, (T)0);
template<typename T> const Vector2t<T> Vector2t<T>::UnitY = Vector2t<T>((T)0, (T)1);
template<typename T> const Vector2t<T> Vector2t<T>::Right = Vector2t<T>((T)1, (T)0);
template<typename T> const Vector2t<T> Vector2t<T>::Forward = Vector2t<T>((T)0, (T)1);

template <typename T>
inline bool operator==(const Vector2t<T>& v1, const Vector2t<T>& v2) noexcept
{
	return v1.x == v2.x && v1.y == v2.y;
}

template <typename T>
inline bool operator!=(const Vector2t<T>& v1, const Vector2t<T>& v2) noexcept
{
	return !(v1 == v2);
}

template <typename T>
inline Vector2t<T> operator*(const Vector2t<T>& v1, const Vector2t<T>& v2) noexcept
{
	return Vector2t<T>(v1.x * v2.x, v1.y * v2.y);
}

template <typename T>
inline Vector2t<T> operator+(const Vector2t<T>& v1, const Vector2t<T>& v2) noexcept
{
	return Vector2t<T>(v1.x + v2.x, v1.y + v2.y);
}

template <typename T>
inline Vector2t<T> operator-(const Vector2t<T>& v1, const Vector2t<T>& v2) noexcept
{
	return Vector2t<T>(v1.x - v2.x, v1.y - v2.y);
}

template <typename T>
inline Vector2t<T> operator*(T f, const Vector2t<T>& v) noexcept
{
	return Vector2t<T>(f*v.x, f*v.y);
}

template <typename T>
inline Vector2t<T> operator*(const Vector2t<T>& v, T f) noexcept
{
	return Vector2t<T>(f*v.x, f*v.y);
}

template <typename T>
inline Vector2t<T> operator/(T f, const Vector2t<T>& v) noexcept
{
	return Vector2t<T>(f / v.x, f / v.y);
}

template <typename T>
inline Vector2t<T> operator/(const Vector2t<T>& v, T f) noexcept
{
	return Vector2t<T>(v.x / f, v.y / f);
}

template <typename T>
inline Vector2t<T> operator/(const Vector2t<T>& v1, const Vector2t<T>& v2) noexcept
{
	return Vector2t<T>(v1.x / v2.x, v1.y / v2.y);
}

template <typename T>
inline Vector2t<T> operator-(const Vector2t<T>& v) noexcept
{
	return Vector2t<T>(-v.x, -v.y);
}

template <typename T>
inline Vector2t<T> operator~(const Vector2t<T>& v) noexcept
{
	return Vector2t<T>(v).normalize();
}

template <typename T>
inline Vector2t<T> operator*(unsigned char scale, const Vector2t<T>& v) noexcept
{
	return Vector2t<T>(v.x * scale, v.y * scale);
}

template <typename T>
inline Vector2t<T> operator/(unsigned char scale, const Vector2t<T>& v) noexcept
{
	return Vector2t<T>(v.x / scale, v.y / scale);
}

template <typename T>
inline Vector2t<T> operator*(unsigned scale, const Vector2t<T>& v) noexcept
{
	return Vector2t<T>(v.x * scale, v.y * scale);
}

template <typename T>
inline Vector2t<T> operator/(unsigned scale, const Vector2t<T>& v) noexcept
{
	return Vector2t<T>(v.x / scale, v.y / scale);
}

template<typename ostream, typename T, std::enable_if_t<trait::has_left_shift<ostream, T>::value, int> = 0>
inline ostream& operator << (ostream& os, const Vector2t<T>& v)
{
	os << v.x << ", " << v.y;
	return os;
}

template<typename istream, typename T, std::enable_if_t<trait::has_right_shift<istream>::value, int> = 0>
inline istream& operator >> (istream& is, Vector2t<T>& v)
{
	is >> v.x;
	is.ignore(2);
	is >> v.y;
	return is;
}

namespace math
{
	template<typename T>
	inline T dot(const Vector2t<T>& v1, const Vector2t<T>& v2) noexcept
	{
		return v1.x * v2.x + v1.y * v2.y;
	}

	template<typename T>
	inline T length2(const Vector2t<T>& v) noexcept
	{
		return dot(v, v);
	}

	template<typename T>
	inline T length(const Vector2t<T>& v) noexcept
	{
		return std::sqrt(length2(v));
	}

	template<typename T>
	inline T distance(const Vector2t<T>& v1, const Vector2t<T>& v2) noexcept
	{
		return length(v1 - v2);
	}

	template<typename T>
	inline T sqrDistance(const Vector2t<T>& v1, const Vector2t<T>& v2) noexcept
	{
		return length2(v1 - v2);
	}

	template <typename T>
	inline T min(const Vector2t<T>& v) noexcept
	{
		return std::min(v.x, v.y);
	}

	template <typename T>
	inline T max(const Vector2t<T>& v) noexcept
	{
		return std::max(v.x, v.y);
	}

	template<typename T>
	inline bool equal(const Vector2t<T>& v1, const Vector2t<T>& v2) noexcept
	{
		return math::equal<T>(v1.x, v2.x) && math::equal<T>(v1.y, v2.y);
	}

	template<typename T>
	inline bool isfinite(const Vector2t<T>& v) noexcept
	{
		return std::isfinite(v.x) && std::isfinite(v.y);
	}

	template<typename T>
	inline bool lineIntersection(const Vector2t<T>& x0, const Vector2t<T>& x1, const Vector2t<T>& y0, const Vector2t<T>& y1, Vector2t<T>& res) noexcept
	{
		Vector2t<T> dx = x1 - x0;
		Vector2t<T> dy = y1 - y0;

		T dyx = dy.x * dx.y - dy.y * dx.x;
		if (dyx == 0.0f)
			return false;

		Vector2t<T> d = x0 - y0;
		dyx = (d.x * dx.y - d.y * dx.x) / dyx;
		if (dyx <= 0 || dyx >= 1)
			return false;

		res.x = y0.x + dyx * dy.x;
		res.y = y0.y + dyx * dy.y;

		return true;
	}

	template<typename T>
	Vector2t<T> cross(const Vector2t<T>& v1, const Vector3t<T>& v2) noexcept
	{
		return Vector2t<T>(
			v1.y * v2.x - v1.x * v2.y,
			v1.x * v2.y - v1.y * v2.x);
	}

	template<typename T>
	inline Vector2t<T> negate(const Vector2t<T>& v) noexcept
	{
		return Vector2t<T>(-v.x, -v.y);
	}

	template<typename T>
	inline Vector2t<T> clamp(const Vector2t<T>& t, T min, T max) noexcept
	{
		return Vector2t<T>(
			std::max(min, std::min(max, t.x)),
			std::max(min, std::min(max, t.y))
			);
	}

	template<typename T>
	inline Vector2t<T> saturate(const Vector2t<T>& v) noexcept
	{
		return clamp(v, T(0.0f), T(1.0f));
	}

	template<typename T>
	inline Vector2t<T> normalize(const Vector2t<T>& v) noexcept
	{
		T magSq = length2(v);
		if (magSq > static_cast<T>(0.0))
		{
			T invSqrt = 1.0f / sqrt(magSq);
			return v * invSqrt;
		}

		return v;
	}

	template<typename T>
	inline Vector2t<T> snorm2unorm(const Vector2t<T>& v) noexcept
	{
		return Vector2t<T>(
			_mathutil<T>::snorm2unorm(v.x),
			_mathutil<T>::snorm2unorm(v.y)
			);
	}

	template<typename T>
	inline Vector2t<T> unorm2snorm(const Vector2t<T>& v) noexcept
	{
		return Vector2t<T>(
			_mathutil<T>::unorm2snorm(v.x),
			_mathutil<T>::unorm2snorm(v.y)
			);
	}

	template <typename T>
	inline Vector2t<T> min(const Vector2t<T>& a, const Vector2t<T>& b) noexcept
	{
		return Vector2t<T>(std::min(a.x, b.x), std::min(a.y, b.y));
	}

	template <typename T>
	inline Vector2t<T> max(const Vector2t<T>& a, const Vector2t<T>& b) noexcept
	{
		return Vector2t<T>(std::max(a.x, b.x), std::max(a.y, b.y));
	}

	template<typename T>
	inline Vector2t<T> abs(const Vector2t<T>& v) noexcept
	{
		return Vector2t<T>(std::abs(v.x), std::abs(v.y));
	}

	template<typename T>
	inline Vector2t<T> cos(const Vector2t<T>& v) noexcept
	{
		return Vector2t<T>(std::cos(v.x), std::cos(v.y));
	}

	template<typename T>
	inline Vector2t<T> sin(const Vector2t<T>& v) noexcept
	{
		return Vector2t<T>(std::sin(v.x), std::sin(v.y));
	}

	template<typename T>
	inline Vector2t<T> tan(const Vector2t<T>& v) noexcept
	{
		return Vector2t<T>(std::tan(v.x), std::tan(v.y));
	}

	template<typename T>
	inline Vector2t<T> acos(const Vector2t<T>& v) noexcept
	{
		return Vector2t<T>(std::acos(v.x), std::acos(v.y));
	}

	template<typename T>
	inline Vector2t<T> asin(const Vector2t<T>& v) noexcept
	{
		return Vector2t<T>(std::asin(v.x), std::asin(v.y));
	}

	template<typename T>
	inline Vector2t<T> atan(const Vector2t<T>& v) noexcept
	{
		return Vector2t<T>(std::atan(v.x), std::atan(v.y));
	}

	template<typename T>
	inline Vector2t<T> exp(const Vector2t<T>& v) noexcept
	{
		return Vector2t<T>(std::exp(v.x), std::exp(v.y));
	}

	template<typename T>
	inline Vector2t<T> exp2(const Vector2t<T>& v) noexcept
	{
		return Vector2t<T>(std::exp2(v.x), std::exp2(v.y));
	}

	template<typename T>
	inline Vector2t<T> log(const Vector2t<T>& v) noexcept
	{
		return Vector2t<T>(std::log(v.x), std::log(v.y));
	}

	template<typename T>
	inline Vector2t<T> log2(const Vector2t<T>& v) noexcept
	{
		return Vector2t<T>(std::log2(v.x), std::log2(v.y));
	}

	template<typename T>
	inline Vector2t<T> log10(const Vector2t<T>& v) noexcept
	{
		return Vector2t<T>(std::log10(v.x), std::log10(v.y));
	}

	template<typename T>
	inline Vector2t<T> sqrt(const Vector2t<T>& v1) noexcept
	{
		return Vector2t<T>(std::sqrt(v1.x), std::sqrt(v1.y));
	}

	template<typename T>
	inline Vector2t<T> pow(const Vector2t<T>& v1, const Vector2t<T>& v2) noexcept
	{
		return Vector2t<T>(std::pow(v1.x, v2.x), std::pow(v1.y, v2.y));
	}

	template<typename T>
	inline Vector2t<T> random(const Vector2t<T>& min, const Vector2t<T>& max) noexcept
	{
		return Vector2t<T>(random(min.x, max.x), random(min.y, max.y));
	}

	template<typename T>
	inline Vector2t<T> ceil(const Vector2t<T>& v) noexcept
	{
		return Vector2t<T>(std::ceil(v.x), std::ceil(v.y));
	}

	template<typename T>
	inline Vector2t<T> floor(const Vector2t<T>& v) noexcept
	{
		return Vector2t<T>(std::floor(v.x), std::floor(v.y));
	}

	template<typename T>
	static Vector2t<T> barycentric(const Vector2t<T>& p1, const Vector2t<T>& p2, const Vector2t<T>& p3, const Vector2t<T>& p) noexcept
	{
		// http://www.blackpawn.com/texts/pointinpoly/

		float2 v0 = p3 - p1;
		float2 v1 = p2 - p1;
		float2 v2 = p - p1;

		float dot00 = math::dot(v0, v0);
		float dot01 = math::dot(v0, v1);
		float dot02 = math::dot(v0, v2);
		float dot11 = math::dot(v1, v1);
		float dot12 = math::dot(v1, v2);

		float invDenom = 1.0f / (dot00 * dot11 - dot01 * dot01);
		float u = (dot11 * dot02 - dot01 * dot12) * invDenom;
		float v = (dot00 * dot12 - dot01 * dot02) * invDenom;

		return float2(u, v);
	}
}

_NAME_END

#endif