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

_NAME_BEGIN

class EXPORT RenderPipeline : public rtti::Interface
{
	__DeclareSubClass(RenderPipeline, rtti::Interface)
public:
	RenderPipeline() noexcept;
	~RenderPipeline() noexcept;

	bool setup(RenderPipelineDevicePtr pipelineDevice, WindHandle window, std::uint32_t w, std::uint32_t h, std::uint32_t dpi_w, std::uint32_t dpi_h, GraphicsSwapInterval interval) noexcept;
	void close() noexcept;

	void renderBegin() noexcept;
	void renderEnd() noexcept;

	void setWindowResolution(std::uint32_t w, std::uint32_t h) noexcept;
	void getWindowResolution(std::uint32_t& w, std::uint32_t& h) const noexcept;

	void setWindowResolutionDPI(std::uint32_t w, std::uint32_t h) noexcept;
	void getWindowResolutionDPI(std::uint32_t& w, std::uint32_t& h) const noexcept;

	GraphicsDeviceType getDeviceType() const noexcept;

	void setSwapInterval(GraphicsSwapInterval interval) noexcept;
	GraphicsSwapInterval getSwapInterval() const noexcept;

	void setCamera(CameraPtr camera) noexcept;
	CameraPtr getCamera() const noexcept;

	void setViewport(std::uint32_t i, const Viewport& view) noexcept;
	const Viewport& getViewport(std::uint32_t i) const noexcept;

	void setScissor(std::uint32_t i, const Scissor& scissor) noexcept;
	const Scissor& getScissor(std::uint32_t i) const noexcept;

	void setFramebuffer(GraphicsFramebufferPtr target) noexcept;
	void clearFramebuffer(std::uint32_t i, GraphicsClearFlags flags, const float4& color, float depth, std::int32_t stencil) noexcept;
	void discardFramebuffer(std::uint32_t i) noexcept;

	void setMaterialPass(const MaterialPassPtr& pass) noexcept;
	void setVertexBuffer(std::uint32_t i, GraphicsDataPtr vbo, std::intptr_t offset) noexcept;
	void setIndexBuffer(GraphicsDataPtr ibo, std::intptr_t offset, GraphicsIndexType indexType) noexcept;

	void generateMipmap(GraphicsTexturePtr texture);

	void drawCone(const MaterialTech& tech, std::uint32_t layer = 0) noexcept;
	void drawSphere(const MaterialTech& tech, std::uint32_t layer = 0) noexcept;
	void drawScreenQuad(const MaterialTech& tech, std::uint32_t instanceCount = 1) noexcept;
	void drawScreenQuadLayer(const MaterialTech& tech, std::uint32_t layer, std::uint32_t instanceCount = 1) noexcept;
	
	void draw(std::uint32_t numVertices, std::uint32_t numInstances, std::uint32_t startVertice, std::uint32_t startInstances) noexcept;
	void drawIndexed(std::uint32_t numIndices, std::uint32_t numInstances, std::uint32_t startIndice, std::uint32_t startVertice, std::uint32_t startInstances) noexcept;

	void drawLayer(std::uint32_t numVertices, std::uint32_t numInstances, std::uint32_t startVertice, std::uint32_t startInstances, std::uint32_t layer) noexcept;
	void drawIndexedLayer(std::uint32_t numIndices, std::uint32_t numInstances, std::uint32_t startIndice, std::uint32_t startVertice, std::uint32_t startInstances, std::uint32_t layer) noexcept;

	void drawRenderQueue(RenderQueue queue) noexcept;
	void drawRenderQueue(RenderQueue queue, const MaterialTechPtr& tech) noexcept;

	void addPostProcess(RenderPostProcessPtr& postprocess) noexcept;
	void removePostProcess(RenderPostProcessPtr& postprocess) noexcept;
	bool drawPostProcess(RenderQueue queue, GraphicsFramebufferPtr& source, GraphicsFramebufferPtr swap) noexcept;
	void destroyPostProcess() noexcept;

	void present() noexcept;

	bool isTextureSupport(GraphicsFormat format) noexcept;
	bool isTextureDimSupport(GraphicsTextureDim dimension) noexcept;
	bool isVertexSupport(GraphicsFormat format) noexcept;
	bool isShaderSupport(GraphicsShaderStageFlagBits stage) noexcept;

	void setTransform(const float4x4& transform) noexcept;
	void setTransformInverse(const float4x4& transform) noexcept;

	const MaterialSemanticPtr& getSemanticParam(GlobalSemanticType type) const noexcept;

	GraphicsDataPtr createGraphicsData(const GraphicsDataDesc& desc) noexcept;
	GraphicsDataPtr createVertexBuffer(const MeshProperty& mesh, ModelMakerFlags flags) noexcept;
	GraphicsDataPtr createIndexBuffer(const MeshProperty& mesh) noexcept;

	GraphicsInputLayoutPtr createInputLayout(const GraphicsInputLayoutDesc& desc) noexcept;

	GraphicsTexturePtr createTexture(const GraphicsTextureDesc& desc) noexcept;
	GraphicsTexturePtr createTexture(std::uint32_t w, std::uint32_t h, GraphicsTextureDim dim, GraphicsFormat format, GraphicsSamplerFilter filter = GraphicsSamplerFilter::GraphicsSamplerFilterLinear, GraphicsSamplerWrap wrap = GraphicsSamplerWrap::GraphicsSamplerWrapRepeat) noexcept;
	GraphicsFramebufferPtr createFramebuffer(const GraphicsFramebufferDesc& desc) noexcept;
	GraphicsFramebufferLayoutPtr createFramebufferLayout(const GraphicsFramebufferLayoutDesc& desc) noexcept;

	MaterialPtr createMaterial(const std::string& name) noexcept;
	void destroyMaterial(MaterialPtr material) noexcept;

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

	std::uint32_t _dpi_w;
	std::uint32_t _dpi_h;

	RenderPipelineDevicePtr _pipelineDevice;
	RenderPipelineManagerPtr _pipelineManager;

	GraphicsSwapchainPtr _graphicsSwapchain;
	GraphicsContextPtr _graphicsContext;

	CameraPtr _camera;

	GraphicsDataPtr _screenQuadVbo;
	GraphicsDataPtr _screenQuadIbo;
	std::uint32_t _numScreenQuadFace;

	GraphicsDataPtr _sphereVbo;
	GraphicsDataPtr _sphereIbo;
	std::uint32_t _numSphereFace;

	GraphicsDataPtr _coneVbo;
	GraphicsDataPtr _coneIbo;
	std::uint32_t _numConeFace;	

	MaterialSemanticManagerPtr _semanticsManager;

	RenderDataManagerPtr _dataManager;

	RenderPostProcessor _postprocessors;
};

_NAME_END

#endif