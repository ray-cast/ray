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
#ifndef _H_PHYSICS_BODY_H_
#define _H_PHYSICS_BODY_H_

#include <ray/physics_shape.h>

class btMotionState;

_NAME_BEGIN

class EXPORT PhysicsBodyListener
{
public:
	virtual void onFetchResult() noexcept = 0;
	virtual void onCollisionStay() noexcept = 0;
};

class EXPORT PhysicsBody
{
public:
	PhysicsBody() noexcept;
	~PhysicsBody() noexcept;

	void setup(PhysicsShapePtr shape) noexcept;
	void close() noexcept;

	void setMass(float value) noexcept;
	void setRestitution(float value) noexcept;
	void setLinearVelocity(const Vector3& value) noexcept;
	void setAngularVelocity(const Vector3& value) noexcept;
	void setLinearDamping(float value)  noexcept;
	void setAngularDamping(float value) noexcept;
	void setFriction(float value) noexcept;
	void setGravity(const Vector3& value) noexcept;

	void isKinematic(bool isKinematic) noexcept;
	bool isKinematic() const noexcept;

	void sleep(bool sleep) noexcept;
	bool isSleep() const noexcept;

	void setLayer(std::uint8_t layer) noexcept;
	void setLayerMask(std::uint16_t mask) noexcept;
	std::uint8_t getLayer() const noexcept;
	std::uint16_t getLayerMask() const noexcept;

	float getMass() const noexcept;
	float getRestitution() const noexcept;
	float getFriction() const noexcept;
	float getLinearDamping() const noexcept;
	float getAngularDamping() const noexcept;

	const Vector3& getGravity() const noexcept;
	const Vector3& getLinearVelocity() const noexcept;
	const Vector3& getAngularVelocity() const noexcept;

	void setWorldTransform(const float4x4& transform) noexcept;
	void getWorldTransform(float4x4& transform) const noexcept;

	void addForce(const Vector3& force) noexcept;
	void addRelativeForce(const Vector3& force, const Vector3& axis) noexcept;

	void addTorque(const Vector3& force) noexcept;
	void addImpulse(const Vector3& force, const Vector3& axis) noexcept;

	void setPhysicsScene(PhysicsScenePtr scene) noexcept;

	void setRigidbodyListener(PhysicsBodyListener* listener) noexcept;
	PhysicsBodyListener* getRigidbodyListener() noexcept;

private:
	friend class PhysicsJoint;
	friend class PhysicsScene;
	btRigidBody* getRigidbody() noexcept;

private:
	PhysicsBody(const PhysicsBody&) = delete;
	PhysicsBody& operator=(const PhysicsBody&) = delete;

private:

	std::unique_ptr<btRigidBody> _rigidbody;

	std::uint8_t _layer;
	std::uint16_t _layerMask;

	bool _sleep;
	bool _isKinematic;

	float _mass;

	float _linearDamping;
	float _angularDamping;

	float _friction;
	float _restitution;

	Vector3 _inertia;
	Vector3 _gravity;

	Vector3 _linearVelocity;
	Vector3 _angularVelocity;

	class MotionState* _motion;
	PhysicsSceneWeakPtr _scene;
	PhysicsBodyListener* _listener;
};

_NAME_END

#endif