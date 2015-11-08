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
#ifndef _H_VECTOR4_H_
#define _H_VECTOR4_H_

#include <ray/vector3.h>

_NAME_BEGIN

template<typename T>
class Vector4t
{
public:
	static const Vector4t<T> Zero;
	static const Vector4t<T> One;
	static const Vector4t<T> UnitX;
	static const Vector4t<T> UnitY;
	static const Vector4t<T> UnitZ;

	T x, y, z, w;

	Vector4t() noexcept
	{
	}

	Vector4t(const Vector3t<T>& copy, T w = 1.0)  noexcept
		: x(copy.x)
		, y(copy.y)
		, z(copy.z)
		, w(w)
	{
	}

	Vector4t(const Vector4t<T>& copy) noexcept
		: x(copy.x)
		, y(copy.y)
		, z(copy.z)
		, w(copy.w)
	{
	}

	Vector4t(T xx, T yy, T zz, T ww = 1.0)  noexcept
		: x(xx)
		, y(yy)
		, z(zz)
		, w(ww)
	{
	}

	Vector4t(const Vector2t<T>& pt, const Vector2t<T>& sz) noexcept
		: x(pt.x)
		, y(pt.y)
		, z(sz.x)
		, w(sz.y)
	{
	}

	Vector4t(const Vector2t<T>& pt, float z, float w = 1.0) noexcept
		: x(pt.x)
		, y(pt.y)
		, z(z)
		, w(w)
	{
	}

	Vector4t<T>& operator*=(T scale) noexcept { x *= scale; y *= scale; z *= scale; w *= scale; return *this; }
	Vector4t<T>& operator/=(T scale) noexcept { x /= scale; y /= scale; z /= scale; w /= scale; return *this; }

	Vector4t<T>& operator+=(const Vector3t<T>& rt) { x += rt.x; y += rt.y; z += rt.z; return *this; }
	Vector4t<T>& operator-=(const Vector3t<T>& rt) { x -= rt.x; y -= rt.y; z -= rt.z; return *this; }
	Vector4t<T>& operator/=(const Vector3t<T>& rt) { x *= rt.x; y *= rt.y; z *= rt.z; return *this; }
	Vector4t<T>& operator*=(const Vector3t<T>& rt) { x /= rt.x; y /= rt.y; z /= rt.z; return *this; }

	Vector4t<T>& operator+=(const Vector4t<T>& rt) { x += rt.x; y += rt.y; z += rt.z; w += rt.w; return *this; }
	Vector4t<T>& operator-=(const Vector4t<T>& rt) { x -= rt.x; y -= rt.y; z -= rt.z; w -= rt.w; return *this; }
	Vector4t<T>& operator/=(const Vector4t<T>& rt) { x *= rt.x; y *= rt.y; z *= rt.z; w *= rt.w; return *this; }
	Vector4t<T>& operator*=(const Vector4t<T>& rt) { x /= rt.x; y /= rt.y; z /= rt.z; w /= rt.w; return *this; }

	Vector2t<T> xx()  const { return Vector2t<T>(x, x); }
	Vector2t<T> xy()  const { return Vector2t<T>(x, y); }
	Vector2t<T> xz()  const { return Vector2t<T>(x, z); }
	Vector2t<T> xw()  const { return Vector2t<T>(x, w); }
	Vector3t<T> xxx() const { return Vector3t<T>(x, x, x); }
	Vector3t<T> xxy() const { return Vector3t<T>(x, x, y); }
	Vector3t<T> xxz() const { return Vector3t<T>(x, x, z); }
	Vector3t<T> xxw() const { return Vector3t<T>(x, x, w); }
	Vector3t<T> xyx() const { return Vector3t<T>(x, y, x); }
	Vector3t<T> xyy() const { return Vector3t<T>(x, y, y); }
	Vector3t<T> xyz() const { return Vector3t<T>(x, y, z); }
	Vector3t<T> xyw() const { return Vector3t<T>(x, y, w); }
	Vector3t<T> xzx() const { return Vector3t<T>(x, z, x); }
	Vector3t<T> xzy() const { return Vector3t<T>(x, z, y); }
	Vector3t<T> xzz() const { return Vector3t<T>(x, z, z); }
	Vector3t<T> xzw() const { return Vector3t<T>(x, z, w); }
	Vector3t<T> xwx() const { return Vector3t<T>(x, w, x); }
	Vector3t<T> xwy() const { return Vector3t<T>(x, w, y); }
	Vector3t<T> xwz() const { return Vector3t<T>(x, w, z); }
	Vector3t<T> xww() const { return Vector3t<T>(x, w, w); }
	Vector2t<T> yx()  const { return Vector2t<T>(y, x); }
	Vector2t<T> yy()  const { return Vector2t<T>(y, y); }
	Vector2t<T> yz()  const { return Vector2t<T>(y, z); }
	Vector2t<T> yw()  const { return Vector2t<T>(y, w); }
	Vector3t<T> yxx() const { return Vector3t<T>(y, x, x); }
	Vector3t<T> yxy() const { return Vector3t<T>(y, x, y); }
	Vector3t<T> yxz() const { return Vector3t<T>(y, x, z); }
	Vector3t<T> yxw() const { return Vector3t<T>(y, x, w); }
	Vector3t<T> yyx() const { return Vector3t<T>(y, y, x); }
	Vector3t<T> yyy() const { return Vector3t<T>(y, y, y); }
	Vector3t<T> yyz() const { return Vector3t<T>(y, y, z); }
	Vector3t<T> yyw() const { return Vector3t<T>(y, y, w); }
	Vector3t<T> yzx() const { return Vector3t<T>(y, z, x); }
	Vector3t<T> yzy() const { return Vector3t<T>(y, z, y); }
	Vector3t<T> yzz() const { return Vector3t<T>(y, z, z); }
	Vector3t<T> yzw() const { return Vector3t<T>(y, z, w); }
	Vector3t<T> ywx() const { return Vector3t<T>(y, w, x); }
	Vector3t<T> ywy() const { return Vector3t<T>(y, w, y); }
	Vector3t<T> ywz() const { return Vector3t<T>(y, w, z); }
	Vector3t<T> yww() const { return Vector3t<T>(y, w, w); }
	Vector2t<T> zx()  const { return Vector2t<T>(z, x); }
	Vector2t<T> zy()  const { return Vector2t<T>(z, y); }
	Vector2t<T> zz()  const { return Vector2t<T>(z, z); }
	Vector2t<T> zw()  const { return Vector2t<T>(z, w); }
	Vector3t<T> zxx() const { return Vector3t<T>(z, x, x); }
	Vector3t<T> zxy() const { return Vector3t<T>(z, x, y); }
	Vector3t<T> zxz() const { return Vector3t<T>(z, x, z); }
	Vector3t<T> zxw() const { return Vector3t<T>(z, x, w); }
	Vector3t<T> zyx() const { return Vector3t<T>(z, y, x); }
	Vector3t<T> zyy() const { return Vector3t<T>(z, y, y); }
	Vector3t<T> zyz() const { return Vector3t<T>(z, y, z); }
	Vector3t<T> zyw() const { return Vector3t<T>(z, y, w); }
	Vector3t<T> zzx() const { return Vector3t<T>(z, z, x); }
	Vector3t<T> zzy() const { return Vector3t<T>(z, z, y); }
	Vector3t<T> zzz() const { return Vector3t<T>(z, z, z); }
	Vector3t<T> zzw() const { return Vector3t<T>(z, z, w); }
	Vector3t<T> zwx() const { return Vector3t<T>(z, w, x); }
	Vector3t<T> zwy() const { return Vector3t<T>(z, w, y); }
	Vector3t<T> zwz() const { return Vector3t<T>(z, w, z); }
	Vector3t<T> zww() const { return Vector3t<T>(z, w, w); }
	Vector2t<T> wx()  const { return Vector2t<T>(w, x); }
	Vector2t<T> wy()  const { return Vector2t<T>(w, y); }
	Vector2t<T> wz()  const { return Vector2t<T>(w, z); }
	Vector2t<T> ww()  const { return Vector2t<T>(w, w); }
	Vector3t<T> wxx() const { return Vector3t<T>(w, x, x); }
	Vector3t<T> wxy() const { return Vector3t<T>(w, x, y); }
	Vector3t<T> wxz() const { return Vector3t<T>(w, x, z); }
	Vector3t<T> wxw() const { return Vector3t<T>(w, x, w); }
	Vector3t<T> wyx() const { return Vector3t<T>(w, y, x); }
	Vector3t<T> wyy() const { return Vector3t<T>(w, y, y); }
	Vector3t<T> wyz() const { return Vector3t<T>(w, y, z); }
	Vector3t<T> wyw() const { return Vector3t<T>(w, y, w); }
	Vector3t<T> wzx() const { return Vector3t<T>(w, z, x); }
	Vector3t<T> wzy() const { return Vector3t<T>(w, z, y); }
	Vector3t<T> wzz() const { return Vector3t<T>(w, z, z); }
	Vector3t<T> wzw() const { return Vector3t<T>(w, z, w); }
	Vector3t<T> wwx() const { return Vector3t<T>(w, w, x); }
	Vector3t<T> wwy() const { return Vector3t<T>(w, w, y); }
	Vector3t<T> wwz() const { return Vector3t<T>(w, w, z); }
	Vector3t<T> www() const { return Vector3t<T>(w, w, w); }

	void set(T xx, T yy, T zz)
	{
		x = xx; y = yy; z = zz;
	}

	void set(T xx, T yy, T zz, T ww)
	{
		x = xx; y = yy; z = zz; w = ww;
	}

	void set(const Vector3t<T>& v)
	{
		x = v.x; y = v.y; z = v.z;
	}

	template<typename ostream>
	ostream& operator<<(ostream& os)
	{
		os << x << ", " << y << ", " << z << ", " << w;
		return os;
	}

	template<typename istream>
	istream& operator>>(istream& is)
	{
		is >> x;
		is.ignore(2);
		is >> y;
		is.ignore(2);
		is >> z;
		is.ignore(2);
		is >> w;
		return is;
	}

	T& getComponent(unsigned char index)
	{
		switch (index)
		{
		case COMPONENT_X: return x;
		case COMPONENT_Y: return y;
		case COMPONENT_Z: return z;
		case COMPONENT_W: return w;
		case 'x': return x;
		case 'y': return y;
		case 'z': return z;
		case 'w': return w;
		default:
			assert(false);
		}
	}

	T normalize()
	{
		T magSq = length2();

		if (magSq > static_cast<T>(0.0))
		{
			T invSqrt = 1 / sqrt(magSq);
			*this *= invSqrt;
		}

		return magSq;
	}

	T dot() const { return x * x + y * y + z * z + w * w; }
	T dot(const Vector4t<T>& v) const { return x * v.x + y * v.y + z * v.z + w * v.w; }

	T length() const { return sqrt(length2()); }
	T length2() const { return x * x + y * y + z * z + w * w; }

	void conjugate() { *this = Vector4t<T>(-x, -y, -z); }
	void inverse() { *this = Vector4t<T>(-x, -y, -z) / length2(); }

	void cross(const Vector4t& q)
	{
		x = w * q.x + x * q.w + z * q.y - y * q.z;
		y = w * q.y + y * q.w + x * q.z - z * q.x;
		z = w * q.z + z * q.w + y * q.x - x * q.y;
		w = w * q.w - x * q.x - y * q.y - z * q.z;
	}

	void cross(const Vector4t& q1, const Vector4t& q2)
	{
		x = q1.w * q2.x + q1.x * q2.w + q1.z * q2.y - q1.y * q2.z;
		y = q1.w * q2.y + q1.y * q2.w + q1.x * q2.z - q1.z * q2.x;
		z = q1.w * q2.z + q1.z * q2.w + q1.y * q2.x - q1.x * q2.y;
		w = q1.w * q2.w - q1.x * q2.x - q1.y * q2.y - q1.z * q2.z;
	}

	T* ptr() { return (T*)this; }
	const T* ptr() const { return (const T*)this; }
	T* data() { return (T*)this; }
	const T* data() const { return (const T*)this; }
};

template<typename T> const Vector4t<T> Vector4t<T>::Zero = Vector4t<T>((T)0.0, (T)0.0, (T)0.0, (T)0.0);
template<typename T> const Vector4t<T> Vector4t<T>::One = Vector4t<T>((T)1.0, (T)1.0, (T)1.0, (T)1.0);
template<typename T> const Vector4t<T> Vector4t<T>::UnitX = Vector4t<T>((T)1.0, (T)0.0, (T)0.0, (T)1.0);
template<typename T> const Vector4t<T> Vector4t<T>::UnitY = Vector4t<T>((T)0.0, (T)1.0, (T)0.0, (T)1.0);
template<typename T> const Vector4t<T> Vector4t<T>::UnitZ = Vector4t<T>((T)0.0, (T)0.0, (T)1.0, (T)1.0);

template<typename T>
inline bool operator==(const Vector4t<T>& v1, const Vector4t<T>& v2) noexcept
{
	return v1.x == v2.x && v1.y == v2.y && v1.z == v2.z && v1.w == v2.w;
}

template<typename T>
inline bool operator!=(const Vector4t<T>& v1, const Vector4t<T>& v2) noexcept
{
	return v1.x != v2.x || v1.y != v2.y || v1.z != v2.z || v1.w != v2.w;
}

template<typename T>
inline Vector4t<T> operator-(const Vector4t<T>& v) noexcept
{
	return Vector4t<T>(v.conjugate());
}

template<typename T>
inline Vector4t<T> operator+(const Vector4t<T>& v1, const Vector4t<T>& v2) noexcept
{
	return Vector4t<T>(v1.x + v2.x, v1.y + v2.y, v1.z + v2.z, v1.w + v2.w);
}

template<typename T>
inline Vector4t<T> operator-(const Vector4t<T>& v1, const Vector4t<T>& v2) noexcept
{
	return Vector4t<T>(v1.x - v2.x, v1.y - v2.y, v1.z - v2.z, v1.w - v2.w);
}

template<typename T>
inline Vector4t<T> operator*(const Vector4t<T>& v1, const Vector4t<T>& v2) noexcept
{
	return Vector4t<T>(v1.x * v2.x, v1.y * v2.y, v1.z * v2.z, v1.w * v2.w);
}

template<typename T>
inline Vector4t<T> operator/(const Vector4t<T>& v1, const Vector4t<T>& v2) noexcept
{
	return Vector4t<T>(v1.x / v2.x, v1.y / v2.y, v1.z / v2.z);
}

template<typename T>
inline Vector4t<T> operator+(const Vector4t<T>& v1, const Vector3t<T>& v2) noexcept
{
	return Vector4t<T>(v1.x + v2.x, v1.y + v2.y, v1.z + v2.z, v1.w);
}

template<typename T>
inline Vector4t<T> operator-(const Vector4t<T>& v1, const Vector3t<T>& v2) noexcept
{
	return Vector4t<T>(v1.x - v2.x, v1.y - v2.y, v1.z - v2.z, v1.w);
}

template<typename T>
inline Vector4t<T> operator*(const Vector4t<T>& v1, const Vector3t<T>& v2) noexcept
{
	return Vector4t<T>(v1.x * v2.x, v1.y * v2.y, v1.z * v2.z, v1.w);
}

template<typename T>
inline Vector4t<T> operator/(const Vector4t<T>& v1, const Vector3t<T>& v2) noexcept
{
	return Vector4t<T>(v1.x / v2.x, v1.y / v2.y, v1.z / v2.z, v1.w);
}

template<typename T>
inline T operator*(const Vector4t<T>& v1, const Vector3t<T>& v2)
{
	return v1.x * v2.x + v1.y * v2.y + v1.z * v2.z + v1.w;
}

template<typename T>
inline Vector4t<T> operator*(const Vector4t<T>& v1, T scale) noexcept
{
	return Vector4t<T>(v1.x * scale, v1.y * scale, v1.z * scale, v1.w * scale);
}

template<typename T>
inline Vector4t<T> operator/(const Vector4t<T>& v1, T scale) noexcept
{
	return Vector4t<T>(v1.x / scale, v1.y / scale, v1.z / scale, v1.w / scale);
}

template <typename T>
inline Vector4t<T> operator~(const Vector4t<T>& v)
{
	Vector4t<T> tmp = v;
	tmp.normalize();
	return tmp;
}

_NAME_END

#endif
