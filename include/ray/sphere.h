// +----------------------------------------------------------------------
// | Project : ray.
// All rights reserved.
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
#ifndef _H_SPHERE_H_
#define _H_SPHERE_H_

#include <ray/aabb.h>

_NAME_BEGIN

template<typename T>
class Spheret
{
public:
	typedef typename trait::_typeaddition<T>::value_type value_type;
	typedef typename trait::_typeaddition<T>::pointer pointer;
	typedef typename trait::_typeaddition<T>::const_pointer const_pointer;
	typedef typename trait::_typeaddition<T>::reference reference;
	typedef typename trait::_typeaddition<T>::const_reference const_reference;

	Spheret() noexcept {}
	Spheret(const Vector3t<T>& min, const Vector3t<T>& max) noexcept { this->set(min, max); }
	Spheret(const Vector3t<T>& pt, T radius) noexcept : _center(pt), _radius(radius) {}

	void set(const AABBt<T>& aabb) noexcept
	{
		_box = aabb;
		_center = _box.center();
		_radius = math::length(_box.size()) * 0.5f;
	}

	void set(const Vector3t<T>& min, const Vector3t<T>& max) noexcept
	{
		_box = AABBt<T>(min, max);
		_center = _box.center();
		_radius = math::length(_box.size()) * 0.5f;
	}

	void setCenter(const Vector3t<T>& center) noexcept
	{
		_box.min = _box.min - _center + center;
		_box.max = _box.max - _center + center;
		_center = center;
		_radius = math::length(_box.size()) * 0.5f;
	}

	void reset() noexcept
	{
		_radius = 0;
		_center = Vector3t<T>::Zero;
		_box.reset();
	}

	void encapsulate(const Vector3t<T>& pt) noexcept
	{
		_box.encapsulate(pt);
		_center = _box.center();
		_radius = math::length(_box.size()) * 0.5f;
	}

	void encapsulate(const Vector3t<T> pt[], std::size_t n) noexcept
	{
		assert(pt);

		_box.encapsulate(pt, n);
		_center = _box.center();
		_radius = math::length(_box.size()) * 0.5f;
	}

	void encapsulate(const Vector3t<T> pt[], const std::uint8_t* indices, std::size_t indicesCount) noexcept
	{
		assert(pt);
		assert(indices && indicesCount > 0);

		_box.encapsulate(pt, indices, indicesCount);
		_center = _box.center();
		_radius = math::length(_box.size()) * 0.5f;
	}

	void encapsulate(const Vector3t<T> pt[], const std::uint16_t* indices, std::size_t indicesCount) noexcept
	{
		assert(pt);
		assert(indices && indicesCount > 0);

		_box.encapsulate(pt, indices, indicesCount);
		_center = _box.center();
		_radius = math::length(_box.size()) * 0.5f;
	}

	void encapsulate(const Vector3t<T> pt[], const std::uint32_t* indices, std::size_t indicesCount) noexcept
	{
		assert(pt);
		assert(indices && indicesCount > 0);

		_box.encapsulate(pt, indices, indicesCount);
		_center = _box.center();
		_radius = math::length(_box.size()) * 0.5f;
	}

	void encapsulate(const Spheret<T>& sphere) noexcept
	{
		_box.encapsulate(sphere._box);
		_center = _box.center();
		_radius = math::length(_box.size()) * 0.5f;
	}

	void applyMatrix(const Matrix3x3t<T>& m, const Vector3t<T>& translate = Vector3t<T>::Zero) noexcept
	{
		_box.applyMatrix(m, translate);
		_center = _box.center();
		_radius = math::length(_box.size()) * 0.5f;
	}

	void applyMatrix(const Matrix4x4t<T>& m) noexcept
	{
		_box.applyMatrix(m);
		_center = _box.center();
		_radius = math::length(_box.size()) * 0.5f;
	}

	bool empty() const noexcept
	{
		return _box.empty();
	}


	bool contains(const Vector3t<T>& pt) const noexcept
	{
		Vector3t<T> p = pt - _center;

		if (p.x > _radius) { return false; }
		if (p.y > _radius) { return false; }
		if (p.z > _radius) { return false; }

		return true;
	}

	bool intersects(const Spheret<T>& sphere) const noexcept
	{
		Vector3t<T>  distance = _center - sphere._center;

		T dist2 = distance.length2();

		T radiusSum = _radius + sphere._radius;

		return dist2 <= radiusSum * radiusSum;
	}

	bool intersects(const Plane3t<T>& p) const noexcept
	{
		float dist = _center.dot(p.normal) - p.distance;

		return abs(dist) <= _radius;
	}

	bool intersects(const AABBt<T>& _box) const noexcept
	{
		Vector3t<T>  point = _box.closestPoint(_center);
		float dinstance2 = _center.sqrDistance(point);
		return  dinstance2 < (_radius * _radius);
	}

	bool intersects(const Raycast3t<T>& ray) const noexcept
	{
		Vector3 m = ray.origin - _center;

		float c = math::length2(m) - _radius * _radius;
		if (c <= 0.0)
			return true;

		float b = math::dot(m, ray.normal);
		if (b > 0.0)
			return 0;

		float disc = b * b - c;

		if (disc < 0.0)
			return 0;

		return 1;
	}

	T distance(const Vector3t<T>& pt) const noexcept
	{
		Vector3t<T>  d = _center - pt;
		return d.length() - _radius;
	}

	T sqrDistance(const Vector3t<T> & pt) const noexcept
	{
		Vector3t<T>  d = _center - pt;

		return d.length2() - _radius * _radius;
	}

	T radius() const noexcept
	{
		return _radius;
	}

	const AABBt<T>& aabb() const noexcept { return _box; }

	const Vector3t<T>& min() const noexcept { return _box.min; }
	const Vector3t<T>& max() const noexcept { return _box.max; }
	const Vector3t<T>& center() const noexcept { return _center; }

	Vector3t<T> size() const noexcept { return _box.size(); }

	Vector3t<T> closestPoint(const Vector3t<T> & pt) const noexcept
	{
		Vector3t<T>  d = _center - pt;

		T len = d.length();

		T dist = len - _radius;

		Vector3t<T>  b = dist / len * d;

		return pt + b;
	}

private:
	T _radius;
	Vector3t<T> _center;
	AABBt<T> _box;
};

_NAME_END

#endif