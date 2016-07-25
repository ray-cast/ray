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
#include <ray/camera.h>
#include <ray/graphics_swapchain.h>
#include <ray/graphics_texture.h>
#include <ray/render_system.h>
#include <ray/render_object_manager.h>

_NAME_BEGIN

__ImplementSubClass(Camera, RenderObject, "Camera")

Camera::Camera() noexcept
	: _left(-1.0f)
	, _right(1.0f)
	, _top(1.0f)
	, _bottom(-1.0f)
	, _aperture(45.0f)
	, _ratio(1.0f)
	, _znear(1.0f)
	, _zfar(100.0f)
	, _viewport(0.0f, 0.0f, 1.0f, 1.0f)
	, _clearFlags(GraphicsClearFlagBits::GraphicsClearFlagAllBit)
	, _clearColor(float4(0.0f, 0.0f, 0.0f, 1.0f))
	, _cameraType(CameraType::CameraTypePerspective)
	, _cameraOrder(CameraOrder::CameraOrder3D)
	, _cameraRenderFlags(CameraRenderFlagBits::CameraRenderScreenBit | CameraRenderFlagBits::CameraRenderShadingBit)
	, _needUpdateViewProject(true)
{
	_dataManager = std::make_shared<DefaultRenderDataManager>();
}

Camera::~Camera() noexcept
{
}

void
Camera::setAperture(float aspect) noexcept
{
	_aperture = aspect;
	_needUpdateViewProject = true;
}

float
Camera::getAperture() const noexcept
{
	return _aperture;
}

void
Camera::setNear(float znear) noexcept
{
	_znear = znear;
	_needUpdateViewProject = true;
}

float
Camera::getNear() const noexcept
{
	return _znear;
}

void
Camera::setFar(float zfar) noexcept
{
	_zfar = zfar;
	_needUpdateViewProject = true;
}

float
Camera::getFar() const noexcept
{
	return _zfar;
}

void
Camera::setRatio(float ratio) noexcept
{
	_ratio = ratio;
	_needUpdateViewProject = true;
}

float
Camera::getRatio() const noexcept
{
	return _ratio;
}

void
Camera::setOrtho(float left, float right, float bottom, float top) noexcept
{
	_left = left;
	_right = right;
	_top = top;
	_bottom = bottom;
	_needUpdateViewProject = true;
}

void
Camera::getOrtho(float& left, float& right, float& bottom, float& top) noexcept
{
	left = _left;
	right = _right;
	top = _top;
	bottom = _bottom;
}

const float4x4&
Camera::getView() const noexcept
{
	return this->getTransformInverse();
}

const float4x4&
Camera::getViewInverse() const noexcept
{
	return this->getTransform();
}

const float4x4&
Camera::getProject() const noexcept
{
	_updateViewProject();
	return _project;
}

const float4x4&
Camera::getProjectInverse() const noexcept
{
	_updateViewProject();
	return _projectInverse;
}

const float4x4&
Camera::getViewProject() const noexcept
{
	_updateViewProject();
	return _viewProejct;
}

const float4x4&
Camera::getViewProjectInverse() const noexcept
{
	_updateViewProject();
	return _viewProjectInverse;
}

const float4&
Camera::getClipConstant() const noexcept
{
	_updateViewProject();
	return _clipConstant;
}

float3
Camera::worldToScreen(const float3& pos) const noexcept
{
	float4 viewport = this->getPixelViewport();

	float w = viewport.z * 0.5f;
	float h = viewport.w * 0.5f;

	float4 v = this->getViewProject() * float4(pos, 1.0);
	if (v.w != 0)
		v /= v.w;

	v.x = v.x * w + w + viewport.x;
	v.y = v.y * h + h + viewport.y;

	return float3(v.x, v.y, v.z);
}

float3
Camera::worldToProject(const float3& pos) const noexcept
{
	float4 v(pos, 1.0);

	v = this->getViewProject() * v;
	if (v.w != 0)
		v /= v.w;

	return v.xyz();
}

float3
Camera::screenToWorld(const float3& pos) const noexcept
{
	float4 viewport = this->getPixelViewport();

	float w = viewport.z * 0.5f;
	float h = viewport.w * 0.5f;

	float4 v(pos, 1.0);
	v.y = viewport.w - pos.y;

	v.x = v.x / w - 1.0f - viewport.x;
	v.y = v.y / h - 1.0f - viewport.y;

	v = this->getViewProjectInverse() * v;
	if (v.w != 0)
		v /= v.w;

	return float3(v.x, v.y, v.z);
}

float3
Camera::screenToDirection(const float2& pos) const noexcept
{
	float4 viewport = this->getPixelViewport();

	float w = viewport.z * 0.5f;
	float h = viewport.w * 0.5f;

	float4 v(pos, 1.0, 1.0);

	v.x = v.x / w - 1.0f - viewport.x;
	v.y = v.y / h - 1.0f - viewport.y;

	return (this->getProjectInverse() * v).xyz();
}

void
Camera::setClearColor(const float4& color) noexcept
{
	_clearColor = color;
}

const float4&
Camera::getClearColor() const noexcept
{
	return _clearColor;
}

void 
Camera::setClearFlags(GraphicsClearFlags flags) noexcept
{
	_clearFlags = flags;
}

GraphicsClearFlags
Camera::getClearFlags() const noexcept
{
	return _clearFlags;
}

void
Camera::setViewport(const float4& viewport) noexcept
{
	_viewport = viewport;
}

const float4&
Camera::getViewport() const noexcept
{
	return _viewport;
}

float4
Camera::getPixelViewport() const noexcept
{
	std::uint32_t width, height;
	if (_swapchain)
	{
		width = _swapchain->getGraphicsSwapchainDesc().getWidth();
		height = _swapchain->getGraphicsSwapchainDesc().getHeight();
	}
	else
	{
		RenderSystem::instance()->getWindowResolution(width, height);
	}

	float4 result;
	result.x = _viewport.x * width;
	result.y = _viewport.y * height;
	result.z = _viewport.z * width;
	result.w = _viewport.w * height;
	return result;
}

float4
Camera::getPixelViewportDPI() const noexcept
{
	std::uint32_t width, height;
	RenderSystem::instance()->getWindowResolutionDPI(width, height);

	float4 result;
	result.x = _viewport.x * width;
	result.y = _viewport.y * height;
	result.z = _viewport.z * width;
	result.w = _viewport.w * height;
	return result;
}

void
Camera::setCameraType(CameraType type) noexcept
{
	_cameraType = type;
	_needUpdateViewProject = true;
}

void
Camera::setCameraOrder(CameraOrder order) noexcept
{
	_cameraOrder = order;
}

void
Camera::setCameraRenderFlags(CameraRenderFlags flags) noexcept
{
	_cameraRenderFlags = flags;
}

CameraType
Camera::getCameraType() const noexcept
{
	return _cameraType;
}

CameraOrder
Camera::getCameraOrder() const noexcept
{
	return _cameraOrder;
}

CameraRenderFlags
Camera::getCameraRenderFlags() const noexcept
{
	return _cameraRenderFlags;
}

void
Camera::setSwapchain(GraphicsSwapchainPtr swapchin) noexcept
{
	_swapchain = swapchin;
}

const GraphicsSwapchainPtr&
Camera::getSwapchain() const noexcept
{
	return _swapchain;
}

void
Camera::setFramebuffer(GraphicsFramebufferPtr texture) noexcept
{
	_framebuffer = texture;
}

const GraphicsFramebufferPtr&
Camera::getFramebuffer() const noexcept
{
	return _framebuffer;
}

void
Camera::setDepthLinearFramebuffer(GraphicsFramebufferPtr texture) noexcept
{
	_depthLinearFramebuffer = texture;
}

const GraphicsFramebufferPtr&
Camera::getDepthLinearFramebuffer() const noexcept
{
	return _depthLinearFramebuffer;
}

void
Camera::setRenderDataManager(RenderDataManagerPtr manager) noexcept
{
	assert(manager);
	_dataManager = manager;
}

const RenderDataManagerPtr&
Camera::getRenderDataManager() const noexcept
{
	return _dataManager;
}

void
Camera::_updateOrtho() const noexcept
{
	_project.makeOrtho_lh(_left, _right, _bottom, _top, _znear, _zfar);
	_projectInverse = math::inverse(_project);

	_clipConstant.x = _znear;
	_clipConstant.y = (_zfar - _znear);
	_clipConstant.z = _znear;
	_clipConstant.w = _zfar;
}

void
Camera::_updatePerspective() const noexcept
{
	float ratio = 1.0;

	if (_cameraOrder != CameraOrder::CameraOrderShadow)
	{
		std::uint32_t width, height;
		RenderSystem::instance()->getWindowResolution(width, height);

		ratio = (float)width / height;
	}

	_project.makePerspective_fov_lh(_aperture, _ratio * ratio, _znear, _zfar);
	_projectInverse = math::inverse(_project);

	_clipConstant.x = _znear * (_zfar / (_zfar - _znear));
	_clipConstant.y = _zfar / (_zfar - _znear);
	_clipConstant.z = _znear;
	_clipConstant.w = _zfar;
}

void
Camera::_updateViewProject() const noexcept
{
	if (_needUpdateViewProject)
	{
		if (_cameraType == CameraType::CameraTypePerspective)
			this->_updatePerspective();
		else
			this->_updateOrtho();

		_viewProejct = _project * this->getView();
		_viewProjectInverse = math::inverse(_viewProejct);

		_needUpdateViewProject = false;
	}
}

void
Camera::onMoveAfter() noexcept
{
	_needUpdateViewProject = true;
}

void 
Camera::onRenderPre(const Camera& camera) noexcept
{
	RenderObject::onRenderPre(camera);

	if (_dataManager)
		_dataManager->needUpdateVisiable(true);
}

void 
Camera::onRenderPost(const Camera& camera) noexcept
{
	RenderObject::onRenderPost(camera);
}

_NAME_END