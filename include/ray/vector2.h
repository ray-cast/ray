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

_NAME_BEGIN

template<typename _Ty>
class Vector2t
{
public:
    typedef Vector2t<_Ty> _Myt;

    typedef _Ty value_type;
    typedef typename trait::_typeaddition<_Ty>::pointer pointer;
    typedef typename trait::_typeaddition<_Ty>::const_pointer const_pointer;
    typedef typename trait::_typeaddition<_Ty>::reference reference;
    typedef typename trait::_typeaddition<_Ty>::const_reference const_reference;

    value_type x, y;

    static const _Myt Zero;
    static const _Myt One;
    static const _Myt UnitX;
    static const _Myt UnitY;

    Vector2t() noexcept
    {
    }

    Vector2t(value_type xx, value_type yy) noexcept
        :x(xx)
        , y(yy)
    {
    }

    Vector2t(value_type xx, value_type yy, bool normal) noexcept
        : x(xx)
        , y(yy)
    {
        if (normal)
            this->normalize();
    }

    Vector2t(const _Myt& copy) noexcept
        :x(copy.x)
        , y(copy.y)
    {
    }

    _Myt& operator+=(const _Myt& v) noexcept
    {
        x += v.x;
        y += v.y;
        return *this;
    }

    _Myt& operator-=(const _Myt& v) noexcept
    {
        x -= v.x;
        y -= v.y;
        return *this;
    }

    _Myt& operator*=(const _Myt& v) noexcept
    {
        x *= v.x;
        y *= v.y;
        return *this;
    }

    _Myt& operator/=(const _Myt& v) noexcept
    {
        x /= v.x;
        y /= v.y;
        return *this;
    }

    _Myt& operator*=(value_type scale) noexcept
    {
        x *= scale;
        y *= scale;
        return *this;
    }

    _Myt& operator/=(value_type scale) noexcept
    {
        x /= scale;
        y /= scale;
        return *this;
    }

    _Myt operator-() noexcept
    {
        return _Myt(-x, -y);
    }

    value_type operator[](unsigned int i) const noexcept
    {
        return *(&x + i);
    }

    reference operator[](unsigned int i) noexcept
    {
        return *(&x + i);
    }

    _Myt& operator= (value_type f) noexcept
    {
        x = y = f;
        return *this;
    }

    void set(value_type val) noexcept
    {
        x = y = val;
    }

    void set(value_type xx, value_type yy) noexcept
    {
        x = xx;
        y = yy;
    }

    void zero() noexcept
    {
        this->set(0.0);
    }

    void one() noexcept
    {
        this->set(1.0);
    }

    void up() noexcept
    {
        x = 0.0;
        y = 1.0;
    }

    value_type length() const noexcept
    {
        return sqrt(length2());
    }

    value_type length2() const noexcept
    {
        return x * x + y * y;
    }

    value_type distance(const _Myt& v) const noexcept
    {
        return (v - *this).length();
    }

    value_type sqrDistance(const _Myt& v) const noexcept
    {
        return (v - *this).dot();
    }

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

    value_type normalize() noexcept
    {
        value_type magSq = length2();

        if (magSq > static_cast<value_type>(0.0))
        {
            value_type invSqrt = 1 / sqrt(magSq);
            *this *= invSqrt;
        }

        return magSq;
    }

    _Myt negate() const noexcept
    {
        return _Myt(-x, -y);
    }

    pointer ptr() noexcept
    {
        return (pointer)this;
    }

    const_pointer ptr() const noexcept
    {
        return (const_pointer)this;
    }

    pointer data() noexcept
    {
        return (pointer)this;
    }

    const_pointer data() const noexcept
    {
        return (const_pointer)this;
    }
};

template<>
class Vector2t<std::int8_t>
{
public:
    typedef Vector2t<std::int8_t> _Myt;

    typedef std::int8_t int_type;
    typedef trait::_typeaddition<int_type>::pointer pointer;
    typedef trait::_typeaddition<int_type>::const_pointer const_pointer;
    typedef trait::_typeaddition<int_type>::reference reference;
    typedef trait::_typeaddition<int_type>::const_reference const_reference;

    int_type x, y;

    static const _Myt Zero;
    static const _Myt One;
    static const _Myt UnitX;
    static const _Myt UnitY;

    Vector2t() noexcept
    {
    }

    Vector2t(const Vector2t& copy) noexcept
        :x(copy.x), y(copy.y)
    {
    }

    Vector2t(int_type xx, int_type yy) noexcept
        : x(xx)
        , y(yy)
    {
    }

    explicit Vector2t(int_type v) noexcept
        : x(v)
        , y(v)
    {
    }

    _Myt& operator+=(const _Myt& v) noexcept
    {
        x += v.x;
        y += v.y;
        return *this;
    }

    _Myt& operator-=(const _Myt& v) noexcept
    {
        x -= v.x;
        y -= v.y;
        return *this;
    }

    _Myt& operator*=(const _Myt& v) noexcept
    {
        x *= v.x;
        y *= v.y;
        return *this;
    }

    _Myt& operator/=(const _Myt& v) noexcept
    {
        x /= v.x;
        y /= v.y;
        return *this;
    }

    _Myt& operator*=(int_type scale) noexcept
    {
        x *= scale;
        y *= scale;
        return *this;
    }

    _Myt& operator/=(int_type scale) noexcept
    {
        x /= scale;
        y /= scale;
        return *this;
    }

    _Myt operator-() noexcept
    {
        return Vector2t(-x, -y);
    }

    void set(int_type val) noexcept
    {
        x = y = val;
    }

    void set(int_type xx, int_type yy) noexcept
    {
        x = xx;
        y = yy;
    }

    void zero() noexcept
    {
        set(0);
    }

    void one() noexcept
    {
        set(1);
    }

    void up() noexcept
    {
        x = 0; y = 1;
    }

    int_type min() const noexcept
    {
        return x < y ? x : y;
    }

    int_type max() const noexcept
    {
        return x > y ? x : y;
    }

    _Myt negate() const noexcept
    {
        return _Myt(-x, -y);
    }

    int_type& getComponent(int_type index) noexcept
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

    pointer ptr() noexcept
    {
        return (pointer)this;
    }

    const_pointer ptr() const noexcept
    {
        return (const_pointer)this;
    }

    pointer data() noexcept
    {
        return (pointer)this;
    }

    const_pointer data() const noexcept
    {
        return (const_pointer)this;
    }
};

template<>
class Vector2t<unsigned>
{
public:
    typedef Vector2t<unsigned> _Myt;

    typedef unsigned int_type;
    typedef trait::_typeaddition<int_type>::pointer pointer;
    typedef trait::_typeaddition<int_type>::const_pointer const_pointer;
    typedef trait::_typeaddition<int_type>::reference reference;
    typedef trait::_typeaddition<int_type>::const_reference const_reference;

    int_type x, y;

    static const _Myt Zero;
    static const _Myt One;
    static const _Myt UnitX;
    static const _Myt UnitY;

    Vector2t() noexcept
    {
    }

    Vector2t(const _Myt& copy) noexcept
        :x(copy.x)
        , y(copy.y)
    {
    }

    Vector2t(int_type xx, int_type yy) noexcept
        : x(xx)
        , y(yy)
    {
    }

    explicit Vector2t(int_type v) noexcept
        : x(v)
        , y(v)
    {
    }

    _Myt& operator+=(const _Myt& v) noexcept
    {
        x += v.x;
        y += v.y;
        return *this;
    }

    _Myt& operator-=(const _Myt& v) noexcept
    {
        x -= v.x;
        y -= v.y;
        return *this;
    }

    _Myt& operator*=(const _Myt& v) noexcept
    {
        x *= v.x;
        y *= v.y;
        return *this;
    }

    _Myt& operator/=(const _Myt& v) noexcept
    {
        x /= v.x;
        y /= v.y;
        return *this;
    }

    _Myt& operator*=(int_type scale) noexcept
    {
        x *= scale;
        y *= scale;
        return *this;
    }

    _Myt& operator/=(int_type scale) noexcept
    {
        x /= scale;
        y /= scale;
        return *this;
    }

    void set(int_type val) noexcept
    {
        x = y = val;
    }

    void set(int_type xx, int_type yy) noexcept
    {
        x = xx;
        y = yy;
    }

    void zero() noexcept
    {
        set(0);
    }

    void one() noexcept
    {
        set(1);
    }

    void up() noexcept
    {
        x = 0;
        y = 1;
    }

    int_type dot() const noexcept
    {
        return x * x + y * y;
    }

    int_type dot(const _Myt& v) const noexcept
    {
        return x * v.x + y * v.y;
    }

    int_type length2() const noexcept
    {
        return dot();
    }

    int_type sqrDistance(const _Myt& v) const noexcept
    {
        return _Myt(v.x - x, v.y - y).dot();
    }

    int_type min() const noexcept
    {
        return std::min(x, y);
    }

    int_type max() const noexcept
    {
        return std::max(x, y);
    }

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

    pointer ptr() noexcept
    {
        return (pointer)this;
    }

    const_pointer ptr() const noexcept
    {
        return (const_pointer)this;
    }

    pointer data() noexcept
    {
        return (pointer)this;
    }

    const_pointer data() const noexcept
    {
        return (const_pointer)this;
    }
};

template<typename T> const Vector2t<T> Vector2t<T>::Zero = Vector2t<T>((T)0, (T)0);
template<typename T> const Vector2t<T> Vector2t<T>::One = Vector2t<T>((T)1, (T)1);
template<typename T> const Vector2t<T> Vector2t<T>::UnitX = Vector2t<T>((T)1, (T)0);
template<typename T> const Vector2t<T> Vector2t<T>::UnitY = Vector2t<T>((T)0, (T)1);

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
