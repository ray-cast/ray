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
#ifndef _H_QUATERNION_H_
#define _H_QUATERNION_H_

#include <ray/mathfwd.h>

_NAME_BEGIN

template <typename T>
class Quaterniont
{
public:
	T x, y, z, w;

	Quaterniont() noexcept {}
	Quaterniont(T x, T y, T z, T w) noexcept : x(x), y(y), z(z), w(w) {}
	Quaterniont(const Quaterniont& q) noexcept : x(q.x), y(q.y), z(q.z), w(q.w) {}
	Quaterniont(const Vector3t<T>& axis, T angle) noexcept { this->makeRotate(axis, angle); }

	explicit Quaterniont(const Vector3t<T>& euler) noexcept { this->makeRotate(euler); }
	explicit Quaterniont(const EulerAnglest<T>& euler) noexcept { this->makeRotate(euler); }
	explicit Quaterniont(const Matrix3x3t<T>& rotate) noexcept { this->makeRotate(rotate); }
	explicit Quaterniont(const Matrix4x4t<T>& rotate) noexcept { this->makeRotate(rotate); }

	Quaterniont& operator+=(T f) noexcept { w += w; x += f; y += f; z += f; return *this; }
	Quaterniont& operator-=(T f) noexcept { w -= w; x -= f; y -= f; z -= f; return *this; }
	Quaterniont& operator*=(T f) noexcept { w *= w; x *= f; y *= f; z *= f; return *this; }
	Quaterniont& operator/=(T f) noexcept { w /= w; x /= f; y /= f; z /= f; return *this; }
	Quaterniont& operator+=(const Quaterniont& v) noexcept { w += v.w; x += v.x; y += v.y; z += v.z; return *this; }
	Quaterniont& operator-=(const Quaterniont& v) noexcept { w -= v.w; x -= v.x; y -= v.y; z -= v.z; return *this; }
	Quaterniont& operator*=(const Quaterniont& v) noexcept { w *= v.w; x *= v.x; y *= v.y; z *= v.z; return *this; }
	Quaterniont& operator/=(const Quaterniont& v) noexcept { w /= v.w; x /= v.x; y /= v.y; z /= v.z; return *this; }

	Quaterniont<T>& identity() noexcept
	{
		x = y = z = 0.0f; w = 1.0f;
		return *this;
	}

	Quaterniont<T>& set(T xx, T yy, T zz) noexcept
	{
		x = xx;
		y = yy;
		z = zz;
		w = 1.0;
		
		this->normalize();

		const T t = static_cast<T>(1.0) - (x*x) - (y*y) - (z*z);
		if (t < static_cast<T>(0.0))
			w = static_cast<T>(0.0);
		else
			w = sqrt(t);
		return *this;
	}

	Quaterniont<T>& set(T xx, T yy, T zz, T ww) noexcept
	{
		x = xx;
		y = yy;
		z = zz;
		w = ww;
		return *this;
	}

	Quaterniont<T>& set(const Vector3t<T>& v) noexcept
	{
		return this->set(v.x, v.y, v.z);
	}

	Quaterniont<T>& set(const Vector4t<T>& v) noexcept
	{
		return this->set(v.x, v.y, v.z, v.w);
	}

	Quaterniont& conjugate() noexcept
	{
		x = -x;
		y = -y;
		z = -z;
		return *this;
	}

	Quaterniont& inverse() noexcept
	{
		return this->conjugate();
	}

	Quaterniont& pow(float exponent) noexcept
	{
		if (fabs(this->w) > 0.9999f)
			return *this;

		T alpha = acos(this->w);
		T newAlpha = alpha * exponent;
		T mult = sin(newAlpha) / sin(alpha);

		x *= mult;
		y *= mult;
		z *= mult;
		w = cos(newAlpha);
		return *this;
	}

	Quaterniont<T>& slerp(const Quaterniont& q1, const Quaterniont& q2, T t) noexcept
	{
		if (t <= 0.0f) { *this = q1; return *this; }
		if (t >= 1.0f) { *this = q2; return *this; }

		T cosOmega = clamp<T>(q1.dot(q2), -1.0f, 1.0f);

		if (cosOmega < 0.0f)
		{
			double theta = acos(-cosOmega);
			if (fabs(theta) < 1.0e-10)
			{
				*this = q1;
				return *this;
			}

			double st = sin(theta);
			double inv_st = 1.0 / st;
			double c0 = sin((1.0 - t) * theta) * inv_st;
			double c1 = sin(t * theta) * inv_st;

			this->x = c0 * q1.x - c1 * q2.x;
			this->y = c0 * q1.y - c1 * q2.y;
			this->z = c0 * q1.z - c1 * q2.z;
			this->w = c0 * q1.w - c1 * q2.w;
		}
		else
		{
			double theta = acos(cosOmega);
			if (fabs(theta) < 1.0e-10)
			{
				*this = q1;
				return *this;
			}

			double st = sin(theta);
			double inv_st = 1.0 / st;
			double c0 = sin((1.0 - t) * theta) * inv_st;
			double c1 = sin(t * theta) * inv_st;

			this->x = c0 * q1.x + c1 * q2.x;
			this->y = c0 * q1.y + c1 * q2.y;
			this->z = c0 * q1.z + c1 * q2.z;
			this->w = c0 * q1.w + c1 * q2.w;
		}

		return *this;
	}

	Quaterniont<T>& interpolate(const Quaterniont& start, const Quaterniont& end, T factor) noexcept
	{
		T cosom = start.x * end.x + start.y * end.y + start.z * end.z + start.w * end.w;

		Quaterniont end = end;
		if (cosom <static_cast<T>(0.0))
		{
			cosom = -cosom;
			end.x = -end.x;
			end.y = -end.y;
			end.z = -end.z;
			end.w = -end.w;
		}

		T sclp, sclq;
		if ((static_cast<T>(1.0) - cosom) > static_cast<T>(0.0001)) // 0.0001 -> some epsillon
		{
			T omega, sinom;
			omega = acos(cosom);
			sinom = sin(omega);
			sclp = sin((static_cast<T>(1.0) - factor) * omega) / sinom;
			sclq = sin(factor * omega) / sinom;
		}
		else
		{
			sclp = static_cast<T>(1.0) - factor;
			sclq = factor;
		}

		x = sclp * start.x + sclq * end.x;
		y = sclp * start.y + sclq * end.y;
		z = sclp * start.z + sclq * end.z;
		w = sclp * start.w + sclq * end.w;
		return *this;
	}

	Quaterniont<T>& makeRotateAboutX(T theta) noexcept
	{
		T thetaOver2 = theta * 0.5f;

		w = cos(thetaOver2);
		x = sin(thetaOver2);
		y = 0.0f;
		z = 0.0f;
		return *this;
	}

	Quaterniont<T>& makeRotateAboutY(T theta) noexcept
	{
		T thetaOver2 = theta * 0.5f;

		w = cos(thetaOver2);
		x = 0.0f;
		y = sin(thetaOver2);
		z = 0.0f;
		return *this;
	}

	Quaterniont<T>& makeRotateAboutZ(T theta) noexcept
	{
		T thetaOver2 = theta * 0.5f;

		w = cos(thetaOver2);
		x = 0.0f;
		y = 0.0f;
		z = sin(thetaOver2);

		return *this;
	}

	Quaterniont<T>& makeRotate(const Vector3t<T>& axis, T angle) noexcept
	{
		assert(axis.x <= 1.0 && axis.y <= 1.0 && axis.z <= 1.0);

		const T sin_a = sin(angle / 2);
		const T cos_a = cos(angle / 2);
		x = axis.x * sin_a;
		y = axis.y * sin_a;
		z = axis.z * sin_a;
		w = cos_a;
		return *this;
	}

	Quaterniont<T>& makeRotate(const EulerAnglest<T>& euler) noexcept
	{
		T sp, sb, sh;
		T cp, cb, ch;

		sinCos(&sp, &cp, euler.x * 0.5f);
		sinCos(&sh, &ch, euler.y * 0.5f);
		sinCos(&sb, &cb, euler.z * 0.5f);

		x = ch * sp * cb + sh * cp * sb;
		y = sh * cp * cb - ch * sp * sb;
		z = ch * cp * sb - sh * sp * cb;
		w = ch * cp * cb + sh * sp * sb;
		return *this;
	}

	Quaterniont<T>& makeRotate(const Vector3t<T>& euler) noexcept
	{
		return this->makeRotate(EulerAnglest<T>(euler));
	}

	Quaterniont<T>& makeRotate(const Matrix3x3t<T>& rotate) noexcept
	{
		T t = 1 + rotate.a1 + rotate.b2 + rotate.c3;

		if (t > static_cast<T>(0.001))
		{
			T s = sqrt(t) * static_cast<T>(2.0);
			x = (rotate.c2 - rotate.b3) / s;
			y = (rotate.a3 - rotate.c1) / s;
			z = (rotate.b1 - rotate.a2) / s;
			w = static_cast<T>(0.25) * s;
		}
		else if (rotate.a1 > rotate.b2 && rotate.a1 > rotate.c3)
		{
			T s = sqrt(static_cast<T>(1.0) + rotate.a1 - rotate.b2 - rotate.c3) * static_cast<T>(2.0);
			x = static_cast<T>(0.25) * s;
			y = (rotate.b1 + rotate.a2) / s;
			z = (rotate.a3 + rotate.c1) / s;
			w = (rotate.c2 - rotate.b3) / s;
		}
		else if (rotate.b2 > rotate.c3)
		{
			T s = sqrt(static_cast<T>(1.0) + rotate.b2 - rotate.a1 - rotate.c3) * static_cast<T>(2.0);
			x = (rotate.b1 + rotate.a2) / s;
			y = static_cast<T>(0.25) * s;
			z = (rotate.c2 + rotate.b3) / s;
			w = (rotate.a3 - rotate.c1) / s;
		}
		else
		{
			T s = sqrt(static_cast<T>(1.0) + rotate.c3 - rotate.a1 - rotate.b2) * static_cast<T>(2.0);
			x = (rotate.a3 + rotate.c1) / s;
			y = (rotate.c2 + rotate.b3) / s;
			z = static_cast<T>(0.25) * s;
			w = (rotate.b1 - rotate.a2) / s;
		}

		return *this;
	}

	Quaterniont<T>& makeRotate(const Matrix4x4t<T>& m) noexcept
	{
		return makeRotate(Matrix3x3t<T>(m));
	}

	Quaterniont<T> cross(const Quaterniont<T>& q) const noexcept
	{
		return Quaterniont<T>(
			w * q.x + x * q.w + y * q.z - z * q.y,
			w * q.y + y * q.w + z * q.x - x * q.z,
			w * q.z + z * q.w + x * q.y - y * q.x,
			w * q.w - x * q.x - y * q.y - z * q.z);
	}

	T length() const noexcept
	{
		return sqrt(length2());
	}

	T length2() const noexcept
	{
		return x * x + y * y + z * z + w * w;
	}

	T dot(const Quaterniont<T>& q) const noexcept
	{
		return x * q.x + y * q.y + z * q.z + w * q.w;
	}

	T normalize() noexcept
	{
		const T magSqrt = this->length2();
		if (magSqrt)
		{
			const T inv = (T)(1.0) / std::sqrt(magSqrt);
			x *= inv;
			y *= inv;
			z *= inv;
			w *= inv;
		}
		return magSqrt;
	}

	T getRotationAngle() const noexcept
	{
		return safeAcos(w) * 2.0f;
	}

	Vector3t<T> getRotationAxis() const noexcept
	{
		T sinThetaOver2Sq = 1.0f - w * w;

		if (sinThetaOver2Sq <= 0.0f)
		{
			return Vector3t<T>(1.0f, 0.0f, 0.0f);
		}

		T oneOverSinThetaOver2 = 1.0f / sqrt(sinThetaOver2Sq);

		return Vector3t<T>(x * oneOverSinThetaOver2, y * oneOverSinThetaOver2, z * oneOverSinThetaOver2);
	}

	Vector3t<T> rotate(const Vector3t<T>& v) const noexcept
	{
		Quaterniont q2(v.x, v.y, v.z, 0.f);
		Quaterniont q = *this, qinv = *this;

		qinv.inverse();
		q = q.cross(q2.cross(qinv));
		return Vector3t<T>(q.x, q.y, q.z);
	}
};

template<typename T>
inline bool operator==(const Quaterniont<T>& q1, const Quaterniont<T>& q2) noexcept
{
	return q1.x == q2.x && q1.y == q2.y && q1.z == q2.z && q1.w == q2.w;
}

template<typename T>
inline bool operator!=(const Quaterniont<T>& q1, const Quaterniont<T>& q2) noexcept
{
	return q1.x != q2.x || q1.y != q2.y || q1.z != q2.z || q1.w != q2.w;
}

template <typename T>
inline Quaterniont<T> operator-(const Quaterniont<T>& q) noexcept
{
	return Quaterniont<T>(q).conjugate();
}

template <typename T>
inline Quaterniont<T> min(const Quaterniont<T>& a, const Quaterniont<T>& b) noexcept
{
	return Quaterniont<T>(std::min(a.w, b.w), std::min(a.x, b.x), std::min(a.y, b.y), std::min(a.z, b.z));
}

template <typename T>
inline Quaterniont<T> max(const Quaterniont<T>& a, const Quaterniont<T>& b) noexcept
{
	return Quaterniont<T>(std::max(a.w, b.w), std::max(a.x, b.x), std::max(a.y, b.y), std::max(a.z, b.z));
}

template<typename T>
inline Quaterniont<T> operator+(const Quaterniont<T>& a, const Quaterniont<T>& b) noexcept
{
	return Quaterniont<T>(a.x + b.x, a.y + b.y, a.z + b.z, a.w + b.w);
}

template<typename T>
inline Quaterniont<T> operator/(const Quaterniont<T>& q, T f) noexcept
{
	return Quaterniont<T>(q.x / f, q.y / f, q.z / f, q.w / f);
}

template<typename T>
inline Quaterniont<T> operator/(T f, const Quaterniont<T>& q) noexcept
{
	return Quaterniont<T>(q.x / f, q.y / f, q.z / f, q.w / f);
}

template<typename T>
inline Quaterniont<T> operator^(const Quaterniont<T>& q1, const Quaterniont<T>& q2) noexcept
{
	return q1.cross(q2);
}

_NAME_END

#endif