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
#include <ray/physics_joint_spring.h>

_NAME_BEGIN

PhysicsJointSpring::PhysicsJointSpring() noexcept
	: _spring(0)
	, _damper(0)
	, _minDistance(0)
	, _maxDistance(10)
{
}

PhysicsJointSpring::~PhysicsJointSpring() noexcept
{
}

void 
PhysicsJointSpring::setup() noexcept
{
}

void 
PhysicsJointSpring::close() noexcept
{
}

void
PhysicsJointSpring::setSpring(float spring) noexcept
{
	_spring = spring;
}

void
PhysicsJointSpring::setDamper(float damper) noexcept
{
	_damper = damper;
}

void
PhysicsJointSpring::setMinDistance(float distance) noexcept
{
	_minDistance = distance;
}

void
PhysicsJointSpring::setMaxDistance(float distance) noexcept
{
	_maxDistance = distance;
}

float
PhysicsJointSpring::getSpring() const noexcept
{
	return _spring;
}

float
PhysicsJointSpring::getDamper() const noexcept
{
	return _damper;
}

float
PhysicsJointSpring::getMinDistance() const noexcept
{
	return _minDistance;
}

float
PhysicsJointSpring::getMaxDistance() const noexcept
{
	return _maxDistance;
}

_NAME_END