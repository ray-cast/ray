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
#ifndef _H_EULER_H_
#define _H_EULER_H_

#include <ray/mathfwd.h>

_NAME_BEGIN

template<typename T>
class EulerAnglest
{
public:
	T x, y, z;

	EulerAnglest() noexcept {};
	EulerAnglest(T xx, T yy, T zz) noexcept : x(xx), y(yy), z(zz) {}
	EulerAnglest(const EulerAnglest<T>& e) noexcept : x(e.x), y(e.y), z(e.z) {}
	EulerAnglest(const Vector3t<T>& v) noexcept : x(v.x), y(v.y), z(v.z) {}
	EulerAnglest(const Quaterniont<T>& q) noexcept { this->makeRotate(q); }

	EulerAnglest<T>& operator+=(const EulerAnglest<T>& e) noexcept { x += e.x; y += e.y; z += e.z; return *this; }
	EulerAnglest<T>& operator-=(const EulerAnglest<T>& e) noexcept { x -= e.x; y -= e.y; z -= e.z; return *this; }
	EulerAnglest<T>& operator*=(const EulerAnglest<T>& e) noexcept { x *= e.x; y *= e.y; z *= e.z; return *this; }
	EulerAnglest<T>& operator/=(const EulerAnglest<T>& e) noexcept { x /= e.x; y /= e.y; z /= e.z; return *this; }

	T operator[](std::uint8_t i) const noexcept
	{
		assert(i < 3);
		return *(&x + i);
	}

	T& operator[](std::uint8_t i) noexcept
	{
		assert(i < 3);
		return *(&x + i);
	}

	explicit operator Vector3t<T>() const noexcept
	{
		return Vector3t<T>(x, y, z);
	}

	EulerAnglest<T>& identity() noexcept
	{
		x = z = y = 0.0f;
	}

	EulerAnglest<T>& makeRotate(const Quaterniont<T>& q) noexcept
	{
		this->x = RAD_TO_DEG(asin(2.0f * (q.w * q.x - q.y * q.z)));
		this->y = RAD_TO_DEG(atan2(q.w * q.y + q.x * q.z, 1.0f - 2.0f * (q.x * q.x + q.y * q.y)));
		this->z = RAD_TO_DEG(atan2(q.w * q.z + q.x * q.y, 1.0f - 2.0f * (q.x * q.x + q.z * q.z)));
		return *this;
	}
};

template<typename T>
inline bool operator==(const EulerAnglest<T>& e1, const EulerAnglest<T>& e2) noexcept
{
	return e1.x == e2.x && e1.y == e2.y && e1.z == e2.z;
}

template<typename T>
inline bool operator!=(const EulerAnglest<T>& e1, const EulerAnglest<T>& e2) noexcept
{
	return e1.x != e2.x || e1.y != e2.y || e1.z != e2.z;
}

template<typename T>
inline EulerAnglest<T> operator+(const EulerAnglest<T>& v1, const EulerAnglest<T>& v2) noexcept
{
	return EulerAnglest<T>(v1.x + v2.x, v1.y + v2.y, v1.z + v2.z);
}

template<typename T>
inline EulerAnglest<T> operator-(const EulerAnglest<T>& v1, const EulerAnglest<T>& v2) noexcept
{
	return EulerAnglest<T>(v1.x - v2.x, v1.y - v2.y, v1.z - v2.z);
}

template<typename T>
inline EulerAnglest<T> operator*(const EulerAnglest<T>& v1, const EulerAnglest<T>& v2) noexcept
{
	return EulerAnglest<T>(v1.x * v2.x, v1.y * v2.y, v1.z * v2.z);
}

template<typename T>
inline EulerAnglest<T> operator/(const EulerAnglest<T>& v1, const EulerAnglest<T>& v2) noexcept
{
	return EulerAnglest<T>(v1.x / v2.x, v1.y / v2.y, v1.z / v2.z);
}

template<typename ostream, typename T, std::enable_if_t<trait::has_left_shift<ostream, T>::value, int> = 0>
inline ostream& operator << (ostream& os, const EulerAnglest<T>& v) noexcept
{
	os << v.x << ", " << v.y << ", " << v.z;
	return os;
}

template<typename istream, typename T, std::enable_if_t<trait::has_right_shift<istream>::value, int> = 0>
inline istream& operator >> (istream& is, EulerAnglest<T>& v) noexcept
{
	is >> v.x;
	is.ignore(2);
	is >> v.y;
	is.ignore(2);
	is >> v.z;
	return is;
}

_NAME_END

#endif
