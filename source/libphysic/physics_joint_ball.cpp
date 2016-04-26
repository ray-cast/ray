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
#include <ray/physics_joint_ball.h>

_NAME_BEGIN

JointBall::JointBall() noexcept
	: Joint(Joint::Ball)
	, _lowLimit(Vector3::Zero)
	, _highLimit(Vector3::Zero)
	, _angularLowLimit(Vector3::Zero)
	, _angularHighLimit(Vector3::Zero)
{
}

JointBall::~JointBall() noexcept
{
}

void
JointBall::setLinearLowerLimit(const Vector3& limit) noexcept
{
	_lowLimit = limit;
}

void
JointBall::setLinearHighLimit(const Vector3& limit) noexcept
{
	_highLimit = limit;
}

void
JointBall::setAngularLowerLimit(const Vector3& limit) noexcept
{
	_angularLowLimit = limit;
}

void
JointBall::setAngularHighLimit(const Vector3& limit) noexcept
{
	_angularHighLimit = limit;
}

const Vector3&
JointBall::getLinearLowerLimit() const noexcept
{
	return _lowLimit;
}

const Vector3&
JointBall::getLinearHighLimit() const noexcept
{
	return _highLimit;
}

const Vector3&
JointBall::getAngularLowerLimit() const noexcept
{
	return _angularLowLimit;
}

const Vector3&
JointBall::getAngularHighLimit() const noexcept
{
	return _angularHighLimit;
}

_NAME_END