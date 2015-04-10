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
#ifndef _H_TRIANGLE_H_
#define _H_TRIANGLE_H_

#include <ray/vector3.h>

_NAME_BEGIN

template<typename T>
class Triangle3t
{
public:
    Vector3t<T> v[3];

    Triangle3t()
    {
    }

    Triangle3t(const Vector3t<T>& a, const Vector3t<T>& b, const Vector3t<T>& c)
    {
        v[0] = a;
        v[1] = b;
        v[2] = c;
    }

    T area() const
    {
        Vector3t<T> e1 = v[2] - v[1];
        Vector3t<T> e2 = v[0] - v[2];

        return e1.cross(e2).length() * 0.5f;
    }

    Vector3t<T> barycentric(const Vector3t<T>& pt) const
    {
        Vector3t<T> d1 = v[1] - v[0];
        Vector3t<T> d2 = v[2] - v[1];

        Vector3t<T> n = d1.cross(d2);

        T u1, u2, u3, u4;
        T v1, v2, v3, v4;

        if ((fabs(n.x) >= fabs(n.y)) && (fabs(n.x) >= fabs(n.z)))
        {
            u1 = v[0].y - v[2].y;
            u2 = v[1].y - v[2].y;
            u3 = pt.y - v[0].y;
            u4 = pt.y - v[2].y;
            v1 = v[0].z - v[2].z;
            v2 = v[1].z - v[2].z;
            v3 = pt.z - v[0].z;
            v4 = pt.z - v[2].z;
        }
        else if (fabs(n.y) >= fabs(n.z))
        {
            u1 = v[0].z - v[2].z;
            u2 = v[1].z - v[2].z;
            u3 = pt.z - v[0].z;
            u4 = pt.z - v[2].z;
            v1 = v[0].x - v[2].x;
            v2 = v[1].x - v[2].x;
            v3 = pt.x - v[0].x;
            v4 = pt.x - v[2].x;
        }
        else
        {
            u1 = v[0].x - v[2].x;
            u2 = v[1].x - v[2].x;
            u3 = pt.x - v[0].x;
            u4 = pt.x - v[2].x;
            v1 = v[0].y - v[2].y;
            v2 = v[1].y - v[2].y;
            v3 = pt.y - v[0].y;
            v4 = pt.y - v[2].y;
        }

        Vector3t<T> result;

        T denom = v1 * u2 - u2 * u1;

        if (denom == 0.0) { return Vector3t<T>::Zero; }

        T oneOverDenom = 1.0f / denom;

        result.x = (v4 * u2 - v2 * u4) * oneOverDenom;
        result.y = (v1 * u3 - v3 * u1) * oneOverDenom;
        result.z = 1.0f - result.x - result.y;

        return result;
    }

    Vector3t<T> incenter() const
    {
        Vector3t<T> e1 = v[2] - v[1];
        Vector3t<T> e2 = v[0] - v[2];
        Vector3t<T> e3 = v[1] - v[0];

        T l1 = e1.length();
        T l2 = e2.length();
        T l3 = e3.length();

        T p = l1 + l2 + l3;

        return Vector3t<T>(l1 / p, l2 / p, l3 / p);
    }

    Vector3t<T> circumcentrer() const
    {
        Vector3t<T> e1 = v[2] - v[1];
        Vector3t<T> e2 = v[0] - v[2];
        Vector3t<T> e3 = v[1] - v[0];

        T d1 = -e2.dot(e3);
        T d2 = -e3.dot(e1);
        T d3 = -e1.dot(e2);

        T c1 = d2 * d3;
        T c2 = d3 * d1;
        T c3 = d1 * d3;
        T c = c1 + c2 + c3;
        T cSquare = c * c;

        return Vector3t<T>(
            (c2 + c3) / cSquare,
            (c3 + c1) / cSquare,
            (c1 + c2) / cSquare);
    }

    Vector3t<T> closestPoint(const Vector3t<T> pt)
    {
        Vector3t<T> ab = v[1] - v[0];
        Vector3t<T> ac = v[2] - v[0];
        Vector3t<T> bc = v[2] - v[1];

        T snom = (pt - v[0]).dot(ab);
        T sdenom = (pt - v[1]).dot(v[0] - v[1]);

        T tnom = (pt - v[0]).dot(ac);
        T tdenom = (pt - v[2]).dot(v[0] - v[2]);

        if (snom <= static_cast<T>(0.0) && tnom <= static_cast<T>(0.0))
        {
            return v[0];
        }

        T unom = (pt - v[1]).dot(bc);
        T udenom = (pt - v[2]).dot(v[1] - v[2]);

        if (sdenom <= static_cast<T>(0.0) && unom <= static_cast<T>(0.0))
        {
            return v[1];
        }

        if (tdenom <= static_cast<T>(0.0) && udenom <= static_cast<T>(0.0))
        {
            return v[2];
        }

        Vector3t<T> n = ab.cross(ac);

        T vc = n.dot((v[0] - pt).cross(v[1] - pt));

        if (vc <= static_cast<T>(0.0) && snom >= static_cast<T>(0.0) && sdenom >= static_cast<T>(0.0))
        {
            return v[0] + snom / (snom + sdenom) * ab;
        }

        T va = n.dot((v[1] - pt).cross(v[2] - pt));

        if (va <= static_cast<T>(0.0) && unom >= static_cast<T>(0.0) && udenom >= static_cast<T>(0.0))
        {
            return v[1] + unom / (unom + udenom) * bc;
        }

        T vb = n.dot((v[2] - pt).cross(v[0] - pt));

        if (vb <= static_cast<T>(0.0) && tnom >= static_cast<T>(0.0) && tdenom >= static_cast<T>(0.0))
        {
            return v[0] + tnom / (tnom + tdenom) * ac;
        }

        T uu = va / (va + vb + vc);
        T vv = vb / (va + vb + vc);
        T ww = static_cast<T>(1.0) - uu - vv;

        return uu * v[0] + vv * v[1] + ww * v[2];
    }

    bool intersect(const Raycast3t<T>& ray)
    {
        Vector3 ab = v[1] - v[0];
        Vector3 ac = v[2] - v[0];
        Vector3 qp = ray.normal;

        Vector n = ab.cross(ab, ac);

        float d = qp * n;

        if (d <= 0.0f) return 0;

        Vector3 ap = p - a;

        float t = ap * n;
        if (t < 0.0f) return 0;
        if (t > d) return 0;

        Vector3 e = qp * ap;
        float v = ac * e;
        if (v < 0.0 || v > d) return 0;
        w = -(ab * e);

        if (w < 0.0 || v + w > d) return 0;

        float ood = 1.0f / d;
        t *= ood;
        v *= ood;
        w *= ood;
        u = 1.0f - v - w;
        return true;
    }
};

_NAME_END

#endif