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
#include <ray/anim.h>
#include <ray/modhelp.h>

_NAME_BEGIN

BoneAnimation::BoneAnimation() noexcept
	: _bone(0)
	, _frame(-1)
{
}

BoneAnimation::~BoneAnimation() noexcept
{
}

void
BoneAnimation::setName(const std::string& name) noexcept
{
	_name = name;
}

const std::string&
BoneAnimation::getName() const noexcept
{
	return _name;
}

void
BoneAnimation::setPosition(const Vector3& position) noexcept
{
	_position = position;
}

const Vector3&
BoneAnimation::getPosition() const noexcept
{
	return _position;
}

void
BoneAnimation::setRotation(const Quaternion& quat) noexcept
{
	_rotation = quat;
}

const Quaternion&
BoneAnimation::getRotation() const noexcept
{
	return _rotation;
}

void
BoneAnimation::setBoneIndex(const std::size_t bone) noexcept
{
	_bone = bone;
}

std::int32_t
BoneAnimation::getBoneIndex() const noexcept
{
	return _bone;
}

void
BoneAnimation::setFrameNo(std::int32_t frame) noexcept
{
	_frame = frame;
}

std::int32_t
BoneAnimation::getFrameNo() const noexcept
{
	return _frame;
}

void
BoneAnimation::setInterpolation(const Interpolation& interp) noexcept
{
	_interpolation = interp;
}

const Interpolation&
BoneAnimation::getInterpolation() const noexcept
{
	return _interpolation;
}

AnimationProperty::AnimationProperty() noexcept
	:_frame(0)
{
}

AnimationProperty::~AnimationProperty() noexcept
{
}

void
AnimationProperty::setName(const std::string& name) noexcept
{
	_name = name;
}

const std::string&
AnimationProperty::getName() const noexcept
{
	return _name;
}

void
AnimationProperty::setCurrentFrame(std::size_t frame) noexcept
{
	_frame = frame;
}

std::size_t
AnimationProperty::getCurrentFrame() const noexcept
{
	return _frame;
}

void
AnimationProperty::addBoneAnimation(const BoneAnimation& anim) noexcept
{
	_boneAnimation.push_back(anim);
}

BoneAnimation&
AnimationProperty::getBoneAnimation(std::size_t index) noexcept
{
	return _boneAnimation[index];
}

const BoneAnimation&
AnimationProperty::getBoneAnimation(std::size_t index) const noexcept
{
	return _boneAnimation[index];
}

std::size_t
AnimationProperty::getNumBoneAnimation() const noexcept
{
	return _boneAnimation.size();
}

void
AnimationProperty::addMorphAnimation(const MorphAnimation& anim) noexcept
{
	_morphAnimation.push_back(anim);
}

MorphAnimation&
AnimationProperty::getMorphAnimation(std::size_t index) noexcept
{
	return _morphAnimation[index];
}

const MorphAnimation&
AnimationProperty::getMorphAnimation(std::size_t index) const noexcept
{
	return _morphAnimation[index];
}

std::size_t
AnimationProperty::getNumMorphAnimation() const noexcept
{
	return _morphAnimation.size();
}

void
AnimationProperty::setBoneArray(Bones bones) noexcept
{
	if (bones.empty())
	{
		_bones.clear();
		_bindAnimation.clear();
		return;
	}

	std::map<std::string, std::vector<std::size_t>> bindMap;

	std::size_t numAnimation = this->getNumBoneAnimation();
	for (std::size_t i = 0; i < numAnimation; i++)
	{
		bindMap[this->getBoneAnimation(i).getName()].push_back(i);
	}

	_bindAnimation.resize(bones.size());

	for (std::size_t i = 0; i < bones.size(); i++)
	{
		auto& motion = bindMap[bones[i].getName()];
		if (!motion.empty())
		{
			_bindAnimation[i] = motion;
		}
	}

	_bones = bones;
}

const Bones&
AnimationProperty::getBoneArray() const noexcept
{
	return _bones;
}

void 
AnimationProperty::setIKArray(InverseKinematics ik) noexcept
{
	_iks = ik;
}

InverseKinematics
AnimationProperty::getIKArray() noexcept
{
	return _iks;
}

AnimationPropertyPtr
AnimationProperty::clone() noexcept
{
	auto anim = std::make_shared<AnimationProperty>();
	anim->_name = this->_name;
	anim->_boneAnimation = this->_boneAnimation;
	anim->_morphAnimation = this->_morphAnimation;
	anim->_frame = this->_frame;
	return anim;
}

void
AnimationProperty::update() noexcept
{
	for (std::size_t i = 0; i < _bones.size(); i++)
	{
		this->updateBoneMotion(_bones[i], i);
	}

	this->updateIK();

	for (auto& bone : _bones)
	{
		this->updateBoneMatrix(bone);
	}

	_frame++;
}

void
AnimationProperty::updateBoneMotion(Bone& bone, int index)
{
	assert(bone.getParent() < index);

	const auto& motion = _bindAnimation[index];

	if (motion.empty())
	{
		bone.setRotation(Quaternion(0, 0, 0, 1));

		if (bone.getParent() != (-1))
		{
			auto& parent = _bones[bone.getParent()];
			auto position = (bone.getPosition() - parent.getPosition());
			Matrix4x4 m;
			m.makeTranslate(position);
			bone.setLocalTransform(m);
		}
		else
		{
			Matrix4x4 m;
			m.makeTranslate(bone.getPosition());
			bone.setLocalTransform(m);
		}
	}
	else
	{
		Vector3 position;
		Quaternion rotate;
		this->interpolateMotion(rotate, position, motion, _frame);

		if (bone.getParent() == (-1))
		{
			Matrix4x4 m;
			m.makeRotate(rotate);
			m.setTranslate(bone.getPosition() + position);	

			bone.setRotation(rotate);
			bone.setLocalTransform(m);
		}
		else
		{
			Matrix4x4 m;
			m.makeRotate(rotate);
			m.setTranslate(bone.getPosition() + position - _bones[bone.getParent()].getPosition());

			bone.setRotation(rotate);
			bone.setLocalTransform(m);
		}
	}
}

void
AnimationProperty::updateBoneMatrix(Bone& bone)
{
	if (bone.getParent() != (-1))
	{
		auto parent = _bones.at(bone.getParent());
		updateBoneMatrix(parent);
		bone.setTransform(parent.getTransform() * bone.getLocalTransform());
	}
	else
	{
		bone.setTransform(bone.getLocalTransform());
	}
}

void
AnimationProperty::getCurrentBoneMatrix(Matrix4x4& mat, Bone& bone)
{
	updateBoneMatrix(bone);
	mat = bone.getTransform();
}

void
AnimationProperty::getCurrentBonePosition(Vector3& v, Bone& bone)
{
	updateBoneMatrix(bone);
	v = bone.getTransform().getTranslate();
}

void
AnimationProperty::updateIK() noexcept
{
	for (auto& ik : _iks)
		this->updateIK(ik);
}

void
AnimationProperty::updateIK(const IKAttr& ik) noexcept
{
	auto effector = _bones.at(ik.IKBoneIndex);
	auto target = _bones.at(ik.IKTargetBoneIndex);

	Vector3 effectorPos;
	getCurrentBonePosition(effectorPos, effector);

	for (std::uint32_t i = 0; i < ik.IKLoopCount; i++)
	{
		for (std::uint32_t j = 0; j < ik.IKLinkCount; j++)
		{
			auto bone = _bones[ik.IKList[j].BoneIndex];

			Vector3 targetPos;
			getCurrentBonePosition(targetPos, target);

			Matrix4x4 worldInverse;
			getCurrentBoneMatrix(worldInverse, bone);
			worldInverse.inverse();

			Vector3 basis2Effector = worldInverse * effectorPos;
			Vector3 basis2Target = worldInverse * targetPos;

			basis2Effector.normalize();
			basis2Target.normalize();

			float rotationDotProduct = basis2Effector.dot(basis2Target);
			rotationDotProduct = clamp(rotationDotProduct, -1.0f, 1.0f);

			float rotationAngle = acosf(rotationDotProduct);
			rotationAngle /= ik.IKLinkCount;

			Vector3 rotationAxis = basis2Target.cross(basis2Effector);
			rotationAxis.normalize();

			Quaternion q0(rotationAxis, rotationAngle);
			Quaternion q1 = bone.getRotation();
			Quaternion qq = q1 * q0;

			Matrix4x4 m;
			m.makeRotate(qq);
			m.setTranslate(bone.getLocalTransform().getTranslate());

			bone.setRotation(qq);
			bone.setLocalTransform(m);
		}
	}
}

MotionSegment
AnimationProperty::findMotionSegment(int frame, const std::vector<std::size_t>& motions)
{
	MotionSegment ms;
	ms.m0 = 0;
	ms.m1 = motions.size() - 1;

	auto& anim1 = this->getBoneAnimation(motions[ms.m1]);

	if (frame >= anim1.getFrameNo())
	{
		ms.m0 = ms.m1;
		ms.m1 = -1;
		return ms;
	}

	for (;;)
	{
		int middle = (ms.m0 + ms.m1) / 2;
		auto& anim2 = this->getBoneAnimation(motions[middle]);

		if (middle == ms.m0)
			return ms;

		if (anim2.getFrameNo() == frame)
		{
			ms.m0 = middle;
			ms.m1 = -1;
			return ms;
		}

		if (anim2.getFrameNo() > frame)
			ms.m1 = middle;
		else
			ms.m0 = middle;
	}
}

static double BezierEval(const std::uint8_t* ip, float t)
{
	double xa = ip[0] / 256.0;
	double xb = ip[2] / 256.0;
	double ya = ip[1] / 256.0;
	double yb = ip[3] / 256.0;

	double min = 0;
	double max = 1;

	double ct = t;
	for (;;)
	{
		double x11 = xa * ct;
		double x12 = xa + (xb - xa) * ct;
		double x13 = xb + (1.0 - xb) * ct;

		double x21 = x11 + (x12 - x11) * ct;
		double x22 = x12 + (x13 - x12) * ct;

		double x3 = x21 + (x22 - x21) * ct;

		if (std::fabs(x3 - t) < 0.0001)
		{
			double y11 = ya * ct;
			double y12 = ya + (yb - ya) * ct;
			double y13 = yb + (1.0 - yb) * ct;

			double y21 = y11 + (y12 - y11) * ct;
			double y22 = y12 + (y13 - y12) * ct;

			double y3 = y21 + (y22 - y21) * ct;

			return y3;
		}
		else if (x3 < t)
		{
			min = ct;
		}
		else
		{
			max = ct;
		}

		ct = min * 0.5 + max * 0.5;
	}
}

void
AnimationProperty::interpolateMotion(Quaternion& rotation, Vector3& position, const std::vector<std::size_t>& motions, float frame)
{
	auto ms = findMotionSegment(_frame, motions);

	if (ms.m1 == -1)
	{
		position = this->getBoneAnimation(motions[ms.m0]).getPosition();
		rotation = this->getBoneAnimation(motions[ms.m0]).getRotation();
	}
	else
	{
		auto& anim0 = this->getBoneAnimation(motions[ms.m0]);
		auto& anim1 = this->getBoneAnimation(motions[ms.m1]);

		int diff = anim1.getFrameNo() - anim0.getFrameNo();
		float a0 = frame - anim0.getFrameNo();
		float ratio = a0 / diff;

		float tx = BezierEval(anim0.getInterpolation().interpX, ratio);
		float ty = BezierEval(anim0.getInterpolation().interpY, ratio);
		float tz = BezierEval(anim0.getInterpolation().interpZ, ratio);
		float tr = BezierEval(anim0.getInterpolation().interpW, ratio);

		position = Vector3(1 - tx, 1 - ty, 1 - tz) * anim0.getPosition();
		position += Vector3(tx, ty, tz) * anim1.getPosition();

		Quaternion r0 = anim0.getRotation();
		Quaternion r1 = anim1.getRotation();

		rotation.slerp(r0, r1, tr);
	}
}

_NAME_END