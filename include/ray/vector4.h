// +----------------------------------------------------------------------
// | Project : ray.
// | All rights reserved.
// +----------------------------------------------------------------------
// | Copyright (c) 2013-2017.
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
	typedef typename trait::_typeaddition<T>::value_type value_type;
	typedef typename trait::_typeaddition<T>::pointer pointer;
	typedef typename trait::_typeaddition<T>::const_pointer const_pointer;
	typedef typename trait::_typeaddition<T>::reference reference;
	typedef typename trait::_typeaddition<T>::const_reference const_reference;

	static const Vector4t<T> Zero;
	static const Vector4t<T> One;
	static const Vector4t<T> UnitX;
	static const Vector4t<T> UnitY;
	static const Vector4t<T> UnitZ;

	T x, y, z, w;

	Vector4t() noexcept {}
	Vector4t(T xyz, T ww)  noexcept : x(xyz), y(xyz), z(xyz), w(ww) {}
	Vector4t(T xx, T yy, T zz, T ww = 1.0)  noexcept : x(xx), y(yy), z(zz), w(ww) {}
	Vector4t(T xx, const Vector3t<T>& yzw) noexcept : x(xx), y(yzw.x), z(yzw.y), w(yzw.z) {}
	Vector4t(T xx, T yy, const Vector2t<T>& zw) noexcept : x(xx), y(yy), z(zw.x), w(zw.y) {}
	Vector4t(const Vector3t<T>& v, T w)  noexcept : x(v.x), y(v.y), z(v.z), w(w) {}
	Vector4t(const Vector2t<T>& xy, const Vector2t<T>& zw) noexcept : x(xy.x), y(xy.y), z(zw.x), w(zw.y) {}
	Vector4t(const Vector2t<T>& xy, T z, T w) noexcept : x(xy.x), y(xy.y), z(z), w(w) {}
	explicit Vector4t(T xyzw) noexcept : x(xyzw), y(xyzw), z(xyzw), w(xyzw) {}

	template<typename S, typename = std::enable_if<std::is_pointer<S>::value>>
	explicit Vector4t(S xyzw[4]) noexcept
		: x(static_cast<typename trait::_typeaddition<S>::value_type>(xyzw[0]))
		, y(static_cast<typename trait::_typeaddition<S>::value_type>(xyzw[1]))
		, z(static_cast<typename trait::_typeaddition<S>::value_type>(xyzw[2]))
		, w(static_cast<typename trait::_typeaddition<S>::value_type>(xyzw[3]))
	{
		assert(xyzw[0] < std::numeric_limits<typename trait::_typeaddition<S>::value_type>::max());
		assert(xyzw[1] < std::numeric_limits<typename trait::_typeaddition<S>::value_type>::max());
		assert(xyzw[2] < std::numeric_limits<typename trait::_typeaddition<S>::value_type>::max());
		assert(xyzw[3] < std::numeric_limits<typename trait::_typeaddition<S>::value_type>::max());
	}

	Vector4t<T>& operator+=(T scale) noexcept { x += scale; y += scale; z += scale; w += scale; return *this; }
	Vector4t<T>& operator-=(T scale) noexcept { x -= scale; y -= scale; z -= scale; w -= scale; return *this; }
	Vector4t<T>& operator*=(T scale) noexcept { x *= scale; y *= scale; z *= scale; w *= scale; return *this; }
	Vector4t<T>& operator/=(T scale) noexcept { x /= scale; y /= scale; z /= scale; w /= scale; return *this; }

	Vector4t<T>& operator+=(const Vector2t<T>& rt) noexcept { x += rt.x; y += rt.y; return *this; }
	Vector4t<T>& operator-=(const Vector2t<T>& rt) noexcept { x -= rt.x; y -= rt.y; return *this; }
	Vector4t<T>& operator/=(const Vector2t<T>& rt) noexcept { x *= rt.x; y *= rt.y; return *this; }
	Vector4t<T>& operator*=(const Vector2t<T>& rt) noexcept { x /= rt.x; y /= rt.y; return *this; }

	Vector4t<T>& operator+=(const Vector3t<T>& rt) noexcept { x += rt.x; y += rt.y; z += rt.z; return *this; }
	Vector4t<T>& operator-=(const Vector3t<T>& rt) noexcept { x -= rt.x; y -= rt.y; z -= rt.z; return *this; }
	Vector4t<T>& operator/=(const Vector3t<T>& rt) noexcept { x *= rt.x; y *= rt.y; z *= rt.z; return *this; }
	Vector4t<T>& operator*=(const Vector3t<T>& rt) noexcept { x /= rt.x; y /= rt.y; z /= rt.z; return *this; }

	Vector4t<T>& operator+=(const Vector4t<T>& rt) noexcept { x += rt.x; y += rt.y; z += rt.z; w += rt.w; return *this; }
	Vector4t<T>& operator-=(const Vector4t<T>& rt) noexcept { x -= rt.x; y -= rt.y; z -= rt.z; w -= rt.w; return *this; }
	Vector4t<T>& operator/=(const Vector4t<T>& rt) noexcept { x *= rt.x; y *= rt.y; z *= rt.z; w *= rt.w; return *this; }
	Vector4t<T>& operator*=(const Vector4t<T>& rt) noexcept { x /= rt.x; y /= rt.y; z /= rt.z; w /= rt.w; return *this; }

	template<typename S, typename = std::enable_if<!std::is_pointer<S>::value>>
	explicit operator Vector4t<S>() const noexcept
	{
		assert(x <= std::numeric_limits<typename trait::_typeaddition<S>::value_type>::max());
		assert(y <= std::numeric_limits<typename trait::_typeaddition<S>::value_type>::max());
		assert(z <= std::numeric_limits<typename trait::_typeaddition<S>::value_type>::max());

		return Vector4t<S>(static_cast<S>(x), static_cast<S>(y), static_cast<S>(z), static_cast<S>(w));
	}

	Vector2t<T> xx()  const noexcept { return Vector2t<T>(x, x); }
	Vector2t<T> xy()  const noexcept { return Vector2t<T>(x, y); }
	Vector2t<T> xz()  const noexcept { return Vector2t<T>(x, z); }
	Vector2t<T> xw()  const noexcept { return Vector2t<T>(x, w); }
	Vector3t<T> xxx() const noexcept { return Vector3t<T>(x, x, x); }
	Vector3t<T> xxy() const noexcept { return Vector3t<T>(x, x, y); }
	Vector3t<T> xxz() const noexcept { return Vector3t<T>(x, x, z); }
	Vector3t<T> xxw() const noexcept { return Vector3t<T>(x, x, w); }
	Vector3t<T> xyx() const noexcept { return Vector3t<T>(x, y, x); }
	Vector3t<T> xyy() const noexcept { return Vector3t<T>(x, y, y); }
	Vector3t<T> xyz() const noexcept { return Vector3t<T>(x, y, z); }
	Vector3t<T> xyw() const noexcept { return Vector3t<T>(x, y, w); }
	Vector3t<T> xzx() const noexcept { return Vector3t<T>(x, z, x); }
	Vector3t<T> xzy() const noexcept { return Vector3t<T>(x, z, y); }
	Vector3t<T> xzz() const noexcept { return Vector3t<T>(x, z, z); }
	Vector3t<T> xzw() const noexcept { return Vector3t<T>(x, z, w); }
	Vector3t<T> xwx() const noexcept { return Vector3t<T>(x, w, x); }
	Vector3t<T> xwy() const noexcept { return Vector3t<T>(x, w, y); }
	Vector3t<T> xwz() const noexcept { return Vector3t<T>(x, w, z); }
	Vector3t<T> xww() const noexcept { return Vector3t<T>(x, w, w); }
	Vector2t<T> yx()  const noexcept { return Vector2t<T>(y, x); }
	Vector2t<T> yy()  const noexcept { return Vector2t<T>(y, y); }
	Vector2t<T> yz()  const noexcept { return Vector2t<T>(y, z); }
	Vector2t<T> yw()  const noexcept { return Vector2t<T>(y, w); }
	Vector3t<T> yxx() const noexcept { return Vector3t<T>(y, x, x); }
	Vector3t<T> yxy() const noexcept { return Vector3t<T>(y, x, y); }
	Vector3t<T> yxz() const noexcept { return Vector3t<T>(y, x, z); }
	Vector3t<T> yxw() const noexcept { return Vector3t<T>(y, x, w); }
	Vector3t<T> yyx() const noexcept { return Vector3t<T>(y, y, x); }
	Vector3t<T> yyy() const noexcept { return Vector3t<T>(y, y, y); }
	Vector3t<T> yyz() const noexcept { return Vector3t<T>(y, y, z); }
	Vector3t<T> yyw() const noexcept { return Vector3t<T>(y, y, w); }
	Vector3t<T> yzx() const noexcept { return Vector3t<T>(y, z, x); }
	Vector3t<T> yzy() const noexcept { return Vector3t<T>(y, z, y); }
	Vector3t<T> yzz() const noexcept { return Vector3t<T>(y, z, z); }
	Vector3t<T> yzw() const noexcept { return Vector3t<T>(y, z, w); }
	Vector3t<T> ywx() const noexcept { return Vector3t<T>(y, w, x); }
	Vector3t<T> ywy() const noexcept { return Vector3t<T>(y, w, y); }
	Vector3t<T> ywz() const noexcept { return Vector3t<T>(y, w, z); }
	Vector3t<T> yww() const noexcept { return Vector3t<T>(y, w, w); }
	Vector2t<T> zx()  const noexcept { return Vector2t<T>(z, x); }
	Vector2t<T> zy()  const noexcept { return Vector2t<T>(z, y); }
	Vector2t<T> zz()  const noexcept { return Vector2t<T>(z, z); }
	Vector2t<T> zw()  const noexcept { return Vector2t<T>(z, w); }
	Vector3t<T> zxx() const noexcept { return Vector3t<T>(z, x, x); }
	Vector3t<T> zxy() const noexcept { return Vector3t<T>(z, x, y); }
	Vector3t<T> zxz() const noexcept { return Vector3t<T>(z, x, z); }
	Vector3t<T> zxw() const noexcept { return Vector3t<T>(z, x, w); }
	Vector3t<T> zyx() const noexcept { return Vector3t<T>(z, y, x); }
	Vector3t<T> zyy() const noexcept { return Vector3t<T>(z, y, y); }
	Vector3t<T> zyz() const noexcept { return Vector3t<T>(z, y, z); }
	Vector3t<T> zyw() const noexcept { return Vector3t<T>(z, y, w); }
	Vector3t<T> zzx() const noexcept { return Vector3t<T>(z, z, x); }
	Vector3t<T> zzy() const noexcept { return Vector3t<T>(z, z, y); }
	Vector3t<T> zzz() const noexcept { return Vector3t<T>(z, z, z); }
	Vector3t<T> zzw() const noexcept { return Vector3t<T>(z, z, w); }
	Vector3t<T> zwx() const noexcept { return Vector3t<T>(z, w, x); }
	Vector3t<T> zwy() const noexcept { return Vector3t<T>(z, w, y); }
	Vector3t<T> zwz() const noexcept { return Vector3t<T>(z, w, z); }
	Vector3t<T> zww() const noexcept { return Vector3t<T>(z, w, w); }
	Vector2t<T> wx()  const noexcept { return Vector2t<T>(w, x); }
	Vector2t<T> wy()  const noexcept { return Vector2t<T>(w, y); }
	Vector2t<T> wz()  const noexcept { return Vector2t<T>(w, z); }
	Vector2t<T> ww()  const noexcept { return Vector2t<T>(w, w); }
	Vector3t<T> wxx() const noexcept { return Vector3t<T>(w, x, x); }
	Vector3t<T> wxy() const noexcept { return Vector3t<T>(w, x, y); }
	Vector3t<T> wxz() const noexcept { return Vector3t<T>(w, x, z); }
	Vector3t<T> wxw() const noexcept { return Vector3t<T>(w, x, w); }
	Vector3t<T> wyx() const noexcept { return Vector3t<T>(w, y, x); }
	Vector3t<T> wyy() const noexcept { return Vector3t<T>(w, y, y); }
	Vector3t<T> wyz() const noexcept { return Vector3t<T>(w, y, z); }
	Vector3t<T> wyw() const noexcept { return Vector3t<T>(w, y, w); }
	Vector3t<T> wzx() const noexcept { return Vector3t<T>(w, z, x); }
	Vector3t<T> wzy() const noexcept { return Vector3t<T>(w, z, y); }
	Vector3t<T> wzz() const noexcept { return Vector3t<T>(w, z, z); }
	Vector3t<T> wzw() const noexcept { return Vector3t<T>(w, z, w); }
	Vector3t<T> wwx() const noexcept { return Vector3t<T>(w, w, x); }
	Vector3t<T> wwy() const noexcept { return Vector3t<T>(w, w, y); }
	Vector3t<T> wwz() const noexcept { return Vector3t<T>(w, w, z); }
	Vector3t<T> www() const noexcept { return Vector3t<T>(w, w, w); }
	Vector4t<T> xxxx() const noexcept { return Vector4t<T>(x, x, x, x); }
	Vector4t<T> xxxy() const noexcept { return Vector4t<T>(x, x, x, y); }
	Vector4t<T> xxxz() const noexcept { return Vector4t<T>(x, x, x, z); }
	Vector4t<T> xxxw() const noexcept { return Vector4t<T>(x, x, x, w); }
	Vector4t<T> xxyx() const noexcept { return Vector4t<T>(x, x, y, x); }
	Vector4t<T> xxyy() const noexcept { return Vector4t<T>(x, x, y, y); }
	Vector4t<T> xxyz() const noexcept { return Vector4t<T>(x, x, y, z); }
	Vector4t<T> xxyw() const noexcept { return Vector4t<T>(x, x, y, w); }
	Vector4t<T> xxzx() const noexcept { return Vector4t<T>(x, x, z, x); }
	Vector4t<T> xxzy() const noexcept { return Vector4t<T>(x, x, z, y); }
	Vector4t<T> xxzz() const noexcept { return Vector4t<T>(x, x, z, z); }
	Vector4t<T> xxzw() const noexcept { return Vector4t<T>(x, x, z, w); }
	Vector4t<T> xxwx() const noexcept { return Vector4t<T>(x, x, w, x); }
	Vector4t<T> xxwy() const noexcept { return Vector4t<T>(x, x, w, y); }
	Vector4t<T> xxwz() const noexcept { return Vector4t<T>(x, x, w, z); }
	Vector4t<T> xxww() const noexcept { return Vector4t<T>(x, x, w, w); }
	Vector4t<T> xyxx() const noexcept { return Vector4t<T>(x, y, x, x); }
	Vector4t<T> xyxy() const noexcept { return Vector4t<T>(x, y, x, y); }
	Vector4t<T> xyxz() const noexcept { return Vector4t<T>(x, y, x, z); }
	Vector4t<T> xyxw() const noexcept { return Vector4t<T>(x, y, x, w); }
	Vector4t<T> xyyx() const noexcept { return Vector4t<T>(x, y, y, x); }
	Vector4t<T> xyyy() const noexcept { return Vector4t<T>(x, y, y, y); }
	Vector4t<T> xyyz() const noexcept { return Vector4t<T>(x, y, y, z); }
	Vector4t<T> xyyw() const noexcept { return Vector4t<T>(x, y, y, w); }
	Vector4t<T> xyzx() const noexcept { return Vector4t<T>(x, y, z, x); }
	Vector4t<T> xyzy() const noexcept { return Vector4t<T>(x, y, z, y); }
	Vector4t<T> xyzz() const noexcept { return Vector4t<T>(x, y, z, z); }
	Vector4t<T> xyzw() const noexcept { return Vector4t<T>(x, y, z, w); }
	Vector4t<T> xywx() const noexcept { return Vector4t<T>(x, y, w, x); }
	Vector4t<T> xywy() const noexcept { return Vector4t<T>(x, y, w, y); }
	Vector4t<T> xywz() const noexcept { return Vector4t<T>(x, y, w, z); }
	Vector4t<T> xyww() const noexcept { return Vector4t<T>(x, y, w, w); }
	Vector4t<T> xzxx() const noexcept { return Vector4t<T>(x, z, x, x); }
	Vector4t<T> xzxy() const noexcept { return Vector4t<T>(x, z, x, y); }
	Vector4t<T> xzxz() const noexcept { return Vector4t<T>(x, z, x, z); }
	Vector4t<T> xzxw() const noexcept { return Vector4t<T>(x, z, x, w); }
	Vector4t<T> xzyx() const noexcept { return Vector4t<T>(x, z, y, x); }
	Vector4t<T> xzyy() const noexcept { return Vector4t<T>(x, z, y, y); }
	Vector4t<T> xzyz() const noexcept { return Vector4t<T>(x, z, y, z); }
	Vector4t<T> xzyw() const noexcept { return Vector4t<T>(x, z, y, w); }
	Vector4t<T> xzzx() const noexcept { return Vector4t<T>(x, z, z, x); }
	Vector4t<T> xzzy() const noexcept { return Vector4t<T>(x, z, z, y); }
	Vector4t<T> xzzz() const noexcept { return Vector4t<T>(x, z, z, z); }
	Vector4t<T> xzzw() const noexcept { return Vector4t<T>(x, z, z, w); }
	Vector4t<T> xzwx() const noexcept { return Vector4t<T>(x, z, w, x); }
	Vector4t<T> xzwy() const noexcept { return Vector4t<T>(x, z, w, y); }
	Vector4t<T> xzwz() const noexcept { return Vector4t<T>(x, z, w, z); }
	Vector4t<T> xzww() const noexcept { return Vector4t<T>(x, z, w, w); }
	Vector4t<T> xwxx() const noexcept { return Vector4t<T>(x, w, x, x); }
	Vector4t<T> xwxy() const noexcept { return Vector4t<T>(x, w, x, y); }
	Vector4t<T> xwxz() const noexcept { return Vector4t<T>(x, w, x, z); }
	Vector4t<T> xwxw() const noexcept { return Vector4t<T>(x, w, x, w); }
	Vector4t<T> xwyx() const noexcept { return Vector4t<T>(x, w, y, x); }
	Vector4t<T> xwyy() const noexcept { return Vector4t<T>(x, w, y, y); }
	Vector4t<T> xwyz() const noexcept { return Vector4t<T>(x, w, y, z); }
	Vector4t<T> xwyw() const noexcept { return Vector4t<T>(x, w, y, w); }
	Vector4t<T> xwzx() const noexcept { return Vector4t<T>(x, w, z, x); }
	Vector4t<T> xwzy() const noexcept { return Vector4t<T>(x, w, z, y); }
	Vector4t<T> xwzz() const noexcept { return Vector4t<T>(x, w, z, z); }
	Vector4t<T> xwzw() const noexcept { return Vector4t<T>(x, w, z, w); }
	Vector4t<T> xwwx() const noexcept { return Vector4t<T>(x, w, w, x); }
	Vector4t<T> xwwy() const noexcept { return Vector4t<T>(x, w, w, y); }
	Vector4t<T> xwwz() const noexcept { return Vector4t<T>(x, w, w, z); }
	Vector4t<T> xwww() const noexcept { return Vector4t<T>(x, w, w, w); }
	Vector4t<T> yxxx() const noexcept { return Vector4t<T>(y, x, x, x); }
	Vector4t<T> yxxy() const noexcept { return Vector4t<T>(y, x, x, y); }
	Vector4t<T> yxxz() const noexcept { return Vector4t<T>(y, x, x, z); }
	Vector4t<T> yxxw() const noexcept { return Vector4t<T>(y, x, x, w); }
	Vector4t<T> yxyx() const noexcept { return Vector4t<T>(y, x, y, x); }
	Vector4t<T> yxyy() const noexcept { return Vector4t<T>(y, x, y, y); }
	Vector4t<T> yxyz() const noexcept { return Vector4t<T>(y, x, y, z); }
	Vector4t<T> yxyw() const noexcept { return Vector4t<T>(y, x, y, w); }
	Vector4t<T> yxzx() const noexcept { return Vector4t<T>(y, x, z, x); }
	Vector4t<T> yxzy() const noexcept { return Vector4t<T>(y, x, z, y); }
	Vector4t<T> yxzz() const noexcept { return Vector4t<T>(y, x, z, z); }
	Vector4t<T> yxzw() const noexcept { return Vector4t<T>(y, x, z, w); }
	Vector4t<T> yxwx() const noexcept { return Vector4t<T>(y, x, w, x); }
	Vector4t<T> yxwy() const noexcept { return Vector4t<T>(y, x, w, y); }
	Vector4t<T> yxwz() const noexcept { return Vector4t<T>(y, x, w, z); }
	Vector4t<T> yxww() const noexcept { return Vector4t<T>(y, x, w, w); }
	Vector4t<T> yyxx() const noexcept { return Vector4t<T>(y, y, x, x); }
	Vector4t<T> yyxy() const noexcept { return Vector4t<T>(y, y, x, y); }
	Vector4t<T> yyxz() const noexcept { return Vector4t<T>(y, y, x, z); }
	Vector4t<T> yyxw() const noexcept { return Vector4t<T>(y, y, x, w); }
	Vector4t<T> yyyx() const noexcept { return Vector4t<T>(y, y, y, x); }
	Vector4t<T> yyyy() const noexcept { return Vector4t<T>(y, y, y, y); }
	Vector4t<T> yyyz() const noexcept { return Vector4t<T>(y, y, y, z); }
	Vector4t<T> yyyw() const noexcept { return Vector4t<T>(y, y, y, w); }
	Vector4t<T> yyzx() const noexcept { return Vector4t<T>(y, y, z, x); }
	Vector4t<T> yyzy() const noexcept { return Vector4t<T>(y, y, z, y); }
	Vector4t<T> yyzz() const noexcept { return Vector4t<T>(y, y, z, z); }
	Vector4t<T> yyzw() const noexcept { return Vector4t<T>(y, y, z, w); }
	Vector4t<T> yywx() const noexcept { return Vector4t<T>(y, y, w, x); }
	Vector4t<T> yywy() const noexcept { return Vector4t<T>(y, y, w, y); }
	Vector4t<T> yywz() const noexcept { return Vector4t<T>(y, y, w, z); }
	Vector4t<T> yyww() const noexcept { return Vector4t<T>(y, y, w, w); }
	Vector4t<T> yzxx() const noexcept { return Vector4t<T>(y, z, x, x); }
	Vector4t<T> yzxy() const noexcept { return Vector4t<T>(y, z, x, y); }
	Vector4t<T> yzxz() const noexcept { return Vector4t<T>(y, z, x, z); }
	Vector4t<T> yzxw() const noexcept { return Vector4t<T>(y, z, x, w); }
	Vector4t<T> yzyx() const noexcept { return Vector4t<T>(y, z, y, x); }
	Vector4t<T> yzyy() const noexcept { return Vector4t<T>(y, z, y, y); }
	Vector4t<T> yzyz() const noexcept { return Vector4t<T>(y, z, y, z); }
	Vector4t<T> yzyw() const noexcept { return Vector4t<T>(y, z, y, w); }
	Vector4t<T> yzzx() const noexcept { return Vector4t<T>(y, z, z, x); }
	Vector4t<T> yzzy() const noexcept { return Vector4t<T>(y, z, z, y); }
	Vector4t<T> yzzz() const noexcept { return Vector4t<T>(y, z, z, z); }
	Vector4t<T> yzzw() const noexcept { return Vector4t<T>(y, z, z, w); }
	Vector4t<T> yzwx() const noexcept { return Vector4t<T>(y, z, w, x); }
	Vector4t<T> yzwy() const noexcept { return Vector4t<T>(y, z, w, y); }
	Vector4t<T> yzwz() const noexcept { return Vector4t<T>(y, z, w, z); }
	Vector4t<T> yzww() const noexcept { return Vector4t<T>(y, z, w, w); }
	Vector4t<T> ywxx() const noexcept { return Vector4t<T>(y, w, x, x); }
	Vector4t<T> ywxy() const noexcept { return Vector4t<T>(y, w, x, y); }
	Vector4t<T> ywxz() const noexcept { return Vector4t<T>(y, w, x, z); }
	Vector4t<T> ywxw() const noexcept { return Vector4t<T>(y, w, x, w); }
	Vector4t<T> ywyx() const noexcept { return Vector4t<T>(y, w, y, x); }
	Vector4t<T> ywyy() const noexcept { return Vector4t<T>(y, w, y, y); }
	Vector4t<T> ywyz() const noexcept { return Vector4t<T>(y, w, y, z); }
	Vector4t<T> ywyw() const noexcept { return Vector4t<T>(y, w, y, w); }
	Vector4t<T> ywzx() const noexcept { return Vector4t<T>(y, w, z, x); }
	Vector4t<T> ywzy() const noexcept { return Vector4t<T>(y, w, z, y); }
	Vector4t<T> ywzz() const noexcept { return Vector4t<T>(y, w, z, z); }
	Vector4t<T> ywzw() const noexcept { return Vector4t<T>(y, w, z, w); }
	Vector4t<T> ywwx() const noexcept { return Vector4t<T>(y, w, w, x); }
	Vector4t<T> ywwy() const noexcept { return Vector4t<T>(y, w, w, y); }
	Vector4t<T> ywwz() const noexcept { return Vector4t<T>(y, w, w, z); }
	Vector4t<T> ywww() const noexcept { return Vector4t<T>(y, w, w, w); }
	Vector4t<T> zxxx() const noexcept { return Vector4t<T>(z, x, x, x); }
	Vector4t<T> zxxy() const noexcept { return Vector4t<T>(z, x, x, y); }
	Vector4t<T> zxxz() const noexcept { return Vector4t<T>(z, x, x, z); }
	Vector4t<T> zxxw() const noexcept { return Vector4t<T>(z, x, x, w); }
	Vector4t<T> zxyx() const noexcept { return Vector4t<T>(z, x, y, x); }
	Vector4t<T> zxyy() const noexcept { return Vector4t<T>(z, x, y, y); }
	Vector4t<T> zxyz() const noexcept { return Vector4t<T>(z, x, y, z); }
	Vector4t<T> zxyw() const noexcept { return Vector4t<T>(z, x, y, w); }
	Vector4t<T> zxzx() const noexcept { return Vector4t<T>(z, x, z, x); }
	Vector4t<T> zxzy() const noexcept { return Vector4t<T>(z, x, z, y); }
	Vector4t<T> zxzz() const noexcept { return Vector4t<T>(z, x, z, z); }
	Vector4t<T> zxzw() const noexcept { return Vector4t<T>(z, x, z, w); }
	Vector4t<T> zxwx() const noexcept { return Vector4t<T>(z, x, w, x); }
	Vector4t<T> zxwy() const noexcept { return Vector4t<T>(z, x, w, y); }
	Vector4t<T> zxwz() const noexcept { return Vector4t<T>(z, x, w, z); }
	Vector4t<T> zxww() const noexcept { return Vector4t<T>(z, x, w, w); }
	Vector4t<T> zyxx() const noexcept { return Vector4t<T>(z, y, x, x); }
	Vector4t<T> zyxy() const noexcept { return Vector4t<T>(z, y, x, y); }
	Vector4t<T> zyxz() const noexcept { return Vector4t<T>(z, y, x, z); }
	Vector4t<T> zyxw() const noexcept { return Vector4t<T>(z, y, x, w); }
	Vector4t<T> zyyx() const noexcept { return Vector4t<T>(z, y, y, x); }
	Vector4t<T> zyyy() const noexcept { return Vector4t<T>(z, y, y, y); }
	Vector4t<T> zyyz() const noexcept { return Vector4t<T>(z, y, y, z); }
	Vector4t<T> zyyw() const noexcept { return Vector4t<T>(z, y, y, w); }
	Vector4t<T> zyzx() const noexcept { return Vector4t<T>(z, y, z, x); }
	Vector4t<T> zyzy() const noexcept { return Vector4t<T>(z, y, z, y); }
	Vector4t<T> zyzz() const noexcept { return Vector4t<T>(z, y, z, z); }
	Vector4t<T> zyzw() const noexcept { return Vector4t<T>(z, y, z, w); }
	Vector4t<T> zywx() const noexcept { return Vector4t<T>(z, y, w, x); }
	Vector4t<T> zywy() const noexcept { return Vector4t<T>(z, y, w, y); }
	Vector4t<T> zywz() const noexcept { return Vector4t<T>(z, y, w, z); }
	Vector4t<T> zyww() const noexcept { return Vector4t<T>(z, y, w, w); }
	Vector4t<T> zzxx() const noexcept { return Vector4t<T>(z, z, x, x); }
	Vector4t<T> zzxy() const noexcept { return Vector4t<T>(z, z, x, y); }
	Vector4t<T> zzxz() const noexcept { return Vector4t<T>(z, z, x, z); }
	Vector4t<T> zzxw() const noexcept { return Vector4t<T>(z, z, x, w); }
	Vector4t<T> zzyx() const noexcept { return Vector4t<T>(z, z, y, x); }
	Vector4t<T> zzyy() const noexcept { return Vector4t<T>(z, z, y, y); }
	Vector4t<T> zzyz() const noexcept { return Vector4t<T>(z, z, y, z); }
	Vector4t<T> zzyw() const noexcept { return Vector4t<T>(z, z, y, w); }
	Vector4t<T> zzzx() const noexcept { return Vector4t<T>(z, z, z, x); }
	Vector4t<T> zzzy() const noexcept { return Vector4t<T>(z, z, z, y); }
	Vector4t<T> zzzz() const noexcept { return Vector4t<T>(z, z, z, z); }
	Vector4t<T> zzzw() const noexcept { return Vector4t<T>(z, z, z, w); }
	Vector4t<T> zzwx() const noexcept { return Vector4t<T>(z, z, w, x); }
	Vector4t<T> zzwy() const noexcept { return Vector4t<T>(z, z, w, y); }
	Vector4t<T> zzwz() const noexcept { return Vector4t<T>(z, z, w, z); }
	Vector4t<T> zzww() const noexcept { return Vector4t<T>(z, z, w, w); }
	Vector4t<T> zwxx() const noexcept { return Vector4t<T>(z, w, x, x); }
	Vector4t<T> zwxy() const noexcept { return Vector4t<T>(z, w, x, y); }
	Vector4t<T> zwxz() const noexcept { return Vector4t<T>(z, w, x, z); }
	Vector4t<T> zwxw() const noexcept { return Vector4t<T>(z, w, x, w); }
	Vector4t<T> zwyx() const noexcept { return Vector4t<T>(z, w, y, x); }
	Vector4t<T> zwyy() const noexcept { return Vector4t<T>(z, w, y, y); }
	Vector4t<T> zwyz() const noexcept { return Vector4t<T>(z, w, y, z); }
	Vector4t<T> zwyw() const noexcept { return Vector4t<T>(z, w, y, w); }
	Vector4t<T> zwzx() const noexcept { return Vector4t<T>(z, w, z, x); }
	Vector4t<T> zwzy() const noexcept { return Vector4t<T>(z, w, z, y); }
	Vector4t<T> zwzz() const noexcept { return Vector4t<T>(z, w, z, z); }
	Vector4t<T> zwzw() const noexcept { return Vector4t<T>(z, w, z, w); }
	Vector4t<T> zwwx() const noexcept { return Vector4t<T>(z, w, w, x); }
	Vector4t<T> zwwy() const noexcept { return Vector4t<T>(z, w, w, y); }
	Vector4t<T> zwwz() const noexcept { return Vector4t<T>(z, w, w, z); }
	Vector4t<T> zwww() const noexcept { return Vector4t<T>(z, w, w, w); }
	Vector4t<T> wxxx() const noexcept { return Vector4t<T>(w, x, x, x); }
	Vector4t<T> wxxy() const noexcept { return Vector4t<T>(w, x, x, y); }
	Vector4t<T> wxxz() const noexcept { return Vector4t<T>(w, x, x, z); }
	Vector4t<T> wxxw() const noexcept { return Vector4t<T>(w, x, x, w); }
	Vector4t<T> wxyx() const noexcept { return Vector4t<T>(w, x, y, x); }
	Vector4t<T> wxyy() const noexcept { return Vector4t<T>(w, x, y, y); }
	Vector4t<T> wxyz() const noexcept { return Vector4t<T>(w, x, y, z); }
	Vector4t<T> wxyw() const noexcept { return Vector4t<T>(w, x, y, w); }
	Vector4t<T> wxzx() const noexcept { return Vector4t<T>(w, x, z, x); }
	Vector4t<T> wxzy() const noexcept { return Vector4t<T>(w, x, z, y); }
	Vector4t<T> wxzz() const noexcept { return Vector4t<T>(w, x, z, z); }
	Vector4t<T> wxzw() const noexcept { return Vector4t<T>(w, x, z, w); }
	Vector4t<T> wxwx() const noexcept { return Vector4t<T>(w, x, w, x); }
	Vector4t<T> wxwy() const noexcept { return Vector4t<T>(w, x, w, y); }
	Vector4t<T> wxwz() const noexcept { return Vector4t<T>(w, x, w, z); }
	Vector4t<T> wxww() const noexcept { return Vector4t<T>(w, x, w, w); }
	Vector4t<T> wyxx() const noexcept { return Vector4t<T>(w, y, x, x); }
	Vector4t<T> wyxy() const noexcept { return Vector4t<T>(w, y, x, y); }
	Vector4t<T> wyxz() const noexcept { return Vector4t<T>(w, y, x, z); }
	Vector4t<T> wyxw() const noexcept { return Vector4t<T>(w, y, x, w); }
	Vector4t<T> wyyx() const noexcept { return Vector4t<T>(w, y, y, x); }
	Vector4t<T> wyyy() const noexcept { return Vector4t<T>(w, y, y, y); }
	Vector4t<T> wyyz() const noexcept { return Vector4t<T>(w, y, y, z); }
	Vector4t<T> wyyw() const noexcept { return Vector4t<T>(w, y, y, w); }
	Vector4t<T> wyzx() const noexcept { return Vector4t<T>(w, y, z, x); }
	Vector4t<T> wyzy() const noexcept { return Vector4t<T>(w, y, z, y); }
	Vector4t<T> wyzz() const noexcept { return Vector4t<T>(w, y, z, z); }
	Vector4t<T> wyzw() const noexcept { return Vector4t<T>(w, y, z, w); }
	Vector4t<T> wywx() const noexcept { return Vector4t<T>(w, y, w, x); }
	Vector4t<T> wywy() const noexcept { return Vector4t<T>(w, y, w, y); }
	Vector4t<T> wywz() const noexcept { return Vector4t<T>(w, y, w, z); }
	Vector4t<T> wyww() const noexcept { return Vector4t<T>(w, y, w, w); }
	Vector4t<T> wzxx() const noexcept { return Vector4t<T>(w, z, x, x); }
	Vector4t<T> wzxy() const noexcept { return Vector4t<T>(w, z, x, y); }
	Vector4t<T> wzxz() const noexcept { return Vector4t<T>(w, z, x, z); }
	Vector4t<T> wzxw() const noexcept { return Vector4t<T>(w, z, x, w); }
	Vector4t<T> wzyx() const noexcept { return Vector4t<T>(w, z, y, x); }
	Vector4t<T> wzyy() const noexcept { return Vector4t<T>(w, z, y, y); }
	Vector4t<T> wzyz() const noexcept { return Vector4t<T>(w, z, y, z); }
	Vector4t<T> wzyw() const noexcept { return Vector4t<T>(w, z, y, w); }
	Vector4t<T> wzzx() const noexcept { return Vector4t<T>(w, z, z, x); }
	Vector4t<T> wzzy() const noexcept { return Vector4t<T>(w, z, z, y); }
	Vector4t<T> wzzz() const noexcept { return Vector4t<T>(w, z, z, z); }
	Vector4t<T> wzzw() const noexcept { return Vector4t<T>(w, z, z, w); }
	Vector4t<T> wzwx() const noexcept { return Vector4t<T>(w, z, w, x); }
	Vector4t<T> wzwy() const noexcept { return Vector4t<T>(w, z, w, y); }
	Vector4t<T> wzwz() const noexcept { return Vector4t<T>(w, z, w, z); }
	Vector4t<T> wzww() const noexcept { return Vector4t<T>(w, z, w, w); }
	Vector4t<T> wwxx() const noexcept { return Vector4t<T>(w, w, x, x); }
	Vector4t<T> wwxy() const noexcept { return Vector4t<T>(w, w, x, y); }
	Vector4t<T> wwxz() const noexcept { return Vector4t<T>(w, w, x, z); }
	Vector4t<T> wwxw() const noexcept { return Vector4t<T>(w, w, x, w); }
	Vector4t<T> wwyx() const noexcept { return Vector4t<T>(w, w, y, x); }
	Vector4t<T> wwyy() const noexcept { return Vector4t<T>(w, w, y, y); }
	Vector4t<T> wwyz() const noexcept { return Vector4t<T>(w, w, y, z); }
	Vector4t<T> wwyw() const noexcept { return Vector4t<T>(w, w, y, w); }
	Vector4t<T> wwzx() const noexcept { return Vector4t<T>(w, w, z, x); }
	Vector4t<T> wwzy() const noexcept { return Vector4t<T>(w, w, z, y); }
	Vector4t<T> wwzz() const noexcept { return Vector4t<T>(w, w, z, z); }
	Vector4t<T> wwzw() const noexcept { return Vector4t<T>(w, w, z, w); }
	Vector4t<T> wwwx() const noexcept { return Vector4t<T>(w, w, w, x); }
	Vector4t<T> wwwy() const noexcept { return Vector4t<T>(w, w, w, y); }
	Vector4t<T> wwwz() const noexcept { return Vector4t<T>(w, w, w, z); }
	Vector4t<T> wwww() const noexcept { return Vector4t<T>(w, w, w, w); }

	Vector4t<T>& set(T xx, T yy, T zz) noexcept
	{
		x = xx; y = yy; z = zz;
		return *this;
	}

	Vector4t<T>& set(T xx, T yy, T zz, T ww) noexcept
	{
		x = xx; y = yy; z = zz; w = ww;
		return *this;
	}

	Vector4t<T>& set(const Vector2t<T>& v) noexcept
	{
		x = v.x; y = v.y;
		return *this;
	}

	 Vector4t<T>& set(const Vector3t<T>& v) noexcept
	{
		x = v.x; y = v.y; z = v.z;
		return *this;
	}

	template<typename S, typename = std::enable_if<std::is_pointer<S>::value>>
	Vector4t<T>& set(S xyzw[3]) noexcept
	{
		assert(xyzw[0] <= std::numeric_limits<typename trait::_typeaddition<S>::value_type>::max());
		assert(xyzw[1] <= std::numeric_limits<typename trait::_typeaddition<S>::value_type>::max());
		assert(xyzw[2] <= std::numeric_limits<typename trait::_typeaddition<S>::value_type>::max());
		assert(xyzw[3] <= std::numeric_limits<typename trait::_typeaddition<S>::value_type>::max());

		x = static_cast<typename trait::_typeaddition<S>::value_type>(xyzw[0]);
		y = static_cast<typename trait::_typeaddition<S>::value_type>(xyzw[1]);
		z = static_cast<typename trait::_typeaddition<S>::value_type>(xyzw[2]);
		w = static_cast<typename trait::_typeaddition<S>::value_type>(xyzw[3]);

		return *this;
	}

	T& getComponent(std::uint8_t index) noexcept
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
			return x;
		}
	}

	T* ptr() noexcept { return (T*)this; }
	const T* ptr() const noexcept { return (const T*)this; }
	T* data() noexcept { return (T*)this; }
	const T* data() const noexcept { return (const T*)this; }
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
	return !(v1 == v2);
}

template<typename T>
inline Vector4t<T> operator-(const Vector4t<T>& v) noexcept
{
	return Vector4t<T>(-v.x, -v.y, -v.z, -v.w);
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
inline Vector4t<T> operator*(const Vector4t<T>& v1, T scale) noexcept
{
	return Vector4t<T>(v1.x * scale, v1.y * scale, v1.z * scale, v1.w * scale);
}

template<typename T>
inline Vector4t<T> operator/(const Vector4t<T>& v1, T scale) noexcept
{
	return Vector4t<T>(v1.x / scale, v1.y / scale, v1.z / scale, v1.w / scale);
}

template<typename ostream, typename T, std::enable_if_t<trait::has_left_shift<ostream, T>::value, int> = 0>
inline ostream& operator << (ostream& os, const Vector4t<T>& v)
{
	os << v.x << ", " << v.y << ", " << v.z << ", " << v.w;
	return os;
}

template<typename istream, typename T, std::enable_if_t<trait::has_right_shift<istream>::value, int> = 0>
inline istream& operator >> (istream& is, Vector4t<T>& v)
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
	inline T dot(const Vector4t<T>& v1, const Vector4t<T>& v2) noexcept
	{
		return v1.x * v2.x + v1.y * v2.y + v1.z * v2.z + v1.w * v2.w;
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
	inline T distance(const Vector4t<T>& v1, const Vector4t<T>& v2) noexcept
	{
		return length(v1 - v2);
	}

	template<typename T>
	inline T sqrDistance(const Vector4t<T>& v1, const Vector4t<T>& v2) noexcept
	{
		return length2(v1 - v2);
	}

	template<typename T>
	inline bool equal(const Vector4t<T>& v1, const Vector4t<T>& v2) noexcept
	{
		return
			math::equal<T>(v1.x, v2.x) &&
			math::equal<T>(v1.y, v2.y) &&
			math::equal<T>(v1.z, v2.z) &&
			math::equal<T>(v1.w, v2.w);
	}

	template<typename T>
	inline bool isfinite(const Vector4t<T>& v) noexcept
	{
		return std::isfinite(v.x) && std::isfinite(v.y) && std::isfinite(v.z) && std::isfinite(v.w);
	}

	template<typename T>
	inline Vector4t<T> cross(const Vector4t<T>& v1, const Vector4t<T>& v2) noexcept
	{
		return Vector4t<T>(
			v1.w * v2.x + v1.x * v2.w + v1.z * v2.y - v1.y * v2.z,
			v1.w * v2.y + v1.y * v2.w + v1.x * v2.z - v1.z * v2.x,
			v1.w * v2.z + v1.z * v2.w + v1.y * v2.x - v1.x * v2.y,
			v1.w * v2.w - v1.x * v2.x - v1.y * v2.y - v1.z * v2.z);
	}

	template<typename T>
	inline Vector4t<T> negate(const Vector4t<T>& v) noexcept
	{
		return Vector4t<T>(-v.x, -v.y, -v.z, -v.w);
	}

	template<typename T>
	inline Vector4t<T> clamp(const Vector4t<T>& t, T min, T max) noexcept
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
		return clamp(v, T(0.0f), T(1.0f));
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

	template<typename T>
	inline Vector4t<T> snorm2unorm(const Vector4t<T>& v) noexcept
	{
		return Vector4t<T>(
			_mathutil<T>::snorm2unorm(v.x),
			_mathutil<T>::snorm2unorm(v.y),
			_mathutil<T>::snorm2unorm(v.z),
			_mathutil<T>::snorm2unorm(v.w)
			);
	}

	template<typename T>
	inline Vector4t<T> unorm2snorm(const Vector4t<T>& v) noexcept
	{
		return Vector4t<T>(
			_mathutil<T>::unorm2snorm(v.x),
			_mathutil<T>::unorm2snorm(v.y),
			_mathutil<T>::unorm2snorm(v.z),
			_mathutil<T>::unorm2snorm(v.w)
			);
	}

	template <typename T>
	inline Vector4t<T> min(const Vector4t<T>& a, const Vector4t<T>& b) noexcept
	{
		return Vector4t<T>(std::min(a.x, b.x), std::min(a.y, b.y), std::min(a.z, b.z), std::min(a.w, b.w));
	}

	template <typename T>
	inline Vector4t<T> max(const Vector4t<T>& a, const Vector4t<T>& b) noexcept
	{
		return Vector4t<T>(std::max(a.x, b.x), std::max(a.y, b.y), std::max(a.z, b.z), std::max(a.w, b.w));
	}

	template<typename T>
	inline Vector4t<T> abs(const Vector4t<T>& v) noexcept
	{
		return Vector4t<T>(std::abs(v.x), std::abs(v.y), std::abs(v.z), std::abs(v.w));
	}

	template<typename T>
	inline Vector4t<T> cos(const Vector4t<T>& v) noexcept
	{
		return Vector4t<T>(std::cos(v.x), std::cos(v.y), std::cos(v.z), std::cos(v.w));
	}

	template<typename T>
	inline Vector4t<T> sin(const Vector4t<T>& v) noexcept
	{
		return Vector4t<T>(std::sin(v.x), std::sin(v.y), std::sin(v.z), std::sin(v.w));
	}

	template<typename T>
	inline Vector4t<T> tan(const Vector4t<T>& v) noexcept
	{
		return Vector4t<T>(std::tan(v.x), std::tan(v.y), std::tan(v.z), std::tan(v.w));
	}

	template<typename T>
	inline Vector4t<T> acos(const Vector4t<T>& v) noexcept
	{
		return Vector4t<T>(std::acos(v.x), std::acos(v.y), std::acos(v.z), std::acos(v.w));
	}

	template<typename T>
	inline Vector4t<T> asin(const Vector4t<T>& v) noexcept
	{
		return Vector4t<T>(std::asin(v.x), std::asin(v.y), std::asin(v.z), std::asin(v.w));
	}

	template<typename T>
	inline Vector4t<T> atan(const Vector4t<T>& v) noexcept
	{
		return Vector4t<T>(std::atan(v.x), std::atan(v.y), std::atan(v.z), std::atan(v.w));
	}

	template<typename T>
	inline Vector4t<T> exp(const Vector4t<T>& v) noexcept
	{
		return Vector4t<T>(std::exp(v.x), std::exp(v.y), std::exp(v.z), std::exp(v.w));
	}

	template<typename T>
	inline Vector4t<T> exp2(const Vector4t<T>& v) noexcept
	{
		return Vector4t<T>(std::exp2(v.x), std::exp2(v.y), std::exp2(v.z), std::exp2(v.w));
	}

	template<typename T>
	inline Vector4t<T> log(const Vector4t<T>& v) noexcept
	{
		return Vector4t<T>(std::log(v.x), std::log(v.y), std::log(v.z), std::log(v.w));
	}

	template<typename T>
	inline Vector4t<T> log2(const Vector4t<T>& v) noexcept
	{
		return Vector4t<T>(std::log2(v.x), std::log2(v.y), std::log2(v.z), std::log2(v.w));
	}

	template<typename T>
	inline Vector4t<T> log10(const Vector4t<T>& v) noexcept
	{
		return Vector4t<T>(std::log10(v.x), std::log10(v.y), std::log10(v.z), std::log10(v.w));
	}

	template<typename T>
	inline Vector4t<T> sqrt(const Vector4t<T>& v) noexcept
	{
		return Vector4t<T>(std::sqrt(v.x), std::sqrt(v.y), std::sqrt(v.z), std::sqrt(v.w));
	}

	template<typename T>
	inline Vector4t<T> pow(const Vector4t<T>& v1, const Vector4t<T>& v2) noexcept
	{
		return Vector4t<T>(std::pow(v1.x, v2.x), std::pow(v1.y, v2.y), std::pow(v1.z, v2.z), std::pow(v1.w, v2.w));
	}

	template<typename T>
	inline Vector4t<T> random(const Vector4t<T>& min, const Vector4t<T>& max) noexcept
	{
		return Vector4t<T>(random(min.x, max.x), random(min.y, max.y), random(min.z, max.z), random(min.w, max.w));
	}

	template<typename T>
	inline Vector4t<T> ceil(const Vector4t<T>& v) noexcept
	{
		return Vector4t<T>(std::ceil(v.x), std::ceil(v.y), std::ceil(v.z), std::ceil(v.w));
	}

	template<typename T>
	inline Vector4t<T> floor(const Vector4t<T>& v) noexcept
	{
		return Vector4t<T>(std::floor(v.x), std::floor(v.y), std::floor(v.z), std::floor(v.w));
	}
}

_NAME_END

#endif