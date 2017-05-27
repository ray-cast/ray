// +----------------------------------------------------------------------
// | Project : ray.
// | All rights reserved.
// +----------------------------------------------------------------------
// | Copyright (c) 2013-2017.
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

MeshRenderComponent::MeshRenderComponent(const MaterialPtr& material, bool shared) noexcept
{
	if (shared)
		this->setSharedMaterial(material);
	else
		this->setMaterial(material);
}

MeshRenderComponent::MeshRenderComponent(const MaterialPtr&& material, bool shared) noexcept
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

MeshRenderComponent::MeshRenderComponent(const archivebuf& reader) noexcept
	: MeshRenderComponent()
{
	this->load(reader);
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
MeshRenderComponent::setMaterial(const MaterialPtr& material) noexcept
{
	auto _this = _materials.empty() ? nullptr : _materials.front().get();
	if (_this != material.get())
	{
		_materials.clear();
		_materials.push_back(material);

		this->_updateMaterials();
	}
}

void
MeshRenderComponent::setSharedMaterial(const MaterialPtr& material) noexcept
{
	auto _this = _sharedMaterials.empty() ? nullptr : _sharedMaterials.front().get();
	if (_this != material.get())
	{
		_sharedMaterials.clear();
		_sharedMaterials.push_back(material);

		this->_updateMaterials();
	}
}

void
MeshRenderComponent::setMaterial(MaterialPtr&& material) noexcept
{
	auto _this = _materials.empty() ? nullptr : _materials.front().get();
	if (_this != material.get())
	{
		_materials.clear();
		_materials.push_back(std::move(material));

		this->_updateMaterials();
	}
}

void
MeshRenderComponent::setSharedMaterial(MaterialPtr&& material) noexcept
{
	auto _this = _sharedMaterials.empty() ? nullptr : _sharedMaterials.front().get();
	if (_this != material.get())
	{
		_sharedMaterials.clear();
		_sharedMaterials.push_back(std::move(material));

		this->_updateMaterials();
	}
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
MeshRenderComponent::load(const archivebuf& reader) noexcept
{
	RenderComponent::load(reader);

	reader["material"] >> _material;
	reader["castshadow"] >> _isCastShadow;
	reader["receiveshadow"] >> _isReceiveShadow;
}

void
MeshRenderComponent::save(archivebuf& write) noexcept
{
	RenderComponent::save(write);

	write["material"] << _material;
	write["castshadow"] << _isCastShadow;
	write["receiveshadow"] << _isReceiveShadow;
}

GameComponentPtr
MeshRenderComponent::clone() const noexcept
{
	auto result = std::make_shared<MeshRenderComponent>();
	result->setName(this->getName());
	result->setActive(this->getActive());
	result->setCastShadow(this->getCastShadow());
	result->setReceiveShadow(this->getReceiveShadow());
	result->setSharedMaterials(this->getMaterials());
	result->_material = this->_material;
	result->_renderMeshVbo = this->_renderMeshVbo;
	result->_renderMeshIbo = this->_renderMeshIbo;

	for (auto& material : this->getMaterials())
		result->_materials.push_back(material->clone());

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
MeshRenderComponent::onActivate() except
{
	this->addComponentDispatch(GameDispatchType::GameDispatchTypeMoveAfter, this);
	this->onMeshChange();
}

void
MeshRenderComponent::onDeactivate() noexcept
{
	this->removeComponentDispatch(GameDispatchType::GameDispatchTypeMoveAfter, this);

	this->_destroyMaterials();
	this->_destroyRenderhObjects();
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
MeshRenderComponent::onMeshChange() except
{
	if (!this->getActive())
		return;

	auto component = this->getGameObject()->getComponent<MeshComponent>();
	if (!component)
	{
		this->_destroyRenderhObjects();
		return;
	}

	auto mesh = component->getMesh();
	if (!mesh)
	{
		this->_destroyRenderhObjects();
		return;
	}

	if (!this->hasMaterial() && !this->hasSharedMaterial())
		return;

	_buildRenderObjects(*mesh, ModelMakerFlagBits::ModelMakerFlagBitALL);
	_attacRenderObjects();
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
MeshRenderComponent::_buildMaterials(const util::string& filename) noexcept
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
MeshRenderComponent::_buildRenderObjects(const MeshProperty& mesh, ModelMakerFlags flags) noexcept
{
	if (!_renderObjects.empty())
	{
		for (auto& it : _renderObjects)
			it->setRenderScene(nullptr);

		_renderObjects.clear();
	}

	if (mesh.getNumVertices())
	{
		if (!_renderMeshVbo)
			_renderMeshVbo = ResManager::instance()->createVertexBuffer(mesh, flags);

		if (!_renderMeshVbo)
			return false;
	}

	if (mesh.getNumIndices())
	{
		if (!_renderMeshIbo)
			_renderMeshIbo = ResManager::instance()->createIndexBuffer(mesh);

		if (!_renderMeshIbo)
			return false;
	}

	auto& meshes = mesh.getMeshSubsets();
	for (auto& it : meshes)
	{
		auto renderObject = std::make_shared<Geometry>();
		renderObject->setVertexBuffer(_renderMeshVbo, it.offsetVertices);
		renderObject->setIndexBuffer(_renderMeshIbo, it.offsetIndices, GraphicsIndexType::GraphicsIndexTypeUInt32);
		renderObject->setBoundingBox(mesh.getBoundingBox());
		renderObject->setOwnerListener(this);
		renderObject->setCastShadow(this->getCastShadow());
		renderObject->setReceiveShadow(this->getReceiveShadow());
		renderObject->setLayer(this->getGameObject()->getLayer());
		renderObject->setTransform(this->getGameObject()->getWorldTransform());
		renderObject->setGraphicsIndirect(std::make_shared<GraphicsIndirect>(it.indicesCount * 3, it.indicesCount, 1, it.startVertices, it.startIndices));

		_renderObjects.push_back(renderObject);
	};

	_updateMaterials();
	return true;
}

void
MeshRenderComponent::_updateMaterial(std::size_t n) noexcept
{
	assert(n < _renderObjects.size());

	if (_materials.empty() && _sharedMaterials.empty())
		_renderObjects[n]->setMaterial(nullptr);
	else
	{
		if (!_materials.empty())
		{
			if (_materials.size() > n)
				_renderObjects[n]->setMaterial(_materials[n] ? _materials[n] : _materials[0]);
			else
				_renderObjects[n]->setMaterial(_materials[0]);
		}
		else
		{
			if (_sharedMaterials.size() > n)
				_renderObjects[n]->setMaterial(_sharedMaterials[n] ? _sharedMaterials[n] : _sharedMaterials[0]);
			else
				_renderObjects[n]->setMaterial(_sharedMaterials[0]);
		}
	}
}

void
MeshRenderComponent::_updateMaterials() noexcept
{
	std::size_t objectCount = _renderObjects.size();
	for (std::size_t i = 0; i < objectCount; i++)
		this->_updateMaterial(i);
}

_NAME_END