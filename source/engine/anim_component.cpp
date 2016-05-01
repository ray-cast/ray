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
AnimotionComponent::onFrameBegin() except
{
}

void
AnimotionComponent::onFrame() except
{
	if (_animtion && _playing)
	{
		auto component = _meshComponent.lock();
		if (component)
		{
			_animtion->updateBone(component->downcast<MeshComponent>()->getMesh()->getBoneArray());
			_animtion->updateBonePose(component->downcast<MeshComponent>()->getMesh()->getBoneArray());
			/*_updateVertex(component->downcast<MeshComponent>()->getMesh()->getBoneArray(),
				component->downcast<MeshComponent>()->getMesh(),
				component->downcast<MeshComponent>()->getSharedMesh());
			component->downcast<MeshComponent>()->needUpdate();*/
		}
	}
}

void
AnimotionComponent::onFrameEnd() except
{
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
			_animtion->setCurrentFrame(1399);
			_animtion->updateBone(mesh->getBoneArray());
			_animtion->updateBonePose(mesh->getBoneArray());

			this->_updateVertex(mesh->getBoneArray(), mesh, component->getSharedMesh());
		}

		_meshComponent = component;
	}

	return true;
}

void
AnimotionComponent::_updateVertex(const Bones& bones, MeshPropertyPtr mesh, MeshPropertyPtr model)
{
	const auto& bonesWeight = model->getWeightArray();
	const auto& vertices = model->getVertexArray();
	const auto& normals = model->getNormalArray();

	for (std::size_t i = 0; i < vertices.size(); i++)
	{
		const auto& v = vertices[i];
		const auto& n = normals[i];

		const auto& weight = bonesWeight[i];

		const auto& b1 = bones[weight.bone1];
		const auto& b2 = bones[weight.bone2];
		const auto& b3 = bones[weight.bone3];
		const auto& b4 = bones[weight.bone4];

		auto v1 = v * b1.getLocalTransform();
		auto v2 = v * b2.getLocalTransform();
		auto v3 = v * b3.getLocalTransform();
		auto v4 = v * b4.getLocalTransform();

		auto n1 = n * float3x3(b1.getLocalTransform());
		auto n2 = n * float3x3(b2.getLocalTransform());
		auto n3 = n * float3x3(b3.getLocalTransform());
		auto n4 = n * float3x3(b4.getLocalTransform());

		mesh->getVertexArray()[i] = weight.weight1 * v1 + weight.weight2 * v2 + weight.weight3 * v3 + weight.weight4 * v4;
		mesh->getNormalArray()[i] = math::normalize(weight.weight1 * n1 + weight.weight2 * n2 + weight.weight3 * n3 + weight.weight4 * n4);
	}

	auto meshes = mesh->getChildren();
	for (std::size_t i = 0; i < meshes.size(); i++)
	{
		this->_updateVertex(bones, meshes[i], model->getChildren()[i]);
	};

	mesh->computeBoundingBox();
}

_NAME_END