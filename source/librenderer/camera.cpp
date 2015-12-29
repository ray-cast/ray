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
#include <ray/render_scene.h>

_NAME_BEGIN

__ImplementSubClass(Camera, RenderObject, "Camera")

Camera::Camera() noexcept
	: _left(0)
	, _right(0)
	, _top(0)
	, _bottom(0)
	, _aperture(0)
	, _ratio(1)
	, _zNear(0)
	, _zFar(0)
	, _translate(0, 0, 0)
	, _lookat(0, 0, 1)
	, _up(0, 1, 0)
	, _project(Matrix4x4::One)
	, _projectInverse(Matrix4x4::One)
	, _viewProejct(Matrix4x4::One)
	, _viewProjectInverse(Matrix4x4::One)
	, _viewport(0, 0, 0, 0)
	, _projLength(0, 0)
	, _projConstant(0, 0, 0, 0)
	, _cameraType(CameraType::CT_PERSPECTIVE)
	, _cameraOrder(CameraOrder::CO_MAIN)
	, _cameraRender(CameraRender::CR_RENDER_TO_SCREEN)
	, _clearFlags(ClearFlags::Default)
	, _clearColor(Vector4::Zero)
{
}

Camera::~Camera() noexcept
{
}

float 
Camera::getAperture() const noexcept
{
	return _aperture;
}

float 
Camera::getNear() const noexcept
{
	return _zNear;
}

float 
Camera::getFar() const noexcept
{
	return _zFar;
}

const Vector3&
Camera::getTranslate() const noexcept
{
	return _translate;
}

const Vector3&
Camera::getLookAt() const noexcept
{
	return _lookat;
}

const Vector3&
Camera::getUpVector() const noexcept
{
	return _up;
}

const Matrix4x4&
Camera::getView() const noexcept
{
	return this->getTransform();
}

const Matrix4x4&
Camera::getViewInverse() const noexcept
{
	return this->getTransformInverse();
}

const Matrix4x4&
Camera::getViewInverseTranspose() const noexcept
{
	return this->getTransformInverseTranspose();
}

const Matrix4x4&
Camera::getProject() const noexcept
{
	return _project;
}

const Matrix4x4&
Camera::getViewProject() const noexcept
{
	return _viewProejct;
}

const Matrix4x4&
Camera::getProjectInverse() const noexcept
{
	return _projectInverse;
}

const Matrix4x4&
Camera::getViewProjectInverse() const noexcept
{
	return _viewProjectInverse;
}

const Vector2&
Camera::getProjLength() const noexcept
{
	return _projLength;
}

const Vector4&
Camera::getProjConstant() const noexcept
{
	return _projConstant;
}

const Vector4&
Camera::getClipConstant() const noexcept
{
	return _clipConstant;
}

void
Camera::makeLookAt(const Vector3& pos, const Vector3& lookat, const Vector3& up) noexcept
{
	float4x4 view;
	float4x4 viewInverse;
	float4x4 viewInverseTranspose;

	_up = up;
	_lookat = lookat;
	_translate = pos;

	viewInverse = view.makeLookAt_lh(_translate, _lookat, _up);
	viewInverseTranspose = viewInverse.inverse();
	viewInverseTranspose.transpose();

	this->setTransform(view);
	this->setTransformInverse(viewInverse);
	this->setTransformInverseTranspose(viewInverseTranspose);

	this->makeViewPorject();
}

void
Camera::makeOrtho(float left, float right, float top, float bottom, float znear, float zfar, float ratio) noexcept
{
	_left = left;
	_right = right;
	_top = top;
	_bottom = bottom;
	_ratio = ratio;
	_zNear = znear;
	_zFar = zfar;

	_project.makeOrtho_lh(_left, _right, _bottom / ratio, _top / ratio, _zNear, _zFar);
	_projectInverse = _project;
	_projectInverse.inverse();

	_projLength.x = _project.a1;
	_projLength.y = _project.b2;

	_projConstant.x = 2.0 / _projLength.x;
	_projConstant.y = 2.0 / _projLength.y;
	_projConstant.z = -(1.0 + _project.c1) / _projLength.x;
	_projConstant.w = -(1.0 + _project.c2) / _projLength.y;

	_clipConstant.x = (_zNear * _zFar);
	_clipConstant.y = _zFar - _zNear;
	_clipConstant.z = _zFar;
	_clipConstant.w = 1.0;

	this->makeViewPorject();
	this->setCameraType(CT_ORTHO);
}

void
Camera::makePerspective(float aperture, float znear, float zfar, float ratio) noexcept
{
	_aperture = aperture;
	_ratio = ratio;
	_zNear = znear;
	_zFar = zfar;

	_project.makePerspective_lh(_aperture, _ratio, _zNear, _zFar);
	_projectInverse = _project;
	_projectInverse.inverse();

	_projLength.x = _project.a1;
	_projLength.y = _project.b2;

	_projConstant.x = 2.0 / _projLength.x;
	_projConstant.y = 2.0 / _projLength.y;
	_projConstant.z = -(1.0 + _project.c1) / _projLength.x;
	_projConstant.w = -(1.0 + _project.c2) / _projLength.y;

	_clipConstant.x = (_zNear * _zFar);
	_clipConstant.y = _zFar - _zNear;
	_clipConstant.z = _zFar;
	_clipConstant.w = 1.0;

	this->makeViewPorject();
	this->setCameraType(CT_PERSPECTIVE);
}

void 
Camera::makeViewPorject() noexcept
{
	_viewProejct = this->getView() * _project;
	_viewProjectInverse = _viewProejct;
	_viewProjectInverse.inverse();

#ifdef _BUILD_OPENGL
	auto adjustProject = (Matrix4x4().makeScale(1.0, 1.0, 2.0) * Matrix4x4().makeTranslate(0, 0, -1));
	_viewProejct = _viewProejct * adjustProject;
#endif
}

void 
Camera::getOrtho(float& left, float& right, float& top, float& bottom, float& ratio, float& znear, float& zfar) noexcept
{
	left = _left;
	right = _right;
	top = _top;
	bottom = _bottom;
	ratio = _ratio;
	znear = _zNear;
	zfar = _zFar;
}

void 
Camera::getPerspective(float& aperture, float& ratio, float& znear, float& zfar) noexcept
{
	aperture = _aperture;
	ratio = _ratio;
	znear = _zNear;
	zfar = _zFar;
}

Vector3
Camera::worldToScreen(const Vector3& pos) const noexcept
{
	int w = (int)_viewport.width >> 1;
	int h = (int)_viewport.height >> 1;

	Vector4 v = pos * _viewProejct;
	if (v.w != 0)
		v /= v.w;

	v.x = v.x * w + w + _viewport.left;
	v.y = v.y * h + h + _viewport.top;
	v.z = v.z * 0.5 + 0.5;

	return Vector3(v.x, v.y, v.z);
}

Vector3
Camera::worldToProject(const Vector3& pos) const noexcept
{
	Vector4 v(pos);

	v = v * _viewProejct;
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

	v = v * _viewProjectInverse;
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

	return (float4((_projectInverse * v).xy(), 1.0f, 1.0f) * this->getViewInverse()).xyz();
}

void
Camera::setClearFlags(ClearFlags flags) noexcept
{
	_clearFlags = flags;
}

ClearFlags
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
Camera::setRenderScene(RenderScenePtr scene) noexcept
{
	auto renderScene = _renderScene.lock();
	if (renderScene)
	{
		renderScene->removeCamera(this->downcast<Camera>());
	}

	_renderScene = scene;

	if (scene)
	{
		scene->addCamera(this->downcast<Camera>());
	}
}

RenderScenePtr
Camera::getRenderScene() const noexcept
{
	return _renderScene.lock();
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

CameraPtr
Camera::clone() const noexcept
{
	auto camera = std::make_shared<Camera>();
	camera->setCameraOrder(this->getCameraOrder());
	camera->setCameraType(this->getCameraType());
	camera->setViewport(this->getViewport());
	camera->setTransform(this->getTransform());
	camera->setTransformInverse(this->getTransformInverse());
	camera->setTransformInverseTranspose(this->getTransformInverseTranspose());
	camera->setBoundingBox(this->getBoundingBox());
	camera->makeLookAt(this->getTranslate(), this->getLookAt(), this->getUpVector());

	if (CameraType::CT_PERSPECTIVE == camera->getCameraType())
		camera->makePerspective(_aperture, _ratio, _zNear, _zFar);
	else if (CameraType::CT_ORTHO == camera->getCameraType())
		camera->makeOrtho(_left, _right, _bottom, _top, _ratio, _zNear, _zFar);

	return camera;
}

_NAME_END