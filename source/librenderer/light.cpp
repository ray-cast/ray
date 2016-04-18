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
	, _lightColor(1.0, 1.0, 1.0)
	, _lightAttenuation(1.0, 0.0, 0.0)
	, _spotInnerCone(5.0f, cos(math::degrees(5.0f)))
	, _spotOuterCone(40.0f, cos(math::degrees(40.0f)))
	, _subsurfaceScattering(false)
	, _shadow(false)
	, _shadowSoftEnable(false)
	, _shadowSize(512)
	, _shaodwBias(0.001)
	, _shadowFormat(GraphicsFormat::GraphicsFormatD16UNorm)
{
	_shadowCamera[0]= std::make_shared<Camera>();
	_shadowCamera[0]->setOwnerListener(this);
	_shadowCamera[0]->setCameraOrder(CameraOrder::CameraOrderShadow);
	_shadowCamera[0]->setCameraRender(CameraRender::CameraRenderTexture);
	_shadowCamera[0]->setAperture(90.0);
	_shadowCamera[0]->setNear(0.1);
	_shadowCamera[0]->setRatio(1.0);
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
	_spotInnerCone.x = std::min(_spotOuterCone.x, value);
	_spotInnerCone.y = std::cos(DEG_TO_RAD(_spotInnerCone.x));
}

void
Light::setSpotOuterCone(float value) noexcept
{
	_spotOuterCone.x = std::max(_spotInnerCone.x, value);
	_spotOuterCone.y = std::cos(DEG_TO_RAD(_spotOuterCone.x));
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

float
Light::getSpotInnerCone() const noexcept
{
	return _spotInnerCone.x;
}

float
Light::getSpotOuterCone() const noexcept
{
	return _spotOuterCone.x;
}

float 
Light::getSpotCosInnerCone() const noexcept
{
	return _spotInnerCone.y;
}

float 
Light::getSpotCosOuterCone() const noexcept
{
	return _spotOuterCone.y;
}

void
Light::setShadow(bool shadow) noexcept
{
	if (_shadow != shadow)
	{
		auto loopCount = this->getLightType() == LightType::LightTypePoint ? 6 : 1;
		for (auto i = 0; i < loopCount; i++)
		{
			if (!_shadowCamera[i])
				continue;

			if (shadow)
				_shadowCamera[i]->setRenderScene(_renderScene.lock());
			else
				_shadowCamera[i]->setRenderScene(nullptr);
		}

		_shadow = shadow;

		if (shadow)
			this->_buildShadowMap(_shadowSize);
		else
			this->_destroyShadowMap();
	}
}

bool
Light::getShadow() const noexcept
{
	return _shadow;
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
Light::setShadowMap(GraphicsTexturePtr texture) noexcept
{
	_shaodwMap = texture;
}

GraphicsTexturePtr
Light::getShadowMap() const noexcept
{
	return _shaodwMap;
}

void 
Light::setShadowSize(float size) noexcept
{
	if (_shadowSize != size)
	{
		_destroyShadowMap();
		_buildShadowMap(size);
		_shadowSize = size;
	}
}

float 
Light::getShadowSize() const noexcept
{
	return _shadowSize;
}

void 
Light::setShadowBias(float bias) noexcept
{
	_shaodwBias = bias;
}

float 
Light::getShadowBias() const noexcept
{
	return _shaodwBias;
}

bool
Light::_buildShadowMap(float size) noexcept
{
	assert(size > 0);
	assert(!_shadowCamera[0]->getFramebuffer());

	if (!_shadow)
		return false;

	GraphicsFramebufferLayoutDesc shadowLayoutDesc;
	shadowLayoutDesc.addComponent(GraphicsAttachmentDesc(GraphicsViewLayout::GraphicsViewLayoutDepthStencilAttachmentOptimal, _shadowFormat, 0));
	_shaodwViewLayout = RenderSystem::instance()->createFramebufferLayout(shadowLayoutDesc);
	if (!_shaodwViewLayout)
		return false;

	std::uint32_t loopCount = 0;

	if (_lightType == LightType::LightTypeSun ||
		_lightType == LightType::LightTypeDirectional ||
		_lightType == LightType::LightTypeSpot)
	{
		loopCount = 1;
	}
	else if (_lightType == LightType::LightTypePoint)
	{
		loopCount = 6;
	}

	for (std::uint32_t i = 0; i < loopCount; i++)
	{
		GraphicsTextureDesc shaodwMapDesc;
		shaodwMapDesc.setWidth(size);
		shaodwMapDesc.setHeight(size);
		shaodwMapDesc.setTexDim(GraphicsTextureDim::GraphicsTextureDim2D);
		shaodwMapDesc.setTexFormat(_shadowFormat);
		_shaodwMap = RenderSystem::instance()->createTexture(shaodwMapDesc);
		if (!_shaodwMap)
			return false;

		GraphicsFramebufferDesc shadowViewDesc;
		shadowViewDesc.setWidth(size);
		shadowViewDesc.setHeight(size);
		shadowViewDesc.setSharedDepthStencilTexture(_shaodwMap);
		shadowViewDesc.setGraphicsFramebufferLayout(_shaodwViewLayout);
		_shaodwView = RenderSystem::instance()->createFramebuffer(shadowViewDesc);
		if (!_shaodwView)
			return false;

		_shadowCamera[i]->setFramebuffer(_shaodwView);
		_shadowCamera[i]->setViewport(Viewport(0, 0, _shadowSize, _shadowSize));
	}

	return true;
}

void 
Light::_destroyShadowMap() noexcept
{
	_shaodwMap.reset();
	_shaodwViewLayout.reset();
	_shaodwView.reset();
}

void
Light::_updateShadow() noexcept
{
	auto loopCount = this->getLightType() == LightType::LightTypePoint ? 6 : 1;
	for (auto i = 0; i < loopCount; i++)
	{
		if (!_shadowCamera[i])
			continue;

		_shadowCamera[i]->setTransform(
			this->getTransform(),
			this->getTransformInverse(),
			this->getTransformInverseTranspose()
			);
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
			_shadowCamera[0]->setAperture(this->getSpotOuterCone());
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
	if (this->getShadow())
		manager.addRenderData(RenderQueue::RenderQueueShadow, this->upcast<RenderObject>());

	manager.addRenderData(RenderQueue::RenderQueueLighting, this->upcast<RenderObject>());
}

void 
Light::onRenderObjectPre(RenderPipeline& pipeline) noexcept
{
	auto listener = this->getOwnerListener();
	if (listener)
		listener->onRenderObjectPre(pipeline);
}

void 
Light::onRenderObjectPost(RenderPipeline& pipeline) noexcept
{
	auto listener = this->getOwnerListener();
	if (listener)
		listener->onRenderObjectPost(pipeline);
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
	light->setCastShadow(this->getCastShadow());
	light->setSpotInnerCone(this->getSpotInnerCone());
	light->setSpotOuterCone(this->getSpotOuterCone());
	light->setTransform(this->getTransform(), this->getTransformInverse(), this->getTransformInverseTranspose());
	light->setBoundingBox(this->getBoundingBox());

	return light;
}

_NAME_END