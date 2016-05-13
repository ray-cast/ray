// +----------------------------------------------------------------------
// | Project : ray.
// | All rights reserved.
// +----------------------------------------------------------------------
// | Copyright (c) 2013-2016.
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
#ifndef _H_RIGIDBODY_COMPONENT_H_
#define _H_RIGIDBODY_COMPONENT_H_

#include <ray/physics_forward.h>

_NAME_BEGIN

class EXPORT PhysicsBodyComponent final : public GameComponent, public PhysicsBodyListener
{
	__DeclareSubClass(PhysicsBodyComponent, GameComponent)
public:
	PhysicsBodyComponent() noexcept;
	~PhysicsBodyComponent() noexcept;

	void setMass(float value) noexcept;
	void setRestitution(float value) noexcept;
	void setFriction(float friction) noexcept;
	void setLinearVelocity(const Vector3& value) noexcept;
	void setAngularVelocity(const Vector3& value) noexcept;
	void setLinearDamping(float value)  noexcept;
	void setAngularDamping(float value) noexcept;
	void setGravity(const Vector3& value) noexcept;
	void setConstantForce(const Vector3& value) noexcept;
	void setConstantTorque(const Vector3& value) noexcept;
	void setConstantVelocity(const Vector3& value) noexcept;
	void setConstanAngularVelocity(const Vector3& value) noexcept;

	void setCollisionMask(std::uint16_t mask) noexcept;
	std::uint16_t getCollisionMask() const noexcept;

	void sleep(bool sleep) noexcept;
	bool isSleep() const noexcept;

	void isKinematic(bool isKinematic) noexcept;
	bool isKinematic() const noexcept;

	float getMass() const noexcept;
	float getRestitution() const noexcept;
	float getFriction() const noexcept;
	float getLinearDamping() const noexcept;
	float getAngularDamping() const noexcept;

	const Vector3& getGravity() const noexcept;
	const Vector3& getLinearVelocity() const noexcept;
	const Vector3& getAngularVelocity() const noexcept;
	const Vector3& getConstantForce() const noexcept;
	const Vector3& getConstantTorque() const noexcept;
	const Vector3& getConstantVelocity() const noexcept;
	const Vector3& getConstantAngularVelocity() const noexcept;

	void addForce(const Vector3& force) noexcept;
	void addRelativeForce(const Vector3& force, const Vector3& axis) noexcept;

	void addTorque(const Vector3& force) noexcept;
	void addImpulse(const Vector3& force, const Vector3& axis) noexcept;

	virtual GameComponentPtr clone() const noexcept;

private:
	void _buildRigibody() noexcept;

private:
	friend class PhysicsJointComponent;
	PhysicsBody* getPhysicsBody() const noexcept;

private:
	virtual void onActivate() noexcept;
	virtual void onDeactivate() noexcept;

	virtual void onAttachComponent(GameComponentPtr& component) noexcept;
	virtual void onDetachComponent(GameComponentPtr& component) noexcept;

	virtual void onCollisionChange() noexcept;
	virtual void onCollisionEnter() noexcept;
	virtual void onCollisionExit() noexcept;
	virtual void onCollisionStay() noexcept;

	virtual void onMoveAfter() noexcept;

	virtual void onFetchResult() noexcept;

private:
	PhysicsBodyComponent(const PhysicsBodyComponent&) noexcept = delete;
	PhysicsBodyComponent& operator=(const PhysicsBodyComponent&) noexcept = delete;

private:
	bool _isEnableForce;

	Vector3 _constantForce;
	Vector3 _constantTorque;
	Vector3 _constantVelocity;
	Vector3 _constantAngularVelocity;

	delegate<void()> _onActivate;
	delegate<void()> _onDeactivate;

	std::function<void()> _onCollisionChange;

	std::unique_ptr<PhysicsBody> _body;
};

_NAME_END

#endif