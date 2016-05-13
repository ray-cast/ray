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
#ifndef _H_ANIM_H_
#define _H_ANIM_H_

#include <ray/bone.h>

_NAME_BEGIN

struct Interpolation
{
	std::uint8_t interpX[4];
	std::uint8_t interpY[4];
	std::uint8_t interpZ[4];
	std::uint8_t interpW[4];
};

struct MotionSegment
{
	int m0;
	int m1;
};

class EXPORT BoneAnimation
{
public:
	BoneAnimation() noexcept;
	~BoneAnimation() noexcept;

	void setName(const std::string& name) noexcept;
	const std::string& getName() const noexcept;

	void setPosition(const Vector3& position) noexcept;
	const Vector3& getPosition() const noexcept;

	void setRotation(const Quaternion& position) noexcept;
	const Quaternion& getRotation() const noexcept;

	void setBoneIndex(const std::int32_t) noexcept;
	std::int32_t getBoneIndex() const noexcept;

	void setFrameNo(std::int32_t frame) noexcept;
	std::int32_t getFrameNo() const noexcept;

	void setInterpolation(const Interpolation& interp) noexcept;
	const Interpolation& getInterpolation() const noexcept;

private:
	std::string _name;

	std::int32_t _bone;
	std::int32_t _frame;

	Vector3 _position;
	Quaternion _rotation;
	Interpolation _interpolation;
};

class EXPORT MorphAnimation
{
public:
};

class EXPORT AnimationProperty final
{
public:
	AnimationProperty() noexcept;
	~AnimationProperty() noexcept;

	void setName(const std::string& name) noexcept;
	const std::string& getName() const noexcept;

	void setCurrentFrame(std::size_t frame) noexcept;
	std::size_t getCurrentFrame() const noexcept;

	void setBoneArray(const Bones& bones) noexcept;
	void setBoneArray(Bones&& bones) noexcept;
	const Bones& getBoneArray() const noexcept;

	void setIKArray(const InverseKinematics& ik) noexcept;
	void setIKArray(InverseKinematics&& ik) noexcept;
	const InverseKinematics& getIKArray() const noexcept;

	void addBoneAnimation(const BoneAnimation& anim) noexcept;
	BoneAnimation& getBoneAnimation(std::size_t index) noexcept;
	const BoneAnimation& getBoneAnimation(std::size_t index) const noexcept;
	std::size_t getNumBoneAnimation() const noexcept;

	void addMorphAnimation(const MorphAnimation& anim) noexcept;
	MorphAnimation& getMorphAnimation(std::size_t index) noexcept;
	const MorphAnimation& getMorphAnimation(std::size_t index) const noexcept;
	std::size_t getNumMorphAnimation() const noexcept;

	AnimationPropertyPtr clone() noexcept;

	void updateFrame(float delta) noexcept;
	void updateMotion() noexcept;
	void updateBoneMotion() noexcept;
	bool updateBoneMotion(std::size_t index) noexcept;
	void updateBoneMatrix() noexcept;
	void updateBoneMatrix(Bone& bone) noexcept;
	void updateIK() noexcept;

	MotionSegment findMotionSegment(int frame, const std::vector<std::size_t>& motions) noexcept;
	void interpolateMotion(Quaternion& rotation, Vector3& position, const std::vector<std::size_t>& motions, float frame) noexcept;

private:
	AnimationProperty(const AnimationProperty&) = delete;
	AnimationProperty& operator=(const AnimationProperty&) = delete;

private:
	void updateIK(Bones& _bones, const IKAttr& ik) noexcept;
	void updateBones(const Bones& _bones) noexcept;
	void updateTransform(Bone& bone, const float3& translate, const Quaternion& rotate) noexcept;

private:

	std::string _name;

	std::size_t _fps;
	std::size_t _frame;

	float _delta;

	Bones _bones;
	InverseKinematics _iks;

	std::vector<BoneAnimation> _boneAnimation;
	std::vector<MorphAnimation> _morphAnimation;
	std::vector<std::vector<std::size_t>> _bindAnimation;
	std::vector<std::vector<std::size_t>> _bindAnimations;
};

_NAME_END

#endif
