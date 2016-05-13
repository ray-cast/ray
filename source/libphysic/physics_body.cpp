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
#include <ray/physics_body.h>
#include <ray/physics_system.h>
#include "bullet_types.h"

_NAME_BEGIN

class MotionState final : public btMotionState
{
public:
	btTransform _graphicsWorldTrans;
	PhysicsBody* _userPointer;

	MotionState(PhysicsBody* body)
		: _userPointer(body)
	{
		_graphicsWorldTrans.setIdentity();
	}

	void setFromOpenGLMatrix(const float4x4& centerOfMassWorldTrans)
	{
		_graphicsWorldTrans.setFromOpenGLMatrix(centerOfMassWorldTrans.data());
	}

	void getFromOpenGLMatrix(float4x4& centerOfMassWorldTrans)
	{
		_graphicsWorldTrans.getOpenGLMatrix(centerOfMassWorldTrans.data());
	}

private:
	virtual void getWorldTransform(btTransform& centerOfMassWorldTrans) const
	{
		centerOfMassWorldTrans = _graphicsWorldTrans;
	}

	virtual void setWorldTransform(const btTransform& centerOfMassWorldTrans)
	{
		_graphicsWorldTrans = centerOfMassWorldTrans;

		auto listener = _userPointer->getRigidbodyListener();
		if (listener)
			listener->onFetchResult();
	}

private:
	MotionState(const MotionState&) = delete;
	MotionState& operator=(const MotionState&) = delete;
};

PhysicsBody::PhysicsBody() noexcept
	: _isKinematic(false)
	, _mass(0.0f)
	, _layer(0)
	, _layerMask(0xFFFF)
	, _friction(0.5f)
	, _restitution(0.0f)
	, _linearDamping(0.05f)
	, _angularDamping(0.05f)
	, _inertia(Vector3::Zero)
	, _gravity(Vector3::Zero)
	, _linearVelocity(Vector3::Zero)
	, _angularVelocity(Vector3::Zero)
	, _sleep(false)
	, _rigidbody(nullptr)
	, _listener(nullptr)
{
	_motion = new MotionState(this);
}

PhysicsBody::~PhysicsBody() noexcept
{
	delete _motion;
	this->close();
}

void
PhysicsBody::setup(PhysicsShapePtr shape) noexcept
{
	assert(shape);
	assert(shape->getCollisionShape());

	btCollisionShape* collision = shape->getCollisionShape();
	btVector3 btv3LocalInertia(0.0f, 0.0f, 0.0f);

	if (_isKinematic)
		_mass = 0.0f;

	if (_mass != 0.0f)
	{
		if (collision->getShapeType() != TRIANGLE_MESH_SHAPE_PROXYTYPE)
			collision->calculateLocalInertia(_mass, btv3LocalInertia);
	}

	_rigidbody = std::make_unique<btRigidBody>(_mass, _motion, shape->getCollisionShape(), btv3LocalInertia);
	_rigidbody->setUserPointer(this);
	_rigidbody->setRestitution(_restitution);
	_rigidbody->setFriction(_friction);
	_rigidbody->setSleepingThresholds(0.0f, 0.0f);

	this->isKinematic(_isKinematic);
	this->setGravity(_gravity);
	this->setLinearVelocity(_linearVelocity);
	this->setAngularVelocity(_angularVelocity);
	this->setLinearDamping(_linearDamping);
	this->setAngularDamping(_angularDamping);
	this->sleep(_sleep);
	this->setPhysicsScene(PhysicsSystem::instance()->getPhysicsScene());
}

void
PhysicsBody::close() noexcept
{
	this->setPhysicsScene(nullptr);
	_rigidbody.reset();
}

void
PhysicsBody::setMass(float value) noexcept
{
	if (_rigidbody)
	{
		btVector3 btv3LocalInertia;
		btv3LocalInertia.setX(_inertia.x);
		btv3LocalInertia.setY(_inertia.y);
		btv3LocalInertia.setZ(_inertia.z);

		btCollisionShape* collision = _rigidbody->getCollisionShape();
		if (_mass != 0.0f)
			collision->calculateLocalInertia(_mass, btv3LocalInertia);

		_rigidbody->setMassProps(value, btv3LocalInertia);
	}

	_mass = value;
}

void
PhysicsBody::setRestitution(float value) noexcept
{
	if (_rigidbody)
		_rigidbody->setRestitution(value);
	_restitution = value;
}

void
PhysicsBody::setLinearVelocity(const Vector3& value) noexcept
{
	if (_rigidbody)
	{
		btVector3 velocity;
		velocity.setX(value.x);
		velocity.setY(value.y);
		velocity.setZ(value.z);

		_rigidbody->setLinearVelocity(velocity);
	}

	_linearVelocity = value;
}

void
PhysicsBody::setAngularVelocity(const Vector3& value) noexcept
{
	if (_rigidbody)
	{
		btVector3 velocity;
		velocity.setX(value.x);
		velocity.setY(value.y);
		velocity.setZ(value.z);

		_rigidbody->setAngularVelocity(velocity);
	}

	_angularVelocity = value;
}

void
PhysicsBody::setLinearDamping(float damping) noexcept
{
	if (damping < 0.f)
		damping = 0.f;

	if (_rigidbody)
		_rigidbody->setDamping(_linearDamping, _angularDamping);

	_linearDamping = damping;
}

void
PhysicsBody::setAngularDamping(float damping) noexcept
{
	if (damping < 0.f)
		damping = 0.f;

	if (_rigidbody)
		_rigidbody->setDamping(_linearDamping, _angularDamping);

	_angularDamping = damping;
}

void
PhysicsBody::setFriction(float value) noexcept
{
	if (_rigidbody)
		_rigidbody->setFriction(value);
	_friction = value;
}

void
PhysicsBody::setGravity(const Vector3& value) noexcept
{
	if (_rigidbody)
	{
		btVector3 gravity;
		gravity.setX(value.x);
		gravity.setY(value.y);
		gravity.setZ(value.z);

		_rigidbody->setGravity(gravity);
	}

	_gravity = value;
}

void
PhysicsBody::setWorldTransform(const float4x4& value) noexcept
{
	_motion->setFromOpenGLMatrix(value);
}

void
PhysicsBody::isKinematic(bool isKinematic) noexcept
{
	if (_rigidbody)
	{
		if (isKinematic)
			_rigidbody->setCollisionFlags(_rigidbody->getCollisionFlags() | btCollisionObject::CF_KINEMATIC_OBJECT);
		else
			_rigidbody->setCollisionFlags(_rigidbody->getCollisionFlags() & ~btCollisionObject::CF_KINEMATIC_OBJECT);
	}

	_isKinematic = isKinematic;
}

bool
PhysicsBody::isKinematic() const noexcept
{
	return _isKinematic;
}

void
PhysicsBody::sleep(bool sleep) noexcept
{
	if (_rigidbody)
	{
		if (sleep)
			_rigidbody->setActivationState(ISLAND_SLEEPING);
		else
			_rigidbody->setActivationState(DISABLE_DEACTIVATION);
	}

	_sleep = sleep;
}

float
PhysicsBody::getMass() const noexcept
{
	assert(_rigidbody);
	return _mass;
}

float
PhysicsBody::getRestitution() const noexcept
{
	assert(_rigidbody);
	return _rigidbody->getRestitution();
}

float
PhysicsBody::getFriction() const noexcept
{
	assert(_rigidbody);
	return _rigidbody->getFriction();
}

float
PhysicsBody::getLinearDamping() const noexcept
{
	return _linearDamping;
}

float
PhysicsBody::getAngularDamping() const noexcept
{
	return _angularDamping;
}

const Vector3&
PhysicsBody::getGravity() const noexcept
{
	return _gravity;
}

const Vector3&
PhysicsBody::getLinearVelocity() const noexcept
{
	return _linearVelocity;
}

const Vector3&
PhysicsBody::getAngularVelocity() const noexcept
{
	return _angularVelocity;
}

void
PhysicsBody::getWorldTransform(float4x4& m) const noexcept
{
	_motion->getFromOpenGLMatrix(m);
}

void
PhysicsBody::addForce(const Vector3& value) noexcept
{
	assert(_rigidbody);

	btVector3 force;
	force.setX(value.x);
	force.setY(value.y);
	force.setZ(value.z);

	_rigidbody->applyCentralForce(force);
}

void
PhysicsBody::addRelativeForce(const Vector3& value, const Vector3& axis) noexcept
{
	assert(_rigidbody);

	btVector3 force;
	force.setX(value.x);
	force.setY(value.y);
	force.setZ(value.z);

	btVector3 pos;
	pos.setX(axis.x);
	pos.setY(axis.y);
	pos.setZ(axis.z);

	_rigidbody->applyForce(force, pos);
}

void
PhysicsBody::addTorque(const Vector3& value) noexcept
{
	assert(_rigidbody);

	btVector3 torque;
	torque.setX(value.x);
	torque.setY(value.y);
	torque.setZ(value.z);

	_rigidbody->applyTorque(torque);
}

void
PhysicsBody::addImpulse(const Vector3& value, const Vector3& axis) noexcept
{
	assert(_rigidbody);

	btVector3 force;
	force.setX(value.x);
	force.setY(value.y);
	force.setZ(value.z);

	btVector3 pos;
	pos.setX(axis.x);
	pos.setY(axis.y);
	pos.setZ(axis.z);

	_rigidbody->applyImpulse(force, pos);
}

bool
PhysicsBody::isSleep() const noexcept
{
	if (_rigidbody)
		return _rigidbody->wantsSleeping();
	return _sleep;
}

void
PhysicsBody::setLayer(std::uint8_t layer) noexcept
{
	_layer = layer;
}

void
PhysicsBody::setLayerMask(std::uint16_t mask) noexcept
{
	_layerMask = mask;
}

std::uint8_t
PhysicsBody::getLayer() const noexcept
{
	return _layer;
}

std::uint16_t
PhysicsBody::getLayerMask() const noexcept
{
	return _layerMask;
}

void
PhysicsBody::setPhysicsScene(PhysicsScenePtr scene) noexcept
{
	if (_scene.lock())
		_scene.lock()->removeRigidbody(this);

	_scene = scene;

	if (scene)
		scene->addRigidbody(this);
}

void
PhysicsBody::setRigidbodyListener(PhysicsBodyListener* listener) noexcept
{
	_listener = listener;
}

PhysicsBodyListener*
PhysicsBody::getRigidbodyListener() noexcept
{
	return _listener;
}

btRigidBody*
PhysicsBody::getRigidbody() noexcept
{
	return _rigidbody.get();
}

_NAME_END