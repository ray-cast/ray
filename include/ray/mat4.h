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
#ifndef _H_MATRIX4X4_H_
#define _H_MATRIX4X4_H_

#include <ray/vector4.h>
#include <ray/mat3.h>
#include <ray/quat.h>

_NAME_BEGIN

template<typename T>
class Matrix4x4t
{
public:
	static const Matrix4x4t<T> Zero;
	static const Matrix4x4t<T> One;

	T a1, a2, a3, a4;
	T b1, b2, b3, b4;
	T c1, c2, c3, c4;
	T d1, d2, d3, d4;

	Matrix4x4t() noexcept
	{
	}

	Matrix4x4t(T _a1, T _a2, T _a3, T _a4,
		T _b1, T _b2, T _b3, T _b4,
		T _c1, T _c2, T _c3, T _c4,
		T _d1, T _d2, T _d3, T _d4) noexcept
		: a1(_a1), a2(_a2), a3(_a3), a4(_a4)
		, b1(_b1), b2(_b2), b3(_b3), b4(_b4)
		, c1(_c1), c2(_c2), c3(_c3), c4(_c4)
		, d1(_d1), d2(_d2), d3(_d3), d4(_d4)
	{
	}

	template<typename S>
	explicit Matrix4x4t(const Matrix4x4t<S>& m) noexcept
	{
		a1 = static_cast<T>(m.a1); a2 = static_cast<T>(m.a2); a3 = static_cast<T>(m.a3); a4 = static_cast<T>(m.a4);
		b1 = static_cast<T>(m.b1); b2 = static_cast<T>(m.b2); b3 = static_cast<T>(m.b3); b4 = static_cast<T>(m.b4);
		c1 = static_cast<T>(m.c1); c2 = static_cast<T>(m.c2); c3 = static_cast<T>(m.c3); c4 = static_cast<T>(m.c4);
		d1 = static_cast<T>(m.d1); d2 = static_cast<T>(m.d2); d3 = static_cast<T>(m.d3); d4 = static_cast<T>(m.d4);
	}

	Matrix4x4t(const Matrix4x4t<T>& m1, const Matrix4x4t<T>& m2) noexcept
	{
		this->multiplyMatrices(m1, m2);
	}

	explicit Matrix4x4t(const Matrix3x3t<T>& m) noexcept
	{
		a1 = m.a1; a2 = m.a2; a3 = m.a3; a4 = static_cast<T>(0.0);
		b1 = m.b1; b2 = m.b2; b3 = m.b3; b4 = static_cast<T>(0.0);
		c1 = m.c1; c2 = m.c2; c3 = m.c3; c4 = static_cast<T>(0.0);
		d1 = static_cast<T>(0.0); d2 = static_cast<T>(0.0); d3 = static_cast<T>(0.0); d4 = static_cast<T>(1.0);
	}

	template <typename S>
	operator Matrix4x4t<S>() const noexcept
	{
		return Matrix4x4t<S>(
			static_cast<S>(a1), static_cast<S>(a2), static_cast<S>(a3), static_cast<S>(a4),
			static_cast<S>(b1), static_cast<S>(b2), static_cast<S>(b3), static_cast<S>(b4),
			static_cast<S>(c1), static_cast<S>(c2), static_cast<S>(c3), static_cast<S>(c4),
			static_cast<S>(d1), static_cast<S>(d2), static_cast<S>(d3), static_cast<S>(d4));
	}

	T& operator[](std::size_t n) noexcept
	{
		return *((&a1) + n);
	}

	const T& operator[](std::size_t n) const noexcept
	{
		return *((&a1) + n);
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
	const T* ptr() const noexcept  { return (const T*)&a1; }
	T* data() noexcept { return (T*)&a1; }
	const T* data() const noexcept  { return (const T*)&a1; }

	Matrix4x4t<T>& set(T mt00, T mt01, T mt02, T mt03, T mt10, T mt11, T mt12, T mt13, T mt20, T mt21, T mt22, T mt23, T mt30, T mt31, T mt32, T mt33) noexcept
	{
		a1 = static_cast<T>(mt00);
		a2 = static_cast<T>(mt01);
		a3 = static_cast<T>(mt02);
		a4 = static_cast<T>(mt03);
		b1 = static_cast<T>(mt10);
		b2 = static_cast<T>(mt11);
		b3 = static_cast<T>(mt12);
		b4 = static_cast<T>(mt13);
		c1 = static_cast<T>(mt20);
		c2 = static_cast<T>(mt21);
		c3 = static_cast<T>(mt22);
		c4 = static_cast<T>(mt23);
		d1 = static_cast<T>(mt30);
		d2 = static_cast<T>(mt31);
		d3 = static_cast<T>(mt32);
		d4 = static_cast<T>(mt33);
		return *this;
	}

	Matrix4x4t<T>& set(const Matrix3x3t<T>& m) noexcept
	{
		a1 = m.a1; a2 = m.a2; a3 = m.a3;
		b1 = m.b1; b2 = m.b2; b3 = m.b3;
		c1 = m.c1; c2 = m.c2; c3 = m.c3;
		return *this;
	}

	Matrix4x4t<T>& set(int line, const Vector3t<T>& v) noexcept
	{
		((Vector3t<T>&)(*(*this)[line])) = v;
		return *this;
	}

	Matrix4x4t<T>& set(std::size_t line, const Vector3t<T>& v) noexcept
	{
		((Vector3t<T>&)(*(*this)[line])) = v;
		return *this;
	}

	Matrix4x4t<T>& set(const Vector3t<T>& v1, const Vector3t<T>& v2, const Vector3t<T>& v3) noexcept
	{
		a1 = v1.x; a2 = v1.y; a3 = v1.z;
		b1 = v2.x; b2 = v2.y; b3 = v2.z;
		c1 = v3.x; c2 = v3.y; c3 = v3.z;
		return *this;
	}

	Matrix4x4t<T>& set(const Vector4t<T>& v1, const Vector4t<T>& v2, const Vector4t<T>& v3, const Vector4t<T>& v4) noexcept
	{
		a1 = v1.x; a2 = v1.y; a3 = v1.z; a4 = v1.w;
		b1 = v2.x; b2 = v2.y; b3 = v2.z; b4 = v2.w;
		c1 = v3.x; c2 = v3.y; c3 = v3.z; c4 = v3.w;
		d1 = v4.x; d2 = v4.y; d3 = v4.z; d4 = v4.w;
		return *this;
	}

	Matrix4x4t<T>& loadIdentity() noexcept
	{
		set(1, 0, 0, 0,
			0, 1, 0, 0,
			0, 0, 1, 0,
			0, 0, 0, 1);
		return *this;
	}

	T determinant() const noexcept
	{
		return
			  a1*b2*c3*d4 - a1*b2*c4*d3 + a1*b3*c4*d2 - a1*b3*c2*d4
			+ a1*b4*c2*d3 - a1*b4*c3*d2 - a2*b3*c4*d1 + a2*b3*c1*d4
			- a2*b4*c1*d3 + a2*b4*c3*d1 - a2*b1*c3*d4 + a2*b1*c4*d3
			+ a3*b4*c1*d2 - a3*b4*c2*d1 + a3*b1*c2*d4 - a3*b1*c4*d2
			+ a3*b2*c4*d1 - a3*b2*c1*d4 - a4*b1*c2*d3 + a4*b1*c3*d2
			- a4*b2*c3*d1 + a4*b2*c1*d3 - a4*b3*c1*d2 + a4*b3*c2*d1;
	}

	bool isOnlyTranslate() const noexcept
	{
		const static T epsilon = 10e-3f;

		return (
			a1 <= epsilon && a1 >= -epsilon &&
			a2 <= epsilon && a2 >= -epsilon &&
			a3 <= epsilon && a3 >= -epsilon &&
			b1 <= epsilon && b1 >= -epsilon &&
			b2 <= epsilon && b2 >= -epsilon &&
			b3 <= epsilon && b3 >= -epsilon &&
			c1 <= epsilon && c1 >= -epsilon &&
			c2 <= epsilon && c2 >= -epsilon &&
			c3 <= epsilon && c3 >= -epsilon &&
			a4 <= 1.f + epsilon && a4 >= 1.f - epsilon &&
			b4 <= 1.f + epsilon && b4 >= 1.f - epsilon &&
			c4 <= 1.f + epsilon && c4 >= 1.f - epsilon);
	}

	Matrix4x4t<T>& applyMatrix(const Matrix4x4t<T>& m) noexcept
	{
		Matrix4x4t matrix(*this);
		multiplyMatrices(matrix, m);
		return *this;
	}

	Matrix4x4t<T>& multiplyMatrices(const Matrix4x4t<T>& m1, const Matrix4x4t<T>& m2) noexcept
	{
		a1 = m1.a1 * m2.a1 + m1.b1 * m2.a2 + m1.c1 * m2.a3 + m1.d1 * m2.a4;
		a2 = m1.a2 * m2.a1 + m1.b2 * m2.a2 + m1.c2 * m2.a3 + m1.d2 * m2.a4;
		a3 = m1.a3 * m2.a1 + m1.b3 * m2.a2 + m1.c3 * m2.a3 + m1.d3 * m2.a4;
		a4 = m1.a4 * m2.a1 + m1.b4 * m2.a2 + m1.c4 * m2.a3 + m1.d4 * m2.a4;
		b1 = m1.a1 * m2.b1 + m1.b1 * m2.b2 + m1.c1 * m2.b3 + m1.d1 * m2.b4;
		b2 = m1.a2 * m2.b1 + m1.b2 * m2.b2 + m1.c2 * m2.b3 + m1.d2 * m2.b4;
		b3 = m1.a3 * m2.b1 + m1.b3 * m2.b2 + m1.c3 * m2.b3 + m1.d3 * m2.b4;
		b4 = m1.a4 * m2.b1 + m1.b4 * m2.b2 + m1.c4 * m2.b3 + m1.d4 * m2.b4;
		c1 = m1.a1 * m2.c1 + m1.b1 * m2.c2 + m1.c1 * m2.c3 + m1.d1 * m2.c4;
		c2 = m1.a2 * m2.c1 + m1.b2 * m2.c2 + m1.c2 * m2.c3 + m1.d2 * m2.c4;
		c3 = m1.a3 * m2.c1 + m1.b3 * m2.c2 + m1.c3 * m2.c3 + m1.d3 * m2.c4;
		c4 = m1.a4 * m2.c1 + m1.b4 * m2.c2 + m1.c4 * m2.c3 + m1.d4 * m2.c4;
		d1 = m1.a1 * m2.d1 + m1.b1 * m2.d2 + m1.c1 * m2.d3 + m1.d1 * m2.d4;
		d2 = m1.a2 * m2.d1 + m1.b2 * m2.d2 + m1.c2 * m2.d3 + m1.d2 * m2.d4;
		d3 = m1.a3 * m2.d1 + m1.b3 * m2.d2 + m1.c3 * m2.d3 + m1.d3 * m2.d4;
		d4 = m1.a4 * m2.d1 + m1.b4 * m2.d2 + m1.c4 * m2.d3 + m1.d4 * m2.d4;
		return *this;
	}

	Matrix4x4t<T>& multiplyScalar(T scale) noexcept
	{
		a1 *= scale; a2 *= scale; a3 *= scale; a4 *= scale;
		b1 *= scale; b2 *= scale; b3 *= scale; b4 *= scale;
		c1 *= scale; c2 *= scale; c3 *= scale; c4 *= scale;
		d1 *= scale; d2 *= scale; d3 *= scale; d4 *= scale;
		return *this;
	}

	Matrix4x4t<T>& makeTranslate(const Vector2t<T>& pt) noexcept { return makeTranslate(pt.x, pt.y, 0); }
	Matrix4x4t<T>& makeTranslate(const Vector3t<T>& pt) noexcept  { return makeTranslate(pt.x, pt.y, pt.z); }
	Matrix4x4t<T>& makeTranslate(T x, T y) noexcept { return makeTranslate(x, y, 0); }
	Matrix4x4t<T>& makeTranslate(T x, T y, T z) noexcept
	{
		set(1, 0, 0, x,
			0, 1, 0, y,
			0, 0, 1, z,
			0, 0, 0, 1);
		return *this;
	}

	Matrix4x4t<T>& setTranslate(const Vector2t<T>& pt) noexcept { return setTranslate(pt.x, pt.y, 0); }
	Matrix4x4t<T>& setTranslate(const Vector3t<T>& pt) noexcept  { return setTranslate(pt.x, pt.y, pt.z); }
	Matrix4x4t<T>& setTranslate(T x, T y) noexcept  { return setTranslate(x, y); }
	Matrix4x4t<T>& setTranslate(T x, T y, T z) noexcept
	{
		a4 = x;
		b4 = y;
		c4 = z;
		return *this;
	}

	Matrix4x4t<T>& translate(const Vector2t<T>& pt) noexcept { return translate(pt.x, pt.y, 0); }
	Matrix4x4t<T>& translate(const Vector3t<T>& pt) noexcept { return translate(pt.x, pt.y, pt.z); }
	Matrix4x4t<T>& translate(T x, T y) noexcept { return translate(x, y, 0); }
	Matrix4x4t<T>& translate(T x, T y, T z) noexcept
	{
		a4 += x;
		b4 += y;
		c4 += z;
		return *this;
	}

	Vector3t<T> getTranslate() const noexcept
	{
		return Vector3t<T>(a4, b4, c4);
	}

	Matrix4x4t<T>& makeScale(const Vector2t<T>& sz) noexcept  { return makeScale(sz.x, sz.y, 1.0f); }
	Matrix4x4t<T>& makeScale(const Vector3t<T>& sz) noexcept  { return makeScale(sz.x, sz.y, sz.z); }
	Matrix4x4t<T>& makeScale(T cx, T cy) noexcept { return makeScale(cx, cy, 1.0f); }
	Matrix4x4t<T>& makeScale(T x, T y, T z) noexcept
	{
		set(
			x, 0, 0, 0,
			0, y, 0, 0,
			0, 0, z, 0,
			0, 0, 0, 1);
		return *this;
	}

	Matrix4x4t<T>& scale(const Vector2t<T>& sz) noexcept  { return scale(sz.x, sz.y, 1.0f); }
	Matrix4x4t<T>& scale(const Vector3t<T>& sz) noexcept { return scale(sz.x, sz.y, sz.z); }
	Matrix4x4t<T>& scale(T v) noexcept  { return scale(v, v, v); }
	Matrix4x4t<T>& scale(T cx, T cy) noexcept { return scale(cx, cy, 1.0f); }
	Matrix4x4t<T>& scale(T x, T y, T z) noexcept
	{
		a1 *= x;
		a2 *= x;
		a3 *= x;
		b1 *= y;
		b2 *= y;
		b3 *= y;
		c1 *= z;
		c2 *= z;
		c3 *= z;
		return *this;
	}

	Matrix4x4t<T>& makeRotationX(T theta) noexcept
	{
		T c, s, ang = DEG_TO_RAD(theta);

		math::sinCos(&s, &c, ang);

		set(
			1, 0, 0, 0,
			0, c, -s, 0,
			0, s, c, 0,
			0, 0, 0, 1);

		return *this;
	}

	Matrix4x4t<T>& makeRotationY(T theta) noexcept
	{
		T c, s, ang = DEG_TO_RAD(theta);

		math::sinCos(&s, &c, ang);

		set(
			c, 0, s, 0,
			0, 1, 0, 0,
			-s, 0, c, 0,
			0, 0, 0, 1);

		return *this;
	}

	Matrix4x4t<T>& makeRotationZ(T theta) noexcept
	{
		T c, s, ang = DEG_TO_RAD(theta);

		math::sinCos(&s, &c, ang);

		set(
			c, -s, 0, 0,
			s, c, 0, 0,
			0, 0, 1, 0,
			0, 0, 0, 1);

		return *this;
	}

	Matrix4x4t<T>& makeRotate(const EulerAnglest<T>& euler) noexcept
	{
		T sh, ch, sp, cp, sb, cb;

		math::sinCos(&sp, &cp, euler.x);
		math::sinCos(&sh, &ch, euler.y);
		math::sinCos(&sb, &cb, euler.z);

		a1 = ch * cb + sh * sp * sb;
		a2 = -ch * sb + sh * sp * cb;
		a3 = sh * cp;

		b1 = sb * cp;
		b2 = cb * cp;
		b3 = -sp;

		c1 = -sh * cb + ch * sp * sb;
		c2 = sb * sh + ch * sp * cb;
		c3 = ch * cp;

		d1 = 0;
		d2 = 0;
		d3 = 0;
		d4 = 1;

		return *this;
	}

	Matrix4x4t<T>& makeRotateEulerAngles(T x, T y, T z) noexcept
	{
		EulerAnglest<T> euler(x, y, z);
		this->makeRotate(euler);
		return *this;
	}

	Matrix4x4t<T>& makeRotateEulerAngles(const Vector3t<T>& xyz) noexcept
	{
		EulerAnglest<T> euler(xyz);
		this->makeRotate(euler);
	}

	Matrix4x4t<T>& makeRotate(T angle, T x, T y, T z) noexcept
	{
		return makeRotate(angle, Vector3t<T>(x, y, z));
	}

	Matrix4x4t<T>& makeRotate(const Vector3t<T>& from, const Vector3t<T>& to) noexcept
	{
		Matrix3x3t<T> m3;
		Matrix3x3t<T>::fromToMatrix(from, to, m3);
		*this = Matrix4x4t<T>(m3);
		return *this;
	}

	Matrix4x4t<T>& makeRotate(const Quaterniont<T>& q) noexcept
	{
		a1 = 1.0f - 2.0f * (q.y * q.y + q.z * q.z);
		a2 = 2.0f * (q.x * q.y - q.z * q.w);
		a3 = 2.0f * (q.x * q.z + q.y * q.w);
		a4 = 0;

		b1 = 2.0f * (q.x * q.y + q.z * q.w);
		b2 = 1.0f - 2.0f * (q.x * q.x + q.z * q.z);
		b3 = 2.0f * (q.y * q.z - q.x * q.w);
		b4 = 0;

		c1 = 2.0f * (q.x * q.z - q.y * q.w);
		c2 = 2.0f * (q.y * q.z + q.x * q.w);
		c3 = 1.0f - 2.0f * (q.x * q.x + q.y * q.y);
		c4 = 0;

		d1 = 0;
		d2 = 0;
		d3 = 0;
		d4 = 1;

		return *this;
	}

	Matrix4x4t<T>& makeRotate(T angle, const Vector3t<T>& axis) noexcept
	{
		T c, s;

		math::sinCos(&s, &c, DEG_TO_RAD(angle));

		Vector3t<T> v = ~axis;

		T x = v.x;
		T y = v.y;
		T z = v.z;

		T t = 1 - c;
		T tx = t*x, ty = t*y;

		a1 = (tx * x + c);
		a2 = (tx * y + s * z);
		a3 = (tx * z - s * y);
		a4 = 0;

		b1 = (tx * y - s * z);
		b2 = (ty * y + c);
		b3 = (ty * z + s * x);
		b4 = 0;

		c1 = (tx * z + s * y);
		c2 = (ty * z - s * x);
		c3 = (t * z * z + c);
		c4 = 0;

		d1 = 0;
		d2 = 0;
		d3 = 0;
		d4 = 1;

		return *this;
	}

	Matrix4x4t<T>& makeRotate(const Vector3t<T>& forward, const Vector3t<T>& up, const Vector3t<T>& right) noexcept
	{
		a1 = right.x;
		b1 = right.y;
		c1 = right.z;
		d1 = 0.0f;

		a2 = up.x;
		b2 = up.y;
		c2 = up.z;
		d2 = 0.0f;

		a3 = forward.x;
		b3 = forward.y;
		c3 = forward.z;
		d3 = 0.0f;

		a4 = 0.0f;
		b4 = 0.0f;
		c4 = 0.0f;
		d4 = 1.0f;
		return *this;
	}

	Vector3t<T> getRight() const noexcept
	{
		return Vector3t<T>(a1, b1, c1);
	}

	Vector3t<T> getUpVector() const noexcept
	{
		return Vector3t<T>(a2, b2, c2);
	}

	Vector3t<T> getForward() const noexcept
	{
		return Vector3t<T>(a3, b3, c3);
	}

	Vector4t<T> getAxisX() const noexcept
	{
		return Vector4t<T>(a1, a2, a3, a4);
	}

	Vector4t<T> getAxisY() const noexcept
	{
		return Vector4t<T>(b1, b2, b3, b4);
	}

	Vector4t<T> getAxisZ() const noexcept
	{
		return Vector4t<T>(c1, c2, c3, c4);
	}

	Matrix4x4t<T>& makeTransform(const Vector3t<T>& translate, const Quaterniont<T>& rotate, const Vector3t<T>& scale) noexcept
	{
		this->makeRotate(rotate);
		this->setTranslate(translate);
		this->scale(scale);
		return *this;
	}

	void getTransform(Quaterniont<T>& rotation, Vector3t<T>& position) const noexcept
	{
		const Matrix4x4t<T>& _this = *this;

		// extract translation
		position.x = _this[3][0];
		position.y = _this[3][1];
		position.z = _this[3][2];

		// extract rotation
		rotation = Quaterniont<T>((Matrix3x3t<T>)_this);
	}

	void getTransform(Vector3t<T>& scaling, Quaterniont<T>& rotation, Vector3t<T>& position) const noexcept
	{
		const Matrix4x4t<T>& _this = *this;

		// extract translation
		position.x = _this[0][3];
		position.y = _this[1][3];
		position.z = _this[2][3];

		// extract the rows of the matrix
		Vector3t<T> vRows[3] =
		{
			Vector3t<T>(_this[0][0],_this[1][0],_this[2][0]),
			Vector3t<T>(_this[0][1],_this[1][1],_this[2][1]),
			Vector3t<T>(_this[0][2],_this[1][2],_this[2][2])
		};

		// extract the scaling factors
		scaling.x = vRows[0].length();
		scaling.y = vRows[1].length();
		scaling.z = vRows[2].length();

		// and the sign of the scaling
		if (determinant() < 0)
		{
			scaling.x = -scaling.x;
			scaling.y = -scaling.y;
			scaling.z = -scaling.z;
		}

		// and remove all scaling from the matrix
		if (scaling.x)
		{
			vRows[0] /= scaling.x;
		}
		if (scaling.y)
		{
			vRows[1] /= scaling.y;
		}
		if (scaling.z)
		{
			vRows[2] /= scaling.z;
		}

		// build a 3x3 rotation matrix
		Matrix3x3t<T> m(
			vRows[0].x, vRows[1].x, vRows[2].x,
			vRows[0].y, vRows[1].y, vRows[2].y,
			vRows[0].z, vRows[1].z, vRows[2].z);

		// and generate the rotation quaternion from it
		rotation = Quaterniont<T>(m);
	}

	Matrix4x4t<T>& makeOrtho_lh(T width, T height, T zNear, T zFar) noexcept
	{
		T cx = 2.0f / width;
		T cy = 2.0f / height;
		T cz = 1.0f / (zFar - zNear);
		T tz = zNear / (zNear - zFar);

		set(cx, 0.0, 0.0, 0.0,
			0.0, cy, 0.0, 0.0,
			0.0, 0.0, cz, tz,
			0.0, 0.0, 0.0, 1.0);

		return *this;
	}

	Matrix4x4t<T>& makeOrtho_lh(T left, T right, T bottom, T top, T zNear, T zFar) noexcept
	{
		T tx = -(right + left) / (right - left);
		T ty = -(top + bottom) / (top - bottom);
		T tz = -zNear / (zFar - zNear);
		T cx = 2.0f / (right - left);
		T cy = 2.0f / (top - bottom);
		T cz = 1.0f / (zFar - zNear);

		set(cx, 0.0, 0.0, tx,
			0.0, cy, 0.0, ty,
			0.0, 0.0, cz, tz,
			0.0, 0.0, 0.0, 1.0);

		return *this;
	}

	Matrix4x4t<T>& makeOrtho_rh(T left, T right, T bottom, T top, T zNear, T zFar) noexcept
	{
		T tx = -(right + left) / (right - left);
		T ty = -(top + bottom) / (top - bottom);
		T tz = -(zFar + zNear) / (zFar - zNear);
		T cx = 2.0f / (right - left);
		T cy = 2.0f / (top - bottom);
		T cz = -2.0f / (zFar - zNear);

		set(cx, 0.0, 0.0, tx,
			0.0, cy, 0.0, ty,
			0.0, 0.0, cz, tz,
			0.0, 0.0, 0.0, 1.0);

		return *this;
	}

	bool getOrtho_rh(T& left, T& right, T& bottom, T& top, T& zNear, T& zFar) const noexcept
	{
		if (a4 != 0.0 || b4 != 0.0 || c4 != 0.0 || c4 != 1.0) return false;

		zNear = (d3 + 1.0f) / c3;
		zFar = (d3 - 1.0f) / c3;

		left = -(1.0f + d1) / a1;
		right = (1.0f - d1) / a1;

		bottom = -(1.0f + d2) / b2;
		top = (1.0f - d2) / b2;

		return true;
	}

	Matrix4x4t<T>& makeFrustumt_lh(T left, T right, T bottom, T top, T zNear, T zFar) noexcept
	{
		T A = (left + right) / (left - right);
		T B = (bottom + top) / (bottom - top);
		T C = (zFar > DBL_MAX) ? 1.0f : (zFar + zNear) / (zFar - zNear);
		T D = (zFar > DBL_MAX) ? zNear : 2.0 * zNear * zFar / (zNear - zFar);
		T cx = 2.0f * zNear / (right - left);
		T cy = 2.0f * zNear / (top - bottom);

		set(cx, 0.0, A, 0.0,
			0.0, cy, B, 0.0,
			0.0, 0.0, C, D,
			0.0, 0.0, 1.0, 0.0);

		return *this;
	}

	Matrix4x4t<T>& makeFrustumt_rh(T left, T right, T bottom, T top, T zNear, T zFar) noexcept
	{
		T A = (right + left) / (right - left);
		T B = (top + bottom) / (top - bottom);
		T C = (zFar > DBL_MAX) ? -1.0f : -(zFar + zNear) / (zFar - zNear);
		T D = (zFar > DBL_MAX) ? -zNear : -2.0 * zFar * zNear / (zFar - zNear);
		T cx = 2.0f * zNear / (right - left);
		T cy = 2.0f * zNear / (top - bottom);

		set(cx, 0.0, A, 0.0,
			0.0, cy, B, 0.0,
			0.0, 0.0, C, D,
			0.0, 0.0, -1.0, 0.0);

		return *this;
	}

	bool getFrustumt_rh(T& left, T& right, T& bottom, T& top, T& zNear, T& zFar) const noexcept
	{
		if (a4 != 0.0 || b4 != 0.0 || c4 != -1.0 || c4 != 0.0)
			return false;

		T temp_near = d3 / (c3 - 1.0f);
		T temp_far = d3 / (1.0f + c3);

		left = temp_near * (c1 - 1.0f) / a1;
		right = temp_near * (1.0f + c1) / a1;

		top = temp_near * (1.0f + c2) / b2;
		bottom = temp_near * (c2 - 1.0f) / b2;

		zNear = temp_near;
		zFar = temp_far;

		return true;
	}

	template <typename T>
	Matrix4x4t<T>& makePerspective_fov_lh(const T& fov, const T& aspect, const T& nearPlane, const T& farPlane) noexcept
	{
		const T h = 1.0f / tan(DEG_TO_RAD(fov * 0.5f));
		const T w = h / aspect;
		const T q = farPlane / (farPlane - nearPlane);

		set(w, 0, 0, 0,
			0, h, 0, 0,
			0, 0, q, -nearPlane * q,
			0, 0, 1.0f, 0);

		return *this;
	}

	template <typename T>
	Matrix4x4t<T>& makePerspective_fov_rh(const T& fov, const T& aspect, const T& nearPlane, const T& farPlane) noexcept
	{
		const T h = 1.0f / tan(DEG_TO_RAD(fov * 0.5f));
		const T w = h / aspect;
		const T q = farPlane / (farPlane - nearPlane);

		set(w, 0, 0, 0,
			0, h, 0, 0,
			0, 0, q, nearPlane * q,
			0, 0, -1.0f, 0);

		return *this;
	}

	Matrix4x4t<T>& makePerspective_off_center_lh(const T& fovy, const T& aspectRatio, const T& znear, const T& zfar) noexcept
	{
		T tan_fovy = tan(DEG_TO_RAD(fovy * 0.5f));
		T right = tan_fovy * aspectRatio * znear;
		T left = -right;
		T top = tan_fovy * znear;
		T bottom = -top;

		makeFrustumt_lh(left, right, bottom, top, znear, zfar);
		return *this;
	}

	Matrix4x4t<T>& makePerspective_off_center_rh(const T& fovy, const T& aspectRatio, const T& zNear, const T& zFar) noexcept
	{
		T tan_fovy = tan(DEG_TO_RAD(fovy * 0.5f));
		T right = tan_fovy * aspectRatio * zNear;
		T left = -right;
		T top = tan_fovy * zNear;
		T bottom = -top;

		makeFrustumt_rh(left, right, bottom, top, zNear, zFar);
		return *this;
	}

	bool getPerspective_rh(T& fovy, T& aspectRatio, T& zNear, T& zFar) const noexcept
	{
		T right = 0.0;
		T left = 0.0;
		T top = 0.0;
		T bottom = 0.0;

		T temp_near = 0.0;
		T temp_far = 0.0;

		bool r = getFrustumt(left, right, bottom, top, temp_near, temp_far);

		if (r)
		{
			fovy = degrees(atan(top / temp_near) - atan(bottom / temp_near));
			aspectRatio = (right - left) / (top - bottom);
		}

		zNear = temp_near;
		zFar = temp_far;
		return r;
	}

	Matrix4x4t<T>& makeLookAt_lh(const Vector3t<T>& eye, const Vector3t<T>& center, const Vector3t<T>& up) noexcept
	{
		Vector3t<T> x, y, z;

		z = center - eye;
		z = math::normalize(z);

		x = math::cross(up, z);
		x = math::normalize(x);

		y = math::cross(z, x);
		y = math::normalize(y);

		set(x.x, x.y, x.z, 0.0,
			y.x, y.y, y.z, 0.0,
			z.x, z.y, z.z, 0.0,
			0.0, 0.0, 0.0, 1.0);

		Vector3t<T> tmp = -eye;
		if (tmp.x != 0)
		{
			a4 += tmp.x * a1;
			b4 += tmp.x * b1;
			c4 += tmp.x * c1;
			d4 += tmp.x * d1;
		}

		if (tmp.y != 0)
		{
			a4 += tmp.y * a2;
			b4 += tmp.y * b2;
			c4 += tmp.y * c2;
			d4 += tmp.y * d2;
		}

		if (tmp.z != 0)
		{
			a4 += tmp.z * a3;
			b4 += tmp.z * b3;
			c4 += tmp.z * c3;
			d4 += tmp.z * d3;
		}

		return *this;
	}

	Matrix4x4t<T>& makeLookAt_rh(const Vector3t<T>& eye, const Vector3t<T>& center, const Vector3t<T>& up) noexcept
	{
		Vector3t<T> x, y, z;

		z = eye - center;
		z = math::normalize(z);

		x = math::cross(up, z);
		x = math::normalize(x);

		y = math::cross(z, x);
		y = math::normalize(y);

		set(x.x, x.y, x.z, 0.0,
			y.x, y.y, y.z, 0.0,
			z.x, z.y, z.z, 0.0,
			0.0, 0.0, 0.0, 1.0);

		Vector3t<T> tmp = -eye;
		if (tmp.x != 0)
		{
			a4 += tmp.x * a1;
			b4 += tmp.x * b1;
			c4 += tmp.x * c1;
			d4 += tmp.x * d1;
		}

		if (tmp.y != 0)
		{
			a4 += tmp.y * a2;
			b4 += tmp.y * b2;
			c4 += tmp.y * c2;
			d4 += tmp.y * d2;
		}

		if (tmp.z != 0)
		{
			a4 += tmp.z * a3;
			b4 += tmp.z * b3;
			c4 += tmp.z * c3;
			d4 += tmp.z * d3;
		}

		return *this;
	}

	Matrix4x4t<T>& makeViewport(std::size_t left, std::size_t top, std::size_t width, std::size_t height) noexcept
	{
		T cx = (T)(width >> 1);
		T cy = (T)(height >> 1);
		T A = cx + left;
		T B = cy + top;

		set(cx, 0.0, 0.0, A,
			0.0, cy, 0.0, B,
			0.0, 0.0, 0.5, 0.5,
			0.0, 0.0, 0.0, 1.0);

		return *this;
	}
};

template<typename T> const Matrix4x4t<T> Matrix4x4t<T>::Zero(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
template<typename T> const Matrix4x4t<T> Matrix4x4t<T>::One(1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1);

template <typename T>
inline bool operator==(const Matrix4x4t<T>& m1, const Matrix4x4t<T>& m2) noexcept
{
	return
		m1.a1 == m2.a1 && m1.a2 == m2.a2 && m1.a3 == m2.a3 && m1.a4 == m2.a4 &&
		m1.b1 == m2.b1 && m1.b2 == m2.b2 && m1.b3 == m2.b3 && m1.b4 == m2.b4 &&
		m1.c1 == m2.c1 && m1.c2 == m2.c2 && m1.c3 == m2.c3 && m1.c4 == m2.c4 &&
		m1.d1 == m2.d1 && m1.d2 == m2.d2 && m1.d3 == m2.d3 && m1.d4 == m2.c4;
}

template <typename T>
inline bool operator!=(const Matrix4x4t<T>& m1, const Matrix4x4t<T>& m2) noexcept
{
	return
		m1.a1 != m2.a1 || m1.a2 != m2.a2 || m1.a3 != m2.a3 || m1.a4 != m2.a4 ||
		m1.b1 != m2.b1 || m1.b2 != m2.b2 || m1.b3 != m2.b3 || m1.b4 != m2.b4 ||
		m1.c1 != m2.c1 || m1.c2 != m2.c2 || m1.c3 != m2.c3 || m1.c4 != m2.c4 ||
		m1.d1 != m2.d1 || m1.d2 != m2.d2 || m1.d3 != m2.d3 || m1.d4 != m2.c4;
}

template<typename T>
inline Matrix4x4t<T> operator*(const Matrix4x4t<T>& m1, const Matrix4x4t<T>& m2) noexcept
{
	return Matrix4x4t<T>(m1, m2);
}

template<typename T>
inline Vector3t<T> operator*(const Vector3t<T>& v, const Matrix4x4t<T>& m) noexcept
{
	return Vector3t<T>(
		m.a1 * v.x + m.a2 * v.y + m.a3 * v.z + m.a4,
		m.b1 * v.x + m.b2 * v.y + m.b3 * v.z + m.b4,
		m.c1 * v.x + m.c2 * v.y + m.c3 * v.z + m.c4);
}

template<typename T>
inline Vector3t<T> operator*(const Matrix4x4t<T>& m, const Vector3t<T>& v) noexcept
{
	return Vector3t<T>(
		m.a1 * v.x + m.b1 * v.y + m.c1 * v.z + m.d1,
		m.a2 * v.x + m.b2 * v.y + m.c2 * v.z + m.d2,
		m.a3 * v.x + m.b3 * v.y + m.c3 * v.z + m.d3);
}

template<typename T>
inline Vector3t<T>& operator*=(const Matrix4x4t<T>& m, Vector3t<T>& v) noexcept
{
	v = m * v;
	return v;
}

template<typename T>
inline Vector3t<T>& operator*=(Vector3t<T>& v, const Matrix4x4t<T>& m) noexcept
{
	v = v * m;
	return v;
}

template<typename T>
inline Vector4t<T> operator*(const Matrix4x4t<T>& m, const Vector4t<T>& v) noexcept
{
	return Vector4t<T>(
		m.a1 * v.x + m.b1 * v.y + m.c1 * v.z + m.d1 * v.w,
		m.a2 * v.x + m.b2 * v.y + m.c2 * v.z + m.d2 * v.w,
		m.a3 * v.x + m.b3 * v.y + m.c3 * v.z + m.d3 * v.w,
		m.a4 * v.x + m.b4 * v.y + m.c4 * v.z + m.d4 * v.w);
}

template<typename T>
inline Vector4t<T> operator*(const Vector4t<T>& v, const Matrix4x4t<T>& m) noexcept
{
	return Vector4t<T>(
		m.a1 * v.x + m.a2 * v.y + m.a3 * v.z + v.w * m.a4,
		m.b1 * v.x + m.b2 * v.y + m.b3 * v.z + v.w * m.b4,
		m.c1 * v.x + m.c2 * v.y + m.c3 * v.z + v.w * m.c4,
		m.d1 * v.x + m.d2 * v.y + m.d3 * v.z + v.w * m.d4);
}

template<typename T>
inline Vector4t<T>& operator*=(Vector4t<T>& v, const Matrix4x4t<T>& m) noexcept
{
	v = v * m;
	return v;
}

template<typename T>
inline Matrix4x4t<T>& operator*=(Matrix4x4t<T> m1, const Matrix4x4t<T>& m2) noexcept
{
	m1 = Matrix4x4t<T>(
		m1.a1 * m2.a1 + m1.b1 * m2.a2 + m1.c1 * m2.a3 + m1.d1 * m2.a4,
		m1.a2 * m2.a1 + m1.b2 * m2.a2 + m1.c2 * m2.a3 + m1.d2 * m2.a4,
		m1.a3 * m2.a1 + m1.b3 * m2.a2 + m1.c3 * m2.a3 + m1.d3 * m2.a4,
		m1.a4 * m2.a1 + m1.b4 * m2.a2 + m1.c4 * m2.a3 + m1.d4 * m2.a4,
		m1.a1 * m2.b1 + m1.b1 * m2.b2 + m1.c1 * m2.b3 + m1.d1 * m2.b4,
		m1.a2 * m2.b1 + m1.b2 * m2.b2 + m1.c2 * m2.b3 + m1.d2 * m2.b4,
		m1.a3 * m2.b1 + m1.b3 * m2.b2 + m1.c3 * m2.b3 + m1.d3 * m2.b4,
		m1.a4 * m2.b1 + m1.b4 * m2.b2 + m1.c4 * m2.b3 + m1.d4 * m2.b4,
		m1.a1 * m2.c1 + m1.b1 * m2.c2 + m1.c1 * m2.c3 + m1.d1 * m2.c4,
		m1.a2 * m2.c1 + m1.b2 * m2.c2 + m1.c2 * m2.c3 + m1.d2 * m2.c4,
		m1.a3 * m2.c1 + m1.b3 * m2.c2 + m1.c3 * m2.c3 + m1.d3 * m2.c4,
		m1.a4 * m2.c1 + m1.b4 * m2.c2 + m1.c4 * m2.c3 + m1.d4 * m2.c4,
		m1.a1 * m2.d1 + m1.b1 * m2.d2 + m1.c1 * m2.d3 + m1.d1 * m2.d4,
		m1.a2 * m2.d1 + m1.b2 * m2.d2 + m1.c2 * m2.d3 + m1.d2 * m2.d4,
		m1.a3 * m2.d1 + m1.b3 * m2.d2 + m1.c3 * m2.d3 + m1.d3 * m2.d4,
		m1.a4 * m2.d1 + m1.b4 * m2.d2 + m1.c4 * m2.d3 + m1.d4 * m2.d4);
	return m1;
}

namespace math
{
	template<typename T>
	Matrix4x4t<T> orthonormalize(const Matrix4x4t<T>& _m) noexcept
	{
		Matrix4x4t<T> m = _m;
		Vector3t<T> x(m.a1, m.b1, m.c1);
		Vector3t<T> y(m.a2, m.b2, m.c2);
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
	Matrix4x4t<T> transpose(const Matrix4x4t<T>& _m) noexcept
	{
		Matrix4x4t<T> m = _m;
		std::swap((T&)m.b1, (T&)m.a2);
		std::swap((T&)m.c1, (T&)m.a3);
		std::swap((T&)m.c2, (T&)m.b3);
		std::swap((T&)m.d1, (T&)m.a4);
		std::swap((T&)m.d2, (T&)m.b4);
		std::swap((T&)m.d3, (T&)m.c4);
		return m;
	}

	template<typename T>
	Matrix4x4t<T> inverse(const Matrix4x4t<T>& _m) noexcept
	{
		Matrix4x4t<T> m = _m;

		const T det = m.determinant();
		if (det == static_cast<T>(0.0))
		{
			const T nan = std::numeric_limits<T>::quiet_NaN();
			m.set
				(
					nan, nan, nan, nan,
					nan, nan, nan, nan,
					nan, nan, nan, nan,
					nan, nan, nan, nan
					);

			return m;
		}

		const T invdet = static_cast<T>(1.0) / det;
		m.a1 = invdet * (_m.b2 * (_m.c3 * _m.d4 - _m.c4 * _m.d3) + _m.b3 * (_m.c4 * _m.d2 - _m.c2 * _m.d4) + _m.b4 * (_m.c2 * _m.d3 - _m.c3 * _m.d2));
		m.a2 = -invdet * (_m.a2 * (_m.c3 * _m.d4 - _m.c4 * _m.d3) + _m.a3 * (_m.c4 * _m.d2 - _m.c2 * _m.d4) + _m.a4 * (_m.c2 * _m.d3 - _m.c3 * _m.d2));
		m.a3 = invdet * (_m.a2 * (_m.b3 * _m.d4 - _m.b4 * _m.d3) + _m.a3 * (_m.b4 * _m.d2 - _m.b2 * _m.d4) + _m.a4 * (_m.b2 * _m.d3 - _m.b3 * _m.d2));
		m.a4 = -invdet * (_m.a2 * (_m.b3 * _m.c4 - _m.b4 * _m.c3) + _m.a3 * (_m.b4 * _m.c2 - _m.b2 * _m.c4) + _m.a4 * (_m.b2 * _m.c3 - _m.b3 * _m.c2));
		m.b1 = -invdet * (_m.b1 * (_m.c3 * _m.d4 - _m.c4 * _m.d3) + _m.b3 * (_m.c4 * _m.d1 - _m.c1 * _m.d4) + _m.b4 * (_m.c1 * _m.d3 - _m.c3 * _m.d1));
		m.b2 = invdet * (_m.a1 * (_m.c3 * _m.d4 - _m.c4 * _m.d3) + _m.a3 * (_m.c4 * _m.d1 - _m.c1 * _m.d4) + _m.a4 * (_m.c1 * _m.d3 - _m.c3 * _m.d1));
		m.b3 = -invdet * (_m.a1 * (_m.b3 * _m.d4 - _m.b4 * _m.d3) + _m.a3 * (_m.b4 * _m.d1 - _m.b1 * _m.d4) + _m.a4 * (_m.b1 * _m.d3 - _m.b3 * _m.d1));
		m.b4 = invdet * (_m.a1 * (_m.b3 * _m.c4 - _m.b4 * _m.c3) + _m.a3 * (_m.b4 * _m.c1 - _m.b1 * _m.c4) + _m.a4 * (_m.b1 * _m.c3 - _m.b3 * _m.c1));
		m.c1 = invdet * (_m.b1 * (_m.c2 * _m.d4 - _m.c4 * _m.d2) + _m.b2 * (_m.c4 * _m.d1 - _m.c1 * _m.d4) + _m.b4 * (_m.c1 * _m.d2 - _m.c2 * _m.d1));
		m.c2 = -invdet * (_m.a1 * (_m.c2 * _m.d4 - _m.c4 * _m.d2) + _m.a2 * (_m.c4 * _m.d1 - _m.c1 * _m.d4) + _m.a4 * (_m.c1 * _m.d2 - _m.c2 * _m.d1));
		m.c3 = invdet * (_m.a1 * (_m.b2 * _m.d4 - _m.b4 * _m.d2) + _m.a2 * (_m.b4 * _m.d1 - _m.b1 * _m.d4) + _m.a4 * (_m.b1 * _m.d2 - _m.b2 * _m.d1));
		m.c4 = -invdet * (_m.a1 * (_m.b2 * _m.c4 - _m.b4 * _m.c2) + _m.a2 * (_m.b4 * _m.c1 - _m.b1 * _m.c4) + _m.a4 * (_m.b1 * _m.c2 - _m.b2 * _m.c1));
		m.d1 = -invdet * (_m.b1 * (_m.c2 * _m.d3 - _m.c3 * _m.d2) + _m.b2 * (_m.c3 * _m.d1 - _m.c1 * _m.d3) + _m.b3 * (_m.c1 * _m.d2 - _m.c2 * _m.d1));
		m.d2 = invdet * (_m.a1 * (_m.c2 * _m.d3 - _m.c3 * _m.d2) + _m.a2 * (_m.c3 * _m.d1 - _m.c1 * _m.d3) + _m.a3 * (_m.c1 * _m.d2 - _m.c2 * _m.d1));
		m.d3 = -invdet * (_m.a1 * (_m.b2 * _m.d3 - _m.b3 * _m.d2) + _m.a2 * (_m.b3 * _m.d1 - _m.b1 * _m.d3) + _m.a3 * (_m.b1 * _m.d2 - _m.b2 * _m.d1));
		m.d4 = invdet * (_m.a1 * (_m.b2 * _m.c3 - _m.b3 * _m.c2) + _m.a2 * (_m.b3 * _m.c1 - _m.b1 * _m.c3) + _m.a3 * (_m.b1 * _m.c2 - _m.b2 * _m.c1));

		return m;
	}
}

_NAME_END

#endif
