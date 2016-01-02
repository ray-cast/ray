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

	Matrix4x4t()
	{
	}

	Matrix4x4t(T _a1, T _a2, T _a3, T _a4,
		T _b1, T _b2, T _b3, T _b4,
		T _c1, T _c2, T _c3, T _c4,
		T _d1, T _d2, T _d3, T _d4)
		: a1(_a1), a2(_a2), a3(_a3), a4(_a4)
		, b1(_b1), b2(_b2), b3(_b3), b4(_b4)
		, c1(_c1), c2(_c2), c3(_c3), c4(_c4)
		, d1(_d1), d2(_d2), d3(_d3), d4(_d4)
	{
	}

	template<typename S>
	explicit Matrix4x4t(const Matrix4x4t<S>& m)
	{
		a1 = static_cast<T>(m.a1); a2 = static_cast<T>(m.a2); a3 = static_cast<T>(m.a3); a4 = static_cast<T>(m.a4);
		b1 = static_cast<T>(m.b1); b2 = static_cast<T>(m.b2); b3 = static_cast<T>(m.b3); b4 = static_cast<T>(m.b4);
		c1 = static_cast<T>(m.c1); c2 = static_cast<T>(m.c2); c3 = static_cast<T>(m.c3); c4 = static_cast<T>(m.c4);
		d1 = static_cast<T>(m.d1); d2 = static_cast<T>(m.d2); d3 = static_cast<T>(m.d3); d4 = static_cast<T>(m.d4);
	}

	Matrix4x4t(const Matrix4x4t<T>& m1, const Matrix4x4t<T>& m2)
	{
		this->multiplyMatrices(m1, m2);
	}

	template <typename S>
	operator Matrix4x4t<S>() const
	{
		return Matrix4x4t<S>(
			static_cast<S>(a1), static_cast<S>(a2), static_cast<S>(a3), static_cast<S>(a4),
			static_cast<S>(b1), static_cast<S>(b2), static_cast<S>(b3), static_cast<S>(b4),
			static_cast<S>(c1), static_cast<S>(c2), static_cast<S>(c3), static_cast<S>(c4),
			static_cast<S>(d1), static_cast<S>(d2), static_cast<S>(d3), static_cast<S>(d4));
	}

	explicit Matrix4x4t(const Matrix3x3t<T>& m)
	{
		a1 = m.a1; a2 = m.a2; a3 = m.a3; a4 = static_cast<T>(0.0);
		b1 = m.b1; b2 = m.b2; b3 = m.b3; b4 = static_cast<T>(0.0);
		c1 = m.c1; c2 = m.c2; c3 = m.c3; c4 = static_cast<T>(0.0);
		d1 = static_cast<T>(0.0); d2 = static_cast<T>(0.0); d3 = static_cast<T>(0.0); d4 = static_cast<T>(1.0);
	}

	T& operator[](std::size_t n)
	{
		return *((&a1) + n);
	}

	const T& operator[](std::size_t n) const
	{
		return *((&a1) + n);
	}

	explicit operator T*()
	{
		return this->ptr();
	}

	explicit operator const T*() const
	{
		return this->ptr();
	}

	T* ptr() { return (T*)&a1; }
	const T* ptr() const { return (const T*)&a1; }
	T* data() { return (T*)&a1; }
	const T* data() const { return (const T*)&a1; }

	Matrix4x4t<T>& set(T mt00, T mt01, T mt02, T mt03, T mt10, T mt11, T mt12, T mt13, T mt20, T mt21, T mt22, T mt23, T mt30, T mt31, T mt32, T mt33)
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

	Matrix4x4t<T>& set(const Matrix3x3t<T>& m)
	{
		a1 = m.a1; a2 = m.a2; a3 = m.a3; a4 = 0.0;
		b1 = m.b1; b2 = m.b2; b3 = m.b3; b4 = 0.0;
		c1 = m.c1; c2 = m.c2; c3 = m.c3; c4 = 0.0;
		return *this;
	}

	Matrix4x4t<T>& set(int line, const Vector3t<T>& v)
	{
		((Vector3t<T>&)(*(*this)[line])) = v;
		return *this;
	}

	Matrix4x4t<T>& set(std::size_t line, const Vector3t<T>& v)
	{
		((Vector3t<T>&)(*(*this)[line])) = v;
		return *this;
	}

	Matrix4x4t<T>& loadIdentity()
	{
		set(1, 0, 0, 0,
			0, 1, 0, 0,
			0, 0, 1, 0,
			0, 0, 0, 1);
		return *this;
	}

	Matrix4x4t<T>& transpose()
	{
		std::swap((T&)b1, (T&)a2);
		std::swap((T&)c1, (T&)a3);
		std::swap((T&)c2, (T&)b3);
		std::swap((T&)d1, (T&)a4);
		std::swap((T&)d2, (T&)b4);
		std::swap((T&)d3, (T&)c4);
		return *this;
	}

	T determinant() const
	{
		return
			  a1*b2*c3*d4 - a1*b2*c4*d3 + a1*b3*c4*d2 - a1*b3*c2*d4
			+ a1*b4*c2*d3 - a1*b4*c3*d2 - a2*b3*c4*d1 + a2*b3*c1*d4
			- a2*b4*c1*d3 + a2*b4*c3*d1 - a2*b1*c3*d4 + a2*b1*c4*d3
			+ a3*b4*c1*d2 - a3*b4*c2*d1 + a3*b1*c2*d4 - a3*b1*c4*d2
			+ a3*b2*c4*d1 - a3*b2*c1*d4 - a4*b1*c2*d3 + a4*b1*c3*d2
			- a4*b2*c3*d1 + a4*b2*c1*d3 - a4*b3*c1*d2 + a4*b3*c2*d1;
	}

	Matrix4x4t<T>& inverse() noexcept
	{
		Matrix4x4t<T> m;

		const T det = this->determinant();
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

			return *this;
		}

		const T invdet = static_cast<T>(1.0) / det;

		m.a1 =  invdet * (b2 * (c3 * d4 - c4 * d3) + b3 * (c4 * d2 - c2 * d4) + b4 * (c2 * d3 - c3 * d2));
		m.a2 = -invdet * (a2 * (c3 * d4 - c4 * d3) + a3 * (c4 * d2 - c2 * d4) + a4 * (c2 * d3 - c3 * d2));
		m.a3 =  invdet * (a2 * (b3 * d4 - b4 * d3) + a3 * (b4 * d2 - b2 * d4) + a4 * (b2 * d3 - b3 * d2));
		m.a4 = -invdet * (a2 * (b3 * c4 - b4 * c3) + a3 * (b4 * c2 - b2 * c4) + a4 * (b2 * c3 - b3 * c2));
		m.b1 = -invdet * (b1 * (c3 * d4 - c4 * d3) + b3 * (c4 * d1 - c1 * d4) + b4 * (c1 * d3 - c3 * d1));
		m.b2 =  invdet * (a1 * (c3 * d4 - c4 * d3) + a3 * (c4 * d1 - c1 * d4) + a4 * (c1 * d3 - c3 * d1));
		m.b3 = -invdet * (a1 * (b3 * d4 - b4 * d3) + a3 * (b4 * d1 - b1 * d4) + a4 * (b1 * d3 - b3 * d1));
		m.b4 =  invdet * (a1 * (b3 * c4 - b4 * c3) + a3 * (b4 * c1 - b1 * c4) + a4 * (b1 * c3 - b3 * c1));
		m.c1 =  invdet * (b1 * (c2 * d4 - c4 * d2) + b2 * (c4 * d1 - c1 * d4) + b4 * (c1 * d2 - c2 * d1));
		m.c2 = -invdet * (a1 * (c2 * d4 - c4 * d2) + a2 * (c4 * d1 - c1 * d4) + a4 * (c1 * d2 - c2 * d1));
		m.c3 =  invdet * (a1 * (b2 * d4 - b4 * d2) + a2 * (b4 * d1 - b1 * d4) + a4 * (b1 * d2 - b2 * d1));
		m.c4 = -invdet * (a1 * (b2 * c4 - b4 * c2) + a2 * (b4 * c1 - b1 * c4) + a4 * (b1 * c2 - b2 * c1));
		m.d1 = -invdet * (b1 * (c2 * d3 - c3 * d2) + b2 * (c3 * d1 - c1 * d3) + b3 * (c1 * d2 - c2 * d1));
		m.d2 =  invdet * (a1 * (c2 * d3 - c3 * d2) + a2 * (c3 * d1 - c1 * d3) + a3 * (c1 * d2 - c2 * d1));
		m.d3 = -invdet * (a1 * (b2 * d3 - b3 * d2) + a2 * (b3 * d1 - b1 * d3) + a3 * (b1 * d2 - b2 * d1));
		m.d4 =  invdet * (a1 * (b2 * c3 - b3 * c2) + a2 * (b3 * c1 - b1 * c3) + a3 * (b1 * c2 - b2 * c1));

		*this = m;
		return *this;
	}

	bool isOnlyTranslate() const
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

	void decompose(Vector3t<T>& scaling, Quaterniont<T>& rotation, Vector3t<T>& position) const
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

	void decompose(Quaterniont<T>& rotation, Vector3t<T>& position) const
	{
		const Matrix4x4t<T>& _this = *this;

		// extract translation
		position.x = _this[3][0];
		position.y = _this[3][1];
		position.z = _this[3][2];

		// extract rotation
		rotation = Quaterniont<T>((Matrix3x3t<T>)_this);
	}

	Matrix4x4t<T>& fromEulerAnglesXYZ(T x, T y, T z)
	{
		T cr = cos(x);
		T sr = sin(x);
		T cp = cos(y);
		T sp = sin(y);
		T cy = cos(z);
		T sy = sin(z);

		T srsp = sr*sp;
		T crsp = cr*sp;

		a1 = cp * cy;
		a2 = cp * sy;
		a3 = -sp;

		b1 = srsp * cy - cr * sy;
		b2 = srsp * sy + cr * cy;
		b3 = sr * cp;

		c1 = crsp * cy + sr * sy;
		c2 = crsp * sy - sr * cy;
		c3 = cr * cp;

		return *this;
	}

	Matrix4x4t<T>& fromEulerAnglesXYZ(const Vector3t<T>& blubb)
	{
		return fromEulerAnglesXYZ(blubb.x, blubb.y, blubb.z);
	}

	Matrix4x4t<T>& makeNormalMatrix(const Matrix4x4t<T>& m)
	{
		return inverse(m).transpose();
	}

	Matrix4x4t<T>& applyMatrix(const Matrix4x4t<T>& m)
	{
		Matrix4x4t matrix(*this);
		multiplyMatrices(matrix, m);
		return *this;
	}
	Matrix4x4t<T>& multiplyMatrices(const Matrix4x4t<T>& m1, const Matrix4x4t<T>& m2)
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

	Matrix4x4t<T>& multiplyScalar(T scale)
	{
		a1 *= scale; a2 *= scale; a3 *= scale; a4 *= scale;
		b1 *= scale; b2 *= scale; b3 *= scale; b4 *= scale;
		c1 *= scale; c2 *= scale; c3 *= scale; c4 *= scale;
		d1 *= scale; d2 *= scale; d3 *= scale; d4 *= scale;
		return *this;
	}

	void setTranslate(const Vector2t<T>& pt) { setTranslate(pt.x, pt.y, 0); }
	void setTranslate(const Vector3t<T>& pt) { setTranslate(pt.x, pt.y, pt.z); }
	void setTranslate(const Vector4t<T>& pt) { setTranslate(pt.x, pt.y, pt.z, pt.w); }
	void setTranslate(T x, T y) { setTranslate(x, y); }
	void setTranslate(T x, T y, T z)
	{
		a4 = x;
		b4 = y;
		c4 = z;
	}

	void setTranslate(T x, T y, T z, T w)
	{
		a4 = x;
		b4 = y;
		c4 = z;
		d4 = w;
	}

	Vector3t<T> getTranslate() const
	{
		return Vector3t<T>(a4, b4, c4);
	}

	void translate(const Vector2t<T>& pt) { translate(pt.x, pt.y, 0); }
	void translate(const Vector3t<T>& pt) { translate(pt.x, pt.y, pt.z); }
	void translate(T x, T y) { translate(x, y, 0); }
	void translate(T x, T y, T z)
	{
		a4 += x;
		b4 += y;
		c4 += z;
	}

	Matrix4x4t<T>& makeTranslate(const Vector2t<T>& pt) { return makeTranslate(pt.x, pt.y, 0); }
	Matrix4x4t<T>& makeTranslate(const Vector3t<T>& pt) { return makeTranslate(pt.x, pt.y, pt.z); }
	Matrix4x4t<T>& makeTranslate(T x, T y) { return makeTranslate(x, y, 0); }
	Matrix4x4t<T>& makeTranslate(T x, T y, T z)
	{
		set(1, 0, 0, x,
			0, 1, 0, y,
			0, 0, 1, z,
			0, 0, 0, 1);
		return *this;
	}

	void setScale(const Vector2t<T>& sz) { setScale(sz.x, sz.y, 0); }
	void setScale(const Vector3t<T>& sz) { setScale(sz.x, sz.y, sz.z); }
	void setScale(T cx, T cy) { setScale(cx, cy, 0); }
	void setScale(T x, T y, T z)
	{
		a1 = x;
		b2 = y;
		c3 = z;
	}

	void scale(const Vector2t<T>& sz) { scale(sz.x, sz.y, 0); }
	void scale(const Vector3t<T>& sz) { scale(sz.x, sz.y, sz.z); }
	void scale(T v) { scale(v, v, v); }
	void scale(T cx, T cy) { scale(cx, cy, 0); }
	void scale(T x, T y, T z)
	{
		a1 *= x;
		b1 *= x;
		c1 *= x;
		a2 *= y;
		b2 *= y;
		c2 *= y;
		a3 *= z;
		b3 *= z;
		c3 *= z;
	}

	Matrix4x4t<T>& makeScale(const Vector2t<T>& sz) { return makeScale(sz.x, sz.y, 0); }
	Matrix4x4t<T>& makeScale(const Vector3t<T>& sz) { return makeScale(sz.x, sz.y, sz.z); }
	Matrix4x4t<T>& makeScale(T cx, T cy) { return makeScale(cx, cy, 0); }
	Matrix4x4t<T>& makeScale(T x, T y, T z)
	{
		set(
			x, 0, 0, 0,
			0, y, 0, 0,
			0, 0, z, 0,
			0, 0, 0, 1);
		return *this;
	}

	Matrix4x4t<T>& rotate(T angle, const Vector3t<T>& axis)
	{
		rotate(angle, axis.x, axis.y, axis.z);
		return *this;
	}

	Matrix4x4t<T>& rotate(T angle, T x, T y, T z)
	{
		Matrix4x4t<T> m;
		m.makeRotate(angle, x, y, z);
		applyMatrix(m);
		return *this;
	}

	Matrix4x4t<T>& rotate(const Quaterniont<T>& q)
	{
		Matrix4x4t<T> m;
		m.makeRotate(q);
		applyMatrix(m);
		return *this;
	}

	Matrix4x4t<T>& makeRotate(const Quaterniont<T>& q)
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

	void makeRotate(const EulerAnglest<T>& orientation)
	{
		T sh, ch, sp, cp, sb, cb;

		sinCos(&sh, &ch, orientation.heading);
		sinCos(&sp, &cp, orientation.pitch);
		sinCos(&sb, &cb, orientation.bank);

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
	}

	Matrix4x4t<T>& makeRotate(const Vector3t<T>& axis)
	{
		T a, b, c, d, e, f;

		sinCos(&b, &a, axis.x);
		sinCos(&d, &c, axis.y);
		sinCos(&f, &e, axis.z);

		T ae = a * e;
		T af = a * f;
		T be = b * e;
		T bf = b * f;

		a1 = c * e;
		a2 = -c * f;
		a3 = d;
		a4 = 0;

		b1 = af + be * d;
		b2 = ae - bf * d;
		b3 = -b * c;
		b4 = 0;

		c1 = bf - ae * d;
		c2 = be + af * d;
		c3 = a * c;
		c4 = 0;

		d1 = 0;
		d2 = 0;
		d3 = 0;
		d4 = 1;
		return *this;
	}

	Matrix4x4t<T>& makeRotate(T angle, T x, T y, T z)
	{
		return makeRotate(angle, Vector3t<T>(x, y, z));
	}

	Matrix4x4t<T>& makeRotate(T angle, const Vector3t<T>& axis)
	{
		T c, s;

		sinCos(&s, &c, angle);

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

	Quaterniont<T> getRotate() const
	{
		Quaterniont<T> q;

		T s;
		T tq[4];

		int i, j;
		tq[0] = 1 + a1 + b2 + c3;
		tq[1] = 1 + a1 - b2 - c3;
		tq[2] = 1 - a1 + b2 - c3;
		tq[3] = 1 - a1 - b2 + c3;

		j = 0;
		for (i = 1; i<4; i++) j = (tq[i]>tq[j]) ? i : j;

		if (j == 0)
		{
			q.w = tq[0];
			q.x = b3 - c2;
			q.y = c1 - a3;
			q.z = a2 - b1;
		}
		else if (j == 1)
		{
			q.w = b3 - c2;
			q.x = tq[1];
			q.y = a2 + b1;
			q.z = c1 + a3;
		}
		else if (j == 2)
		{
			q.w = c1 - a3;
			q.x = a2 + b1;
			q.y = tq[2];
			q.z = b3 + c2;
		}
		else
		{
			q.w = a2 - b1;
			q.x = c1 + a3;
			q.y = b3 + c2;
			q.z = tq[3];
		}

		s = sqrt(0.25f / tq[j]);
		q.w *= s;
		q.x *= s;
		q.y *= s;
		q.z *= s;

		return q;
	}

	Matrix4x4t<T>& makeRotationX(T theta)
	{
		T c, s, ang = theta;

		sinCos(&s, &c, ang);

		set(
			1, 0, 0, 0,
			0, c, -s, 0,
			0, s, c, 0,
			0, 0, 0, 1);

		return *this;
	}

	Matrix4x4t<T>& makeRotationY(T theta)
	{
		T c, s, ang = theta;

		sinCos(&s, &c, ang);

		set(
			c, 0, s, 0,
			0, 1, 0, 0,
			-s, 0, c, 0,
			0, 0, 0, 1);

		return *this;
	}

	Matrix4x4t<T>& makeRotationZ(T theta)
	{
		T c, s, ang = theta;

		sinCos(&s, &c, ang);

		set(
			c, -s, 0, 0,
			s, c, 0, 0,
			0, 0, 1, 0,
			0, 0, 0, 1);

		return *this;
	}

	static Matrix4x4t<T>& fromToMatrix(const Vector3t<T>& from, const Vector3t<T>& to, Matrix4x4t& out)
	{
		Matrix3x3t<T> m3;
		Matrix3x3t<T>::fromToMatrix(from, to, m3);
		out = Matrix4x4t<T>(m3);
		return out;
	}

	Matrix4x4t<T>& makeTransform(const Vector3t<T>& translate, const Quaterniont<T>& rotate, const Vector3t<T>& scale)
	{
		this->makeRotate(rotate);
		this->setTranslate(translate);
		this->scale(scale);
		return *this;
	}

	Matrix4x4t<T>& makeOrtho_lh(T width, T height, T zNear, T zFar)
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

	Matrix4x4t<T>& makeOrtho_lh(T left, T right, T bottom, T top, T zNear, T zFar)
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

	Matrix4x4t<T>& makeOrtho_rh(T left, T right, T bottom, T top, T zNear, T zFar)
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

	bool getOrtho_rh(T& left, T& right, T& bottom, T& top, T& zNear, T& zFar) const
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

	Matrix4x4t<T>& makeFrustumt_lh(T left, T right, T bottom, T top, T zNear, T zFar)
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

	Matrix4x4t<T>& makeFrustumt_rh(T left, T right, T bottom, T top, T zNear, T zFar)
	{
		T A = (right + left) / (right - left);
		T B = (top + bottom) / (top - bottom);
		T C = (zFar > DBL_MAX) ? 1.0f : (zFar + zNear) / (zFar - zNear);
		T D = (zFar > DBL_MAX) ? zNear : 2.0 * zNear * zFar / (zFar - zNear);
		T cx = 2.0f * zNear / (right - left);
		T cy = 2.0f * zNear / (top - bottom);

		set(cx, 0.0, A, 0.0,
			0.0, cy, B, 0.0,
			0.0, 0.0, C, D,
			0.0, 0.0, -1.0, 0.0);

		return *this;
	}

	bool getFrustumt_rh(T& left, T& right, T& bottom, T& top, T& zNear, T& zFar) const
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

	Matrix4x4t<T>& makePerspective_lh(T fovy, T aspectRatio, T zNear, T zFar)
	{
		T tan_fovy = tan(degrees(fovy * 0.5f));
		T right = tan_fovy * aspectRatio * zNear;
		T left = -right;
		T top = tan_fovy * zNear;
		T bottom = -top;

		makeFrustumt_lh(left, right, bottom, top, zNear, zFar);
		return *this;
	}

	Matrix4x4t<T>& makePerspective_rh(T fovy, T aspectRatio, T zNear, T zFar)
	{
		T tan_fovy = tan(degrees(fovy * 0.5f));
		T right = tan_fovy * aspectRatio * zNear;
		T left = -right;
		T top = tan_fovy * zNear;
		T bottom = -top;

		makeFrustumt_rh(left, right, bottom, top, zNear, zFar);
		return *this;
	}

	bool getPerspective_rh(T& fovy, T& aspectRatio, T& zNear, T& zFar) const
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

	Matrix4x4t<T>& makeLookAt_lh(const Vector3t<T>& eye, const Vector3t<T>& center, const Vector3t<T>& up)
	{
		Vector3t<T> x, y, z;

		z = center - eye;
		z.normalize();

		x = up.cross(z);
		x.normalize();

		y = z.cross(x);
		y.normalize();

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

	Matrix4x4t<T>& makeLookAt_rh(const Vector3t<T>& eye, const Vector3t<T>& center, const Vector3t<T>& up)
	{
		Vector3t<T> x, y, z;

		z = eye - center;
		z.normalize();

		x = up.cross(z);
		x.normalize();

		y = z.cross(x);
		y.normalize();

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

	Matrix4x4t<T>& makeViewport(std::size_t left, std::size_t top, std::size_t width, std::size_t height)
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

	void fromInertialToObjectQuaterniont(const Quaterniont<T>& q)
	{
		a1 = 1.0f - 2.0f * (q.y * q.y + q.z * q.z);
		a2 = 2.0f * (q.x * q.y + q.w * q.z);
		a3 = 2.0f * (q.x * q.z - q.w * q.y);

		b1 = 2.0f * (q.x * q.y - q.w * q.z);
		b2 = 1.0f - 2.0f * (q.x * q.x + q.z * q.z);
		b3 = 2.0f * (q.y * q.z + q.w * q.x);

		c1 = 2.0f * (q.x * q.z + q.w * q.y);
		c2 = 2.0f * (q.y * q.z - q.w * q.x);
		c3 = 1.0f - 2.0f * (q.x * q.x + q.y * q.y);
	}

	void fromObjectToInertialQuaterniont(const Quaterniont<T>& q)
	{
		a1 = 1.0f - 2.0f * (q.y * q.y + q.z * q.z);
		a2 = 2.0f * (q.x * q.y - q.w * q.z);
		a3 = 2.0f * (q.x * q.z + q.w * q.y);

		b1 = 2.0f * (q.x * q.y + q.w * q.z);
		b2 = 1.0f - 2.0f * (q.x * q.x + q.z * q.z);
		b3 = 2.0f * (q.y * q.z - q.w * q.x);

		c1 = 2.0f * (q.x * q.z - q.w * q.y);
		c2 = 2.0f * (q.y * q.z + q.w * q.x);
		c3 = 1.0f - 2.0f * (q.x * q.x + q.y * q.y);
	}

	Vector3t<T> inertialToObject(const Vector3t<T>& v) const
	{
		return Vector3t<T>(
			a1 * v.x + b1 * v.y + c1 * v.z,
			a2 * v.x + b2 * v.y + c2 * v.z,
			a3 * v.x + b3 * v.y + c3 * v.z);
	}

	Vector3t<T> objectToInertial(const Vector3t<T>& v) const
	{
		return Vector3t<T>(
			a1 * v.x + a2 * v.y + a3 * v.z,
			b1 * v.x + b2 * v.y + b3 * v.z,
			c1 * v.x + c2 * v.y + c3 * v.z);
	}

	T a1, a2, a3, a4;
	T b1, b2, b3, b4;
	T c1, c2, c3, c4;
	T d1, d2, d3, d4;
};

template<typename T> const Matrix4x4t<T> Matrix4x4t<T>::Zero(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
template<typename T> const Matrix4x4t<T> Matrix4x4t<T>::One(1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1);

template <typename T>
inline bool operator==(const Matrix4x4t<T>& m1, const Matrix4x4t<T>& m2)
{
	return
		m1.a1 == m2.a1 && m1.a2 == m2.a2 && m1.a3 == m2.a3 && m1.a4 == m2.a4 &&
		m1.b1 == m2.b1 && m1.b2 == m2.b2 && m1.b3 == m2.b3 && m1.b4 == m2.b4 &&
		m1.c1 == m2.c1 && m1.c2 == m2.c2 && m1.c3 == m2.c3 && m1.c4 == m2.c4 &&
		m1.d1 == m2.d1 && m1.d2 == m2.d2 && m1.d3 == m2.d3 && m1.d4 == m2.c4;
}

template <typename T>
inline bool operator!=(const Matrix4x4t<T>& m1, const Matrix4x4t<T>& m2)
{
	return
		m1.a1 != m2.a1 || m1.a2 != m2.a2 || m1.a3 != m2.a3 || m1.a4 != m2.a4 ||
		m1.b1 != m2.b1 || m1.b2 != m2.b2 || m1.b3 != m2.b3 || m1.b4 != m2.b4 ||
		m1.c1 != m2.c1 || m1.c2 != m2.c2 || m1.c3 != m2.c3 || m1.c4 != m2.c4 ||
		m1.d1 != m2.d1 || m1.d2 != m2.d2 || m1.d3 != m2.d3 || m1.d4 != m2.c4;
}

template<typename T>
inline Matrix4x4t<T> operator*(const Matrix4x4t<T>& m1, const Matrix4x4t<T>& m2)
{
	return Matrix4x4t<T>(m1, m2);
}

template<typename T>
inline Vector3t<T> operator*(const Vector3t<T>& v, const Matrix4x4t<T>& m)
{
	return Vector3t<T>(
		m.a1 * v.x + m.a2 * v.y + m.a3 * v.z + m.a4,
		m.b1 * v.x + m.b2 * v.y + m.b3 * v.z + m.b4,
		m.c1 * v.x + m.c2 * v.y + m.c3 * v.z + m.c4);
}

template<typename T>
inline Vector3t<T> operator*(const Matrix4x4t<T>& m, const Vector3t<T>& v)
{
	return Vector3t<T>(
		m.a1 * v.x + m.b1 * v.y + m.c1 * v.z + m.d1,
		m.a2 * v.x + m.b2 * v.y + m.c2 * v.z + m.d2,
		m.a3 * v.x + m.b3 * v.y + m.c3 * v.z + m.d3);
}

template<typename T>
inline Vector3t<T>& operator*=(const Matrix4x4t<T>& m, Vector3t<T>& v)
{
	v = m * v;
	return v;
}

template<typename T>
inline Vector3t<T>& operator*=(Vector3t<T>& v, const Matrix4x4t<T>& m)
{
	v = v * m;
	return v;
}

template<typename T>
inline Vector4t<T> operator*(const Matrix4x4t<T>& m, const Vector4t<T>& v)
{
	return Vector4t<T>(
		m.a1 * v.x + m.b1 * v.y + m.c1 * v.z + m.d1 * v.w,
		m.a2 * v.x + m.b2 * v.y + m.c2 * v.z + m.d2 * v.w,
		m.a3 * v.x + m.b3 * v.y + m.c3 * v.z + m.d3 * v.w,
		m.a4 * v.x + m.b4 * v.y + m.c4 * v.z + m.d4 * v.w);
}

template<typename T>
inline Vector4t<T> operator*(const Vector4t<T>& v, const Matrix4x4t<T>& m)
{
	return Vector4t<T>(
		m.a1 * v.x + m.a2 * v.y + m.a3 * v.z + v.w * m.a4,
		m.b1 * v.x + m.b2 * v.y + m.b3 * v.z + v.w * m.b4,
		m.c1 * v.x + m.c2 * v.y + m.c3 * v.z + v.w * m.c4,
		m.d1 * v.x + m.d2 * v.y + m.d3 * v.z + v.w * m.d4);
}

template<typename T>
inline Vector4t<T>& operator*=(Vector4t<T>& v, const Matrix4x4t<T>& m)
{
	v = v * m;
	return v;
}

template<typename T>
inline Matrix4x4t<T>& operator*=(Matrix4x4t<T> m1, const Matrix4x4t<T>& m2)
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

_NAME_END

#endif // AI_MATRIX4X4_H_INC
