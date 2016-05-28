// +----------------------------------------------------------------------
// | Project : ray.
// | All rights reserved.
// +----------------------------------------------------------------------
// | Copyright (c) 2013-2015.
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

	Vector2t<T>& operator+=(T scale) noexcept { x += scale; y += scale; return *this; }
	Vector2t<T>& operator-=(T scale) noexcept { x -= scale; y -= scale; return *this; }
	Vector2t<T>& operator*=(T scale) noexcept { x *= scale; y *= scale; return *this; }
	Vector2t<T>& operator/=(T scale) noexcept { x /= scale; y /= scale; return *this; }

    Vector2t<T>& operator+=(const Vector2t<T>& v) noexcept { x += v.x; y += v.y; return *this; }
    Vector2t<T>& operator-=(const Vector2t<T>& v) noexcept { x -= v.x; y -= v.y; return *this; }
    Vector2t<T>& operator*=(const Vector2t<T>& v) noexcept { x *= v.x; y *= v.y; return *this; }
    Vector2t<T>& operator/=(const Vector2t<T>& v) noexcept { x /= v.x; y /= v.y; return *this; }

	template <typename S>
	explicit operator Vector2t<S>() const
	{
		return Vector2t<S>(static_cast<S>(x), static_cast<S>(y));
	}

    T operator[](unsigned int i) const noexcept { return *(&x + i); }
    reference operator[](unsigned int i) noexcept { return *(&x + i); }

    void set(T val) noexcept { x = y = val; }
    void set(T xx, T yy) noexcept { x = xx; y = yy; }

    reference getComponent(unsigned char index) noexcept
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
    const_pointer ptr() const noexcept { return (const_pointer)this;}
    pointer data() noexcept { return (pointer)this; }
    const_pointer data() const noexcept { return (const_pointer)this; }

    template<typename ostream>
    ostream& operator << (ostream& os)
    {
        os << x << ", " << y;
        return os;
    }

    template<typename istream>
    istream& operator >> (istream& is)
    {
        is >> x;
        is.ignore(2);
        is >> y;
        return is;
    }
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
    return (v * (1 / f));
}

template <typename T>
inline Vector2t<T> operator/(const Vector2t<T>& v, T f) noexcept
{
    return (v * (1 / f));
}

template <typename T>
inline Vector2t<T> operator/(const Vector2t<T>& v, const Vector2t<T>& v2) noexcept
{
    return Vector2t<T>(v.x / v2.x, v.y / v2.y);
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
	Vector2t<T> cross(const Vector2t<T>& v1, const Vector3t<T>& v2) noexcept
	{
		return Vector2t<T>(
			v1.y * v2.x - v1.x * v2.y,
			v1.x * v2.y - v1.y * v2.x);
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
	inline T distance(const Vector2t<T>& v) noexcept
	{
		return length(v - *this);
	}

	template<typename T>
	inline T sqrDistance(const Vector2t<T>& v1, const Vector2t<T>& v2) noexcept
	{
		return length2(v1 - v2);
	}

	template<typename T>
	inline bool equal(const Vector2t<T>& v1, const Vector2t<T>& v2) noexcept
	{
		return math::equal<T>(v1.x, v2.x) && math::equal<T>(v1.y, v2.y);
	}

	template<typename T>
	inline Vector2t<T> negate(const Vector2t<T>& v) noexcept
	{
		return Vector2t<T>(-v.x, -v.y);
	}

	template<typename T>
	inline Vector2t<T> clamp(const Vector2t<T>& t, T min, T max)
	{
		return Vector2t<T>(
			std::max(min, std::min(max, t.x)),
			std::max(min, std::min(max, t.y))
			);
	}

	template<typename T>
	inline Vector2t<T> saturate(const Vector2t<T>& v) noexcept
	{
		return clamp(v, 0.0f, 1.0f);
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
	inline Vector2t<T> abs(const Vector2t<T>& v)
	{
		return Vector2t<T>(std::abs(v.x), std::abs(v.y));
	}

	template<typename T>
	inline Vector2t<T> cos(const Vector2t<T>& v)
	{
		return Vector2t<T>(std::cos(v.x), std::cos(v.y));
	}

	template<typename T>
	inline Vector2t<T> sin(const Vector2t<T>& v)
	{
		return Vector2t<T>(std::sin(v.x), std::sin(v.y));
	}

	template<typename T>
	inline Vector2t<T> tan(const Vector2t<T>& v)
	{
		return Vector2t<T>(std::tan(v.x), std::tan(v.y));
	}

	template<typename T>
	inline Vector2t<T> acos(const Vector2t<T>& v)
	{
		return Vector2t<T>(std::acos(v.x), std::acos(v.y));
	}

	template<typename T>
	inline Vector2t<T> asin(const Vector2t<T>& v)
	{
		return Vector2t<T>(std::asin(v.x), std::asin(v.y));
	}

	template<typename T>
	inline Vector2t<T> atan(const Vector2t<T>& v)
	{
		return Vector2t<T>(std::atan(v.x), std::atan(v.y));
	}

	template<typename T>
	inline Vector2t<T> exp(const Vector2t<T>& v)
	{
		return Vector2t<T>(std::exp(v.x), std::exp(v.y));
	}

	template<typename T>
	inline Vector2t<T> log(const Vector2t<T>& v)
	{
		return Vector2t<T>(std::log(v.x), std::log(v.y));
	}

	template<typename T>
	inline Vector2t<T> log10(const Vector2t<T>& v)
	{
		return Vector2t<T>(std::log10(v.x), std::log10(v.y));
	}

	template<typename T>
	inline Vector2t<T> pow(const Vector2t<T>& v1, const Vector2t<T>& v2)
	{
		return Vector2t<T>(std::pow(v1.x, v2.x), std::pow(v1.y, v2.y));
	}

	template<typename T>
	inline Vector2t<T> random(const Vector2t<T>& min, const Vector2t<T>& max)
	{
		return Vector2t<T>(random(min.x, max.x), random(min.y, max.y));
	}
}

_NAME_END

#endif
