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
#include <ray/physics_joint_configurable.h>
#include <ray/physics_system.h>
#include "bullet_types.h"

_NAME_BEGIN

PhysicsJointConfigurable::PhysicsJointConfigurable() noexcept
	: _lowLimit(float3::Zero)
	, _highLimit(float3::Zero)
	, _angularLowLimit(float3::Zero)
	, _angularHighLimit(float3::Zero)
	, _movementConstant(float3::Zero)
	, _rotationConstant(float3::Zero)
	, _translate(float3::Zero)
	, _quaternion(Quaternion::Zero)
{
}

PhysicsJointConfigurable::~PhysicsJointConfigurable() noexcept
{
	this->onDeactivate();
}

void 
PhysicsJointConfigurable::setLinearSpring(const float3& spring) noexcept
{
	_translate = spring;
}

void 
PhysicsJointConfigurable::setAngularSprint(const Quaternion& spring) noexcept
{
	_quaternion = spring;
}

void
PhysicsJointConfigurable::setLinearLowerLimit(const float3& limit) noexcept
{
	if (_joint)
		_joint->setLinearLowerLimit(btVector3(_lowLimit.x, _lowLimit.y, _lowLimit.z));
	_lowLimit = limit;
}

void
PhysicsJointConfigurable::setLinearHighLimit(const float3& limit) noexcept
{
	if (_joint)
		_joint->setLinearUpperLimit(btVector3(_highLimit.x, _highLimit.y, _highLimit.z));
	_highLimit = limit;
}

void
PhysicsJointConfigurable::setAngularLowerLimit(const float3& limit) noexcept
{
	if (_joint)
		_joint->setAngularLowerLimit(btVector3(_angularLowLimit.x, _angularLowLimit.y, _angularLowLimit.z));
	_angularLowLimit = limit;
}

void
PhysicsJointConfigurable::setAngularHighLimit(const float3& limit) noexcept
{
	if (_joint)
		_joint->setAngularUpperLimit(btVector3(_angularHighLimit.x, _angularHighLimit.y, _angularHighLimit.z));
	_angularHighLimit = limit;
}

void 
PhysicsJointConfigurable::setMovementConstant(const float3& constant) noexcept
{
	if (_joint)
	{
		if (constant.x != 0.0f)
		{
			_joint->enableSpring(0, true);
			_joint->setStiffness(0, constant.x);
		}

		if (constant.y != 0.0f)
		{
			_joint->enableSpring(1, true);
			_joint->setStiffness(1, constant.y);
		}

		if (constant.z != 0.0f)
		{
			_joint->enableSpring(2, true);
			_joint->setStiffness(2, constant.z);
		}
	}

	_movementConstant = constant;
}

void 
PhysicsJointConfigurable::setRotationConstant(const float3& constant) noexcept
{
	if (_joint)
	{
		if (constant.x != 0.0f)
		{
			_joint->enableSpring(3, true);
			_joint->setStiffness(3, constant.x);
		}

		if (constant.y != 0.0f)
		{
			_joint->enableSpring(4, true);
			_joint->setStiffness(4, constant.y);
		}

		if (constant.z != 0.0f)
		{
			_joint->enableSpring(5, true);
			_joint->setStiffness(5, constant.z);
		}
	}

	_rotationConstant = constant;
}

const float3&
PhysicsJointConfigurable::getLinearSpring() const noexcept
{
	return _translate;
}

const Quaternion&
PhysicsJointConfigurable::getAngularSprint() const noexcept
{
	return _quaternion;
}

const float3&
PhysicsJointConfigurable::getLinearLowerLimit() const noexcept
{
	return _lowLimit;
}

const float3&
PhysicsJointConfigurable::getLinearHighLimit() const noexcept
{
	return _highLimit;
}

const float3&
PhysicsJointConfigurable::getAngularLowerLimit() const noexcept
{
	return _angularLowLimit;
}

const float3&
PhysicsJointConfigurable::getAngularHighLimit() const noexcept
{
	return _angularHighLimit;
}

const float3& 
PhysicsJointConfigurable::getMovementConstant() const noexcept
{
	return _movementConstant;
}

const float3& 
PhysicsJointConfigurable::getRotationConstant() const noexcept
{
	return _rotationConstant;
}

void
PhysicsJointConfigurable::onActivate() noexcept
{
	assert(!_joint);
	assert(this->getRawRigidbody());
	assert(this->getRawConnectRigidbody());

	auto bodyA = this->getRawRigidbody();
	auto bodyB = this->getRawConnectRigidbody();

	btTransform	 bttrTransform;

	bttrTransform.setIdentity();
	bttrTransform.setOrigin(btVector3(_translate.x, _translate.y, _translate.z));
	bttrTransform.setRotation(btQuaternion(_quaternion.x, _quaternion.y, _quaternion.z, _quaternion.w));

	btTransform bttrRigidAInvTransform = bodyA->getWorldTransform().inverse();
	btTransform bttrRigidBInvTransform = bodyB->getWorldTransform().inverse();

	bttrRigidAInvTransform = bttrRigidAInvTransform * bttrTransform;
	bttrRigidBInvTransform = bttrRigidBInvTransform * bttrTransform;

	_joint = std::make_unique<btGeneric6DofSpringConstraint>(*bodyA, *bodyB, bttrRigidAInvTransform, bttrRigidBInvTransform, true);

	this->setLinearLowerLimit(_lowLimit);
	this->setLinearHighLimit(_highLimit);

	this->setAngularLowerLimit(_angularLowLimit);
	this->setAngularHighLimit(_angularHighLimit);

	this->setMovementConstant(_movementConstant);
	this->setRotationConstant(_rotationConstant);

	this->setPhysicsScene(PhysicsSystem::instance()->getPhysicsScene());
}

void
PhysicsJointConfigurable::onDeactivate() noexcept
{
	this->setPhysicsScene(nullptr);
	_joint.reset();
}

void
PhysicsJointConfigurable::setPhysicsScene(PhysicsScenePtr scene) noexcept
{
	if (_scene.lock())
		_scene.lock()->removeJoint(_joint.get());

	_scene = scene;

	if (scene)
		scene->addJoint(_joint.get());
}

_NAME_END