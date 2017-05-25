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
#include <ray/camera_component.h>
#include <ray/render_feature.h>
#include <ray/game_server.h>

_NAME_BEGIN

__ImplementSubClass(CameraComponent, GameComponent, "Camera")

CameraComponent::CameraComponent() noexcept
{
	_camera = std::make_shared<Camera>();
	_camera->setOwnerListener(this);
	_camera->setCameraOrder(CameraOrder::CameraOrder3D);
}

CameraComponent::CameraComponent(const archivebuf& reader) noexcept
	: CameraComponent()
{
	this->load(reader);
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
CameraComponent::setOrtho(const float4& ortho) noexcept
{
	_camera->setOrtho(ortho);
}

const float4&
CameraComponent::getOrtho() const noexcept
{
	return _camera->getOrtho();
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
CameraComponent::setClearColor(const float4& color) noexcept
{
	_camera->setClearColor(color);
}

const float4&
CameraComponent::getClearColor() const noexcept
{
	return _camera->getClearColor();
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
CameraComponent::setCameraClearFlags(CameraClearFlags flags) noexcept
{
	_camera->setClearFlags(flags);
}

CameraClearFlags
CameraComponent::getCameraClearFlags() const noexcept
{
	return _camera->getClearFlags();
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
CameraComponent::load(const archivebuf& reader) noexcept
{
	GameComponent::load(reader);

	const auto& aperture = reader["aperture"];
	const auto& znear = reader["znear"];
	const auto& zfar = reader["zfar"];
	const auto& ratio = reader["ratio"];
	const auto& ortho = reader["ortho"];
	const auto& viewport = reader["viewport"];
	const auto& clear_color = reader["clear_color"];
	const auto& order = reader["order"];
	const auto& project = reader["project"];
	const auto& clear_flags = reader["clear_flags"];
	const auto& render_flags = reader["render_flags"];

	if (aperture.is_numeric())
		this->setAperture(aperture.get<float>());

	if (znear.is_numeric())
		this->setNear(znear.get<float>());

	if (zfar.is_numeric())
		this->setFar(zfar.get<float>());

	if (ratio.is_numeric())
		this->setRatio(ratio.get<float>());

	if (ortho.is_array())
	{
		float4 fru;

		const auto& values = ortho.get<archive::array_t>();
		for (std::uint8_t i = 0; i < 4; ++i)
			fru[i] = values[i].get<archive::number_float_t>();

		this->setOrtho(fru);
	}

	if (viewport.is_array())
	{
		float4 vp;

		const auto& values = ortho.get<archive::array_t>();
		for (std::uint8_t i = 0; i < 4; ++i)
			vp[i] = values[i].get<archive::number_float_t>();

		this->setViewport(vp);
	}

	if (clear_color.is_array())
	{
		float4 color;

		const auto& values = clear_color.get<archive::array_t>();
		for (std::uint8_t i = 0; i < 4; ++i)
			color[i] = values[i].get<archive::number_float_t>();

		_camera->setClearColor(color);
	}

	if (project.is_string())
	{
		if (project.get<archive::string_t>() == "ortho")
			this->setCameraType(CameraType::CameraTypeOrtho);
		else
			this->setCameraType(CameraType::CameraTypePerspective);
	}

	if (order.is_string())
	{
		if (order.get<archive::string_t>() == "2D")
			this->setCameraOrder(CameraOrder::CameraOrder2D);
		else
			this->setCameraOrder(CameraOrder::CameraOrder3D);
	}

	if (clear_flags.is_string())
	{
		GraphicsClearFlags flags = 0;

		std::vector<util::string> args;
		util::split(args, clear_flags.get<archive::string_t>(), "|");

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

		this->setCameraClearFlags(flags);
	}

	if (render_flags.is_string())
	{
		CameraRenderFlags flags = 0;

		std::vector<util::string> args;
		util::split(args, render_flags.get<archive::string_t>(), "|");

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
CameraComponent::save(archivebuf& write) noexcept
{
	write["aperture"] << this->getAperture();
	write["znear"] << this->getNear();
	write["zfar"] << this->getFar();
	write["ratio"] << this->getRatio();
	write["ortho"] << this->getOrtho();
	write["viewport"] << this->getViewport();
	write["clear_color"] << this->getClearColor();
	write["order"] << static_cast<archive::number_unsigned_t>(this->getCameraOrder());
	write["camera_type"] << static_cast<archive::number_unsigned_t>(this->getCameraType());
	write["clear_flags"] << this->getCameraClearFlags();
	write["render_flags"] << this->getCameraRenderFlags();
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