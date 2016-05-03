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
#include <ray/skybox_component.h>
#include <ray/game_object.h>
#include <ray/render_system.h>
#include <ray/geometry.h>
#include <ray/graphics_context.h>
#include <ray/material.h>
#include <ray/graphics_texture.h>

_NAME_BEGIN

__ImplementSubClass(SkyboxComponent, MeshRenderComponent, "Skybox")

SkyboxComponent::SkyboxComponent() noexcept
	: _enableSkyBox(false)
	, _enableSkyLighting(false)
	, _needUpdateSkybox(true)
	, _needUpdateSkyDiffuse(true)
	, _needUpdateSkySpecular(true)
	, _skyboxSize(100.0f)
	, _skyLightingIntensity(1.0f, 1.0f)
{
}

SkyboxComponent::SkyboxComponent(MaterialPtr material) noexcept
{
	this->setMaterial(material);
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
SkyboxComponent::setSkyLightMap(const std::string& texture) noexcept
{
	if (_skyMap != texture)
	{
		_needUpdateSkybox = true;
		_skyMap = texture;
	}
}

const std::string&
SkyboxComponent::getSkyLightMap() const noexcept
{
	return _skyMap;
}

void
SkyboxComponent::setSkyLightDiffuse(const std::string& diffuse) noexcept
{
	if (_skyDiffuse != diffuse)
	{
		_needUpdateSkyDiffuse = true;
		_skyDiffuse = diffuse;
	}
}

const std::string&
SkyboxComponent::getSkyLightDiffuse() const noexcept
{
	return _skyDiffuse;
}

void
SkyboxComponent::setSkyLightSpecular(const std::string& specular) noexcept
{
	if (_skySpecular != specular)
	{
		_needUpdateSkySpecular = true;
		_skySpecular = specular;
	}
}

const std::string&
SkyboxComponent::getSkyLightSpecular() const noexcept
{
	return _skySpecular;
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
SkyboxComponent::setSkyLightingIntensity(const float2& intensity) noexcept
{
	_skyLightingIntensity = intensity;
	_updateMaterial();
}

const float2& 
SkyboxComponent::getSkyLightingIntensity() const noexcept
{
	return _skyLightingIntensity;
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

	auto skyTexture = RenderSystem::instance()->createTexture(texture,
		GraphicsTextureDim::GraphicsTextureDim2D,
		GraphicsSamplerFilter::GraphicsSamplerFilterNearest);

	if (!skyTexture)
		return false;

	_skyTexture = skyTexture;
	return true;
}

bool
SkyboxComponent::_loadSkyDiffuse(const std::string& texture) noexcept
{
	auto skyDiffTexture = RenderSystem::instance()->createTexture(texture,
		GraphicsTextureDim::GraphicsTextureDimCube,
		GraphicsSamplerFilter::GraphicsSamplerFilterLinear);

	if (!skyDiffTexture)
		return false;

	_skyDiffTexture = skyDiffTexture;
	return true;
}

bool
SkyboxComponent::_loadSkySpecular(const std::string& texture) noexcept
{
	auto skySpecTexture = RenderSystem::instance()->createTexture(texture,
		GraphicsTextureDim::GraphicsTextureDimCube,
		GraphicsSamplerFilter::GraphicsSamplerFilterLinearMipmapLinear);

	if (!skySpecTexture)
		return false;

	_skySpecTexture = skySpecTexture;
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
	_renderScreenQuad = RenderSystem::instance()->createRenderMesh(mesh, ModelMakerFlagBits::ModelMakerFlagBitVertex);
	if (!_renderScreenQuad)
		return true;
	return false;
}

bool 
SkyboxComponent::_buildQuadRenderObject(const MeshProperty& mesh, MaterialTechPtr technique) noexcept
{
	_quadObject = std::make_shared<Geometry>();
	_quadObject->setMaterialTech(RenderQueue::RenderQueueOpaqueShading, technique);
	MeshProperty sphere;
	sphere.makeSphere(1, 16, 12);
	return _buildRenderObject(_quadObject, sphere, _renderScreenQuad);
}

bool 
SkyboxComponent::_buildSphereRenderMesh(const MeshProperty& mesh) noexcept
{
	_renderSphere = RenderSystem::instance()->createRenderMesh(mesh, ModelMakerFlagBits::ModelMakerFlagBit_VER_NORMAL);
	if (!_renderSphere)
		return true;
	return false;
}

bool 
SkyboxComponent::_buildSphereRenderObject(const MeshProperty& mesh, MaterialTechPtr technique) noexcept
{
	_sphereObject = std::make_shared<Geometry>();
	_sphereObject->setMaterialTech(RenderQueue::RenderQueueOpaqueSpecific, technique);
	return _buildRenderObject(_sphereObject, mesh, _renderSphere);
}

bool
SkyboxComponent::_setupMaterial() noexcept
{
	auto material = RenderSystem::instance()->createMaterial("sys:fx/sky.fxml");
	if (!material)
		return false;

	this->setMaterial(material);
	return true;
}

bool
SkyboxComponent::_setupSkybox() noexcept
{
	if (_loadSkybox(_skyMap) && this->getMaterial())
	{
		MeshProperty sphereMesh;
		_buildSphereMesh(sphereMesh);
		_buildSphereRenderMesh(sphereMesh);
		_buildSphereRenderObject(sphereMesh, this->getMaterial()->getTech("SkyboxRender"));

		this->_attacRenderObject(_sphereObject);
		return true;
	}

	return false;
}

bool 
SkyboxComponent::_setupSkyLighting() noexcept
{
	if (!_skyDiffuse.empty() && !_skySpecular.empty() && this->getMaterial())
	{
		if (_loadSkyDiffuse(_skyDiffuse) && _loadSkySpecular(_skySpecular))
		{
			MeshProperty quadMesh;
			_buildQuadMesh(quadMesh);
			_buildQuadRenderMesh(quadMesh);
			_buildQuadRenderObject(quadMesh, this->getMaterial()->getTech("SkyLighting"));

			this->_attacRenderObject(_quadObject);
		}

		return true;
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
	
	_renderSphere.reset();
}

void 
SkyboxComponent::_destroySkyLighting() noexcept
{
	if (_quadObject)
	{
		this->_destroyRenderhObject(_quadObject);
		_quadObject.reset();
	}
	
	_renderScreenQuad.reset();
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
	transforam.setTranslate(this->getGameObject()->getTranslate());
	
	if (_sphereObject)
		_sphereObject->setTransform(transforam, math::inverse(transforam), float4x4().loadIdentity());
	
	if (_quadObject)
		_quadObject->setTransform(transforam, math::inverse(transforam), float4x4().loadIdentity());
}

void 
SkyboxComponent::_updateMaterial() noexcept
{
	auto material = this->getMaterial();
	if (material)
	{
		if (_enableSkyBox)
		{
			auto texSkybox = material->getParameter("texSkybox");
			if (texSkybox)
				texSkybox->uniformTexture(_skyTexture);
		}

		if (_enableSkyLighting)
		{
			auto texEnvDiffuse = material->getParameter("texEnvDiffuse");
			if (texEnvDiffuse)
				texEnvDiffuse->uniformTexture(_skyDiffTexture);

			auto texEnvSpecular = material->getParameter("texEnvSpecular");
			if (texEnvSpecular)
				texEnvSpecular->uniformTexture(_skySpecTexture);

			auto texEnvFactor = material->getParameter("texEnvFactor");
			if (texEnvFactor)
			{
				float3 factor;
				factor.x = _skySpecTexture->getGraphicsTextureDesc().getMipLevel();
				factor.y = _skyLightingIntensity.x;
				factor.z = _skyLightingIntensity.y;
				texEnvFactor->uniform3f(factor);
			}
				
		}
	}
}

void
SkyboxComponent::onActivate() noexcept
{
	if (_enableSkyBox || _enableSkyLighting)
		this->_setupMaterial();

	if (_enableSkyBox && _needUpdateSkybox)
	{
		this->_setupSkybox();
		_needUpdateSkybox = false;
	}

	if (_enableSkyLighting && (_needUpdateSkyDiffuse || _needUpdateSkySpecular))
	{
		this->_setupSkyLighting();
		_needUpdateSkyDiffuse = false;
		_needUpdateSkySpecular = false;
	}

	if (_enableSkyBox || _enableSkyLighting)
	{
		_updateMaterial();
		_updateTransform();

		this->_attacRenderObjects();
	}
}

void
SkyboxComponent::onDeactivate() noexcept
{
	this->_destroyMaterial();
	this->_destroySkybox();
	this->_destroySkyLighting();
}

void
SkyboxComponent::onFrameEnd() noexcept
{
	if (_needUpdateSkybox)
	{
		_reloadSkybox(_skyMap);
		_needUpdateSkybox = false;
	}

	if (_needUpdateSkyDiffuse)
	{
		_reloadSkyDiffuse(_skyDiffuse);
		_needUpdateSkyDiffuse = false;
	}

	if (_needUpdateSkySpecular)
	{
		_reloadSkySpecular(_skySpecular);
		_needUpdateSkySpecular = false;
	}
}

void
SkyboxComponent::onMoveAfter() noexcept
{
	_updateTransform();
}

_NAME_END