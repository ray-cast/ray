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
#ifndef _H_JOINT_H_
#define _H_JOINT_H_

#include <ray/physics_rigidbody.h>

_NAME_BEGIN

class EXPORT Joint
{
public:
	enum JointType
	{
		Ball,
		Fixed,
		Hinge,
		Spring,
		Universal,
	};
public:
	Joint(JointType type) noexcept;
	virtual ~Joint() noexcept;

	JointType getType() const noexcept;

	void setRigidbody(PhysicsRigidbody* body) noexcept;
	void setBreakForce(float force) noexcept;
	void setBreakTorque(float torque) noexcept;
	void setAxis(const Vector3& axis) noexcept;
	void setAnchor(const Vector3& axis) noexcept;

	PhysicsRigidbody* getRigidbody() const noexcept;
	float getBreakForce() const noexcept;
	float getBreakTorque() const noexcept;
	const Vector3& getAxis() const noexcept;
	const Vector3& getAnchor() const noexcept;

private:
	Joint(const Joint&) = delete;
	Joint& operator=(const Joint&) = delete;

private:

	JointType _type;

	PhysicsRigidbody* _rigidbody;

	float _breakForce;
	float _breakTorque;

	Vector3 _axis;
	Vector3 _anchor;
};

_NAME_END

#endif