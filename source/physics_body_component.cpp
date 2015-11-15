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
#include <ray/physics_body_component.h>
#include <ray/physics_shape_component.h>
#include <ray/physics_rigidbody.h>
#include <ray/physics_features.h>
#include <ray/game_server.h>

_NAME_BEGIN

__ImplementSubClass(PhysicsBodyComponent, GameComponent)

PhysicsBodyComponent::PhysicsBodyComponent() noexcept
	: _constantForce(Vector3::Zero)
	, _constantTorque(Vector3::Zero)
	, _constantVelocity(Vector3::Zero)
	, _constantAngularVelocity(Vector3::Zero)
	, _enableKinematic(false)
{
	_body = std::make_unique<PhysicsRigidbody>();
	_body->setRigidbodyListener(this);
}

PhysicsBodyComponent::~PhysicsBodyComponent() noexcept
{
	if (_body)
	{
		_body.reset();
		_body = nullptr;
	}
}

void
PhysicsBodyComponent::setMass(float value) noexcept
{
	_body->setMass(value);
}

void
PhysicsBodyComponent::setLinearVelocity(const Vector3& value) noexcept
{
	_body->setLinearVelocity(value);
}

void
PhysicsBodyComponent::setAngularVelocity(const Vector3& value) noexcept
{
	_body->setAngularVelocity(value);
}

void
PhysicsBodyComponent::setLinearDamping(float value)  noexcept
{
	_body->setLinearDamping(value);
}

void
PhysicsBodyComponent::setAngularDamping(float value) noexcept
{
	_body->setAngularDamping(value);
}

void
PhysicsBodyComponent::setGravity(const Vector3& value) noexcept
{
	_body->setGravity(value);
}

void
PhysicsBodyComponent::setConstantForce(const Vector3& value) noexcept
{
	if (_constantForce.length2() > 0.01f)
		_constantForce = value;
}

void
PhysicsBodyComponent::setConstantTorque(const Vector3& value) noexcept
{
	if (_constantTorque.length2() > 0.01f)
		_constantTorque = value;
}

void
PhysicsBodyComponent::setConstantVelocity(const Vector3& value) noexcept
{
	if (_constantVelocity.length2() > 0.01f)
		_constantVelocity = value;
}

void
PhysicsBodyComponent::setConstanAngularVelocity(const Vector3& value) noexcept
{
	if (_constantAngularVelocity.length2() > 0.01f)
		_constantAngularVelocity = value;
}

void
PhysicsBodyComponent::sleep(bool sleep) noexcept
{
	_body->sleep(sleep);
}

bool
PhysicsBodyComponent::isSleep() const noexcept
{
	return _body->isSleep();
}

float
PhysicsBodyComponent::getMass() const noexcept
{
	return _body->getMass();
}

float
PhysicsBodyComponent::getLinearDamping() const noexcept
{
	return _body->getLinearDamping();
}

float
PhysicsBodyComponent::getAngularDamping() const noexcept
{
	return _body->getAngularDamping();
}

const Vector3&
PhysicsBodyComponent::getGravity() const noexcept
{
	return _body->getGravity();
}

const Vector3&
PhysicsBodyComponent::getLinearVelocity() const noexcept
{
	return _body->getLinearVelocity();
}

const Vector3&
PhysicsBodyComponent::getAngularVelocity() const noexcept
{
	return _body->getAngularVelocity();
}

const Vector3&
PhysicsBodyComponent::getConstantForce() const noexcept
{
	return _constantForce;
}

const Vector3&
PhysicsBodyComponent::getConstantTorque() const noexcept
{
	return _constantTorque;
}

const Vector3&
PhysicsBodyComponent::getConstantVelocity() const noexcept
{
	return _constantVelocity;
}

const Vector3&
PhysicsBodyComponent::getConstantAngularVelocity() const noexcept
{
	return _constantAngularVelocity;
}

void
PhysicsBodyComponent::addForce(const Vector3& force) noexcept
{
	_body->addForce(force);
}

void
PhysicsBodyComponent::addRelativeForce(const Vector3& force, const Vector3& axis) noexcept
{
	_body->addRelativeForce(force, axis);
}

void
PhysicsBodyComponent::addTorque(const Vector3& force) noexcept
{
	_body->addTorque(force);
}

void
PhysicsBodyComponent::addImpulse(const Vector3& force, const Vector3& axis) noexcept
{
	_body->addImpulse(force, axis);
}

void
PhysicsBodyComponent::onActivate() noexcept
{
	auto physics = this->getGameObject()->getGameServer()->getFeature<PhysicFeatures>();
	assert(physics);

	auto physicsScene = physics->getPhysicsScene(this->getGameObject()->getGameScene());
	assert(physicsScene);

	auto collisionShape = this->getGameObject()->getComponent<PhysicsShapeComponent>();
	if (collisionShape)
	{
		auto shape = collisionShape->getCollisionShape();
		if (shape)
		{
			_body->setup(shape);
			_body->setMovePosition(this->getGameObject()->getTranslate());
			_body->setMoveRotation(this->getGameObject()->getRotate());
			_body->setPhysicsScene(physicsScene.get());
		}
	}
}

void
PhysicsBodyComponent::onDeactivate() noexcept
{
	if (_body)
		_body->close();
}

void
PhysicsBodyComponent::onFrame() noexcept
{
	if (_enableKinematic)
	{
		_body->addForce(_constantForce);
		_body->addTorque(_constantTorque);
		_body->setLinearVelocity(_body->getLinearVelocity() + _constantVelocity);
		_body->setAngularVelocity(_body->getAngularVelocity() + _constantAngularVelocity);
	}
}

void
PhysicsBodyComponent::onFrameEnd() noexcept
{
	auto& position = _body->getMovePosition();
	if (position != this->getGameObject()->getTranslate())
	{
		auto lookat = this->getGameObject()->getLookAt();
		auto translate = this->getGameObject()->getTranslate();
		lookat = lookat - translate;

		this->getGameObject()->setTranslate(position);
		this->getGameObject()->setLookAt(position + lookat);
	}

	this->getGameObject()->setRotate(_body->getMoveRotation());
}

void
PhysicsBodyComponent::onMoveAfter() noexcept
{
}

void
PhysicsBodyComponent::onCollisionStay() noexcept
{
	//this->sendMessage("onCollisionStay", nullptr);
}

GameComponentPtr
PhysicsBodyComponent::clone() const noexcept
{
	return std::make_shared<PhysicsBodyComponent>();
}

_NAME_END
#endif