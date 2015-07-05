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
#if _BUILD_PHYSIC
#include <ray/physics_heightmap_component.h>
#include <ray/physics_shape_terrain.h>

_NAME_BEGIN

__ImplementSubClass(PhysicsHeightMapComponent, PhysicsShapeComponent)

PhysicsHeightMapComponent::PhysicsHeightMapComponent() noexcept
{
	_shape = std::make_unique<PhysicsShapeTerrain>();
}

PhysicsHeightMapComponent::~PhysicsHeightMapComponent() noexcept
{
}

void
PhysicsHeightMapComponent::setHeightMap(std::size_t w, std::size_t z, int size, const char* data) noexcept
{
	_shape->setHeightMap(w, z, PHYDataType(size), data);
}

void
PhysicsHeightMapComponent::setSize(const Vector3& size) noexcept
{
	_shape->setSize(size);
}

const Vector3&
PhysicsHeightMapComponent::getSize() const noexcept
{
	return _shape->getSize();
}

void
PhysicsHeightMapComponent::setMinHeight(float min)
{
	_shape->setMinHeight(min);
}

void
PhysicsHeightMapComponent::setMaxHeight(float max)
{
	_shape->setMaxHeight(max);
}

void
PhysicsHeightMapComponent::setScaleHeight(float scale) noexcept
{
	_shape->setScaleHeight(scale);
}

float
PhysicsHeightMapComponent::getMinHeight() const noexcept
{
	return _shape->getMinHeight();
}

float
PhysicsHeightMapComponent::getMaxHeight() const noexcept
{
	return _shape->getMaxHeight();
}

float
PhysicsHeightMapComponent::getScaleHeight() const noexcept
{
	return _shape->getScaleHeight();
}

void
PhysicsHeightMapComponent::onActivate() noexcept
{
	if (_shape)
	{
		_shape->setup();
	}
}

void
PhysicsHeightMapComponent::onDeactivate() noexcept
{
	if (_shape)
	{
		_shape.reset();
		_shape = nullptr;
	}
}

PhysicsShapePtr
PhysicsHeightMapComponent::getCollisionShape() noexcept
{
	return _shape;
}

GameComponentPtr
PhysicsHeightMapComponent::clone() const noexcept
{
	auto shape = std::make_shared<PhysicsHeightMapComponent>();
	/*shape->_width = this->_width;
	shape->_depth = this->_depth;
	shape->_scale = this->_scale;
	shape->_minHeight = this->_minHeight;
	shape->_maxHeight = this->_maxHeight;
	shape->_flipQuadEdges = this->_flipQuadEdges;
	shape->_type = this->_type;
	shape->_data = this->_data;*/

	return shape;
}

_NAME_END
#endif