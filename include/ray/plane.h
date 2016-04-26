// +----------------------------------------------------------------------
// | Project : ray.
// All rights reserved.
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
#ifndef _H_PLANE3_H_
#define _H_PLANE3_H_

#include <ray/vector3.h>

_NAME_BEGIN

template<typename T>
class Plane2t
{
public:
	float distance;
	Vector2t<T> normal;

	Plane2t()
	{
	}

	Plane2t(const Plane2t& copy)
		: distance(copy.distance)
		, normal(copy.normal)
	{
	}

	Plane2t(const Vector2t<T>& pt0, const Vector2t<T>& pt1)
	{
		compute(pt0, pt1);
	}

	Plane2t(const Vector2t<T>& n, float dist)
	{
		setNormal(n);
		distance = dist;
	}

	void setNormal(const Vector2t<T>& n)
	{
		normal = n;
		normal.normalize();
	}

	void setDistance(const Vector2t<T>& pt)
	{
		distance = -normal.dot(pt);
	}

	void compute(const Vector2t<T>& pt0, const Vector2t<T>& pt1)
	{
		setNormal(pt1 - pt0);
		setDistance(pt0);
	}

	float getDistance(const Vector2t<T>& pt) const
	{
		return normal.dot(pt) + distance;
	}

	Vector2t<T> project(const Vector2t<T>& pt) const
	{
		return pt - getDistance(pt) * normal;
	}
};

template<typename T>
class Plane3t
{
public:

	Vector3t<T> normal;
	T distance;

	enum PLANE_CLASSIFICATIONS
	{
		PLANE_FRONT,
		PLANE_BACK,
		PLANE_INTERSECT
	};

	Plane3t()
	{
	}

	Plane3t(const Plane3t& copy)
		: normal(copy.normal)
		, distance(copy.distance)
	{
	}

	Plane3t(const Vector3t<T>& normal, const Vector3t<T>& pt)
	{
		setNormal(normal);
		setDistance(pt);
	}

	Plane3t(const Vector3t<T>& n, float dist)
	{
		setNormal(n);
		distance = dist;
	}

	Plane3t(const Vector3t<T>& a, const Vector3t<T>& b, const Vector3t<T>& c)
	{
		compute(a, b, c);
	}

	Plane3t(const Vector3t<T> pt[], int n)
	{
		compute(pt, n);
	}

	Plane3t& operator=(const Plane3t& rhs)
	{
		if (this == &rhs)
		{
			return (*this);
		}

		normal = rhs.normal;
		distance = rhs.distance;

		return *this;
	}

	Plane3t* clone() const
	{
		return new Plane3t(*this);
	}

	void setNormal(const Vector3t<T>& n)
	{
		normal = n;
		normal.normalize();
	}

	void normalize()
	{
		float inv = 1 / math::length(normal);
		normal *= inv;
		distance *= inv;
	}

	void setDistance(const Vector3t<T>& pt)
	{
		distance = dot(-normal, pt);
	}

	void compute(const Vector3t<T>& o, const Vector3t<T>& a, const Vector3t<T>& b)
	{
		Vector3t<T> edge1 = a - o;
		Vector3t<T> edge2 = b - o;

		normal = cross(edge2, edge1);
		normal.normalize();

		distance = math::dot(-normal, o);
	}

	void compute(const Vector3t<T> pt[], int n)
	{
		const Vector3t<T> *p = &pt[n - 1];

		normal = Vector3t<T>::Zero;

		for (int i = 0; i < n; i++)
		{
			const Vector3t<T> *center = &p[i];

			normal.x += (p->z + center->z) * (p->y - center->y);
			normal.y += (p->x + center->x) * (p->z - center->z);
			normal.z += (p->y + center->y) * (p->x - center->x);

			p = center;
		}

		normal.normalize();

		float dist;

		for (int i = 0; i < n; i++)
		{
			dist = math::dot(-normal, pt[i]);
		}

		distance = dist / n;
	}

	float getDistance(const Vector3t<T>& pt) const
	{
		return math::dot(normal, pt) + distance;
	}

	Vector3t<T> closestPoint(const Vector3t<T>& pt) const
	{
		return pt - math::dot(normal, getDistance(pt));
	}

	bool intersect(const Rect3t<T>& rc) const
	{
		if ((getDistance(rc.pos0) > static_cast<T>(0.0)) ||
			(getDistance(rc.pos1) > static_cast<T>(0.0)) ||
			(getDistance(Vector3t<T>(rc.pos0.x, rc.pos1.y, rc.pos1.z)) > static_cast<T>(0.0)) ||
			(getDistance(Vector3t<T>(rc.pos1.x, rc.pos0.y, rc.pos1.z)) > static_cast<T>(0.0)))
		{
			return true;
		}

		return false;
	}

	int classify(const Vector3t<T>& pt) const
	{
		float dist = getDistance(pt);

		if ((dist > 0))
			return PLANE_INTERSECT;

		if (dist < distance)
			return PLANE_FRONT;

		if (dist > distance)
			return PLANE_BACK;

		return PLANE_BACK;
	}

	int classify(const Line3t<T>& line) const
	{
		int classify0 = classify(line.pos0);
		int classify1 = classify(line.pos1);

		if (classify0 == classify1)
			return classify1;

		return PLANE_INTERSECT;
	}

	int classify(const Rect3t<T>& rc) const
	{
		Vector3t<T> minPoint, maxPoint;

		if (normal.x > 0.0f)
		{
			minPoint.x = rc.pos0.x;
			maxPoint.x = rc.pos1.x;
		}
		else
		{
			minPoint.x = rc.pos1.x;
			maxPoint.x = rc.pos0.x;
		}

		if (normal.y > 0.0f)
		{
			minPoint.y = rc.pos0.y;
			maxPoint.y = rc.pos1.y;
		}
		else
		{
			minPoint.y = rc.pos1.y;
			maxPoint.y = rc.pos0.y;
		}

		if (normal.z > 0.0f)
		{
			minPoint.z = rc.pos0.z;
			maxPoint.z = rc.pos1.z;
		}
		else
		{
			minPoint.z = rc.pos1.z;
			maxPoint.z = rc.pos0.z;
		}

		float dmin = getDistance(minPoint);
		float dmax = getDistance(minPoint);

		if (dmin * dmax < 0.0f)
		{
			return PLANE_INTERSECT;
		}
		else if (dmin)
		{
			return PLANE_FRONT;
		}

		return PLANE_BACK;
	}
};

_NAME_END

#endif