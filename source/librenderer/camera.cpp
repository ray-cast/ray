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
#include <ray/camera.h>

_NAME_BEGIN

__ImplementSubClass(Camera, RenderObject, "Camera")

static float4x4 adjustProject = (float4x4().makeScale(1.0, 1.0, 2.0).setTranslate(0, 0, -1));

Camera::Camera() noexcept
	: _left(0.0f)
	, _right(0.0f)
	, _top(0.0f)
	, _bottom(0.0f)
	, _aperture(45.0f)
	, _ratio(1.0f)
	, _znear(0.1f)
	, _zfar(100.0f)
	, _viewport(0, 0, 0, 0)
	, _clearFlags(GraphicsClearFlags::GraphicsClearFlagsAll)
	, _clearColor(Vector4::Zero)
	, _cameraType(CameraType::CT_PERSPECTIVE)
	, _cameraOrder(CameraOrder::CO_MAIN)
	, _cameraRender(CameraRender::CR_RENDER_TO_SCREEN)
	, _needUpdateProject(true)
	, _needUpdateViewProject(true)
{
}

Camera::~Camera() noexcept
{
}

void
Camera::setAperture(float aspect) noexcept
{
	_aperture = aspect;
	_needUpdateProject = true;
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
	_needUpdateProject = true;
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
	_needUpdateProject = true;
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
	_needUpdateProject = true;
}

float
Camera::getRatio() const noexcept
{
	return _ratio;
}

void
Camera::setOrtho(float left, float right, float top, float bottom) noexcept
{
	_left = left;
	_right = right;
	_top = top;
	_bottom = bottom;
	_needUpdateProject = true;
}

void
Camera::getOrtho(float& left, float& right, float& top, float& bottom) noexcept
{
	left = _left;
	right = _right;
	top = _top;
	bottom = _bottom;
}

const Matrix4x4&
Camera::getView() const noexcept
{
	return this->getTransformInverse();
}

const Matrix4x4&
Camera::getViewInverse() const noexcept
{
	return this->getTransform();
}

const Matrix4x4&
Camera::getProject() const noexcept
{
	_updateViewProject();
	return _project;
}

const Matrix4x4&
Camera::getProjectInverse() const noexcept
{
	_updateViewProject();
	return _projectInverse;
}

const Matrix4x4&
Camera::getViewProject() const noexcept
{
	_updateViewProject();
	return _viewProejct;
}

const Matrix4x4&
Camera::getViewProjectInverse() const noexcept
{
	_updateViewProject();
	return _viewProjectInverse;
}

const Vector2&
Camera::getProjLength() const noexcept
{
	_updateViewProject();
	return _projLength;
}

const Vector4&
Camera::getProjConstant() const noexcept
{
	_updateViewProject();
	return _projConstant;
}

const Vector4&
Camera::getClipConstant() const noexcept
{
	_updateViewProject();
	return _clipConstant;
}

Vector3
Camera::worldToScreen(const Vector3& pos) const noexcept
{
	int w = (int)_viewport.width >> 1;
	int h = (int)_viewport.height >> 1;

	Vector4 v = pos * this->getViewProject();
	if (v.w != 0)
		v /= v.w;

	v.x = v.x * w + w + _viewport.left;
	v.y = v.y * h + h + _viewport.top;
#ifdef _BUILD_OPENGL
	v.z = v.z * 0.5 + 0.5;
#endif

	return Vector3(v.x, v.y, v.z);
}

Vector3
Camera::worldToProject(const Vector3& pos) const noexcept
{
	Vector4 v(pos);

	v = v * this->getViewProject();
	if (v.w != 0)
		v /= v.w;

	return v.xyz();
}

Vector3
Camera::screenToWorld(const Vector3& pos) const noexcept
{
	int w = (int)_viewport.width >> 1;
	int h = (int)_viewport.height >> 1;

	Vector4 v(pos);
	v.y = _viewport.height - pos.y;

	v.x = v.x / w - 1.0f - _viewport.left;
	v.y = v.y / h - 1.0f - _viewport.top;

	v = v * this->getViewProjectInverse();
	if (v.w != 0)
		v /= v.w;

	return Vector3(v.x, v.y, v.z);
}

Vector3
Camera::screenToDirection(const Vector2& pos) const noexcept
{
	int w = (int)_viewport.width >> 1;
	int h = (int)_viewport.height >> 1;

	Vector4 v(pos, 1.0, 1.0);

	v.x = v.x / w - 1.0f - _viewport.left;
	v.y = v.y / h - 1.0f - _viewport.top;

	return (float4((this->getProjectInverse() * v).xy(), 1.0f, 1.0f) * this->getViewInverse()).xyz();
}

void
Camera::setClearFlags(GraphicsClearFlags flags) noexcept
{
	_clearFlags = flags;
}

GraphicsClearFlags
Camera::getCameraFlags() const noexcept
{
	return _clearFlags;
}

void
Camera::setClearColor(const Vector4& color) noexcept
{
	_clearColor = color;
}

const Vector4&
Camera::getClearColor() const noexcept
{
	return _clearColor;
}

void
Camera::setViewport(const Viewport& viewport) noexcept
{
	_viewport = viewport;
}

void
Camera::setCameraType(CameraType type) noexcept
{
	_cameraType = type;
	_needUpdateProject = true;
}

void
Camera::setCameraOrder(CameraOrder order) noexcept
{
	_cameraOrder = order;
}

void
Camera::setCameraRender(CameraRender render) noexcept
{
	_cameraRender = render;
}

const Viewport&
Camera::getViewport() const noexcept
{
	return _viewport;
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

CameraRender
Camera::getCameraRender() const noexcept
{
	return _cameraRender;
}

void
Camera::setRenderTexture(GraphicsRenderTexturePtr texture) noexcept
{
	_renderTexture = texture;
}

GraphicsRenderTexturePtr
Camera::getRenderTexture() const noexcept
{
	return _renderTexture;
}

void
Camera::setGraphicsContext(GraphicsContextPtr window) noexcept
{
	_graphicsContext = window;
}

GraphicsContextPtr
Camera::getGraphicsContext() const noexcept
{
	return _graphicsContext;
}

void
Camera::_updateOrtho() const noexcept
{
	_project.makeOrtho_lh(_left, _right, _bottom / _ratio, _top / _ratio, _znear, _zfar);
	_projectInverse = _project;
	_projectInverse.inverse();

	_projLength.x = _project.a1;
	_projLength.y = _project.b2;

	_projConstant.x = 2.0 / _projLength.x;
	_projConstant.y = 2.0 / _projLength.y;
	_projConstant.z = -(1.0 + _project.c1) / _projLength.x;
	_projConstant.w = -(1.0 + _project.c2) / _projLength.y;

	_clipConstant.x = (2 * _znear * _zfar);
	_clipConstant.y = _zfar - _znear;
	_clipConstant.z = _zfar + _znear;
	_clipConstant.w = 1.0;
}

void
Camera::_updatePerspective() const noexcept
{
	_project.makePerspective_lh(_aperture, _ratio, _znear, _zfar);
	_projectInverse = _project;
	_projectInverse.inverse();

	_projLength.x = _project.a1;
	_projLength.y = _project.b2;

	_projConstant.x = 2.0 / _projLength.x;
	_projConstant.y = 2.0 / _projLength.y;
	_projConstant.z = -(1.0 + _project.c1) / _projLength.x;
	_projConstant.w = -(1.0 + _project.c2) / _projLength.y;

	_clipConstant.x = (2 * _znear * _zfar);
	_clipConstant.y = _zfar - _znear;
	_clipConstant.z = _zfar + _znear;
	_clipConstant.w = 1.0;
}

void
Camera::_updateProject() const noexcept
{
	if (_needUpdateProject)
	{
		if (_cameraType == CameraType::CT_PERSPECTIVE)
			this->_updatePerspective();
		else
			this->_updateOrtho();

		_needUpdateProject = false;
	}
}

void
Camera::_updateViewProject() const noexcept
{
	_updateProject();

	if (_needUpdateViewProject)
	{
		_viewProjectInverse = _viewProejct = this->getView() * _project;
		_viewProjectInverse.inverse();

#ifdef _BUILD_OPENGL
		_viewProejct = _viewProejct * adjustProject;
#endif
		_needUpdateViewProject = false;
	}
}

void 
Camera::onMoveAfter() noexcept
{
	_needUpdateViewProject = true;
}

_NAME_END