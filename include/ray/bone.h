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
#ifndef _H_BONE_H_
#define _H_BONE_H_

#include <ray/modtypes.h>

_NAME_BEGIN

class EXPORT VertexWeight
{
public:
	float weight1;
	float weight2;
	float weight3;
	float weight4;

	std::int16_t bone1;
	std::int16_t bone2;
	std::int16_t bone3;
	std::int16_t bone4;
};

class EXPORT IKChild
{
public:
	std::uint16_t boneIndex;
	std::uint8_t rotateLimited;

	float angleWeight;
	
	float3 minimumDegrees;
	float3 maximumDegrees;
};

class EXPORT IKAttr
{
public:
	std::uint16_t boneIndex;
	std::uint16_t targetBoneIndex;
	std::uint32_t iterations;
	std::uint32_t chainLength;

	std::vector<IKChild> child;
};

class EXPORT Bone final
{
public:
	Bone() noexcept;
	Bone(const std::string& name) noexcept;
	~Bone() noexcept;

	void setName(const std::string& name) noexcept;
	const std::string& getName() const noexcept;

	void setParent(std::int16_t parent) noexcept;
	std::int16_t getParent() const noexcept;

	void setPosition(const float3& position) noexcept;
	const float3& getPosition() const noexcept;

	void setRotation(const Quaternion& rotate) noexcept;
	const Quaternion& getRotation() const noexcept;

	void setScaling(const float3& scale) noexcept;
	const float3& getScaling() const noexcept;

	void setLocalTransform(const Matrix4x4& transform) noexcept;
	const Matrix4x4& getLocalTransform() const noexcept;

	void setTransform(const Matrix4x4& transform) noexcept;
	const Matrix4x4& getTransform() const noexcept;

private:
	std::string _name;

	bool _isLeg;

	std::int16_t _parent;

	float3 _translate;
	float3 _scaling;
	Quaternion _rotation;

	float4x4 _transform;
	float4x4 _localTransform;
};

_NAME_END

#endif
