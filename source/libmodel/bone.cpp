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
#include <ray/bone.h>

_NAME_BEGIN

Bone::Bone() noexcept
	: _parent(-1)
	, _rotation(0.0f,0.0f,0.0f,1.0f)
	, _scaling(float3::One)
	, _isLeg(false)
{
	_localTransform.loadIdentity();
	_transform.loadIdentity();
}

Bone::Bone(const std::string& name) noexcept
{
	_name = name;
}

Bone::~Bone() noexcept
{
}

void
Bone::setName(const std::string& name) noexcept
{
	_name = name;
}

const std::string&
Bone::getName() const noexcept
{
	return _name;
}

void
Bone::setParent(std::int16_t parent) noexcept
{
	_parent = parent;
}

std::int16_t
Bone::getParent() const noexcept
{
	return _parent;
}

void
Bone::setPosition(const float3& position) noexcept
{
	_translate = position;
}

const float3&
Bone::getPosition() const noexcept
{
	return _translate;
}

void
Bone::setScaling(const float3& scale) noexcept
{
	_scaling = scale;
}

const float3&
Bone::getScaling() const noexcept
{
	return _scaling;
}

void
Bone::setRotation(const Quaternion& rotate) noexcept
{
	_rotation = rotate;
}

const Quaternion&
Bone::getRotation() const noexcept
{
	return _rotation;
}

void
Bone::setLocalTransform(const Matrix4x4& transform) noexcept
{
	_localTransform = transform;
}

const Matrix4x4&
Bone::getLocalTransform() const noexcept
{
	return _localTransform;
}

void
Bone::setTransform(const Matrix4x4& transform) noexcept
{
	_transform = transform;
}

const Matrix4x4&
Bone::getTransform() const noexcept
{
	return _transform;
}

_NAME_END