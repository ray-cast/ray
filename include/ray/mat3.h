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
#ifndef _H_MATRIX3X3_H_
#define _H_MATRIX3X3_H_

#include <ray/vector3.h>

_NAME_BEGIN

template<typename T>
class Matrix3x3t
{
public:
	Matrix3x3t() noexcept
	{
	}

	Matrix3x3t(T _a1, T _a2, T _a3, T _b1, T _b2, T _b3, T _c1, T _c2, T _c3) noexcept
		: a1(_a1), a2(_a2), a3(_a3)
		, b1(_b1), b2(_b2), b3(_b3)
		, c1(_c1), c2(_c2), c3(_c3)
	{
	}

	Matrix3x3t(const Matrix3x3t<T>& m1, const Matrix3x3t<T>& m2) noexcept
	{
		this->multiplyMatrices(m1, m2);
	}

	Matrix3x3t(const Vector3t<T>& axis, T angle) noexcept
	{
		this->makeRotate(axis, angle);
	}

	explicit Matrix3x3t(const Quaterniont<T>& q) noexcept
	{
		this->makeRotate(q);
	}

	template <typename S>
	explicit Matrix3x3t(const Matrix4x4t<S>& m) noexcept
	{
		a1 = m.a1; a2 = m.a2; a3 = m.a3;
		b1 = m.b1; b2 = m.b2; b3 = m.b3;
		c1 = m.c1; c2 = m.c2; c3 = m.c3;
	}

	template <typename S>
	explicit Matrix3x3t(Matrix4x4t<S>&& m) noexcept
	{
		a1 = m.a1; a2 = m.a2; a3 = m.a3;
		b1 = m.b1; b2 = m.b2; b3 = m.b3;
		c1 = m.c1; c2 = m.c2; c3 = m.c3;
	}

	template <typename S>
	explicit operator Matrix3x3t<S>() const noexcept
	{
		return Matrix3x3t<S>(
			S(a1), S(a2), S(a3),
			S(b1), S(b2), S(b3),
			S(c1), S(c2), S(c3));
	}

	template <typename S>
	Matrix3x3t<T>& operator -= (const Matrix3x3t<S>& m) noexcept
	{
		a1 -= m.a1; b1 -= m.a2; c1 -= m.c1;
		a2 -= m.a2; b2 -= m.a2; c2 -= m.c2;
		a3 -= m.a3; b3 -= m.a2; c3 -= m.c3;
		return *this;
	}

	template <typename S>
	Matrix3x3t<T>& operator += (const Matrix3x3t<S>& m) noexcept
	{
		a1 += m.a1; b1 += m.a2; c1 += m.c1;
		a2 += m.a2; b2 += m.a2; c2 += m.c2;
		a3 += m.a3; b3 += m.a2; c3 += m.c3;
		return *this;
	}

	template <typename S>
	Matrix3x3t<T>& operator *= (const Matrix3x3t<S>& m) noexcept
	{
		*this = Matrix3x3t<T>(
			m.a1 * a1 + m.b1 * a2 + m.c1 * a3,
			m.a2 * a1 + m.b2 * a2 + m.c2 * a3,
			m.a3 * a1 + m.b3 * a2 + m.c3 * a3,
			m.a1 * b1 + m.b1 * b2 + m.c1 * b3,
			m.a2 * b1 + m.b2 * b2 + m.c2 * b3,
			m.a3 * b1 + m.b3 * b2 + m.c3 * b3,
			m.a1 * c1 + m.b1 * c2 + m.c1 * c3,
			m.a2 * c1 + m.b2 * c2 + m.c2 * c3,
			m.a3 * c1 + m.b3 * c2 + m.c3 * c3);
		return *this;
	}

	T& operator() (std::size_t m, std::size_t n) noexcept
	{
		return *(&a1)[m * 3 + n];
	}

	const T& operator() (std::size_t m, std::size_t n) const noexcept
	{
		return *(&a1)[m * 3 + n];
	}

	T& operator[] (std::size_t n) noexcept
	{
		return *(&a1)[n];
	}

	const T& operator[] (std::size_t n) const noexcept
	{
		return *(&a1)[n];
	}

	explicit operator T*() noexcept
	{
		return this->ptr();
	}

	explicit operator const T*() const noexcept
	{
		return this->ptr();
	}

	T* ptr() noexcept { return (T*)&a1; }
	T* data() noexcept { return (T*)&a1; }

	const T* ptr() const noexcept { return (const T*)&a1; }
	const T* data() const noexcept { return (const T*)&a1; }

	Matrix3x3t<T>& set(T mt00, T mt01, T mt02, T mt10, T mt11, T mt12, T mt20, T mt21, T mt22) noexcept
	{
		a1 = T(mt00);
		a2 = T(mt01);
		a3 = T(mt02);
		b1 = T(mt10);
		b2 = T(mt11);
		b3 = T(mt12);
		c1 = T(mt20);
		c2 = T(mt21);
		c3 = T(mt22);
		return *this;
	}

	Matrix3x3t<T>& set(const Matrix4x4t<T>& m) noexcept
	{
		a1 = m.a1; a2 = m.a2; a3 = m.a3;
		b1 = m.b1; b2 = m.b2; b3 = m.b3;
		c1 = m.c1; c2 = m.c2; c3 = m.c3;
		return *this;
	}

	Matrix3x3t<T>& loadIdentity() noexcept
	{
		set(1, 0, 0,
			0, 1, 0,
			0, 0, 1);
		return *this;
	}

	Matrix3x3t<T>& multiplyMatrices(const Matrix3x3t<T>& m1, const Matrix3x3t<T>& m2) noexcept
	{
		a1 = m1.a1 * m2.a1 + m1.b1 * m2.a2 + m1.c1 * m2.a3;
		a2 = m1.a2 * m2.a1 + m1.b2 * m2.a2 + m1.c2 * m2.a3;
		a3 = m1.a3 * m2.a1 + m1.b3 * m2.a2 + m1.c3 * m2.a3;
		b1 = m1.a1 * m2.b1 + m1.b1 * m2.b2 + m1.c1 * m2.b3;
		b2 = m1.a2 * m2.b1 + m1.b2 * m2.b2 + m1.c2 * m2.b3;
		b3 = m1.a3 * m2.b1 + m1.b3 * m2.b2 + m1.c3 * m2.b3;
		c1 = m1.a1 * m2.c1 + m1.b1 * m2.c2 + m1.c1 * m2.c3;
		c2 = m1.a2 * m2.c1 + m1.b2 * m2.c2 + m1.c2 * m2.c3;
		c3 = m1.a3 * m2.c1 + m1.b3 * m2.c2 + m1.c3 * m2.c3;
		return *this;
	}

	template<typename S>
	Matrix3x3t<T>& multiplyScalar(S scale) noexcept
	{
		a1 *= T(scale);
		a2 *= T(scale);
		a3 *= T(scale);
		b1 *= T(scale);
		b2 *= T(scale);
		b3 *= T(scale);
		c1 *= T(scale);
		c2 *= T(scale);
		c3 *= T(scale);
		return *this;
	}

	Matrix3x3t<T>& makeRotationX(T theta) noexcept
	{
		T ang = DEG_TO_RAD(theta);
		T c, s;

		sinCos(&s, &c, ang);

		return set(
				1, 0, 0,
				0, c, -s,
				0, s, c);
	}

	Matrix3x3t<T>& makeRotationY(T theta) noexcept
	{
		T ang = DEG_TO_RAD(theta);
		T c, s;

		sinCos(&s, &c, ang);

		return set(
				c, 0, s,
				0, 1, 0,
				-s, 0, c);
	}

	Matrix3x3t<T>& makeRotationZ(T theta) noexcept
	{
		T c, s, ang = DEG_TO_RAD(theta);

		sinCos(&s, &c, ang);

		return set(
				c, -s, 0,
				s, c, 0,
				0, 0, 1);
	}

	Matrix3x3t<T>& makeRotate(T eulerX, T eulerY, T eulerZ) noexcept
	{
		T sh, ch, sp, cp, sb, cb;

		math::sinCos(&sp, &cp, DEG_TO_RAD(eulerX));
		math::sinCos(&sh, &ch, DEG_TO_RAD(eulerY));
		math::sinCos(&sb, &cb, DEG_TO_RAD(eulerZ));

		a1 = ch * cb;
		b1 = ch * sb;
		c1 = -sh;

		a2 = sh * sc - cs;
		b2 = sh * ss + cc;
		c2 = ch * sp;

		a3 = sh * cc + ss;
		b3 = sh * cs - sc;
		c3 = ch * cp;

		return *this;
	}

	Matrix3x3t<T>& makeRotate(const Vector3t<T>& forward, const Vector3t<T>& up, const Vector3t<T>& right) noexcept
	{
		a1 = right.x;
		a2 = right.y;
		a3 = right.z;

		b1 = up.x;
		b2 = up.y;
		b3 = up.z;

		c1 = forward.x;
		c2 = forward.y;
		c3 = forward.z;
		return *this;
	}

	Matrix3x3t<T>& makeRotate(const Vector3t<T>& axis, T angle) noexcept
	{
		T c, s;

		sinCos(&s, &c, DEG_TO_RAD(angle));

		Vector3t<T> v = axis;
		v.normalize();

		T x = v.x;
		T y = v.y;
		T z = v.z;

		T t = 1 - c;

		T tx = t * x;
		T ty = t * y;
		T tz = t * z;

		a1 = (tx * x + c);
		b1 = (tx * y + s * z);
		c1 = (tx * z - s * y);

		a2 = (tx * y - s * z);
		b2 = (ty * y + c);
		c2 = (ty * z + s * x);

		a3 = (tx * z + s * y);
		b3 = (ty * z - s * x);
		c3 = (tz * z + c);

		return *this;
	}

	Matrix3x3t<T>& makeRotate(const Quaterniont<T>& q) noexcept
	{
		T xs = q.x * T(2.0), ys = q.y * T(2.0), zs = q.z * T(2.0);
		T wx = q.w * xs, wy = q.w * ys, wz = q.w * zs;
		T xx = q.x * xs, xy = q.x * ys, xz = q.x * zs;
		T yy = q.y * ys, yz = q.y * zs, zz = q.z * zs;

		a1 = T(1.0) - (yy + zz);
		b1 = xy - wz;
		c1 = xz + wy;

		a2 = xy + wz;
		b2 = T(1.0) - (xx + zz);
		c2 = yz - wx;

		a3 = xz - wy;
		b3 = yz + wx;
		c3 = T(1.0) - (xx + yy);

		return *this;
	}

	Matrix3x3t<T>& rotate(const Vector3t<T>& axis, T angle) noexcept
	{
		Matrix3x3t<T> m1(axis, angle);
		Matrix3x3t<T> m2(*this);
		return multiplyMatrices(m1, m2);
	}

	Matrix3x3t<T>& rotate(const Quaterniont<T>& q) noexcept
	{
		return rotate(q.x, q.y, q.z, q.w);
	}

	Quaterniont<T> getRotate() const noexcept
	{
		return Quaterniont<T>(this->getForward(), this->getUpVector(), this->getRight());
	}

	Matrix3x3t<T>& makeScale(const Vector3t<T>& sz) noexcept
	{
		set(sz.x, 0, 0,
			0, sz.y, 0,
			0, 0, sz.z);
		return *this;
	}

	Matrix3x3t<T>& makeScale(T x, T y, T z) noexcept
	{
		set(x, 0, 0,
			0, y, 0,
			0, 0, z);
		return *this;
	}

	Matrix3x3t<T>& scale(const Vector3t<T>& sz) noexcept
	{
		return scale(sz.x, sz.y, sz.z);
	}

	Matrix3x3t<T>& scale(T x, T y, T z) noexcept
	{
		a1 *= x;
		a2 *= x;
		a3 *= x;
		b1 *= y;
		b2 *= y;
		b3 *= y;
		c1 *= z;
		c2 *= z;
		return *this;
	}

	const Vector3t<T>& getRight() const noexcept
	{
		return *(Vector3t<T>*)&a1;
	}

	const Vector3t<T>& getUpVector() const noexcept
	{
		return *(Vector3t<T>*)&b1;
	}

	const Vector3t<T>& getForward() const noexcept
	{
		return *(Vector3t<T>*)&c1;
	}

public:
	T a1, a2, a3;
	T b1, b2, b3;
	T c1, c2, c3;
};

template <typename T>
inline bool operator==(const Matrix3x3t<T>& m1, const Matrix3x3t<T>& m2)
{
	return
		m1.a1 == m2.a1 && m1.a2 == m2.a2 && m1.a3 == m2.a3 &&
		m1.b1 == m2.b1 && m1.b2 == m2.b2 && m1.b3 == m2.b3 &&
		m1.c1 == m2.c1 && m1.c2 == m2.c2 && m1.c3 == m2.c3;
}

template <typename T>
inline bool operator!=(const Matrix3x3t<T>& m1, const Matrix3x3t<T>& m2)
{
	return
		m1.a1 != m2.a1 || m1.a2 != m2.a2 || m1.a3 != m2.a3 ||
		m1.b1 != m2.b1 || m1.b2 != m2.b2 || m1.b3 != m2.b3 ||
		m1.c1 != m2.c1 || m1.c2 != m2.c2 || m1.c3 != m2.c3;
}

template<typename T>
inline Vector3t<T> operator*(const Vector3t<T>& v, const Matrix3x3t<T>& m)
{
	return Vector3t<T>(
		m.a1 * v.x + m.a2 * v.y + m.a3 * v.z,
		m.b1 * v.x + m.b2 * v.y + m.b3 * v.z,
		m.c1 * v.x + m.c2 * v.y + m.c3 * v.z);
}

template<typename T>
inline Vector3t<T> operator*(const Matrix3x3t<T>& m, const Vector3t<T>& v)
{
	return Vector3t<T>(
		m.a1 * v.x + m.b1 * v.y + m.c1 * v.z,
		m.a2 * v.x + m.b2 * v.y + m.c2 * v.z,
		m.a3 * v.x + m.b3 * v.y + m.c3 * v.z);
}

template<typename T>
inline Matrix3x3t<T> operator*(T& scale, const Matrix3x3t<T>& m1)
{
	Matrix3x3t<T> m;
	m.a1 = m1.a1 * scale; m.b1 = m1.b1 * scale; m.c1 = m1.c1 * scale;
	m.a2 = m1.a2 * scale; m.b2 = m1.b2 * scale; m.c2 = m1.c2 * scale;
	m.a3 = m1.a3 * scale; m.b3 = m1.b3 * scale; m.c3 = m1.c3 * scale;
	return m;
}

template<typename T>
inline Matrix3x3t<T> operator*(const Matrix3x3t<T>& m1, T& scale)
{
	Matrix3x3t<T> m;
	m.a1 = m1.a1 * scale; m.b1 = m1.b1 * scale; m.c1 = m1.c1 * scale;
	m.a2 = m1.a2 * scale; m.b2 = m1.b2 * scale; m.c2 = m1.c2 * scale;
	m.a3 = m1.a3 * scale; m.b3 = m1.b3 * scale; m.c3 = m1.c3 * scale;
	return m;
}

template<typename T>
inline Matrix3x3t<T> operator*(const Matrix3x3t<T>& m1, const Matrix3x3t<T>& m2)
{
	return Matrix3x3t<T>(m1, m2);
}

template<typename T>
inline Matrix3x3t<T>& operator*=(Matrix3x3t<T>& m1, const Matrix3x3t<T>& m2)
{
	m1 = m1 * m2;
	return m1;
}

template<typename T>
inline Vector3t<T>& operator*=(Vector3t<T>& v, const Matrix3x3t<T>& m)
{
	v = v * m;
	return v;
}

namespace math
{
	template<typename T>
	bool isIdentity(const Matrix3x3t<T>& m) noexcept
	{
		constexpr T epsilon = 10e-3f;
		return (
			m.a2 <= epsilon && m.a2 >= -epsilon &&
			m.a3 <= epsilon && m.a3 >= -epsilon &&
			m.b1 <= epsilon && m.b1 >= -epsilon &&
			m.b3 <= epsilon && m.b3 >= -epsilon &&
			m.c1 <= epsilon && m.c1 >= -epsilon &&
			m.c2 <= epsilon && m.c2 >= -epsilon &&
			m.a1 <= 1.f + epsilon && m.a1 >= 1.f - epsilon &&
			m.b2 <= 1.f + epsilon && m.b2 >= 1.f - epsilon &&
			m.c3 <= 1.f + epsilon && m.c3 >= 1.f - epsilon);
	}

	template<typename T>
	bool isOnlyRotate(const Matrix3x3t<T>& m) noexcept
	{
		constexpr T epsilon = 10e-3f;
		return (
			m.a1 <= 1.f + epsilon && m.a1 >= 1.f - epsilon &&
			m.a2 <= 1.f + epsilon && m.a2 >= 1.f - epsilon &&
			m.a3 <= 1.f + epsilon && m.a3 >= 1.f - epsilon &&
			m.b1 <= 1.f + epsilon && m.b1 >= 1.f - epsilon &&
			m.b2 <= 1.f + epsilon && m.b2 >= 1.f - epsilon &&
			m.b3 <= 1.f + epsilon && m.b3 >= 1.f - epsilon &&
			m.c1 <= 1.f + epsilon && m.c1 >= 1.f - epsilon &&
			m.c2 <= 1.f + epsilon && m.c2 >= 1.f - epsilon &&
			m.c3 <= 1.f + epsilon && m.c3 >= 1.f - epsilon);
	}

	template<typename T>
	T determinant(const Matrix3x3t<T>& m) noexcept
	{
		return m.a1 * m.b2 * m.c3 - m.a1 * m.b3 * m.c2 + m.a2 * m.b3 * m.c1 - m.a2 * m.b1 * m.c3 + m.a3 * m.b1 * m.c2 - m.a3 * m.b2 * m.c1;
	}

	template<typename T>
	Matrix3x3t<T> orthonormalize(const Matrix3x3t<T>& _m)
	{
		Matrix3x3t<T> m;
		Vector3t<T> x(_m.a1, _m.b1, _m.c1);
		Vector3t<T> y(_m.a2, _m.b2, _m.c2);
		Vector3t<T> z;
		x = math::normalize(x);
		z = math::cross(x, y);
		z = math::normalize(z);
		y = math::cross(z, x);
		y = math::normalize(y);
		m.a1 = x.x; m.a2 = x.y; m.a3 = x.z;
		m.b1 = y.x; m.b2 = y.y; m.b3 = y.z;
		m.c1 = z.x; m.c3 = z.y; m.c3 = z.z;
		return m;
	}

	template<typename T>
	Matrix3x3t<T> transpose(const Matrix3x3t<T>& _m)
	{
		Matrix3x3t<T> m = _m;
		std::swap((T&)m.a2, (T&)m.b1);
		std::swap((T&)m.a3, (T&)m.c1);
		std::swap((T&)m.b3, (T&)m.c2);
		return m;
	}

	template<typename T>
	Matrix3x3t<T>& inverse(const Matrix3x3t<T>& _m)
	{
		const T det = determinant(_m);
		if (det == T(0.0))
		{
			const T nan = std::numeric_limits<T>::quiet_NaN();
			*this = Matrix3x3t<T>(
				nan, nan, nan,
				nan, nan, nan,
				nan, nan, nan);

			return *this;
		}

		T invdet = T(1.0) / det;

		Matrix4x4t<T> m;
		m.a1 =  invdet * (_m.b2 * _m.c3 - _m.b3 * _m.c2);
		m.a2 = -invdet * (_m.a2 * _m.c3 - _m.a3 * _m.c2);
		m.a3 =  invdet * (_m.a2 * _m.b3 - _m.a3 * _m.b2);
		m.b1 = -invdet * (_m.b1 * _m.c3 - _m.b3 * _m.c1);
		m.b2 =  invdet * (_m.a1 * _m.c3 - _m.a3 * _m.c1);
		m.b3 = -invdet * (_m.a1 * _m.b3 - _m.a3 * _m.b1);
		m.c1 =  invdet * (_m.b1 * _m.c2 - _m.b2 * _m.c1);
		m.c2 = -invdet * (_m.a1 * _m.c2 - _m.a2 * _m.c1);
		m.c3 =  invdet * (_m.a1 * _m.b2 - _m.a2 * _m.b1);

		return m;
	}
}

_NAME_END

#endif
