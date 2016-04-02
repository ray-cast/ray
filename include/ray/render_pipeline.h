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
#ifndef _H_RENDER_PIPELINE_H_
#define _H_RENDER_PIPELINE_H_

#include <ray/render_object_manager_base.h>
#include <ray/graphics_context.h>

_NAME_BEGIN

class EXPORT RenderPipeline
{
public:
	RenderPipeline() noexcept;
	virtual ~RenderPipeline() noexcept;

	bool open(WindHandle window, std::uint32_t w, std::uint32_t h) noexcept;
	void close() noexcept;

	void renderBegin() noexcept;
	void renderEnd() noexcept;

	void setCamera(CameraPtr renderer) noexcept;
	CameraPtr getCamera() const noexcept;

	void setRenderDataManager(RenderDataManagerPtr manager) noexcept;
	RenderDataManagerPtr getRenderDataManagerPtr() const noexcept;

	void setSwapInterval(GraphicsSwapInterval interval) noexcept;
	GraphicsSwapInterval getSwapInterval() const noexcept;
	
	void setWindowResolution(std::uint32_t w, std::uint32_t h) noexcept;
	void getWindowResolution(std::uint32_t& w, std::uint32_t& h) const noexcept;

	GraphicsFramebufferPtr createRenderTexture(const GraphicsFramebufferDesc& desc) noexcept;
	GraphicsTexturePtr createTexture(const GraphicsTextureDesc& desc) noexcept;
	GraphicsTexturePtr createTexture(std::uint32_t w, std::uint32_t h, GraphicsTextureDim dim, GraphicsFormat format) noexcept;
	GraphicsTexturePtr createTexture(const std::string& name) noexcept;

	MaterialPtr createMaterial(const std::string& name) noexcept;

	GraphicsInputLayoutPtr createInputLayout(const GraphicsInputLayoutDesc& desc) noexcept;

	GraphicsDataPtr createGraphicsData(const GraphicsDataDesc& desc) noexcept;
	RenderBufferPtr createRenderBuffer(GraphicsDataPtr vb, GraphicsDataPtr ib) noexcept;
	RenderBufferPtr createRenderBuffer(const MeshProperty& mesh) noexcept;
	RenderBufferPtr createRenderBuffer(const MeshPropertys& mesh) noexcept;

	void addRenderData(RenderQueue queue, RenderPass pass, RenderObjectPtr object) noexcept;
	RenderObjects& getRenderData(RenderQueue queue, RenderPass pass) noexcept;

	void setViewport(const Viewport& view, std::size_t i) noexcept;
	const Viewport& getViewport(std::size_t i) const noexcept;

	void setRenderTexture(GraphicsFramebufferPtr target) noexcept;
	void clearRenderTexture(GraphicsClearFlags flags, const Vector4& color, float depth, std::int32_t stencil) noexcept;
	void clearRenderTexture(GraphicsClearFlags flags, const Vector4& color, float depth, std::int32_t stencil, std::size_t i) noexcept;
	void discradRenderTexture() noexcept;
	void readRenderTexture(GraphicsFramebufferPtr target, GraphicsFormat pfd, std::size_t w, std::size_t h, void* data) noexcept;
	void blitRenderTexture(GraphicsFramebufferPtr srcTarget, const Viewport& src, GraphicsFramebufferPtr destTarget, const Viewport& dest) noexcept;

	void setMaterialPass(MaterialPassPtr pass) noexcept;
	MaterialPassPtr getMaterialPass() noexcept;

	void setMaterialManager(MaterialManagerPtr manager) noexcept;
	MaterialManagerPtr getMaterialManager() noexcept;

	bool updateBuffer(GraphicsDataPtr& data, void* str, std::size_t cnt) noexcept;
	void* mapBuffer(GraphicsDataPtr& data, std::uint32_t access) noexcept;
	void unmapBuffer(GraphicsDataPtr& data) noexcept;

	void setRenderBuffer(RenderBufferPtr buffer) noexcept;
	void drawRenderBuffer(const GraphicsIndirect& renderable) noexcept;
	void drawRenderBuffer(const GraphicsIndirect renderable[], std::size_t first, std::size_t count) noexcept;

	void addPostProcess(RenderPostProcessPtr postprocess) noexcept;
	void removePostProcess(RenderPostProcessPtr postprocess) noexcept;

	void drawCone(MaterialPassPtr pass, const float4x4& transform) noexcept;
	void drawSphere(MaterialPassPtr pass, const float4x4& transform) noexcept;
	void drawScreenQuad(MaterialPassPtr pass) noexcept;
	void drawMesh(MaterialPassPtr pass, RenderBufferPtr mesh, const GraphicsIndirect& renderable) noexcept;
	void drawRenderQueue(RenderQueue type, RenderPass pass, MaterialPassPtr material = nullptr, GraphicsFramebufferPtr target = nullptr) noexcept;
	void drawPostProcess(RenderQueue queue, GraphicsTexturePtr source, GraphicsFramebufferPtr swap, GraphicsFramebufferPtr dest) noexcept;

protected:

	virtual void onRenderObjectPre(RenderObject& object, RenderQueue queue, RenderPass type, MaterialPassPtr pass) noexcept;
	virtual void onRenderObject(RenderObject& object, RenderQueue queue, RenderPass type, MaterialPassPtr pass) noexcept;
	virtual void onRenderObjectPost(RenderObject& object, RenderQueue queue, RenderPass type, MaterialPassPtr pass) noexcept;

private:
	RenderPipeline(const RenderPipeline&) noexcept = delete;
	RenderPipeline& operator=(const RenderPipeline&) noexcept = delete;

private:

	std::uint32_t _width;
	std::uint32_t _height;

	CameraPtr _camera;

	GraphicsDevicePtr _graphicsDevice;
	GraphicsSwapchainPtr _graphicsSwapchain;
	GraphicsContextPtr _graphicsContext;

	RenderBufferPtr _renderScreenQuad;
	RenderBufferPtr _renderSphere;
	RenderBufferPtr _renderCone;

	GraphicsIndirect _renderConeIndirect;
	GraphicsIndirect _renderSphereIndirect;
	GraphicsIndirect _renderScreenQuadIndirect;

	MaterialManagerPtr _materialManager;

	MaterialVariantPtr _materialMatModel;
	MaterialVariantPtr _materialMatModelInverse;
	MaterialVariantPtr _materialMatModelInverseTranspose;
	MaterialVariantPtr _materialMatProject;
	MaterialVariantPtr _materialMatProjectInverse;
	MaterialVariantPtr _materialMatView;
	MaterialVariantPtr _materialMatViewInverse;
	MaterialVariantPtr _materialMatViewInverseTranspose;
	MaterialVariantPtr _materialMatViewProject;
	MaterialVariantPtr _materialMatViewProjectInverse;
	MaterialVariantPtr _materialCameraAperture;
	MaterialVariantPtr _materialCameraFar;
	MaterialVariantPtr _materialCameraNear;
	MaterialVariantPtr _materialCameraPosition;
	MaterialVariantPtr _materialCameraDirection;

	MaterialPassPtr _materialPass;

	RenderDataManagerPtr _dataManager;
	RenderPostProcessor _postprocessors[RenderQueue::RenderQueueRangeSize];
};

_NAME_END

#endif