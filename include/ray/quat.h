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

	static const Quaterniont Zero;
	static const Quaterniont One;
	static const Quaterniont UnitX;
	static const Quaterniont UnitY;
	static const Quaterniont UnitZ;

	T x, y, z, w;

	Quaterniont() noexcept {}
	Quaterniont(T x, T y, T z, T w) noexcept : x(x), y(y), z(z), w(w) {}

	Quaterniont(T rotx, T roty, T rotz) noexcept
	{
		this->makeRotate(rotx, roty, rotz);
	}

	Quaterniont(const Vector3t<T>& rotate) noexcept
	{
		this->makeRotate(rotate);
	}

	Quaterniont(const Vector3t<T>& axis, T angle) noexcept
	{
		this->makeRotate(axis, angle);
	}

	Quaterniont(const Matrix3x3t<T>& rotate) noexcept
	{
		this->makeRotate(rotate);
	}

	Quaterniont operator-() const noexcept { return this->conjugate(); }
	Quaterniont& operator*=(T f) noexcept { w *= w; x *= f; y *= f; z *= f; return *this; }
	Quaterniont& operator/=(T f) noexcept { w /= w; x /= f; y /= f; z /= f; return *this; }
	Quaterniont& operator+=(const Quaterniont& v) noexcept { w += v.w; x += v.x; y += v.y; z += v.z; return *this; }
	Quaterniont& operator-=(const Quaterniont& v) noexcept { w -= v.w; x -= v.x; y -= v.y; z -= v.z; return *this; }
	Quaterniont& operator*=(const Quaterniont& v) noexcept { w *= v.w; x *= v.x; y *= v.y; z *= v.z; return *this; }
	Quaterniont& operator/=(const Quaterniont& v) noexcept { w /= v.w; x /= v.x; y /= v.y; z /= v.z; return *this; }

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

	Quaterniont<T> cross(const Quaterniont<T>& q) const noexcept
	{
		return Quaterniont<T>(
			w * q.x + x * q.w + y * q.z - z * q.y,
			w * q.y + y * q.w + z * q.x - x * q.z,
			w * q.z + z * q.w + x * q.y - y * q.x,
			w * q.w - x * q.x - y * q.y - z * q.z);
	}

	void identity() noexcept
	{
		x = y = z = 0.0f; w = 1.0f;
	}

	void set(T xx, T yy, T zz) noexcept
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
	}

	void set(T xx, T yy, T zz, T ww) noexcept
	{
		x = xx;
		y = yy;
		z = zz;
		w = ww;
	}

	void set(const Vector3t<T>& v) noexcept
	{
		this->set(v.x, v.y, v.z);
	}

	void set(const Vector4t<T>& v) noexcept
	{
		this->set(v.x, v.y, v.z, v.w);
	}

	Quaterniont& conjugate() noexcept
	{
		x = -x;
		y = -y;
		z = -z;
		return *this;
	}

	const Quaterniont& inverse() noexcept
	{
		return this->conjugate();
	}

	Quaterniont pow(const Quaterniont& q, float exponent) noexcept
	{
		if (fabs(q.w) > 0.9999f)
		{
			return q;
		}

		float alpha = acos(q.w);

		T newAlpha = alpha * exponent;

		T mult = sin(newAlpha) / sin(alpha);

		return Quaterniont(
			q.x * mult,
			q.y * mult,
			q.z * mult,
			cos(newAlpha));
	}

	void slerp(const Quaterniont& q1, const Quaterniont& q2, T t) noexcept
	{
		if (t <= 0.0f) { *this = q1; return; }
		if (t >= 1.0f) { *this = q2; return; }

		T cosOmega = clamp<T>(q1.dot(q2), -1.0, 1.0);

		if (cosOmega < 0.0f)
		{
			double theta = acos(-cosOmega);
			if (fabs(theta) < 1.0e-10)
			{
				*this = q1;
				return;
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
				return;
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
	}

	void setToRotateAboutX(T theta) noexcept
	{
		T thetaOver2 = theta * 0.5f;

		w = cos(thetaOver2);
		x = sin(thetaOver2);
		y = 0.0f;
		z = 0.0f;
	}

	void setToRotateAboutY(T theta) noexcept
	{
		T thetaOver2 = theta * 0.5f;

		w = cos(thetaOver2);
		x = 0.0f;
		y = sin(thetaOver2);
		z = 0.0f;
	}

	void setToRotateAboutZ(T theta) noexcept
	{
		T thetaOver2 = theta * 0.5f;

		w = cos(thetaOver2);
		x = 0.0f;
		y = 0.0f;
		z = sin(thetaOver2);
	}

	T getRotationAngle() const noexcept
	{
		T thetaOver2 = safeAcos(w);

		return thetaOver2 * 2.0f;
	}

	Vector3t<T> rotate(const Vector3t<T>& v) noexcept
	{
		Quaterniont q2(v.x, v.y, v.z, 0.f);
		Quaterniont q = *this, qinv = q;

		qinv.inverse();

		q = q ^ q2 ^ qinv;

		return Vector3t<T>(q.x, q.y, q.z);
	}

	const Quaterniont& normalize() noexcept
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
		return *this;
	}

	void makeRotate(const Vector3t<T>& axis) noexcept
	{
		this->makeRotate(axis.x, axis.y, axis.z);
	}

	void makeRotate(T pitch, T yaw, T roll) noexcept
	{
		const T fSinPitch(sin(pitch*static_cast<T>(0.5)));
		const T fCosPitch(cos(pitch*static_cast<T>(0.5)));
		const T fSinYaw(sin(yaw*static_cast<T>(0.5)));
		const T fCosYaw(cos(yaw*static_cast<T>(0.5)));
		const T fSinRoll(sin(roll*static_cast<T>(0.5)));
		const T fCosRoll(cos(roll*static_cast<T>(0.5)));
		const T fCosPitchCosYaw(fCosPitch*fCosYaw);
		const T fSinPitchSinYaw(fSinPitch*fSinYaw);
		x = fSinRoll * fCosPitchCosYaw - fCosRoll * fSinPitchSinYaw;
		y = fCosRoll * fCosPitch * fSinYaw - fSinRoll * fSinPitch * fCosYaw;
		z = fCosRoll * fSinPitch * fCosYaw + fSinRoll * fCosPitch * fSinYaw;
		w = fCosRoll * fCosPitchCosYaw + fSinRoll * fSinPitchSinYaw;
	}

	void makeRotate(const Vector3t<T>& axis, T angle) noexcept
	{
		assert(axis.x <= 1.0 && axis.y <= 1.0 && axis.z <= 1.0);

		const T sin_a = sin(angle / 2);
		const T cos_a = cos(angle / 2);
		x = axis.x * sin_a;
		y = axis.y * sin_a;
		z = axis.z * sin_a;
		w = cos_a;
	}

	void makeRotate(const Matrix3x3t<T>& rotate) noexcept
	{
		T t = 1 + rotate.a1 + rotate.b2 + rotate.c3;

		// large enough
		if (t > static_cast<T>(0.001))
		{
			T s = sqrt(t) * static_cast<T>(2.0);
			x = (rotate.c2 - rotate.b3) / s;
			y = (rotate.a3 - rotate.c1) / s;
			z = (rotate.b1 - rotate.a2) / s;
			w = static_cast<T>(0.25) * s;
		} // else we have to check several cases
		else if (rotate.a1 > rotate.b2 && rotate.a1 > rotate.c3)
		{
			// Column 0:
			T s = sqrt(static_cast<T>(1.0) + rotate.a1 - rotate.b2 - rotate.c3) * static_cast<T>(2.0);
			x = static_cast<T>(0.25) * s;
			y = (rotate.b1 + rotate.a2) / s;
			z = (rotate.a3 + rotate.c1) / s;
			w = (rotate.c2 - rotate.b3) / s;
		}
		else if (rotate.b2 > rotate.c3)
		{
			// Column 1:
			T s = sqrt(static_cast<T>(1.0) + rotate.b2 - rotate.a1 - rotate.c3) * static_cast<T>(2.0);
			x = (rotate.b1 + rotate.a2) / s;
			y = static_cast<T>(0.25) * s;
			z = (rotate.c2 + rotate.b3) / s;
			w = (rotate.a3 - rotate.c1) / s;
		}
		else
		{
			// Column 2:
			T s = sqrt(static_cast<T>(1.0) + rotate.c3 - rotate.a1 - rotate.b2) * static_cast<T>(2.0);
			x = (rotate.a3 + rotate.c1) / s;
			y = (rotate.c2 + rotate.b3) / s;
			z = static_cast<T>(0.25) * s;
			w = (rotate.b1 - rotate.a2) / s;
		}
	}

	void setToRotateObjectToInertial(const EulerAnglest<T>& orientation) noexcept
	{
		T sp, sb, sh;
		T cp, cb, ch;

		sinCos(&sp, &cp, orientation.pitch * 0.5f);
		sinCos(&sb, &cb, orientation.bank * 0.5f);
		sinCos(&sh, &ch, orientation.heading * 0.5f);

		x = ch * sp * cb + sh * cp * sb;
		y = -ch * sp * sb + sh * cp * cb;
		z = -sh * sp * cb + ch * cp * sb;
		w = ch * cp * cb + sh * sp * sb;
	}

	void setToRotateInertialToObject(const EulerAnglest<T>& orientation) noexcept
	{
		T sp, sb, sh;
		T cp, cb, ch;

		sinCos(&sp, &cp, orientation.pitch * 0.5f);
		sinCos(&sb, &cb, orientation.bank * 0.5f);
		sinCos(&sh, &ch, orientation.heading * 0.5f);

		x = -ch * sp * cb - sh * cp * sb;
		y = ch * sp * sb - sh * cb * cp;
		z = sh * sp *cb - ch * cp * sb;
		w = ch * cp * cb + sh * sp * sb;
	}

	EulerAnglest<T> fromInertialToObjectQuaterniont() const noexcept
	{
		EulerAnglest<T> euler;

		T sp = -2.0f * (y * z + w * x);

		if (fabs(sp) > 0.9999f)
		{
			euler.pitch = M_PI_2 * sp;
			euler.heading = atan2(-x * z - w * y, 0.5f - y * y - z * z);
			euler.bank = 0.0f;
		}
		else
		{
			euler.pitch = asin(sp);
			euler.heading = atan2(x * z - w * y, 0.5f - x * x - y * y);
			euler.bank = atan2(x * y - w * z, 0.5f - x * x - z * z);
		}

		return euler;
	}

	EulerAnglest<T> fromObjectToInertialQuaterniont() const noexcept
	{
		EulerAnglest<T> euler;

		T sp = -2.0f * (y * z - w * x);

		if (fabs(sp) > 0.9999f)
		{
			euler.pitch = M_PI_2 * sp;
			euler.heading = atan2(-x * z + w * y, 0.5f - y * y - z * z);
			euler.bank = 0.0f;
		}
		else
		{
			euler.pitch = asin(sp);
			euler.heading = atan2(x * z + w * y, 0.5f - x * x - y * y);
			euler.bank = atan2(x * y + w * z, 0.5f - x * x - z * z);
		}

		return euler;
	}

	const Vector3t<T> getRotationAxis() const noexcept
	{
		T sinThetaOver2Sq = 1.0f - w * w;

		if (sinThetaOver2Sq <= 0.0f)
		{
			return Vector3t<T>(1.0f, 0.0f, 0.0f);
		}

		T oneOverSinThetaOver2 = 1.0f / sqrt(sinThetaOver2Sq);

		return Vector3t<T>(x * oneOverSinThetaOver2, y * oneOverSinThetaOver2, z * oneOverSinThetaOver2);
	}

	void interpolate(const Quaterniont& pStart, const Quaterniont& pEnd, T pFactor) noexcept
	{
		// calc cosine theta
		T cosom = pStart.x * pEnd.x + pStart.y * pEnd.y + pStart.z * pEnd.z + pStart.w * pEnd.w;

		// adjust signs (if necessary)
		Quaterniont end = pEnd;
		if (cosom <static_cast<T>(0.0))
		{
			cosom = -cosom;
			end.x = -end.x;   // Reverse all signs
			end.y = -end.y;
			end.z = -end.z;
			end.w = -end.w;
		}

		// Calculate coefficients
		T sclp, sclq;
		if ((static_cast<T>(1.0) - cosom) > static_cast<T>(0.0001)) // 0.0001 -> some epsillon
		{
			// Standard case (slerp)
			T omega, sinom;
			omega = acos(cosom); // extract theta from dot product's cos theta
			sinom = sin(omega);
			sclp = sin((static_cast<T>(1.0) - pFactor) * omega) / sinom;
			sclq = sin(pFactor * omega) / sinom;
		}
		else
		{
			// Very close, do linear interp (because it's faster)
			sclp = static_cast<T>(1.0) - pFactor;
			sclq = pFactor;
		}

		x = sclp * pStart.x + sclq * end.x;
		y = sclp * pStart.y + sclq * end.y;
		z = sclp * pStart.z + sclq * end.z;
		w = sclp * pStart.w + sclq * end.w;
	}
};

template <typename T>
inline Quaterniont<T> min(const Quaterniont<T>& a, const Quaterniont<T>& b)
{
	return Quaterniont<T>(std::min(a.w, b.w), std::min(a.x, b.x), std::min(a.y, b.y), std::min(a.z, b.z));
}

template <typename T>
inline Quaterniont<T> max(const Quaterniont<T>& a, const Quaterniont<T>& b)
{
	return Quaterniont<T>(std::max(a.w, b.w), std::max(a.x, b.x), std::max(a.y, b.y), std::max(a.z, b.z));
}

template<typename T>
inline bool operator==(const Quaterniont<T>& q1, const Quaterniont<T>& q2)
{
	return q1.x == q2.x && q1.y == q2.y && q1.z == q2.z && q1.w == q2.w;
}

template<typename T>
inline bool operator!=(const Quaterniont<T>& q1, const Quaterniont<T>& q2)
{
	return q1.x != q2.x || q1.y != q2.y || q1.z != q2.z || q1.w != q2.w;
}

template<typename T>
inline Quaterniont<T> operator+(const Quaterniont<T>& a, const Quaterniont<T>& b)
{
	return Quaterniont<T>(a.x + b.x, a.y + b.y, a.z + b.z, 1.0);
}

template<typename T>
inline Quaterniont<T> operator/(const Quaterniont<T>& q, T f)
{
	return Quaterniont<T>(q.x / f, q.y / f, q.z / f, q.w / f);
}

template<typename T>
inline Quaterniont<T> operator/(T f, const Quaterniont<T>& q)
{
	return Quaterniont<T>(q.x / f, q.y / f, q.z / f, q.w / f);
}

template<typename T>
inline Quaterniont<T> operator*(const Quaterniont<T>& a, const Quaterniont<T>& b)
{
	Quaterniont<T> q;

	// scalar component
	q.w = a.w * b.w - a.x * b.x - a.y * b.y - a.z * b.z;

	// cross product
	q.x = a.y * b.z - a.z * b.y;
	q.y = a.z * b.x - a.x * b.z;
	q.z = a.x * b.y - a.y * b.x;

	q.x = (a.w * b.x) + (b.w * a.x) + q.x;
	q.y = (a.w * b.y) + (b.w * a.y) + q.y;
	q.z = (a.w * b.z) + (b.w * a.z) + q.z;

	q.normalize();

	return q;
}

template<typename T>
inline Quaterniont<T> operator^(const Quaterniont<T>& q1, const Quaterniont<T>& q2)
{
	return Quaterniont<T>(
		q1.w * q2.x + q1.x * q2.w + q1.y * q2.z - q1.z * q2.y,
		q1.w * q2.y + q1.y * q2.w + q1.z * q2.x - q1.x * q2.z,
		q1.w * q2.z + q1.z * q2.w + q1.x * q2.y - q1.y * q2.x,
		q1.w * q2.w - q1.x * q2.x - q1.y * q2.y - q1.z * q2.z);
}

_NAME_END

#endif
