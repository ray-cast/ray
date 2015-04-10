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
#ifndef _H_MATRIX2X2_H_
#define _H_MATRIX2X2_H_

#include <ray/vector2.h>
#include <ray/quat.h>

_NAME_BEGIN

template<typename T>
class Matrix2x2t
{
public:
    Matrix2x2t() noexcept
    {
    }

    Matrix2x2t(T mt00, T mt01, T mt10, T mt11)
    {
        set(mt00, mt01, mt10, mt11);
    }

    Matrix2x2t& set(T mt00, T mt01, T mt10, T mt11)
    {
        this->a1 = mt00; this->a2 = mt01;
        this->b1 = mt10; this->b2 = mt11;
        return *this;
    }

    void setScale(const Vector2t<T>& sz)
    {
        setScale(sz.x, sz.y);
    }

    void setScale(T x, T y)
    {
        this->a1 *= x;
        this->a2 *= x;
        this->b1 *= y;
        this->b2 *= y;
    }

    void makeScale(const Vector2t<T>& sz)
    {
        makeScale(sz.x, sz.y);
    }

    void makeScale(T x, T y)
    {
        set(x, 0,
            0, y);
    }

    void scale(const Vector2t<T>& sz)
    {
        scale(sz.x, sz.y);
    }

    void scale(T x, T y)
    {
        this->a1 *= x;
        this->b2 *= y;
    }

    void setRotate(const Quaterniont<T>& q)
    {
        setRotate(q.w, q.x, q.y, q.z);
    }

    void setRotate(T angle, const Vector3t<T>& axis)
    {
        setRotate(angle, axis.x, axis.y, axis.z);
    }

    void setRotate(T angle, T x, T y, T z)
    {
        makeRotate(angle, x, y, z);
    }

    void makeRotate(const Quaterniont<T>& q)
    {
        makeRotate(q.w, q.x, q.y, q.z);
    }

    void makeRotate(T angle, const Vector3t<T>& axis)
    {
        T c, s;

        sinCos(&s, &c, degrees(angle));

        Vector3t<T> v = axis;
        v.normalize();

        T x = v.x;
        T y = v.y;
        T z = v.z;

        T t = 1 - c;
        T tx = t*x, ty = t*y;

        this->a1 = (tx * x + c);
        this->a2 = (tx * y + s * z);

        this->b1 = (tx * y - s * z);
        this->b2 = (ty * y + c);
    }

    void makeRotate(T angle, T x, T y, T z)
    {
        makeRotate(angle, Vector3t<T>(x, y, z));
    }

    void makeRotationX(T theta)
    {
        T ang = degrees(theta);
        T c, s;

        sinCos(&s, &c, ang);

        set(
            1, 0,
            0, c);
    }

    void makeRotationY(T theta)
    {
        T ang = degrees(theta);
        T c, s;

        sinCos(&s, &c, ang);

        set(
            c, 0,
            0, 1);
    }

    void makeRotationZ(T theta)
    {
        T ang = degrees(theta);
        T c, s;

        sinCos(&s, &c, ang);

        set(
            c, -s,
            s, c);
    }

    T* ptr() { return (T*)this; }
    const T* ptr() const { return (const T*)this; }
    T* data() { return (T*)&a1; }
    const T* data() const { return (const T*)this; }

private:
    T a1, a2;
    T b1, b2;
};

_NAME_END

#endif