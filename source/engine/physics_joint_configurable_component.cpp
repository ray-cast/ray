// +----------------------------------------------------------------------
// | Project : ray.
// | All rights reserved.
// +----------------------------------------------------------------------
// | Copyright (c) 2013-2017.
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
#include <ray/physics_joint_configurable_component.h>
#include <ray/physics_joint_configurable.h>
#include <ray/physics_body_component.h>

_NAME_BEGIN

__ImplementSubClass(PhysicsJointConfigurableComponent, PhysicsJointComponent, "PhysicsJointConfigurable")

PhysicsJointConfigurableComponent::PhysicsJointConfigurableComponent() noexcept
{
	_joint = std::make_unique<PhysicsJointConfigurable>();
	_joint->setOwnerListener(this);
}

PhysicsJointConfigurableComponent::~PhysicsJointConfigurableComponent() noexcept
{
}

void 
PhysicsJointConfigurableComponent::setLinearSpring(const float3& spring) noexcept
{
	_joint->setLinearSpring(spring);
}

void 
PhysicsJointConfigurableComponent::setAngularSprint(const Quaternion& spring) noexcept
{
	_joint->setAngularSprint(spring);
}

void 
PhysicsJointConfigurableComponent::setLinearLowerLimit(const float3& limit) noexcept
{
	_joint->setLinearLowerLimit(limit);
}

void 
PhysicsJointConfigurableComponent::setLinearHighLimit(const float3& limit) noexcept
{
	_joint->setLinearHighLimit(limit);
}

void 
PhysicsJointConfigurableComponent::setAngularLowerLimit(const float3& limit) noexcept
{
	_joint->setAngularLowerLimit(limit);
}

void 
PhysicsJointConfigurableComponent::setAngularHighLimit(const float3& limit) noexcept
{
	_joint->setAngularHighLimit(limit);
}

void 
PhysicsJointConfigurableComponent::setMovementConstant(const float3& constant) noexcept
{
	_joint->setMovementConstant(constant);
}

void 
PhysicsJointConfigurableComponent::setRotationConstant(const float3& constant) noexcept
{
	_joint->setRotationConstant(constant);
}

const float3& 
PhysicsJointConfigurableComponent::getLinearSpring() const noexcept
{
	return _joint->getLinearSpring();
}

const Quaternion& 
PhysicsJointConfigurableComponent::getAngularSprint() const noexcept
{
	return _joint->getAngularSprint();
}

const float3& 
PhysicsJointConfigurableComponent::getLinearLowerLimit() const noexcept
{
	return _joint->getLinearLowerLimit();
}

const float3& 
PhysicsJointConfigurableComponent::getLinearHighLimit() const noexcept
{
	return _joint->getLinearHighLimit();
}

const float3& 
PhysicsJointConfigurableComponent::getAngularLowerLimit() const noexcept
{
	return _joint->getAngularLowerLimit();
}

const float3& 
PhysicsJointConfigurableComponent::getAngularHighLimit() const noexcept
{
	return _joint->getAngularHighLimit();
}

const float3& 
PhysicsJointConfigurableComponent::getMovementConstant() const noexcept
{
	return _joint->getMovementConstant();
}

const float3& 
PhysicsJointConfigurableComponent::getRotationConstant() const noexcept
{
	return _joint->getRotationConstant();
}

void 
PhysicsJointConfigurableComponent::load(iarchive& reader) noexcept
{
}

void 
PhysicsJointConfigurableComponent::save(archivebuf& write) noexcept
{
}

GameComponentPtr 
PhysicsJointConfigurableComponent::clone() const noexcept
{
	auto joint = std::make_shared<PhysicsJointConfigurableComponent>();
	joint->setLinearSpring(this->getLinearSpring());
	joint->setAngularSprint(this->getAngularSprint());
	joint->setLinearLowerLimit(this->getLinearLowerLimit());
	joint->setLinearHighLimit(this->getLinearHighLimit());
	joint->setAngularLowerLimit(this->getAngularLowerLimit());
	joint->setAngularHighLimit(this->getAngularHighLimit());
	joint->setMovementConstant(this->getMovementConstant());
	joint->setRotationConstant(this->getRotationConstant());
	joint->setConnectRigidbody(this->getConnectRigidbody());
	return joint;
}

void 
PhysicsJointConfigurableComponent::onActivate() except
{
	if (!_joint->getActive())
	{
		auto rawRigidbodyA = this->getRawRigidbody();
		auto rawRigidbodyB = this->getRawConnectRigidbody();

		if (rawRigidbodyA && rawRigidbodyB)
		{
			_joint->setRigidbody(rawRigidbodyA);
			_joint->setConnectRigidbody(rawRigidbodyB);
			_joint->setActive(true);
		}
	}
}

void 
PhysicsJointConfigurableComponent::onDeactivate() noexcept
{
	_joint->setActive(false);
}

void 
PhysicsJointConfigurableComponent::onAttachComponent(GameComponentPtr& component) except
{
	if (component->isInstanceOf<PhysicsBodyComponent>())
	{
		if (this->getActive())
			this->onActivate();
	}
}

void 
PhysicsJointConfigurableComponent::onDetachComponent(GameComponentPtr& component) noexcept
{
	if (component->isInstanceOf<PhysicsBodyComponent>())
	{
		if (this->getActive())
			this->onDeactivate();
	}
}

void
PhysicsJointConfigurableComponent::onBodyChange() noexcept
{
	if (this->getActive())
	{
		this->onDeactivate();
		this->onActivate();
	}
}

_NAME_END