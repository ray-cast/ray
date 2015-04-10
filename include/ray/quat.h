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

    explicit Quaterniont(const Matrix3x3t<T>& pRotMatrix)
    {
        makeRotate(pRotMatrix);
    }

    Quaterniont(T rotx, T roty, T rotz)
    {
        setToRotateObjectToInertial(rotx, roty, rotz);
    }

    Quaterniont(const Vector3t<T>& axis, T angle)
    {
        makeRotate(axis, angle);
    }

    explicit Quaterniont(const Vector3t<T>& rotate)
    {
        x = rotate.x;
        y = rotate.y;
        z = rotate.z;
        w = 1.0;

        this->normalize();

        const T t = static_cast<T>(1.0) - (x*x) - (y*y) - (z*z);

        if (t < static_cast<T>(0.0))
            w = static_cast<T>(0.0);
        else
            w = sqrt(t);
    }

    Quaterniont operator-() const { return this->conjugate(); }
    Quaterniont& operator*=(T f) { w *= w; x *= f; y *= f; z *= f; return *this; }
    Quaterniont& operator/=(T f) { w /= w; x /= f; y /= f; z /= f; return *this; }
    Quaterniont& operator+=(const Quaterniont& v) { w += v.w; x += v.x; y += v.y; z += v.z; return *this; }
    Quaterniont& operator-=(const Quaterniont& v) { w -= v.w; x -= v.x; y -= v.y; z -= v.z; return *this; }
    Quaterniont& operator*=(const Quaterniont& v) { w *= v.w; x *= v.x; y *= v.y; z *= v.z; return *this; }
    Quaterniont& operator/=(const Quaterniont& v) { w /= v.w; x /= v.x; y /= v.y; z /= v.z; return *this; }

    T length() const
    {
        return sqrt(length2());
    }

    T length2() const
    {
        return x * x + y * y + z * z;
    }

    void identity()
    {
        x = y = z = 0.0f; w = 1.0f;
    }

    Quaterniont& conjugate()
    {
        x = -x;
        y = -y;
        z = -z;
        return *this;
    }

    const Quaterniont& inverse()
    {
        return conjugate();
    }

    Quaterniont pow(const Quaterniont& q, float exponent)
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

    Quaterniont slerp(const Quaterniont& q1, const Quaterniont& q2, T t)
    {
        if (t <= 0.0f) { return q1; }
        if (t >= 1.0f) { return q2; }

        T cosOmega = q1.dot(q2);

        T q2x = q2.x;
        T q2y = q2.y;
        T q2z = q2.z;
        T q2w = q2.w;

        if (cosOmega < 0.0f)
        {
            q2x = -q2x;
            q2y = -q2y;
            q2z = -q2z;
            q2w = -q2w;
            cosOmega = -cosOmega;
        }

        assert(cosOmega < 1.1f);

        T k0, k1;

        if (cosOmega > 0.9999f)
        {
            k0 = 1.0f - t;
            k1 = t;
        }
        else
        {
            T sinOmega = sqrt(1.0f - cosOmega * cosOmega);

            T omega = atan2(sinOmega, cosOmega);

            T oneOverSinOmega = 1.0f / sinOmega;

            k0 = sin((1.0f - t) * omega) * oneOverSinOmega;
            k1 = sin(t * omega) * oneOverSinOmega;
        }

        return Quaterniont(
            k0 * q1.x + k1 * q2x,
            k0 * q1.y + k1 * q2y,
            k0 * q1.z + k1 * q2z,
            k0 * q1.w + k1 * q2w);
    }

    void setToRotateAboutx(T theta)
    {
        T thetaOver2 = theta * 0.5f;

        w = cos(thetaOver2);
        x = sin(thetaOver2);
        y = 0.0f;
        z = 0.0f;
    }

    void setToRotateAbouty(T theta)
    {
        T thetaOver2 = theta * 0.5f;

        w = cos(thetaOver2);
        x = 0.0f;
        y = sin(thetaOver2);
        z = 0.0f;
    }

    void setToRotateAboutz(T theta)
    {
        T thetaOver2 = theta * 0.5f;

        w = cos(thetaOver2);
        x = 0.0f;
        y = 0.0f;
        z = sin(thetaOver2);
    }

    // ---------------------------------------------------------------------------
    /** Rotate a point by this quaternion */
    Vector3t<T> rotate(const Vector3t<T>& v)
    {
        Quaterniont q2(0.f, v.x, v.y, v.z);
        Quaterniont q = *this, qinv = q;

        q.conjugate();

        q = q ^ q2 ^ qinv;
        return Vector3t<T>(q.x, q.y, q.z);
    }

    // ---------------------------------------------------------------------------
    /** normalize the quaternion */
    const Quaterniont& normalize()
    {
        const T mag = sqrt(x*x + y*y + z*z + w*w);
        if (mag)
        {
            const T inv = (T)(1.0) / mag;
            x *= inv;
            y *= inv;
            z *= inv;
            w *= inv;
        }
        return *this;
    }

    // ---------------------------------------------------------------------------
    // Construction from an axis-angle pair
    void makeRotate(Vector3t<T>& axis, T angle)
    {
        axis.normalize();

        const T sin_a = sin(angle / 2);
        const T cos_a = cos(angle / 2);
        x = axis.x * sin_a;
        y = axis.y * sin_a;
        z = axis.z * sin_a;
        w = cos_a;
    }

    void makeRotate(const Matrix3x3t<T>& pRotMatrix)
    {
        T t = 1 + pRotMatrix.a1 + pRotMatrix.b2 + pRotMatrix.c3;

        // large enough
        if (t > static_cast<T>(0.001))
        {
            T s = sqrt(t) * static_cast<T>(2.0);
            x = (pRotMatrix.c2 - pRotMatrix.b3) / s;
            y = (pRotMatrix.a3 - pRotMatrix.c1) / s;
            z = (pRotMatrix.b1 - pRotMatrix.a2) / s;
            w = static_cast<T>(0.25) * s;
        } // else we have to check several cases
        else if (pRotMatrix.a1 > pRotMatrix.b2 && pRotMatrix.a1 > pRotMatrix.c3)
        {
            // Column 0:
            T s = sqrt(static_cast<T>(1.0) + pRotMatrix.a1 - pRotMatrix.b2 - pRotMatrix.c3) * static_cast<T>(2.0);
            x = static_cast<T>(0.25) * s;
            y = (pRotMatrix.b1 + pRotMatrix.a2) / s;
            z = (pRotMatrix.a3 + pRotMatrix.c1) / s;
            w = (pRotMatrix.c2 - pRotMatrix.b3) / s;
        }
        else if (pRotMatrix.b2 > pRotMatrix.c3)
        {
            // Column 1:
            T s = sqrt(static_cast<T>(1.0) + pRotMatrix.b2 - pRotMatrix.a1 - pRotMatrix.c3) * static_cast<T>(2.0);
            x = (pRotMatrix.b1 + pRotMatrix.a2) / s;
            y = static_cast<T>(0.25) * s;
            z = (pRotMatrix.c2 + pRotMatrix.b3) / s;
            w = (pRotMatrix.a3 - pRotMatrix.c1) / s;
        }
        else
        {
            // Column 2:
            T s = sqrt(static_cast<T>(1.0) + pRotMatrix.c3 - pRotMatrix.a1 - pRotMatrix.b2) * static_cast<T>(2.0);
            x = (pRotMatrix.a3 + pRotMatrix.c1) / s;
            y = (pRotMatrix.c2 + pRotMatrix.b3) / s;
            z = static_cast<T>(0.25) * s;
            w = (pRotMatrix.b1 - pRotMatrix.a2) / s;
        }
    }

    // ---------------------------------------------------------------------------
    // Construction from euler angles
    void setToRotateObjectToInertial(T fPitch, T fYaw, T fRoll)
    {
        const T fSinPitch(sin(fPitch*static_cast<T>(0.5)));
        const T fCosPitch(cos(fPitch*static_cast<T>(0.5)));
        const T fSinYaw(sin(fYaw*static_cast<T>(0.5)));
        const T fCosYaw(cos(fYaw*static_cast<T>(0.5)));
        const T fSinRoll(sin(fRoll*static_cast<T>(0.5)));
        const T fCosRoll(cos(fRoll*static_cast<T>(0.5)));
        const T fCosPitchCosYaw(fCosPitch*fCosYaw);
        const T fSinPitchSinYaw(fSinPitch*fSinYaw);
        x = fSinRoll * fCosPitchCosYaw - fCosRoll * fSinPitchSinYaw;
        y = fCosRoll * fSinPitch * fCosYaw + fSinRoll * fCosPitch * fSinYaw;
        z = fCosRoll * fCosPitch * fSinYaw - fSinRoll * fSinPitch * fCosYaw;
        w = fCosRoll * fCosPitchCosYaw + fSinRoll * fSinPitchSinYaw;
    }

    void setToRotateObjectToInertial(const EulerAnglest<T>& orientation)
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

    void setToRotateInertialToObject(const EulerAnglest<T>& orientation)
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

    EulerAnglest<T> fromInertialToObjectQuaterniont()
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

    EulerAnglest<T> fromObjectToInertialQuaterniont()
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

    T getRotationAngle() const
    {
        T thetaOver2 = safeAcos(w);

        return thetaOver2 * 2.0f;
    }

    const Vector3t<T> getRotationAxis() const
    {
        T sinThetaOver2Sq = 1.0f - w * w;

        if (sinThetaOver2Sq <= 0.0f)
        {
            return Vector3t<T>(1.0f, 0.0f, 0.0f);
        }

        T oneOverSinThetaOver2 = 1.0f / sqrt(sinThetaOver2Sq);

        return Vector3t<T>(x * oneOverSinThetaOver2, y * oneOverSinThetaOver2, z * oneOverSinThetaOver2);
    }

    // ---------------------------------------------------------------------------
    /** Performs a spherical interpolation between two quaternions and writes the result into the third.
     * @param pOut Target object to received the interpolated rotation.
     * @param pStart Start rotation of the interpolation at factor == 0.
     * @param pEnd End rotation, factor == 1.
     * @param pFactor Interpolation factor between 0 and 1. Values outside of this range yield undefined results.
     */
    void interpolate(const Quaterniont& pStart, const Quaterniont& pEnd, T pFactor)
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
inline Quaterniont<T> operator^(const Quaterniont<T>& q1, const Quaterniont<T>& q2)
{
    return Quaterniont<T>(
        q1.w * q2.w - q1.x * q2.x - q1.y * q2.y - q1.z * q2.z,
        q1.w * q2.x + q1.x * q2.w + q1.y * q2.z - q1.z * q2.y,
        q1.w * q2.y + q1.y * q2.w + q1.z * q2.x - q1.x * q2.z,
        q1.w * q2.z + q1.z * q2.w + q1.x * q2.y - q1.y * q2.x);
}

_NAME_END

#endif
