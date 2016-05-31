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
#ifndef _H_RENDER_SYSTEM_H_
#define _H_RENDER_SYSTEM_H_

#include <ray/render_setting.h>

_NAME_BEGIN

class EXPORT RenderSystem final
{
	__DeclareSingleton(RenderSystem)
public:
	RenderSystem() noexcept;
	~RenderSystem() noexcept;

	bool setup(const RenderSetting& setting) noexcept;
	void close() noexcept;

	void setRenderSetting(const RenderSetting& setting) noexcept;
	const RenderSetting& getRenderSetting() const noexcept;

	void setWindowResolution(std::uint32_t w, std::uint32_t h) noexcept;
	void getWindowResolution(std::uint32_t& w, std::uint32_t& h) const noexcept;

	RenderScenePtr createRenderScene() noexcept;
	void removeRenderScene(RenderScenePtr scene) noexcept;

	void setViewport(const Viewport& view) noexcept;
	const Viewport& getViewport() const noexcept;

	void setScissor(const Scissor& scissor) noexcept;
	const Scissor& getScissor() const noexcept;

	void setFramebuffer(GraphicsFramebufferPtr target) noexcept;
	void clearFramebuffer(std::uint32_t i, GraphicsClearFlags flags, const float4& color, float depth, std::int32_t stencil) noexcept;

	void setMaterialPass(const MaterialPassPtr& pass) noexcept;
	void setVertexBuffer(GraphicsDataPtr vbo) noexcept;
	void setIndexBuffer(GraphicsDataPtr ibo) noexcept;

	void drawCone(const MaterialTech& tech) noexcept;
	void drawSphere(const MaterialTech& tech) noexcept;
	void drawScreenQuad(const MaterialTech& tech) noexcept;
	void drawMesh(const GraphicsIndirect& renderable) noexcept;
	void drawArray(std::uint32_t numVertices, std::uint32_t numInstances, std::uint32_t firstVertex, std::uint32_t firstInstance) noexcept;

	bool isTextureSupport(GraphicsFormat format) noexcept;
	bool isTextureDimSupport(GraphicsTextureDim dimension) noexcept;
	bool isVertexSupport(GraphicsFormat format) noexcept;
	bool isShaderSupport(GraphicsShaderStageFlagBits stage) noexcept;

	GraphicsTexturePtr createTexture(const GraphicsTextureDesc& desc) noexcept;
	GraphicsTexturePtr createTexture(const std::string& name, GraphicsTextureDim dim, GraphicsSamplerFilter filter = GraphicsSamplerFilter::GraphicsSamplerFilterLinear) noexcept;
	GraphicsTexturePtr createTexture(std::uint32_t w, std::uint32_t h, GraphicsTextureDim dim, GraphicsFormat format, GraphicsSamplerFilter filter = GraphicsSamplerFilter::GraphicsSamplerFilterLinear) noexcept;

	GraphicsDataPtr createGraphicsData(const GraphicsDataDesc& desc) noexcept;
	GraphicsDataPtr createVertexBuffer(const MeshProperty& mesh, ModelMakerFlags flags) noexcept;
	GraphicsDataPtr createIndexBuffer(const MeshProperty& mesh) noexcept;

	GraphicsInputLayoutPtr createInputLayout(const GraphicsInputLayoutDesc& desc) noexcept;
	GraphicsFramebufferPtr createFramebuffer(const GraphicsFramebufferDesc& desc) noexcept;
	GraphicsFramebufferLayoutPtr createFramebufferLayout(const GraphicsFramebufferLayoutDesc& desc) noexcept;
	GraphicsPipelinePtr createGraphicsPipeline(const GraphicsPipelineDesc& desc) noexcept;
	MaterialPtr createMaterial(const std::string& name) noexcept;

	void renderBegin() noexcept;
	void render() noexcept;
	void renderEnd() noexcept;

private:
	RenderSystem(const RenderSystem&) noexcept = delete;
	RenderSystem& operator=(const RenderSystem&) noexcept = delete;

private:
	RenderScenes _sceneList;
	RenderPipelineManagerPtr _pipelineManager;
};

_NAME_END

#endif