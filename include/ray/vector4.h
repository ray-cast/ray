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
#ifndef _H_VECTOR4_H_
#define _H_VECTOR4_H_

#include <ray/vector3.h>

_NAME_BEGIN

template<typename T>
class Vector4t
{
public:
	static const Vector4t<T> Zero;
	static const Vector4t<T> One;
	static const Vector4t<T> UnitX;
	static const Vector4t<T> UnitY;
	static const Vector4t<T> UnitZ;

	T x, y, z, w;

	Vector4t() noexcept {}
	Vector4t(T xyz, T ww)  noexcept : x(xyz), y(xyz), z(xyz), w(ww) {}
	Vector4t(T xx, T yy, T zz, T ww = 1.0)  noexcept : x(xx), y(yy), z(zz), w(ww) {}
	Vector4t(const Vector3t<T>& v, T w)  noexcept : x(v.x) , y(v.y) , z(v.z), w(w) {}
	Vector4t(const Vector4t<T>& v) noexcept : x(v.x), y(v.y) , z(v.z), w(v.w) {}
	Vector4t(const Vector2t<T>& xy, const Vector2t<T>& zw) noexcept : x(xy.x) , y(xy.y), z(zw.x), w(zw.y) {}
	Vector4t(const Vector2t<T>& xy, float z, float w) noexcept : x(xy.x), y(xy.y), z(z), w(w) {}
	Vector4t(T xx, const Vector3t<T>& yzw) noexcept : x(xx), y(yzw.x), z(yzw.y), w(yzw.z) {}
	Vector4t(T xx, T yy, const Vector2t<T>& zw) noexcept : x(xx), y(yy), z(zw.x), w(zw.y) {}

	explicit Vector4t(T xyzw) noexcept : x(xyzw), y(xyzw), z(xyzw), w(xyzw) {}

	Vector4t<T>& operator+=(T scale) noexcept { x += scale; y += scale; z += scale; w += scale; return *this; }
	Vector4t<T>& operator-=(T scale) noexcept { x -= scale; y -= scale; z -= scale; w -= scale; return *this; }
	Vector4t<T>& operator*=(T scale) noexcept { x *= scale; y *= scale; z *= scale; w *= scale; return *this; }
	Vector4t<T>& operator/=(T scale) noexcept { x /= scale; y /= scale; z /= scale; w /= scale; return *this; }

	Vector4t<T>& operator+=(const Vector2t<T>& rt) { x += rt.x; y += rt.y; return *this; }
	Vector4t<T>& operator-=(const Vector2t<T>& rt) { x -= rt.x; y -= rt.y; return *this; }
	Vector4t<T>& operator/=(const Vector2t<T>& rt) { x *= rt.x; y *= rt.y; return *this; }
	Vector4t<T>& operator*=(const Vector2t<T>& rt) { x /= rt.x; y /= rt.y; return *this; }

	Vector4t<T>& operator+=(const Vector3t<T>& rt) { x += rt.x; y += rt.y; z += rt.z; return *this; }
	Vector4t<T>& operator-=(const Vector3t<T>& rt) { x -= rt.x; y -= rt.y; z -= rt.z; return *this; }
	Vector4t<T>& operator/=(const Vector3t<T>& rt) { x *= rt.x; y *= rt.y; z *= rt.z; return *this; }
	Vector4t<T>& operator*=(const Vector3t<T>& rt) { x /= rt.x; y /= rt.y; z /= rt.z; return *this; }

	Vector4t<T>& operator+=(const Vector4t<T>& rt) { x += rt.x; y += rt.y; z += rt.z; w += rt.w; return *this; }
	Vector4t<T>& operator-=(const Vector4t<T>& rt) { x -= rt.x; y -= rt.y; z -= rt.z; w -= rt.w; return *this; }
	Vector4t<T>& operator/=(const Vector4t<T>& rt) { x *= rt.x; y *= rt.y; z *= rt.z; w *= rt.w; return *this; }
	Vector4t<T>& operator*=(const Vector4t<T>& rt) { x /= rt.x; y /= rt.y; z /= rt.z; w /= rt.w; return *this; }

	template <typename S>
	explicit operator Vector4t<S>() const
	{
		return Vector4t<S>(static_cast<S>(x), static_cast<S>(y), static_cast<S>(z), static_cast<S>(w));
	}

	Vector2t<T> xx()  const { return Vector2t<T>(x, x); }
	Vector2t<T> xy()  const { return Vector2t<T>(x, y); }
	Vector2t<T> xz()  const { return Vector2t<T>(x, z); }
	Vector2t<T> xw()  const { return Vector2t<T>(x, w); }
	Vector3t<T> xxx() const { return Vector3t<T>(x, x, x); }
	Vector3t<T> xxy() const { return Vector3t<T>(x, x, y); }
	Vector3t<T> xxz() const { return Vector3t<T>(x, x, z); }
	Vector3t<T> xxw() const { return Vector3t<T>(x, x, w); }
	Vector3t<T> xyx() const { return Vector3t<T>(x, y, x); }
	Vector3t<T> xyy() const { return Vector3t<T>(x, y, y); }
	Vector3t<T> xyz() const { return Vector3t<T>(x, y, z); }
	Vector3t<T> xyw() const { return Vector3t<T>(x, y, w); }
	Vector3t<T> xzx() const { return Vector3t<T>(x, z, x); }
	Vector3t<T> xzy() const { return Vector3t<T>(x, z, y); }
	Vector3t<T> xzz() const { return Vector3t<T>(x, z, z); }
	Vector3t<T> xzw() const { return Vector3t<T>(x, z, w); }
	Vector3t<T> xwx() const { return Vector3t<T>(x, w, x); }
	Vector3t<T> xwy() const { return Vector3t<T>(x, w, y); }
	Vector3t<T> xwz() const { return Vector3t<T>(x, w, z); }
	Vector3t<T> xww() const { return Vector3t<T>(x, w, w); }
	Vector2t<T> yx()  const { return Vector2t<T>(y, x); }
	Vector2t<T> yy()  const { return Vector2t<T>(y, y); }
	Vector2t<T> yz()  const { return Vector2t<T>(y, z); }
	Vector2t<T> yw()  const { return Vector2t<T>(y, w); }
	Vector3t<T> yxx() const { return Vector3t<T>(y, x, x); }
	Vector3t<T> yxy() const { return Vector3t<T>(y, x, y); }
	Vector3t<T> yxz() const { return Vector3t<T>(y, x, z); }
	Vector3t<T> yxw() const { return Vector3t<T>(y, x, w); }
	Vector3t<T> yyx() const { return Vector3t<T>(y, y, x); }
	Vector3t<T> yyy() const { return Vector3t<T>(y, y, y); }
	Vector3t<T> yyz() const { return Vector3t<T>(y, y, z); }
	Vector3t<T> yyw() const { return Vector3t<T>(y, y, w); }
	Vector3t<T> yzx() const { return Vector3t<T>(y, z, x); }
	Vector3t<T> yzy() const { return Vector3t<T>(y, z, y); }
	Vector3t<T> yzz() const { return Vector3t<T>(y, z, z); }
	Vector3t<T> yzw() const { return Vector3t<T>(y, z, w); }
	Vector3t<T> ywx() const { return Vector3t<T>(y, w, x); }
	Vector3t<T> ywy() const { return Vector3t<T>(y, w, y); }
	Vector3t<T> ywz() const { return Vector3t<T>(y, w, z); }
	Vector3t<T> yww() const { return Vector3t<T>(y, w, w); }
	Vector2t<T> zx()  const { return Vector2t<T>(z, x); }
	Vector2t<T> zy()  const { return Vector2t<T>(z, y); }
	Vector2t<T> zz()  const { return Vector2t<T>(z, z); }
	Vector2t<T> zw()  const { return Vector2t<T>(z, w); }
	Vector3t<T> zxx() const { return Vector3t<T>(z, x, x); }
	Vector3t<T> zxy() const { return Vector3t<T>(z, x, y); }
	Vector3t<T> zxz() const { return Vector3t<T>(z, x, z); }
	Vector3t<T> zxw() const { return Vector3t<T>(z, x, w); }
	Vector3t<T> zyx() const { return Vector3t<T>(z, y, x); }
	Vector3t<T> zyy() const { return Vector3t<T>(z, y, y); }
	Vector3t<T> zyz() const { return Vector3t<T>(z, y, z); }
	Vector3t<T> zyw() const { return Vector3t<T>(z, y, w); }
	Vector3t<T> zzx() const { return Vector3t<T>(z, z, x); }
	Vector3t<T> zzy() const { return Vector3t<T>(z, z, y); }
	Vector3t<T> zzz() const { return Vector3t<T>(z, z, z); }
	Vector3t<T> zzw() const { return Vector3t<T>(z, z, w); }
	Vector3t<T> zwx() const { return Vector3t<T>(z, w, x); }
	Vector3t<T> zwy() const { return Vector3t<T>(z, w, y); }
	Vector3t<T> zwz() const { return Vector3t<T>(z, w, z); }
	Vector3t<T> zww() const { return Vector3t<T>(z, w, w); }
	Vector2t<T> wx()  const { return Vector2t<T>(w, x); }
	Vector2t<T> wy()  const { return Vector2t<T>(w, y); }
	Vector2t<T> wz()  const { return Vector2t<T>(w, z); }
	Vector2t<T> ww()  const { return Vector2t<T>(w, w); }
	Vector3t<T> wxx() const { return Vector3t<T>(w, x, x); }
	Vector3t<T> wxy() const { return Vector3t<T>(w, x, y); }
	Vector3t<T> wxz() const { return Vector3t<T>(w, x, z); }
	Vector3t<T> wxw() const { return Vector3t<T>(w, x, w); }
	Vector3t<T> wyx() const { return Vector3t<T>(w, y, x); }
	Vector3t<T> wyy() const { return Vector3t<T>(w, y, y); }
	Vector3t<T> wyz() const { return Vector3t<T>(w, y, z); }
	Vector3t<T> wyw() const { return Vector3t<T>(w, y, w); }
	Vector3t<T> wzx() const { return Vector3t<T>(w, z, x); }
	Vector3t<T> wzy() const { return Vector3t<T>(w, z, y); }
	Vector3t<T> wzz() const { return Vector3t<T>(w, z, z); }
	Vector3t<T> wzw() const { return Vector3t<T>(w, z, w); }
	Vector3t<T> wwx() const { return Vector3t<T>(w, w, x); }
	Vector3t<T> wwy() const { return Vector3t<T>(w, w, y); }
	Vector3t<T> wwz() const { return Vector3t<T>(w, w, z); }
	Vector3t<T> www() const { return Vector3t<T>(w, w, w); }
	Vector4t<T> xxxx() const { return Vector4t<T>(x, x, x, x); }
	Vector4t<T> xxxy() const { return Vector4t<T>(x, x, x, y); }
	Vector4t<T> xxxz() const { return Vector4t<T>(x, x, x, z); }
	Vector4t<T> xxxw() const { return Vector4t<T>(x, x, x, w); }
	Vector4t<T> xxyx() const { return Vector4t<T>(x, x, y, x); }
	Vector4t<T> xxyy() const { return Vector4t<T>(x, x, y, y); }
	Vector4t<T> xxyz() const { return Vector4t<T>(x, x, y, z); }
	Vector4t<T> xxyw() const { return Vector4t<T>(x, x, y, w); }
	Vector4t<T> xxzx() const { return Vector4t<T>(x, x, z, x); }
	Vector4t<T> xxzy() const { return Vector4t<T>(x, x, z, y); }
	Vector4t<T> xxzz() const { return Vector4t<T>(x, x, z, z); }
	Vector4t<T> xxzw() const { return Vector4t<T>(x, x, z, w); }
	Vector4t<T> xxwx() const { return Vector4t<T>(x, x, w, x); }
	Vector4t<T> xxwy() const { return Vector4t<T>(x, x, w, y); }
	Vector4t<T> xxwz() const { return Vector4t<T>(x, x, w, z); }
	Vector4t<T> xxww() const { return Vector4t<T>(x, x, w, w); }
	Vector4t<T> xyxx() const { return Vector4t<T>(x, y, x, x); }
	Vector4t<T> xyxy() const { return Vector4t<T>(x, y, x, y); }
	Vector4t<T> xyxz() const { return Vector4t<T>(x, y, x, z); }
	Vector4t<T> xyxw() const { return Vector4t<T>(x, y, x, w); }
	Vector4t<T> xyyx() const { return Vector4t<T>(x, y, y, x); }
	Vector4t<T> xyyy() const { return Vector4t<T>(x, y, y, y); }
	Vector4t<T> xyyz() const { return Vector4t<T>(x, y, y, z); }
	Vector4t<T> xyyw() const { return Vector4t<T>(x, y, y, w); }
	Vector4t<T> xyzx() const { return Vector4t<T>(x, y, z, x); }
	Vector4t<T> xyzy() const { return Vector4t<T>(x, y, z, y); }
	Vector4t<T> xyzz() const { return Vector4t<T>(x, y, z, z); }
	Vector4t<T> xyzw() const { return Vector4t<T>(x, y, z, w); }
	Vector4t<T> xywx() const { return Vector4t<T>(x, y, w, x); }
	Vector4t<T> xywy() const { return Vector4t<T>(x, y, w, y); }
	Vector4t<T> xywz() const { return Vector4t<T>(x, y, w, z); }
	Vector4t<T> xyww() const { return Vector4t<T>(x, y, w, w); }
	Vector4t<T> xzxx() const { return Vector4t<T>(x, z, x, x); }
	Vector4t<T> xzxy() const { return Vector4t<T>(x, z, x, y); }
	Vector4t<T> xzxz() const { return Vector4t<T>(x, z, x, z); }
	Vector4t<T> xzxw() const { return Vector4t<T>(x, z, x, w); }
	Vector4t<T> xzyx() const { return Vector4t<T>(x, z, y, x); }
	Vector4t<T> xzyy() const { return Vector4t<T>(x, z, y, y); }
	Vector4t<T> xzyz() const { return Vector4t<T>(x, z, y, z); }
	Vector4t<T> xzyw() const { return Vector4t<T>(x, z, y, w); }
	Vector4t<T> xzzx() const { return Vector4t<T>(x, z, z, x); }
	Vector4t<T> xzzy() const { return Vector4t<T>(x, z, z, y); }
	Vector4t<T> xzzz() const { return Vector4t<T>(x, z, z, z); }
	Vector4t<T> xzzw() const { return Vector4t<T>(x, z, z, w); }
	Vector4t<T> xzwx() const { return Vector4t<T>(x, z, w, x); }
	Vector4t<T> xzwy() const { return Vector4t<T>(x, z, w, y); }
	Vector4t<T> xzwz() const { return Vector4t<T>(x, z, w, z); }
	Vector4t<T> xzww() const { return Vector4t<T>(x, z, w, w); }
    Vector4t<T> xwxx() const { return Vector4t<T>(x, w, x, x); }
    Vector4t<T> xwxy() const { return Vector4t<T>(x, w, x, y); }
    Vector4t<T> xwxz() const { return Vector4t<T>(x, w, x, z); }
    Vector4t<T> xwxw() const { return Vector4t<T>(x, w, x, w); }
    Vector4t<T> xwyx() const { return Vector4t<T>(x, w, y, x); }
    Vector4t<T> xwyy() const { return Vector4t<T>(x, w, y, y); }
    Vector4t<T> xwyz() const { return Vector4t<T>(x, w, y, z); }
    Vector4t<T> xwyw() const { return Vector4t<T>(x, w, y, w); }
    Vector4t<T> xwzx() const { return Vector4t<T>(x, w, z, x); }
    Vector4t<T> xwzy() const { return Vector4t<T>(x, w, z, y); }
    Vector4t<T> xwzz() const { return Vector4t<T>(x, w, z, z); }
    Vector4t<T> xwzw() const { return Vector4t<T>(x, w, z, w); }
    Vector4t<T> xwwx() const { return Vector4t<T>(x, w, w, x); }
    Vector4t<T> xwwy() const { return Vector4t<T>(x, w, w, y); }
    Vector4t<T> xwwz() const { return Vector4t<T>(x, w, w, z); }
    Vector4t<T> xwww() const { return Vector4t<T>(x, w, w, w); }
    Vector4t<T> yxxx() const { return Vector4t<T>(y, x, x, x); }
    Vector4t<T> yxxy() const { return Vector4t<T>(y, x, x, y); }
    Vector4t<T> yxxz() const { return Vector4t<T>(y, x, x, z); }
    Vector4t<T> yxxw() const { return Vector4t<T>(y, x, x, w); }
    Vector4t<T> yxyx() const { return Vector4t<T>(y, x, y, x); }
    Vector4t<T> yxyy() const { return Vector4t<T>(y, x, y, y); }
    Vector4t<T> yxyz() const { return Vector4t<T>(y, x, y, z); }
    Vector4t<T> yxyw() const { return Vector4t<T>(y, x, y, w); }
    Vector4t<T> yxzx() const { return Vector4t<T>(y, x, z, x); }
    Vector4t<T> yxzy() const { return Vector4t<T>(y, x, z, y); }
    Vector4t<T> yxzz() const { return Vector4t<T>(y, x, z, z); }
    Vector4t<T> yxzw() const { return Vector4t<T>(y, x, z, w); }
    Vector4t<T> yxwx() const { return Vector4t<T>(y, x, w, x); }
    Vector4t<T> yxwy() const { return Vector4t<T>(y, x, w, y); }
    Vector4t<T> yxwz() const { return Vector4t<T>(y, x, w, z); }
    Vector4t<T> yxww() const { return Vector4t<T>(y, x, w, w); }
    Vector4t<T> yyxx() const { return Vector4t<T>(y, y, x, x); }
    Vector4t<T> yyxy() const { return Vector4t<T>(y, y, x, y); }
    Vector4t<T> yyxz() const { return Vector4t<T>(y, y, x, z); }
    Vector4t<T> yyxw() const { return Vector4t<T>(y, y, x, w); }
    Vector4t<T> yyyx() const { return Vector4t<T>(y, y, y, x); }
    Vector4t<T> yyyy() const { return Vector4t<T>(y, y, y, y); }
    Vector4t<T> yyyz() const { return Vector4t<T>(y, y, y, z); }
    Vector4t<T> yyyw() const { return Vector4t<T>(y, y, y, w); }
    Vector4t<T> yyzx() const { return Vector4t<T>(y, y, z, x); }
    Vector4t<T> yyzy() const { return Vector4t<T>(y, y, z, y); }
    Vector4t<T> yyzz() const { return Vector4t<T>(y, y, z, z); }
    Vector4t<T> yyzw() const { return Vector4t<T>(y, y, z, w); }
    Vector4t<T> yywx() const { return Vector4t<T>(y, y, w, x); }
    Vector4t<T> yywy() const { return Vector4t<T>(y, y, w, y); }
    Vector4t<T> yywz() const { return Vector4t<T>(y, y, w, z); }
    Vector4t<T> yyww() const { return Vector4t<T>(y, y, w, w); }
    Vector4t<T> yzxx() const { return Vector4t<T>(y, z, x, x); }
    Vector4t<T> yzxy() const { return Vector4t<T>(y, z, x, y); }
    Vector4t<T> yzxz() const { return Vector4t<T>(y, z, x, z); }
    Vector4t<T> yzxw() const { return Vector4t<T>(y, z, x, w); }
    Vector4t<T> yzyx() const { return Vector4t<T>(y, z, y, x); }
    Vector4t<T> yzyy() const { return Vector4t<T>(y, z, y, y); }
    Vector4t<T> yzyz() const { return Vector4t<T>(y, z, y, z); }
    Vector4t<T> yzyw() const { return Vector4t<T>(y, z, y, w); }
    Vector4t<T> yzzx() const { return Vector4t<T>(y, z, z, x); }
    Vector4t<T> yzzy() const { return Vector4t<T>(y, z, z, y); }
    Vector4t<T> yzzz() const { return Vector4t<T>(y, z, z, z); }
    Vector4t<T> yzzw() const { return Vector4t<T>(y, z, z, w); }
    Vector4t<T> yzwx() const { return Vector4t<T>(y, z, w, x); }
    Vector4t<T> yzwy() const { return Vector4t<T>(y, z, w, y); }
    Vector4t<T> yzwz() const { return Vector4t<T>(y, z, w, z); }
    Vector4t<T> yzww() const { return Vector4t<T>(y, z, w, w); }
    Vector4t<T> ywxx() const { return Vector4t<T>(y, w, x, x); }
    Vector4t<T> ywxy() const { return Vector4t<T>(y, w, x, y); }
    Vector4t<T> ywxz() const { return Vector4t<T>(y, w, x, z); }
    Vector4t<T> ywxw() const { return Vector4t<T>(y, w, x, w); }
    Vector4t<T> ywyx() const { return Vector4t<T>(y, w, y, x); }
    Vector4t<T> ywyy() const { return Vector4t<T>(y, w, y, y); }
    Vector4t<T> ywyz() const { return Vector4t<T>(y, w, y, z); }
    Vector4t<T> ywyw() const { return Vector4t<T>(y, w, y, w); }
    Vector4t<T> ywzx() const { return Vector4t<T>(y, w, z, x); }
    Vector4t<T> ywzy() const { return Vector4t<T>(y, w, z, y); }
    Vector4t<T> ywzz() const { return Vector4t<T>(y, w, z, z); }
    Vector4t<T> ywzw() const { return Vector4t<T>(y, w, z, w); }
    Vector4t<T> ywwx() const { return Vector4t<T>(y, w, w, x); }
    Vector4t<T> ywwy() const { return Vector4t<T>(y, w, w, y); }
    Vector4t<T> ywwz() const { return Vector4t<T>(y, w, w, z); }
    Vector4t<T> ywww() const { return Vector4t<T>(y, w, w, w); }
    Vector4t<T> zxxx() const { return Vector4t<T>(z, x, x, x); }
    Vector4t<T> zxxy() const { return Vector4t<T>(z, x, x, y); }
    Vector4t<T> zxxz() const { return Vector4t<T>(z, x, x, z); }
    Vector4t<T> zxxw() const { return Vector4t<T>(z, x, x, w); }
    Vector4t<T> zxyx() const { return Vector4t<T>(z, x, y, x); }
    Vector4t<T> zxyy() const { return Vector4t<T>(z, x, y, y); }
    Vector4t<T> zxyz() const { return Vector4t<T>(z, x, y, z); }
    Vector4t<T> zxyw() const { return Vector4t<T>(z, x, y, w); }
    Vector4t<T> zxzx() const { return Vector4t<T>(z, x, z, x); }
    Vector4t<T> zxzy() const { return Vector4t<T>(z, x, z, y); }
    Vector4t<T> zxzz() const { return Vector4t<T>(z, x, z, z); }
    Vector4t<T> zxzw() const { return Vector4t<T>(z, x, z, w); }
    Vector4t<T> zxwx() const { return Vector4t<T>(z, x, w, x); }
    Vector4t<T> zxwy() const { return Vector4t<T>(z, x, w, y); }
    Vector4t<T> zxwz() const { return Vector4t<T>(z, x, w, z); }
    Vector4t<T> zxww() const { return Vector4t<T>(z, x, w, w); }
    Vector4t<T> zyxx() const { return Vector4t<T>(z, y, x, x); }
    Vector4t<T> zyxy() const { return Vector4t<T>(z, y, x, y); }
    Vector4t<T> zyxz() const { return Vector4t<T>(z, y, x, z); }
    Vector4t<T> zyxw() const { return Vector4t<T>(z, y, x, w); }
    Vector4t<T> zyyx() const { return Vector4t<T>(z, y, y, x); }
    Vector4t<T> zyyy() const { return Vector4t<T>(z, y, y, y); }
    Vector4t<T> zyyz() const { return Vector4t<T>(z, y, y, z); }
    Vector4t<T> zyyw() const { return Vector4t<T>(z, y, y, w); }
    Vector4t<T> zyzx() const { return Vector4t<T>(z, y, z, x); }
    Vector4t<T> zyzy() const { return Vector4t<T>(z, y, z, y); }
    Vector4t<T> zyzz() const { return Vector4t<T>(z, y, z, z); }
    Vector4t<T> zyzw() const { return Vector4t<T>(z, y, z, w); }
    Vector4t<T> zywx() const { return Vector4t<T>(z, y, w, x); }
    Vector4t<T> zywy() const { return Vector4t<T>(z, y, w, y); }
    Vector4t<T> zywz() const { return Vector4t<T>(z, y, w, z); }
    Vector4t<T> zyww() const { return Vector4t<T>(z, y, w, w); }
    Vector4t<T> zzxx() const { return Vector4t<T>(z, z, x, x); }
    Vector4t<T> zzxy() const { return Vector4t<T>(z, z, x, y); }
    Vector4t<T> zzxz() const { return Vector4t<T>(z, z, x, z); }
    Vector4t<T> zzxw() const { return Vector4t<T>(z, z, x, w); }
    Vector4t<T> zzyx() const { return Vector4t<T>(z, z, y, x); }
    Vector4t<T> zzyy() const { return Vector4t<T>(z, z, y, y); }
    Vector4t<T> zzyz() const { return Vector4t<T>(z, z, y, z); }
    Vector4t<T> zzyw() const { return Vector4t<T>(z, z, y, w); }
    Vector4t<T> zzzx() const { return Vector4t<T>(z, z, z, x); }
    Vector4t<T> zzzy() const { return Vector4t<T>(z, z, z, y); }
    Vector4t<T> zzzz() const { return Vector4t<T>(z, z, z, z); }
    Vector4t<T> zzzw() const { return Vector4t<T>(z, z, z, w); }
    Vector4t<T> zzwx() const { return Vector4t<T>(z, z, w, x); }
    Vector4t<T> zzwy() const { return Vector4t<T>(z, z, w, y); }
    Vector4t<T> zzwz() const { return Vector4t<T>(z, z, w, z); }
    Vector4t<T> zzww() const { return Vector4t<T>(z, z, w, w); }
    Vector4t<T> zwxx() const { return Vector4t<T>(z, w, x, x); }
    Vector4t<T> zwxy() const { return Vector4t<T>(z, w, x, y); }
    Vector4t<T> zwxz() const { return Vector4t<T>(z, w, x, z); }
    Vector4t<T> zwxw() const { return Vector4t<T>(z, w, x, w); }
    Vector4t<T> zwyx() const { return Vector4t<T>(z, w, y, x); }
    Vector4t<T> zwyy() const { return Vector4t<T>(z, w, y, y); }
    Vector4t<T> zwyz() const { return Vector4t<T>(z, w, y, z); }
    Vector4t<T> zwyw() const { return Vector4t<T>(z, w, y, w); }
    Vector4t<T> zwzx() const { return Vector4t<T>(z, w, z, x); }
    Vector4t<T> zwzy() const { return Vector4t<T>(z, w, z, y); }
    Vector4t<T> zwzz() const { return Vector4t<T>(z, w, z, z); }
    Vector4t<T> zwzw() const { return Vector4t<T>(z, w, z, w); }
    Vector4t<T> zwwx() const { return Vector4t<T>(z, w, w, x); }
    Vector4t<T> zwwy() const { return Vector4t<T>(z, w, w, y); }
    Vector4t<T> zwwz() const { return Vector4t<T>(z, w, w, z); }
    Vector4t<T> zwww() const { return Vector4t<T>(z, w, w, w); }
    Vector4t<T> wxxx() const { return Vector4t<T>(w, x, x, x); }
    Vector4t<T> wxxy() const { return Vector4t<T>(w, x, x, y); }
    Vector4t<T> wxxz() const { return Vector4t<T>(w, x, x, z); }
    Vector4t<T> wxxw() const { return Vector4t<T>(w, x, x, w); }
    Vector4t<T> wxyx() const { return Vector4t<T>(w, x, y, x); }
    Vector4t<T> wxyy() const { return Vector4t<T>(w, x, y, y); }
    Vector4t<T> wxyz() const { return Vector4t<T>(w, x, y, z); }
    Vector4t<T> wxyw() const { return Vector4t<T>(w, x, y, w); }
    Vector4t<T> wxzx() const { return Vector4t<T>(w, x, z, x); }
    Vector4t<T> wxzy() const { return Vector4t<T>(w, x, z, y); }
    Vector4t<T> wxzz() const { return Vector4t<T>(w, x, z, z); }
    Vector4t<T> wxzw() const { return Vector4t<T>(w, x, z, w); }
    Vector4t<T> wxwx() const { return Vector4t<T>(w, x, w, x); }
    Vector4t<T> wxwy() const { return Vector4t<T>(w, x, w, y); }
    Vector4t<T> wxwz() const { return Vector4t<T>(w, x, w, z); }
    Vector4t<T> wxww() const { return Vector4t<T>(w, x, w, w); }
    Vector4t<T> wyxx() const { return Vector4t<T>(w, y, x, x); }
    Vector4t<T> wyxy() const { return Vector4t<T>(w, y, x, y); }
    Vector4t<T> wyxz() const { return Vector4t<T>(w, y, x, z); }
    Vector4t<T> wyxw() const { return Vector4t<T>(w, y, x, w); }
    Vector4t<T> wyyx() const { return Vector4t<T>(w, y, y, x); }
    Vector4t<T> wyyy() const { return Vector4t<T>(w, y, y, y); }
    Vector4t<T> wyyz() const { return Vector4t<T>(w, y, y, z); }
    Vector4t<T> wyyw() const { return Vector4t<T>(w, y, y, w); }
    Vector4t<T> wyzx() const { return Vector4t<T>(w, y, z, x); }
    Vector4t<T> wyzy() const { return Vector4t<T>(w, y, z, y); }
    Vector4t<T> wyzz() const { return Vector4t<T>(w, y, z, z); }
    Vector4t<T> wyzw() const { return Vector4t<T>(w, y, z, w); }
    Vector4t<T> wywx() const { return Vector4t<T>(w, y, w, x); }
    Vector4t<T> wywy() const { return Vector4t<T>(w, y, w, y); }
    Vector4t<T> wywz() const { return Vector4t<T>(w, y, w, z); }
    Vector4t<T> wyww() const { return Vector4t<T>(w, y, w, w); }
    Vector4t<T> wzxx() const { return Vector4t<T>(w, z, x, x); }
    Vector4t<T> wzxy() const { return Vector4t<T>(w, z, x, y); }
    Vector4t<T> wzxz() const { return Vector4t<T>(w, z, x, z); }
    Vector4t<T> wzxw() const { return Vector4t<T>(w, z, x, w); }
    Vector4t<T> wzyx() const { return Vector4t<T>(w, z, y, x); }
    Vector4t<T> wzyy() const { return Vector4t<T>(w, z, y, y); }
    Vector4t<T> wzyz() const { return Vector4t<T>(w, z, y, z); }
    Vector4t<T> wzyw() const { return Vector4t<T>(w, z, y, w); }
    Vector4t<T> wzzx() const { return Vector4t<T>(w, z, z, x); }
    Vector4t<T> wzzy() const { return Vector4t<T>(w, z, z, y); }
    Vector4t<T> wzzz() const { return Vector4t<T>(w, z, z, z); }
    Vector4t<T> wzzw() const { return Vector4t<T>(w, z, z, w); }
    Vector4t<T> wzwx() const { return Vector4t<T>(w, z, w, x); }
    Vector4t<T> wzwy() const { return Vector4t<T>(w, z, w, y); }
    Vector4t<T> wzwz() const { return Vector4t<T>(w, z, w, z); }
    Vector4t<T> wzww() const { return Vector4t<T>(w, z, w, w); }
    Vector4t<T> wwxx() const { return Vector4t<T>(w, w, x, x); }
    Vector4t<T> wwxy() const { return Vector4t<T>(w, w, x, y); }
    Vector4t<T> wwxz() const { return Vector4t<T>(w, w, x, z); }
    Vector4t<T> wwxw() const { return Vector4t<T>(w, w, x, w); }
    Vector4t<T> wwyx() const { return Vector4t<T>(w, w, y, x); }
    Vector4t<T> wwyy() const { return Vector4t<T>(w, w, y, y); }
    Vector4t<T> wwyz() const { return Vector4t<T>(w, w, y, z); }
    Vector4t<T> wwyw() const { return Vector4t<T>(w, w, y, w); }
    Vector4t<T> wwzx() const { return Vector4t<T>(w, w, z, x); }
    Vector4t<T> wwzy() const { return Vector4t<T>(w, w, z, y); }
    Vector4t<T> wwzz() const { return Vector4t<T>(w, w, z, z); }
    Vector4t<T> wwzw() const { return Vector4t<T>(w, w, z, w); }
    Vector4t<T> wwwx() const { return Vector4t<T>(w, w, w, x); }
    Vector4t<T> wwwy() const { return Vector4t<T>(w, w, w, y); }
    Vector4t<T> wwwz() const { return Vector4t<T>(w, w, w, z); }
    Vector4t<T> wwww() const { return Vector4t<T>(w, w, w, w); }

	Vector4t<T>& set(T xx, T yy, T zz)
	{
		x = xx; y = yy; z = zz;
		return *this;
	}

	Vector4t<T>& set(T xx, T yy, T zz, T ww)
	{
		x = xx; y = yy; z = zz; w = ww;
		return *this;
	}

	Vector4t<T>& set(const Vector3t<T>& v)
	{
		x = v.x; y = v.y; z = v.z;
		return *this;
	}

	template<typename ostream>
	ostream& operator << (ostream& os)
	{
		os << x << ", " << y << ", " << z << ", " << w;
		return os;
	}

	template<typename istream>
	istream& operator >> (istream& is)
	{
		is >> x;
		is.ignore(2);
		is >> y;
		is.ignore(2);
		is >> z;
		is.ignore(2);
		is >> w;
		return is;
	}

	T& getComponent(unsigned char index)
	{
		switch (index)
		{
		case COMPONENT_X: return x;
		case COMPONENT_Y: return y;
		case COMPONENT_Z: return z;
		case COMPONENT_W: return w;
		case 'x': return x;
		case 'y': return y;
		case 'z': return z;
		case 'w': return w;
		default:
			assert(false);
		}
	}

	T* ptr() { return (T*)this; }
	const T* ptr() const { return (const T*)this; }
	T* data() { return (T*)this; }
	const T* data() const { return (const T*)this; }
};

template<typename T> const Vector4t<T> Vector4t<T>::Zero = Vector4t<T>((T)0.0, (T)0.0, (T)0.0, (T)0.0);
template<typename T> const Vector4t<T> Vector4t<T>::One = Vector4t<T>((T)1.0, (T)1.0, (T)1.0, (T)1.0);
template<typename T> const Vector4t<T> Vector4t<T>::UnitX = Vector4t<T>((T)1.0, (T)0.0, (T)0.0, (T)1.0);
template<typename T> const Vector4t<T> Vector4t<T>::UnitY = Vector4t<T>((T)0.0, (T)1.0, (T)0.0, (T)1.0);
template<typename T> const Vector4t<T> Vector4t<T>::UnitZ = Vector4t<T>((T)0.0, (T)0.0, (T)1.0, (T)1.0);

template<typename T>
inline bool operator==(const Vector4t<T>& v1, const Vector4t<T>& v2) noexcept
{
	return v1.x == v2.x && v1.y == v2.y && v1.z == v2.z && v1.w == v2.w;
}

template<typename T>
inline bool operator!=(const Vector4t<T>& v1, const Vector4t<T>& v2) noexcept
{
	return v1.x != v2.x || v1.y != v2.y || v1.z != v2.z || v1.w != v2.w;
}

template<typename T>
inline Vector4t<T> operator-(const Vector4t<T>& v) noexcept
{
	return Vector4t<T>(-v.x, -v.y, -v.z, -v.w)
}

template<typename T>
inline Vector4t<T> operator+(const Vector4t<T>& v1, const Vector4t<T>& v2) noexcept
{
	return Vector4t<T>(v1.x + v2.x, v1.y + v2.y, v1.z + v2.z, v1.w + v2.w);
}

template<typename T>
inline Vector4t<T> operator-(const Vector4t<T>& v1, const Vector4t<T>& v2) noexcept
{
	return Vector4t<T>(v1.x - v2.x, v1.y - v2.y, v1.z - v2.z, v1.w - v2.w);
}

template<typename T>
inline Vector4t<T> operator*(const Vector4t<T>& v1, const Vector4t<T>& v2) noexcept
{
	return Vector4t<T>(v1.x * v2.x, v1.y * v2.y, v1.z * v2.z, v1.w * v2.w);
}

template<typename T>
inline Vector4t<T> operator/(const Vector4t<T>& v1, const Vector4t<T>& v2) noexcept
{
	return Vector4t<T>(v1.x / v2.x, v1.y / v2.y, v1.z / v2.z);
}

template<typename T>
inline Vector4t<T> operator+(const Vector4t<T>& v1, const Vector3t<T>& v2) noexcept
{
	return Vector4t<T>(v1.x + v2.x, v1.y + v2.y, v1.z + v2.z, v1.w);
}

template<typename T>
inline Vector4t<T> operator-(const Vector4t<T>& v1, const Vector3t<T>& v2) noexcept
{
	return Vector4t<T>(v1.x - v2.x, v1.y - v2.y, v1.z - v2.z, v1.w);
}

template<typename T>
inline Vector4t<T> operator*(const Vector4t<T>& v1, const Vector3t<T>& v2) noexcept
{
	return Vector4t<T>(v1.x * v2.x, v1.y * v2.y, v1.z * v2.z, v1.w);
}

template<typename T>
inline Vector4t<T> operator/(const Vector4t<T>& v1, const Vector3t<T>& v2) noexcept
{
	return Vector4t<T>(v1.x / v2.x, v1.y / v2.y, v1.z / v2.z, v1.w);
}

template<typename T>
inline T operator*(const Vector4t<T>& v1, const Vector3t<T>& v2)
{
	return v1.x * v2.x + v1.y * v2.y + v1.z * v2.z + v1.w;
}

template<typename T>
inline Vector4t<T> operator*(const Vector4t<T>& v1, T scale) noexcept
{
	return Vector4t<T>(v1.x * scale, v1.y * scale, v1.z * scale, v1.w * scale);
}

template<typename T>
inline Vector4t<T> operator/(const Vector4t<T>& v1, T scale) noexcept
{
	return Vector4t<T>(v1.x / scale, v1.y / scale, v1.z / scale, v1.w / scale);
}

namespace math
{
	template<typename T>
	inline T dot(const Vector4t<T>& v1, const Vector4t<T>& v2) noexcept
	{
		return v1.x * v2.x + v1.y * v2.y + v1.z * v2.z + v1.w * v2.w;
	}

	template<typename T>
	Vector4t<T> cross(const Vector4t<T>& v1, const Vector4t<T>& v2) noexcept
	{
		return Vector4t<T>(
			v1.w * v2.x + v1.x * v2.w + v1.z * v2.y - v1.y * v2.z,
			v1.w * v2.y + v1.y * v2.w + v1.x * v2.z - v1.z * v2.x,
			v1.w * v2.z + v1.z * v2.w + v1.y * v2.x - v1.x * v2.y,
			v1.w * v2.w - v1.x * v2.x - v1.y * v2.y - v1.z * v2.z);
	}

	template<typename T>
	inline T length2(const Vector4t<T>& v) noexcept
	{
		return dot(v, v);
	}

	template<typename T>
	inline T length(const Vector4t<T>& v) noexcept
	{
		return std::sqrt(length2(v));
	}

	template<typename T>
	inline T distance(const Vector4t<T>& v) noexcept
	{
		return length(v - *this);
	}

	template<typename T>
	inline T sqrDistance(const Vector4t<T>& v1, const Vector4t<T>& v2) noexcept
	{
		return length2(v1 - v2);
	}

	template<typename T>
	inline Vector4t<T> negate(const Vector4t<T>& v) noexcept
	{
		return Vector4t<T>(-v.x, -v.y, -v.z, -v.w);
	}

	template<typename T>
	inline Vector4t<T> clamp(const Vector4t<T>& t, T min, T max)
	{
		return Vector4t<T>(
			std::max(min, std::min(max, t.x)),
			std::max(min, std::min(max, t.y)),
			std::max(min, std::min(max, t.z)),
			std::max(min, std::min(max, t.w))
			);
	}

	template<typename T>
	inline Vector4t<T> saturate(const Vector4t<T>& v) noexcept
	{
		return clamp(v, 0.0f, 1.0f);
	}

	template<typename T>
	inline Vector4t<T> normalize(const Vector4t<T>& v) noexcept
	{
		T magSq = length2(v);
		if (magSq > static_cast<T>(0.0))
		{
			T invSqrt = 1.0f / sqrt(magSq);
			return v * invSqrt;
		}

		return v;
	}

	template <typename T>
	inline Vector4t<T> min(const Vector4t<T>& a, const Vector4t<T>& b)
	{
		return Vector4t<T>(std::min(a.x, b.x), std::min(a.y, b.y), std::min(a.z, b.z), std::min(a.w, b.w));
	}

	template <typename T>
	inline Vector4t<T> max(const Vector4t<T>& a, const Vector4t<T>& b)
	{
		return Vector4t<T>(std::max(a.x, b.x), std::max(a.y, b.y), std::max(a.z, b.z), std::max(a.w, b.w));
	}

	template<typename T>
	inline Vector4t<T> abs(const Vector4t<T>& v)
	{
		return Vector4t<T>(std::abs(v.x), std::abs(v.y), std::abs(v.z), std::abs(v.w));
	}

	template<typename T>
	inline Vector4t<T> cos(const Vector4t<T>& v)
	{
		return Vector4t<T>(std::cos(v.x), std::cos(v.y), std::cos(v.z), std::cos(v.w));
	}

	template<typename T>
	inline Vector4t<T> sin(const Vector4t<T>& v)
	{
		return Vector4t<T>(std::sin(v.x), std::sin(v.y), std::sin(v.z), std::sin(v.w));
	}

	template<typename T>
	inline Vector4t<T> tan(const Vector4t<T>& v)
	{
		return Vector4t<T>(std::tan(v.x), std::tan(v.y), std::tan(v.z), std::tan(v.w));
	}

	template<typename T>
	inline Vector4t<T> acos(const Vector4t<T>& v)
	{
		return Vector4t<T>(std::acos(v.x), std::acos(v.y), std::acos(v.z), std::acos(v.w));
	}

	template<typename T>
	inline Vector4t<T> asin(const Vector4t<T>& v)
	{
		return Vector4t<T>(std::asin(v.x), std::asin(v.y), std::asin(v.z), std::asin(v.w));
	}

	template<typename T>
	inline Vector4t<T> atan(const Vector4t<T>& v)
	{
		return Vector4t<T>(std::atan(v.x), std::atan(v.y), std::atan(v.z), std::atan(v.w));
	}

	template<typename T>
	inline Vector4t<T> exp(const Vector4t<T>& v)
	{
		return Vector4t<T>(std::exp(v.x), std::exp(v.y), std::exp(v.z), std::exp(v.w));
	}

	template<typename T>
	inline Vector4t<T> log(const Vector4t<T>& v)
	{
		return Vector4t<T>(std::log(v.x), std::log(v.y), std::log(v.z), std::log(v.w));
	}

	template<typename T>
	inline Vector4t<T> log10(const Vector4t<T>& v)
	{
		return Vector4t<T>(std::log10(v.x), std::log10(v.y), std::log10(v.z), std::log10(v.w));
	}

	template<typename T>
	inline Vector4t<T> pow(const Vector4t<T>& v1, const Vector4t<T>& v2)
	{
		return Vector4t<T>(std::pow(v1.x, v2.x), std::pow(v1.y, v2.y), std::pow(v1.z, v2.z), std::pow(v1.w, v2.w));
	}

	template<typename T>
	inline Vector4t<T> random(const Vector4t<T>& min, const Vector4t<T>& max)
	{
		return Vector4t<T>(random(min.x, max.x), random(min.y, max.y), random(min.z, max.z), random(min.w, max.w));
	}
}

_NAME_END

#endif
