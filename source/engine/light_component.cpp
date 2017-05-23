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
#if defined(_BUILD_RENDERER)
#include <ray/light_component.h>
#include <ray/skybox_component.h>
#include <ray/game_server.h>
#include <ray/render_feature.h>

_NAME_BEGIN

__ImplementSubClass(LightComponent, RenderComponent, "Light")

LightComponent::LightComponent() noexcept
	: _onSkyBoxChange(std::bind(&LightComponent::onSkyBoxChange, this))
	, _onSkyLightingDiffuseChange(std::bind(&LightComponent::onSkyLightingDiffuseChange, this))
	, _onSkyLightingSpecularChange(std::bind(&LightComponent::onSkyLightingSpecularChange, this))
	, _onEnableSkyBox(std::bind(&LightComponent::onEnableSkyBox, this, std::placeholders::_1))
	, _onEnableSkyLighting(std::bind(&LightComponent::onEnableSkyLighting, this, std::placeholders::_1))
{
	_light = std::make_shared<Light>();
	_light->setOwnerListener(this);
}

LightComponent::~LightComponent() noexcept
{
	_light->setRenderScene(nullptr);
}

void
LightComponent::setLightRange(float range) noexcept
{
	_light->setLightRange(range);
}

void
LightComponent::setLightIntensity(float intensity) noexcept
{
	_light->setLightIntensity(intensity);
}

void
LightComponent::setLightAttenuation(const float3& atten) noexcept
{
	return _light->setLightAttenuation(atten);
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
LightComponent::getLightRange() const noexcept
{
	return _light->getLightRange();
}

float
LightComponent::getLightIntensity() const noexcept
{
	return _light->getLightIntensity();
}

const float3&
LightComponent::getLightAttenuation() const noexcept
{
	return _light->getLightAttenuation();
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
LightComponent::setShadowMode(ShadowMode shadow) noexcept
{
	_light->setShadowMode(shadow);
}

ShadowMode
LightComponent::getShadowMode() const noexcept
{
	return _light->getShadowMode();
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
LightComponent::setGlobalIllumination(bool enable) noexcept
{
	_light->setGlobalIllumination(enable);
}

bool
LightComponent::getGlobalIllumination() const noexcept
{
	return _light->getGlobalIllumination();
}

void
LightComponent::setLightColor(const float3& color) noexcept
{
	_light->setLightColor(color);
}

const float3&
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
	std::string shadowMode;
	float2 spot(5.0f, 40.0f);
	float3 lightColor(1, 1, 1);
	float3 lightAtten;
	float lightIntensity = 1.0f;
	float lightRange = 1.0f;
	float shadowBias = 0.0;
	bool enableGI = false;

	GameComponent::load(reader);

	if (reader.getValue("atten", lightAtten))
		this->setLightAttenuation(lightAtten);

	reader >> make_archive(lightIntensity, "intensity");
	reader >> make_archive(lightRange, "range");
	reader >> make_archive(lightColor, "color");
	reader >> make_archive(lightAtten, "atten");
	reader >> make_archive(lightType, "type");
	reader >> make_archive(enableGI, "GI");
	reader >> make_archive(shadowBias, "bias");
	reader >> make_archive(shadowMode, "shadow");
	reader >> make_archive(spot, "spot");

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
	else if (lightType == "environment")
		this->setLightType(LightType::LightTypeEnvironment);
	else
		this->setLightType(LightType::LightTypePoint);

	if (shadowMode == "hard")
		this->setShadowMode(ShadowMode::ShadowModeHard);
	else if (shadowMode == "soft")
		this->setShadowMode(ShadowMode::ShadowModeSoft);
	else
		this->setShadowMode(ShadowMode::ShadowModeNone);

	this->setLightColor(lightColor);
	this->setLightRange(lightRange);
	this->setLightIntensity(lightIntensity);
	this->setSpotInnerCone(spot.x);
	this->setSpotOuterCone(spot.y);
	this->setShadowBias(shadowBias);
	this->setGlobalIllumination(enableGI);
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
	this->addComponentDispatch(GameDispatchType::GameDispatchTypeMoveAfter, this);

	_light->setRenderScene(GameServer::instance()->getFeature<RenderFeature>()->getRenderScene());
	_light->setTransform(this->getGameObject()->getWorldTransform());
}

void
LightComponent::onDeactivate() noexcept
{
	this->removeComponentDispatch(GameDispatchType::GameDispatchTypeMoveAfter, this);

	_light->setRenderScene(nullptr);
}

void
LightComponent::onAttachComponent(GameComponentPtr& component) noexcept
{
	if (component->isInstanceOf<SkyboxComponent>())
	{
		auto skyboxComponent = component->downcast<SkyboxComponent>();
		skyboxComponent->addSkyBoxChangeListener(&_onSkyBoxChange);
		skyboxComponent->addSkyLightingDiffuseChangeListener(&_onSkyLightingDiffuseChange);
		skyboxComponent->addSkyLightingSpecularChangeListener(&_onSkyLightingSpecularChange);
		skyboxComponent->addEnableSkyBoxListener(&_onEnableSkyBox);
		skyboxComponent->addEnableSkyLightingListener(&_onEnableSkyLighting);

		if (skyboxComponent->getSkyboxEnable())
		{
			_light->setLightVisible(skyboxComponent->getSkyLightingEnable());
			_light->setSkyBox(skyboxComponent->getSkyBox());
			_light->setSkyLightingDiffuse(skyboxComponent->getSkyLightDiffuse());
			_light->setSkyLightingSpecular(skyboxComponent->getSkyLightSpecular());
		}
		else
		{
			_light->setSkyBox(nullptr);
		}
	}
}

void
LightComponent::onDetachComponent(GameComponentPtr& component) noexcept
{
	if (component->isInstanceOf<SkyboxComponent>())
	{
		auto skyboxComponent = component->downcast<SkyboxComponent>();
		skyboxComponent->removeSkyBoxChangeListener(&_onSkyBoxChange);
		skyboxComponent->removeSkyLightingDiffuseChangeListener(&_onSkyLightingDiffuseChange);
		skyboxComponent->removeSkyLightingSpecularChangeListener(&_onSkyLightingSpecularChange);
		skyboxComponent->removeEnableSkyBoxListener(&_onEnableSkyBox);
		skyboxComponent->removeEnableSkyLightingListener(&_onEnableSkyLighting);

		_light->setLightVisible(false);
		_light->setSkyBox(nullptr);
		_light->setSkyLightingDiffuse(nullptr);
		_light->setSkyLightingSpecular(nullptr);
	}
}

void
LightComponent::onMoveAfter() noexcept
{
	_light->setTransform(this->getGameObject()->getWorldTransform());
}

void
LightComponent::onEnableSkyBox(bool enable) noexcept
{
}

void
LightComponent::onEnableSkyLighting(bool enable) noexcept
{
	auto component = this->getGameObject()->getComponent<SkyboxComponent>();
	if (component)
	{
		_light->setLightVisible(enable);
		_light->setSkyLightingDiffuse(component->getSkyLightDiffuse());
		_light->setSkyLightingSpecular(component->getSkyLightSpecular());
	}
	else
	{
		_light->setLightVisible(false);
		_light->setSkyLightingDiffuse(nullptr);
		_light->setSkyLightingSpecular(nullptr);
	}
}

void
LightComponent::onSkyBoxChange() noexcept
{
	auto component = this->getGameObject()->getComponent<SkyboxComponent>();
	if (component)
		_light->setSkyBox(component->getSkyBox());
}

void
LightComponent::onSkyLightingDiffuseChange() noexcept
{
	auto component = this->getGameObject()->getComponent<SkyboxComponent>();
	if (component)
		_light->setSkyLightingDiffuse(component->getSkyLightDiffuse());
}

void
LightComponent::onSkyLightingSpecularChange() noexcept
{
	auto component = this->getGameObject()->getComponent<SkyboxComponent>();
	if (component)
		_light->setSkyLightingSpecular(component->getSkyLightSpecular());
}

_NAME_END

#endif