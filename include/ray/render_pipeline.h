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

#include <ray/render_types.h>
#include <ray/graphics_context.h>

_NAME_BEGIN

class EXPORT RenderPipeline : public rtti::Interface
{
	__DeclareSubClass(RenderPipeline, rtti::Interface)
public:
	RenderPipeline() noexcept;
	~RenderPipeline() noexcept;

	bool setup(RenderPipelineDevicePtr pipelineDevice, WindHandle window, std::uint32_t w, std::uint32_t h) noexcept;
	void close() noexcept;

	void renderBegin() noexcept;
	void renderEnd() noexcept;

	void setWindowResolution(std::uint32_t w, std::uint32_t h) noexcept;
	void getWindowResolution(std::uint32_t& w, std::uint32_t& h) const noexcept;

	void setSwapInterval(GraphicsSwapInterval interval) noexcept;
	GraphicsSwapInterval getSwapInterval() const noexcept;

	void setCamera(CameraPtr camera) noexcept;
	CameraPtr getCamera() const noexcept;

	void setViewport(const Viewport& view) noexcept;
	const Viewport& getViewport() const noexcept;

	void addRenderData(RenderQueue queue, RenderPass pass, RenderObjectPtr object) noexcept;
	const RenderObjects& getRenderData(RenderQueue queue, RenderPass pass) const noexcept;

	void setTransform(const float4x4& transform) noexcept;
	void setTransformInverse(const float4x4& transform) noexcept;
	void setTransformInverseTranspose(const float4x4& transform) noexcept;

	void setFramebuffer(GraphicsFramebufferPtr target) noexcept;
	void clearFramebuffer(GraphicsClearFlags flags, const float4& color, float depth, std::int32_t stencil) noexcept;
	void discradRenderTexture() noexcept;
	void readFramebuffer(GraphicsFramebufferPtr target, GraphicsFormat pfd, std::size_t w, std::size_t h, void* data) noexcept;
	void blitFramebuffer(GraphicsFramebufferPtr srcTarget, const Viewport& src, GraphicsFramebufferPtr destTarget, const Viewport& dest) noexcept;

	bool updateBuffer(GraphicsDataPtr& data, void* str, std::size_t cnt) noexcept;
	void* mapBuffer(GraphicsDataPtr& data, std::uint32_t access) noexcept;
	void unmapBuffer(GraphicsDataPtr& data) noexcept;
	
	void drawCone(MaterialPassPtr pass) noexcept;
	void drawSphere(MaterialPassPtr pass) noexcept;
	void drawMesh(MaterialPassPtr pass, RenderBufferPtr mesh, const GraphicsIndirect& renderable) noexcept;
	void drawScreenQuad(MaterialPassPtr pass) noexcept;
	void drawRenderQueue(RenderQueue type, RenderPass pass, MaterialPassPtr material = nullptr) noexcept;
	void drawPostProcess(RenderQueue queue, GraphicsFramebufferPtr source, GraphicsFramebufferPtr swap, GraphicsFramebufferPtr dest) noexcept;

	void addPostProcess(RenderPostProcessPtr postprocess) noexcept;
	void removePostProcess(RenderPostProcessPtr postprocess) noexcept;
	void destroyPostProcess() noexcept;

	void present() noexcept;

	GraphicsDataPtr createGraphicsData(const GraphicsDataDesc& desc) noexcept;
	GraphicsInputLayoutPtr createInputLayout(const GraphicsInputLayoutDesc& desc) noexcept;
	GraphicsTexturePtr createTexture(const std::string& name) noexcept;
	GraphicsTexturePtr createTexture(const GraphicsTextureDesc& desc) noexcept;
	GraphicsTexturePtr createTexture(std::uint32_t w, std::uint32_t h, GraphicsTextureDim dim, GraphicsFormat format) noexcept;
	GraphicsFramebufferPtr createFramebuffer(const GraphicsFramebufferDesc& desc) noexcept;
	GraphicsFramebufferLayoutPtr createFramebufferLayout(const GraphicsFramebufferLayoutDesc& desc) noexcept;

	MaterialPtr createMaterial(const std::string& name) noexcept;
	RenderBufferPtr createRenderBuffer(GraphicsDataPtr vb, GraphicsDataPtr ib) noexcept;
	RenderBufferPtr createRenderBuffer(const MeshProperty& mesh) noexcept;
	RenderBufferPtr createRenderBuffer(const MeshPropertys& mesh) noexcept;

	MaterialVariantPtr createSemantic(const std::string& name, GraphicsUniformType type) noexcept;
	void destroySemantic(MaterialVariantPtr semantic) const noexcept;
	MaterialVariantPtr getSemantic(const std::string& semantic) const noexcept;

private:
	bool setupDeviceContext(WindHandle window, std::uint32_t w, std::uint32_t h) noexcept;
	bool setupMaterialSemantic() noexcept;
	bool setupBaseMeshes() noexcept;
	bool setupDataManager() noexcept;

	void destroyDeviceContext() noexcept;
	void destroyMaterialSemantic() noexcept;
	void destroyBaseMeshes() noexcept;
	void destroyDataManager() noexcept;

private:
	RenderPipeline(const RenderPipeline&) = delete;
	RenderPipeline& operator=(const RenderPipeline&) = delete;

private:
	std::uint32_t _width;
	std::uint32_t _height;

	RenderPipelineDevicePtr _pipelineDevice;

	GraphicsSwapchainPtr _graphicsSwapchain;
	GraphicsContextPtr _graphicsContext;

	CameraPtr _camera;

	RenderBufferPtr _renderScreenQuad;
	RenderBufferPtr _renderSphere;
	RenderBufferPtr _renderCone;

	GraphicsIndirect _renderConeIndirect;
	GraphicsIndirect _renderSphereIndirect;
	GraphicsIndirect _renderScreenQuadIndirect;

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
	MaterialVariantPtr _materialMatModelView;
	MaterialVariantPtr _materialMatModelViewProject;
	MaterialVariantPtr _materialCameraAperture;
	MaterialVariantPtr _materialCameraFar;
	MaterialVariantPtr _materialCameraNear;
	MaterialVariantPtr _materialCameraPosition;
	MaterialVariantPtr _materialCameraDirection;
	MaterialVariantPtr _materialDepthMap;
	MaterialVariantPtr _materialColorMap;
	MaterialVariantPtr _materialNormalMap;
	MaterialVariantPtr _materialDeferredDepthMap;
	MaterialVariantPtr _materialDeferredDepthLinearMap;
	MaterialVariantPtr _materialDeferredGraphicMap;
	MaterialVariantPtr _materialDeferredNormalMap;
	MaterialVariantPtr _materialDeferredLightMap;
	MaterialVariantPtr _materialDeferredShadowMap;

	RenderDataManagerPtr _dataManager;

	RenderPostProcessor _postprocessors[RenderQueue::RenderQueueRangeSize];
};

_NAME_END

#endif