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
#ifndef _H_FRUSTUM_H_
#define _H_FRUSTUM_H_

#include <ray/plane.h>
#include <ray/mat4.h>

_NAME_BEGIN

template<typename T>
class Frustumt
{
public:
	typedef Frustumt<T> _Myt;

	enum FRUSTUM_CLASSIFICATIONS
	{
		FRUSTUM_LEFT,
		FRUSTUM_LEFT_INTERSECT,
		FRUSTUM_RIGHT,
		FRUSTUM_RIGHT_INTERSECT,
		FRUSTUM_TOP,
		FRUSTUM_TOP_INTERSECT,
		FRUSTUM_BOTTOM,
		FRUSTUM_BOTTOM_INTERSECT,
		FRUSTUM_NEAR,
		FRUSTUM_NEAR_INTERSECT,
		FRUSTUM_FAR,
		FRUSTUM_FAR_INTERSECT,
		FRUSTUM_CONTAINS
	};

	Frustumt() noexcept
	{
	}

	Frustumt(const Matrix4x4t<T>& m) noexcept
	{
		this->extract(m);
	}

	void makeFrustum(float angle, float ratio, float znear, float zfar, const Vector3t<T>& position, const Vector3t<T>& lookat, const Vector3t<T>& up) noexcept
	{
		Vector3t<T> z = position - lookat;
		z.normalize();

		Vector3t<T> x = up.cross(z);
		x.normalize();

		Vector3t<T> y = z.cross(x);

		Vector3t<T> nc = position - z * znear;
		Vector3t<T> fc = position - z * zfar;

		float tang = std::tan(degrees(angle) * 0.5);
		float nh = znear * tang;
		float nw = nh * ratio;
		float fh = zfar  * tang;
		float fw = fh * ratio;

		Vector3t<T> ntl = nc + y * nh - x * nw;
		Vector3t<T> ntr = nc + y * nh + x * nw;
		Vector3t<T> nbl = nc - y * nh - x * nw;
		Vector3t<T> nbr = nc - y * nh + x * nw;

		Vector3t<T> ftl = fc + y * fh - x * fw;
		Vector3t<T> ftr = fc + y * fh + x * fw;
		Vector3t<T> fbl = fc - y * fh - x * fw;
		Vector3t<T> fbr = fc - y * fh + x * fw;

		_top.compute(ntl, ntr, ftl);
		_bottom.compute(nbr, nbl, fbr);
		_left.compute(nbl, ntl, fbl);
		_right.compute(ntr, nbr, fbr);
		_near.compute(ntr, ntl, nbr);
		_far.compute(ftl, ftr, fbl);
	}

	void extract(const Matrix4x4t<T>& m, bool normalize = false) noexcept
	{
		_left.normal.x = m.d1 + m.a1;
		_left.normal.y = m.d2 + m.a2;
		_left.normal.z = m.d3 + m.a3;
		_left.distance = m.d4 + m.a4;

		_right.normal.x = m.d1 - m.a1;
		_right.normal.y = m.d2 - m.a2;
		_right.normal.z = m.d3 - m.a3;
		_right.distance = m.d4 - m.a4;

		_bottom.normal.x = m.d1 + m.b1;
		_bottom.normal.y = m.d2 + m.b2;
		_bottom.normal.z = m.d3 + m.b3;
		_bottom.distance = m.d4 + m.b4;

		_top.normal.x = m.d1 - m.b1;
		_top.normal.y = m.d2 - m.b2;
		_top.normal.z = m.d3 - m.b3;
		_top.distance = m.d4 - m.b4;

		_near.normal.x = m.d1 + m.c1;
		_near.normal.y = m.d2 + m.c2;
		_near.normal.z = m.d3 + m.c3;
		_near.distance = m.d4 + m.c4;

		_far.normal.x = m.d1 - m.c1;
		_far.normal.y = m.d2 - m.c2;
		_far.normal.z = m.d3 - m.c3;
		_far.distance = m.d4 - m.c4;

		if (normalize)
		{
			_left.normalize();
			_right.normalize();
			_top.normalize();
			_bottom.normalize();
			_near.normalize();
			_far.normalize();
		}
	}

	bool contains(const Vector3t<T>& pt) const noexcept
	{
		if ((_left.getDistance(pt) < 0.0f) ||
			(_right.getDistance(pt) < 0.0f) ||
			(_top.getDistance(pt) < 0.0f) ||
			(_bottom.getDistance(pt) < 0.0f) ||
			(_near.getDistance(pt) < 0.0f) ||
			(_far.getDistance(pt) < 0.0f))
		{
			return false;
		}

		return true;
	}

	bool contains(const Rect3t<T>& rc) const  noexcept
	{
		if (!_left.classify(rc) ||
			!_right.classify(rc) ||
			!_top.classify(rc) ||
			!_bottom.classify(rc) ||
			!_near.classify(rc) ||
			!_far.classify(rc))
		{
			return false;
		}

		return true;
	}

	bool contains(const AABBt<T>& box) const noexcept
	{
		auto _contains = [](const Plane3t<T>& plane, const AABBt<T>& box)
		{
			Vector3t<T> min = box.min;
			Vector3t<T> max = box.max;

			if (plane.normal.x <= 0)
				std::swap(min.x, max.x);

			if (plane.normal.y <= 0)
				std::swap(min.y, max.y);

			if (plane.normal.z <= 0)
				std::swap(min.z, max.z);

			return (math::dot(plane.normal, max) + plane.distance) < 0 ? false : true;
		};

		if (_contains(_left, box) && _contains(_right, box) &&
			_contains(_top, box) && _contains(_bottom, box) &&
			_contains(_near, box) && _contains(_far, box))
		{
			return true;
		}

		return false;
	}

	bool contains(const Spheret<T>& sphere) const  noexcept
	{
		if ((_left.getDistance(sphere.center()) < -sphere.radius()) ||
			(_right.getDistance(sphere.center()) < -sphere.radius()) ||
			(_top.getDistance(sphere.center()) < -sphere.radius()) ||
			(_bottom.getDistance(sphere.center()) < -sphere.radius()) ||
			(_near.getDistance(sphere.center()) < -sphere.radius()) ||
			(_far.getDistance(sphere.center()) < -sphere.radius()))
		{
			return false;
		}

		return true;
	}

	bool contains(const Vector3t<T>& p, float radius) const  noexcept
	{
		if ((_left.getDistance(p) < -radius) ||
			(_right.getDistance(p) < -radius) ||
			(_top.getDistance(p) < -radius) ||
			(_bottom.getDistance(p) < -radius) ||
			(_near.getDistance(p) < -radius) ||
			(_far.getDistance(p) < -radius))
		{
			return false;
		}

		return true;
	}

	int classify(const Vector3t<T>& pt) const  noexcept
	{
		if (_left.classify(pt) != Plane3t<T>::PLANE_INTERSECT) { return FRUSTUM_LEFT; }
		if (_right.classify(pt) != Plane3t<T>::PLANE_INTERSECT) { return FRUSTUM_RIGHT; }
		if (_top.classify(pt) != Plane3t<T>::PLANE_INTERSECT) { return FRUSTUM_TOP; }
		if (_bottom.classify(pt) != Plane3t<T>::PLANE_INTERSECT) { return FRUSTUM_BOTTOM; }
		if (_near.classify(pt) != Plane3t<T>::PLANE_INTERSECT) { return FRUSTUM_NEAR; }
		if (_far.classify(pt) != Plane3t<T>::PLANE_INTERSECT) { return FRUSTUM_FAR; }

		return FRUSTUM_CONTAINS;
	}

	int classify(const Rect3t<T>& rc) const  noexcept
	{
		if (!_left.classify(rc)) { return FRUSTUM_LEFT; }
		if (!_right.classify(rc)) { return FRUSTUM_RIGHT; }
		if (!_top.classify(rc)) { return FRUSTUM_TOP; }
		if (!_bottom.classify(rc)) { return FRUSTUM_BOTTOM; }
		if (!_near.classify(rc)) { return FRUSTUM_NEAR; }
		if (!_far.classify(rc)) { return FRUSTUM_FAR; }

		return FRUSTUM_CONTAINS;
	}

	int classify(const Spheret<T>& sphere) const  noexcept
	{
		if ((_left.getDistance(sphere.center) < -sphere.radius) ||
			(_right.getDistance(sphere.center) < -sphere.radius) ||
			(_top.getDistance(sphere.center) < -sphere.radius) ||
			(_far.getDistance(sphere.center) < -sphere.radius) ||
			(_near.getDistance(sphere.center) < -sphere.radius) ||
			(_bottom.getDistance(sphere.center) < -sphere.radius))
		{
			return classify(sphere.center);
		}

		return FRUSTUM_CONTAINS;
	}

	float getFar() const noexcept
	{
		return _far.distance;
	}

	float getNear() const noexcept
	{
		return _near.distance;
	}

private:
	Plane3t<T> _left;
	Plane3t<T> _right;
	Plane3t<T> _top;
	Plane3t<T> _bottom;
	Plane3t<T> _near;
	Plane3t<T> _far;
};

_NAME_END

#endif