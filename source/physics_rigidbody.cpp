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
#include <ray/physics_rigidbody.h>
#include <ray/physics_scene.h>

#include <btBulletDynamicsCommon.h>

_NAME_BEGIN

PhysicsRigidbody::PhysicsRigidbody() noexcept
    : _enablejointFather(false)
    , _enableGravity(true)
    , _mass(0.0f)
    , _linearDamping(0.05f)
    , _angularDamping(0.05f)
    , _position(Vector3::Zero)
    , _rotate(Vector3::Zero)
    , _inertia(Vector3::Zero)
    , _gravity(Vector3::Zero)
    , _linearVelocity(Vector3::Zero)
    , _angularVelocity(Vector3::Zero)
    , _sleep(false)
    , _rigidbody(nullptr)
    , _motionState(nullptr)
    , _scene(nullptr)
    , _listener(nullptr)
{
}

PhysicsRigidbody::~PhysicsRigidbody() noexcept
{
    this->close();
}

void
PhysicsRigidbody::setup(PhysicsShapePtr shape) noexcept
{
    assert(shape);

    btTransform startTransform;
    startTransform.setIdentity();
    startTransform.setOrigin(btVector3(_position.x, _position.y, _position.z));
    startTransform.setRotation(btQuaternion(_rotate.x, _rotate.y, _rotate.z, _rotate.w));

    _motionState = new btDefaultMotionState(startTransform);
    _rigidbody = new btRigidBody(_mass, _motionState, shape->getCollisionShape());
    _rigidbody->setUserPointer(this);

    this->setGravity(_gravity);
    this->setLinearVelocity(_linearVelocity);
    this->setAngularVelocity(_angularVelocity);
    this->setLinearDamping(_linearDamping);
    this->setAngularDamping(_angularDamping);
    this->sleep(_sleep);
}

void
PhysicsRigidbody::close() noexcept
{
    if (_motionState)
    {
        delete _motionState;
        _motionState = nullptr;
    }

    if (_rigidbody)
    {
        this->setPhysicsScene(nullptr);

        delete _rigidbody;
        _rigidbody = nullptr;
    }
}

void
PhysicsRigidbody::setMass(float value) noexcept
{
    if (_rigidbody)
    {
        btVector3 inertia;
        inertia.setX(_inertia.x);
        inertia.setY(_inertia.y);
        inertia.setZ(_inertia.z);

        _rigidbody->setMassProps(value, inertia);
    }

    _mass = value;
}

void
PhysicsRigidbody::setLinearVelocity(const Vector3& value) noexcept
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
PhysicsRigidbody::setAngularVelocity(const Vector3& value) noexcept
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
PhysicsRigidbody::setLinearDamping(float damping)  noexcept
{
    if (damping < 0.f)
        damping = 0.f;

    if (_rigidbody)
        _rigidbody->setDamping(_linearDamping, _angularDamping);

    _linearDamping = damping;
}

void
PhysicsRigidbody::setAngularDamping(float damping) noexcept
{
    if (damping < 0.f)
        damping = 0.f;

    if (_rigidbody)
        _rigidbody->setDamping(_linearDamping, _angularDamping);

    _angularDamping = damping;
}

void
PhysicsRigidbody::setGravity(const Vector3& value) noexcept
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
PhysicsRigidbody::setMovePosition(const Vector3& value) noexcept
{
    if (_rigidbody)
    {
        auto motion = _rigidbody->getMotionState();
        if (motion)
        {
            btTransform transform;

            motion->getWorldTransform(transform);

            btVector3 pos;
            pos.setX(value.x);
            pos.setY(value.y);
            pos.setZ(value.z);

            transform.setOrigin(pos);

            motion->setWorldTransform(transform);

            _rigidbody->setMotionState(_motionState);
        }
    }

    _position = value;
}

void
PhysicsRigidbody::setMoveRotation(const Quaternion& value) noexcept
{
    if (_rigidbody)
    {
        auto motion = _rigidbody->getMotionState();
        if (motion)
        {
            btTransform transform;

            motion->getWorldTransform(transform);

            btQuaternion quat;
            quat.setX(value.x);
            quat.setY(value.y);
            quat.setZ(value.z);
            quat.setW(value.w);

            transform.setRotation(quat);

            motion->setWorldTransform(transform);

            _rigidbody->setMotionState(_motionState);
        }
    }

    _rotate = value;
}

void
PhysicsRigidbody::sleep(bool sleep) noexcept
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
PhysicsRigidbody::getMass() const noexcept
{
    assert(_rigidbody);

    return 1 / _rigidbody->getInvMass();
}

float
PhysicsRigidbody::getLinearDamping() const noexcept
{
    return _linearDamping;
}

float
PhysicsRigidbody::getAngularDamping() const noexcept
{
    return _angularDamping;
}

const Vector3&
PhysicsRigidbody::getGravity() const noexcept
{
    return _gravity;
}

const Vector3&
PhysicsRigidbody::getLinearVelocity() const noexcept
{
    return _linearVelocity;
}

const Vector3&
PhysicsRigidbody::getAngularVelocity() const noexcept
{
    return _angularVelocity;
}

const Vector3&
PhysicsRigidbody::getMovePosition() const noexcept
{
    assert(_rigidbody);

    auto motion = _rigidbody->getMotionState();
    if (motion)
    {
        btTransform transform;
        motion->getWorldTransform(transform);

        auto pos = transform.getOrigin();

        _position.x = pos.x();
        _position.y = pos.y();
        _position.z = pos.z();
    }

    return _position;
}

const Quaternion&
PhysicsRigidbody::getMoveRotation() const noexcept
{
    assert(_rigidbody);

    auto motion = _rigidbody->getMotionState();
    if (motion)
    {
        btTransform transform;
        motion->getWorldTransform(transform);

        auto rot = transform.getRotation();

        _rotate.x = rot.x();
        _rotate.y = rot.y();
        _rotate.z = rot.z();
        _rotate.w = rot.w();
    }

    return _rotate;
}

void
PhysicsRigidbody::addForce(const Vector3& value) noexcept
{
    assert(_rigidbody);

    btVector3 force;
    force.setX(value.x);
    force.setY(value.y);
    force.setZ(value.z);

    _rigidbody->applyCentralForce(force);
}

void
PhysicsRigidbody::addRelativeForce(const Vector3& value, const Vector3& axis) noexcept
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
PhysicsRigidbody::addTorque(const Vector3& value) noexcept
{
    assert(_rigidbody);

    btVector3 torque;
    torque.setX(value.x);
    torque.setY(value.y);
    torque.setZ(value.z);

    _rigidbody->applyTorque(torque);
}

void
PhysicsRigidbody::addImpulse(const Vector3& value, const Vector3& axis) noexcept
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
PhysicsRigidbody::isSleep() const noexcept
{
    if (_rigidbody)
    {
        _sleep = _rigidbody->wantsSleeping();
    }

    return _sleep;
}

void
PhysicsRigidbody::setPhysicsScene(PhysicsScene* scene) noexcept
{
    if (_scene)
    {
        _scene->removeRigidbody(_rigidbody);
    }

    _scene = scene;

    if (_scene)
    {
        _scene->addRigidbody(_rigidbody);
    }
}

void
PhysicsRigidbody::setRigidbodyListener(PhysicsRigidbodyListener* listener) noexcept
{
    _listener = listener;
}

PhysicsRigidbodyListener*
PhysicsRigidbody::getRigidbodyListener() noexcept
{
    return _listener;
}

_NAME_END