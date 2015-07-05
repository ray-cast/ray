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
#include <ray/physics_capsule_component.h>
#include <ray/physics_shape_capsule.h>

_NAME_BEGIN

__ImplementSubClass(PhysicsCapsuleComponent, PhysicsShapeComponent)

PhysicsCapsuleComponent::PhysicsCapsuleComponent() noexcept
{
	_shape = std::make_shared<PhysicsShapeCapsule>();
}

PhysicsCapsuleComponent::PhysicsCapsuleComponent(float width, float height) noexcept
{
	_shape = std::make_shared<PhysicsShapeCapsule>();
	_shape->setWidth(width);
	_shape->setHeight(height);
}

PhysicsCapsuleComponent::~PhysicsCapsuleComponent() noexcept
{
}

void
PhysicsCapsuleComponent::setWidth(float width) noexcept
{
	_shape->setWidth(width);
}

void
PhysicsCapsuleComponent::setHeight(float height) noexcept
{
	_shape->setHeight(height);
}

void
PhysicsCapsuleComponent::onActivate() noexcept
{
	_shape->setup();
}

void
PhysicsCapsuleComponent::onDeactivate() noexcept
{
	_shape->close();
}

PhysicsShapePtr
PhysicsCapsuleComponent::getCollisionShape() noexcept
{
	return _shape;
}

GameComponentPtr
PhysicsCapsuleComponent::clone() const noexcept
{
	auto component = std::make_shared<PhysicsCapsuleComponent>();
	component->setWidth(_shape->getWidth());
	component->setHeight(_shape->getHeight());

	return component;
}

_NAME_END
#endif