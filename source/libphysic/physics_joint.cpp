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
#include <ray/physics_joint.h>

_NAME_BEGIN

PhysicsJointListener::PhysicsJointListener() noexcept
{
}

PhysicsJointListener::~PhysicsJointListener() noexcept
{
}

PhysicsJoint::PhysicsJoint() noexcept
	: _rigidbodyA(nullptr)
	, _rigidbodyB(nullptr)
	, _active(false)
	, _axis(Vector3::Zero)
	, _anchor(Vector3::Zero)
	, _breakForce(std::numeric_limits<float>::max())
	, _breakTorque(std::numeric_limits<float>::max())
	, _listener(nullptr)
{
}

PhysicsJoint::~PhysicsJoint() noexcept
{
}

void
PhysicsJoint::setActive(bool active) except
{
	if (_active != active)
	{
		if (active)
			this->onActivate();
		else
			this->onDeactivate();

		_active = active;
	}
}

bool
PhysicsJoint::getActive() const noexcept
{
	return _active;
}

void
PhysicsJoint::setRigidbody(PhysicsBody* body) noexcept
{
	_rigidbodyA = body;
}

PhysicsBody*
PhysicsJoint::getRigidbody() const noexcept
{
	return _rigidbodyA;
}

void
PhysicsJoint::setConnectRigidbody(PhysicsBody* body) noexcept
{
	_rigidbodyB = body;
}

PhysicsBody*
PhysicsJoint::getConnectRigidbody() const noexcept
{
	return _rigidbodyB;
}

void
PhysicsJoint::setBreakForce(float force) noexcept
{
	_breakForce = force;
}

void
PhysicsJoint::setBreakTorque(float torque) noexcept
{
	_breakTorque = torque;
}

void
PhysicsJoint::setAxis(const Vector3& axis) noexcept
{
	_axis = axis;
}

void
PhysicsJoint::setAnchor(const Vector3& axis) noexcept
{
	_anchor = axis;
}

float
PhysicsJoint::getBreakForce() const noexcept
{
	return _breakForce;
}

float
PhysicsJoint::getBreakTorque() const noexcept
{
	return _breakTorque;
}

const Vector3&
PhysicsJoint::getAxis() const noexcept
{
	return _axis;
}

const Vector3&
PhysicsJoint::getAnchor() const noexcept
{
	return _anchor;
}

void 
PhysicsJoint::setOwnerListener(PhysicsJointListener* listener) noexcept
{
	_listener = listener;
}

PhysicsJointListener* 
PhysicsJoint::getOwnerListener() const noexcept
{
	return _listener;
}

btRigidBody* 
PhysicsJoint::getRawRigidbody() const noexcept
{
	if (_rigidbodyA)
		return _rigidbodyA->getRigidbody();
	return nullptr;
}

btRigidBody* 
PhysicsJoint::getRawConnectRigidbody() const noexcept
{
	if (_rigidbodyB)
		return _rigidbodyB->getRigidbody();
	return nullptr;
}

void
PhysicsJoint::onActivate() except
{
}

void
PhysicsJoint::onDeactivate() except
{
}

_NAME_END