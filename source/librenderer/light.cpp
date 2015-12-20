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
	, _shadowUpVector(Vector3::UnitY)
	, _shadowLookAt(Vector3::Zero)
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
Light::setLightUp(const Vector3& up) noexcept
{
	_shadowUpVector = up;
	_shadowUpdated = false;
}

void
Light::setLightLookat(const Vector3& lookat) noexcept
{
	_shadowLookAt = lookat;
	_shadowUpdated = false;
}

void
Light::setLightAttenuation(const Vector3& attenuation) noexcept
{
	_lightAttenuation = attenuation;
}

const Vector3&
Light::getLightUp() const noexcept
{
	return _shadowUpVector;
}

const Vector3&
Light::getLightLookat() const noexcept
{
	return _shadowLookAt;
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
Light::setRenderScene(RenderScenePtr scene) noexcept
{
	auto renderScene = _renderScene.lock();
	if (renderScene)
	{
		if (this->getShadow())
			_shadowCamera->setRenderScene(nullptr);

		renderScene->removeLight(this->downcast<Light>());
	}

	_renderScene = scene;

	if (scene)
	{
		if (this->getShadow())
			_shadowCamera->setRenderScene(scene);

		scene->addLight(this->downcast<Light>());
	}
}

RenderScenePtr
Light::getRenderScene() const noexcept
{
	return _renderScene.lock();
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
			auto depthTexture = RenderSystem::instance()->createRenderTexture(_shadowSize, _shadowSize, TextureDim::DIM_2D, TextureFormat::DEPTH_COMPONENT32);
			_shadowCamera->setRenderTexture(depthTexture);
		}
	}

	_shadowCamera->setViewport(Viewport(0, 0, _shadowSize, _shadowSize));

	_shadowCamera->makeLookAt(this->getTransform().getTranslate(), _shadowLookAt, _shadowUpVector);
	_shadowCamera->makePerspective(90.0, 0.1, _lightRange);

	_shadowUpdated = true;

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
Light::onWillRenderObject(const Camera& camera) noexcept
{
	auto listener = this->getOwnerListener();
	if (listener)
		listener->onWillRenderObject(camera);
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