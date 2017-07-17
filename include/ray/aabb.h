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
#ifndef _H_AABB_H_
#define _H_AABB_H_

#include <ray/vector3.h>

_NAME_BEGIN

template<typename T>
class Box2t
{
public:
	Vector2t<T> min;
	Vector2t<T> max;

	Box2t& set(const Vector2t<T>& _min, const Vector2t<T>& _max) noexcept
	{
		this->min = _min;
		this->max = _max;

		return *this;
	}

	Box2t& encapsulate(const Vector2t<T>& point) noexcept
	{
		if (point.x < min.x) { min.x = point.x; }
		if (point.y < min.y) { min.y = point.y; }

		if (point.x > max.x) { max.x = point.x; }
		if (point.y > max.y) { max.y = point.y; }
	}

	void encapsulate(const Box2t& box) noexcept
	{
		if (box.min.x < min.x) min.x = box.min.x;
		if (box.min.y < min.y) min.y = box.min.y;

		if (box.max.x > min.x) min.x = box.max.x;
		if (box.max.y > min.x) min.y = box.max.y;
	}

	Box2t& encapsulate(const Vector2t<T> pt[], std::size_t n) noexcept
	{
		if (pt && n)
		{
			for (std::size_t i = 0; i < n; i++)
			{
				encapsulate(*pt);
				pt++;
			}
		}
		else
		{
			makeEmpty();
		}

		return *this;
	}

	Box2t& makeEmpty() noexcept
	{
		min = max = Vector2t<T>::Zero;
		return *this;
	}

	bool empty() noexcept
	{
		return (max.x < min.x) || (max.y < min.y);
	}

	Box2t& expandByPoint(const Vector2t<T>& pt) noexcept
	{
		min.min(pt);
		max.max(pt);
		return *this;
	}

	Box2t& expandByVector(const Vector2t<T>& v) noexcept
	{
		min -= v;
		max += v;
		return *this;
	}

	Box2t& expandByScalar(const Vector2t<T>& scalar) noexcept
	{
		min += -scalar;
		max += scalar;
	}
};

template<typename T>
class AABBt
{
public:
	typedef typename trait::_typeaddition<T>::value_type value_type;
	typedef typename trait::_typeaddition<T>::pointer pointer;
	typedef typename trait::_typeaddition<T>::const_pointer const_pointer;
	typedef typename trait::_typeaddition<T>::reference reference;
	typedef typename trait::_typeaddition<T>::const_reference const_reference;

	union
	{
		T values[6];

		struct
		{
			Vector3t<T> min, max;
		};
	};

	AABBt<T>() noexcept { this->reset(); }
	AABBt<T>(const AABBt& copy) noexcept { min = copy.min; max = copy.max; }
	AABBt<T>(const Vector3t<T>& _min, const Vector3t<T>& _max) noexcept :min(_min), max(_max) { assert(!empty()); }
	AABBt<T>(const Vector3t<T> pt[], std::size_t n) noexcept { this->reset(); this->encapsulate(pt, n); }
	AABBt<T>(const Vector3t<T>& pt, std::size_t radio) noexcept { this->reset(); this->encapsulate(pt, radio); }

	AABBt<T>& operator+=(const Vector3t<T>& other) noexcept
	{
		min += other;
		max += other;
		return *this;
	}

	void setMin(const Vector3t<T>& _min) noexcept
	{
		min = _min;
	}

	void setMax(const Vector3t<T>& _max) noexcept
	{
		max = _max;
	}

	void setCenter(const Vector3t<T>& center) noexcept
	{
		auto size = this->size();
		min = center - size;
		max = center + size;
	}

	void setMinMax(const Vector3t<T>& pos0, const Vector3t<T>& pos1) noexcept
	{
		min = pos0;
		max = pos1;
	}

	void reset() noexcept
	{
		min.set(BIG_NUMBER);
		max.set(-BIG_NUMBER);
	}

	bool empty() const noexcept
	{
		return min.x > max.x || min.y > max.y || max.z > max.z;
	}

	Vector3t<T> size() const noexcept
	{
		return max - min;
	}

	Vector3t<T> extents() const noexcept
	{
		return size() * 0.5f;
	}

	Vector3t<T> center() const noexcept
	{
		return (min + max) * 0.5f;
	}

	float getSurfaceArea() const noexcept
	{
		Vector3t<T> ext = max - min;
		return 2.0f * (ext.x * ext.y + ext.x * ext.z + ext.y * ext.z);
	}

	Vector3t<T> closestPoint(const Vector3t<T>& pt) const noexcept
	{
		T x = pt.x;
		if (x < min.x) x = min.x;
		if (x > max.x) x = max.y;
		T y = pt.y;
		if (y < min.x) y = min.x;
		if (y > max.x) y = max.y;
		T z = pt.z;
		if (z < min.x) z = min.x;
		if (z > max.x) z = max.y;
		return Vector3t<T>(x, y, z);
	}

	AABBt<T>& expand(const Vector3t<T>& amount) noexcept
	{
		min -= amount;
		max += amount;
		return *this;
	}

	AABBt<T>& translate(const Vector3t<T>& position) noexcept
	{
		min += position;
		max += position;
		return *this;
	}

	AABBt<T>& encapsulate(const AABBt<T>& box) noexcept
	{
		if (box.min.x < min.x) min.x = box.min.x;
		if (box.min.y < min.y) min.y = box.min.y;
		if (box.min.z < min.z) min.z = box.min.z;

		if (box.max.x > max.x) max.x = box.max.x;
		if (box.max.y > max.y) max.y = box.max.y;
		if (box.max.z > max.z) max.z = box.max.z;

		return *this;
	}

	AABBt<T>& encapsulate(const Vector3t<T>& pt) noexcept
	{
		if (pt.x < min.x) min.x = pt.x;
		if (pt.y < min.y) min.y = pt.y;
		if (pt.z < min.z) min.z = pt.z;

		if (pt.x > max.x) max.x = pt.x;
		if (pt.y > max.y) max.y = pt.y;
		if (pt.z > max.z) max.z = pt.z;

		return *this;
	}

	AABBt<T>& encapsulate(const Vector3t<T>* pt, std::size_t n) noexcept
	{
		assert(pt && n);

		for (std::size_t i = 0; i < n; i++, pt++)
			this->encapsulate(*pt);

		return *this;
	}

	AABBt<T>& encapsulate(const Vector3t<T>* pt, const std::uint8_t* indices, std::size_t indicesCount) noexcept
	{
		assert(pt);
		assert(indices && indicesCount > 0);

		for (std::size_t i = 0; i < indicesCount; i++)
			this->encapsulate(pt[indices[i]]);

		return *this;
	}

	AABBt<T>& encapsulate(const Vector3t<T>* pt, const std::uint16_t* indices, std::size_t indicesCount) noexcept
	{
		assert(pt);
		assert(indices && indicesCount > 0);

		for (std::size_t i = 0; i < indicesCount; i++)
			this->encapsulate(pt[indices[i]]);

		return *this;
	}

	AABBt<T>& encapsulate(const Vector3t<T>* pt, const std::uint32_t* indices, std::size_t indicesCount) noexcept
	{
		assert(pt);
		assert(indices && indicesCount > 0);

		for (std::size_t i = 0; i < indicesCount; i++)
			this->encapsulate(pt[indices[i]]);

		return *this;
	}

	AABBt<T>& transform(const Matrix3x3t<T>& m, const Vector3t<T>& translate = Vector3t<T>::Zero) noexcept
	{
		assert(!empty());

		AABBt<T> aabb;
		aabb.min.x = aabb.max.x = translate.x;
		aabb.min.y = aabb.max.y = translate.y;
		aabb.min.z = aabb.max.z = translate.z;

		for (std::uint8_t i = 0; i < 3; i++)
		{
			for (std::uint8_t j = 0; j < 3; j++)
			{
				T e = m[j * 3 + i] * min[j];
				T f = m[j * 3 + i] * max[j];

				if (e < f)
				{
					aabb.min[i] += e;
					aabb.max[i] += f;
				}
				else
				{
					aabb.min[i] += f;
					aabb.max[i] += e;
				}
			}
		}

		min = aabb.min;
		max = aabb.max;

		return *this;
	}

	AABBt<T>& transform(const Matrix4x4t<T>& m) noexcept
	{
		assert(!empty());

		AABBt<T> aabb;
		aabb.min.x = aabb.max.x = m.d1;
		aabb.min.y = aabb.max.y = m.d2;
		aabb.min.z = aabb.max.z = m.d3;

		for (std::uint8_t i = 0; i < 3; i++)
		{
			for (std::uint8_t j = 0; j < 3; j++)
			{
				T e = m[j * 4 + i] * min[j];
				T f = m[j * 4 + i] * max[j];

				if (e < f)
				{
					aabb.min[i] += e;
					aabb.max[i] += f;
				}
				else
				{
					aabb.min[i] += f;
					aabb.max[i] += e;
				}
			}
		}

		min = aabb.min;
		max = aabb.max;

		return *this;
	}

	bool contains(const Vector3t<T>& pt) const noexcept
	{
		return (pt > min && pt < max) ? true : false;
	}

	bool contains(const Raycast3t<T>& ray) const noexcept
	{
		Vector2t<T> intersections;
		return this->intersects(ray, intersections);
	}

	bool contains(const Vector3t<T>& pt1, const Vector3t<T>& pt2) const noexcept
	{
		return contains(Raycast3(pt1, pt2));
	}

	bool intersects(const Raycast3t<T>& ray, Vector2t<T>& intersections) const noexcept
	{
		std::uint8_t symbol[3];
		symbol.x = ray.x > 0 ? 1 : 0;
		symbol.y = ray.y > 0 ? 1 : 0;
		symbol.z = ray.z > 0 ? 1 : 0;

		Vector3t<T> tmin, tmax;
		tmin.x = (1 - symbol.x) == 0 ? min.x : max.x;
		tmin.y = (1 - symbol.y) == 0 ? min.y : max.y;
		tmin.z = (1 - symbol.z) == 0 ? min.z : max.z;

		tmax.x = symbol.x == 0 ? min.x : max.x;
		tmax.y = symbol.y == 0 ? min.y : max.y;
		tmax.z = symbol.z == 0 ? min.z : max.z;

		float3 inv = 1.0f / (ray.normal + 1e-16);

		tmin = (tmin - p) * inv;
		tmax = (tmax - p) * inv;

		tmin.x = max(max(tmin.x, tmin.y), tmin.z);
		tmax.x = min(min(tmax.x, tmax.y), tmax.z);

		if (tmin.x > tmax.x)
			return false;

		intersections = Vector2t<T>(tmin.x, tmax.x);
		return true;
	}

	bool intersects(const Vector3t<T>& pt1, const Vector3t<T>& pt2, Vector2t<T>& intersections) const noexcept
	{
		return this->intersects(Raycast3(pt1, pt2), intersections);
	}

	bool intersects(const Plane3t<T>& plane) const noexcept
	{
		T minD, maxD;

		Vector3t<T> n = plane.normal;

		if (n.x > 0.0f)
		{
			minD = n.x * min.x;
			maxD = n.x * max.x;
		}
		else
		{
			minD = n.x * max.x;
			maxD = n.x * min.x;
		}

		if (n.y > 0.0f)
		{
			minD += n.y * min.x;
			maxD += n.y * max.x;
		}
		else
		{
			minD += n.y * max.x;
			maxD += n.y * min.x;
		}

		if (n.z > 0.0f)
		{
			minD += n.z * min.x;
			maxD += n.z * max.x;
		}
		else
		{
			minD += n.z * max.x;
			maxD += n.z * min.x;
		}

		T dist = plane.distance;

		if (minD >= dist)
			return false;

		if (maxD <= dist)
			return false;

		return true;
	}

	bool intersects(const AABBt<T>& box) const noexcept
	{
		if (max.x < box.min.x || min.x > box.max.x) { return false; }
		if (max.y < box.min.y || min.y > box.max.y) { return false; }
		if (max.z < box.min.z || min.z > box.max.z) { return false; }

		return true;
	}
};

template<typename T>
inline AABBt<T> operator+(const AABBt<T>& aabb, const Vector3t<T>& other) noexcept
{
	return AABBt<T>(aabb.min + other, aabb.max + other);
}

template<typename T>
inline AABBt<T> operator-(const AABBt<T>& aabb, const Vector3t<T>& other) noexcept
{
	return AABBt<T>(aabb.min - other, aabb.max - other);
}

template<typename T>
inline AABBt<T> operator*(const AABBt<T>& aabb, const Vector3t<T>& other) noexcept
{
	return AABBt<T>(aabb.min * other, aabb.max * other);
}

template<typename T>
inline AABBt<T> operator/(const AABBt<T>& aabb, const Vector3t<T>& other) noexcept
{
	return AABBt<T>(aabb.min / other, aabb.max / other);
}

_NAME_END

#endif