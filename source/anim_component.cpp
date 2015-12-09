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
#include <ray/resource.h>
#include <ray/model.h>
#include <ray/mstream.h>
#include <ray/ioserver.h>

_NAME_BEGIN

__ImplementSubClass(AnimotionComponent, GameComponent, "Animation")

AnimotionComponent::AnimotionComponent() noexcept
{
}

AnimotionComponent::~AnimotionComponent() noexcept
{
}

bool
AnimotionComponent::play(const std::string& filename) noexcept
{
	if (!filename.empty())
	{
		this->setName(filename);

		ResLoader<Model> loader;

		loader.load(filename,
			[&](ray::ModelPtr model, const std::string& name) {
			
			StreamReaderPtr stream;
			if (IoServer::instance()->openFile(stream, filename))
				return model->load(*stream);
			return false;
		}
		);

		auto model = loader.data();
		if (model && model->hasAnimations())
		{
			_sharedAnimtion = model->getAnimationList().back();
			_animtion = _sharedAnimtion->clone();
		}

		auto component = this->getGameObject()->getComponent<MeshComponent>();
		if (component && _animtion)
		{
			_mesh = component->getMesh();

			if (_mesh)
			{				
				_sharedMesh = component->getSharedMesh();

				_animtion->setBoneArray(_mesh->getBoneArray());
				_animtion->setIKArray(_mesh->getInverseKinematics());
				_animtion->setCurrentFrame(240);
				_animtion->update();
			}
		}

		return true;
	}

	return false;
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
	this->play(this->getName());
}

void
AnimotionComponent::onDeactivate() except
{
	if (_animtion)
	{
		_animtion.reset();
		_animtion = nullptr;
	}
}

void
AnimotionComponent::onFrameBegin() except
{
}

void
AnimotionComponent::onFrame() except
{
	/*if (_animtion)
		_animtion->update();*/

	//this->_updateVertex(_mesh, _sharedMesh);
}

void
AnimotionComponent::onFrameEnd() except
{
}

void
AnimotionComponent::_updateVertex(MeshPropertyPtr mesh, MeshPropertyPtr model)
{
	const auto& bonesWeight = model->getWeightArray();
	const auto& vertices = model->getVertexArray();

	const auto& bones = _animtion->getBoneArray();

	for (std::size_t i = 0; i < vertices.size(); i++)
	{
		const auto& weight = bonesWeight[i];

		const auto& b1 = bones[weight.bone1];
		const auto& b2 = bones[weight.bone2];
		const auto& b3 = bones[weight.bone3];
		const auto& b4 = bones[weight.bone4];

		auto p1 = vertices[i] - b1.getPosition();
		auto p2 = vertices[i] - b2.getPosition();
		auto p3 = vertices[i] - b3.getPosition();
		auto p4 = vertices[i] - b4.getPosition();

		auto v1 = b1.getTransform() * p1;
		auto v2 = b2.getTransform() * p2;
		auto v3 = b3.getTransform() * p3;
		auto v4 = b4.getTransform() * p4;

		mesh->getVertexArray()[i] = weight.weight1 * v1 + weight.weight2 * v2 + weight.weight3 * v3 + weight.weight4 * v4;
	}

	auto meshes = mesh->getChildren();
	for (std::size_t i = 0; i < meshes.size(); i++)
	{
		this->_updateVertex(meshes[i], model->getChildren()[i]);
	};

	mesh->computeBoundingBox();
}


_NAME_END