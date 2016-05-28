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

	static const Quaterniont<T> Zero;

	Quaterniont() noexcept {}
	Quaterniont(T x, T y, T z, T w) noexcept : x(x), y(y), z(z), w(w) {}
	Quaterniont(const Quaterniont& q) noexcept : x(q.x), y(q.y), z(q.z), w(q.w) {}
	Quaterniont(const Vector3t<T>& axis, T angle) noexcept { this->makeRotate(axis, angle); }
	Quaterniont(const Vector3t<T>& forward, const Vector3t<T>& up, const Vector3t<T>& right) noexcept { this->makeRotate(forward, up, right); }

	explicit Quaterniont(const Vector3t<T>& eulerXYZ) noexcept { this->makeRotate(eulerXYZ); }
	explicit Quaterniont(const EulerAnglest<T>& eulerXYZ) noexcept { this->makeRotate(eulerXYZ); }

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

		const T t = T(1.0) - (x*x) - (y*y) - (z*z);
		if (t < T(0.0))
			w = T(0.0);
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

	Quaterniont<T>& makeRotateAboutX(T theta) noexcept
	{
		T thetaOver2 = DEG_TO_RAD(theta * 0.5f);

		w = cos(thetaOver2);
		x = sin(thetaOver2);
		y = 0.0f;
		z = 0.0f;
		return *this;
	}

	Quaterniont<T>& makeRotateAboutY(T theta) noexcept
	{
		T thetaOver2 = DEG_TO_RAD(theta * 0.5f);

		w = cos(thetaOver2);
		x = 0.0f;
		y = sin(thetaOver2);
		z = 0.0f;
		return *this;
	}

	Quaterniont<T>& makeRotateAboutZ(T theta) noexcept
	{
		T thetaOver2 = DEG_TO_RAD(theta * 0.5f);

		w = cos(thetaOver2);
		x = 0.0f;
		y = 0.0f;
		z = sin(thetaOver2);
		return *this;
	}

	Quaterniont<T>& makeRotate(const Vector3t<T>& axis, T theta) noexcept
	{
		T thetaOver2 = DEG_TO_RAD(theta * 0.5f);

		const T sin_a = sin(thetaOver2);
		const T cos_a = cos(thetaOver2);

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

		math::sinCos(&sp, &cp, DEG_TO_RAD(euler.x) * 0.5f);
		math::sinCos(&sh, &ch, DEG_TO_RAD(euler.y) * 0.5f);
		math::sinCos(&sb, &cb, DEG_TO_RAD(euler.z) * 0.5f);

		x = sp * ch * cb - cp * sh * sb;
		y = cp * sh * cb + sp * ch * sb;
		z = cp * ch * sb - sp * sh * cb;
		w = cp * ch * cb + sp * sh * sb;
		return *this;
	}

	Quaterniont<T>& makeRotate(const Vector3t<T>& euler) noexcept
	{
		return this->makeRotate(EulerAnglest<T>(euler));
	}

	Quaterniont<T>& makeRotate(const Vector3t<T>& forward, const Vector3t<T>& up, const Vector3t<T>& right) noexcept
	{
		T t = 1 + right.x + up.y + forward.z;

		if (t > 0.001f)
		{
			T s = std::sqrt(t) * 2.0f;
			this->x = (up.z - forward.y) / s;
			this->y = (forward.x - right.z) / s;
			this->z = (right.y - up.x) / s;
			this->w = 0.25f * s;
		}
		else if (right.x > up.y && right.x > forward.z)
		{
			T s = std::sqrt(1.0f + right.x - up.y - forward.z) * 2.0f;
			this->x = 0.25f * s;
			this->y = (right.y + up.x) / s;
			this->z = (forward.x + right.z) / s;
			this->w = (up.z - forward.y) / s;
		}
		else if (up.y > forward.z)
		{
			T s = std::sqrt(1.0f + up.y - right.x - forward.z) * 2.0f;
			this->x = (right.y + up.x) / s;
			this->y = 0.25f * s;
			this->z = (up.z + forward.y) / s;
			this->w = (forward.x - right.z) / s;
		}
		else
		{
			T s = std::sqrt(1.0f + forward.z - right.x - up.y) * 2.0f;
			this->x = (forward.x + right.z) / s;
			this->y = (up.z + forward.y) / s;
			this->z = 0.25f * s;
			this->w = (right.y - up.x) / s;
		}

		return *this;
	}

	T getRotationAngle() const noexcept
	{
		return safeAcos(w) * T(2.0);
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
};

template<typename T> const Quaterniont<T> Quaterniont<T>::Zero = Quaterniont<T>((T)0.0, (T)0.0, (T)0.0, (T)1.0);

template<typename T>
inline bool operator==(const Quaterniont<T>& q1, const Quaterniont<T>& q2) noexcept
{
	return q1.x == q2.x && q1.y == q2.y && q1.z == q2.z && q1.w == q2.w;
}

template<typename T>
inline bool operator!=(const Quaterniont<T>& q1, const Quaterniont<T>& q2) noexcept
{
	return !(q1 == q2);
}

template<typename T>
inline Quaterniont<T> operator-(const Quaterniont<T>& q) noexcept
{
	return Quaterniont<T>(-q.x, -q.y, -q.z, -q.w);
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

template<typename ostream, typename T, std::enable_if_t<trait::has_left_shift<ostream, T>::value, int> = 0>
inline ostream& operator << (ostream& os, const Quaterniont<T>& v)
{
	os << v.x << ", " << v.y << ", " << v.z << ", " << v.w;
	return os;
}

template<typename istream, typename T, std::enable_if_t<trait::has_right_shift<istream>::value, int> = 0>
inline istream& operator >> (istream& is, Quaterniont<T>& v)
{
	is >> v.x;
	is.ignore(2);
	is >> v.y;
	is.ignore(2);
	is >> v.z;
	is.ignore(2);
	is >> v.w;
	return is;
}

namespace math
{
	template<typename T>
	inline Quaterniont<T> conjugate(const Quaterniont<T>& q) noexcept
	{
		return Quaterniont<T>(-q.x, -q.y, -q.z, q.w);
	}

	template<typename T>
	inline Quaterniont<T> inverse(const Quaterniont<T>& q) noexcept
	{
		return conjugate(q);
	}

	template<typename T>
	inline Quaterniont<T> pow(const Quaterniont<T>& q, T exponent) noexcept
	{
		if (std::fabs(q.w) > 0.9999f)
			return *this;

		T alpha = std::acos(q.w);
		T newAlpha = alpha * exponent;
		T mult = std::sin(newAlpha) / std::sin(alpha);

		Quaterniont<T> result;
		result.x = q.x * mult;
		result.y = q.y * mult;
		result.z = q.z * mult;
		result.w = std::cos(newAlpha);
		return *this;
	}

	template<typename T>
	inline Quaterniont<T> slerp(const Quaterniont<T>& q1, const Quaterniont<T>& q2, T t) noexcept
	{
		if (t <= 0.0f) { return q1; }
		if (t >= 1.0f) { return q2; }

		T cosOmega = clamp(dot(q1, q2), -1.0f, 1.0f);

		if (cosOmega < 0.0f)
		{
			double theta = std::acos(-cosOmega);
			if (std::fabs(theta) < 1.0e-10)
			{
				return q1;
			}

			T st = std::sin(theta);
			T inv_st = 1.0 / st;
			T c0 = std::sin((1.0 - t) * theta) * inv_st;
			T c1 = std::sin(t * theta) * inv_st;

			Quaterniont<T> result;
			result.x = c0 * q1.x - c1 * q2.x;
			result.y = c0 * q1.y - c1 * q2.y;
			result.z = c0 * q1.z - c1 * q2.z;
			result.w = c0 * q1.w - c1 * q2.w;
			return result;
		}
		else
		{
			double theta = std::acos(cosOmega);
			if (fabs(theta) < 1.0e-10)
			{
				return q1;
			}

			T st = std::sin(theta);
			T inv_st = 1.0 / st;
			T c0 = std::sin((1.0 - t) * theta) * inv_st;
			T c1 = std::sin(t * theta) * inv_st;

			Quaterniont<T> result;
			result.x = c0 * q1.x + c1 * q2.x;
			result.y = c0 * q1.y + c1 * q2.y;
			result.z = c0 * q1.z + c1 * q2.z;
			result.w = c0 * q1.w + c1 * q2.w;
			return result;
		}
	}

	template<typename T>
	inline Quaterniont<T> interpolate(const Quaterniont<T>& start, const Quaterniont<T>& end, T factor) noexcept
	{
		T cosom = start.x * end.x + start.y * end.y + start.z * end.z + start.w * end.w;

		Quaterniont<T> end = end;
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

		Quaterniont<T> result;
		result.x = sclp * start.x + sclq * end.x;
		result.y = sclp * start.y + sclq * end.y;
		result.z = sclp * start.z + sclq * end.z;
		result.w = sclp * start.w + sclq * end.w;
		return result;
	}

	template<typename T>
	inline Quaterniont<T> cross(const Quaterniont<T>& q1, const Quaterniont<T>& q2) noexcept
	{
		return Quaterniont<T>(
			q1.w * q2.x + q1.x * q2.w + q1.y * q2.z - q1.z * q2.y,
			q1.w * q2.y + q1.y * q2.w + q1.z * q2.x - q1.x * q2.z,
			q1.w * q2.z + q1.z * q2.w + q1.x * q2.y - q1.y * q2.x,
			q1.w * q2.w - q1.x * q2.x - q1.y * q2.y - q1.z * q2.z);
	}

	template<typename T>
	inline T dot(const Quaterniont<T>& q1, const Quaterniont<T>& q2) noexcept
	{
		return q1.x * q2.x + q1.y * q2.y + q1.z * q2.z + q1.w * q2.w;
	}

	template<typename T>
	inline T length2(const Quaterniont<T>& q) noexcept
	{
		return q.x * q.x + q.y * q.y + q.z * q.z + q.w * q.w;
	}

	template<typename T>
	inline T length(const Quaterniont<T>& q) noexcept
	{
		return std::sqrt(length2(q));
	}

	template<typename T>
	inline bool equal(const Quaterniont<T>& q1, const Quaterniont<T>& q2) noexcept
	{
		return
			math::equal<T>(q1.x, q2.x) &&
			math::equal<T>(q1.y, q2.y) &&
			math::equal<T>(q1.z, q2.z) &&
			math::equal<T>(q1.w, q2.w);
	}

	template<typename T>
	inline Quaterniont<T> normalize(const Quaterniont<T>& q) noexcept
	{
		Quaterniont<T> result = q;

		const T magSqrt = length2(q);
		if (magSqrt)
		{
			const T inv = (T)(1.0) / std::sqrt(magSqrt);
			result.x *= inv;
			result.y *= inv;
			result.z *= inv;
			result.w *= inv;
		}

		return result;
	}

	template<typename T>
	inline Vector3t<T> rotate(const Quaterniont<T>& q, const Vector3t<T>& v) noexcept
	{
		Quaterniont<T> q1(v.x, v.y, v.z, 0.f);
		Quaterniont<T> qinv = math::inverse(q);
		Quaterniont<T> q2 = cross(q, math::cross(q1, qinv));
		return Vector3t<T>(q2.x, q2.y, q2.z);
	}
}

_NAME_END

#endif