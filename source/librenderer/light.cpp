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
	, _enableSoftShadow(false)
	, _enableSubsurfaceScattering(false)
	, _enableGlobalIllumination(false)
	, _shadowType(LightShadowType::LightShadowTypeNone)
	, _shadowBias(0.1f)
{
	_shadowCameras.push_back(std::make_shared<Camera>());
	_shadowCameras[0]->setOwnerListener(this);
	_shadowCameras[0]->setCameraOrder(CameraOrder::CameraOrderShadow);
	_shadowCameras[0]->setCameraRenderFlags(CameraRenderFlagBits::CameraRenderTextureBit);
	_shadowCameras[0]->setAperture(90.0f);
	_shadowCameras[0]->setNear(0.1f);
	_shadowCameras[0]->setRatio(1.0f);
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
		_shadowType = shadowType;

		if (shadowType != LightShadowType::LightShadowTypeNone)
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

LightShadowType
Light::getShadowType() const noexcept
{
	return _shadowType;
}

void
Light::setSoftShadow(bool enable) noexcept
{
	_enableSoftShadow = enable;
}

bool
Light::getSoftShadow() const noexcept
{
	return _enableSoftShadow;
}

void
Light::setSubsurfaceScattering(bool enable) noexcept
{
	_enableSubsurfaceScattering = enable;
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
Light::getSubsurfaceScattering() const noexcept
{
	return _enableSubsurfaceScattering;
}

bool 
Light::getGlobalIllumination() const noexcept
{
	return _enableGlobalIllumination;
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

float
Light::getShadowBias() const noexcept
{
	return _shadowBias;
}

void
Light::setColorTexture(GraphicsTexturePtr texture) noexcept
{
	_shadowColorMap = texture;
}

const GraphicsTexturePtr&
Light::getColorTexture() const noexcept
{
	return _shadowColorMap;
}

void
Light::setNormalTexture(GraphicsTexturePtr texture) noexcept
{
	_shadowNormalMap = texture;
}

const GraphicsTexturePtr&
Light::getNormalTexture() const noexcept
{
	return _shadowNormalMap;
}

void
Light::setDepthLinearTexture(GraphicsTexturePtr texture) noexcept
{
	_shadowDepthLinearMap = texture;
}

const GraphicsTexturePtr&
Light::getDepthLinearTexture() const noexcept
{
	return _shadowDepthLinearMap;
}

bool
Light::setupShadowMap() noexcept
{
	std::uint32_t shadowMapSize = 0;
	if (_shadowType == LightShadowType::LightShadowTypeLow)
		shadowMapSize = LightShadowSize::LightShadowSizeLow;
	else if (_shadowType == LightShadowType::LightShadowTypeMedium)
		shadowMapSize = LightShadowSize::LightShadowSizeMedium;
	else if (_shadowType == LightShadowType::LightShadowTypeHigh)
		shadowMapSize = LightShadowSize::LightShadowSizeHigh;
	else if (_shadowType == LightShadowType::LightShadowTypeVeryHigh)
		shadowMapSize = LightShadowSize::LightShadowSizeVeryHigh;
	else
		return false;

	GraphicsFormat depthLinearFormat;
	if (RenderSystem::instance()->isTextureSupport(GraphicsFormat::GraphicsFormatR32SFloat))
		depthLinearFormat = GraphicsFormat::GraphicsFormatR32SFloat;
	else if (RenderSystem::instance()->isTextureSupport(GraphicsFormat::GraphicsFormatR8G8B8A8UNorm))
		depthLinearFormat = GraphicsFormat::GraphicsFormatR8G8B8A8UNorm;
	else
		return false;

	GraphicsTextureDesc depthLinearDesc;
	depthLinearDesc.setWidth(shadowMapSize);
	depthLinearDesc.setHeight(shadowMapSize);
	depthLinearDesc.setTexDim(GraphicsTextureDim::GraphicsTextureDim2D);
	depthLinearDesc.setTexFormat(depthLinearFormat);
	depthLinearDesc.setSamplerWrap(GraphicsSamplerWrap::GraphicsSamplerWrapClampToEdge);
	_shadowDepthLinearMap = RenderSystem::instance()->createTexture(depthLinearDesc);
	if (!_shadowDepthLinearMap)
		return false;

	GraphicsFramebufferLayoutDesc shaodwMapLayoutDesc;
	shaodwMapLayoutDesc.addComponent(GraphicsAttachment(0, GraphicsImageLayout::GraphicsImageLayoutColorAttachmentOptimal, depthLinearFormat));
	_shadowDepthLinearViewLayout = RenderSystem::instance()->createFramebufferLayout(shaodwMapLayoutDesc);
	if (!_shadowDepthLinearViewLayout)
		return false;

	GraphicsFramebufferDesc shadowViewDesc;
	shadowViewDesc.setWidth(shadowMapSize);
	shadowViewDesc.setHeight(shadowMapSize);
	shadowViewDesc.attach(_shadowDepthLinearMap);
	shadowViewDesc.setGraphicsFramebufferLayout(_shadowDepthLinearViewLayout);
	_shadowDepthLinearView = RenderSystem::instance()->createFramebuffer(shadowViewDesc);
	if (!_shadowDepthLinearView)
		return false;

	_shadowCameras[0]->setDepthLinearFramebuffer(_shadowDepthLinearView);
	return true;
}

bool 
Light::setupReflectiveShadowMap() noexcept
{
	std::uint32_t shadowMapSize = 0;
	if (_shadowType == LightShadowType::LightShadowTypeLow)
		shadowMapSize = LightShadowSize::LightShadowSizeLow;
	else if (_shadowType == LightShadowType::LightShadowTypeMedium)
		shadowMapSize = LightShadowSize::LightShadowSizeMedium;
	else if (_shadowType == LightShadowType::LightShadowTypeHigh)
		shadowMapSize = LightShadowSize::LightShadowSizeHigh;
	else if (_shadowType == LightShadowType::LightShadowTypeVeryHigh)
		shadowMapSize = LightShadowSize::LightShadowSizeVeryHigh;
	else
		return false;

	GraphicsFormat shadowDepthFormat;
	if (RenderSystem::instance()->isTextureSupport(GraphicsFormat::GraphicsFormatD32_SFLOAT))
		shadowDepthFormat = GraphicsFormat::GraphicsFormatD32_SFLOAT;
	else if (RenderSystem::instance()->isTextureSupport(GraphicsFormat::GraphicsFormatX8_D24UNormPack32))
		shadowDepthFormat = GraphicsFormat::GraphicsFormatX8_D24UNormPack32;
	else if (RenderSystem::instance()->isTextureSupport(GraphicsFormat::GraphicsFormatD16UNorm))
		shadowDepthFormat = GraphicsFormat::GraphicsFormatD16UNorm;
	else
		return false;

	GraphicsFormat shadowColorFormat;
	GraphicsFormat shadowNormalFormat;

	if (RenderSystem::instance()->isTextureSupport(GraphicsFormat::GraphicsFormatR8G8B8A8UNorm))
		shadowColorFormat = shadowNormalFormat = GraphicsFormat::GraphicsFormatR8G8B8A8UNorm;
	else
		return false;

	GraphicsTextureDesc shadowDepthDesc;
	shadowDepthDesc.setWidth(shadowMapSize);
	shadowDepthDesc.setHeight(shadowMapSize);
	shadowDepthDesc.setTexDim(GraphicsTextureDim::GraphicsTextureDim2D);
	shadowDepthDesc.setTexFormat(shadowDepthFormat);
	shadowDepthDesc.setSamplerFilter(GraphicsSamplerFilter::GraphicsSamplerFilterNearest);
	_shadowDepthMap = RenderSystem::instance()->createTexture(shadowDepthDesc);
	if (!_shadowDepthMap)
		return false;

	GraphicsTextureDesc shadowColorDesc;
	shadowColorDesc.setWidth(shadowMapSize);
	shadowColorDesc.setHeight(shadowMapSize);
	shadowColorDesc.setTexDim(GraphicsTextureDim::GraphicsTextureDim2D);
	shadowColorDesc.setTexFormat(shadowColorFormat);
	shadowColorDesc.setSamplerFilter(GraphicsSamplerFilter::GraphicsSamplerFilterNearest);
	_shadowColorMap = RenderSystem::instance()->createTexture(shadowColorDesc);
	if (!_shadowColorMap)
		return false;

	GraphicsTextureDesc shadowNormalDesc;
	shadowNormalDesc.setWidth(shadowMapSize);
	shadowNormalDesc.setHeight(shadowMapSize);
	shadowNormalDesc.setTexDim(GraphicsTextureDim::GraphicsTextureDim2D);
	shadowNormalDesc.setTexFormat(shadowNormalFormat);
	shadowNormalDesc.setSamplerFilter(GraphicsSamplerFilter::GraphicsSamplerFilterNearest);
	_shadowNormalMap = RenderSystem::instance()->createTexture(shadowNormalDesc);
	if (!_shadowNormalMap)
		return false;

	GraphicsFramebufferLayoutDesc shaodwRSMMapLayoutDesc;
	shaodwRSMMapLayoutDesc.addComponent(GraphicsAttachment(0, GraphicsImageLayout::GraphicsImageLayoutDepthStencilReadOnlyOptimal, shadowDepthFormat));
	shaodwRSMMapLayoutDesc.addComponent(GraphicsAttachment(1, GraphicsImageLayout::GraphicsImageLayoutColorAttachmentOptimal, shadowColorFormat));
	shaodwRSMMapLayoutDesc.addComponent(GraphicsAttachment(2, GraphicsImageLayout::GraphicsImageLayoutColorAttachmentOptimal, shadowNormalFormat));
	_shadowRSMViewLayout = RenderSystem::instance()->createFramebufferLayout(shaodwRSMMapLayoutDesc);
	if (!_shadowRSMViewLayout)
		return false;

	GraphicsFramebufferDesc shadowRSMViewDesc;
	shadowRSMViewDesc.setWidth(shadowMapSize);
	shadowRSMViewDesc.setHeight(shadowMapSize);
	shadowRSMViewDesc.attach(_shadowColorMap);
	shadowRSMViewDesc.attach(_shadowNormalMap);
	shadowRSMViewDesc.setSharedDepthStencilTexture(_shadowDepthMap);
	shadowRSMViewDesc.setGraphicsFramebufferLayout(_shadowRSMViewLayout);
	_shadowRSMView = RenderSystem::instance()->createFramebuffer(shadowRSMViewDesc);
	if (!_shadowRSMView)
		return false;

	_shadowCameras[0]->setFramebuffer(_shadowRSMView);
	return true;
}

void
Light::destroyShadowMap() noexcept
{
	for (auto& camera : _shadowCameras)
		camera->setDepthLinearFramebuffer(nullptr);

	_shadowDepthLinearMap.reset();

	_shadowRSMView.reset();
	_shadowRSMViewLayout.reset();

	_shadowDepthLinearView.reset();
	_shadowDepthLinearViewLayout.reset();
}

void
Light::destroyReflectiveShadowMap() noexcept
{
	for (auto& camera : _shadowCameras)
		camera->setFramebuffer(nullptr);

	_shadowDepthMap.reset();
	_shadowColorMap.reset();
	_shadowNormalMap.reset();

	_shadowRSMView.reset();
	_shadowRSMViewLayout.reset();
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
	Bound boundingBox;

	for (auto& camera : _shadowCameras)
	{
		if (_lightType == LightType::LightTypeAmbient)
		{
			Vector3 min(-_lightRange, -_lightRange, -_lightRange);
			Vector3 max(_lightRange, _lightRange, _lightRange);

			min *= 0.5f;
			max *= 0.5f;

			Bound bound;
			bound.encapsulate(min);
			bound.encapsulate(max);

			boundingBox.encapsulate(bound);
		}
		else
		{
			float znear = camera->getNear();
			float zfar = _lightRange;

			float3 corners[8];
			corners[0] = float3(-znear, +znear, znear);
			corners[1] = float3(+znear, +znear, znear);
			corners[2] = float3(-znear, -znear, znear);
			corners[3] = float3(+znear, -znear, znear);
			corners[4] = float3(-zfar, +zfar, zfar);
			corners[5] = float3(+zfar, +zfar, zfar);
			corners[6] = float3(-zfar, -zfar, zfar);
			corners[7] = float3(+zfar, -zfar, zfar);

			Bound bound;
			bound.encapsulate(corners, 8);
			bound.applyMatrix((float3x3)camera->getTransform());

			boundingBox.encapsulate(bound);

			if (_lightType == LightType::LightTypeSun || _lightType == LightType::LightTypeDirectional)
			{
				float w = bound.size().x * 0.5;
				float h = bound.size().y * 0.5;

				camera->setOrtho(-w, w, -h, h);
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

	this->setBoundingBox(boundingBox);
}

void
Light::onSceneChangeBefor() noexcept
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
	light->setIntensity(this->getIntensity());
	light->setRange(this->getRange());
	light->setTransform(this->getTransform());
	light->setBoundingBox(this->getBoundingBox());

	light->_spotInnerCone = _spotInnerCone;
	light->_spotOuterCone = _spotOuterCone;
	return light;
}

_NAME_END