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
#include <ray/light.h>
#include <ray/camera.h>
#include <ray/graphics_framebuffer.h>
#include <ray/graphics_texture.h>
#include <ray/render_system.h>
#include <ray/render_object_manager.h>
#include <ray/render_pipeline_framebuffer.h>
#include "shadow_render_framebuffer.h"
#include "reflective_shadow_render_framebuffer.h"

_NAME_BEGIN

__ImplementSubClass(Light, RenderObject, "Light")

Light::Light() noexcept
	: _lightType(LightType::LightTypePoint)
	, _lightIntensity(1.0f)
	, _lightRange(10.0f)
	, _lightColor(float3::One)
	, _lightAttenuation(float3::UnitZ)
	, _spotInnerCone(5.0f, math::cos(math::deg2rad(5.0f)))
	, _spotOuterCone(40.0f, math::cos(math::deg2rad(40.0f)))
	, _enableLighting(false)
	, _enableSoftShadow(false)
	, _enableGlobalIllumination(false)
	, _shadowMode(ShadowMode::ShadowModeNone)
	, _shadowBias(0.1f)
	, _shadowFactor(600.0f)
{
}

Light::~Light() noexcept
{
	this->destroyShadowMap();
	this->destroyReflectiveShadowMap();
}

void
Light::setLightType(LightType type) noexcept
{
	_lightType = type;
	this->_updateBoundingBox();
}

void
Light::setLightIntensity(float intensity) noexcept
{
	_lightIntensity = intensity;
}

void
Light::setLightRange(float range) noexcept
{
	_lightRange = range;
	this->_updateBoundingBox();
}

void
Light::setLightColor(const float3& color) noexcept
{
	_lightColor = color;
}

void
Light::setSpotInnerCone(float value) noexcept
{
	_spotInnerCone.x = math::min(_spotOuterCone.x, value);
	_spotInnerCone.y = math::cos(math::deg2rad(_spotInnerCone.x));
}

void
Light::setSpotOuterCone(float value) noexcept
{
	_spotOuterCone.x = math::max(_spotInnerCone.x, value);
	_spotOuterCone.y = math::cos(math::deg2rad(_spotOuterCone.x));
	this->_updateBoundingBox();
}

LightType
Light::getLightType() const noexcept
{
	return _lightType;
}

float
Light::getLightIntensity() const noexcept
{
	return _lightIntensity;
}

float
Light::getLightRange() const noexcept
{
	return _lightRange;
}

const float3&
Light::getLightColor() const noexcept
{
	return _lightColor;
}

void
Light::setLightAttenuation(const float3& attenuation) noexcept
{
	_lightAttenuation = attenuation;
}

const float3&
Light::getLightAttenuation() const noexcept
{
	return _lightAttenuation;
}

const float2&
Light::getSpotInnerCone() const noexcept
{
	return _spotInnerCone;
}

const float2&
Light::getSpotOuterCone() const noexcept
{
	return _spotOuterCone;
}

void
Light::setShadowMode(ShadowMode shadowMode) noexcept
{
	if (_shadowMode != shadowMode)
	{
		_shadowMode = shadowMode;

		if (shadowMode != ShadowMode::ShadowModeNone)
			this->setupShadowMap();
		else
			this->destroyShadowMap();

		if (_enableGlobalIllumination)
		{
			this->destroyReflectiveShadowMap();
			this->setupReflectiveShadowMap();
		}
	}
}

ShadowMode
Light::getShadowMode() const noexcept
{
	return _shadowMode;
}

void
Light::setGlobalIllumination(bool enable) noexcept
{
	if (_enableGlobalIllumination != enable)
	{
		if (enable)
			this->setupReflectiveShadowMap();
		else
			this->destroyReflectiveShadowMap();

		_enableGlobalIllumination = enable;
	}
}

bool
Light::getGlobalIllumination() const noexcept
{
	return _enableGlobalIllumination;
}

void
Light::setLightVisible(bool enable) noexcept
{
	_enableLighting = enable;
}

bool
Light::getLightVisible() const noexcept
{
	return _enableLighting;
}

const Cameras&
Light::getCameras() const noexcept
{
	return _shadowCameras;
}

const CameraPtr&
Light::getCamera(std::size_t i) const noexcept
{
	assert(i < _shadowCameras.size());
	return _shadowCameras[i];
}

void
Light::setShadowBias(float bias) noexcept
{
	_shadowBias = bias;
}

void
Light::setShadowFactor(float factor) noexcept
{
	_shadowFactor = factor;
}

float
Light::getShadowBias() const noexcept
{
	return _shadowBias;
}

float
Light::getShadowFactor() const noexcept
{
	return _shadowFactor;
}

void
Light::setSkyBox(const GraphicsTexturePtr& texture) noexcept
{
	assert(!texture || texture->getGraphicsTextureDesc().getTexDim() == GraphicsTextureDim::GraphicsTextureDim2D);
	_skybox = texture;
}

const GraphicsTexturePtr&
Light::getSkyBox() const noexcept
{
	return _skybox;
}

void
Light::setSkyLightingDiffuse(const GraphicsTexturePtr& texture) noexcept
{
	assert(!texture || texture->getGraphicsTextureDesc().getTexDim() == GraphicsTextureDim::GraphicsTextureDimCube);
	_skyDiffuseIBL = texture;
}

const GraphicsTexturePtr&
Light::getSkyLightingDiffuse() const noexcept
{
	return _skyDiffuseIBL;
}

void
Light::setSkyLightingSpecular(const GraphicsTexturePtr& texture) noexcept
{
	assert(!texture || texture->getGraphicsTextureDesc().getTexDim() == GraphicsTextureDim::GraphicsTextureDimCube);
	_skySpecularIBL = texture;
}

const GraphicsTexturePtr&
Light::getSkyLightingSpecular() const noexcept
{
	return _skySpecularIBL;
}

bool
Light::setupShadowMap() noexcept
{
	auto framebuffer = std::make_shared<ray::ShadowRenderFramebuffer>();
	if (!framebuffer->setup())
		return false;

	_shadowCameras.push_back(std::make_shared<Camera>());
	_shadowCameras[0]->setOwnerListener(this);
	_shadowCameras[0]->setCameraOrder(CameraOrder::CameraOrderShadow);
	_shadowCameras[0]->setCameraRenderFlags(CameraRenderFlagBits::CameraRenderTextureBit);
	_shadowCameras[0]->setAperture(90.0f);
	_shadowCameras[0]->setNear(0.1f);
	_shadowCameras[0]->setRatio(1.0f);
	_shadowCameras[0]->setRenderPipelineFramebuffer(framebuffer);

	return true;
}

bool
Light::setupReflectiveShadowMap() noexcept
{
	auto framebuffer = std::make_shared<ray::ReflectiveShadowRenderFramebuffer>();
	if (!framebuffer->setup())
		return false;

	_shadowCameras.push_back(std::make_shared<Camera>());
	_shadowCameras[0]->setOwnerListener(this);
	_shadowCameras[0]->setCameraOrder(CameraOrder::CameraOrderShadow);
	_shadowCameras[0]->setCameraRenderFlags(CameraRenderFlagBits::CameraRenderTextureBit);
	_shadowCameras[0]->setAperture(90.0f);
	_shadowCameras[0]->setNear(0.1f);
	_shadowCameras[0]->setRatio(1.0f);
	_shadowCameras[0]->setRenderPipelineFramebuffer(framebuffer);

	return true;
}

void
Light::destroyShadowMap() noexcept
{
	for (auto& camera : _shadowCameras)
		camera->getRenderPipelineFramebuffer()->setFramebuffer(nullptr);

	_shadowCameras.clear();
}

void
Light::destroyReflectiveShadowMap() noexcept
{
	for (auto& camera : _shadowCameras)
		camera->getRenderPipelineFramebuffer()->setFramebuffer(nullptr);
}

void
Light::_updateTransform() noexcept
{
	for (auto& camera : _shadowCameras)
		camera->setTransform(this->getTransform());
}

void
Light::_updateBoundingBox() noexcept
{
	BoundingBox boundingBox;

	if (_shadowCameras.empty())
	{
		Vector3 min(-_lightRange, -_lightRange, -_lightRange);
		Vector3 max(_lightRange, _lightRange, _lightRange);

		BoundingBox bound;
		bound.encapsulate(min);
		bound.encapsulate(max);

		boundingBox.encapsulate(bound);
	}
	else
	{
		for (auto& camera : _shadowCameras)
		{
			if (_lightType == LightType::LightTypeAmbient || _lightType == LightType::LightTypeEnvironment)
			{
				Vector3 min(-_lightRange, -_lightRange, -_lightRange);
				Vector3 max(_lightRange, _lightRange, _lightRange);

				BoundingBox bound;
				bound.encapsulate(min);
				bound.encapsulate(max);

				boundingBox.encapsulate(bound);
			}
			else
			{
				float znear = camera->getNear();
				float zfar = _lightRange;

				float3 corners[8];
				corners[0].set(-znear, +znear, znear);
				corners[1].set(+znear, +znear, znear);
				corners[2].set(-znear, -znear, znear);
				corners[3].set(+znear, -znear, znear);
				corners[4].set(-zfar, +zfar, zfar);
				corners[5].set(+zfar, +zfar, zfar);
				corners[6].set(-zfar, -zfar, zfar);
				corners[7].set(+zfar, -zfar, zfar);

				BoundingBox bound;
				bound.encapsulate(corners, 8);
				bound.transform((float3x3)camera->getTransform());

				boundingBox.encapsulate(bound);

				if (_lightType == LightType::LightTypeSun || _lightType == LightType::LightTypeDirectional)
				{
					float w = bound.size().x * 0.5f;
					float h = bound.size().y * 0.5f;

					camera->setOrtho(float4(-w, w, -h, h));
					camera->setFar(zfar);
					camera->setCameraType(CameraType::CameraTypeOrtho);
				}
				else if (_lightType == LightType::LightTypeSpot)
				{
					camera->setAperture(this->getSpotOuterCone().x * 2);
					camera->setFar(zfar);
					camera->setCameraType(CameraType::CameraTypePerspective);
				}
				else if (_lightType == LightType::LightTypePoint)
				{
					camera->setAperture(90.0f);
					camera->setCameraType(CameraType::CameraTypePerspective);
				}
			}
		}
	}

	this->setBoundingBox(boundingBox);
}

void
Light::onSceneChangeBefore() noexcept
{
	auto renderScene = this->getRenderScene();
	if (renderScene)
	{
		for (auto& camera : _shadowCameras)
			camera->setRenderScene(nullptr);
	}
}

void
Light::onSceneChangeAfter() noexcept
{
	auto renderScene = this->getRenderScene();
	if (renderScene)
	{
		for (auto& camera : _shadowCameras)
			camera->setRenderScene(renderScene);
	}
}

void
Light::onAddRenderData(RenderDataManager& manager) noexcept
{
	manager.addRenderData(RenderQueue::RenderQueueLighting, this);
}

void
Light::onRenderObjectPre(const Camera& camera) noexcept
{
	auto listener = this->getOwnerListener();
	if (listener)
		listener->onRenderObjectPre(camera);
}

void
Light::onRenderObjectPost(const Camera& camera) noexcept
{
	auto listener = this->getOwnerListener();
	if (listener)
		listener->onRenderObjectPost(camera);
}

void
Light::onMoveAfter() noexcept
{
	_updateTransform();
}

RenderObjectPtr
Light::clone() const noexcept
{
	auto light = std::make_shared<Light>();
	light->setLightType(this->getLightType());
	light->setLightColor(this->getLightColor());
	light->setLightIntensity(this->getLightIntensity());
	light->setLightRange(this->getLightRange());
	light->setTransform(this->getTransform());
	light->setBoundingBox(this->getBoundingBox());

	light->_spotInnerCone = _spotInnerCone;
	light->_spotOuterCone = _spotOuterCone;
	return light;
}

_NAME_END