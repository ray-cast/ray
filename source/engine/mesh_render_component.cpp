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

#include <ray/res_manager.h>

_NAME_BEGIN

__ImplementSubClass(MeshRenderComponent, GameComponent, "MeshRender")

MeshRenderComponent::MeshRenderComponent() noexcept
	: _flags(ModelMakerFlagBits::ModelMakerFlagBit_VER_TEX_NORMAL)
	, _defaultMaterial("sys:fx/opacity.fxml.o")
{
}

MeshRenderComponent::~MeshRenderComponent() noexcept
{
	this->_cleanupMaterials();
	this->_cleanupRenderhObjects();
}

void
MeshRenderComponent::setMaterial(MaterialPtr material) noexcept
{
	if (_materials.empty())
		_materials.push_back(material);
	else
		_materials[0] = material;

	this->_updateMaterial(0);
}

void
MeshRenderComponent::setSharedMaterial(MaterialPtr material) noexcept
{
	if (_sharedMaterials.empty())
		_sharedMaterials.push_back(material);
	else
		_sharedMaterials[0] = material;

	this->_updateMaterial(0);
}

MaterialPtr
MeshRenderComponent::getMaterial() const noexcept
{
	return _materials.empty() ? nullptr : _materials.front();
}

MaterialPtr
MeshRenderComponent::getSharedMaterial() const noexcept
{
	return _sharedMaterials.empty() ? nullptr : _sharedMaterials.front();
}

void
MeshRenderComponent::setMaterials(const Materials& materials) noexcept
{
	_materials = materials;
	this->_updateMaterials();
}

void
MeshRenderComponent::setSharedMaterials(const Materials& materials) noexcept
{
	_sharedMaterials = materials;
	this->_updateMaterials();
}

const Materials&
MeshRenderComponent::getMaterials() const noexcept
{
	return _materials;
}

const Materials&
MeshRenderComponent::getSharedMaterials() const noexcept
{
	return _sharedMaterials;
}

bool
MeshRenderComponent::hasMaterial() const noexcept
{
	return !_materials.empty();
}

bool
MeshRenderComponent::hasSharedMaterial() const noexcept
{
	return !_sharedMaterials.empty();
}

void
MeshRenderComponent::load(iarchive& reader) noexcept
{
	RenderComponent::load(reader);
	reader.getValue("material", _material);
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
	result->setActive(this->getActive());
	result->setCastShadow(this->getCastShadow());
	result->setReceiveShadow(this->getReceiveShadow());
	result->setSharedMaterials(this->getSharedMaterials());
	result->_setDefaultMaterial(this->_getDefaultMaterial());

	if (this->hasSharedMaterial())
	{
		result->setMaterial(this->getSharedMaterial()->clone());
	}

	return result;
}

void
MeshRenderComponent::onAttachComponent(GameComponentPtr& component) noexcept
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
	auto component = this->getGameObject()->getComponent<MeshComponent>();
	if (!component)
		return;

	auto mesh = component->getMesh();
	if (!mesh)
		return;

	if (!this->hasMaterial())
	{
		if (!_material.empty())
			_buildMaterials(_material);
		else
			_buildDefaultMaterials(_defaultMaterial);
	}

	_buildRenderObjects(mesh, _flags);
	_attacRenderObjects();
}

void
MeshRenderComponent::onDeactivate() noexcept
{
	this->_cleanupMaterials();
	this->_cleanupRenderhObjects();
}

void
MeshRenderComponent::onMeshChange() except
{
	if (!this->getGameObject()->getActive())
		return;

	auto component = this->getGameObject()->getComponent<MeshComponent>();
	if (component)
	{
		auto mesh = component->getMesh();
		if (!mesh)
		{
			this->_cleanupRenderhObjects();
			return;
		}

		_buildRenderObjects(mesh, _flags);
		_attacRenderObjects();
	}
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
	{
		if (it->getRenderMesh() && it->getMaterial())
			it->setRenderScene(renderScene);
	}		
}

void
MeshRenderComponent::_cleanupRenderhObjects() noexcept
{
	for (auto& it : _renderObjects)
		it->setRenderScene(nullptr);

	_renderObjects.clear();
	_renderMesh.reset();
}

void
MeshRenderComponent::_cleanupMaterials() noexcept
{
	_materials.clear();
	_sharedMaterials.clear();
}

bool
MeshRenderComponent::_buildMaterials(const std::string& filename) noexcept
{
	assert(!filename.empty());

	auto material = RenderSystem::instance()->createMaterial(filename);
	if (material)
	{
		this->setMaterial(material);
		this->setSharedMaterial(material);
		return true;
	}

	return false;
}

bool 
MeshRenderComponent::_buildDefaultMaterials(const std::string& filename) noexcept
{
	assert(!filename.empty());

	auto component = this->getGameObject()->getComponent<MeshComponent>();
	if (!component)
		return false;

	auto model = ResManager::instance()->find<Model>(component->getName());
	if (!model)
		return false;

	auto& materials = model->getMaterialsList();
	if (materials.empty())
		return false;

	for (auto& material : materials)
	{
		float3 specular(0.5f);
		float3 diffuseColor(1.0f);
		float shininess(0.5);
		std::string diffuseTexture;
		
		if (!material->get(MATKEY_TEXTURE_DIFFUSE(0), diffuseTexture))
		{
			material->get(MATKEY_COLOR_DIFFUSE, diffuseColor);
		}
		
		material->get(MATKEY_COLOR_SPECULAR, specular);
		material->get(MATKEY_SHININESS, shininess);

		MaterialPtr effect;
		if (diffuseTexture.empty())
		{
			effect = RenderSystem::instance()->createMaterial(filename);
			if (effect)
			{
				auto luminance = [](float3 rgb)
				{
					const float3 lumfact = float3(0.2126f, 0.7152f, 0.0722f);
					return math::dot(rgb, lumfact);
				};

				effect->getParameter("diffuse")->uniform3f(diffuseColor);
				effect->getParameter("specular")->uniform1f(luminance(specular));
				effect->getParameter("shininess")->uniform1f(shininess);

				_materials.push_back(effect);
			}
		}
		else
		{
			effect = RenderSystem::instance()->createMaterial("sys:fx/opacity_diff_only.fxml.o");
			if (effect)
			{
				auto luminance = [](float3 rgb)
				{
					const float3 lumfact = float3(0.2126f, 0.7152f, 0.0722f);
					return math::dot(rgb, lumfact);
				};

				auto texture = RenderSystem::instance()->createTexture(model->getDirectory() + diffuseTexture, GraphicsTextureDim::GraphicsTextureDim2D);
				if (texture)
				{
					effect->getParameter("diffuse")->uniformTexture(texture);
					effect->getParameter("specular")->uniform1f(luminance(specular));
					effect->getParameter("shininess")->uniform1f(shininess);

					_materials.push_back(effect);
				}
				else 
				{
					_materials.push_back(nullptr);
				}
			}
		}
	}

	return true;
}

bool
MeshRenderComponent::_buildRenderObjects(MeshPropertyPtr mesh, ModelMakerFlags flags) noexcept
{
	assert(mesh);

	_cleanupRenderhObjects();

	_renderMesh = RenderSystem::instance()->createRenderMesh(*mesh, flags);
	if (!_renderMesh)
		return false;

	std::size_t startVertice = 0;
	std::size_t startIndice = 0;

	if (!_buildRenderObject(mesh, startVertice, startIndice))
		return false;

	auto& meshes = mesh->getChildren();
	for (auto& it : meshes)
	{
		if (!_buildRenderObject(it, startVertice, startIndice))
			return false;
	};

	_updateMaterials();
	return true;
}

bool
MeshRenderComponent::_buildRenderObject(MeshPropertyPtr mesh, std::size_t& startVertice, std::size_t& startIndice) noexcept
{
	auto renderObject = std::make_shared<Geometry>();

	if (this->_buildRenderObject(renderObject, mesh, _renderMesh))
	{
		auto renderable = renderObject->getGraphicsIndirect();

		renderable->startVertice = startVertice;
		renderable->startIndice = startIndice;
		renderable->numVertices = mesh->getNumVertices();
		renderable->numIndices = mesh->getNumIndices();

		startVertice += mesh->getNumVertices();
		startIndice += mesh->getNumIndices();

		_renderObjects.push_back(renderObject);
		return true;
	}

	return false;
}

bool
MeshRenderComponent::_buildRenderObject(GeometryPtr renderObject, MeshPropertyPtr mesh, RenderMeshPtr buffer) noexcept
{
	renderObject->setRenderMesh(buffer);
	renderObject->setBoundingBox(mesh->getBoundingBox());
	renderObject->setOwnerListener(this);

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

void 
MeshRenderComponent::_updateMaterial(std::size_t n) noexcept
{
	if (_renderObjects.size() <= n)
		return;
	
	if (_renderObjects.size() == _materials.size())
	{
		_renderObjects[n]->setMaterial(_materials[n]);
		return;
	}

	this->_updateMaterials();
}

void
MeshRenderComponent::_updateMaterials() noexcept
{
	std::size_t objectCount = _renderObjects.size();
	for (std::size_t i = 0; i < objectCount; i++)
	{
		if (_materials.empty())
			_renderObjects[i]->setMaterial(nullptr);
		else
		{
			if (_materials.size() > i)
				_renderObjects[i]->setMaterial(_materials[i] ? _materials[i] : _materials[0]);
			else
				_renderObjects[i]->setMaterial(_materials[0]);
		}
	}
}

void
MeshRenderComponent::_setModelMakerFlags(ModelMakerFlags flags) noexcept
{
	_flags = flags;
}

ModelMakerFlags 
MeshRenderComponent::_getModelMakerFlags() const noexcept
{
	return _flags;
}

void 
MeshRenderComponent::_setDefaultMaterial(const std::string& material) noexcept
{
	_defaultMaterial = material;
}

const std::string&
MeshRenderComponent::_getDefaultMaterial() const noexcept
{
	return _defaultMaterial;
}

_NAME_END

#endif