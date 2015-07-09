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
#include <ray/mesh_render_component.h>
#include <ray/mesh_component.h>

#include <ray/render_features.h>
#include <ray/render_factory.h>
#include <ray/render_buffer.h>

#include <ray/material_maker.h>
#include <ray/game_server.h>
#include <ray/resource.h>

_NAME_BEGIN

__ImplementSubClass(MeshRenderComponent, GameComponent)

MeshRenderComponent::MeshRenderComponent() noexcept
{
}

MeshRenderComponent::~MeshRenderComponent() noexcept
{
}

GameComponentPtr
MeshRenderComponent::clone() const noexcept
{
	auto result = std::make_shared<MeshRenderComponent>();
	result->setCastShadow(this->getCastShadow());
	result->setReceiveShadow(this->getReceiveShadow());
	result->setName(this->getName());
	result->setVisible(this->getVisible());

	for (auto& it : this->getSharedMaterials())
	{
		result->addMaterial(it);
		result->addSharedMaterial(it);
	}

	return result;
}

void
MeshRenderComponent::onAttach() except
{
	RenderComponent::onAttach();

	if (!this->hasSharedMaterial())
	{
		auto component = this->getGameObject()->getComponent<MeshComponent>();
		if (!component)
			return;

		auto model = ResLoader<Model>::find(component->getName());
		if (!model)
			return;

		_buildMaterials(model);
	}
}

void
MeshRenderComponent::onRemove() noexcept
{
	RenderComponent::onRemove();
}

void
MeshRenderComponent::onActivate() except
{
	RenderComponent::onActivate();

	auto component = this->getGameObject()->getComponent<MeshComponent>();
	if (!component)
		return;

	auto mesh = component->getMesh();
	if (!mesh)
		return;

	auto model = ResLoader<Model>::find(component->getName());
	if (!model)
		return;

	_buildRenderObjects(mesh);
	_attacRenderObjects();
}

void
MeshRenderComponent::onDeactivate() noexcept
{
	RenderComponent::onDeactivate();

	_dttachRenderhObjects();

	if (_renderObjects.empty())
	{
		for (auto& it : _renderObjects)
			it.reset();

		_renderObjects.clear();
	}

	if (_renderBuffers.empty())
	{
		for (auto& it : _renderBuffers)
			it.reset();

		_renderBuffers.clear();
	}
}

void
MeshRenderComponent::onMoveAfter() noexcept
{
	RenderComponent::onMoveAfter();

	for (auto& it : _renderObjects)
	{
		it->setTransform(this->getGameObject()->getTransform());
		it->setTransformInverse(this->getGameObject()->getTransformInverse());
		it->setTransformInverseTranspose(this->getGameObject()->getTransformInverseTranspose());
	}
}

void
MeshRenderComponent::onLayerChangeAfter() noexcept
{
	RenderComponent::onLayerChangeAfter();

	for (auto& it : _renderObjects)
	{
		it->setLayer(this->getGameObject()->getLayer());
	}
}

void
MeshRenderComponent::_attacRenderObjects() noexcept
{
	for (auto& it : _renderObjects)
	{
		auto renderer = this->getGameObject()->getGameServer()->getFeature<RenderFeatures>();
		if (renderer)
		{
			auto renderScene = renderer->getRenderScene(this->getGameObject()->getGameScene());
			if (renderScene)
			{
				it->setRenderScene(renderScene);
			}
		}
	}
}

void
MeshRenderComponent::_dttachRenderhObjects() noexcept
{
	for (auto& it : _renderObjects)
	{
		it->setRenderScene(nullptr);
		it = nullptr;
	}

	_renderObjects.clear();
}

void
MeshRenderComponent::_buildMaterials(ModelPtr model) except
{
	auto materials = model->getMaterialsList();
	for (auto& it : materials)
	{
		std::string diffuseName;
		TexturePtr diffuseTexture;

		float transparent = 1;
		float shininess = 0;
		Vector3 diffuse = Vector3::Zero;
		Vector3 specular = Vector3::Zero;
		it->get(MATKEY_TEXTURE_DIFFUSE(0), diffuseName);
		it->get(MATKEY_OPACITY, transparent);
		it->get(MATKEY_SHININESS, shininess);
		it->get(MATKEY_COLOR_DIFFUSE, diffuse);
		it->get(MATKEY_COLOR_SPECULAR, specular);

		MaterialPtr material = nullptr;

		MaterialMaker maker;
		if (transparent == 1)
			material = maker.load("sys:fx/opacity.glsl");
		else
			material = maker.load("sys:fx/transparent.glsl");

		this->addMaterial(material);
		this->addSharedMaterial(material);

		if (!diffuseName.empty())
		{
			const std::string& directory = model->getDirectory();
			if (!directory.empty())
			{
				diffuseTexture = RenderFactory::createTexture(directory + diffuseName);
				if (diffuseTexture)
				{
					material->getParameter("decal")->assign(diffuseTexture);
					material->getParameter("diffuse")->assign(Vector4(diffuse, 0));
				}
			}
		}
		else
		{
			material->getParameter("diffuse")->assign(Vector4(diffuse, 1));
		}

		material->getParameter("opacity")->assign(transparent);
		material->getParameter("shininess")->assign(shininess);
		material->getParameter("specular")->assign(specular);
	}
}

void
MeshRenderComponent::_buildRenderObjects(MeshPropertyPtr mesh) noexcept
{
	auto material = this->getMaterial(mesh->getMaterialID());
	if (material)
	{
		auto renderBuffer = RenderFactory::createRenderBuffer(*mesh);
		auto renderObject = std::make_shared<RenderMesh>();

		renderObject->setRenderBuffer(renderBuffer, VertexType::GPU_TRIANGLE);
		renderObject->setBoundingBox(mesh->getBoundingBox());

		renderObject->setMaterial(material);

		renderObject->setCastShadow(this->getCastShadow());
		renderObject->setReceiveShadow(this->getReceiveShadow());

		renderObject->setLayer(this->getGameObject()->getLayer());

		renderObject->setTransform(this->getGameObject()->getTransform());
		renderObject->setTransformInverse(this->getGameObject()->getTransformInverse());
		renderObject->setTransformInverseTranspose(this->getGameObject()->getTransformInverseTranspose());

		_renderObjects.push_back(renderObject);
		_renderBuffers.push_back(renderBuffer);
	}

	auto meshes = mesh->getChildren();
	for (std::size_t i = 0; i < meshes.size(); i++)
	{
		this->_buildRenderObjects(meshes[i]);
	};
}

_NAME_END