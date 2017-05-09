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

#include <ray/vector4.h>

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

	T center1;
	T center2;
	T left;
	T right;
	T up;
	T down;

	HemisphereWeight() noexcept {}
	HemisphereWeight(float c1, float c2, float l, float r, float u, float b) : center1(c1), center2(c2), left(l), right(r), up(u), down(b) noexcept {}
	HemisphereWeight(const Vector2t<T>& c, const Vector2t<T>& lr, const Vector2t<T>& ub) : center1(c.x), center2(c.y), left(lr.x), right(lr.y), up(ub.x), down(ub.y) noexcept {}
	HemisphereWeight(const Vector3t<T>& c, const Vector3t<T>& rub) : center1(c.x), center2(c.y), left(c.z), right(rub.x), up(rub.y), down(rub.z) noexcept {}
	HemisphereWeight(const Vector2t<T>& c, const Vector4t<T>& lrub) : center1(c.x), center2(c.y), left(lrub.x), right(lrub.y), up(lrub.z), down(lrub.w) noexcept {}
	HemisphereWeight(const Vector4t<T>& clr, const Vector2t<T>& ub) : center1(clr.x), center2(clr.y), left(clr.z), right(clr.w), up(ub.x), down(ub.y) noexcept {}

	template<typename S>
	explicit HemisphereWeight(S* w) : center1(w[0]), center2(w[1]), left(w[2]), right(w[3]), up(w[4]), down(w[5]) {}
	
	HemisphereWeight<T>& operator+=(const T sz) { center1 += sz; center2 += sz; left += sz; right += sz; up += sz; down += sz; return *this; }
	HemisphereWeight<T>& operator-=(const T sz) { center1 -= sz; center2 -= sz; left -= sz; right -= sz; up -= sz; down -= sz; return *this; }
	HemisphereWeight<T>& operator*=(const T sz) { center1 *= sz; center2 *= sz; left *= sz; right *= sz; up *= sz; down *= sz; return *this; }
	HemisphereWeight<T>& operator/=(const T sz) { center1 /= sz; center2 /= sz; left /= sz; right /= sz; up /= sz; down /= sz; return *this; }

	HemisphereWeight<T>& operator+=(const HemisphereWeight<T>& w) { center1 += w.center1; center2 += w.center2; left += w.left; right += w.right; up += w.up; down += w.down; return *this; }
	HemisphereWeight<T>& operator-=(const HemisphereWeight<T>& w) { center1 -= w.center1; center2 -= w.center2; left -= w.left; right -= w.right; up -= w.up; down -= w.down; return *this; }
	HemisphereWeight<T>& operator*=(const HemisphereWeight<T>& w) { center1 *= w.center1; center2 *= w.center2; left *= w.left; right *= w.right; up *= w.up; down *= w.down; return *this; }
	HemisphereWeight<T>& operator/=(const HemisphereWeight<T>& w) { center1 /= w.center1; center2 /= w.center2; left /= w.left; right /= w.right; up /= w.up; down /= w.down; return *this; }

	template <typename S>
	explicit operator HemisphereWeight<S>() const
	{
		return HemisphereWeight<S>(
			HemisphereWeight<S>(center1), 
			HemisphereWeight<S>(center2), 
			HemisphereWeight<S>(left),
			HemisphereWeight<S>(right),
			HemisphereWeight<S>(up),
			HemisphereWeight<S>(down));
	}
	
	T operator[](std::size_t i) const
	{
		return *(&center1 + i);
	}

	T& operator[](std::size_t i)
	{
		return *(&center1 + i);
	}
	
	T* ptr() { return (T*)this; }
	const T* ptr() const { return (const T*)this; }
	T* data() { return (T*)this; }
	const T* data() const { return (const T*)this; }
};

_NAME_END

#endif