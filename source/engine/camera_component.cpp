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
#include <ray/camera_component.h>
#include <ray/skybox_component.h>
#include <ray/render_feature.h>
#include <ray/game_server.h>

_NAME_BEGIN

__ImplementSubClass(CameraComponent, GameComponent, "Camera")

CameraComponent::CameraComponent() noexcept
	: _onSkyBoxChange(std::bind(&CameraComponent::onSkyBoxChange, this))
	, _onSkyLightingDiffuseChange(std::bind(&CameraComponent::onSkyLightingDiffuseChange, this))
	, _onSkyLightingSpecularChange(std::bind(&CameraComponent::onSkyLightingSpecularChange, this))
	, _onEnableSkyBox(std::bind(&CameraComponent::onEnableSkyBox, this, std::placeholders::_1))
	, _onEnableSkyLighting(std::bind(&CameraComponent::onEnableSkyLighting, this, std::placeholders::_1))
{
	_camera = std::make_shared<Camera>();
	_camera->setOwnerListener(this);
	_camera->setCameraOrder(CameraOrder::CameraOrder3D);
}

CameraComponent::~CameraComponent() noexcept
{
	_camera->setRenderScene(nullptr);
}

void
CameraComponent::setAperture(float aspect) noexcept
{
	_camera->setAperture(aspect);
}

float
CameraComponent::getAperture() const noexcept
{
	return _camera->getAperture();
}

void
CameraComponent::setNear(float znear) noexcept
{
	_camera->setNear(znear);
}

float
CameraComponent::getNear() const noexcept
{
	return _camera->getNear();
}

void
CameraComponent::setFar(float zfar) noexcept
{
	_camera->setFar(zfar);
}

float
CameraComponent::getFar() const noexcept
{
	return _camera->getFar();
}

void
CameraComponent::setRatio(float ratio) noexcept
{
	_camera->setRatio(ratio);
}

float
CameraComponent::getRatio() const noexcept
{
	return _camera->getRatio();
}

void
CameraComponent::setOrtho(float left, float right, float bottom, float top) noexcept
{
	_camera->setOrtho(left, right, bottom, top);
}

void
CameraComponent::getOrtho(float& left, float& right, float& bottom, float& top) noexcept
{
	_camera->getOrtho(left, right, bottom, top);
}

const Matrix4x4&
CameraComponent::getView() const noexcept
{
	return _camera->getView();
}

const Matrix4x4&
CameraComponent::getViewInverse() const noexcept
{
	return _camera->getViewInverse();
}

const Matrix4x4&
CameraComponent::getProject() const noexcept
{
	return _camera->getProject();
}

const Matrix4x4&
CameraComponent::getProjectInverse() const noexcept
{
	return _camera->getProjectInverse();
}

const Matrix4x4&
CameraComponent::getViewProject() const noexcept
{
	return _camera->getViewProject();
}

const Matrix4x4&
CameraComponent::getViewProjectInverse() const noexcept
{
	return _camera->getViewProjectInverse();
}

Vector3
CameraComponent::worldToScreen(const Vector3& pos) const noexcept
{
	return _camera->worldToScreen(pos);
}

Vector3
CameraComponent::screenToWorld(const Vector3& pos) const noexcept
{
	return _camera->screenToWorld(pos);
}

Vector3
CameraComponent::screenToDirection(const Vector2& pos) const noexcept
{
	return _camera->screenToDirection(pos);
}

void
CameraComponent::setViewport(const float4& viewport) noexcept
{
	_camera->setViewport(viewport);
}

const float4&
CameraComponent::getViewport() const noexcept
{
	return _camera->getViewport();
}

float4
CameraComponent::getPixelViewport() const noexcept
{
	return _camera->getPixelViewport();
}

void
CameraComponent::setCameraType(CameraType type) noexcept
{
	_camera->setCameraType(type);
}

void
CameraComponent::setCameraOrder(CameraOrder order) noexcept
{
	_camera->setCameraOrder(order);
}

CameraType
CameraComponent::getCameraType() const noexcept
{
	return _camera->getCameraType();
}

CameraOrder
CameraComponent::getCameraOrder() const noexcept
{
	return _camera->getCameraOrder();
}

void
CameraComponent::setCameraRenderFlags(CameraRenderFlags flags) noexcept
{
	_camera->setCameraRenderFlags(flags);
}

CameraRenderFlags
CameraComponent::getCameraRenderFlags() const noexcept
{
	return _camera->getCameraRenderFlags();
}

void
CameraComponent::load(iarchive& reader) noexcept
{
	GameComponent::load(reader);

	std::string type;
	float aperture;
	float znear;
	float zfar;
	float ratio;
	float4 viewport;
	float4 ortho;
	float4 clearColor;
	std::string order;
	std::string flagsString;
	std::string skymap;
	std::string skydiffuse;
	std::string skyspecular;

	if (reader.getValue("aperture", aperture))
		this->setAperture(aperture);

	if (reader.getValue("znear", znear))
		this->setNear(znear);

	if (reader.getValue("zfar", zfar))
		this->setFar(zfar);

	if (reader.getValue("ratio", ratio))
		this->setRatio(ratio);

	if (reader.getValue("ortho", ortho))
		this->setOrtho(ortho.x, ortho.y, ortho.z, ortho.w);

	if (reader.getValue("viewport", viewport))
		this->setViewport(float4(viewport.x, viewport.y, viewport.z, viewport.w));

	if (reader.getValue("color", clearColor))
		_camera->setClearColor(clearColor);

	if (reader.getValue("type", type))
	{
		if (type == "ortho")
			this->setCameraType(CameraType::CameraTypeOrtho);
		else
			this->setCameraType(CameraType::CameraTypePerspective);
	}

	if (reader.getValue("order", order))
	{
		if (order == "2D")
			this->setCameraOrder(CameraOrder::CameraOrder2D);
		else
			this->setCameraOrder(CameraOrder::CameraOrder3D);
	}

	if (reader.getValue("clearflags", flagsString))
	{
		GraphicsClearFlags flags = 0;

		std::vector<std::string> args;
		util::split(args, flagsString, "|");

		for (auto& flag : args)
		{
			flag = util::trim(flag, ' ');

			if (flag == "color")
				flags |= GraphicsClearFlagBits::GraphicsClearFlagColorBit;
			else if (flag == "depth")
				flags |= GraphicsClearFlagBits::GraphicsClearFlagDepthBit;
			else if (flag == "stencil")
				flags |= GraphicsClearFlagBits::GraphicsClearFlagStencilBit;
		}

		this->setCameraRenderFlags(flags);
	}

	if (reader.getValue("renderflags", flagsString))
	{
		CameraRenderFlags flags = 0;

		std::vector<std::string> args;
		util::split(args, flagsString, "|");

		for (auto& flag : args)
		{
			flag = util::trim(flag, ' ');

			if (flag == "screen")
				flags |= CameraRenderFlagBits::CameraRenderScreenBit;
			else if (flag == "render_to_texture")
				flags |= CameraRenderFlagBits::CameraRenderTextureBit;
			else if (flag == "gbuffer1")
				flags |= CameraRenderFlagBits::CameraRenderGbuffer1Bit;
			else if (flag == "gbuffer2")
				flags |= CameraRenderFlagBits::CameraRenderGbuffer2Bit;
			else if (flag == "gbuffer3")
				flags |= CameraRenderFlagBits::CameraRenderGbuffer3Bit;
			else if (flag == "lighting")
				flags |= CameraRenderFlagBits::CameraRenderLightingBit;
			else if (flag == "shading")
				flags |= CameraRenderFlagBits::CameraRenderShadingBit;
		}

		this->setCameraRenderFlags(flags);
	}
}

void
CameraComponent::save(oarchive& write) noexcept
{
}

void
CameraComponent::onActivate() noexcept
{
	this->addComponentDispatch(GameDispatchType::GameDispatchTypeMoveAfter, this);

	_camera->setRenderScene(GameServer::instance()->getFeature<RenderFeature>()->getRenderScene());
	_camera->setTransform(this->getGameObject()->getWorldTransform());
}

void
CameraComponent::onDeactivate() noexcept
{
	this->removeComponentDispatch(GameDispatchType::GameDispatchTypeMoveAfter, this);

	_camera->setRenderScene(nullptr);
}

void
CameraComponent::onMoveAfter() noexcept
{
	_camera->setTransform(this->getGameObject()->getWorldTransform());
}

void 
CameraComponent::onAttachComponent(GameComponentPtr& component) noexcept
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
			_camera->setSkyBox(skyboxComponent->getSkyBox());
			_camera->setClearFlags(_camera->getClearFlags() | CameraClearFlagBits::CameraClearSkyboxBit);
		}
			
		else
		{
			_camera->setSkyBox(nullptr);
			_camera->setClearFlags(_camera->getClearFlags() & ~CameraClearFlagBits::CameraClearSkyboxBit);
		}
		
		_camera->setSkyLighting(skyboxComponent->getSkyLightingEnable());
		_camera->setSkyLightingDiffuse(skyboxComponent->getSkyLightDiffuse());
		_camera->setSkyLightingSpecular(skyboxComponent->getSkyLightSpecular());
	}
}

void 
CameraComponent::onDetachComponent(GameComponentPtr& component) noexcept
{
	if (component->isInstanceOf<SkyboxComponent>())
	{
		auto skyboxComponent = component->downcast<SkyboxComponent>();
		skyboxComponent->removeSkyBoxChangeListener(&_onSkyBoxChange);
		skyboxComponent->removeSkyLightingDiffuseChangeListener(&_onSkyLightingDiffuseChange);
		skyboxComponent->removeSkyLightingSpecularChangeListener(&_onSkyLightingSpecularChange);
		skyboxComponent->removeEnableSkyBoxListener(&_onEnableSkyBox);
		skyboxComponent->removeEnableSkyLightingListener(&_onEnableSkyLighting);

		_camera->setClearFlags(_camera->getClearFlags() & ~CameraClearFlagBits::CameraClearSkyboxBit);

		_camera->setSkyBox(nullptr);
		_camera->setSkyLighting(false);
		_camera->setSkyLightingDiffuse(nullptr);
		_camera->setSkyLightingSpecular(nullptr);
	}
}

void 
CameraComponent::onEnableSkyBox(bool enable) noexcept
{
	if (enable)
		_camera->setClearFlags(_camera->getClearFlags() | CameraClearFlagBits::CameraClearSkyboxBit);
	else
		_camera->setClearFlags(_camera->getClearFlags() & ~CameraClearFlagBits::CameraClearSkyboxBit);
}

void 
CameraComponent::onEnableSkyLighting(bool enable) noexcept
{
	auto component = this->getGameObject()->getComponent<SkyboxComponent>();
	if (component)
	{
		_camera->setSkyLighting(enable);
		_camera->setSkyLightingDiffuse(component->getSkyLightDiffuse());
		_camera->setSkyLightingSpecular(component->getSkyLightSpecular());
	}
	else
	{
		_camera->setSkyLighting(false);
		_camera->setSkyLightingDiffuse(nullptr);
		_camera->setSkyLightingSpecular(nullptr);
	}
}

void 
CameraComponent::onSkyBoxChange() noexcept
{
	auto component = this->getGameObject()->getComponent<SkyboxComponent>();
	if (component)
		_camera->setSkyBox(component->getSkyBox());
}

void 
CameraComponent::onSkyLightingDiffuseChange() noexcept
{
	auto component = this->getGameObject()->getComponent<SkyboxComponent>();
	if (component)
		_camera->setSkyLightingDiffuse(component->getSkyLightDiffuse());
}

void 
CameraComponent::onSkyLightingSpecularChange() noexcept
{
	auto component = this->getGameObject()->getComponent<SkyboxComponent>();
	if (component)
		_camera->setSkyLightingSpecular(component->getSkyLightSpecular());
}

GameComponentPtr
CameraComponent::clone() const noexcept
{
	auto camera = std::make_shared<CameraComponent>();
	camera->setName(this->getName());
	camera->setCameraOrder(this->getCameraOrder());
	camera->setCameraType(this->getCameraType());
	camera->setViewport(this->getViewport());
	camera->setAperture(this->getAperture());
	camera->setNear(this->getNear());
	camera->setFar(this->getFar());
	camera->setRatio(this->getRatio());

	return camera;
}

_NAME_END