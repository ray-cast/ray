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
#include <ray/mesh_render_component.h>
#include <ray/mesh_component.h>

#include <ray/render_feature.h>
#include <ray/render_system.h>
#include <ray/geometry.h>
#include <ray/material.h>

#include <ray/game_server.h>
#include <ray/graphics_context.h>

#include <ray/res_manager.h>

_NAME_BEGIN

__ImplementSubClass(MeshRenderComponent, RenderComponent, "MeshRender")

MeshRenderComponent::MeshRenderComponent() noexcept
	: _isCastShadow(true)
	, _isReceiveShadow(true)
	, _onMeshChange(std::bind(&MeshRenderComponent::onMeshChange, this))
{
}

MeshRenderComponent::MeshRenderComponent(MaterialPtr& material, bool shared) noexcept
{
	if (shared)
		this->setSharedMaterial(material);
	else
		this->setMaterial(material);
}

MeshRenderComponent::MeshRenderComponent(MaterialPtr&& material, bool shared) noexcept
{
	if (shared)
		this->setSharedMaterial(material);
	else
		this->setMaterial(material);
}

MeshRenderComponent::MeshRenderComponent(const Materials& materials, bool shared) noexcept
{
	if (shared)
		this->setSharedMaterials(materials);
	else
		this->setMaterials(materials);
}

MeshRenderComponent::MeshRenderComponent(Materials&& materials, bool shared) noexcept
{
	if (shared)
		this->setSharedMaterials(materials);
	else
		this->setMaterials(materials);
}

MeshRenderComponent::~MeshRenderComponent() noexcept
{
	this->_destroyMaterials();
	this->_destroyRenderhObjects();
}

void
MeshRenderComponent::setCastShadow(bool value) noexcept
{
	_isCastShadow = value;
}

bool
MeshRenderComponent::getCastShadow() const noexcept
{
	return _isCastShadow;
}

void
MeshRenderComponent::setReceiveShadow(bool value) noexcept
{
	_isReceiveShadow = value;
}

bool
MeshRenderComponent::getReceiveShadow() const noexcept
{
	return _isReceiveShadow;
}

void
MeshRenderComponent::setMaterial(MaterialPtr& material) noexcept
{
	if (_materials.empty())
		_materials.push_back(material);
	else
		_materials[0] = material;

	this->_updateMaterial(0);
}

void
MeshRenderComponent::setSharedMaterial(MaterialPtr& material) noexcept
{
	if (_sharedMaterials.empty())
		_sharedMaterials.push_back(material);
	else
		_sharedMaterials[0] = material;

	this->_updateMaterial(0);
}

void
MeshRenderComponent::setMaterial(MaterialPtr&& material) noexcept
{
	this->setMaterial(material);
}

void
MeshRenderComponent::setSharedMaterial(MaterialPtr&& material) noexcept
{
	this->setSharedMaterial(material);
}

MaterialPtr
MeshRenderComponent::getMaterial() const noexcept
{
	if (!_materials.empty())
		return _materials.front();
	if (!_sharedMaterials.empty())
		return _sharedMaterials.front();
	return nullptr;
}

MaterialPtr
MeshRenderComponent::getSharedMaterial() const noexcept
{
	if (!_sharedMaterials.empty())
		return _sharedMaterials.front();
	return nullptr;
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

void
MeshRenderComponent::setMaterials(Materials&& materials) noexcept
{
	_materials = std::move(materials);
	this->_updateMaterials();
}

void
MeshRenderComponent::setSharedMaterials(Materials&& materials) noexcept
{
	_sharedMaterials = std::move(materials);
	this->_updateMaterials();
}

const Materials&
MeshRenderComponent::getMaterials() const noexcept
{
	if (!_materials.empty())
		return _materials;
	else
		return _sharedMaterials;
}

const Materials&
MeshRenderComponent::getSharedMaterials() const noexcept
{
	return _sharedMaterials;
}

bool
MeshRenderComponent::hasMaterial() const noexcept
{
	return !_materials.empty() || !_sharedMaterials.empty();
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
	reader >> make_archive(_isCastShadow, "castshadow");
	reader >> make_archive(_isReceiveShadow, "receiveshadow");
}

void
MeshRenderComponent::save(oarchive& write) noexcept
{
	RenderComponent::save(write);
	write << make_archive(_isCastShadow, "castshadow");
	write << make_archive(_isReceiveShadow, "receiveshadow");
}

GameComponentPtr
MeshRenderComponent::clone() const noexcept
{
	auto result = std::make_shared<MeshRenderComponent>();
	result->setActive(this->getActive());
	result->setCastShadow(this->getCastShadow());
	result->setReceiveShadow(this->getReceiveShadow());
	result->setSharedMaterials(this->getMaterials());
	result->_material = this->_material;

	for (auto& material : this->_materials)
		result->_materials.push_back(material->clone());

	auto component = this->getGameObject()->getComponent<MeshComponent>();
	if (component)
	{
		if (component->getMesh() == component->getSharedMesh())
		{
			result->_renderMeshVbo = this->_renderMeshVbo;
			result->_renderMeshIbo = this->_renderMeshIbo;
		}
	}

	return result;
}

void
MeshRenderComponent::onAttachComponent(GameComponentPtr& component) noexcept
{
	if (component->isInstanceOf<MeshComponent>())
		component->downcast<MeshComponent>()->addMeshChangeListener(&_onMeshChange);
}

void
MeshRenderComponent::onDetachComponent(GameComponentPtr& component) noexcept
{
	if (component->isInstanceOf<MeshComponent>())
		component->downcast<MeshComponent>()->removeMeshChangeListener(&_onMeshChange);
}

void
MeshRenderComponent::onMoveAfter() noexcept
{
	for (auto& it : _renderObjects)
		it->setTransform(this->getGameObject()->getWorldTransform());
}

void
MeshRenderComponent::onLayerChangeAfter() noexcept
{
	for (auto& it : _renderObjects)
		it->setLayer(this->getGameObject()->getLayer());
}

void
MeshRenderComponent::onActivate() except
{
	this->addComponentDispatch(GameDispatchType::GameDispatchTypeMoveAfter, this);

	auto component = this->getGameObject()->getComponent<MeshComponent>();
	if (!component)
		return;

	auto mesh = component->getMesh();
	if (!mesh)
		return;

	if (!this->hasMaterial() && !this->hasSharedMaterial())
	{
		if (!_material.empty())
			_buildMaterials(_material);
		else
			_buildDefaultMaterials("sys:fx/opacity.fxml");
	}

	_buildRenderObjects(*mesh, ModelMakerFlagBits::ModelMakerFlagBitALL);
	_attacRenderObjects();
}

void
MeshRenderComponent::onDeactivate() noexcept
{
	this->removeComponentDispatch(GameDispatchType::GameDispatchTypeMoveAfter, this);

	this->_destroyMaterials();
	this->_destroyRenderhObjects();
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
			this->_destroyRenderhObjects();
			return;
		}

		if (!this->hasMaterial() && !this->hasSharedMaterial())
		{
			if (!_material.empty())
				_buildMaterials(_material);
			else
				_buildDefaultMaterials("sys:fx/opacity_skinning0.fxml");
		}

		_buildRenderObjects(*mesh, ModelMakerFlagBits::ModelMakerFlagBitALL);
		_attacRenderObjects();
	}
}

void
MeshRenderComponent::_attacRenderObject(GeometryPtr object) noexcept
{
	if (object)
		_renderObjects.push_back(object);
}

void
MeshRenderComponent::_attacRenderObjects() noexcept
{
	auto renderScene = GameServer::instance()->getFeature<RenderFeature>()->getRenderScene();

	for (auto& it : _renderObjects)
		it->setRenderScene(renderScene);
}

void
MeshRenderComponent::_destroyRenderhObject(GeometryPtr object) noexcept
{
	assert(object);

	auto it = std::find(_renderObjects.begin(), _renderObjects.end(), object);
	if (it != _renderObjects.end())
	{
		(*it)->setRenderScene(nullptr);
		_renderObjects.erase(it);
	}
}

void
MeshRenderComponent::_destroyRenderhObjects() noexcept
{
	for (auto& it : _renderObjects)
		it->setRenderScene(nullptr);

	_renderObjects.clear();
	_renderMeshVbo.reset();
	_renderMeshIbo.reset();
}

void
MeshRenderComponent::_destroyMaterials() noexcept
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
		float4 quality(0.0f);
		float shininess = 0.0;
		float opacity = 1.0;
		std::string diffuseTexture;

		material->get(MATKEY_OPACITY, opacity);
		material->get(MATKEY_SHININESS, shininess);
		material->get(MATKEY_COLOR_DIFFUSE, diffuseColor);
		material->get(MATKEY_COLOR_SPECULAR, specular);
		material->get(MATKEY_TEXTURE_DIFFUSE(0), diffuseTexture);

		MaterialPtr effect;
		effect = RenderSystem::instance()->createMaterial("sys:fx/opacity_skinning0.fxml");
		if (!effect)
			_sharedMaterials.push_back(nullptr);
		else
		{
			auto luminance = [](const float3& rgb)
			{
				const float3 lumfact = float3(0.2126f, 0.7152f, 0.0722f);
				return math::dot(rgb, lumfact);
			};

			auto texture = ResManager::instance()->createTexture(model->getDirectory() + diffuseTexture, GraphicsTextureDim::GraphicsTextureDim2D);
			if (texture)
			{
				quality.x = 1.0f;
				effect->getParameter("texDiffuse")->uniformTexture(texture);
			}

			effect->getParameter("quality")->uniform4f(quality);
			effect->getParameter("diffuse")->uniform3f(diffuseColor);
			effect->getParameter("specular")->uniform1f(luminance(specular));
			effect->getParameter("shininess")->uniform1f(shininess);

			_sharedMaterials.push_back(effect);
		}
	}

	return true;
}

bool
MeshRenderComponent::_buildRenderObjects(const MeshProperty& mesh, ModelMakerFlags flags) noexcept
{
	_destroyRenderhObjects();

	if (!_renderMeshVbo)
	{
		_renderMeshVbo = ResManager::instance()->createVertexBuffer(mesh, flags);
		if (!_renderMeshVbo)
			return false;

		_renderMeshIbo = ResManager::instance()->createIndexBuffer(mesh);
		if (!_renderMeshIbo)
			return false;
	}

	std::size_t startVertice = 0;
	std::size_t startIndice = 0;

	if (!_buildRenderObject(mesh, startVertice, startIndice))
		return false;

	auto& meshes = mesh.getChildren();
	for (auto& it : meshes)
	{
		if (!_buildRenderObject(*it, startVertice, startIndice))
			return false;
	};

	_updateMaterials();
	return true;
}

bool
MeshRenderComponent::_buildRenderObject(const MeshProperty& mesh, std::size_t& startVertice, std::size_t& startIndice) noexcept
{
	auto renderObject = std::make_shared<Geometry>();

	if (this->_buildRenderObject(renderObject, mesh, _renderMeshVbo, _renderMeshIbo))
	{
		auto renderable = renderObject->getGraphicsIndirect();

		renderable->startVertice = startVertice;
		renderable->startIndice = startIndice;
		renderable->numVertices = mesh.getNumVertices();
		renderable->numIndices = mesh.getNumIndices();

		startVertice += mesh.getNumVertices();
		startIndice += mesh.getNumIndices();

		_renderObjects.push_back(renderObject);
		return true;
	}

	return false;
}

bool
MeshRenderComponent::_buildRenderObject(GeometryPtr renderObject, const MeshProperty& mesh, GraphicsDataPtr vbo, GraphicsDataPtr ibo) noexcept
{
	renderObject->setVertexBuffer(vbo, 0);
	renderObject->setIndexBuffer(ibo, 0, GraphicsIndexType::GraphicsIndexTypeUInt32);

	renderObject->setBoundingBox(mesh.getBoundingBox());
	renderObject->setOwnerListener(this);

	renderObject->setCastShadow(this->getCastShadow());
	renderObject->setReceiveShadow(this->getReceiveShadow());

	renderObject->setLayer(this->getGameObject()->getLayer());

	renderObject->setTransform(this->getGameObject()->getWorldTransform());

	auto renderable = std::make_shared<GraphicsIndirect>();
	renderable->numVertices = mesh.getNumVertices();
	renderable->numIndices = mesh.getNumIndices();

	renderObject->setGraphicsIndirect(renderable);

	return true;
}

void
MeshRenderComponent::_updateMaterial(std::size_t n) noexcept
{
	if (_renderObjects.size() <= n)
		return;

	if (!_materials.empty())
	{
		if (_renderObjects.size() == _materials.size())
		{
			_renderObjects[n]->setMaterial(_materials[n]);
			return;
		}
	}
	else
	{
		if (_renderObjects.size() == _sharedMaterials.size())
		{
			_renderObjects[n]->setMaterial(_sharedMaterials[n]);
			return;
		}
	}

	this->_updateMaterials();
}

void
MeshRenderComponent::_updateMaterials() noexcept
{
	std::size_t objectCount = _renderObjects.size();
	for (std::size_t i = 0; i < objectCount; i++)
	{
		if (_materials.empty() && _sharedMaterials.empty())
			_renderObjects[i]->setMaterial(nullptr);
		else
		{
			if (!_materials.empty())
			{
				if (_materials.size() > i)
					_renderObjects[i]->setMaterial(_materials[i] ? _materials[i] : _materials[0]);
				else
					_renderObjects[i]->setMaterial(_materials[0]);
			}
			else
			{
				if (_sharedMaterials.size() > i)
					_renderObjects[i]->setMaterial(_sharedMaterials[i] ? _sharedMaterials[i] : _sharedMaterials[0]);
				else
					_renderObjects[i]->setMaterial(_sharedMaterials[0]);
			}
		}
	}
}

_NAME_END