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
#if defined(_BUILD_RENDERER)
#include <ray/mesh_render_component.h>
#include <ray/mesh_component.h>

#include <ray/render_feature.h>
#include <ray/render_system.h>
#include <ray/render_mesh.h>
#include <ray/geometry.h>
#include <ray/material.h>

#include <ray/game_server.h>
#include <ray/graphics_context.h>
#include <ray/resource.h>

_NAME_BEGIN

__ImplementSubClass(MeshRenderComponent, GameComponent, "MeshRender")

MeshRenderComponent::MeshRenderComponent() noexcept
{
}

MeshRenderComponent::~MeshRenderComponent() noexcept
{
	this->_dettachRenderhObjects();
}

void
MeshRenderComponent::setMaterial(MaterialPtr material) noexcept
{
	_material = material;
}

void
MeshRenderComponent::setSharedMaterial(MaterialPtr material) noexcept
{
	_sharedMaterial = material;
}

MaterialPtr
MeshRenderComponent::getMaterial() const noexcept
{
	return _material;
}

MaterialPtr
MeshRenderComponent::getSharedMaterial() const noexcept
{
	return _sharedMaterial;
}

bool
MeshRenderComponent::hasMaterial() const noexcept
{
	return _material != nullptr;
}

bool
MeshRenderComponent::hasSharedMaterial() const noexcept
{
	return _sharedMaterial != nullptr;
}

void
MeshRenderComponent::_attacRenderObjects() noexcept
{
	auto gameServer = this->getGameObject()->getGameServer();
	if (!gameServer) { assert(gameServer); return; }

	auto renderer = gameServer->getFeature<RenderFeature>();
	if (!renderer) { assert(renderer); return; }

	auto renderScene = renderer->getRenderScene(this->getGameObject()->getGameScene());
	if (!renderScene) { assert(renderScene); return; }

	for (auto& it : _renderObjects)
		it->setRenderScene(renderScene);
}

void
MeshRenderComponent::_dettachRenderhObjects() noexcept
{
	for (auto& it : _renderObjects)
		it->setRenderScene(nullptr);
	_renderObjects.clear();
}

void
MeshRenderComponent::load(iarchive& reader) noexcept
{
	RenderComponent::load(reader);

	std::string material;
	reader >> make_archive(material, "material");

	this->setName(material);
}

void
MeshRenderComponent::save(oarchive& write) noexcept
{
	RenderComponent::save(write);
}

GameComponentPtr
MeshRenderComponent::clone() const noexcept
{
	auto result = std::make_shared<MeshRenderComponent>();
	result->setName(this->getName());
	result->setActive(this->getActive());
	result->setCastShadow(this->getCastShadow());
	result->setReceiveShadow(this->getReceiveShadow());

	if (_sharedMaterial)
	{
		result->setMaterial(_sharedMaterial->clone());
		result->setSharedMaterial(_sharedMaterial);
	}

	return result;
}

void
MeshRenderComponent::onAttachComponent(GameComponentPtr& component) except
{
	if (component->isInstanceOf<MeshComponent>())
		component->downcast<MeshComponent>()->addMeshChangeListener(std::bind(&MeshRenderComponent::onMeshChange, this));
}

void
MeshRenderComponent::onDetachComponent(GameComponentPtr& component) noexcept
{
	if (component->isInstanceOf<MeshComponent>())
		component->downcast<MeshComponent>()->removeMeshChangeListener(std::bind(&MeshRenderComponent::onMeshChange, this));
}

void
MeshRenderComponent::onMoveAfter() noexcept
{
	for (auto& it : _renderObjects)
	{
		it->setTransform(
			this->getGameObject()->getTransform(),
			this->getGameObject()->getTransformInverse(),
			this->getGameObject()->getTransformInverseTranspose()
			);
	}
}

void
MeshRenderComponent::onLayerChangeAfter() noexcept
{
	for (auto& it : _renderObjects)
	{
		it->setLayer(this->getGameObject()->getLayer());
	}
}

void
MeshRenderComponent::onActivate() except
{
	if (!_material)
		_buildMaterials();

	if (_renderObjects.empty())
	{
		auto component = this->getGameObject()->getComponent<MeshComponent>();
		if (!component)
			return;

		auto mesh = component->getMesh();
		if (!mesh)
			return;

		_buildRenderObjects(mesh);
	}

	_attacRenderObjects();
}

void
MeshRenderComponent::onDeactivate() noexcept
{
	this->_dettachRenderhObjects();
}

void
MeshRenderComponent::onMeshChange() except
{
	this->_dettachRenderhObjects();

	if (this->getGameObject()->getActive())
	{
		auto component = this->getGameObject()->getComponent<MeshComponent>();
		if (component)
		{
			auto mesh = component->getMesh();
			if (!mesh)
				return;

			_buildMaterials();
			_buildRenderObjects(mesh);
			_attacRenderObjects();
		}
	}
}

bool
MeshRenderComponent::_buildMaterials() except
{
	if (this->hasMaterial())
		return true;

	if (this->getName().empty())
		return true;

	auto material = RenderSystem::instance()->createMaterial(this->getName());
	if (material)
	{
		this->setMaterial(material);
		this->setSharedMaterial(material);
		return true;
	}

	return false;
}

bool
MeshRenderComponent::_buildRenderObjects(MeshPropertyPtr mesh) noexcept
{
	auto material = this->getMaterial();
	if (!material)
		return false;

	_renderObjects.clear();

	MeshPropertys meshes;
	meshes.push_back(mesh);
	meshes.insert(meshes.end(), mesh->getChildren().begin(), mesh->getChildren().end());

	auto flags = ModelMakerFlagBits::ModelMakerFlagBitVertex | ModelMakerFlagBits::ModelMakerFlagBitNormal | ModelMakerFlagBits::ModelMakerFlagBitTexcoord | ModelMakerFlagBits::ModelMakerFlagBitFace;
	auto renderMeshes = RenderSystem::instance()->createRenderMesh(meshes, flags);

	std::size_t startVertice = 0;
	std::size_t startIndice = 0;

	for (auto& it : meshes)
	{
		auto renderObject = std::make_shared<Geometry>();

		if (this->_buildRenderObject(renderObject, it, renderMeshes))
		{
			auto renderable = renderObject->getGraphicsIndirect();

			renderable->startVertice = startVertice;
			renderable->startIndice = startIndice;
			renderable->numVertices = it->getNumVertices();
			renderable->numIndices = it->getNumIndices();

			startVertice += it->getNumVertices();
			startIndice += it->getNumIndices();

			_renderObjects.push_back(renderObject);
		}
	};

	return true;
}

bool
MeshRenderComponent::_buildRenderObject(GeometryPtr renderObject, MeshPropertyPtr mesh, RenderMeshPtr buffer) noexcept
{
	renderObject->setRenderMesh(buffer);
	renderObject->setBoundingBox(mesh->getBoundingBox());
	renderObject->setOwnerListener(this);

	renderObject->setMaterial(this->getMaterial());

	renderObject->setCastShadow(this->getCastShadow());
	renderObject->setReceiveShadow(this->getReceiveShadow());

	renderObject->setLayer(this->getGameObject()->getLayer());

	renderObject->setTransform(
		this->getGameObject()->getTransform(),
		this->getGameObject()->getTransformInverse(),
		this->getGameObject()->getTransformInverseTranspose()
		);

	auto renderable = std::make_shared<GraphicsIndirect>();
	renderable->startVertice = 0;
	renderable->startIndice = 0;
	renderable->numVertices = mesh->getNumVertices();
	renderable->numIndices = mesh->getNumIndices();
	renderable->numInstances = 0;
	renderable->indexType = GraphicsIndexType::GraphicsIndexTypeUInt32;

	renderObject->setGraphicsIndirect(renderable);

	return true;
}

_NAME_END

#endif