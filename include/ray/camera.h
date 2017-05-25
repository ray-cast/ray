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
#ifndef _H_CAMERA_H_
#define _H_CAMERA_H_

#include <ray/render_object.h>

_NAME_BEGIN

class EXPORT Camera final : public RenderObject
{
	__DeclareSubClass(Camera, RenderObject)
public:
	Camera() noexcept;
	~Camera() noexcept;

	void setAperture(float aspect) noexcept;
	void setNear(float znear) noexcept;
	void setFar(float znear) noexcept;
	void setRatio(float ratio) noexcept;

	float getAperture() const noexcept;
	float getNear() const noexcept;
	float getFar() const noexcept;
	float getRatio() const noexcept;

	void setViewport(const float4& viewport) noexcept;
	const float4& getViewport() const noexcept;
	float4 getPixelViewport() const noexcept;
	float4 getPixelViewportDPI() const noexcept;

	void setOrtho(const float4& ortho) noexcept;
	const float4& getOrtho() const noexcept;

	const float4x4& getView() const noexcept;
	const float4x4& getViewInverse() const noexcept;
	const float4x4& getProject() const noexcept;
	const float4x4& getProjectInverse() const noexcept;
	const float4x4& getViewProject() const noexcept;
	const float4x4& getViewProjectInverse() const noexcept;

	float4 getClipConstant() const noexcept;

	float3 worldToScreen(const float3& pos) const noexcept;
	float3 worldToProject(const float3& pos) const noexcept;
	float3 screenToWorld(const float3& pos) const noexcept;
	float3 screenToDirection(const float2& pos) const noexcept;

	void setClearColor(const float4& color) noexcept;
	const float4& getClearColor() const noexcept;

	void setClearFlags(CameraClearFlags flags) noexcept;
	CameraClearFlags getClearFlags() const noexcept;

	void setCameraType(CameraType type) noexcept;
	CameraType getCameraType() const noexcept;

	void setCameraOrder(CameraOrder order) noexcept;
	CameraOrder getCameraOrder() const noexcept;

	void setCameraRenderFlags(CameraRenderFlags flags) noexcept;
	CameraRenderFlags getCameraRenderFlags() const noexcept;

	void setSwapchain(GraphicsSwapchainPtr swapchin) noexcept;
	const GraphicsSwapchainPtr& getSwapchain() const noexcept;

	void setFramebuffer(GraphicsFramebufferPtr texture) noexcept;
	const GraphicsFramebufferPtr& getFramebuffer() const noexcept;

	void setDepthLinearFramebuffer(GraphicsFramebufferPtr texture) noexcept;
	const GraphicsFramebufferPtr& getDepthLinearFramebuffer() const noexcept;

	void setRenderDataManager(RenderDataManagerPtr manager) noexcept;
	const RenderDataManagerPtr& getRenderDataManager() const noexcept;

private:
	void _updateOrtho() const noexcept;
	void _updatePerspective() const noexcept;
	void _updateViewProject() const noexcept;

public:
	virtual void onMoveAfter() noexcept;

	virtual void onRenderPre(const Camera& camera) noexcept;

private:
	Camera(const Camera&) noexcept = delete;
	Camera& operator=(const Camera&) noexcept = delete;

private:
	float4 _ortho;
	float _aperture;
	float _ratio;
	float _znear;
	float _zfar;

	float4 _viewport;
	float4 _clearColor;

	CameraType   _cameraType;
	CameraOrder  _cameraOrder;
	CameraClearFlags _cameraClearType;
	CameraRenderFlags _cameraRenderFlags;

	GraphicsSwapchainPtr _swapchain;
	GraphicsFramebufferPtr _framebuffer;
	GraphicsFramebufferPtr _depthLinearFramebuffer;

	RenderDataManagerPtr _dataManager;

	mutable bool _needUpdateViewProject;

	mutable float4x4 _project;
	mutable float4x4 _projectInverse;

	mutable float4x4 _viewProejct;
	mutable float4x4 _viewProjectInverse;
};

_NAME_END

#endif