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

std::size_t
BoneAnimation::getBoneIndex() const noexcept
{
	return _bone;
}

void
BoneAnimation::setFrameNo(std::size_t frame) noexcept
{
	_frame = frame;
}

std::size_t
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
AnimationProperty::setBoneArray(const Bones& bones) noexcept
{
	if (bones.empty())
	{
		_bindAnimation.clear();
		return;
	}

	std::map<std::string, std::size_t> bindBoneMaps;
	for (std::size_t i = 0; i < bones.size(); i++)
	{
		bindBoneMaps[bones[i].getName()] = i + 1;
	}

	std::size_t numFrame = 0;
	std::size_t numAnimation = this->getNumBoneAnimation();

	for (std::size_t i = 0; i < numAnimation; i++)
	{
		numFrame = std::max(numFrame, _boneAnimation[i].getFrameNo());
	}

	_bindAnimation.resize(bones.size());

	for (std::size_t i = 0; i < numAnimation; i++)
	{
		auto& name = _boneAnimation[i].getName();
		auto& bone = bindBoneMaps[name];
		if (bone > 0)
		{
			_bindAnimation[bone - 1].push_back(i);
		}
	}
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
AnimationProperty::updateBone(Bones& bones) noexcept
{
	this->updateBoneMotion(bones);
	this->updateBoneMatrix(bones);
	this->updateIK(bones);
	_frame++;
}

void
AnimationProperty::updateBoneMotion(Bones& _bones) noexcept
{
	for (std::size_t i = 0; i < _bones.size(); i++)
	{
		auto& bone = _bones[i];
		const auto& motion = _bindAnimation[i];

		if (motion.empty())
		{
			bone.setRotation(Quaternion(0, 0, 0, 1));

			if (bone.getParent() != (-1))
			{
				auto& parent = _bones[bone.getParent()];
				auto position = (bone.getPosition() - parent.getPosition());
				float4x4 m;
				m.makeTranslate(position);
				bone.setLocalTransform(m);
			}
			else
			{
				float4x4 m;
				m.makeTranslate(bone.getPosition());
				bone.setLocalTransform(m);
			}
		}
		else
		{
			Vector3 position;
			Quaternion rotate;
			this->interpolateMotion(rotate, position, motion, _frame / 2);

			if (bone.getParent() == (-1))
			{
				bone.updateTransform(bone.getPosition() + position, rotate);
			}
			else
			{
				bone.updateTransform(bone.getPosition() + position - _bones[bone.getParent()].getPosition(), rotate);
			}
		}
	}
}

void
AnimationProperty::updateBoneMatrix(Bones& _bones) noexcept
{
	std::vector<std::int16_t> bones;

	std::size_t size = _bones.size();
	for (std::intptr_t i = 0; i < size; i++)
	{
		std::intptr_t  parent = _bones[i].getParent();
		if (parent > i)
		{
			bones.push_back(i);
		}
		else
		{
			if ((std::size_t)parent > size)
				_bones[i].setTransform(_bones[i].getLocalTransform());
			else
				_bones[i].setTransform(math::transformMultiply(_bones[i].getLocalTransform(), _bones[parent].getTransform()));
		}
	}

	for (std::size_t i = 0; i < bones.size(); i++)
	{
		std::size_t parent = _bones[i].getParent();
		if (parent > size)
			_bones[i].setTransform(_bones[i].getLocalTransform());
		else
			_bones[i].setTransform(math::transformMultiply(_bones[i].getLocalTransform(), _bones[parent].getTransform()));
	}
}

void
AnimationProperty::updateBoneChild(Bones& _bones, Bone& bone) noexcept
{
	if (bone.getParent() != (-1))
	{
		auto& parent = _bones.at(bone.getParent());
		bone.setTransform(math::transformMultiply(bone.getLocalTransform(), parent.getTransform()));
	}
	else
	{
		bone.setTransform(bone.getLocalTransform());
	}

	if (bone.getChild() != 0)
	{
		auto& child = _bones.at(bone.getChild());
		updateBoneMatrix(_bones, child);
	}
}

void
AnimationProperty::updateBoneMatrix(Bones& _bones, Bone& bone) noexcept
{
	if (bone.getParent() != (-1))
	{
		auto& parent = _bones.at(bone.getParent());
		updateBoneMatrix(_bones, parent);
		bone.setTransform(math::transformMultiply(bone.getLocalTransform(), parent.getTransform()));
	}
	else
	{
		bone.setTransform(bone.getLocalTransform());
	}
}

void 
AnimationProperty::updateBonePose(Bones& _bones) noexcept
{
	for (auto& bone : _bones)
	{
		float4x4 invbindpose;
		invbindpose.makeTranslate(-bone.getPosition());
		bone.setLocalTransform(math::transformMultiply(invbindpose, bone.getTransform()));
	}
}

void
AnimationProperty::updateIK(Bones& _bones) noexcept
{
	for (auto& ik : _iks)
		this->updateIK(_bones, ik);
}

void
AnimationProperty::updateIK(Bones& _bones, const IKAttr& ik) noexcept
{
	auto& effector = _bones.at(ik.boneIndex);
	auto& target = _bones.at(ik.targetBoneIndex);

	Vector3 targetPos = target.getTransform().getTranslate();

	for (std::uint32_t i = 0; i < ik.iterations; i++)
	{
		for (std::uint32_t j = 0; j < ik.chainLength; j++)
		{
			auto& bone = _bones[ik.child[j].BoneIndex];

			if (bone.getLeg())
			{
				if (i == 0)
				{
					auto& base = _bones[ik.child[ik.chainLength - 1].BoneIndex];

					Vector3 localTargetPos = bone.getTransform().getTranslate();
					Vector3 localEffectPos = base.getTransform().getTranslate();

					Vector3 effectVec = effector.getTransform().getTranslate() - localEffectPos;
					Vector3 boneVec = localTargetPos - localEffectPos;
					Vector3 targetVec = targetPos - localTargetPos;

					float el = math::length(effectVec);
					float bl = math::length(boneVec);
					float tl = math::length(targetVec);
					float c = math::clamp((el * el - bl * bl - tl * tl) / (2.0f * bl * tl), -1.0f, 1.0f);

					float rotationAngle = acos(c);
					Vector3 rotationAxis(-1.0f, 0.0f, 0.0f);

					Quaternion q0(rotationAxis, RAD_TO_DEG(rotationAngle));
					Quaternion qq = math::cross(bone.getRotation(), q0);
					bone.updateTransform(bone.getLocalTransform().getTranslate(), qq);

					this->updateBoneMatrix(_bones, bone);
					this->updateBoneMatrix(_bones, target);
				}
			}
			else
			{
				Vector3 effectPos = effector.getTransform().getTranslate();
				if (math::distance(effectPos, targetPos) < 0.001)
					return;

				Vector3 dstLocal = math::invTranslateVector3(bone.getTransform(), target.getTransform().getTranslate());
				Vector3 srcLocal = math::invTranslateVector3(bone.getTransform(), effectPos);

				srcLocal = math::normalize(srcLocal);
				dstLocal = math::normalize(dstLocal);

				float rotationDotProduct = math::clamp(math::dot(dstLocal, srcLocal), -1.0f, 1.0f);
				float rotationAngle = std::acos(rotationDotProduct);
				rotationAngle *= ik.weight;

				if (rotationAngle > 1.0e-5f)
				{
					Vector3 rotationAxis = math::cross(dstLocal, srcLocal);
					rotationAxis = math::normalize(rotationAxis);

					Quaternion q0(rotationAxis, RAD_TO_DEG(rotationAngle));
					Quaternion qq = math::cross(bone.getRotation(), q0);

					bone.updateTransform(bone.getLocalTransform().getTranslate(), qq);
				}

				this->updateBoneMatrix(_bones, bone);
				this->updateBoneMatrix(_bones, target);
			}
		}
	}
}

MotionSegment
AnimationProperty::findMotionSegment(int frame, const std::vector<std::size_t>& motions) noexcept
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

static float BezierEval(const std::uint8_t* ip, float t) noexcept
{
	float xa = ip[0] / 256.0;
	float xb = ip[2] / 256.0;
	float ya = ip[1] / 256.0;
	float yb = ip[3] / 256.0;

	float min = 0;
	float max = 1;

	float ct = t;
	for (;;)
	{
		float x11 = xa * ct;
		float x12 = xa + (xb - xa) * ct;
		float x13 = xb + (1.0 - xb) * ct;

		float x21 = x11 + (x12 - x11) * ct;
		float x22 = x12 + (x13 - x12) * ct;

		float x3 = x21 + (x22 - x21) * ct;

		if (std::fabs(x3 - t) < 0.0001)
		{
			float y11 = ya * ct;
			float y12 = ya + (yb - ya) * ct;
			float y13 = yb + (1.0 - yb) * ct;

			float y21 = y11 + (y12 - y11) * ct;
			float y22 = y12 + (y13 - y12) * ct;

			float y3 = y21 + (y22 - y21) * ct;

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
AnimationProperty::interpolateMotion(Quaternion& rotation, Vector3& position, const std::vector<std::size_t>& motions, float frame) noexcept
{
	auto ms = findMotionSegment(frame, motions);

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

		rotation = math::slerp(r0, r1, tr);
	}
}

_NAME_END