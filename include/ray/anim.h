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

	void setBoneIndex(const std::size_t) noexcept;
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

	void setBoneArray(Bones bone) noexcept;
	const Bones& getBoneArray() const noexcept;

	void setIKArray(InverseKinematics ik) noexcept;
	InverseKinematics getIKArray() noexcept;

	void addBoneAnimation(const BoneAnimation& anim) noexcept;
	BoneAnimation& getBoneAnimation(std::size_t index) noexcept;
	const BoneAnimation& getBoneAnimation(std::size_t index) const noexcept;
	std::size_t getNumBoneAnimation() const noexcept;

	void addMorphAnimation(const MorphAnimation& anim) noexcept;
	MorphAnimation& getMorphAnimation(std::size_t index) noexcept;
	const MorphAnimation& getMorphAnimation(std::size_t index) const noexcept;
	std::size_t getNumMorphAnimation() const noexcept;

	AnimationPropertyPtr clone() noexcept;

	void updateIK() noexcept;
	void updateIK(const IKAttr& ik) noexcept;

	void updateBoneMotion(Bone& bone, int index);
	void updateBoneMatrix(Bone& bone);

	void update() noexcept;

	void getCurrentBoneMatrix(Matrix4x4& mat, Bone& bone);
	void getCurrentBonePosition(Vector3& v, Bone& bone);

	MotionSegment findMotionSegment(int frame, const std::vector<std::size_t>& motions);
	void interpolateMotion(Quaternion& rotation, Vector3& position, const std::vector<std::size_t>& motions, float frame);

private:

	std::string _name;
	std::size_t _frame;

	Bones _bones;
	InverseKinematics _iks;

	std::vector<BoneAnimation> _boneAnimation;
	std::vector<MorphAnimation> _morphAnimation;
	std::vector<std::vector<std::size_t>> _bindAnimation;
};

_NAME_END

#endif
