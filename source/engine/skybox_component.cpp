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
	: _enableSkyBox(false)
	, _enableSkyLighting(false)
	, _skyboxSize(1000.0f)
	, _skyLightingIntensity(1.0f, 1.0f)
{
	this->setCastShadow(false);
	this->setReceiveShadow(false);
}

SkyboxComponent::~SkyboxComponent() noexcept
{
}

void
SkyboxComponent::setSkyboxSize(float size) noexcept
{
	_skyboxSize = size;
	_updateTransform();
}

float
SkyboxComponent::getSkyboxSize() const noexcept
{
	return _skyboxSize;
}

void
SkyboxComponent::setSkyBox(GraphicsTexturePtr texture) noexcept
{
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
SkyboxComponent::setSkyLightDiffuse(GraphicsTexturePtr diffuse) noexcept
{
	if (_skyDiffTexture != diffuse)
	{
		_skyDiffTexture = diffuse;
		_onSkyLightingDiffuseChange.run();
	}
}

GraphicsTexturePtr
SkyboxComponent::getSkyLightDiffuse() const noexcept
{
	return _skyDiffTexture;
}

void
SkyboxComponent::setSkyLightSpecular(GraphicsTexturePtr specular) noexcept
{
	if (_skySpecTexture != specular)
	{
		_skySpecTexture = specular;
		_onSkyLightingSpecularChange.run();
	}
}

GraphicsTexturePtr
SkyboxComponent::getSkyLightSpecular() const noexcept
{
	return _skySpecTexture;
}

void
SkyboxComponent::setSkyboxEnable(bool enable) noexcept
{
	if (_enableSkyBox != enable)
	{
		if (enable)
			this->_reloadSkybox(_skyDiffuse);
		else
			this->_destroySkybox();
		_enableSkyBox = enable;
	}
}

bool
SkyboxComponent::getSkyboxEnable() const noexcept
{
	return _enableSkyBox;
}

void
SkyboxComponent::setSkyLightingEnable(bool enable) noexcept
{
	if (_enableSkyLighting != enable)
	{
		if (enable)
		{
			this->_reloadSkyDiffuse(_skyDiffuse);
			this->_reloadSkySpecular(_skySpecular);
		}
		else
		{
			this->_destroySkyLighting();
		}

		_enableSkyLighting = enable;
	}
}

bool
SkyboxComponent::getSkyLightingEnable() const noexcept
{
	return _enableSkyLighting;
}

void
SkyboxComponent::setSkyDiffuseIntensity(float intensity) noexcept
{
	_skyLightingIntensity.x = intensity;
	_updateMaterial();
}

float
SkyboxComponent::getSkyDiffuseIntensity() const noexcept
{
	return _skyLightingIntensity.x;
}

void
SkyboxComponent::setSkySpecularIntensity(float intensity) noexcept
{
	_skyLightingIntensity.y = intensity;
	_updateMaterial();
}

float
SkyboxComponent::getSkySpecularIntensity() const noexcept
{
	return _skyLightingIntensity.y;
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
SkyboxComponent::load(iarchive& reader) noexcept
{
	GameComponent::load(reader);

	reader.getValue("skymap", _skyMap);
	reader.getValue("skydiffuse", _skyDiffuse);
	reader.getValue("skyspecular", _skySpecular);
	reader.getValue("skyIntensity", _skyLightingIntensity);
	reader.getValue("skysize", _skyboxSize);

	std::string flagsString;
	if (reader.getValue("flags", flagsString))
	{
		std::vector<std::string> args;
		util::split(args, flagsString, "|");

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
SkyboxComponent::save(oarchive& write) noexcept
{
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

bool
SkyboxComponent::_loadSkybox(const std::string& texture) noexcept
{
	if (texture.empty())
		return false;

	auto skyTexture = ResManager::instance()->createTexture(texture,
		GraphicsTextureDim::GraphicsTextureDim2D,
		GraphicsSamplerFilter::GraphicsSamplerFilterLinear);

	if (!skyTexture)
		return false;

	this->setSkyBox(skyTexture);
	return true;
}

bool
SkyboxComponent::_loadSkyDiffuse(const std::string& texture) noexcept
{
	auto skyDiffTexture = ResManager::instance()->createTexture(texture,
		GraphicsTextureDim::GraphicsTextureDimCube,
		GraphicsSamplerFilter::GraphicsSamplerFilterLinear,
		GraphicsSamplerWrap::GraphicsSamplerWrapClampToEdge);

	if (!skyDiffTexture)
		return false;

	this->setSkyLightDiffuse(skyDiffTexture);
	return true;
}

bool
SkyboxComponent::_loadSkySpecular(const std::string& texture) noexcept
{
	auto skySpecTexture = ResManager::instance()->createTexture(texture,
		GraphicsTextureDim::GraphicsTextureDimCube,
		GraphicsSamplerFilter::GraphicsSamplerFilterLinearMipmapLinear,
		GraphicsSamplerWrap::GraphicsSamplerWrapClampToEdge);

	if (!skySpecTexture)
		return false;

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
SkyboxComponent::_buildQuadMesh(MeshProperty& mesh) noexcept
{
	mesh.makePlane(2, 2, 1, 1);
	return true;
}

bool
SkyboxComponent::_buildQuadRenderMesh(const MeshProperty& mesh) noexcept
{
	_renderScreenQuadVbo = ResManager::instance()->createVertexBuffer(mesh, ModelMakerFlagBits::ModelMakerFlagBitVertex);
	if (!_renderScreenQuadVbo)
		return true;

	_renderScreenQuadIbo = ResManager::instance()->createIndexBuffer(mesh);
	if (!_renderScreenQuadIbo)
		return true;

	return false;
}

bool
SkyboxComponent::_buildQuadRenderObject(const MeshProperty& mesh, MaterialPtr technique) noexcept
{
	MeshProperty sphere;
	sphere.makeSphere(1, 16, 12);
	_quadObject = std::make_shared<Geometry>();
	_quadObject->setMaterial(technique);
	_quadObject->setCastShadow(false);
	_quadObject->setReceiveShadow(false);
	return _buildRenderObject(_quadObject, sphere, _renderScreenQuadVbo, _renderScreenQuadIbo);
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
	return _buildRenderObject(_sphereObject, mesh, _renderSphereVbo, _renderSphereIbo);
}

bool
SkyboxComponent::_setupMaterial() noexcept
{
	_skyBoxMaterial = RenderSystem::instance()->createMaterial("sys:fx/skydome.fxml");
	if (!_skyBoxMaterial)
		return false;

	return true;
}

bool
SkyboxComponent::_setupSkybox() noexcept
{
	if (_loadSkybox(_skyMap) && _skyBoxMaterial)
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
			MeshProperty quadMesh;
			_buildQuadMesh(quadMesh);
			_buildQuadRenderMesh(quadMesh);

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
SkyboxComponent::_destroySkyLighting() noexcept
{
	if (_quadObject)
	{
		this->_destroyRenderhObject(_quadObject);
		_quadObject.reset();
	}

	_renderScreenQuadVbo.reset();
	_renderScreenQuadIbo.reset();
}

void
SkyboxComponent::_destroyMaterial() noexcept
{
	this->setMaterial(nullptr);
}

void
SkyboxComponent::_reloadSkybox(const std::string& texture) noexcept
{
	if (!_loadSkybox(texture))
		_updateMaterial();
}

void
SkyboxComponent::_reloadSkyDiffuse(const std::string& texture) noexcept
{
	if (!_loadSkyDiffuse(texture))
		_updateMaterial();
}

void
SkyboxComponent::_reloadSkySpecular(const std::string& texture) noexcept
{
	if (!_loadSkySpecular(texture))
		_updateMaterial();
}

void
SkyboxComponent::_updateTransform() noexcept
{
	float4x4 transforam;
	transforam.makeScale(_skyboxSize, _skyboxSize, _skyboxSize);
	//transforam.setTranslate(this->getGameObject()->getWorldTranslate());

	if (_sphereObject)
		_sphereObject->setTransform(transforam);

	if (_quadObject)
		_quadObject->setTransform(transforam);
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

void
SkyboxComponent::onActivate() noexcept
{
	if (_enableSkyBox || _enableSkyLighting)
	{
		this->_setupMaterial();
	}

	if (_enableSkyBox)
	{
		this->_setupSkybox();
	}

	if (_enableSkyLighting)
	{
		this->_setupSkyLighting();
	}

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
	this->_destroySkyLighting();
}

void
SkyboxComponent::onMoveAfter() noexcept
{
	_updateTransform();
}

_NAME_END