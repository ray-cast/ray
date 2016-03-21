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

float
LightComponent::getSpotInnerCone() const noexcept
{
	return _light->getSpotInnerCone();
}

float
LightComponent::getSpotOuterCone() const noexcept
{
	return _light->getSpotOuterCone();
}

void
LightComponent::setShadow(bool shadow) noexcept
{
	_light->setShadow(shadow);
}

bool
LightComponent::getShadow() const noexcept
{
	return _light->getShadow();
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
	bool shadow = false;

	GameComponent::load(reader);

	reader >> make_archive(lightIntensity, "intensity");
	reader >> make_archive(lightRange, "range");
	reader >> make_archive(shadow, "shadow");
	reader >> make_archive(lightColor, "color");
	reader >> make_archive(lightType, "type");

	if (lightType == "sun")
		this->setLightType(LightType::LightTypeSun);
	else if (lightType == "point")
		this->setLightType(LightType::LightTypePoint);
	else if (lightType == "spot")
		this->setLightType(LightType::LightTypeSpot);
	else if (lightType == "area")
		this->setLightType(LightType::LightTypeArea);
	else if (lightType == "hemiSphere")
		this->setLightType(LightType::LightTypeHemiSphere);
	else if (lightType == "ambient")
		this->setLightType(LightType::LightTypeAmbient);
	else
		this->setLightType(LightType::LightTypePoint);

	_light->setLightColor(lightColor);
	_light->setRange(lightRange);
	_light->setIntensity(lightIntensity);
	_light->setShadow(shadow);
}

void
LightComponent::save(oarchive& write) noexcept
{
	RenderComponent::save(write);
}

GameComponentPtr
LightComponent::clone() const noexcept
{
	auto instance = std::make_shared<LightComponent>();
	instance->_light = _light->clone()->downcast<Light>();
	return instance;
}

void
LightComponent::onActivate() noexcept
{
	auto renderer = this->getGameObject()->getGameServer()->getFeature<RenderFeature>();
	if (renderer)
	{
		auto renderScene = renderer->getRenderScene(this->getGameObject()->getGameScene());
		if (renderScene)
		{
			_light->setTransform(
				this->getGameObject()->getTransform(),
				this->getGameObject()->getTransformInverse(),
				this->getGameObject()->getTransformInverseTranspose()
				);

			_light->setRenderScene(renderScene);
		}
	}
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