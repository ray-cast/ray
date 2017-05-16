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
	static const Vector3t<T> Right;
	static const Vector3t<T> Up;
	static const Vector3t<T> Forward;

	Vector3t() noexcept {}
	Vector3t(T xx, T yy, T zz) noexcept :x(xx), y(yy), z(zz) { }
	Vector3t(T xx, const Vector2t<T>& yz) noexcept :x(xx), y(yz.x), z(yz.y) {}
	Vector3t(const Vector2t<T>& xy, T zz) noexcept :x(xy.x), y(xy.y), z(zz) {}
	explicit Vector3t(const Vector4t<T>& v) noexcept :x(v.x / v.w), y(v.y / v.w), z(v.z / v.w) {}
	explicit Vector3t(T xyz) noexcept :x(xyz), y(xyz), z(xyz) {}
	
	template<typename S, typename = std::enable_if<std::is_pointer<S>::value>>
	explicit Vector3t(S xyz[3]) noexcept
		: x(static_cast<typename trait::_typeaddition<S>::value_type>(xyz[0]))
		, y(static_cast<typename trait::_typeaddition<S>::value_type>(xyz[1]))
		, z(static_cast<typename trait::_typeaddition<S>::value_type>(xyz[2]))
	{
		assert(xyz[0] <= std::numeric_limits<typename trait::_typeaddition<S>::value_type>::max());
		assert(xyz[1] <= std::numeric_limits<typename trait::_typeaddition<S>::value_type>::max());
		assert(xyz[2] <= std::numeric_limits<typename trait::_typeaddition<S>::value_type>::max());
	}

	Vector3t<T>& operator+=(const T sz) noexcept { x += sz; y += sz; z += sz; return *this; }
	Vector3t<T>& operator-=(const T sz) noexcept { x -= sz; y -= sz; z -= sz; return *this; }
	Vector3t<T>& operator*=(const T sz) noexcept { x *= sz; y *= sz; z *= sz; return *this; }
	Vector3t<T>& operator/=(const T sz) noexcept { x /= sz; y /= sz; z /= sz; return *this; }

	Vector3t<T>& operator+=(const Vector2t<T>& rt) noexcept { x += rt.x; y += rt.y; return *this; }
	Vector3t<T>& operator-=(const Vector2t<T>& rt) noexcept { x -= rt.x; y -= rt.y; return *this; }
	Vector3t<T>& operator/=(const Vector2t<T>& rt) noexcept { x *= rt.x; y *= rt.y; return *this; }
	Vector3t<T>& operator*=(const Vector2t<T>& rt) noexcept { x /= rt.x; y /= rt.y; return *this; }

	Vector3t<T>& operator+=(const Vector3t<T>& pt) noexcept { x += pt.x; y += pt.y; z += pt.z; return *this; }
	Vector3t<T>& operator-=(const Vector3t<T>& pt) noexcept { x -= pt.x; y -= pt.y; z -= pt.z; return *this; }
	Vector3t<T>& operator*=(const Vector3t<T>& pt) noexcept { x *= pt.x; y *= pt.y; z *= pt.z; return *this; }
	Vector3t<T>& operator/=(const Vector3t<T>& pt) noexcept { x /= pt.x; y /= pt.y; z /= pt.z; return *this; }

	template<typename S, typename = std::enable_if<!std::is_pointer<S>::value>>
	explicit operator Vector3t<S>() const noexcept
	{
		assert(x <= std::numeric_limits<typename trait::_typeaddition<S>::value_type>::max());
		assert(y <= std::numeric_limits<typename trait::_typeaddition<S>::value_type>::max());
		assert(z <= std::numeric_limits<typename trait::_typeaddition<S>::value_type>::max());

		return Vector3t<S>(static_cast<S>(x), static_cast<S>(y), static_cast<S>(z));
	}

	Vector2t<T> xx()  const noexcept { return Vector2t<T>(x, x); }
	Vector2t<T> xy()  const noexcept { return Vector2t<T>(x, y); }
	Vector2t<T> xz()  const noexcept { return Vector2t<T>(x, z); }
	Vector3t<T> xxx() const noexcept { return Vector3t<T>(x, x, x); }
	Vector3t<T> xxy() const noexcept { return Vector3t<T>(x, x, y); }
	Vector3t<T> xxz() const noexcept { return Vector3t<T>(x, x, z); }
	Vector3t<T> xyx() const noexcept { return Vector3t<T>(x, y, x); }
	Vector3t<T> xyy() const noexcept { return Vector3t<T>(x, y, y); }
	Vector3t<T> xyz() const noexcept { return Vector3t<T>(x, y, z); }
	Vector3t<T> xzx() const noexcept { return Vector3t<T>(x, z, x); }
	Vector3t<T> xzy() const noexcept { return Vector3t<T>(x, z, y); }
	Vector3t<T> xzz() const noexcept { return Vector3t<T>(x, z, z); }
	Vector2t<T> yx()  const noexcept { return Vector2t<T>(y, x); }
	Vector2t<T> yy()  const noexcept { return Vector2t<T>(y, y); }
	Vector2t<T> yz()  const noexcept { return Vector2t<T>(y, z); }
	Vector3t<T> yxx() const noexcept { return Vector3t<T>(y, x, x); }
	Vector3t<T> yxy() const noexcept { return Vector3t<T>(y, x, y); }
	Vector3t<T> yxz() const noexcept { return Vector3t<T>(y, x, z); }
	Vector3t<T> yyx() const noexcept { return Vector3t<T>(y, y, x); }
	Vector3t<T> yyy() const noexcept { return Vector3t<T>(y, y, y); }
	Vector3t<T> yyz() const noexcept { return Vector3t<T>(y, y, z); }
	Vector3t<T> yzx() const noexcept { return Vector3t<T>(y, z, x); }
	Vector3t<T> yzy() const noexcept { return Vector3t<T>(y, z, y); }
	Vector3t<T> yzz() const noexcept { return Vector3t<T>(y, z, z); }
	Vector2t<T> zx()  const noexcept { return Vector2t<T>(z, x); }
	Vector2t<T> zy()  const noexcept { return Vector2t<T>(z, y); }
	Vector2t<T> zz()  const noexcept { return Vector2t<T>(z, z); }
	Vector3t<T> zxx() const noexcept { return Vector3t<T>(z, x, x); }
	Vector3t<T> zxy() const noexcept { return Vector3t<T>(z, x, y); }
	Vector3t<T> zxz() const noexcept { return Vector3t<T>(z, x, z); }
	Vector3t<T> zyx() const noexcept { return Vector3t<T>(z, y, x); }
	Vector3t<T> zyy() const noexcept { return Vector3t<T>(z, y, y); }
	Vector3t<T> zyz() const noexcept { return Vector3t<T>(z, y, z); }
	Vector3t<T> zzx() const noexcept { return Vector3t<T>(z, z, x); }
	Vector3t<T> zzy() const noexcept { return Vector3t<T>(z, z, y); }
	Vector3t<T> zzz() const noexcept { return Vector3t<T>(z, z, z); }

	T operator[](std::uint8_t i) const noexcept
	{
		assert(i < 3);
		return *(&x + i);
	}

	T& operator[](std::uint8_t i) noexcept
	{
		assert(i < 3);
		return *(&x + i);
	}

	Vector3t<T>& set(T val) noexcept { x = y = z = val; return *this; }
	Vector3t<T>& set(T xx, T yy, T zz) noexcept { x = xx; y = yy; z = zz; return *this; }
	Vector3t<T>& set(T xx, const Vector2t<T>& yz) noexcept { x = xx; y = yz.x; z = yz.y; return *this; }
	Vector3t<T>& set(const Vector2t<T>& xy, T zz) noexcept { x = xy.x; y = xy.y; z = zz; return *this; }
	Vector3t<T>& set(const Vector4t<T>& v) noexcept { x = v.x / v.w; y = v.y / v.w; z = v.z / v.w; return *this; }

	template<typename S, typename = std::enable_if<std::is_pointer<S>::value>>
	Vector3t<T>& set(S xyz[3]) noexcept 
	{
		assert(xyz[0] <= std::numeric_limits<typename trait::_typeaddition<S>::value_type>::max());
		assert(xyz[1] <= std::numeric_limits<typename trait::_typeaddition<S>::value_type>::max());
		assert(xyz[2] <= std::numeric_limits<typename trait::_typeaddition<S>::value_type>::max());

		x = static_cast<typename trait::_typeaddition<S>::value_type>(xyz[0]); 
		y = static_cast<typename trait::_typeaddition<S>::value_type>(xyz[1]); 
		z = static_cast<typename trait::_typeaddition<S>::value_type>(xyz[2]); 

		return *this;
	}

	T& getComponent(std::uint8_t index) noexcept
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

	T* ptr() noexcept { return (T*)this; }
	const T* ptr() const noexcept { return (const T*)this; }
	T* data() noexcept { return (T*)this; }
	const T* data() const noexcept { return (const T*)this; }
};

template<typename T> const Vector3t<T> Vector3t<T>::Zero = Vector3t<T>((T)0.0, (T)0.0, (T)0.0);
template<typename T> const Vector3t<T> Vector3t<T>::One = Vector3t<T>((T)1.0, (T)1.0, (T)1.0);
template<typename T> const Vector3t<T> Vector3t<T>::UnitX = Vector3t<T>((T)1.0, (T)0.0, (T)0.0);
template<typename T> const Vector3t<T> Vector3t<T>::UnitY = Vector3t<T>((T)0.0, (T)1.0, (T)0.0);
template<typename T> const Vector3t<T> Vector3t<T>::UnitZ = Vector3t<T>((T)0.0, (T)0.0, (T)1.0);
template<typename T> const Vector3t<T> Vector3t<T>::Right = Vector3t<T>((T)1.0, (T)0.0, (T)0.0);
template<typename T> const Vector3t<T> Vector3t<T>::Up = Vector3t<T>((T)0.0, (T)1.0, (T)0.0);
template<typename T> const Vector3t<T> Vector3t<T>::Forward = Vector3t<T>((T)0.0, (T)0.0, (T)1.0);

template<typename T>
inline bool operator==(const Vector3t<T>& v1, const Vector3t<T>& v2) noexcept
{
	return v1.x == v2.x && v1.y == v2.y && v1.z == v2.z;
}

template<typename T>
inline bool operator!=(const Vector3t<T>& v1, const Vector3t<T>& v2) noexcept
{
	return !(v1 == v2);
}

template<typename T>
inline bool operator<(const Vector3t<T>& v1, const Vector3t<T>& v2) noexcept
{
	return v1.x < v2.x && v1.y < v2.y && v1.z < v2.z;
}

template<typename T>
inline bool operator>(const Vector3t<T>& v1, const Vector3t<T>& v2) noexcept
{
	return v1.x > v2.x && v1.y > v2.y && v1.z > v2.z;
}

template<typename T>
inline bool operator<=(const Vector3t<T>& v1, const Vector3t<T>& v2) noexcept
{
	return v1.x <= v2.x && v1.y <= v2.y && v1.z <= v2.z;
}

template<typename T>
inline bool operator>=(const Vector3t<T>& v1, const Vector3t<T>& v2) noexcept
{
	return v1.x >= v2.x && v1.y >= v2.y && v1.z >= v2.z;
}

template<typename T>
inline Vector3t<T> operator+(const Vector3t<T>& v1, const Vector3t<T>& v2) noexcept
{
	return Vector3t<T>(v1.x + v2.x, v1.y + v2.y, v1.z + v2.z);
}

template<typename T>
inline Vector3t<T> operator-(const Vector3t<T>& v1, const Vector3t<T>& v2) noexcept
{
	return Vector3t<T>(v1.x - v2.x, v1.y - v2.y, v1.z - v2.z);
}

template<typename T>
inline Vector3t<T> operator*(const Vector3t<T>& v1, const Vector3t<T>& v2) noexcept
{
	return Vector3t<T>(v1.x * v2.x, v1.y * v2.y, v1.z * v2.z);
}

template<typename T>
inline Vector3t<T> operator/(const Vector3t<T>& v1, const Vector3t<T>& v2) noexcept
{
	return Vector3t<T>(v1.x / v2.x, v1.y / v2.y, v1.z / v2.z);
}

template<typename T>
inline Vector3t<T> operator-(const Vector3t<T>& v) noexcept
{
	return Vector3t<T>(-v.x, -v.y, -v.z);
}

template<typename T>
inline bool operator==(const Vector3t<T>& v, const T value) noexcept
{
	return v.x == value && v.y == value && v.z == value;
}

template<typename T>
inline bool operator!=(const Vector3t<T>& v, const T value) noexcept
{
	return v.x != value || v.y != value || v.z != value;
}

template<typename T>
inline bool operator<(const Vector3t<T>& v, const T value) noexcept
{
	return v.x < value && v.y < value && v.z < value;
}

template<typename T>
inline bool operator>(const Vector3t<T>& v, const T value) noexcept
{
	return v.x > value && v.y > value && v.z > value;
}

template<typename T>
inline bool operator<=(const Vector3t<T>& v, const T value) noexcept
{
	return v.x <= value && v.y <= value && v.z <= value;
}

template<typename T>
inline bool operator>=(const Vector3t<T>& v, const T value) noexcept
{
	return v.x >= value && v.y >= value && v.z >= value;
}

template<typename T>
inline Vector3t<T> operator+(const Vector3t<T>& v, const T value) noexcept
{
	return Vector3t<T>((v.x + value), (v.y + value), (v.z + value));
}

template<typename T>
inline Vector3t<T> operator-(const Vector3t<T>& v, const T value) noexcept
{
	return Vector3t<T>((v.x - value), (v.y - value), (v.z - value));
}

template<typename T>
inline Vector3t<T> operator*(const Vector3t<T>& v, const T value) noexcept
{
	return Vector3t<T>((v.x * value), (v.y * value), (v.z * value));
}

template<typename T>
inline Vector3t<T> operator/(const Vector3t<T>& v, const T value) noexcept
{
	return Vector3t<T>((v.x / value), (v.y / value), (v.z / value));
}

template<typename T>
inline bool operator==(const T value, const Vector3t<T>& v) noexcept
{
	return v.x == value && v.y == value && v.z == value;
}

template<typename T>
inline bool operator!=(const T value, const Vector3t<T>& v) noexcept
{
	return v.x != value && v.y != value && v.z != value;
}

template<typename T>
inline bool operator<(const T value, const Vector3t<T>& v) noexcept
{
	return value < v.x && value < v.y && value < v.z;
}

template<typename T>
inline bool operator>(const T value, const Vector3t<T>& v) noexcept
{
	return value > v.x && value > v.y && value > v.z;
}

template<typename T>
inline bool operator<=(const T value, const Vector3t<T>& v) noexcept
{
	return value <= v.x && value <= v.y && value <= v.z;
}

template<typename T>
inline bool operator>=(const T value, const Vector3t<T>& v) noexcept
{
	return value >= v.x && value >= v.y && value >= v.z;
}

template<typename T>
inline Vector3t<T> operator+(const T value, const Vector3t<T>& v) noexcept
{
	return Vector3t<T>((value + v.x), (value + v.y), (value + v.z));
}

template<typename T>
inline Vector3t<T> operator-(const T value, const Vector3t<T>& v) noexcept
{
	return Vector3t<T>((value - v.x), (value - v.y), (value - v.z));
}

template<typename T>
inline Vector3t<T> operator*(const T value, const Vector3t<T>& v) noexcept
{
	return Vector3t<T>((value * v.x), (value * v.y), (value * v.z));
}

template<typename T>
inline Vector3t<T> operator/(const T value, const Vector3t<T>& v) noexcept
{
	return Vector3t<T>((value / v.x), (value / v.y), (value / v.z));
}

template<typename ostream, typename T, std::enable_if_t<trait::has_left_shift<ostream, T>::value, int> = 0>
inline ostream& operator << (ostream& os, const Vector3t<T>& v)
{
	os << v.x << ", " << v.y << ", " << v.z;
	return os;
}

template<typename istream, typename T, std::enable_if_t<trait::has_right_shift<istream>::value, int> = 0>
inline istream& operator >> (istream& is, Vector3t<T>& v)
{
	is >> v.x;
	is.ignore(2);
	is >> v.y;
	is.ignore(2);
	is >> v.z;
	return is;
}

namespace math
{
	template<typename T>
	inline T dot(const Vector3t<T>& v1, const Vector3t<T>& v2) noexcept
	{
		return v1.x * v2.x + v1.y * v2.y + v1.z * v2.z;
	}

	template<typename T>
	Vector3t<T> cross(const Vector3t<T>& v1, const Vector3t<T>& v2) noexcept
	{
		return Vector3t<T>(
			v1.y * v2.z - v1.z * v2.y,
			v1.z * v2.x - v1.x * v2.z,
			v1.x * v2.y - v1.y * v2.x);
	}

	template<typename T>
	inline T length2(const Vector3t<T>& v) noexcept
	{
		return dot(v, v);
	}

	template<typename T>
	inline T length(const Vector3t<T>& v) noexcept
	{
		return std::sqrt(length2(v));
	}

	template<typename T>
	inline T distance(const Vector3t<T>& v1, const Vector3t<T>& v2) noexcept
	{
		return length(v1 - v2);
	}

	template<typename T>
	inline T sqrDistance(const Vector3t<T>& v1, const Vector3t<T>& v2) noexcept
	{
		return length2(v1 - v2);
	}

	template<typename T>
	inline bool equal(const Vector3t<T>& v1, const Vector3t<T>& v2) noexcept
	{
		return
			math::equal<T>(v1.x, v2.x) &&
			math::equal<T>(v1.y, v2.y) &&
			math::equal<T>(v1.z, v2.z);
	}

	template<typename T>
	inline bool isfinite(const Vector3t<T>& v) noexcept
	{
		return std::isfinite(v.x) && std::isfinite(v.y) && std::isfinite(v.z);
	}

	template<typename T>
	inline Vector3t<T> negate(const Vector3t<T>& v) noexcept
	{
		return Vector3t<T>(-v.x, -v.y, -v.z);
	}

	template<typename T>
	inline Vector3t<T> clamp(const Vector3t<T>& t, T min, T max) noexcept
	{
		return Vector3t<T>(
			std::max(min, std::min(max, t.x)),
			std::max(min, std::min(max, t.y)),
			std::max(min, std::min(max, t.z))
			);
	}

	template<typename T>
	inline Vector3t<T> saturate(const Vector3t<T>& v) noexcept
	{
		return clamp(v, T(0.0f), T(1.0f));
	}

	template<typename T>
	inline Vector3t<T> normalize(const Vector3t<T>& v) noexcept
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
	inline Vector3t<T> snorm2unorm(const Vector3t<T>& v) noexcept
	{
		return Vector3t<T>(
			_mathutil<T>::snorm2unorm(v.x),
			_mathutil<T>::snorm2unorm(v.y),
			_mathutil<T>::snorm2unorm(v.z)
			);
	}

	template<typename T>
	inline Vector3t<T> unorm2snorm(const Vector3t<T>& v) noexcept
	{
		return Vector3t<T>(
			_mathutil<T>::unorm2snorm(v.x),
			_mathutil<T>::unorm2snorm(v.y),
			_mathutil<T>::unorm2snorm(v.z)
			);
	}

	template <typename T>
	inline Vector3t<T> min(const Vector3t<T>& a, const Vector3t<T>& b) noexcept
	{
		return Vector3t<T>(std::min(a.x, b.x), std::min(a.y, b.y), std::min(a.z, b.z));
	}

	template <typename T>
	inline Vector3t<T> max(const Vector3t<T>& a, const Vector3t<T>& b) noexcept
	{
		return Vector3t<T>(std::max(a.x, b.x), std::max(a.y, b.y), std::max(a.z, b.z));
	}

	template<typename T>
	inline Vector3t<T> abs(const Vector3t<T>& v) noexcept
	{
		return Vector3t<T>(std::abs(v.x), std::abs(v.y), std::abs(v.z));
	}

	template<typename T>
	inline Vector3t<T> cos(const Vector3t<T>& v) noexcept
	{
		return Vector3t<T>(std::cos(v.x), std::cos(v.y), std::cos(v.z));
	}

	template<typename T>
	inline Vector3t<T> sin(const Vector3t<T>& v) noexcept
	{
		return Vector3t<T>(std::sin(v.x), std::sin(v.y), std::sin(v.z));
	}

	template<typename T>
	inline Vector3t<T> tan(const Vector3t<T>& v) noexcept
	{
		return Vector3t<T>(std::tan(v.x), std::tan(v.y), std::tan(v.z));
	}

	template<typename T>
	inline Vector3t<T> acos(const Vector3t<T>& v) noexcept
	{
		return Vector3t<T>(std::acos(v.x), std::acos(v.y), std::acos(v.z));
	}

	template<typename T>
	inline Vector3t<T> asin(const Vector3t<T>& v) noexcept
	{
		return Vector3t<T>(std::asin(v.x), std::asin(v.y), std::asin(v.z));
	}

	template<typename T>
	inline Vector3t<T> atan(const Vector3t<T>& v) noexcept
	{
		return Vector3t<T>(std::atan(v.x), std::atan(v.y), std::atan(v.z));
	}

	template<typename T>
	inline Vector3t<T> exp(const Vector3t<T>& v) noexcept
	{
		return Vector3t<T>(std::exp(v.x), std::exp(v.y), std::exp(v.z));
	}

	template<typename T>
	inline Vector3t<T> exp2(const Vector3t<T>& v) noexcept
	{
		return Vector3t<T>(std::exp2(v.x), std::exp2(v.y), std::exp2(v.z));
	}

	template<typename T>
	inline Vector3t<T> sqrt(const Vector3t<T>& v) noexcept
	{
		return Vector3t<T>(std::sqrt(v.x), std::sqrt(v.y), std::sqrt(v.z));
	}

	template<typename T>
	inline Vector3t<T> log(const Vector3t<T>& v) noexcept
	{
		return Vector3t<T>(std::log(v.x), std::log(v.y), std::log(v.z));
	}

	template<typename T>
	inline Vector3t<T> log2(const Vector3t<T>& v) noexcept
	{
		return Vector3t<T>(std::log2(v.x), std::log2(v.y), std::log2(v.z));
	}

	template<typename T>
	inline Vector3t<T> log10(const Vector3t<T>& v) noexcept
	{
		return Vector3t<T>(std::log10(v.x), std::log10(v.y), std::log10(v.z));
	}

	template<typename T>
	inline Vector3t<T> pow(const Vector3t<T>& v1, T v2) noexcept
	{
		return Vector3t<T>(std::pow(v1.x, v2), std::pow(v1.y, v2), std::pow(v1.z, v2));
	}

	template<typename T>
	inline Vector3t<T> pow(const Vector3t<T>& v1, const Vector3t<T>& v2) noexcept
	{
		return Vector3t<T>(std::pow(v1.x, v2.x), std::pow(v1.y, v2.y), std::pow(v1.z, v2.z));
	}

	template<typename T>
	inline Vector3t<T> random(const Vector3t<T>& min, const Vector3t<T>& max) noexcept
	{
		return Vector3t<T>(random(min.x, max.x), random(min.y, max.y), random(min.z, max.z));
	}

	template<typename T>
	inline Vector3t<T> ceil(const Vector3t<T>& v) noexcept
	{
		return Vector3t<T>(std::ceil(v.x), std::ceil(v.y), std::ceil(v.z));
	}

	template<typename T>
	inline Vector3t<T> floor(const Vector3t<T>& v) noexcept
	{
		return Vector3t<T>(std::floor(v.x), std::floor(v.y), std::floor(v.z));
	}

	template<typename T>
	inline Vector3t<T> randomVector3() noexcept
	{
		float z = 2.0f * std::rand() / RAND_MAX - 1.0f;
		float t = 2.0f * std::rand() / RAND_MAX * M_PI;
		float r = std::sqrt(1.0f - z * z);
		return Vector3t<T>(r * cos(t), r * sin(t), z);
	}

	template<typename T>
	inline Vector3t<T> linear2srgb(const Vector3t<T>& srgb) noexcept
	{
		const float ALPHA = 0.055f;
		return Vector3t<T>(
			srgb.x < 0.0031308f ? 12.92f * srgb.x : (1 + ALPHA) * math::pow(srgb.x, 1 / 2.4f) - ALPHA,
			srgb.y < 0.0031308f ? 12.92f * srgb.y : (1 + ALPHA) * math::pow(srgb.y, 1 / 2.4f) - ALPHA,
			srgb.z < 0.0031308f ? 12.92f * srgb.z : (1 + ALPHA) * math::pow(srgb.z, 1 / 2.4f) - ALPHA);
	};

	template<typename T>
	inline Vector3t<T> srgb2linear(const Vector3t<T>& rgb) noexcept
	{
		const float ALPHA = 0.055f;
		return Vector3t<T>(
			rgb.x < 0.04045f ? rgb.x / 12.92f : math::pow((rgb.x + ALPHA) / (1 + ALPHA), 2.4f),
			rgb.y < 0.04045f ? rgb.y / 12.92f : math::pow((rgb.y + ALPHA) / (1 + ALPHA), 2.4f),
			rgb.z < 0.04045f ? rgb.z / 12.92f : math::pow((rgb.z + ALPHA) / (1 + ALPHA), 2.4f));
	}
}

_NAME_END

#endif