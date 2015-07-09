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
#include <ray/render_texture.h>
#include <ray/render_scene.h>
#include <ray/render_factory.h>

_NAME_BEGIN

LightListener::LightListener() noexcept
{
}

LightListener::~LightListener() noexcept
{
}

Light::Light() noexcept
	: _lightType(LightType::LT_POINT)
	, _lightListener(nullptr)
	, _lightRange(1.0f)
	, _lightIntensity(1.0f)
	, _lightColor(Vector3(1.0, 1.0, 1.0))
	, _lightDirection(-Vector3::UnitY)
	, _spotExponent(1.0f)
	, _spotAngle(cos(M_PI_3))
	, _spotInnerCone(cos(degrees(20.0f)))
	, _spotOuterCone(cos(degrees(40.0f)))
	, _shadow(false)
	, _shadowSize(512)
	, _shadowTranslate(Vector3::Zero)
	, _shadowLookAt(Vector3::Zero)
	, _renderScene(nullptr)
{
}

Light::~Light() noexcept
{
}

void
Light::setup() noexcept
{
	if (this->getShadow())
	{
		this->setupShadow(_shadowSize);
	}
}

void
Light::close() noexcept
{
	this->discardShadow();
	this->setRenderScene(nullptr);
}

void
Light::setLightType(LightType type) noexcept
{
	_lightType = type;
}

void
Light::setLightListener(LightListener* listener) noexcept
{
	_lightListener = listener;
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
}

void
Light::setLightColor(const Vector3& color) noexcept
{
	_lightColor = color;
}

void
Light::setExponent(float value) noexcept
{
	_spotExponent = value;
}

void
Light::setSpotAngle(float value) noexcept
{
	_spotAngle = value;
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
}

LightType
Light::getLightType() const noexcept
{
	return _lightType;
}

LightListener*
Light::getLightListener() const noexcept
{
	return _lightListener;
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
Light::setLightDirection(const Vector3& direction) noexcept
{
	_lightDirection = direction;
	_updateShadow();
}

const Vector3&
Light::getLightDirection() const noexcept
{
	return _lightDirection;
}

float
Light::getExponent() const noexcept
{
	return _spotExponent;
}

float
Light::getSpotAngle() const noexcept
{
	return _spotAngle;
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
	_shadow = shadow;
}

bool
Light::getShadow() const noexcept
{
	return _shadow;
}

void
Light::setupShadow(std::size_t size) noexcept
{
	assert(!_shadowCamera);

	auto depthTexture = RenderFactory::createRenderTarget();
	depthTexture->setup(size, size, TextureDim::DIM_2D, PixelFormat::DEPTH_COMPONENT32);
	depthTexture->setClearFlags(ClearFlags::CLEAR_ALL);

	_shadowCamera = std::make_shared<Camera>();
	_shadowCamera->setCameraOrder(CameraOrder::CO_SHADOW);
	_shadowCamera->setCameraRender(CameraRender::CR_RENDER_TO_TEXTURE);
	_shadowCamera->setViewport(Viewport(0, 0, size, size));
	_shadowCamera->setRenderListener(this);
	_shadowCamera->setRenderTarget(depthTexture);
	_shadowCamera->setRenderScene(_renderScene->shared_from_this());
	_shadowCamera->makePerspective(90.0, 1.0, 0.1, _lightRange);

	_updateShadow();
}

void
Light::discardShadow() noexcept
{
	_shadowCamera = nullptr;
}

void
Light::setTransform(const Matrix4x4& m) noexcept
{
	RenderObject::setTransform(m);
	_updateShadow();
}

CameraPtr
Light::getShadowCamera() const noexcept
{
	return _shadowCamera;
}

TexturePtr
Light::getShadowMap() const noexcept
{
	return _shadowCamera->getRenderTarget()->getResolveTexture();
}

void
Light::setRenderScene(RenderScenePtr scene) noexcept
{
	if (_renderScene)
	{
		_renderScene->removeLight(this);
		if (_shadowCamera)
			_shadowCamera->setRenderScene(nullptr);
	}

	_renderScene = scene.get();

	if (_renderScene)
	{
		_renderScene->addLight(this);
		if (_shadowCamera)
			_shadowCamera->setRenderScene(scene);
	}
}

RenderScenePtr
Light::getRenderScene() const noexcept
{
	return _renderScene->shared_from_this();
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
	light->setSpotAngle(this->getSpotAngle());
	light->setSpotInnerCone(this->getSpotInnerCone());
	light->setSpotOuterCone(this->getSpotOuterCone());
	light->setExponent(this->getExponent());
	light->setTransform(this->getTransform());
	light->setBoundingBox(this->getBoundingBox());

	return light;
}

void
Light::_updateShadow() noexcept
{
	if (_shadowCamera)
	{
		auto direction = -this->getLightDirection();
		auto translate = this->getTransform().getTranslate();
		auto lookat = direction * _lightRange + translate;

		_shadowTranslate = translate;
		_shadowLookAt = lookat;

		_shadowCamera->makeLookAt(translate, _shadowLookAt, Vector3::UnitZ);
		_shadowCamera->makeViewProject();
	}
}

void
Light::onWillRenderObject() noexcept
{
	/*if (_lightType == LT_SUN)
	{
		auto camera = *_renderScene->getCameraList().rbegin();

		auto translate = _shadowTranslate + camera->getTranslate();
		translate.y = _shadowTranslate.y;

		auto lookat = _shadowLookAt + camera->getTranslate();
		lookat.y = _shadowLookAt.y;

		_shadowCamera->makeLookAt(translate, lookat, Vector3::UnitZ);
		_shadowCamera->makeViewProject();
	}*/
}

void
Light::onRenderObject() noexcept
{
}

_NAME_END