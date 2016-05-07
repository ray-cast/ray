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
#if defined(_BUILD_RENDERER)
#include <ray/light_component.h>
#include <ray/game_server.h>
#include <ray/render_feature.h>

_NAME_BEGIN

__ImplementSubClass(LightComponent, RenderComponent, "Light")

LightComponent::LightComponent() noexcept
{
	_light = std::make_shared<Light>();
	_light->setOwnerListener(this);
}

LightComponent::~LightComponent() noexcept
{
	_light->setRenderScene(nullptr);
}

void
LightComponent::setRange(float range) noexcept
{
	_light->setRange(range);
}

void
LightComponent::setIntensity(float intensity) noexcept
{
	_light->setIntensity(intensity);
}

void
LightComponent::setSpotInnerCone(float value) noexcept
{
	_light->setSpotInnerCone(value);
}

void
LightComponent::setSpotOuterCone(float value) noexcept
{
	_light->setSpotOuterCone(value);
}

float
LightComponent::getRange() const noexcept
{
	return _light->getRange();
}

float
LightComponent::getIntensity() const noexcept
{
	return _light->getIntensity();
}

const float2&
LightComponent::getSpotInnerCone() const noexcept
{
	return _light->getSpotInnerCone();
}

const float2&
LightComponent::getSpotOuterCone() const noexcept
{
	return _light->getSpotOuterCone();
}

void
LightComponent::setShadow(bool shadow) noexcept
{
	if (shadow)
		_light->setShadowType(LightShadowType::LightShadowTypeMedium);
	else
		_light->setShadowType(LightShadowType::LightShadowTypeNone);
}

bool
LightComponent::getShadow() const noexcept
{
	return _light->getShadowType() != LightShadowType::LightShadowTypeNone;
}

void
LightComponent::setSoftShadow(bool softEnable) noexcept
{
	_light->setSoftShadow(softEnable);
}

bool
LightComponent::getSoftShadow() const noexcept
{
	return _light->getSoftShadow();
}

void
LightComponent::setShadowBias(float bias) noexcept
{
	_light->setShadowBias(bias);
}

float
LightComponent::getShadowBias() const noexcept
{
	return _light->getShadowBias();
}

void
LightComponent::setSubsurfaceScattering(bool enable) noexcept
{
	_light->setSubsurfaceScattering(enable);
}

bool
LightComponent::getSubsurfaceScattering() const noexcept
{
	return _light->getSubsurfaceScattering();
}

void
LightComponent::setLightColor(const Vector3& color) noexcept
{
	_light->setLightColor(color);
}

const Vector3&
LightComponent::getLightColor() const noexcept
{
	return _light->getLightColor();
}

void
LightComponent::setLightType(LightType type) noexcept
{
	_light->setLightType(type);
}

LightType
LightComponent::getLightType() const noexcept
{
	return _light->getLightType();
}

void
LightComponent::load(iarchive& reader) noexcept
{
	std::string lightType;
	float3 lightColor(1, 1, 1);
	float lightIntensity = 1.0f;
	float lightRange = 1.0f;
	float shadowBias = 0.0;
	bool shadow = false;
	bool softShadow = false;
	bool subsurfaceScattering = false;

	GameComponent::load(reader);

	reader >> make_archive(lightIntensity, "intensity");
	reader >> make_archive(lightRange, "range");
	reader >> make_archive(shadow, "shadow");
	reader >> make_archive(lightColor, "color");
	reader >> make_archive(lightType, "type");
	reader >> make_archive(softShadow, "soft");
	reader >> make_archive(subsurfaceScattering, "sss");
	reader >> make_archive(shadowBias, "bias");

	if (lightType == "sun")
		this->setLightType(LightType::LightTypeSun);
	else if (lightType == "directional")
		this->setLightType(LightType::LightTypeDirectional);
	else if (lightType == "point")
		this->setLightType(LightType::LightTypePoint);
	else if (lightType == "spot")
		this->setLightType(LightType::LightTypeSpot);
	else if (lightType == "ambient")
		this->setLightType(LightType::LightTypeAmbient);
	else
		this->setLightType(LightType::LightTypePoint);

	this->setLightColor(lightColor);
	this->setRange(lightRange);
	this->setIntensity(lightIntensity);
	this->setShadow(shadow);
	this->setShadowBias(shadowBias);
	this->setSoftShadow(softShadow);
	this->setSubsurfaceScattering(subsurfaceScattering);
}

void
LightComponent::save(oarchive& write) noexcept
{
	RenderComponent::save(write);
}

GameComponentPtr
LightComponent::clone() const noexcept
{
	auto result = std::make_shared<LightComponent>();
	result->setName(this->getName());
	result->setActive(this->getActive());
	return result;
}

void
LightComponent::onActivate() noexcept
{
	_light->setRenderScene(GameServer::instance()->getFeature<RenderFeature>()->getRenderScene());
	_light->setTransform(
		this->getGameObject()->getTransform(),
		this->getGameObject()->getTransformInverse(),
		this->getGameObject()->getTransformInverseTranspose()
		);
}

void
LightComponent::onDeactivate() noexcept
{
	_light->setRenderScene(nullptr);
}

void
LightComponent::onMoveAfter() noexcept
{
	_light->setTransform(
		this->getGameObject()->getTransform(),
		this->getGameObject()->getTransformInverse(),
		this->getGameObject()->getTransformInverseTranspose()
		);
}

_NAME_END

#endif