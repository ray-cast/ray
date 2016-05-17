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
#ifndef _H_RENDER_PIPELINE_MANAGER_H_
#define _H_RENDER_PIPELINE_MANAGER_H_

#include <ray/render_setting.h>

_NAME_BEGIN

class RenderPipelineManager final : public rtti::Interface
{
	__DeclareSubClass(RenderPipelineManager, rtti::Interface)
public:
	RenderPipelineManager() noexcept;
	~RenderPipelineManager() noexcept;

	bool setup(const RenderSetting& setting) noexcept;
	void close() noexcept;

	void setWindowResolution(std::uint32_t w, std::uint32_t h) noexcept;
	void getWindowResolution(std::uint32_t& w, std::uint32_t& h) const noexcept;

	void renderBegin() noexcept;
	void renderEnd() noexcept;

	bool setRenderSetting(const RenderSetting& setting) noexcept;
	const RenderSetting& getRenderSetting() const noexcept;

	void setCamera(CameraPtr renderer) noexcept;
	CameraPtr getCamera() const noexcept;

	void setViewport(const Viewport& view) noexcept;
	const Viewport& getViewport() const noexcept;

	void setScissor(const Scissor& scissor) noexcept;
	const Scissor& getScissor() const noexcept;

	void setTransform(const float4x4& transform) noexcept;
	void setTransformInverse(const float4x4& transform) noexcept;

	void setFramebuffer(GraphicsFramebufferPtr target) noexcept;
	void clearFramebuffer(GraphicsClearFlags flags, const float4& color, float depth, std::int32_t stencil) noexcept;
	void discradRenderTexture() noexcept;
	void blitFramebuffer(GraphicsFramebufferPtr srcTarget, const Viewport& src, GraphicsFramebufferPtr destTarget, const Viewport& dest) noexcept;

	void setMaterialPass(const MaterialPassPtr& pass) noexcept;
	void setVertexBuffer(GraphicsDataPtr vbo) noexcept;
	void setIndexBuffer(GraphicsDataPtr ibo) noexcept;

	void drawCone(const MaterialTech& tech) noexcept;
	void drawSphere(const MaterialTech& tech) noexcept;
	void drawScreenQuad(const MaterialTech& tech) noexcept;
	void drawMesh(const GraphicsIndirect& renderable) noexcept;

	void addPostProcess(RenderPostProcessPtr postprocess) noexcept;
	void removePostProcess(RenderPostProcessPtr postprocess) noexcept;
	void destroyPostProcess() noexcept;

	bool isTextureSupport(GraphicsFormat format) noexcept;
	bool isTextureDimSupport(GraphicsTextureDim dimension) noexcept;
	bool isVertexSupport(GraphicsFormat format) noexcept;
	bool isShaderSupport(GraphicsShaderStage stage) noexcept;

	GraphicsSwapchainPtr createSwapchain(const GraphicsSwapchainDesc& desc) noexcept;
	GraphicsContextPtr createDeviceContext(const GraphicsContextDesc& desc) noexcept;
	GraphicsFramebufferPtr createFramebuffer(const GraphicsFramebufferDesc& desc) noexcept;
	GraphicsFramebufferLayoutPtr createFramebufferLayout(const GraphicsFramebufferLayoutDesc& desc) noexcept;
	GraphicsTexturePtr createTexture(const GraphicsTextureDesc& desc) noexcept;
	GraphicsTexturePtr createTexture(std::uint32_t w, std::uint32_t h, GraphicsTextureDim dim, GraphicsFormat format) noexcept;
	GraphicsTexturePtr createTexture(const std::string& name, GraphicsTextureDim dim, GraphicsSamplerFilter filter) noexcept;

	MaterialPtr createMaterial(const std::string& name) noexcept;
	void destroyMaterial(MaterialPtr material) noexcept;

	GraphicsInputLayoutPtr createInputLayout(const GraphicsInputLayoutDesc& desc) noexcept;
	GraphicsPipelinePtr createGraphicsPipeline(const GraphicsPipelineDesc& desc) noexcept;

	GraphicsDataPtr createGraphicsData(const GraphicsDataDesc& desc) noexcept;
	GraphicsDataPtr createVertexBuffer(const MeshProperty& mesh, ModelMakerFlags flags) noexcept;
	GraphicsDataPtr createIndexBuffer(const MeshProperty& mesh) noexcept;

	void render(const RenderScene& scene) noexcept;

private:
	bool setupShadowRenderer(RenderPipelinePtr pipeline) noexcept;
	void destroyShadowRenderer() noexcept;

private:
	RenderPipelineManager(const RenderPipelineManager&) noexcept = delete;
	RenderPipelineManager& operator = (const RenderPipelineManager&) noexcept = delete;

private:
	RenderSetting _setting;
	RenderPostProcessPtr _SSGI;
	RenderPostProcessPtr _SSAO;
	RenderPostProcessPtr _atmospheric;
	RenderPostProcessPtr _SSR;
	RenderPostProcessPtr _SSSS;
	RenderPostProcessPtr _DOF;
	RenderPostProcessPtr _fog;
	RenderPostProcessPtr _lightShaft;
	RenderPostProcessPtr _fimicToneMapping;
	RenderPostProcessPtr _FXAA;
	RenderPostProcessPtr _colorGrading;

	RenderPipelinePtr _pipeline;
	RenderPipelineDevicePtr _pipelineDevice;
	RenderPipelineControllerPtr _deferredLighting;
	RenderPipelineControllerPtr _forwardShading;
	RenderPipelineControllerPtr _shadowMapGen;
};

_NAME_END

#endif