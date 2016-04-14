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
	, _lightColor(1.0, 1.0, 1.0)
	, _lightAttenuation(1.0, 0.0, 0.0)
	, _spotInnerCone(cos(math::degrees(5.0f)))
	, _spotOuterCone(cos(math::degrees(40.0f)))
	, _subsurfaceScattering(false)
	, _shadow(false)
	, _shadowSoftEnable(false)
	, _shadowSize(512)
	, _shadowFormat(GraphicsFormat::GraphicsFormatD16UNorm)
{
	_shadowCamera = std::make_shared<Camera>();
	_shadowCamera->setOwnerListener(this);
	_shadowCamera->setCameraOrder(CameraOrder::CameraOrderShadow);
	_shadowCamera->setCameraRender(CameraRender::CameraRenderTexture);
	_shadowCamera->setAperture(45.0);
	_shadowCamera->setNear(0.0125);
	_shadowCamera->setRatio(1.0);
}

Light::~Light() noexcept
{
}

void
Light::setLightType(LightType type) noexcept
{
	_lightType = type;
}

void
Light::setIntensity(float intensity) noexcept
{
	_lightIntensity = intensity;
}

void
Light::setRange(float range) noexcept
{
	_shadowCamera->setFar(range);
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
	_spotInnerCone = std::min(_spotOuterCone, cos(DEG_TO_RAD(value)));
}

void
Light::setSpotOuterCone(float value) noexcept
{
	_spotOuterCone = std::max(_spotInnerCone, cos(DEG_TO_RAD(value)));
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
	return _shadowCamera->getFar();
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
	return _spotInnerCone;
}

float
Light::getSpotOuterCone() const noexcept
{
	return _spotOuterCone;
}

void
Light::setShadow(bool shadow) noexcept
{
	if (_shadow != shadow)
	{
		if (shadow)
			_shadowCamera->setRenderScene(_renderScene.lock());
		else
			_shadowCamera->setRenderScene(nullptr);

		_shadow = shadow;

		if (shadow)
			this->setupShadowMap(_shadowSize);
		else
			this->destroyShadowMap();
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
Light::getShadowCamera() const noexcept
{
	return _shadowCamera;
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
		destroyShadowMap();
		setupShadowMap(size);
		_shadowSize = size;
	}
}

float 
Light::getShadowSize() const noexcept
{
	return _shadowSize;
}

bool
Light::setupShadowMap(float size) noexcept
{
	assert(size > 0);
	assert(!_shadowCamera->getFramebuffer());

	if (!_shadow)
		return false;

	GraphicsTextureDesc textureDesc;
	textureDesc.setWidth(size);
	textureDesc.setHeight(size);
	textureDesc.setTexDim(GraphicsTextureDim::GraphicsTextureDim2D);
	textureDesc.setTexFormat(_shadowFormat);
	_shaodwMap = RenderSystem::instance()->createTexture(textureDesc);
	if (!_shaodwMap)
		return false;

	GraphicsFramebufferLayoutDesc shadowLayoutDesc;
	shadowLayoutDesc.addComponent(GraphicsAttachmentDesc(GraphicsViewLayout::GraphicsViewLayoutDepthStencilAttachmentOptimal, _shadowFormat, 0));
	_shaodwViewLayout = RenderSystem::instance()->createFramebufferLayout(shadowLayoutDesc);
	if (!_shaodwViewLayout)
		return false;

	GraphicsFramebufferDesc shadowViewDesc;
	shadowViewDesc.setWidth(size);
	shadowViewDesc.setHeight(size);
	shadowViewDesc.setSharedDepthTexture(_shaodwMap);
	shadowViewDesc.setGraphicsFramebufferLayout(_shaodwViewLayout);
	_shaodwView = RenderSystem::instance()->createFramebuffer(shadowViewDesc);
	if (!_shaodwView)
		return false;

	_shadowCamera->setFramebuffer(_shaodwView);
	_shadowCamera->setViewport(Viewport(0, 0, _shadowSize, _shadowSize));

	return true;
}

void 
Light::destroyShadowMap() noexcept
{
	_shaodwMap.reset();
	_shaodwViewLayout.reset();
	_shaodwView.reset();
}

void
Light::_updateShadow() noexcept
{
	_shadowCamera->setTransform(
		this->getTransform(), 
		this->getTransformInverse(), 
		this->getTransformInverseTranspose()
		);
}

void
Light::_updateBoundingBox() noexcept
{
	Bound bound;
	
	auto lightRange = this->getRange();

	if (_lightType == LightType::LightTypeSun ||
		_lightType == LightType::LightTypeDirectional ||
		_lightType == LightType::LightTypeSpot)
	{
		Vector3 min(-lightRange, -lightRange * 2, -lightRange);
		Vector3 max(lightRange, 0, lightRange);

		min *= 0.5;
		max *= 0.5;

		bound.encapsulate(min);
		bound.encapsulate(max);

		this->setBoundingBox(bound);
	}
	else if (_lightType == LightType::LightTypePoint || _lightType == LightType::LightTypeAmbient)
	{
		Vector3 min(-lightRange, -lightRange, -lightRange);
		Vector3 max(lightRange, lightRange, lightRange);

		bound.encapsulate(min);
		bound.encapsulate(max);

		this->setBoundingBox(bound);
	}
}

void 
Light::onSceneChangeBefor() noexcept
{
	auto renderScene = this->getRenderScene();
	if (renderScene)
	{
		if (_shadowCamera)
			_shadowCamera->setRenderScene(nullptr);
	}
}

void 
Light::onSceneChangeAfter() noexcept
{
	auto renderScene = this->getRenderScene();
	if (renderScene)
	{
		if (_shadowCamera)
			_shadowCamera->setRenderScene(renderScene);
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