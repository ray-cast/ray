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
#ifndef _H_PHYSICS_JOINT_CONFIGURABLE_H_
#define _H_PHYSICS_JOINT_CONFIGURABLE_H_

#include <ray/physics_joint.h>

_NAME_BEGIN

class EXPORT PhysicsJointConfigurable final : public PhysicsJoint
{
public:
	PhysicsJointConfigurable() noexcept;
	virtual ~PhysicsJointConfigurable() noexcept;

	void setLinearSpring(const float3& spring) noexcept;
	void setAngularSprint(const Quaternion& spring) noexcept;

	void setLinearLowerLimit(const float3& limit) noexcept;
	void setLinearHighLimit(const float3& limit) noexcept;

	void setAngularLowerLimit(const float3& limit) noexcept;
	void setAngularHighLimit(const float3& limit) noexcept;

	void setMovementConstant(const float3& constant) noexcept;
	void setRotationConstant(const float3& constant) noexcept;

	const float3& getLinearSpring() const noexcept;
	const Quaternion& getAngularSprint() const noexcept;

	const float3& getLinearLowerLimit() const noexcept;
	const float3& getLinearHighLimit() const noexcept;

	const float3& getAngularLowerLimit() const noexcept;
	const float3& getAngularHighLimit() const noexcept;

	const float3& getMovementConstant() const noexcept;
	const float3& getRotationConstant() const noexcept;

	void setPhysicsScene(PhysicsScenePtr scene) noexcept;

private:
	virtual void onActivate() noexcept;
	virtual void onDeactivate() noexcept;

private:
	PhysicsJointConfigurable(const PhysicsJointConfigurable&) = delete;
	PhysicsJointConfigurable& operator=(const PhysicsJointConfigurable&) = delete;

private:
	float3 _translate;
	Quaternion _quaternion;

	float3 _lowLimit;
	float3 _highLimit;

	float3 _angularLowLimit;
	float3 _angularHighLimit;

	float3 _movementConstant;
	float3 _rotationConstant;

	std::unique_ptr<btGeneric6DofSpringConstraint> _joint;

	PhysicsSceneWeakPtr _scene;
};

_NAME_END

#endif