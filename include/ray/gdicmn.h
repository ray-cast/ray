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
#ifndef _H_GDICMN_H_
#define _H_GDICMN_H_

#include <ray/mathfwd.h>

_NAME_BEGIN

template<typename T>
class Sizet
{
public:
	typedef T size_type;
	typedef Sizet<T> _Myt;

	size_type x, y;

	Sizet() noexcept : x(0), y(0) { }
	Sizet(int xx, int yy) noexcept : x(xx), y(yy) { }

	_Myt& operator+=(const _Myt& sz) noexcept { x += sz.x; y += sz.y; return *this; }
	_Myt& operator-=(const _Myt& sz) noexcept { x -= sz.x; y -= sz.y; return *this; }
	_Myt& operator/=(int i) noexcept { x /= i; y /= i; return *this; }
	_Myt& operator*=(int i) noexcept { x *= i; y *= i; return *this; }
	_Myt& operator/=(unsigned int i) noexcept { x /= i; y /= i; return *this; }
	_Myt& operator*=(unsigned int i) noexcept { x *= i; y *= i; return *this; }
	_Myt& operator/=(long i) noexcept { x /= i; y /= i; return *this; }
	_Myt& operator*=(long i) noexcept { x *= i; y *= i; return *this; }
	_Myt& operator/=(unsigned long i) noexcept { x /= i; y /= i; return *this; }
	_Myt& operator*=(unsigned long i) noexcept { x *= i; y *= i; return *this; }
	_Myt& operator/=(double i) noexcept { x = int(x / i); y = int(y / i); return *this; }
	_Myt& operator*=(double i) noexcept { x = int(x*i); y = int(y*i); return *this; }

	void set(int xx, int yy) { x = xx; y = yy; }
};

template<typename T>
inline bool operator==(const Sizet<T>& s1, const Sizet<T>& s2) noexcept
{
	return s1.x == s2.x && s1.y == s2.y;
}

template<typename T>
inline bool operator!=(const Sizet<T>& s1, const Sizet<T>& s2) noexcept
{
	return s1.x != s2.x || s1.y != s2.y;
}

template<typename T>
inline Sizet<T> operator+(const Sizet<T>& s1, const Sizet<T>& s2) noexcept
{
	return Sizet<T>(s1.x + s2.x, s1.y + s2.y);
}

template<typename T>
inline Sizet<T> operator-(const Sizet<T>& s1, const Sizet<T>& s2) noexcept
{
	return Sizet<T>(s1.x - s2.x, s1.y - s2.y);
}

template<typename T>
inline Sizet<T> operator/(const Sizet<T>& s, int i) noexcept
{
	return Sizet<T>(s.x / i, s.y / i);
}

template<typename T>
inline Sizet<T> operator*(const Sizet<T>& s, int i) noexcept
{
	return Sizet<T>(s.x * i, s.y * i);
}

template<typename T>
inline Sizet<T> operator*(int i, const Sizet<T>& s) noexcept
{
	return Sizet<T>(s.x * i, s.y * i);
}

template<typename T>
inline Sizet<T> operator/(const Sizet<T>& s, unsigned int i) noexcept
{
	return Sizet<T>(s.x / i, s.y / i);
}

template<typename T>
inline Sizet<T> operator*(const Sizet<T>& s, unsigned int i) noexcept
{
	return Sizet<T>(s.x * i, s.y * i);
}

template<typename T>
inline Sizet<T> operator*(unsigned int i, const Sizet<T>& s) noexcept
{
	return Sizet<T>(s.x * i, s.y * i);
}

template<typename T>
inline Sizet<T> operator/(const Sizet<T>& s, long i) noexcept
{
	return Sizet<T>(s.x / i, s.y / i);
}

template<typename T>
inline Sizet<T> operator*(const Sizet<T>& s, long i) noexcept
{
	return Sizet<T>(s.x * i, s.y * i);
}

template<typename T>
inline Sizet<T> operator*(long i, const Sizet<T>& s) noexcept
{
	return Sizet<T>(s.x * i, s.y * i);
}

template<typename T>
inline Sizet<T> operator/(const Sizet<T>& s, unsigned long i) noexcept
{
	return Sizet<T>(s.x / i, s.y / i);
}

template<typename T>
inline Sizet<T> operator*(const Sizet<T>& s, unsigned long i) noexcept
{
	return Sizet<T>(s.x * i, s.y * i);
}

template<typename T>
inline Sizet<T> operator*(unsigned long i, const Sizet<T>& s) noexcept
{
	return Sizet<T>(s.x * i, s.y * i);
}

template<typename T>
inline Sizet<T> operator*(const Sizet<T>& s, double i) noexcept
{
	return Sizet<T>(int(s.x * i), int(s.y * i));
}

template<typename T>
inline Sizet<T> operator*(double i, const Sizet<T>& s) noexcept
{
	return Sizet<T>(int(s.x * i), int(s.y * i));
}

template<typename T>
class Pointt
{
public:
	typedef T size_type;
	typedef T type_type;
	typedef Pointt<T> _Myt;
	typedef Sizet<T> _Mysize;

	size_type x, y;

	Pointt()  noexcept : x(0), y(0) { }
	Pointt(size_type xx, size_type yy) noexcept : x(xx), y(yy) { }

	// no copy ctor or assignment operator - the defaults are ok

	//assignment operators
	_Myt& operator+=(const _Myt& p) noexcept { x += p.x; y += p.y; return *this; }
	_Myt& operator-=(const _Myt& p) noexcept { x -= p.x; y -= p.y; return *this; }
	_Myt& operator*=(const _Myt& p) noexcept { x *= p.x; y *= p.y; return *this; }
	_Myt& operator/=(const _Myt& p) noexcept { x /= p.x; y /= p.y; return *this; }

	_Myt& operator+=(const _Mysize& s) noexcept { x += s.x; y += s.y; return *this; }
	_Myt& operator-=(const _Mysize& s) noexcept { x -= s.x; y -= s.y; return *this; }
	_Myt& operator*=(const _Mysize& s) noexcept { x *= s.x; y *= s.y; return *this; }
	_Myt& operator/=(const _Mysize& s) noexcept { x /= s.x; y /= s.y; return *this; }

	_Myt& operator*=(int scale) noexcept { x *= scale; y *= scale; return *this; }
	_Myt& operator/=(int scale) noexcept { x /= scale; y /= scale; return *this; }

	void set(int val) noexcept { x = y = val; }
	void set(int xx, int yy) noexcept { x = xx; y = yy; }

	_Myt negate() const noexcept { return _Myt(-x, -y); }

	int* ptr() noexcept { return (int*)this; }
	const int* ptr() const noexcept { return (const int*)this; }
	int* data() noexcept { return (int*)this; }
	const int* data() const noexcept { return (const int*)this; }
};

template<typename T>
inline bool operator==(const Pointt<T>& p1, const Pointt<T>& p2) noexcept
{
	return p1.x == p2.x && p1.y == p2.y;
}

template<typename T>
inline bool operator!=(const Pointt<T>& p1, const Pointt<T>& p2) noexcept
{
	return !(p1 == p2);
}

template<typename T>
inline Pointt<T> operator+(const Pointt<T>& p1, const Pointt<T>& p2) noexcept
{
	return Pointt<T>(p1.x + p2.x, p1.y + p2.y);
}

template<typename T>
inline Pointt<T> operator-(const Pointt<T>& p1, const Pointt<T>& p2) noexcept
{
	return Pointt<T>(p1.x - p2.x, p1.y - p2.y);
}

template<typename T>
inline Pointt<T> operator+(const Pointt<T>& p, const Sizet<T>& s) noexcept
{
	return Pointt<T>(p.x + s.x, p.y + s.y);
}

template<typename T>
inline Pointt<T> operator-(const Pointt<T>& p, const Sizet<T>& s) noexcept
{
	return Pointt<T>(p.x - s.x, p.y - s.y);
}

template<typename T>
inline Pointt<T> operator+(const Sizet<T>& s, const Pointt<T>& p) noexcept
{
	return Pointt<T>(p.x + s.x, p.y + s.y);
}

template<typename T>
inline Pointt<T> operator-(const Sizet<T>& s, const Pointt<T>& p) noexcept
{
	return Pointt<T>(s.x - p.x, s.y - p.y);
}

template<typename T>
inline Pointt<T> operator-(const Pointt<T>& p) noexcept
{
	return Pointt<T>(-p.x, -p.y);
}

template<typename T>
inline Pointt<T> operator/(const Pointt<T>& s, int i) noexcept
{
	return Pointt<T>(s.x / i, s.y / i);
}

template<typename T>
inline Pointt<T> operator*(const Pointt<T>& s, int i) noexcept
{
	return Pointt<T>(s.x * i, s.y * i);
}

template<typename T>
inline Pointt<T> operator*(int i, const Pointt<T>& s) noexcept
{
	return Pointt<T>(s.x * i, s.y * i);
}

template<typename T>
inline Pointt<T> operator/(const Pointt<T>& s, unsigned int i) noexcept
{
	return Pointt<T>(s.x / i, s.y / i);
}

template<typename T>
inline Pointt<T> operator*(const Pointt<T>& s, unsigned int i) noexcept
{
	return Pointt<T>(s.x * i, s.y * i);
}

template<typename T>
inline Pointt<T> operator*(unsigned int i, const Pointt<T>& s) noexcept
{
	return Pointt<T>(s.x * i, s.y * i);
}

template<typename T>
inline Pointt<T> operator/(const Pointt<T>& s, long i) noexcept
{
	return Pointt<T>(s.x / i, s.y / i);
}

template<typename T>
inline Pointt<T> operator*(const Pointt<T>& s, long i) noexcept
{
	return Pointt<T>(s.x * i, s.y * i);
}

template<typename T>
inline Pointt<T> operator*(long i, const Pointt<T>& s) noexcept
{
	return Pointt<T>(s.x * i, s.y * i);
}

template<typename T>
inline Pointt<T> operator/(const Pointt<T>& s, unsigned long i) noexcept
{
	return Pointt<T>(s.x / i, s.y / i);
}

template<typename T>
inline Pointt<T> operator*(const Pointt<T>& s, unsigned long i) noexcept
{
	return Pointt<T>(s.x * i, s.y * i);
}

template<typename T>
inline Pointt<T> operator*(unsigned long i, const Pointt<T>& s) noexcept
{
	return Pointt<T>(s.x * i, s.y * i);
}

template<typename T>
inline Pointt<T> operator*(const Pointt<T>& s, double i) noexcept
{
	return Pointt<T>(int(s.x * i), int(s.y * i));
}

template<typename T>
inline Pointt<T> operator*(double i, const Pointt<T>& s) noexcept
{
	return Pointt<T>(int(s.x * i), int(s.y * i));
}

template<typename T>
class Rectt
{
public:
	typedef T size_type;
	typedef Rectt<T> _Myt;
	typedef Pointt<T> _Mypoint;
	typedef Sizet<T> _Mysize;

	size_type x, y, w, h;

	Rectt() noexcept : x(0), y(0), w(0), h(0) { }
	Rectt(size_type xx, size_type yy, size_type ww, size_type hh) noexcept : x(xx), y(yy), w(ww), h(hh) { }
	Rectt(const _Mypoint& topLeft, const _Mypoint& bottomRight) noexcept : x(topLeft.x), y(topLeft.y), w(topLeft.x + bottomRight.x), h(topLeft.y + bottomRight.y) {}
	Rectt(const _Mypoint& pt, const _Mysize& size) noexcept : x(pt.x), y(pt.y), w(size.x), h(size.y) { }
	explicit Rectt(const _Mysize& size) noexcept : x(0), y(0), w(size.x), h(size.y) { }

	_Myt& operator+=(const _Myt& rt) noexcept { x += rt.x; y += rt.y; w += rt.w; h += rt.h; return *this; }
	_Myt& operator-=(const _Myt& rt) noexcept { x -= rt.x; y -= rt.y; w -= rt.w; h -= rt.h; return *this; }
	_Myt& operator/=(const _Myt& rt) noexcept { x *= rt.x; y *= rt.y; w *= rt.w; h *= rt.h; return *this; }
	_Myt& operator*=(const _Myt& rt) noexcept { x /= rt.x; y /= rt.y; w /= rt.w; h /= rt.h; return *this; }

	void set(int xx, int yy, int ww, int hh) noexcept { x = xx; y = yy; w = ww; h = hh; }

	int* ptr() noexcept { return (int*)this; }
	const int* ptr() const noexcept { return (const int*)this; }
	int* data() noexcept { return (int*)this; }
	const int* data() const noexcept { return (const int*)this; }

	Pointt<T> xy() noexcept { return Pointt<T>(x, y); }
};

template<typename T>
inline bool operator==(const Rectt<T>& r1, const Rectt<T>& r2) noexcept
{
	return (r1.x == r2.x) && (r1.y == r2.y) && (r1.w == r2.w) && (r1.h == r2.h);
}

template<typename T>
inline bool operator!=(const Rectt<T>& r1, const Rectt<T>& r2) noexcept
{
	return !(r1 == r2);
}

template<typename T>
inline Rectt<T> operator+(const Rectt<T>& r1, const Rectt<T>& r2) noexcept
{
	return Rectt<T>(r1.x + r2.x, r1.y + r2.y, r1.w + r2.w, r1.h + r2.h);
}

template<typename T>
inline Rectt<T> operator-(const Rectt<T>& r1, const Rectt<T>& r2) noexcept
{
	return Rectt<T>(r1.x - r2.x, r1.y - r2.y, r1.w - r2.w, r1.h - r2.h);
}

template<typename T>
inline Rectt<T> operator/(const Rectt<T>& r1, const Rectt<T>& r2) noexcept
{
	return Rectt<T>(r1.x / r2.x, r1.y / r2.y, r1.w / r2.w, r1.h / r2.h);
}

template<typename T>
inline Rectt<T> operator*(const Rectt<T>& r1, const Rectt<T>& r2) noexcept
{
	return Rectt<T>(r1.x * r2.x, r1.y * r2.y, r1.w * r2.w, r1.h * r2.h);
}

template<typename T>
inline Rectt<T> operator*(const Rectt<T>& r1, int i) noexcept
{
	return Rectt<T>(r1.x * i, r1.y * i, r1.w * i, r1.h * i);
}

template<typename T>
inline Rectt<T> operator/(const Rectt<T>& r1, int i) noexcept
{
	return Rectt<T>(r1.x / i, r1.y / i, r1.w / i, r1.h / i);
}

template<typename T>
inline Rectt<T> operator*(int i, const Rectt<T>& r1) noexcept
{
	return Rectt<T>(r1.x * i, r1.y * i, r1.w * i, r1.h * i);
}

template<typename T>
inline Rectt<T> operator/(int i, const Rectt<T>& r1) noexcept
{
	return Rectt<T>(r1.x / i, r1.y / i, r1.w / i, r1.h / i);
}

template<typename T>
class Viewportt
{
public:
	typedef T size_type;
	typedef Viewportt<T> _Myt;

	size_type left, top, width, height, minDepth, maxDepth;

	Viewportt() noexcept
	{}

	Viewportt(const Pointt<T>& pt, const Sizet<T>& sz) noexcept
		: left(pt.x)
		, top(pt.y)
		, width(sz.x)
		, height(sz.y)
		, minDepth(0.0f)
		, maxDepth(1.0f)
	{}

	Viewportt(size_type _left, size_type _top, size_type _width, size_type _height) noexcept
		: left(_left)
		, top(_top)
		, width(_width)
		, height(_height)
		, minDepth(0.0f)
		, maxDepth(1.0f)
	{}

	Viewportt(size_type _left, size_type _top, size_type _width, size_type _height, size_type _minDepth, size_type _maxDepth) noexcept
		: left(_left)
		, top(_top)
		, width(_width)
		, height(_height)
		, minDepth(_minDepth)
		, maxDepth(_maxDepth)
	{}

	size_type& operator[](int n) noexcept { return ((size_type*)this)[n]; }
	const size_type& operator[](int n) const noexcept { return ((size_type*)this)[n]; }

	const T* ptr() const noexcept
	{
		return &left;
	}

	T* ptr() noexcept
	{
		return &left;
	}
};

template<typename T>
bool operator==(const Viewportt<T>& v1, const Viewportt<T>& v2) noexcept
{
	return v1.left == v2.left && v1.top == v2.top && v1.width == v2.width && v1.height == v2.height && v1.minDepth == v2.minDepth && v1.maxDepth == v2.maxDepth;
}

template<typename T>
bool operator!=(const Viewportt<T>& v1, const Viewportt<T>& v2) noexcept
{
	return !(v1 == v2);
}

template<typename T>
class Scissort
{
public:
	typedef T size_type;
	typedef Scissort<T> _Myt;

	size_type left, top, width, height;

	Scissort() noexcept
	{}

	Scissort(const Pointt<T>& pt, const Sizet<T>& sz) noexcept
		: left(pt.x)
		, top(pt.y)
		, width(sz.x)
		, height(sz.y)
	{}

	Scissort(size_type _left, size_type _top, size_type _width, size_type _height) noexcept
		: left(_left)
		, top(_top)
		, width(_width)
		, height(_height)
	{}

	size_type& operator[](int n) noexcept { return ((size_type*)this)[n]; }
	const size_type& operator[](int n) const noexcept { return ((size_type*)this)[n]; }

	const T* ptr() const noexcept
	{
		return &left;
	}

	T* ptr() noexcept
	{
		return &left;
	}
};

template<typename T>
bool operator==(const Scissort<T>& v1, const Scissort<T>& v2) noexcept
{
	return v1.left == v2.left && v1.top == v2.top && v1.width == v2.width && v1.height == v2.height;
}

template<typename T>
bool operator!=(const Scissort<T>& v1, const Scissort<T>& v2) noexcept
{
	return !(v1 == v2);
}

_NAME_END

#endif