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
#include <ray/physics_shape_box.h>

#include <btBulletCollisionCommon.h>

_NAME_BEGIN

PhysicsShapeBox::PhysicsShapeBox() noexcept
    : _size(Vector3::One)
    , _shape(nullptr)
{
}

PhysicsShapeBox::~PhysicsShapeBox() noexcept
{
    this->close();
}

void
PhysicsShapeBox::setup() noexcept
{
    assert(!_shape);

    btVector3 margin;
    margin.setX(_size.x * 0.5f);
    margin.setY(_size.y * 0.5f);
    margin.setZ(_size.z * 0.5f);

    _shape = new btBoxShape(margin);
    _shape->setUserPointer(this);
}

void
PhysicsShapeBox::close() noexcept
{
    if (_shape)
    {
        delete _shape;
        _shape = nullptr;
    }
}

void
PhysicsShapeBox::setSize(const Vector3& size) noexcept
{
    if (_shape)
    {
        btVector3 margin;
        margin.setX(size.x);
        margin.setY(size.y);
        margin.setZ(size.z);

        _shape->setSafeMargin(margin);
    }

    _size = size;
}

const Vector3&
PhysicsShapeBox::getSize() const noexcept
{
    return _size;
}

btCollisionShape*
PhysicsShapeBox::getCollisionShape() noexcept
{
    return _shape;
}

_NAME_END