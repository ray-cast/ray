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

	bool setup(RenderPipelineDevicePtr pipelineDevice, WindHandle window, std::uint32_t w, std::uint32_t h, GraphicsSwapInterval interval) noexcept;
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

	void setScissor(const Scissor& scissor) noexcept;
	const Scissor& getScissor() const noexcept;

	void addRenderData(RenderQueue queue, RenderObjectPtr object) noexcept;
	const RenderObjects& getRenderData(RenderQueue queue) const noexcept;

	void setTransform(const float4x4& transform) noexcept;
	void setTransformInverse(const float4x4& transform) noexcept;
	void setTransformInverseTranspose(const float4x4& transform) noexcept;

	void setFramebuffer(GraphicsFramebufferPtr target) noexcept;
	void clearFramebuffer(GraphicsClearFlags flags, const float4& color, float depth, std::int32_t stencil) noexcept;
	void discradRenderTexture() noexcept;
	void readFramebuffer(GraphicsFramebufferPtr target, GraphicsFormat pfd, std::size_t w, std::size_t h, std::size_t bufsize, void* data) noexcept;
	void blitFramebuffer(GraphicsFramebufferPtr srcTarget, const Viewport& src, GraphicsFramebufferPtr destTarget, const Viewport& dest) noexcept;

	bool updateBuffer(GraphicsDataPtr data, void* str, std::size_t cnt) noexcept;
	void* mapBuffer(GraphicsDataPtr data, std::uint32_t access) noexcept;
	void unmapBuffer(GraphicsDataPtr data) noexcept;

	void drawCone(MaterialTechPtr tech, std::uint32_t layer = 0) noexcept;
	void drawSphere(MaterialTechPtr tech, std::uint32_t layer = 0) noexcept;
	void drawMesh(MaterialTechPtr tech, RenderMeshPtr mesh, const GraphicsIndirect& renderable) noexcept;
	void drawMeshLayer(MaterialTechPtr tech, RenderMeshPtr mesh, const GraphicsIndirect& renderable, std::uint32_t layer) noexcept;
	void drawScreenQuad(MaterialTechPtr tech) noexcept;
	void drawScreenQuadLayer(MaterialTechPtr tech, std::uint32_t layer) noexcept;
	void drawRenderQueue(RenderQueue queue, MaterialTechPtr tech = nullptr) noexcept;

	void addPostProcess(RenderPostProcessPtr postprocess) noexcept;
	void removePostProcess(RenderPostProcessPtr postprocess) noexcept;
	void drawPostProcess(RenderQueue queue, GraphicsFramebufferPtr source, GraphicsFramebufferPtr swap) noexcept;
	void destroyPostProcess() noexcept;

	void present() noexcept;

	bool isTextureSupport(GraphicsFormat format) noexcept;
	bool isTextureDimSupport(GraphicsTextureDim dimension) noexcept;
	bool isVertexSupport(GraphicsFormat format) noexcept;
	bool isShaderSupport(GraphicsShaderStage stage) noexcept;

	GraphicsDataPtr createGraphicsData(const GraphicsDataDesc& desc) noexcept;
	GraphicsInputLayoutPtr createInputLayout(const GraphicsInputLayoutDesc& desc) noexcept;
	GraphicsTexturePtr createTexture(const std::string& name, GraphicsTextureDim dim, GraphicsSamplerFilter filter = GraphicsSamplerFilter::GraphicsSamplerFilterLinear) noexcept;
	GraphicsTexturePtr createTexture(const GraphicsTextureDesc& desc) noexcept;
	GraphicsTexturePtr createTexture(std::uint32_t w, std::uint32_t h, GraphicsTextureDim dim, GraphicsFormat format) noexcept;
	GraphicsFramebufferPtr createFramebuffer(const GraphicsFramebufferDesc& desc) noexcept;
	GraphicsFramebufferLayoutPtr createFramebufferLayout(const GraphicsFramebufferLayoutDesc& desc) noexcept;

	MaterialPtr createMaterial(const std::string& name) noexcept;
	void destroyMaterial(MaterialPtr material) noexcept;

	RenderMeshPtr createRenderMesh(GraphicsDataPtr vb, GraphicsDataPtr ib) noexcept;
	RenderMeshPtr createRenderMesh(const MeshProperty& mesh, ModelMakerFlags flags) noexcept;
	RenderMeshPtr createRenderMesh(const MeshPropertys& mesh, ModelMakerFlags flags) noexcept;

	MaterialParamPtr createSemantic(const std::string& name, GraphicsUniformType type) noexcept;
	void destroySemantic(MaterialParamPtr semantic) const noexcept;
	MaterialParamPtr getSemantic(const std::string& semantic) const noexcept;

private:
	bool setupDeviceContext(WindHandle window, std::uint32_t w, std::uint32_t h, GraphicsSwapInterval interval) noexcept;
	bool setupMaterialSemantic() noexcept;
	bool setupBaseMeshes() noexcept;

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

	RenderMeshPtr _renderScreenQuad;
	RenderMeshPtr _renderSphere;
	RenderMeshPtr _renderCone;

	GraphicsIndirect _renderConeIndirect;
	GraphicsIndirect _renderSphereIndirect;
	GraphicsIndirect _renderScreenQuadIndirect;

	MaterialParamPtr _materialMatModel;
	MaterialParamPtr _materialMatModelInverse;
	MaterialParamPtr _materialMatModelInverseTranspose;
	MaterialParamPtr _materialMatProject;
	MaterialParamPtr _materialMatProjectInverse;
	MaterialParamPtr _materialMatView;
	MaterialParamPtr _materialMatViewInverse;
	MaterialParamPtr _materialMatViewInverseTranspose;
	MaterialParamPtr _materialMatViewProject;
	MaterialParamPtr _materialMatViewProjectAdj;
	MaterialParamPtr _materialMatViewProjectInverse;
	MaterialParamPtr _materialMatModelView;
	MaterialParamPtr _materialMatModelViewProject;
	MaterialParamPtr _materialCameraAperture;
	MaterialParamPtr _materialCameraFar;
	MaterialParamPtr _materialCameraNear;
	MaterialParamPtr _materialCameraPosition;
	MaterialParamPtr _materialCameraDirection;
	MaterialParamPtr _materialDepthMap;
	MaterialParamPtr _materialColorMap;
	MaterialParamPtr _materialNormalMap;
	MaterialParamPtr _materialDeferredDepthMap;
	MaterialParamPtr _materialDeferredDepthLinearMap;
	MaterialParamPtr _materialDeferredGraphicMap;
	MaterialParamPtr _materialDeferredNormalMap;
	MaterialParamPtr _materialDeferredLightMap;
	MaterialParamPtr _materialDeferredShadowMap;

	RenderDataManagerPtr _dataManager;

	RenderPostProcessor _postprocessors[RenderQueue::RenderQueueRangeSize];
};

_NAME_END

#endif