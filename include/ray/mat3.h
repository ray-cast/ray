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
    Matrix3x3t()
    {
    }

    Matrix3x3t(T _a1, T _a2, T _a3, T _b1, T _b2, T _b3, T _c1, T _c2, T _c3)
        : a1(_a1), a2(_a2), a3(_a3)
        , b1(_b1), b2(_b2), b3(_b3)
        , c1(_c1), c2(_c2), c3(_c3)
    {
    }

    Matrix3x3t(const Matrix3x3t<T>& m1, const Matrix3x3t<T>& m2)
    {
        this->multiplyMatrices(m1, m2);
    }

    template <typename S>
    Matrix3x3t(const Matrix3x3t<S>& m)
    {
        a1 = m.a1; a2 = m.a2; a3 = m.a3;
        b1 = m.b1; b2 = m.b2; b3 = m.b3;
        c1 = m.c1; c2 = m.c2; c3 = m.c3;
    }

    template <typename S>
    explicit Matrix3x3t(const Matrix4x4t<S>& m)
    {
        a1 = m.a1; a2 = m.a2; a3 = m.a3;
        b1 = m.b1; b2 = m.b2; b3 = m.b3;
        c1 = m.c1; c2 = m.c2; c3 = m.c3;
    }

    template <typename S>
    operator Matrix3x3t<S>() const
    {
        return Matrix3x3t<S>(
            static_cast<S>(a1), static_cast<S>(a2), static_cast<S>(a3),
            static_cast<S>(b1), static_cast<S>(b2), static_cast<S>(b3),
            static_cast<S>(c1), static_cast<S>(c2), static_cast<S>(c3));
    }

	template <typename S>
	Matrix3x3t<T>& operator -= (const Matrix3x3t<S>& m)
	{
		a1 -= m.a1; b1 -= m.a2; c1 -= m.c1;
		a2 -= m.a2; b2 -= m.a2; c2 -= m.c2;
		a3 -= m.a3; b3 -= m.a2; c3 -= m.c3;
		return *this;
	}

    template <typename S>
    Matrix3x3t<T>& operator += (const Matrix3x3t<S>& m)
    {
        a1 += m.a1; b1 += m.a2; c1 += m.c1;
        a2 += m.a2; b2 += m.a2; c2 += m.c2;
        a3 += m.a3; b3 += m.a2; c3 += m.c3;
        return *this;
    }

	template <typename S>
    Matrix3x3t<T>& operator *= (const Matrix3x3t<S>& m)
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

    T& operator() (std::size_t m, std::size_t n)
    {
        return *(&a1)[m * 3 + n];
    }

    const T& operator() (std::size_t m, std::size_t n) const
    {
        return *(&a1)[m * 3 + n];
    }

    T& operator[] (std::size_t n)
    {
        return *(&a1)[n];
    }

    const T& operator[] (std::size_t n) const
    {
        return *(&a1)[n];
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
    T* data() { return (T*)&a1; }

    const T* ptr() const { return (const T*)&a1; }
    const T* data() const { return (const T*)&a1; }

    Matrix3x3t<T>& set(T mt00, T mt01, T mt02, T mt10, T mt11, T mt12, T mt20, T mt21, T mt22)
    {
        a1 = static_cast<T>(mt00);
        a2 = static_cast<T>(mt01);
        a3 = static_cast<T>(mt02);
        b1 = static_cast<T>(mt10);
        b2 = static_cast<T>(mt11);
        b3 = static_cast<T>(mt12);
        c1 = static_cast<T>(mt20);
        c2 = static_cast<T>(mt21);
        c3 = static_cast<T>(mt22);
        return *this;
    }

    Matrix3x3t<T>& set(const Matrix4x4t<T>& m)
    {
        a1 = m.a1; a2 = m.a2; a3 = m.a3;
        b1 = m.b1; b2 = m.b2; b3 = m.b3;
        c1 = m.c1; c2 = m.c2; c3 = m.c3;
        return *this;
    }

    Matrix3x3t<T>& loadIdentity()
    {
        set(1, 0, 0,
            0, 1, 0,
            0, 0, 1);
        return *this;
    }

    bool isIdentity() const
    {
        return
            a1 == 1 && a2 == 0 && a3 == 0 &&
            b1 == 0 && b2 == 1 && b3 == 0 &&
            c1 == 0 && c2 == 0 && c3 == 1;
    }

    Quaterniont<T> getRotate() const
    {
        T x, y, z, w;
        T t = 1 + a1 + b2 + c3;

        // large enough
        if (t > static_cast<T>(0.001))
        {
            T s = sqrt(t) * static_cast<T>(2.0);
            x = (c2 - b3) / s;
            y = (a3 - c1) / s;
            z = (b1 - a2) / s;
            w = static_cast<T>(0.25) * s;
        } // else we have to check several cases
        else if (a1 > b2 && a1 > c3)
        {
            // Column 0:
            T s = sqrt(static_cast<T>(1.0) + a1 - b2 - c3) * static_cast<T>(2.0);
            x = static_cast<T>(0.25) * s;
            y = (b1 + a2) / s;
            z = (a3 + c1) / s;
            w = (c2 - b3) / s;
        }
        else if (b2 > c3)
        {
            // Column 1:
            T s = sqrt(static_cast<T>(1.0) + b2 - a1 - c3) * static_cast<T>(2.0);
            x = (b1 + a2) / s;
            y = static_cast<T>(0.25) * s;
            z = (c2 + b3) / s;
            w = (a3 - c1) / s;
        }
        else
        {
            // Column 2:
            T s = sqrt(static_cast<T>(1.0) + c3 - a1 - b2) * static_cast<T>(2.0);
            x = (a3 + c1) / s;
            y = (c2 + b3) / s;
            z = static_cast<T>(0.25) * s;
            w = (b1 - a2) / s;
        }

        return Quaterniont<T>(x, y, z, w);
    }

    Vector3t<T> preMult(const Vector3t<T>& v) const
    {
        return Vector3t<T>(
            (a1*v.x + a2*v.y + a3*v.z),
            (b1*v.x + b2*v.y + b3*v.z),
            (c1*v.x + c2*v.y + c3*v.z));
    }

    Matrix3x3t<T>& multiplyMatrices(const Matrix3x3t<T>& m1, const Matrix3x3t<T>& m2)
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

    Matrix3x3t<T>& transpose()
    {
        std::swap((T&)a2, (T&)b1);
        std::swap((T&)a3, (T&)c1);
        std::swap((T&)b3, (T&)c2);
        return *this;
    }

    template<typename S>
    Matrix3x3t<T>& multiplyScalar(S scale)
    {
        a1 *= static_cast<T>(scale);
        a2 *= static_cast<T>(scale);
        a3 *= static_cast<T>(scale);
        b1 *= static_cast<T>(scale);
        b2 *= static_cast<T>(scale);
        b3 *= static_cast<T>(scale);
        c1 *= static_cast<T>(scale);
        c2 *= static_cast<T>(scale);
        c3 *= static_cast<T>(scale);
        return *this;
    }

    Matrix3x3t<T>& inverse()
    {
        // Compute the reciprocal determinant
        const T det = this->determinant();
        if (det == static_cast<T>(0.0))
        {
            // Matrix not invertible. Setting all elements to nan is not really
            // correct in a mathematical sense but it is easy to debug for the
            // programmer.
            const T nan = std::numeric_limits<T>::quiet_NaN();
            *this = Matrix3x3t<T>(
                nan, nan, nan,
                nan, nan, nan,
                nan, nan, nan);

            return *this;
        }

        T invdet = static_cast<T>(1.0) / det;

        a1 =  invdet * (b2 * c3 - b3 * c2);
        a2 = -invdet * (a2 * c3 - a3 * c2);
        a3 =  invdet * (a2 * b3 - a3 * b2);
        b1 = -invdet * (b1 * c3 - b3 * c1);
        b2 =  invdet * (a1 * c3 - a3 * c1);
        b3 = -invdet * (a1 * b3 - a3 * b1);
        c1 =  invdet * (b1 * c2 - b2 * c1);
        c2 = -invdet * (a1 * c2 - a2 * c1);
        c3 =  invdet * (a1 * b2 - a2 * b1);

        return *this;
    }

    T determinant() const
    {
        return a1 * b2 * c3 - a1 * b3 * c2 + a2 * b3 * c1 - a2 * b1 * c3 + a3 * b1 * c2 - a3 * b2 * c1;
    }

	Matrix3x3t<T>& setRotate(const Quaterniont<T>& q) { return setRotate(q.w, q.x, q.y, q.z); }
	Matrix3x3t<T>& setRotate(T angle, const Vector3t<T>& axis) { return setRotate(angle, axis.x, axis.y, axis.z); }
	Matrix3x3t<T>& setRotate(T angle, T x, T y, T z) { return makeRotate(angle, x, y, z); }

	Matrix3x3t<T>& rotate(const Quaterniont<T>& q) { return rotate(q.w, q.x, q.y, q.z); }
	Matrix3x3t<T>& rotate(T angle, const Vector3t<T>& axis) { return rotate(angle, axis.x, axis.y, axis.z); }
	Matrix3x3t<T>& rotate(T angle, T x, T y, T z)
    {
        Matrix3x3t m;
        m.makeRotate(angle, x, y, z);
        *this = m.mult(*this);
		return *this;
    }

	Matrix3x3t<T>& makeRotate(const Quaterniont<T>& q) { return makeRotate(q.w, q.x, q.y, q.z); }
	Matrix3x3t<T>& makeRotate(T angle, T x, T y, T z) { return makeRotate(angle, Vector3t<T>(x, y, z)); }
	Matrix3x3t<T>& makeRotate(T angle, const Vector3t<T>& axis)
    {
        T c, s;

        sinCos(&s, &c, angle);

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
        a2 = (tx * y + s * z);
        a3 = (tx * z - s * y);

        b1 = (tx * y - s * z);
        b2 = (ty * y + c);
        b3 = (ty * z + s * x);

        c1 = (tx * z + s * y);
        c2 = (ty * z - s * x);
        c3 = (tz * z + c);

		return *this;
    }

    Matrix3x3t<T>& makeRotate(const Vector3t<T>& axis)
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

        b1 = af + be * d;
        b2 = ae - bf * d;
        b3 = -b * c;

        c1 = bf - ae * d;
        c2 = be + af * d;
        c3 = a * c;

        return *this;
    }

    void makeRotationX(T theta)
    {
        T ang = theta;
        T c, s;

        sinCos(&s, &c, ang);

        set(
            1, 0, 0,
            0, c, -s,
            0, s, c);
    }

    void makeRotationY(T theta)
    {
        T ang = theta;
        T c, s;

        sinCos(&s, &c, ang);

        set(
            c, 0, s,
            0, 1, 0,
            -s, 0, c);
    }

    void makeRotationZ(T theta)
    {
        T c, s, ang = theta;

        sinCos(&s, &c, ang);

        set(
            c, -s, 0,
            s, c, 0,
            0, 0, 1);
    }

    void setScale(const Vector3t<T>& sz)
    {
        setScale(sz.x, sz.y, sz.z);
    }

    void setScale(T x, T y, T z = 0)
    {
        a1 = x;
        b2 = y;
        c3 = z;
    }

    void scale(const Vector3t<T>& sz)
    {
        scale(sz.x, sz.y, sz.z);
    }

    void scale(T x, T y, T z = 0)
    {
        a1 *= x;
        b2 *= y;
        c3 *= z;
    }

    void makeScale(const Vector3t<T>& sz)
    {
        setScale(sz.x, sz.y, sz.z);
    }

    void makeScale(T x, T y, T z = 0)
    {
        set(0, x, 0, 0);
        set(1, 0, y, 0);
        set(2, 0, 0, z);
    }

    void orthonormalize()
    {
        Vector3t<T> x(a1, b1, c1);
        Vector3t<T> y(a2, b2, c2);
        Vector3t<T> z;
        x.normalize();
        z = x ^ y;
        z.normalize();
        y = z ^ x;
        y.normalize();
        a1 = x.x; a2 = x.y; a3 = x.z;
        b1 = y.x; b2 = y.y; b3 = y.z;
        c1 = z.x; c3 = z.y; c3 = z.z;
    }

    static Matrix3x3t& translation(const Vector2t<T>& v, Matrix3x3t& out)
    {
        out = Matrix3x3t<T>();
        out.a3 = v.x;
        out.b3 = v.y;
        return out;
    }

    // -------------------------------------------------------------------
    /** @brief A function for creating a rotation matrix that rotates a
        *  vector called "from" into another vector called "to".
        * Input : from[3], to[3] which both must be *normalized* non-zero vectors
        * Output: mtx[3][3] -- a 3x3 matrix in colum-major form
        * Authors: Tomas Möller, John Hughes
        *          "Efficiently Building a Matrix to Rotate One Vector to Another"
        *          Journal of Graphics Tools, 4(4):1-4, 1999
        */
    static Matrix3x3t<T>& fromToMatrix(const Vector3t<T>& from, const Vector3t<T>& to, Matrix3x3t<T>& out)
    {
        const T e = from * to;
        const T f = (e < 0) ? -e : e;

        if (f > static_cast<T>(1.0) - static_cast<T>(0.00001))     /* "from" and "to"-vector almost parallel */
        {
            Vector3t<T> u, v;     /* temporary storage vectors */
            Vector3t<T> x;       /* vector most nearly orthogonal to "from" */

            x.x = (from.x > 0.0) ? from.x : -from.x;
            x.y = (from.y > 0.0) ? from.y : -from.y;
            x.z = (from.z > 0.0) ? from.z : -from.z;

            if (x.x < x.y)
            {
                if (x.x < x.z)
                {
                    x.x = static_cast<T>(1.0); x.y = x.z = static_cast<T>(0.0);
                }
                else
                {
                    x.z = static_cast<T>(1.0); x.y = x.z = static_cast<T>(0.0);
                }
            }
            else
            {
                if (x.y < x.z)
                {
                    x.y = static_cast<T>(1.0); x.x = x.z = static_cast<T>(0.0);
                }
                else
                {
                    x.z = static_cast<T>(1.0); x.x = x.y = static_cast<T>(0.0);
                }
            }

            u.x = x.x - from.x; u.y = x.y - from.y; u.z = x.z - from.z;
            v.x = x.x - to.x;   v.y = x.y - to.y;   v.z = x.z - to.z;

            const T c1 = static_cast<T>(2.0) / (u * u);
            const T c2 = static_cast<T>(2.0) / (v * v);
            const T c3 = c1 * c2  * (u * v);

            out.a1 = -c1 * u.x * u.x - c2 * v.x * v.x + c3 * v.x * v.x;
            out.a2 = -c1 * u.x * u.y - c2 * v.x * v.y + c3 * v.x * v.y;
            out.a3 = -c1 * u.x * u.z - c2 * v.x * v.z + c3 * v.x * v.y;

            out.b1 = -c1 * u.y * u.x - c2 * v.y * v.x + c3 * v.y * v.x;
            out.b2 = -c1 * u.y * u.y - c2 * v.y * v.y + c3 * v.y * v.y;
            out.b3 = -c1 * u.y * u.z - c2 * v.y * v.z + c3 * v.y * v.z;

            out.c1 = -c1 * u.z * u.x - c2 * v.z * v.x + c3 * v.z * v.z;
            out.c2 = -c1 * u.z * u.y - c2 * v.z * v.y + c3 * v.z * v.z;
            out.c3 = -c1 * u.z * u.z - c2 * v.z * v.z + c3 * v.z * v.z;
        }
        else  /* the most common case, unless "from"="to", or "from"=-"to" */
        {
            const Vector3t<T> v = from ^ to;
            /* ... use this hand optimized version (9 mults less) */
            const T h = static_cast<T>(1.0) / (static_cast<T>(1.0) + e);      /* optimization by Gottfried Chen */
            const T hvx = h * v.x;
            const T hvz = h * v.z;
            const T hvxy = hvx * v.y;
            const T hvxz = hvx * v.z;
            const T hvyz = hvz * v.y;
            out.a1 = e + hvx * v.x;
            out.a2 = hvxy - v.z;
            out.a3 = hvxz + v.y;

            out.b1 = hvxy + v.z;
            out.b2 = e + h * v.y * v.y;
            out.b3 = hvyz - v.x;

            out.c1 = hvxz - v.y;
            out.c2 = hvyz + v.x;
            out.c3 = e + hvz * v.z;
        }
        return out;
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

_NAME_END

#endif
