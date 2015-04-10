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
#ifndef _H_RECT3_H_
#define _H_RECT3_H_

#include <ray/vector3.h>

_NAME_BEGIN

template<typename T>
class Rect3t
{
public:

    Vector3t<T> pos0;
    Vector3t<T> pos1;

    Rect3t() {};
    Rect3t(const Rect3t& copy) :pos0(copy.pos0), pos1(copy.pos1) {}
    Rect3t(float x0, float x1, float y0, float y1, float z)
    {
        pos0.x = x0; pos0.y = y0; pos0.z = z;
        pos1.x = x1; pos1.y = y1; pos1.z = z;
    }

    Rect3t(const Vector2t<T>& pt0, const Vector2t<T>& pt1, float z)
    {
        pos0.x = pt0.x; pos0.y = pt0.y; pos0.z = z;
        pos1.x = pt1.x; pos1.y = pt1.y; pos1.z = z;
    }

    Rect3t(float cx, float cy)
    {
        pos0.x = pos0.y = pos0.z = pos1.z = static_cast<T>(0.0);
        pos1.x = cx; pos1.y = cy;
    }

    Rect3t(const Vector3t<T>& pos, float cx, float cy)
    {
        pos0 = pos;
        pos1.x = pos0.x + cx;
        pos1.y = pos0.y + cy;
        pos1.z = pos0.z;
    }

    Rect3t(const Vector2t<T>& sz)
    {
        pos0.x = pos0.y = pos0.z = pos1.z = static_cast<T>(0.0);
        pos1.x = sz.x; pos1.y = sz.y;
    }

    void set(const Rect3t& src)
    {
        pos0 = src.pos0;
        pos1 = src.pos1;
    }

    void set(float _x0, float _x1, float _y0, float _y1, float _z)
    {
        pos0.x = _x0;
        pos0.y = _y0;
        pos0.z = _z;
        pos1.x = _x1;
        pos1.y = _y1;
        pos1.z = _z;
    }

    void setSize(float cx, float cy) { pos1.x = cx; pos1.y = cy; }
    void setSize(const Vector2t<T>& size) { pos1 = size; }
    void setPos(const Vector3t<T>& pos) { pos1 = pos1 - pos0 + pos; pos0 = pos; }

    Vector3t<T> size()const { return pos1 - pos0; }
};

_NAME_END

#endif