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
#ifndef _H_VECTOR3_H_
#define _H_VECTOR3_H_

#include <ray/vector2.h>

_NAME_BEGIN

template<typename T>
class Vector3t
{
public:
	typedef typename trait::_typeaddition<T>::value_type value_type;
	typedef typename trait::_typeaddition<T>::pointer pointer;
	typedef typename trait::_typeaddition<T>::const_pointer const_pointer;
	typedef typename trait::_typeaddition<T>::reference reference;
	typedef typename trait::_typeaddition<T>::const_reference const_reference;

    T x, y, z;

    static const Vector3t<T> Zero;
    static const Vector3t<T> One;
    static const Vector3t<T> UnitX;
    static const Vector3t<T> UnitY;
    static const Vector3t<T> UnitZ;

    Vector3t() {}
    Vector3t(T x, T y, T z) :x(x), y(y), z(z) { }
    Vector3t(T x, T y, T z, bool normal) :x(x), y(y), z(z) { if (normal) this->normalize(); }
    Vector3t(const Vector2t<T>& _xy, T _z) :x(_xy.x), y(_xy.y), z(_z) {}
    Vector3t(const Vector3t<T>& v) :x(v.x), y(v.y), z(v.z) {}
    explicit Vector3t(T xyz) :x(xyz), y(xyz), z(xyz) {}
    explicit Vector3t(const Vector2t<T>& v) :x(v.x), y(v.y), z(0.0) {}
    explicit Vector3t(const Vector3t<T>& v, bool normal) :x(v.x), y(v.y), z(v.z) { if (normal) this->normalize(); }
    explicit Vector3t(const Vector4t<T>& v) :x(v.x / v.w), y(v.y / v.w), z(v.z / v.w) {}

    template<typename S>
    Vector3t<T>& operator+=(const Vector3t<S>& pt) { x += pt.x; y += pt.y; z += pt.z; return *this; }

    template<typename S>
    Vector3t<T>& operator-=(const Vector3t<S>& pt) { x -= pt.x; y -= pt.y; z -= pt.z; return *this; }

    template<typename S>
    Vector3t<T>& operator*=(const Vector3t<S>& pt) { x *= pt.x; y *= pt.y; z *= pt.z; return *this; }

    template<typename S>
    Vector3t<T>& operator/=(const Vector3t<S>& pt) { x /= pt.x; y /= pt.y; z /= pt.z; return *this; }

    template<typename S>
    Vector3t<T>& operator+=(const S sz) { x += sz; y += sz; z += sz; return *this; }

    template<typename S>
    Vector3t<T>& operator-=(const S sz) { x -= sz; y -= sz; z -= sz; return *this; }

    template<typename S>
    Vector3t<T>& operator*=(const S sz) { x *= sz; y *= sz; z *= sz; return *this; }

    template<typename S>
    Vector3t<T>& operator/=(const S sz) { x /= sz; y /= sz; z /= sz; return *this; }

    template <typename U>
    explicit operator Vector3t<U>() const
    {
        return Vector3t<U>(static_cast<U>(x), static_cast<U>(y), static_cast<U>(z));
    }

    T operator[](unsigned int i) const
    {
        return *(&x + i);
    }

    T& operator[](unsigned int i)
    {
        return *(&x + i);
    }

    void set(T val) { x = y = z = val; }
    void set(T xx, T yy, T zz) { x = xx; y = yy; z = zz; }
    void zero() { set(0.0); }
    void one() { set(1.0); }

    T dot(const Vector3t<T>& v) const { return x * v.x + y * v.y + z * v.z; }

    Vector3t<T> cross(const Vector3t<T>& v) const
    {
        return Vector3t<T>(
            y * v.z - z * v.y,
            z * v.x - x * v.z,
            x * v.y - y * v.x);
    }

    float normalize()
    {
        T magSq = length2();
        if (magSq > 0)
        {
            T invSqrt = 1 / sqrt(magSq);
            *this *= invSqrt;
        }

        return magSq;
    }

    T length() const { return sqrt(this->length2()); }
    T length2() const { return x * x + y * y + z * z; }

    T distance(const Vector3t<T>& v) const { return (v - *this).length(); }
    T sqrDistance(const Vector3t<T>& v) const { return (v - *this).length2(); }

    T min(T t1, T t2) const { return t1 < t2 ? t1 : t2; }
    T max(T t1, T t2) const { return t1 > t2 ? t1 : t2; }
    T min() const { return min(min(x, y), z); }
    T max() const { return max(max(x, y), z); }

    T& getComponent(unsigned char index)
    {
        switch (index)
        {
        case COMPONENT_X: return x;
        case COMPONENT_Y: return y;
        case COMPONENT_Z: return z;
        case 'x': return x;
        case 'y': return y;
        case 'z': return z;
        default:
            assert(false);
        }
        return x;
    }

    T* ptr() { return (T*)this; }
    const T* ptr() const { return (const T*)this; }
    T* data() { return (T*)this; }
    const T* data() const { return (const T*)this; }
};

template<typename T> const Vector3t<T> Vector3t<T>::Zero = Vector3t<T>((T)0.0, (T)0.0, (T)0.0);
template<typename T> const Vector3t<T> Vector3t<T>::One = Vector3t<T>((T)1.0, (T)1.0, (T)1.0);
template<typename T> const Vector3t<T> Vector3t<T>::UnitX = Vector3t<T>((T)1.0, (T)0.0, (T)0.0);
template<typename T> const Vector3t<T> Vector3t<T>::UnitY = Vector3t<T>((T)0.0, (T)1.0, (T)0.0);
template<typename T> const Vector3t<T> Vector3t<T>::UnitZ = Vector3t<T>((T)0.0, (T)0.0, (T)1.0);

template <typename T>
inline Vector3t<T> min(const Vector3t<T>& a, const Vector3t<T>& b)
{
    return Vector3t<T>(std::min(a.x, b.x), std::min(a.y, b.y), std::min(a.z, b.z));
}

template <typename T>
inline Vector3t<T> max(const Vector3t<T>& a, const Vector3t<T>& b)
{
    return Vector3t<T>(std::max(a.x, b.x), std::max(a.y, b.y), std::max(a.z, b.z));
}

template<typename T>
inline Vector3t<T> abs(const Vector3t<T>& v)
{
    return Vector3t<T>(::abs(v.x), ::abs(v.y), ::abs(v.z));
}

template<typename T>
inline bool operator==(const Vector3t<T>& v1, const Vector3t<T>& v2)
{
    return v1.x == v2.x && v1.y == v2.y && v1.z == v2.z;
}

template<typename T>
inline bool operator!=(const Vector3t<T>& v1, const Vector3t<T>& v2)
{
    return v1.x != v2.x || v1.y != v2.y || v1.z != v2.z;
}

template<typename T>
inline bool operator<(const Vector3t<T>& v1, const Vector3t<T>& v2)
{
    return v1.x < v2.x && v1.y < v2.y && v1.z < v2.z;
}

template<typename T>
inline bool operator>(const Vector3t<T>& v1, const Vector3t<T>& v2)
{
    return v1.x > v2.x && v1.y > v2.y && v1.z > v2.z;
}

template<typename T>
inline bool operator<=(const Vector3t<T>& v1, const Vector3t<T>& v2)
{
    return v1.x <= v2.x && v1.y <= v2.y && v1.z <= v2.z;
}

template<typename T>
inline bool operator>=(const Vector3t<T>& v1, const Vector3t<T>& v2)
{
    return v1.x >= v2.x && v1.y >= v2.y && v1.z >= v2.z;
}

template<typename T>
inline Vector3t<T> operator+(const Vector3t<T>& v1, const Vector3t<T>& v2)
{
    return Vector3t<T>(v1.x + v2.x, v1.y + v2.y, v1.z + v2.z);
}

template<typename T>
inline Vector3t<T> operator-(const Vector3t<T>& v1, const Vector3t<T>& v2)
{
    return Vector3t<T>(v1.x - v2.x, v1.y - v2.y, v1.z - v2.z);
}

template<typename T>
inline Vector3t<T> operator*(const Vector3t<T>& v1, const Vector3t<T>& v2)
{
    return Vector3t<T>(v1.x * v2.x, v1.y * v2.y, v1.z * v2.z);
}

template<typename T>
inline Vector3t<T> operator/(const Vector3t<T>& v1, const Vector3t<T>& v2)
{
    return Vector3t<T>(v1.x / v2.x, v1.y / v2.y, v1.z / v2.z);
}

template<typename T>
inline Vector3t<T> operator-(const Vector3t<T>& v)
{
    return Vector3t<T>(-v.x, -v.y, -v.z);
}

template <typename T>
inline Vector3t<T> operator~(const Vector3t<T>& v)
{
    Vector3t<T> n(v);
    n.normalize();
    return n;
}

template<typename _Tx, typename _Ty>
inline bool operator==(const Vector3t<_Tx>& v, const _Ty value)
{
    return v.x == value && v.y == value && v.z == value;
}

template<typename _Tx, typename _Ty>
inline bool operator==(const _Ty value, const Vector3t<_Tx>& v)
{
    return v.x == value && v.y == value && v.z == value;
}

template<typename _Tx, typename _Ty>
inline bool operator!=(const Vector3t<_Tx>& v, const _Ty value)
{
    return v.x != value || v.y != value || v.z != value;
}

template<typename _Tx, typename _Ty>
inline bool operator!=(const _Ty value, const Vector3t<_Tx>& v)
{
    return v.x != value && v.y != value && v.z != value;
}

template<typename _Tx, typename _Ty>
inline bool operator<(const Vector3t<_Tx>& v, const _Ty value)
{
    return v.x < value && v.y < value && v.z < value;
}

template<typename _Tx, typename _Ty>
inline bool operator<(const _Ty value, const Vector3t<_Tx>& v)
{
    return value < v.x && value < v.y && value < v.z;
}

template<typename _Tx, typename _Ty>
inline bool operator>(const Vector3t<_Tx>& v, const _Ty value)
{
    return v.x > value && v.y > value && v.z > value;
}

template<typename _Tx, typename _Ty>
inline bool operator>(const _Ty value, const Vector3t<_Tx>& v)
{
    return value > v.x && value > v.y && value > v.z;
}

template<typename _Tx, typename _Ty>
inline bool operator<=(const Vector3t<_Tx>& v, const _Ty value)
{
    return v.x <= value && v.y <= value && v.z <= value;
}

template<typename _Tx, typename _Ty>
inline bool operator<=(const _Ty value, const Vector3t<_Tx>& v)
{
    return value <= v.x && value <= v.y && value <= v.z;
}

template<typename _Tx, typename _Ty>
inline bool operator>=(const Vector3t<_Tx>& v, const _Ty value)
{
    return v.x >= value && v.y >= value && v.z >= value;
}

template<typename _Tx, typename _Ty>
inline bool operator>=(const _Ty value, const Vector3t<_Tx>& v)
{
    return value >= v.x && value >= v.y && value >= v.z;
}

template<typename _Tx, typename _Ty>
inline Vector3t<_Tx> operator*(const Vector3t<_Tx>& v, _Ty value)
{
    return Vector3t<_Tx>((_Tx)(v.x * value), (_Tx)(v.y * value), (_Tx)(v.z * value));
}

template<typename _Tx, typename _Ty>
inline Vector3t<_Tx> operator*(_Ty value, const Vector3t<_Tx>& v)
{
    return Vector3t<_Tx>(v.x * value, v.y * value, v.z * value);
}

template<typename _Tx, typename _Ty>
inline Vector3t<_Tx> operator/(_Ty value, const Vector3t<_Tx>& v)
{
    return Vector3t<_Tx>(value / v.x, value / v.y, value / v.z);
}

template<typename _Tx, typename _Ty>
inline Vector3t<_Tx> operator/(const Vector3t<_Tx>& v, _Ty value)
{
    return Vector3t<_Tx>(v.x / value, v.y / value, v.z * value);
}

template<typename T>
inline Vector3t<T> random(const Vector3t<T>& min, const Vector3t<T>& max)
{
    return Vector3t<T>(random(min.x, max.x), random(min.y, max.y), random(min.z, max.z));
}

_NAME_END

#endif
