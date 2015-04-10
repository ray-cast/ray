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
#ifndef _H_DCCMN_H_
#define _H_DCCMN_H_

#include <ray/mathfwd.h>

_NAME_BEGIN

template <typename T>
class Color3t
{
public:
    T r, g, b;

    static const Color3t<T> Black;
    static const Color3t<T> White;
    static const Color3t<T> Red;
    static const Color3t<T> Green;
    static const Color3t<T> Blue;
    static const Color3t<T> Gray;

    Color3t() : r(0.0f), g(0.0f), b(0.0f) {}
    Color3t(T _r, T _g, T _b) : r(_r), g(_g), b(_b) {}

    explicit Color3t(int o)
    {
        r = (char)(o)* (1 / (T)255.0);
        g = (char)(o >> 8) * (1 / (T)255.0);
        b = (char)(o >> 16) * (1 / (T)255.0);
    }

    explicit Color3t(unsigned int o)
    {
        r = (char)(o)* (1 / (T)255.0);
        g = (char)(o >> 8) * (1 / (T)255.0);
        b = (char)(o >> 16) * (1 / (T)255.0);
    }

    Color3t& operator+=(const Color3t& rt) { r += rt.r; g += rt.g; b += rt.b; return *this; }
    Color3t& operator-=(const Color3t& rt) { r -= rt.r; g -= rt.g; b -= rt.b; return *this; }
    Color3t& operator/=(const Color3t& rt) { r *= rt.r; g *= rt.g; b *= rt.b; return *this; }
    Color3t& operator*=(const Color3t& rt) { r /= rt.r; g /= rt.g; b /= rt.b; return *this; }

    T operator[](unsigned int i) const { return *(&r + i); }
    T& operator[](unsigned int i) { return *(&r + i); }

    T* ptr() { return (T*)this; }
    const T* ptr() const { return (const T*)this; }
    T* data() { return (T*)this; }
    const T* data() const { return (const T*)this; }

    bool IsBlack() const
    {
        static const T epsilon = 10e-3f;
        return fabs(r) < epsilon && fabs(g) < epsilon && fabs(b) < epsilon;
    }

    float gray() const
    {
        return r * 0.299f + g * 0.587f + b * 0.114f;
    }
};

template<typename T> const Color3t<T> Color3t<T>::Black = Color3t<T>(0.0, 0.0, 0.0);
template<typename T> const Color3t<T> Color3t<T>::White = Color3t<T>(1.0, 1.0, 1.0);
template<typename T> const Color3t<T> Color3t<T>::Red = Color3t<T>(1.0, 0.0, 0.0);
template<typename T> const Color3t<T> Color3t<T>::Green = Color3t<T>(0.0, 1.0, 0.0);
template<typename T> const Color3t<T> Color3t<T>::Blue = Color3t<T>(0.0, 0.0, 1.0);
template<typename T> const Color3t<T> Color3t<T>::Gray = Color3t<T>(0.004, 0.004, 0.004);

template <typename T>
bool operator==(const Color3t<T>& v1, const Color3t<T>& v2)
{
    return v1.r == v2.r && v1.g == v2.g && v1.b == v2.b;
}

template <typename T>
bool operator!=(const Color3t<T>& v1, const Color3t<T>& v2)
{
    return v1.r != v2.r || v1.g != v2.g || v1.b != v2.b;
}

template <typename T>
Color3t<T> operator+(const Color3t<T>& v1, const Color3t<T>& v2)
{
    return Color3t<T>(v1.r + v2.r, v1.g + v2.g, v1.b + v2.b);
}

template <typename T>
Color3t<T> operator-(const Color3t<T>& v1, const Color3t<T>& v2)
{
    return Color3t<T>(v1.r - v2.r, v1.g - v2.g, v1.b - v2.b);
}

template <typename T>
Color3t<T> operator*(const Color3t<T>& v1, const Color3t<T>& v2)
{
    return Color3t<T>(v1.r * v2.r, v1.g * v2.g, v1.b * v2.b);
}

template <typename T>
Color3t<T> operator/(const Color3t<T>& v1, const Color3t<T>& v2)
{
    return Color3t<T>(v1.r / v2.r, v1.g / v2.g, v1.b / v2.b);
}

template <typename _Tx, typename _Ty>
Color3t<_Tx> operator*(_Ty scale, const Color3t<_Tx>& v)
{
    return Color3t<_Tx>(v.r * scale, v.g * scale, v.b * scale);
}

template <typename _Tx, typename _Ty>
Color3t<_Tx> operator/(_Ty scale, const Color3t<_Tx>& v)
{
    return Color3t<_Tx>(v.r / scale, v.g / scale, v.b / scale);
}

template <typename _Tx, typename _Ty>
Color3t<_Tx> operator*(const Color3t<_Tx>& v, _Ty scale)
{
    return Color3t<_Tx>(v.r * scale, v.g * scale, v.b * scale);
}

template <typename _Tx, typename _Ty>
Color3t<_Tx> operator/(const Color3t<_Tx>& v, _Ty scale)
{
    return Color3t<_Tx>(v.r / scale, v.g / scale, v.b / scale);
}

template <typename T>
class Color4t
{
public:

    // Red, green, blue and alpha color values
    T r, g, b, a;

    static const Color4t<T> Black;
    static const Color4t<T> White;
    static const Color4t<T> Red;
    static const Color4t<T> Green;
    static const Color4t<T> Blue;
    static const Color4t<T> Gray;

    Color4t() :r(), g(), b(), a() {}
    Color4t(T _r, T _g, T _b, T _a) : r(_r), g(_g), b(_b), a(_a) {}
    Color4t(T _r, T _g, T _b) : r(_r), g(_g), b(_b), a(1.0) {}
    Color4t(const Color3t<T>& o, T aa) : r(o.r), g(o.g), b(o.b), a(aa) {}
    Color4t(const Color4t<T>& o) : r(o.r), g(o.g), b(o.b), a(o.a) {}
    Color4t(int o)
    {
        r = (char)(o)* (1 / (T)255.0);
        g = (char)(o >> 8) * (1 / (T)255.0);
        b = (char)(o >> 16) * (1 / (T)255.0);
        a = (char)(o >> 24) * (1 / (T)255.0);
    }

    Color4t(unsigned int o)
    {
        r = (char)(o)* (1 / (T)255.0);
        g = (char)(o >> 8) * (1 / (T)255.0);
        b = (char)(o >> 16) * (1 / (T)255.0);
        a = (char)(o >> 24) * (1 / (T)255.0);
    }

    Color4t& operator+=(const Color4t& rt) { r += rt.r; g += rt.g; b += rt.b; a += rt.a; return *this; }
    Color4t& operator-=(const Color4t& rt) { r -= rt.r; g -= rt.g; b -= rt.b; a -= rt.a; return *this; }
    Color4t& operator/=(const Color4t& rt) { r *= rt.r; g *= rt.g; b *= rt.b; a *= rt.a; return *this; }
    Color4t& operator*=(const Color4t& rt) { r /= rt.r; g /= rt.g; b /= rt.b; a /= rt.a; return *this; }

    inline T operator[](unsigned i) const { return *(&r + i); }
    inline T& operator[](unsigned i) { return *(&r + i); }

    void set(T _r, T _g, T _b, T _a)
    {
        r = _r; g = _g; b = _b; a = _a;
    }

    T* ptr() { return (T*)this; }
    const T* ptr() const { return (const T*)this; }
    T* data() { return (T*)this; }
    const T* data() const { return (const T*)this; }

    inline bool IsBlack() const
    {
        static const T epsilon = 10e-3f;
        return fabs(r) < epsilon && fabs(g) < epsilon && fabs(b) < epsilon;
    }
};

template<typename T> const Color4t<T> Color4t<T>::Black = Color4t<T>(0.0, 0.0, 0.0, 0.0);
template<typename T> const Color4t<T> Color4t<T>::White = Color4t<T>(1.0, 1.0, 1.0, 1.0);
template<typename T> const Color4t<T> Color4t<T>::Red = Color4t<T>(1.0, 0.0, 0.0, 1.0);
template<typename T> const Color4t<T> Color4t<T>::Green = Color4t<T>(0.0, 1.0, 0.0, 1.0);
template<typename T> const Color4t<T> Color4t<T>::Blue = Color4t<T>(0.0, 0.0, 1.0, 1.0);
template<typename T> const Color4t<T> Color4t<T>::Gray = Color4t<T>(0.5, 0.5, 0.5, 1.0);

template <typename T>
inline Color4t<T> min(const Color4t<T>& a, const Color4t<T>& b)
{
    return Color4t<T>(std::min(a.r, b.r), std::min(a.g, b.g), std::min(a.b, b.b), min(a.a, b.a));
}

template <typename T>
inline Color4t<T> max(const Color4t<T>& a, const Color4t<T>& b)
{
    return Color4t<T>(max(a.r, b.r), max(a.g, b.g), max(a.b, b.b), max(a.a, b.a));
}

template <typename T>
bool operator==(const Color4t<T>& lhs, const Color4t<T>& rhs)
{
    return lhs.r == rhs.r && lhs.g == rhs.g && lhs.b == rhs.b && lhs.a == rhs.a;
}

template <typename T>
bool operator!=(const Color4t<T>& lhs, const Color4t<T>& rhs)
{
    return lhs.r != rhs.r || lhs.g != rhs.g || lhs.b != rhs.b || lhs.a != rhs.a;
}

template <typename T>
Color4t<T> operator+(const Color4t<T>& lhs, const Color4t<T>& rhs)
{
    return Color4t<T>(lhs.r + rhs.r, lhs.g + rhs.g, lhs.b + rhs.b, lhs.a + rhs.a);
}

template <typename T>
Color4t<T> operator-(const Color4t<T>& lhs, const Color4t<T>& rhs)
{
    return Color4t<T>(lhs.r - rhs.r, lhs.g - rhs.g, lhs.b - rhs.b, lhs.a - rhs.a);
}

template <typename T>
Color4t<T> operator/(const Color4t<T>& lhs, const Color4t<T>& rhs)
{
    return Color4t<T>(lhs.r / rhs.r, lhs.g / rhs.g, lhs.b / rhs.b, lhs.a / rhs.a);
}

template <typename T>
Color4t<T> operator*(const Color4t<T>& lhs, const Color4t<T>& rhs)
{
    return Color4t<T>(lhs.r * rhs.r, lhs.g * rhs.g, lhs.b * rhs.b, lhs.a * rhs.a);
}

template <typename T>
Color4t<T> operator*(const Color4t<T>& lhs, const T rhs)
{
    return Color4t<T>(lhs.r * rhs, lhs.g * rhs, lhs.b * rhs, lhs.a * rhs);
}

template <typename T>
Color4t<T> operator/(const Color4t<T>& lhs, const T rhs)
{
    return Color4t<T>(lhs.r / rhs, lhs.g / rhs, lhs.b / rhs, lhs.a / rhs);
}

template <typename T>
Color4t<T> operator * (T f, const Color4t<T>& v)
{
    return Color4t<T>(f*v.r, f*v.g, f*v.b, f*v.a);
}

template <typename T>
Color4t<T> operator / (T f, const Color4t<T>& v)
{
    return Color4t<T>(f / v.r, f / v.g, f / v.b, f / v.a);
}

template <typename T>
Color4t<T> operator + (const Color4t<T>& v, T f)
{
    return Color4t<T>(f + v.r, f + v.g, f + v.b, f + v.a);
}

template <typename T>
Color4t<T> operator - (const Color4t<T>& v, T f)
{
    return Color4t<T>(v.r - f, v.g - f, v.b - f, v.a - f);
}

template <typename T>
Color4t<T> operator + (T f, const Color4t<T>& v)
{
    return Color4t<T>(f + v.r, f + v.g, f + v.b, f + v.a);
}

template <typename T>
Color4t<T> operator - (T f, const Color4t<T>& v)
{
    return Color4t<T>(f - v.r, f - v.g, f - v.b, f - v.a);
}

template<typename T>
class RGBt
{
public:
    RGBt() {}
    RGBt(T rr, T gg, T bb) :r(rr), g(gg), b(bb) {}

    T r, g, b;

    operator Color3() const
    {
        return Color3(r / 255.f, g / 255.f, b / 255.f);
    }
};

template <typename T>
bool operator==(const RGBt<T>& lhs, const RGBt<T>& rhs)
{
    return lhs.r == rhs.r && lhs.g == rhs.g && lhs.b == rhs.b;
}

template <typename T>
bool operator!=(const RGBt<T>& lhs, const RGBt<T>& rhs)
{
    return lhs.r != rhs.r || lhs.g != rhs.g || lhs.b != rhs.b;
}

template<typename T>
class RGBAt
{
public:
    RGBAt() {}
    RGBAt(T rr, T gg, T bb, T aa) :r(rr), g(gg), b(bb), a(aa) {}

    T r, g, b, a;

    operator Color4() const
    {
        return Color4(b / 255.f, g / 255.f, r / 255.f, a / 255.f);
    }
};

template <typename T>
bool operator==(const RGBAt<T>& lhs, const RGBAt<T>& rhs)
{
    return lhs.r == rhs.r && lhs.g == rhs.g && lhs.b == rhs.b && lhs.a == rhs.a;
}

template <typename T>
bool operator!=(const RGBAt<T>& lhs, const RGBAt<T>& rhs)
{
    return lhs.r != rhs.r || lhs.g != rhs.g || lhs.b != rhs.b || lhs.a != rhs.a;
}

template<typename T>
class BGRt
{
public:
    BGRt() {}
    BGRt(T rr, T gg, T bb) :r(rr), g(gg), b(bb) {}

    T b, g, r;

    operator Color3() const
    {
        return Color3(b / 255.f, g / 255.f, r / 255.f);
    }
};

template <typename T>
bool operator==(const BGRt<T>& lhs, const BGRt<T>& rhs)
{
    return lhs.r == rhs.r && lhs.g == rhs.g && lhs.b == rhs.b;
}

template <typename T>
bool operator!=(const BGRt<T>& lhs, const BGRt<T>& rhs)
{
    return lhs.r != rhs.r || lhs.g != rhs.g || lhs.b != rhs.b;
}

template<typename T>
class BGRAt
{
public:
    BGRAt() {}
    BGRAt(T rr, T gg, T bb, T aa) :r(rr), g(gg), b(bb), a(aa) {}

    T b, g, r, a;

    operator Color4() const
    {
        return Color4(b / 255.f, g / 255.f, r / 255.f, a / 255.f);
    }
};

template <typename T>
bool operator==(const BGRAt<T>& lhs, const BGRAt<T>& rhs)
{
    return lhs.r == rhs.r && lhs.g == rhs.g && lhs.b == rhs.b && lhs.a == rhs.a;
}

template <typename T>
bool operator!=(const BGRAt<T>& lhs, const BGRAt<T>& rhs)
{
    return lhs.r != rhs.r || lhs.g != rhs.g || lhs.b != rhs.b || lhs.a != rhs.a;
}

_NAME_END

#endif