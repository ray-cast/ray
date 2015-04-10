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
#include <ray/physics_shape_terrain.h>

#include <BulletCollision\CollisionShapes\btHeightfieldTerrainShape.h>

_NAME_BEGIN

PhysicsShapeTerrain::PhysicsShapeTerrain() noexcept
    : _shape(nullptr)
    , _width(0)
    , _depth(0)
    , _type(PHYDataType::UCHAR)
    , _minHeight(0.0)
    , _maxHeight(1.0)
    , _scaleHeight(1.0)
    , _flipQuadEdges(false)
{
}

PhysicsShapeTerrain::~PhysicsShapeTerrain() noexcept
{
    this->close();
}

void
PhysicsShapeTerrain::setup() noexcept
{
    assert(_type == PHYDataType::UCHAR && _type == _type == PHYDataType::SHORT && _type == PHYDataType::INTEGER && _type == PHYDataType::FLOAT);

    PHY_ScalarType _scalarType;

    if (_type == PHYDataType::UCHAR)
        _scalarType = PHY_UCHAR;
    else if (_type == PHYDataType::SHORT)
        _scalarType = PHY_SHORT;
    else if (_type == PHYDataType::INTEGER)
        _scalarType = PHY_INTEGER;
    else if (_type == PHYDataType::FLOAT)
        _scalarType = PHY_FLOAT;
    else
        _scalarType = PHY_UCHAR;

    _shape = new btHeightfieldTerrainShape(_width, _depth, _data.data(), _scaleHeight, _minHeight, _maxHeight, 1, _scalarType, _flipQuadEdges);
    _shape->setUserPointer(this);

    this->setSize(_size);
}

void
PhysicsShapeTerrain::close() noexcept
{
    if (_shape)
    {
        delete _shape;
        _shape = nullptr;
    }
}

void
PhysicsShapeTerrain::setHeightMap(std::size_t w, std::size_t z, PHYDataType type, const char* data) noexcept
{
    assert(type == PHYDataType::UCHAR || type == PHYDataType::SHORT || type == PHYDataType::INTEGER || type == PHYDataType::FLOAT);

    _width = w;
    _depth = z;

    _type = type;

    if (type == PHYDataType::UCHAR)
        _data.resize(_width * _depth * sizeof(char));
    else if (type == PHYDataType::SHORT)
        _data.resize(_width * _depth * sizeof(short));
    else if (type == PHYDataType::INTEGER)
        _data.resize(_width * _depth * sizeof(int));
    else if (type == PHYDataType::FLOAT)
        _data.resize(_width * _depth * sizeof(float));

    std::memcpy(_data.data(), data, _data.size());
}

void
PhysicsShapeTerrain::setSize(const Vector3& size) noexcept
{
    if (_shape)
    {
        btVector3 sz;
        sz.setX(size.x);
        sz.setY(size.y);
        sz.setZ(size.z);

        _shape->setLocalScaling(sz);
    }

    _size = size;
}

const Vector3&
PhysicsShapeTerrain::getSize() const noexcept
{
    return _size;
}

void
PhysicsShapeTerrain::setMinHeight(float min)
{
    _minHeight = min;
}

void
PhysicsShapeTerrain::setMaxHeight(float max)
{
    _maxHeight = max;
}

void
PhysicsShapeTerrain::setScaleHeight(float scale) noexcept
{
    _scaleHeight = scale;
}

float
PhysicsShapeTerrain::getMinHeight() const noexcept
{
    return _minHeight;
}

float
PhysicsShapeTerrain::getMaxHeight() const noexcept
{
    return _maxHeight;
}

float
PhysicsShapeTerrain::getScaleHeight() const noexcept
{
    return _scaleHeight;
}

btCollisionShape*
PhysicsShapeTerrain::getCollisionShape() noexcept
{
    return _shape;
}

_NAME_END