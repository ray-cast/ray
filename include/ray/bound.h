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
#ifndef _H_BOUND_H_
#define _H_BOUND_H_

#include <ray/sphere.h>

_NAME_BEGIN

template<typename T>
class Boundt
{
public:
    Boundt() noexcept
    {
        this->reset();
    }

    Boundt(const Vector3t<T> pt[], int n) noexcept
    {
        this->reset();
        this->encapsulate(pt, n);
    }

    Vector3t<T> size() const noexcept
    {
        return _sp.aabb().size();
    }

    Vector3t<T> extents() const noexcept
    {
        return _sp.aabb().extents();
    }

    const Vector3t<T>& center() const noexcept
    {
        return _sp.center();
    }

    void center(const Vector3t<T>& center) noexcept
    {
        _sp.center(center);
    }

    Vector3t<T>& min() noexcept
    {
        return this->aabb().min;
    }

    Vector3t<T>& max() noexcept
    {
        return this->aabb().max;
    }

    const Vector3t<T>& min() const noexcept
    {
        return this->aabb().min;
    }

    const Vector3t<T>& max() const noexcept
    {
        return this->aabb().max;
    }

    bool empty() const noexcept
    {
        return _sp.empty();
    }

    void reset() noexcept
    {
        _sp.reset();
    }

    T radius() const noexcept
    {
        return _sp.radius;
    }

    Spheret<T>& sphere() noexcept
    {
        return _sp;
    }

    const Spheret<T>& sphere() const noexcept
    {
        return _sp;
    }

    AABBt<T>& aabb() noexcept
    {
        return _sp.aabb();
    }

    const AABBt<T>& aabb() const noexcept
    {
        return _sp.aabb();
    }

    void encapsulate(const Vector3t<T>& pt) noexcept
    {
        _sp.encapsulate(pt);
    }

    void encapsulate(const Vector3t<T> pt[], std::size_t n) noexcept
    {
        _sp.encapsulate(pt, n);
    }

    void encapsulate(const Boundt<T>& bound) noexcept
    {
        _sp.encapsulate(bound._sp);
    }

    bool intersects(const Raycast3t<T>& ray) const noexcept
    {
        return _sp.intersects(ray);
    }

    void applyMatrix(const Matrix4x4t<T>& m) noexcept
    {
        _sp.applyMatrix(m);
    }

private:

    Spheret<T> _sp;
};

_NAME_END

#endif