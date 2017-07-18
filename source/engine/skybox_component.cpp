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
#include <ray/skybox_component.h>
#include <ray/game_object.h>
#include <ray/render_system.h>
#include <ray/geometry.h>
#include <ray/graphics_context.h>
#include <ray/material.h>
#include <ray/res_manager.h>
#include <ray/graphics_texture.h>

_NAME_BEGIN

__ImplementSubClass(SkyboxComponent, MeshRenderComponent, "Skybox")

SkyboxComponent::SkyboxComponent() noexcept
	: _enableSkyBox(true)
	, _enableSkyLighting(true)
	, _skyDiffuseIntensity(1.0f)
	, _skySpecularIntensity(1.0f)
{
	this->setCastShadow(false);
	this->setReceiveShadow(false);
}

SkyboxComponent::SkyboxComponent(const archivebuf& reader) noexcept
	: SkyboxComponent()
{
	this->load(reader);
}

SkyboxComponent::~SkyboxComponent() noexcept
{
}

void
SkyboxComponent::setSkyBox(GraphicsTexturePtr texture) noexcept
{
	assert(!texture || texture->getGraphicsTextureDesc().getTexDim() == ray::GraphicsTextureDim::GraphicsTextureDim2D);

	if (_skyTexture != texture)
	{
		_skyTexture = texture;
		_onSkyBoxChange.run();
	}
}

GraphicsTexturePtr
SkyboxComponent::getSkyBox() const noexcept
{
	return _skyTexture;
}

void
SkyboxComponent::setSkyLightDiffuse(GraphicsTexturePtr texture) noexcept
{
	assert(!texture || texture->getGraphicsTextureDesc().getTexDim() == ray::GraphicsTextureDim::GraphicsTextureDimCube);

	if (_skyDiffTexture != texture)
	{
		_skyDiffTexture = texture;
		_onSkyLightingDiffuseChange.run();
	}
}

GraphicsTexturePtr
SkyboxComponent::getSkyLightDiffuse() const noexcept
{
	return _skyDiffTexture;
}

void
SkyboxComponent::setSkyLightSpecular(GraphicsTexturePtr texture) noexcept
{
	assert(!texture || texture->getGraphicsTextureDesc().getTexDim() == ray::GraphicsTextureDim::GraphicsTextureDimCube);

	if (_skySpecTexture != texture)
	{
		_skySpecTexture = texture;
		_onSkyLightingSpecularChange.run();
	}
}

GraphicsTexturePtr
SkyboxComponent::getSkyLightSpecular() const noexcept
{
	return _skySpecTexture;
}

bool
SkyboxComponent::loadSkybox(const std::string& path, bool cache) noexcept
{
	ray::GraphicsTexturePtr skybox;
	if (ray::ResManager::instance()->createTexture(path, skybox, ray::GraphicsTextureDim::GraphicsTextureDim2D, ray::GraphicsSamplerFilter::GraphicsSamplerFilterLinearMipmapLinear, ray::GraphicsSamplerWrap::GraphicsSamplerWrapClampToEdge, cache))
	{
		this->setSkyBox(skybox);
		return true;
	}

	return false;
}

bool
SkyboxComponent::loadSkyDiffuse(const std::string& path, bool cache) noexcept
{
	ray::GraphicsTexturePtr skydiffuse;
	if (ray::ResManager::instance()->createTexture(path, skydiffuse, ray::GraphicsTextureDim::GraphicsTextureDimCube, ray::GraphicsSamplerFilter::GraphicsSamplerFilterLinearMipmapLinear, ray::GraphicsSamplerWrap::GraphicsSamplerWrapClampToEdge, cache))
	{
		this->setSkyLightDiffuse(skydiffuse);
		return true;
	}

	return false;
}

bool
SkyboxComponent::loadSkySpecular(const std::string& path, bool cache) noexcept
{
	ray::GraphicsTexturePtr skyspecular;
	if (ray::ResManager::instance()->createTexture(path, skyspecular, ray::GraphicsTextureDim::GraphicsTextureDimCube, ray::GraphicsSamplerFilter::GraphicsSamplerFilterLinearMipmapLinear, ray::GraphicsSamplerWrap::GraphicsSamplerWrapClampToEdge, cache))
	{
		this->setSkyLightSpecular(skyspecular);
		return true;
	}

	return false;
}

void
SkyboxComponent::setSkyboxEnable(bool enable) noexcept
{
	_enableSkyBox = enable;
}

bool
SkyboxComponent::getSkyboxEnable() const noexcept
{
	return _enableSkyBox;
}

void
SkyboxComponent::setSkyLightingEnable(bool enable) noexcept
{
	_enableSkyLighting = enable;
}

bool
SkyboxComponent::getSkyLightingEnable() const noexcept
{
	return _enableSkyLighting;
}

void
SkyboxComponent::setSkyDiffuseIntensity(float intensity) noexcept
{
	_skyDiffuseIntensity = intensity;
	_updateMaterial();
}

float
SkyboxComponent::getSkyDiffuseIntensity() const noexcept
{
	return _skyDiffuseIntensity;
}

void
SkyboxComponent::setSkySpecularIntensity(float intensity) noexcept
{
	_skySpecularIntensity = intensity;
	_updateMaterial();
}

float
SkyboxComponent::getSkySpecularIntensity() const noexcept
{
	return _skySpecularIntensity;
}

void
SkyboxComponent::addSkyBoxChangeListener(std::function<void()>* func) noexcept
{
	assert(!_onSkyBoxChange.find(func));
	_onSkyBoxChange.attach(func);
}

void
SkyboxComponent::addSkyLightingDiffuseChangeListener(std::function<void()>* func) noexcept
{
	assert(!_onSkyLightingDiffuseChange.find(func));
	_onSkyLightingDiffuseChange.attach(func);
}

void
SkyboxComponent::addSkyLightingSpecularChangeListener(std::function<void()>* func) noexcept
{
	assert(!_onSkyLightingSpecularChange.find(func));
	_onSkyLightingSpecularChange.attach(func);
}

void
SkyboxComponent::addEnableSkyBoxListener(std::function<void(bool)>* func) noexcept
{
	assert(!_onEnableSkyBox.find(func));
	_onEnableSkyBox.attach(func);
}

void
SkyboxComponent::addEnableSkyLightingListener(std::function<void(bool)>* func) noexcept
{
	assert(!_onEnableSkyLighting.find(func));
	_onEnableSkyLighting.attach(func);
}

void
SkyboxComponent::removeSkyBoxChangeListener(std::function<void()>* func) noexcept
{
	assert(_onSkyBoxChange.find(func));
	_onSkyBoxChange.remove(func);
}

void
SkyboxComponent::removeSkyLightingDiffuseChangeListener(std::function<void()>* func) noexcept
{
	assert(_onSkyLightingDiffuseChange.find(func));
	_onSkyLightingDiffuseChange.remove(func);
}

void
SkyboxComponent::removeSkyLightingSpecularChangeListener(std::function<void()>* func) noexcept
{
	assert(_onSkyLightingSpecularChange.find(func));
	_onSkyLightingSpecularChange.remove(func);
}

void
SkyboxComponent::removeEnableSkyBoxListener(std::function<void(bool)>* func) noexcept
{
	assert(_onEnableSkyBox.find(func));
	_onEnableSkyBox.remove(func);
}

void
SkyboxComponent::removeEnableSkyLightingListener(std::function<void(bool)>* func) noexcept
{
	assert(_onEnableSkyLighting.find(func));
	_onEnableSkyLighting.remove(func);
}

void
SkyboxComponent::load(const archivebuf& reader) noexcept
{
	GameComponent::load(reader);

	reader["skymap"] >> _skyMap;
	reader["skydiffuse"] >> _skyDiffuse;
	reader["skyspecular"] >> _skySpecular;
	reader["skyDiffuse"] >> _skyDiffuseIntensity;
	reader["skySpeculars"] >> _skySpecularIntensity;

	const auto& flags = reader["flags"];
	if (flags.is_string())
	{
		std::vector<util::string> args;
		util::split(args, flags.get<archive::string_t>(), "|");

		for (auto& flag : args)
		{
			flag = util::trim(flag, ' ');

			if (flag == "skybox")
				_enableSkyBox = true;
			else if (flag == "skylighting")
				_enableSkyLighting = true;
		}
	}
}

void
SkyboxComponent::save(archivebuf& write) noexcept
{
	GameComponent::save(write);

	write["skymap"] << _skyMap;
	write["skydiffuse"] << _skyDiffuse;
	write["skyspecular"] << _skySpecular;
	write["skyDiffuse"] << _skyDiffuseIntensity;
	write["skySpeculars"] << _skySpecularIntensity;
}

GameComponentPtr
SkyboxComponent::clone() const noexcept
{
	auto sky = std::make_shared<SkyboxComponent>();
	sky->setName(this->getName());
	sky->setMaterial(this->getMaterial());
	sky->setSharedMaterial(this->getMaterial());
	return sky;
}

void
SkyboxComponent::onActivate() noexcept
{
	if (_enableSkyBox || _enableSkyLighting)
		this->_setupMaterial();

	if (_enableSkyBox)
		this->_setupSkybox();

	if (_enableSkyLighting)
		this->_setupSkyLighting();

	if (_enableSkyBox || _enableSkyLighting)
	{
		_updateMaterial();
		_updateTransform();

		this->_attacRenderObjects();
	}

	this->addComponentDispatch(GameDispatchType::GameDispatchTypeMoveAfter, this);
}

void
SkyboxComponent::onDeactivate() noexcept
{
	this->removeComponentDispatch(GameDispatchType::GameDispatchTypeMoveAfter, this);

	this->_destroyMaterial();
	this->_destroySkybox();
}

void
SkyboxComponent::onMoveAfter() noexcept
{
	_updateTransform();
}

bool
SkyboxComponent::_loadSkybox(const util::string& texture) noexcept
{
	if (texture.empty())
		return false;

	GraphicsTexturePtr skyTexture;
	if (!ResManager::instance()->createTexture(texture, skyTexture, GraphicsTextureDim::GraphicsTextureDim2D, GraphicsSamplerFilter::GraphicsSamplerFilterLinear))
		return false;

	this->setSkyBox(skyTexture);
	return true;
}

bool
SkyboxComponent::_loadSkyDiffuse(const util::string& texture) noexcept
{
	GraphicsTexturePtr skyDiffTexture;

	if (!ResManager::instance()->createTexture(texture, skyDiffTexture,
		GraphicsTextureDim::GraphicsTextureDimCube,
		GraphicsSamplerFilter::GraphicsSamplerFilterLinear,
		GraphicsSamplerWrap::GraphicsSamplerWrapClampToEdge,
		false))
	{
		return false;
	}

	this->setSkyLightDiffuse(skyDiffTexture);
	return true;
}

bool
SkyboxComponent::_loadSkySpecular(const util::string& texture) noexcept
{
	GraphicsTexturePtr skySpecTexture;

	if (!ResManager::instance()->createTexture(texture, skySpecTexture,
		GraphicsTextureDim::GraphicsTextureDimCube,
		GraphicsSamplerFilter::GraphicsSamplerFilterLinearMipmapLinear,
		GraphicsSamplerWrap::GraphicsSamplerWrapClampToEdge,
		false))
	{
		return false;
	}

	this->setSkyLightSpecular(skySpecTexture);
	return true;
}

bool
SkyboxComponent::_buildSphereMesh(MeshProperty& mesh) noexcept
{
	mesh.makeSphere(1, 16, 12);
	return true;
}

bool
SkyboxComponent::_buildSphereRenderMesh(const MeshProperty& mesh) noexcept
{
	_renderSphereVbo = ResManager::instance()->createVertexBuffer(mesh, ModelMakerFlagBits::ModelMakerFlagBitVertex);
	if (!_renderSphereVbo)
		return true;

	_renderSphereIbo = ResManager::instance()->createIndexBuffer(mesh);
	if (!_renderSphereIbo)
		return true;
	return false;
}

bool
SkyboxComponent::_buildSphereRenderObject(const MeshProperty& mesh, MaterialPtr technique) noexcept
{
	_sphereObject = std::make_shared<Geometry>();
	_sphereObject->setMaterial(technique);
	_sphereObject->setCastShadow(false);
	_sphereObject->setReceiveShadow(false);
	_sphereObject->setVertexBuffer(_renderSphereVbo, 0);
	_sphereObject->setIndexBuffer(_renderSphereIbo, 0, GraphicsIndexType::GraphicsIndexTypeUInt32);
	_sphereObject->setBoundingBox(mesh.getBoundingBox());
	_sphereObject->setOwnerListener(this);
	_sphereObject->setCastShadow(this->getCastShadow());
	_sphereObject->setReceiveShadow(this->getReceiveShadow());
	_sphereObject->setLayer(this->getGameObject()->getLayer());
	_sphereObject->setTransform(this->getGameObject()->getWorldTransform(), this->getGameObject()->getWorldTransformInverse());
	_sphereObject->setGraphicsIndirect(std::make_shared<GraphicsIndirect>(mesh.getNumVertices(), mesh.getNumIndices()));

	return true;
}

bool
SkyboxComponent::_setupMaterial() noexcept
{
	_skyBoxMaterial = RenderSystem::instance()->createMaterial("sys:fx/skybox.fxml");
	if (!_skyBoxMaterial)
		return false;

	return true;
}

bool
SkyboxComponent::_setupSkybox() noexcept
{
	if (!_skyTexture && !_skyMap.empty())
		_loadSkybox(_skyMap);

	if (_skyBoxMaterial)
	{
		MeshProperty sphereMesh;
		_buildSphereMesh(sphereMesh);
		_buildSphereRenderMesh(sphereMesh);
		_buildSphereRenderObject(sphereMesh, _skyBoxMaterial);

		this->_attacRenderObject(_sphereObject);
		return true;
	}

	return false;
}

bool
SkyboxComponent::_setupSkyLighting() noexcept
{
	if (!_skyDiffuse.empty() && !_skySpecular.empty())
	{
		if (_loadSkyDiffuse(_skyDiffuse) && _loadSkySpecular(_skySpecular))
		{
			return true;
		}
	}

	return false;
}

void
SkyboxComponent::_destroySkybox() noexcept
{
	if (_sphereObject)
	{
		this->_destroyRenderhObject(_sphereObject);
		_sphereObject.reset();
	}

	_renderSphereVbo.reset();
	_renderSphereIbo.reset();
}

void
SkyboxComponent::_destroyMaterial() noexcept
{
	this->setMaterial(nullptr);
}

void
SkyboxComponent::_reloadSkybox(const util::string& texture) noexcept
{
	if (!_loadSkybox(texture))
		_updateMaterial();
}

void
SkyboxComponent::_reloadSkyDiffuse(const util::string& texture) noexcept
{
	if (!_loadSkyDiffuse(texture))
		_updateMaterial();
}

void
SkyboxComponent::_reloadSkySpecular(const util::string& texture) noexcept
{
	if (!_loadSkySpecular(texture))
		_updateMaterial();
}

void
SkyboxComponent::_updateTransform() noexcept
{
	if (_sphereObject)
		_sphereObject->setTransform(this->getGameObject()->getWorldTransform(), this->getGameObject()->getWorldTransformInverse());
}

void
SkyboxComponent::_updateMaterial() noexcept
{
	if (_enableSkyBox && _skyBoxMaterial)
	{
		auto texSkybox = _skyBoxMaterial->getParameter("texSkybox");
		if (texSkybox)
			texSkybox->uniformTexture(_skyTexture);
	}
}

_NAME_END