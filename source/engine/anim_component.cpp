// +----------------------------------------------------------------------
// | Project : ray.
// | All rights reserved.
// +----------------------------------------------------------------------
// | Copyright (c) 2013-2016.
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
#include <ray/anim_component.h>
#include <ray/mesh_component.h>
#include <ray/res_loader.h>
#include <ray/model.h>
#include <ray/mstream.h>
#include <ray/ioserver.h>
#include <ray/game_server.h>
#include <ray/render_component.h>
#include <ray/ik_solver_component.h>

_NAME_BEGIN

__ImplementSubClass(AnimationComponent, GameComponent, "Animation")

AnimationComponent::AnimationComponent() noexcept
	: _enableAnimation(true)
	, _enableAnimOnVisableOnly(false)
	, _enablePhysics(false)
	, _needUpdate(false)
	, _onMeshChange(std::bind(&AnimationComponent::onMeshChange, this))
	, _onMeshWillRender(std::bind(&AnimationComponent::onMeshWillRender, this, std::placeholders::_1))	
{
}

AnimationComponent::~AnimationComponent() noexcept
{
}

bool
AnimationComponent::play(const std::string& filename) noexcept
{
	if (this->getName() != filename)
	{
		_playAnimation(filename);
		this->setName(filename);
	}

	return _enableAnimation;
}

void 
AnimationComponent::pause() noexcept
{
	_enableAnimation = false;
}

void 
AnimationComponent::stop() noexcept
{
	_destroyAnimation();
}

void 
AnimationComponent::enableAnimOnVisiable(bool visiableOnly) noexcept
{
	_enableAnimOnVisableOnly = visiableOnly;
}

bool 
AnimationComponent::enableAnimOnVisiable() const noexcept
{
	return _enableAnimOnVisableOnly;
}

void
AnimationComponent::enablePhysics(bool physics) noexcept
{
	_enablePhysics = physics;
}

bool 
AnimationComponent::enablePhysics() const noexcept
{
	return _enablePhysics;
}

void
AnimationComponent::setTransforms(const GameObjects& transforms) noexcept
{
	_transforms = transforms;
}

void 
AnimationComponent::setTransforms(GameObjects&& transforms) noexcept
{
	_transforms = std::move(transforms);
}

const GameObjects& 
AnimationComponent::getTransforms() const noexcept
{
	return _transforms;
}

void
AnimationComponent::load(iarchive& reader) noexcept
{
	GameComponent::load(reader);
}

void
AnimationComponent::save(oarchive& write) noexcept
{
}

GameComponentPtr
AnimationComponent::clone() const noexcept
{
	auto animtion = std::make_shared<AnimationComponent>();
	animtion->_animtion = _animtion;
	return animtion;
}

void
AnimationComponent::onActivate() except
{
	_playAnimation(this->getName());
}

void
AnimationComponent::onDeactivate() noexcept
{
	_destroyAnimation();
}

void
AnimationComponent::onAttachComponent(GameComponentPtr& component) noexcept
{
	if (component->isA<RenderComponent>())
		component->cast<RenderComponent>()->addPreRenderListener(&_onMeshWillRender);
}

void
AnimationComponent::onDetachComponent(GameComponentPtr& component) noexcept
{
	if (component->isA<RenderComponent>())
		component->cast<RenderComponent>()->removePreRenderListener(&_onMeshWillRender);
}

void
AnimationComponent::onFrameEnd() noexcept
{
	if (_enableAnimation && !_enableAnimOnVisableOnly)
		_updateAnimation();
	else
		_needUpdate = true;
}

void
AnimationComponent::onMeshChange() noexcept
{
	if (_enableAnimation)
		_playAnimation(this->getName());
}

void
AnimationComponent::onMeshWillRender(RenderPipeline& pipeline) noexcept
{
	if (_needUpdate && _enableAnimOnVisableOnly)
	{
		_updateAnimation();
		_needUpdate = false;
	}
}

bool
AnimationComponent::_playAnimation(const std::string& filename) noexcept
{
	this->_destroyAnimation();

	if (filename.empty())
		return false;

	ResLoader<Model> loader;
	loader.load(filename,
		[&](ray::ModelPtr model, const std::string& name)
	{
		StreamReaderPtr stream;
		if (IoServer::instance()->openFile(stream, name))
			return model->load(*stream);
		return false;
	}
	);

	auto model = loader.data();
	if (!model && !model->hasAnimations())
		return false;
	
	std::intptr_t i = 0;
	std::map<std::string, std::intptr_t> boneMap;
	for (auto& it : _transforms)
	{
		boneMap[it->getName()] = i++;
	}

	std::size_t index = 0;

	std::vector<Bone> bones;
	std::vector<IKAttr> iks;

	for (auto& it : _transforms)
	{
		Bone bone;
		bone.setName(it->getName());
		bone.setPosition(it->getTranslate());
		if (it->getParent())
			bone.setParent(boneMap[it->getParent()->getName()]);
		else
			bone.setParent(-1);

		auto iksolver = it->getComponent<IKSolverComponent>();
		if (iksolver)
		{
			IKAttr attr;
			attr.boneIndex = index;
			attr.targetBoneIndex = boneMap[iksolver->getTargetBone()->getName()];
			attr.chainLength = iksolver->getChainLength();
			attr.iterations = iksolver->getIterations();

			for (auto& child : iksolver->getBones())
			{
				IKChild ikbone;
				ikbone.boneIndex = boneMap[child->bone->getName()];
				ikbone.angleWeight = child->angleWeight;
				ikbone.rotateLimited = child->rotateLimited;
				ikbone.minimumDegrees = child->minimumDegrees;
				ikbone.maximumDegrees = child->maximumDegrees;

				attr.child.push_back(ikbone);
			}

			iks.push_back(attr);
		}

		bones.push_back(bone);

		++index;
	}

	_animtion = model->getAnimationList().back()->clone();
	_animtion->setBoneArray(bones);
	_animtion->setIKArray(iks);
	_animtion->updateMotion(0.0f);

	_enableAnimation = true;
	return true;
}

void
AnimationComponent::_updateAnimation() noexcept
{
	if (_animtion)
	{
		_animtion->updateMotion(GameServer::instance()->getTimer()->delta());

		std::size_t i = 0;
		for (auto& it : _animtion->getBoneArray())
			_transforms[i++]->setTransform(it.getTransform());
	}
}

void 
AnimationComponent::_destroyAnimation() noexcept
{
	_animtion.reset();
	_enableAnimation = false;
}

_NAME_END