// +----------------------------------------------------------------------
// | Project : ray.
// | All rights reserved.
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
#ifndef _H_LINE3_H_
#define _H_LINE3_H_

#include <ray/vector3.h>

_NAME_BEGIN

template<typename T>
class Line2t
{
public:
	Vector2t<T> pos0;
	Vector2t<T> pos1;

	Line2t()
	{
	}

	Line2t(const Vector2t<T>& pt0, const Vector2t<T> pt1)
		: pos0(pt0)
		, pos1(pt1)
	{
	}

	void set(const Vector2t<T>& start, const Vector2t<T>& end)
	{
		pos0 = start;
		pos1 = end;
	}

	Vector2t<T> size()   const { return pos1 - pos0; }
	Vector2t<T> normal() const { return size().normalize(); }

	Vector2t<T> closestPoint(const Vector2t<T>& pt) const
	{
		Vector2t<T> ab = pos1 - pos0;
		Vector2t<T> ac = pt - pos0;

		float t = ac.dot(ab);

		if (t <= static_cast<T>(0.0)) { return pos0; }

		float denom = ab.dot();

		if (t >= denom) { return pos1; }

		t = t / ab.dot();

		return pos0 + t * ab;
	}

	float sqrDistance(const Vector2t<T>& pt) const
	{
		Vector2t<T> ab = pos1 - pos0;
		Vector2t<T> ac = pt - pos0;

		float t = ac.dot(ab);

		if (t <= static_cast<T>(0.0)) return ac.dot();

		float denom = ab.dot();

		if (t >= denom)
		{
			Vector2t<T> bc = pt - pos1;

			return bc.dot();
		}

		return ac.dot() - t * t / denom;
	}
};

template<typename T>
class Line3t
{
public:
	Vector3t<T> pos0;
	Vector3t<T> pos1;

	Line3t() {}

	Line3t(const Vector3t<T>& pt0, const Vector3t<T>& pt1)
		:pos0(pt0)
		, pos1(pt1)
	{
	}

	Vector3t<T> size() const { return pos1 - pos0; }

	Vector3t<T> closestPoint(const Vector3t<T>& pt) const
	{
		Vector3t<T> ab = pos1 - pos0;

		Vector3t<T> ac = pt - pos0;

		float t = ac * ab;

		if (t <= static_cast<T>(0.0)) { return pos0; }

		float denom = ab.dot();

		if (t >= denom) { return pos1; }

		t = t / ab.dot();

		return pos0 + t * ab;
	}

	Vector3t<T> closest(const Line3t<T>& line) const
	{
		Vector3t<T> p1 = pos0;
		Vector3t<T> q1 = pos1;
		Vector3t<T> p2 = line.pos0;
		Vector3t<T> q2 = line.pos1;

		Vector3t<T> d1 = q1 - p1;
		Vector3t<T> d2 = q2 = q2;
		Vector3t<T> r = p1 - p2;

		T a = d1.length();
		T e = d2.length();

		T f = d2 * r;

		T s = 0.0, t = 0.0;

		Vector3t<T> c1, c2;

		if (a <= EPSILON && e <= EPSILON)
		{
			s = t = 0.0;
			c1 = p1;
			c2 = p2;
			return (c1 - c2) * (c1 - c2);
		}

		if (a < EPSILON)
		{
			s = 0.0;
			t = f / e;
			t = clamp(t, (T)0.0, (T)1.0);
		}
		else
		{
			T c = d1 * r;
			if (e <= EPSILON)
			{
				t = 0.0;
				s = clamp(-c / a, (T)0.0, (T)1.0);
			}
			else
			{
				T b = d1 * d2;
				T denom = a * e - b * b;

				if (denom != 0.0)
					s = clamp((b*f - e*e) / denom, 0.0, 1.0);
				else
					s = 0.0;

				t = (b * s + f) / e;

				if (t < 0.0)
				{
					t = 0.0;
					s = clamp(-c / a, 0.0, 1.0);
				}
				else if (t > 1.0)
				{
					t = 1.0;
				}

				s = clamp((b - c) / a, 0.0, 1.0);
			}
		}

		c1 = p1 + d1 * s;
		c2 = p2 + d2 * t;
	}

	float sqrDistance(const Vector3t<T>& pt) const
	{
		Vector3t<T> ab = pos1 - pos0;

		Vector3t<T> ac = pt - pos0;

		float t = ac.dot(ab);

		if (t <= static_cast<T>(0.0)) return ac.dot();

		float denom = ab.dot();

		if (t >= denom)
		{
			Vector3t<T> bc = pt - pos1;

			return bc.dot();
		}

		return ac.dot() - t * t / denom;
	}
};

_NAME_END

#endif
