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
#include <ray/light.h>
#include <ray/camera.h>
#include <ray/graphics_view.h>
#include <ray/graphics_texture.h>
#include <ray/render_scene.h>
#include <ray/render_system.h>

_NAME_BEGIN

__ImplementSubClass(Light, RenderObject, "Light")

Light::Light() noexcept
	: _lightType(LightType::LT_POINT)
	, _lightRange(1.0f)
	, _lightIntensity(1.0f)
	, _lightColor(Vector3(1.0, 1.0, 1.0))
	, _lightAttenuation(1, 1, 1)
	, _spotInnerCone(cos(degrees(20.0f)))
	, _spotOuterCone(cos(degrees(40.0f)))
	, _shadow(false)
	, _shadowUpdated(false)
	, _shadowSize(512)
{
	_shadowCamera = std::make_shared<Camera>();
	_shadowCamera->setOwnerListener(this);
	_shadowCamera->setCameraOrder(CameraOrder::CO_SHADOW);
	_shadowCamera->setCameraRender(CameraRender::CR_RENDER_TO_TEXTURE);
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
	_lightRange = range;
	this->_updateBoundingBox();
}

void
Light::setLightColor(const Vector3& color) noexcept
{
	_lightColor = color;
}

void
Light::setSpotInnerCone(float value) noexcept
{
	_spotInnerCone = value;
}

void
Light::setSpotOuterCone(float value) noexcept
{
	_spotOuterCone = value;
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

const Vector3&
Light::getLightColor() const noexcept
{
	return _lightColor;
}

void
Light::setLightAttenuation(const Vector3& attenuation) noexcept
{
	_lightAttenuation = attenuation;
}

const Vector3&
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
	}
}

bool
Light::getShadow() const noexcept
{
	return _shadow;
}

void
Light::setTransform(const Matrix4x4& m) noexcept
{
	RenderObject::setTransform(m);
	_shadowUpdated = false;
}

CameraPtr
Light::getShadowCamera() const noexcept
{
	_updateShadow();
	return _shadowCamera;
}

GraphicsTexturePtr
Light::getShadowMap() const noexcept
{
	_updateShadow();
	return _shadowCamera->getRenderTexture()->getResolveTexture();
}

void 
Light::_updateShadow() const noexcept
{
	if (_shadowUpdated)
		return;

	if (this->getShadow())
	{
		if (!_shadowCamera->getRenderTexture())
		{
			auto depthTexture = RenderSystem::instance()->createRenderTexture(_shadowSize, _shadowSize, TextureDim::DIM_2D, TextureFormat::DEPTH_COMPONENT16);
			_shadowCamera->setRenderTexture(depthTexture);
		}
	}

	_shadowCamera->setViewport(Viewport(0, 0, _shadowSize, _shadowSize));
	_shadowCamera->setAperture(90.0);
	_shadowCamera->setNear(0.1);
	_shadowCamera->setFar(_lightRange);
	_shadowCamera->setRatio(1.0);

	_shadowCamera->setTransform(this->getTransform());
	_shadowCamera->setTransformInverse(this->getTransformInverse());
	_shadowCamera->setTransformInverseTranspose(this->getTransformInverseTranspose());

	_shadowUpdated = true;
}

void
Light::_updateBoundingBox() noexcept
{
	Bound bound;

	if (_lightType == LightType::LT_SUN ||
		_lightType == LightType::LT_AMBIENT)
	{
		float infinity = std::numeric_limits<float>::max();

		Vector3 min(-infinity, -infinity, -infinity);
		Vector3 max(infinity, infinity, infinity);

		bound.encapsulate(min);
		bound.encapsulate(max);
	}
	else if (_lightType == LightType::LT_POINT)
	{
		Vector3 min(-_lightRange, -_lightRange, -_lightRange);
		Vector3 max(_lightRange, _lightRange, _lightRange);

		bound.encapsulate(min);
		bound.encapsulate(max);
	}
	else if (_lightType == LightType::LT_AREA ||
		_lightType == LightType::LT_SPOT ||
		_lightType == LightType::LT_HEMI_SPHERE)
	{
		Vector3 min(-_lightRange, -_lightRange, -_lightRange);
		Vector3 max(_lightRange, _lightRange, _lightRange);

		bound.encapsulate(min);
		bound.encapsulate(max);
	}

	this->setBoundingBox(bound);
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
Light::onWillRenderObject(const Camera& camera) noexcept
{
	auto listener = this->getOwnerListener();
	if (listener)
		listener->onWillRenderObject(camera);

	this->_updateShadow();
}

void 
Light::onRenderObject(const Camera& camera) noexcept
{
	auto listener = this->getOwnerListener();
	if (listener)
		listener->onRenderObject(camera);
}

LightPtr
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
	light->setTransform(this->getTransform());
	light->setBoundingBox(this->getBoundingBox());

	return light;
}

_NAME_END