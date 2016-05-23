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
#include <ray/light.h>
#include <ray/camera.h>
#include <ray/graphics_framebuffer.h>
#include <ray/graphics_texture.h>
#include <ray/render_system.h>
#include <ray/render_object_manager.h>

_NAME_BEGIN

__ImplementSubClass(Light, RenderObject, "Light")

Light::Light() noexcept
	: _lightType(LightType::LightTypePoint)
	, _lightIntensity(1.0f)
	, _lightRange(10.0f)
	, _lightColor(float3::One)
	, _lightAttenuation(float3::UnitX)
	, _spotInnerCone(5.0f, math::cos(math::deg2rad(5.0f)))
	, _spotOuterCone(40.0f, math::cos(math::deg2rad(40.0f)))
	, _subsurfaceScattering(false)
	, _shadowType(LightShadowType::LightShadowTypeNone)
	, _shadowSoftEnable(false)
	, _shadowBias(0.001f)
	, _shadowFormat(GraphicsFormat::GraphicsFormatR32SFloat)
{
	_shadowCamera[0] = std::make_shared<Camera>();
	_shadowCamera[0]->setOwnerListener(this);
	_shadowCamera[0]->setCameraOrder(CameraOrder::CameraOrderShadow);
	_shadowCamera[0]->setCameraRenderFlags(CameraRenderFlagBits::CameraRenderTextureBit);
	_shadowCamera[0]->setAperture(90.0f);
	_shadowCamera[0]->setNear(0.1f);
	_shadowCamera[0]->setRatio(1.0f);
}

Light::~Light() noexcept
{
}

void
Light::setLightType(LightType type) noexcept
{
	_lightType = type;
	this->_updateBoundingBox();
}

void
Light::setIntensity(float intensity) noexcept
{
	_lightIntensity = intensity;
}

void
Light::setRange(float range) noexcept
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
Light::getIntensity() const noexcept
{
	return _lightIntensity;
}

float
Light::getRange() const noexcept
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
Light::setShadowType(LightShadowType shadowType) noexcept
{
	if (_shadowType != shadowType)
	{
		if (shadowType != LightShadowType::LightShadowTypeNone)
			this->setupShadowMap(shadowType);
		else
			this->destroyShadowMap();

		_shadowType = shadowType;
	}
}

LightShadowType
Light::getShadowType() const noexcept
{
	return _shadowType;
}

void
Light::setSoftShadow(bool enable) noexcept
{
	_shadowSoftEnable = enable;
}

bool
Light::getSoftShadow() const noexcept
{
	return _shadowSoftEnable;
}

void
Light::setSubsurfaceScattering(bool enable) noexcept
{
	_subsurfaceScattering = enable;
}

bool
Light::getSubsurfaceScattering() const noexcept
{
	return _subsurfaceScattering;
}

CameraPtr
Light::getShadowCamera(std::uint8_t i) const noexcept
{
	assert(i < sizeof(_shadowCamera) / sizeof(_shadowCamera[0]));
	return _shadowCamera[i];
}

void
Light::setShadowBias(float bias) noexcept
{
	_shadowBias = bias;
}

float
Light::getShadowBias() const noexcept
{
	return _shadowBias;
}

void
Light::setShadowMap(GraphicsTexturePtr texture) noexcept
{
	_shadowMap = texture;
}

const GraphicsTexturePtr&
Light::getShadowMap() const noexcept
{
	return _shadowMap;
}

bool
Light::setupShadowMap(LightShadowType type) noexcept
{
	if (type == LightShadowType::LightShadowTypeNone)
		return true;

	std::uint32_t shadowMapSize = 0;
	if (type == LightShadowType::LightShadowTypeLow)
		shadowMapSize = LightShadowSize::LightShadowSizeLow;
	else if (type == LightShadowType::LightShadowTypeMedium)
		shadowMapSize = LightShadowSize::LightShadowSizeMedium;
	else if (type == LightShadowType::LightShadowTypeHigh)
		shadowMapSize = LightShadowSize::LightShadowSizeHigh;
	else if (type == LightShadowType::LightShadowTypeVeryHigh)
		shadowMapSize = LightShadowSize::LightShadowSizeVeryHigh;
	else
		return false;

	GraphicsTextureDesc textureDesc;
	textureDesc.setWidth(shadowMapSize);
	textureDesc.setHeight(shadowMapSize);
	textureDesc.setTexDim(GraphicsTextureDim::GraphicsTextureDim2D);
	textureDesc.setTexFormat(_shadowFormat);
	_shadowMap = RenderSystem::instance()->createTexture(textureDesc);
	if (!_shadowMap)
		return false;

	GraphicsFramebufferLayoutDesc shaodwMapLayoutDesc;
	shaodwMapLayoutDesc.addComponent(GraphicsAttachment(0, GraphicsImageLayout::GraphicsImageLayoutColorAttachmentOptimal, _shadowFormat));
	_shadowViewLayout = RenderSystem::instance()->createFramebufferLayout(shaodwMapLayoutDesc);
	if (!_shadowViewLayout)
		return false;

	GraphicsFramebufferDesc shadowViewDesc;
	shadowViewDesc.setWidth(shadowMapSize);
	shadowViewDesc.setHeight(shadowMapSize);
	shadowViewDesc.attach(_shadowMap);
	shadowViewDesc.setGraphicsFramebufferLayout(_shadowViewLayout);
	_shadowView = RenderSystem::instance()->createFramebuffer(shadowViewDesc);
	if (!_shadowView)
		return false;

	_shadowCamera[0]->setFramebuffer(_shadowView);
	return true;
}

void
Light::destroyShadowMap() noexcept
{
	_shadowCamera[0]->setFramebuffer(nullptr);
	_shadowMap.reset();
	_shadowViewLayout.reset();
	_shadowView.reset();
}

void
Light::_updateShadow() noexcept
{
	auto loopCount = this->getLightType() == LightType::LightTypePoint ? 6 : 1;
	for (auto i = 0; i < loopCount; i++)
	{
		if (!_shadowCamera[i])
			continue;

		_shadowCamera[i]->setTransform(this->getTransform());
	}
}

void
Light::_updateBoundingBox() noexcept
{
	Bound bound;

	auto lightRange = this->getRange();
	if (_lightType == LightType::LightTypeSun ||
		_lightType == LightType::LightTypeDirectional ||
		_lightType == LightType::LightTypeAmbient ||
		_lightType == LightType::LightTypeSpot)
	{
		float znear = _shadowCamera[0]->getNear();
		float zfar = lightRange;

		float3 corners[8];
		corners[0] = float3(-znear, +znear, znear);
		corners[1] = float3(+znear, +znear, znear);
		corners[2] = float3(-znear, -znear, znear);
		corners[3] = float3(+znear, -znear, znear);
		corners[4] = float3(-zfar, +zfar, zfar);
		corners[5] = float3(+zfar, +zfar, zfar);
		corners[6] = float3(-zfar, -zfar, zfar);
		corners[7] = float3(+zfar, -zfar, zfar);
		bound.encapsulate(corners, 8);

		if (_lightType == LightType::LightTypeSun || _lightType == LightType::LightTypeDirectional || _lightType == LightType::LightTypeAmbient)
		{
			float w = bound.size().x * 0.5;
			float h = bound.size().y * 0.5;

			_shadowCamera[0]->setOrtho(-w, w, -h, h);
			_shadowCamera[0]->setFar(zfar);
			_shadowCamera[0]->setCameraType(CameraType::CameraTypeOrtho);
		}
		else
		{
			_shadowCamera[0]->setAperture(this->getSpotOuterCone().x*2.0);
			_shadowCamera[0]->setFar(zfar);
			_shadowCamera[0]->setCameraType(CameraType::CameraTypePerspective);
		}
	}
	else if (_lightType == LightType::LightTypePoint)
	{
		Vector3 min(-lightRange, -lightRange, -lightRange);
		Vector3 max(lightRange, lightRange, lightRange);

		min *= 0.5f;
		max *= 0.5f;

		bound.encapsulate(min);
		bound.encapsulate(max);

		auto loopCount = this->getLightType() == LightType::LightTypePoint ? 6 : 1;
		for (auto i = 0; i < loopCount; i++)
		{
			if (!_shadowCamera[i])
				continue;

			_shadowCamera[0]->setAperture(90.0f);
			_shadowCamera[0]->setCameraType(CameraType::CameraTypePerspective);
		}
	}

	this->setBoundingBox(bound);
}

void
Light::onSceneChangeBefor() noexcept
{
	auto renderScene = this->getRenderScene();
	if (renderScene)
	{
		auto loopCount = this->getLightType() == LightType::LightTypePoint ? 6 : 1;
		for (auto i = 0; i < loopCount; i++)
		{
			if (_shadowCamera[i])
				_shadowCamera[i]->setRenderScene(nullptr);
		}
	}
}

void
Light::onSceneChangeAfter() noexcept
{
	auto renderScene = this->getRenderScene();
	if (renderScene)
	{
		auto loopCount = this->getLightType() == LightType::LightTypePoint ? 6 : 1;
		for (auto i = 0; i < loopCount; i++)
		{
			if (_shadowCamera[i])
				_shadowCamera[i]->setRenderScene(renderScene);
		}
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
	_updateShadow();
}

RenderObjectPtr
Light::clone() const noexcept
{
	auto light = std::make_shared<Light>();
	light->setLightType(this->getLightType());
	light->setLightColor(this->getLightColor());
	light->setIntensity(this->getIntensity());
	light->setRange(this->getRange());
	light->setTransform(this->getTransform());
	light->setBoundingBox(this->getBoundingBox());

	light->_spotInnerCone = _spotInnerCone;
	light->_spotOuterCone = _spotOuterCone;
	return light;
}

_NAME_END