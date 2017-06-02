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
#ifndef _H_BOUNDING_BOX_H_
#define _H_BOUNDING_BOX_H_

#include <ray/sphere.h>

_NAME_BEGIN

template<typename T>
class BoundingBoxt
{
public:
	typedef typename trait::_typeaddition<T>::value_type value_type;
	typedef typename trait::_typeaddition<T>::pointer pointer;
	typedef typename trait::_typeaddition<T>::const_pointer const_pointer;
	typedef typename trait::_typeaddition<T>::reference reference;
	typedef typename trait::_typeaddition<T>::const_reference const_reference;

	static const BoundingBoxt<T> Empty;

	BoundingBoxt() noexcept { this->reset(); }
	BoundingBoxt(const Vector3t<T>& min, const Vector3t<T>& max) noexcept : _sp(min, max) {}
	BoundingBoxt(const Vector3t<T> pt[], std::size_t n) noexcept { this->reset(); this->encapsulate(pt, n); }
	BoundingBoxt(const Vector3t<T> pt[], const std::uint8_t* indices, std::size_t indicesCount) noexcept { this->reset(); this->encapsulate(pt, indices, indicesCount); }
	BoundingBoxt(const Vector3t<T> pt[], const std::uint16_t* indices, std::size_t indicesCount) noexcept { this->reset(); this->encapsulate(pt, indices, indicesCount); }
	BoundingBoxt(const Vector3t<T> pt[], const std::uint32_t* indices, std::size_t indicesCount) noexcept { this->reset(); this->encapsulate(pt, indices, indicesCount); }

	void set(const AABBt<T>& aabb) noexcept { _sp.set(aabb); }
	void set(const Vector3t<T>& min, const Vector3t<T>& max) noexcept { _sp.set(min, max); }
	void setCenter(const Vector3t<T>& center) noexcept { _sp.setCenter(center); }
	void reset() noexcept { _sp.reset(); }

	const Vector3t<T>& min() const noexcept { return _sp.min(); }
	const Vector3t<T>& max() const noexcept { return _sp.max(); }
	const Vector3t<T>& center() const noexcept { return _sp.center(); }

	const T radius() const noexcept { return _sp.radius(); }

	const Vector3t<T> size() const noexcept { return _sp.size(); }

	const AABBt<T>& aabb() const noexcept { return _sp.aabb(); }
	const Spheret<T>& sphere() const noexcept { return _sp; }

	void encapsulate(const Vector3t<T>& pt) noexcept { _sp.encapsulate(pt); }
	void encapsulate(const Vector3t<T> pt[], std::size_t n) noexcept { _sp.encapsulate(pt, n); }
	void encapsulate(const Vector3t<T> pt[], const std::uint8_t* indices, std::size_t indicesCount) noexcept { _sp.encapsulate(pt, indices, indicesCount); }
	void encapsulate(const Vector3t<T> pt[], const std::uint16_t* indices, std::size_t indicesCount) noexcept { _sp.encapsulate(pt, indices, indicesCount); }
	void encapsulate(const Vector3t<T> pt[], const std::uint32_t* indices, std::size_t indicesCount) noexcept { _sp.encapsulate(pt, indices, indicesCount); }
	void encapsulate(const BoundingBoxt<T>& bound) noexcept { _sp.encapsulate(bound._sp); }

	bool empty() const noexcept { return _sp.empty(); }

	bool intersects(const Raycast3t<T>& ray) const noexcept { return _sp.intersects(ray); }
	bool intersects(const Spheret<T>& sphere) const noexcept { return _sp.intersects(sphere); }
	bool intersects(const Plane3t<T>& p) const noexcept { return _sp.intersects(p); }
	bool intersects(const AABBt<T>& aabb) const noexcept { return _sp.intersects(aabb); }
	bool intersects(const Vector3t<T>& pt1, const Vector3t<T>& pt2) const noexcept { return _sp.intersects(pt1, pt2); }

	void transform(const Matrix3x3t<T>& m, const Vector3t<T>& translate = Vector3t<T>::Zero) noexcept
	{
		_sp.transform(m, translate);
	}

	void transform(const Matrix4x4t<T>& m) noexcept
	{
		_sp.transform(m);
	}

private:
	Spheret<T> _sp;
};

template<typename T> const BoundingBoxt<T> BoundingBoxt<T>::Empty;

_NAME_END

#endif