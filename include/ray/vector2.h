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
	Vector2t(T xy) noexcept : x(xy), y(xy) {}
    Vector2t(T xx, T yy) noexcept : x(xx), y(yy) {}
    Vector2t(const Vector2t<T>& v) noexcept : x(v.x), y(v.y) {}

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

	T dot(const Vector2t<T>& v) const noexcept { return x * v.x + y * v.y; }

    T length() const noexcept { return sqrt(length2()); }
    T length2() const noexcept { return x * x + y * y; }

    T distance(const Vector2t<T>& v) const noexcept { return (v - *this).length(); }
    T sqrDistance(const Vector2t<T>& v) const noexcept { return (v - *this).length2(); }

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

    T normalize() noexcept
    {
        T magSq = length2();

        if (magSq > static_cast<T>(0.0))
        {
            T invSqrt = 1 / sqrt(magSq);
            *this *= invSqrt;
        }

        return magSq;
    }

    Vector2t<T> negate() const noexcept
    {
        return Vector2t<T>(-x, -y);
    }

    pointer ptr() noexcept { return (pointer)this; }
    const_pointer ptr() const noexcept { return (const_pointer)this;}
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
inline Vector2t<T> min(const Vector2t<T>& a, const Vector2t<T>& b) noexcept
{
    return Vector2t<T>(std::min(a.x, b.x), std::min(a.y, b.y));
}

template <typename T>
inline Vector2t<T> max(const Vector2t<T>& a, const Vector2t<T>& b) noexcept
{
    return Vector2t<T>(std::max(a.x, b.x), std::max(a.y, b.y));
}

template <typename T>
inline bool operator==(const Vector2t<T>& v1, const Vector2t<T>& v2) noexcept
{
    return (v1.x == v2.x && v1.y == v2.y);
}

template <typename T>
inline bool operator!=(const Vector2t<T>& v1, const Vector2t<T>& v2) noexcept
{
    return (v1.x != v2.x || v1.y != v2.y);
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

_NAME_END

#endif
