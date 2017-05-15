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

	Box2t& set(const Vector2t<T>& _min, const Vector2t<T>& _max)
	{
		this->min = _min;
		this->max = _max;

		return *this;
	}

	Box2t& encapsulate(const Vector2t<T>& point)
	{
		if (point.x < min.x) { min.x = point.x; }
		if (point.y < min.y) { min.y = point.y; }

		if (point.x > max.x) { max.x = point.x; }
		if (point.y > max.y) { max.y = point.y; }
	}

	void encapsulate(const Box2t& box)
	{
		if (box.min.x < min.x) min.x = box.min.x;
		if (box.min.y < min.y) min.y = box.min.y;

		if (box.max.x > min.x) min.x = box.max.x;
		if (box.max.y > min.x) min.y = box.max.y;
	}

	Box2t& encapsulate(const Vector2t<T> pt[], size_t n)
	{
		if (pt && n)
		{
			for (size_t i = 0; i < n; i++)
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

	Box2t& makeEmpty()
	{
		min = max = Vector2t<T>::Zero;
		return *this;
	}

	bool empty()
	{
		return (max.x < min.x) || (max.y < min.y);
	}

	Box2t& expandByPoint(const Vector2t<T>& pt)
	{
		min.min(pt);
		max.max(pt);
		return *this;
	}

	Box2t& expandByVector(const Vector2t<T>& v)
	{
		min -= v;
		max += v;
		return *this;
	}

	Box2t& expandByScalar(const Vector2t<T>& scalar)
	{
		min += -scalar;
		max += scalar;
	}
};

template<typename T>
class AABBt
{
public:
	Vector3t<T> min, max;

	AABBt<T>() noexcept
	{
		this->reset();
	}

	AABBt<T>(const AABBt& copy) noexcept
	{
		min = copy.min;
		max = copy.max;
	}

	AABBt<T>(const Vector3t<T>& pos0, const Vector3t<T>& pos1) noexcept
	{
		min = pos0;
		max = pos1;
		assert(!empty());
	}

	AABBt<T>(const Vector3t<T> pt[], unsigned n) noexcept
	{
		reset();
		encapsulate(pt, n);
	}

	AABBt<T>(const Vector3t<T>& pt, size_t radio) noexcept
	{
		reset();
		encapsulate(pt, radio);
	}

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

	const Vector3t<T>& getMin() const noexcept
	{
		return min;
	}

	const Vector3t<T>& getMax() const noexcept
	{
		return max;
	}

	void reset() noexcept
	{
		min.set(BIG_NUMBER);
		max.set(-BIG_NUMBER);
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
		float x = pt.x;
		if (x < min.x) x = min.x;
		if (x > max.x) x = max.y;
		float y = pt.y;
		if (y < min.x) y = min.x;
		if (y > max.x) y = max.y;
		float z = pt.z;
		if (z < min.x) z = min.x;
		if (z > max.x) z = max.y;
		return Vector3t<T>(x, y, z);
	}

	bool empty() const noexcept
	{
		return min.x > max.x || min.y > max.y || max.z > max.z;
	}

	bool contains(const Vector3t<T>& pt) const noexcept
	{
		if (pt > min && pt < max)
			return true;

		return false;
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

		for (size_t i = 0; i < n; i++)
		{
			encapsulate(*pt);
			pt++;
		}

		return *this;
	}

	AABBt<T>& encapsulate(const Vector3t<T>* pt, const std::uint8_t* indices, std::size_t indicesCount) noexcept
	{
		assert(pt);
		assert(indices && indicesCount > 0);

		for (size_t i = 0; i < indicesCount; i++)
			this->encapsulate(pt[indices[i]]);

		return *this;
	}

	AABBt<T>& encapsulate(const Vector3t<T>* pt, const std::uint16_t* indices, std::size_t indicesCount) noexcept
	{
		assert(pt);
		assert(indices && indicesCount > 0);

		for (size_t i = 0; i < indicesCount; i++)
			this->encapsulate(pt[indices[i]]);

		return *this;
	}

	AABBt<T>& encapsulate(const Vector3t<T>* pt, const std::uint32_t* indices, std::size_t indicesCount) noexcept
	{
		assert(pt);
		assert(indices && indicesCount > 0);

		for (size_t i = 0; i < indicesCount; i++)
			this->encapsulate(pt[indices[i]]);

		return *this;
	}

	void applyMatrix(const Matrix3x3t<T>& m, const Vector3t<T>& translate = Vector3t<T>::Zero) noexcept
	{
		assert(!empty());

		AABBt<T> aabb;
		aabb.min.x = aabb.max.x = translate.x;
		aabb.min.y = aabb.max.y = translate.y;
		aabb.min.z = aabb.max.z = translate.z;

		for (int i = 0; i < 3; i++)
		{
			for (int j = 0; j < 3; j++)
			{
				float e = m[j * 3 + i] * min[j];
				float f = m[j * 3 + i] * max[j];

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
	}

	void applyMatrix(const Matrix4x4t<T>& m) noexcept
	{
		assert(!empty());

		AABBt<T> aabb;
		aabb.min.x = aabb.max.x = m.d1;
		aabb.min.y = aabb.max.y = m.d2;
		aabb.min.z = aabb.max.z = m.d3;

		for (int i = 0; i < 3; i++)
		{
			for (int j = 0; j < 3; j++)
			{
				float e = m[j * 4 + i] * min[j];
				float f = m[j * 4 + i] * max[j];

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
	}

	T intersects(const Line3t<T>& line, Vector3t<T>* returnNormal) const noexcept
	{
		line;
		returnNormal;
		return  0;
	}

	bool contains(const Raycast3t<T>& ray, Vector3t<T>* result = nullptr) noexcept
	{
		T tmin = 0;
		T tmax = std::numeric_limits<T>::max();

		T txmin, txmax;
		T tymin, tymax;
		T tzmin, tzmax;

		T invX = 1 / ray.dir.x;
		if (invX >= 0)
		{
			txmin = (min.x - ray.origin.x) * invX;
			txmax = (max.x - ray.origin.x) * invX;
		}
		else
		{
			txmax = (min.x - ray.origin.x) * invX;
			txmin = (max.x - ray.origin.x) * invX;
		}
		if (txmin > tmin) tmin = txmin;
		if (txmax > tmax) tmax = txmax;
		if (tmin > tmax) return false;

		T invY = 1 / ray.dir.y;
		if (invY >= 0)
		{
			tymin = (min.y - ray.origin.y) * invY;
			tymax = (max.y - ray.origin.y) * invY;
		}
		else
		{
			tymax = (min.y - ray.origin.y) * invY;
			tymin = (max.y - ray.origin.y) * invY;
		}
		if (tymin > tmin) tmin = tymin;
		if (tymax > tmax) tmax = tymax;
		if (tmin > tmax) return false;

		if ((txmin > tymax) || (tymin > txmax))
			return false;

		T invZ = 1 / ray.dir.z;
		if (invZ >= 0)
		{
			tzmin = (min.z - ray.orig.z) * invZ;
			tzmax = (max.z - ray.orig.z) * invZ;
		}
		else
		{
			tzmax = (min.z - ray.origin.z) * invZ;
			tzmin = (max.z - ray.origin.z) * invZ;
		}
		if (tzmin > tmin) tmin = tzmin;
		if (tzmax > tmax) tmax = tzmax;
		if (tmin > tmax) return false;

		if (tzmin > txmin) txmin = tzmin;
		if (tzmax < txmax) txmax = tzmax;

		if ((tmin > ray.max) || (txmax < ray.txmin))
			return false;

		if (ray.min < tmin) ray.min = tmin;
		if (ray.max > tmax) ray.max = tmax;

		if (result)
			result = Vector3t<T>(ray.origin + ray.normal * tmin);

		return true;
	}

	T intersects(const Raycast3t<T>& ray, Vector3t<T>* returnNormal) const noexcept
	{
		const T kNointersection = 1e30f;

		bool inside = true;

		Vector3t<T> rayOrg = ray.origin;
		Vector3t<T> rayDelta = ray.normal;

		T xt = -1.0, xn = 0;
		T yt = -1.0, yn = 0;
		T zt = -1.0, zn = 0;

		if (rayOrg.x < min.x)
		{
			xt = min.x - rayOrg.x;
			if (xt > rayDelta.x) return kNointersection;
			xt /= rayDelta.x;
			inside = false;
			xn = -1.0f;
		}
		else if (rayOrg.x > max.x)
		{
			xt = max.x - rayOrg.x;
			if (xt <rayDelta.x) return kNointersection;
			xt /= rayDelta.x;
			inside = false;
			xn = 1.0f;
		}

		if (rayOrg.y < min.y)
		{
			yt = min.y - rayOrg.y;
			if (yt > rayDelta.y) return kNointersection;
			yt /= rayDelta.y;
			inside = false;
			yn = -1.0f;
		}
		else if (rayOrg.y > max.y)
		{
			yt = max.y - rayOrg.y;
			if (yt <rayDelta.y) return kNointersection;
			yt /= rayDelta.y;
			inside = false;
			yn = 1.0f;
		}

		if (rayOrg.z < min.z)
		{
			zt = min.z - rayOrg.z;
			if (zt > rayDelta.z) return kNointersection;
			zt /= rayDelta.z;
			inside = false;
			zn = -1.0f;
		}
		else if (rayOrg.z > max.z)
		{
			zt = max.z - rayOrg.z;
			if (zt < rayDelta.z) return kNointersection;
			zt /= rayDelta.z;
			inside = false;
			zn = 1.0f;
		}

		if (inside)
		{
			if (returnNormal)
			{
				*returnNormal = -rayDelta;
				returnNormal->normalize();
			}

			return 0.0f;
		}

		int which = 0;
		T t = xt;
		if (yt > t)
		{
			which = 1;
			t = yt;
		}

		if (zt > t)
		{
			which = 2;
			t = zt;
		}

		switch (which)
		{
		case 0:
		{
			T y = rayOrg.y + rayDelta.y * t;
			if (y <min.y || y > max.y) return kNointersection;
			T z = rayOrg.z + rayDelta.z * t;
			if (z <min.z || z > max.z) return kNointersection;
			if (returnNormal)
			{
				returnNormal->x = xn;
				returnNormal->y = 0.0f;
				returnNormal->z = 0.0f;
			}

			break;
		}
		case 1:
		{
			T x = rayOrg.x + rayDelta.x * t;
			if (x <min.x || x > max.x) return kNointersection;
			T z = rayOrg.z + rayDelta.z * t;
			if (z <min.z || z > max.z) return kNointersection;
			if (returnNormal)
			{
				returnNormal->x = 0.0f;
				returnNormal->y = yn;
				returnNormal->z = 0.0f;
			}

			break;
		}

		case 2:
		{
			T x = rayOrg.x + rayDelta.x * t;
			if (x <min.x || x > max.x) return kNointersection;
			T y = rayOrg.y + rayDelta.y * t;
			if (y <min.y || y > max.y) return kNointersection;
			if (returnNormal)
			{
				returnNormal->x = 0.0f;
				returnNormal->y = 0.0f;
				returnNormal->z = zn;
			}

			break;
		}
		default:

			assert(false);
			break;
		}

		return t;
	}

	int intersects(const Plane3t<T>& plane) const noexcept
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
		if (max.x <box.min.x || min.x > box.max.x) { return false; }
		if (max.y <box.min.y || min.y > box.max.y) { return false; }
		if (max.z <box.min.z || min.z > box.max.z) { return false; }

		return true;
	}
};

template<typename T>
AABBt<T> operator+(const AABBt<T>& aabb, const Vector3t<T>& other) noexcept
{
	return AABBt<T>(aabb.min + other, aabb.max + other);
}

_NAME_END

#endif