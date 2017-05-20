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
#ifndef _H_HEMISPHERE_WEIGHT_H_
#define _H_HEMISPHERE_WEIGHT_H_

#include <ray/math.h>

_NAME_BEGIN

template<typename T>
class HemisphereWeight
{
public:
	typedef typename trait::_typeaddition<T>::value_type value_type;
	typedef typename trait::_typeaddition<T>::pointer pointer;
	typedef typename trait::_typeaddition<T>::const_pointer const_pointer;
	typedef typename trait::_typeaddition<T>::reference reference;
	typedef typename trait::_typeaddition<T>::const_reference const_reference;

	T front;
	T back;
	T left;
	T right;
	T up;
	T down;

	HemisphereWeight() noexcept {}
	HemisphereWeight(float c1, float c2, float l, float r, float u, float b) noexcept : front(c1), back(c2), left(l), right(r), up(u), down(b) {}
	HemisphereWeight(const Vector2t<T>& c, const Vector2t<T>& lr, const Vector2t<T>& ub) noexcept : front(c.x), back(c.y), left(lr.x), right(lr.y), up(ub.x), down(ub.y) {}
	HemisphereWeight(const Vector3t<T>& c, const Vector3t<T>& rub) noexcept : front(c.x), back(c.y), left(c.z), right(rub.x), up(rub.y), down(rub.z) {}
	HemisphereWeight(const Vector2t<T>& c, const Vector4t<T>& lrub) noexcept : front(c.x), back(c.y), left(lrub.x), right(lrub.y), up(lrub.z), down(lrub.w) {}
	HemisphereWeight(const Vector4t<T>& clr, const Vector2t<T>& ub) noexcept : front(clr.x), back(clr.y), left(clr.z), right(clr.w), up(ub.x), down(ub.y) {}
	explicit HemisphereWeight(float w) noexcept : front(w), back(w), left(w), right(w), up(w), down(w) {}

#pragma warning(push)
#pragma warning(disable : 4244) // float multiply with double
	template<typename S, typename = std::enable_if_t<std::is_same<S, T>::value || !std::is_same<S, T>::value && std::is_floating_point<S>::value>>
	HemisphereWeight<T>& operator+=(const S sz) { front += sz; back += sz; left += sz; right += sz; up += sz; down += sz; return *this; }
	template<typename S, typename = std::enable_if_t<std::is_same<S, T>::value || !std::is_same<S, T>::value && std::is_floating_point<S>::value>>
	HemisphereWeight<T>& operator-=(const S sz) { front -= sz; back -= sz; left -= sz; right -= sz; up -= sz; down -= sz; return *this; }
	template<typename S, typename = std::enable_if_t<std::is_same<S, T>::value || !std::is_same<S, T>::value && std::is_floating_point<S>::value>>
	HemisphereWeight<T>& operator*=(const S sz) { front *= sz; back *= sz; left *= sz; right *= sz; up *= sz; down *= sz; return *this; }
	template<typename S, typename = std::enable_if_t<std::is_same<S, T>::value || !std::is_same<S, T>::value && std::is_floating_point<S>::value>>
	HemisphereWeight<T>& operator/=(const S sz) { front /= sz; back /= sz; left /= sz; right /= sz; up /= sz; down /= sz; return *this; }
#pragma warning(pop)

	HemisphereWeight<T>& operator+=(const HemisphereWeight<T>& w) { front += w.front; back += w.back; left += w.left; right += w.right; up += w.up; down += w.down; return *this; }
	HemisphereWeight<T>& operator-=(const HemisphereWeight<T>& w) { front -= w.front; back -= w.back; left -= w.left; right -= w.right; up -= w.up; down -= w.down; return *this; }
	HemisphereWeight<T>& operator*=(const HemisphereWeight<T>& w) { front *= w.front; back *= w.back; left *= w.left; right *= w.right; up *= w.up; down *= w.down; return *this; }
	HemisphereWeight<T>& operator/=(const HemisphereWeight<T>& w) { front /= w.front; back /= w.back; left /= w.left; right /= w.right; up /= w.up; down /= w.down; return *this; }

	template <typename S>
	explicit operator HemisphereWeight<S>() const
	{
		return HemisphereWeight<S>(
			HemisphereWeight<S>(front), 
			HemisphereWeight<S>(back), 
			HemisphereWeight<S>(left),
			HemisphereWeight<S>(right),
			HemisphereWeight<S>(up),
			HemisphereWeight<S>(down));
	}
	
	T operator[](std::size_t i) const
	{
		return *(&front + i);
	}

	T& operator[](std::size_t i)
	{
		return *(&front + i);
	}
	
	T* ptr() { return (T*)this; }
	const T* ptr() const { return (const T*)this; }
	T* data() { return (T*)this; }
	const T* data() const { return (const T*)this; }
};

template<typename T>
HemisphereWeight<T> operator+=(const HemisphereWeight<T>& w1, const HemisphereWeight<T>& w2)
{ 
	return HemisphereWeight<T>(w1.front += w2.front, w1.back += w2.back, w1.left += w2.left, w1.right += w2.right, w1.up += w2.up, w1.down += w2.down);
}
template<typename T>
HemisphereWeight<T> operator-=(const HemisphereWeight<T>& w1, const HemisphereWeight<T>& w2)
{ 
	return HemisphereWeight<T>(w1.front -= w2.front, w1.back -= w2.back, w1.left -= w2.left, w1.right -= w2.right, w1.up -= w2.up, w1.down -= w2.down);
}
template<typename T>
HemisphereWeight<T> operator*=(const HemisphereWeight<T>& w1, const HemisphereWeight<T>& w2)
{ 
	return HemisphereWeight<T>(w1.front *= w2.front, w1.back *= w2.back, w1.left *= w2.left, w1.right *= w2.right, w1.up *= w2.up, w1.down *= w2.down);
}
template<typename T>
HemisphereWeight<T> operator/=(const HemisphereWeight<T>& w1, const HemisphereWeight<T>& w2)
{ 
	return HemisphereWeight<T>(w1.front /= w2.front, w1.back /= w2.back, w1.left /= w2.left, w1.right /= w2.right, w1.up /= w2.up, w1.down /= w2.down);
}

template<typename T>
inline HemisphereWeight<T> operator-(const HemisphereWeight<T>& w)
{
	return HemisphereWeight<T>(-w.front, -w.back, -w.left, -w.right, -w.up, -w.down);
}

namespace math
{
	template<typename T>
	void makeHemisphereWeights(HemisphereWeight<T> weights[], std::uint32_t size)
	{
		double sum = 0.0;
		double center = (size - 1) * 0.5f;

		for (std::uint32_t y = 0; y < size; y++)
		{
			for (std::uint32_t x = 0; x < size; x++)
			{
				const double u = 2.0f * (static_cast<double>(x) - center) / static_cast<double>(size);
				const double v = 2.0f * (static_cast<double>(y) - center) / static_cast<double>(size);

				const double temp = 1.0 + u * u + v * v;
				const double weight = 1.0 / std::sqrt(temp);

				const double solidAngle = weight * weight * weight;

				weights[size * y + x] = HemisphereWeight<T>(static_cast<T>(solidAngle));

				sum += solidAngle;
			}
		}

		const double weightScale = 1.0 / (sum * 3.0);

		for (std::size_t i = 0; i < size * size; i++)
			weights[i] *= weightScale;		
	}
}

_NAME_END

#endif