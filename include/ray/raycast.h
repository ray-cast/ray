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
#ifndef _H_RAYCAST_H_
#define _H_RAYCAST_H_

#include <ray/vector3.h>

_NAME_BEGIN

template<typename T>
class Raycast3t
{
public:
    Vector3t<T> origin;
    Vector3t<T> normal;

    Vector3t<T> min, max;

    Raycast3t()
    {
    }

    Raycast3t(const Vector3t<T>& pt1, const Vector3t<T>& pt2)
    {
        origin = pt1;
        normal = ~(pt2 - pt1);
        this->min = pt1.z < pt2.z ? pt1 : pt2;
        this->max = pt1.z > pt2.z ? pt1 : pt2;
    }

    Raycast3t(const Raycast3t& o)
        :origin(o.origin)
        , normal(o.normal)
    {
    }

    void setNormal(const Vector3t<T>& n)
    {
        normal = n;
        normal.normalize();
    }

    Vector3t<T> getPoint(float distance) const
    {
        return origin + normal * distance;
    }

    Vector3t<T> closestPoint(const Vector3t<T>& pt) const
    {
        Vector3t<T> vector = pt - origin;

        float t = normal.dot(vector);

        if (t < static_cast<T>(0.0)) return origin;

        return origin + t * normal;
    }

    Vector3t<T> closest(const Raycast3t<T> ray) const
    {
    }

    float sqrDistance(const Vector3t<T>& pt) const
    {
        Vector3t<T> ac = pt - origin;

        float t = normal.dot(ac);

        if (t <= static_cast<T>(0.0)) return ac.dot();

        float denom = normal.dot();

        return ac.dot() - t * t / denom;
    }
};

_NAME_END

#endif
