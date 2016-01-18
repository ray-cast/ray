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
#ifndef _H_CAMERA_H_
#define _H_CAMERA_H_

#include <ray/render_object.h>

_NAME_BEGIN

enum CameraType
{
	CT_ORTHO,
	CT_PERSPECTIVE
};

enum CameraOrder
{
	CO_SHADOW,
	CO_COLOR,
	CO_NORMAL,
	CO_LIGHT,
	CO_SHADING,
	CO_CUSTOM,
	CO_CUBEMAP,
	CO_MAIN
};

enum CameraRender
{
	CR_RENDER_TO_SCREEN,
	CR_RENDER_TO_TEXTURE,
	CR_RENDER_TO_CUBEMAP,
};

class EXPORT Camera final : public RenderObject
{
	__DeclareSubClass(Camera, RenderObject)
public:
	Camera() noexcept;
	~Camera() noexcept;

	void setAperture(float aspect) noexcept;
	float getAperture() const noexcept;

	void setNear(float znear) noexcept;
	float getNear() const noexcept;

	void setFar(float znear) noexcept;
	float getFar() const noexcept;

	void setRatio(float ratio) noexcept;
	float getRatio() const noexcept;

	void setViewport(const Viewport& viewport) noexcept;
	const Viewport&  getViewport() const noexcept;

	void setOrtho(float left, float right, float top, float bottom) noexcept;
	void getOrtho(float& left, float& right, float& top, float& bottom) noexcept;

	const Matrix4x4& getView() const noexcept;
	const Matrix4x4& getViewInverse() const noexcept;

	const Matrix4x4& getProject() const noexcept;
	const Matrix4x4& getProjectInverse() const noexcept;

	const Matrix4x4& getViewProject() const noexcept;
	const Matrix4x4& getViewProjectInverse() const noexcept;

	const Vector2& getProjLength() const noexcept;
	const Vector4& getProjConstant() const noexcept;

	const Vector4& getClipConstant() const noexcept;

	Vector3 worldToScreen(const Vector3& pos) const noexcept;
	Vector3 worldToProject(const Vector3& pos) const noexcept;

	Vector3 screenToWorld(const Vector3& pos) const noexcept;
	Vector3 screenToDirection(const Vector2& pos) const noexcept;

	void setClearFlags(ClearFlags flags) noexcept;
	ClearFlags getCameraFlags() const noexcept;

	void setClearColor(const Vector4& color) noexcept;
	const Vector4& getClearColor() const noexcept;

	void setCameraType(CameraType type) noexcept;
	CameraType getCameraType() const noexcept;

	void setCameraRender(CameraRender mode) noexcept;
	CameraRender getCameraRender() const noexcept;

	void setCameraOrder(CameraOrder order) noexcept;
	CameraOrder getCameraOrder() const noexcept;

	void setRenderTexture(GraphicsRenderTexturePtr texture) noexcept;
	GraphicsRenderTexturePtr getRenderTexture() const noexcept;

	void setGraphicsContext(GraphicsContextPtr window) noexcept;
	GraphicsContextPtr getGraphicsContext() const noexcept;

private:
	void _updateOrtho() const noexcept;
	void _updatePerspective() const noexcept;
	void _updateProject() const noexcept;
	void _updateViewProject() const noexcept;

	void onMoveAfter() noexcept;

private:
	Camera(const Camera&) noexcept = delete;
	Camera& operator=(const Camera&) noexcept = delete;

private:
	float _left;
	float _right;
	float _top;
	float _bottom;
	float _aperture;
	float _ratio;
	float _znear;
	float _zfar;

	Viewport _viewport;

	ClearFlags _clearFlags;
	Vector4    _clearColor;

	CameraType   _cameraType;
	CameraOrder  _cameraOrder;
	CameraRender _cameraRender;

	GraphicsContextPtr _graphicsContext;
	GraphicsRenderTexturePtr _renderTexture;

	mutable bool _needUpdateProject;
	mutable bool _needUpdateViewProject;

	mutable Vector2 _projLength;
	mutable Vector4 _projConstant;
	mutable Vector4 _clipConstant;

	mutable Matrix4x4 _project;
	mutable Matrix4x4 _projectInverse;

	mutable Matrix4x4 _viewProejct;
	mutable Matrix4x4 _viewProjectInverse;
};

_NAME_END

#endif