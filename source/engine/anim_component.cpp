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
#include <ray/anim_component.h>
#include <ray/mesh_component.h>
#include <ray/res_loader.h>
#include <ray/model.h>
#include <ray/mstream.h>
#include <ray/ioserver.h>
#include <ray/utf8.h>
#include <ray/game_server.h>

_NAME_BEGIN

__ImplementSubClass(AnimotionComponent, GameComponent, "Animotion")

AnimotionComponent::AnimotionComponent() noexcept
	: _playing(true)
{
}

AnimotionComponent::~AnimotionComponent() noexcept
{
}

bool
AnimotionComponent::play(const std::string& filename) noexcept
{
	if (this->getName() != filename)
	{
		_buildAnimotion(filename);
		this->setName(filename);
	}

	_playing = _animtion ? true : false;
	return _playing;
}

void
AnimotionComponent::load(iarchive& reader) noexcept
{
	GameComponent::load(reader);
}

void
AnimotionComponent::save(oarchive& write) noexcept
{
}

GameComponentPtr
AnimotionComponent::clone() const noexcept
{
	auto animtion = std::make_shared<AnimotionComponent>();
	animtion->_animtion = _animtion;
	return animtion;
}

void
AnimotionComponent::onActivate() except
{
	_playing = _buildAnimotion(this->getName());
}

void
AnimotionComponent::onDeactivate() noexcept
{
	if (_animtion)
	{
		_animtion.reset();
		_animtion = nullptr;
	}

	_playing = false;
}

void
AnimotionComponent::onAttach() noexcept
{
	_meshComponent = this->getComponent<MeshComponent>();
}

void
AnimotionComponent::onDetach() noexcept
{
	_meshComponent.reset();
}

void
AnimotionComponent::onAttachComponent(GameComponentPtr& component) noexcept
{
	if (component->isInstanceOf<MeshComponent>())
		_meshComponent = component;
}

void
AnimotionComponent::onDetachComponent(GameComponentPtr& component) noexcept
{
	if (component->isInstanceOf<MeshComponent>())
		_meshComponent.reset();
}

void
AnimotionComponent::onFrameEnd() noexcept
{
	if (_animtion && _playing)
	{
		auto component = _meshComponent.lock();
		if (!component)
			return;

		auto mesh = component->downcast<MeshComponent>()->getMesh();
		if (component->downcast<MeshComponent>()->getMesh())
		{
			_animtion->updateBone(mesh->getBoneArray(), this->getGameServer()->getTimer()->delta());
			_animtion->updateBonePose(mesh->getBoneArray());

			mesh->computeBoundingBoxSkeleton();
		}
	}
}

bool
AnimotionComponent::_buildAnimotion(const std::string& filename) noexcept
{
	if (filename.empty())
		return false;

	if (!_meshComponent.lock())
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
	{
		return false;
	}

	_sharedAnimtion = model->getAnimationList().back();
	_animtion = _sharedAnimtion->clone();

	auto component = _meshComponent.lock()->downcast<MeshComponent>();
	if (component && _animtion)
	{
		auto mesh = component->getMesh();
		if (mesh)
		{
			_animtion->setBoneArray(mesh->getBoneArray());
			_animtion->setIKArray(mesh->getInverseKinematics());
			_animtion->setCurrentFrame(0);
			_animtion->updateBone(mesh->getBoneArray(), 0);
			_animtion->updateBonePose(mesh->getBoneArray());
		}

		_meshComponent = component;
	}

	return true;
}

_NAME_END