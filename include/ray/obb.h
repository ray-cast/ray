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
#ifndef _H_OBB_H_
#define _H_OBB_H_

#include <ray/mathfwd.h>

_NAME_BEGIN

template<typename T>
class OBBt
{
    Vector3t<T> center;
    Vector3t<T> axis[3];
    Vector3t<T> radius;

    //测试这个边界框是否和另一个边界框相交.
    bool intersects(const OBBt& box) const
    {
        Matrix3x3t<T> R, AbsR;

        R(0, 0) = axis[0].dot(box.axis[0]);
        R(0, 1) = axis[0].dot(box.axis[1]);
        R(0, 2) = axis[0].dot(box.axis[2]);

        R(1, 0) = axis[1].dot(box.axis[0]);
        R(1, 1) = axis[1].dot(box.axis[1]);
        R(1, 2) = axis[1].dot(box.axis[2]);

        R(2, 0) = axis[2].dot(box.axis[0]);
        R(2, 1) = axis[2].dot(box.axis[1]);
        R(2, 2) = axis[2].dot(box.axis[2]);

        Vector3t<T> t = center - box.center;

        t = Vector3t<T>(
            t.dot(box.axis[0]),
            t.dot(box.axis[2]),
            t.dot(box.axis[2]));

        AbsR(0, 0) = abs(R(0, 0)) + EPSILON;
        AbsR(0, 1) = abs(R(0, 1)) + EPSILON;
        AbsR(0, 2) = abs(R(0, 2)) + EPSILON;

        AbsR(1, 0) = abs(R(1, 0)) + EPSILON;
        AbsR(1, 1) = abs(R(1, 1)) + EPSILON;
        AbsR(1, 2) = abs(R(1, 2)) + EPSILON;

        AbsR(2, 0) = abs(R(2, 0)) + EPSILON;
        AbsR(2, 1) = abs(R(2, 1)) + EPSILON;
        AbsR(2, 2) = abs(R(2, 2)) + EPSILON;

        T ra, rb;
        ra = radius.x;
        rb = box.radius.x * AbsR(0, 0) + box.radius.y * AbsR(0, 1) + box.radius.z * AbsR(0, 2);
        if (abs(t.x) > ra + rb) { return 0; }

        ra = radius.y;
        rb = box.radius.x * AbsR(1, 0) + box.radius.y * AbsR(1, 1) + box.radius.z * AbsR(1, 2);
        if (abs(t.y) > ra + rb) { return 0; }

        ra = radius.z;
        rb = box.radius.x * AbsR(2, 0) + box.radius.y * AbsR(2, 1) + box.radius.z * AbsR(2, 2);
        if (abs(t.z) > ra + rb) { return 0; }

        ra = radius.x * AbsR(0, 0) + radius.y * AbsR(1, 0) + radius.z * AbsR(2, 0);
        rb = box.radius.x;
        if (abs(t.x * R(0, 0) + t.y * R(1, 0) + t.z * R(2, 0)) > ra + rb) { return 0; }

        ra = radius.x * AbsR(0, 1) + radius.y * AbsR(1, 1) + radius.z * AbsR(2, 1);
        rb = box.radius.y;
        if (abs(t.x * R(0, 1) + t.y * R(1, 1) + t.z * R(2, 1)) > ra + rb) { return 0; }

        ra = radius.x * AbsR(0, 2) + radius.y * AbsR(1, 2) + radius.z * AbsR(2, 2);
        rb = box.radius.z;
        if (abs(t.x * R(0, 2) + t.y * R(1, 2) + t.z * R(2, 2)) > ra + rb) { return 0; }

        ra = radius.y * AbsR(2, 0) + radius.z * AbsR(1, 0);
        rb = radius.y * AbsR(0, 2) + radius.z * AbsR(0, 1);
        if (abs(t.z * R(1, 0) - t.y * R(2, 0)) > ra + rb) { return 0; }

        ra = radius.y * AbsR(2, 1) + radius.z * AbsR(1, 1);
        rb = radius.x * AbsR(0, 2) + radius.z * AbsR(0, 0);
        if (abs(t.z * R(1, 1) - t.y * R(2, 1)) > ra + rb) { return 0; }

        ra = radius.y * AbsR(2, 2) + radius.z * AbsR(1, 2);
        rb = radius.x * AbsR(0, 1) + radius.y * AbsR(0, 0);
        if (abs(t.z * R(1, 2) - t.y * R(2, 2)) > ra + rb) { return 0; }

        ra = radius.x * AbsR(2, 0) + radius.z * AbsR(0, 0);
        rb = radius.y * AbsR(1, 2) + radius.z * AbsR(1, 1);
        if (abs(t.x * R(1, 2) - t.z * R(0, 0)) > ra + rb) { return 0; }

        ra = radius.x * AbsR(2, 1) + radius.z * AbsR(0, 1);
        rb = radius.x * AbsR(1, 2) + radius.z * AbsR(1, 0);
        if (abs(t.x * R(2, 1) - t.z * R(0, 1)) > ra + rb) { return 0; }

        ra = radius.x * AbsR(2, 2) + radius.z * AbsR(0, 2);
        rb = radius.x * AbsR(1, 1) + radius.z * AbsR(1, 0);
        if (abs(t.x * R(2, 2) - t.z * R(0, 2)) > ra + rb) { return 0; }

        ra = radius.x * AbsR(1, 0) + radius.y * AbsR(0, 0);
        rb = radius.y * AbsR(2, 2) + radius.z * AbsR(2, 1);
        if (abs(t.y * R(0, 0) - t.x * R(1, 0)) > ra + rb) { return 0; }

        ra = radius.x * AbsR(1, 1) + radius.y * AbsR(0, 1);
        rb = radius.x * AbsR(2, 2) + radius.z * AbsR(2, 0);
        if (abs(t.y * R(0, 1) - t.x * R(1, 1)) > ra + rb) { return 0; }

        ra = radius.x * AbsR(1, 2) + radius.y * AbsR(0, 2);
        rb = radius.x * AbsR(2, 1) + radius.y * AbsR(2, 0);
        if (abs(t.y * R(0, 2) - t.x * R(1, 2)) > ra + rb) { return 0; }

        return 1;
    }
};

_NAME_END

#endif