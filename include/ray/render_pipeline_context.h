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
#ifndef _H_RENDER_PIPELINE_CONTEXT_H_
#define _H_RENDER_PIPELINE_CONTEXT_H_

#include <ray/render_types.h>

_NAME_BEGIN

class RenderPipelineContext
{
public:
	RenderPipelineContext(RenderPipeline& pipeline) noexcept;
	~RenderPipelineContext() noexcept;

	void renderBegin() noexcept;
	void renderEnd() noexcept;

	void setViewport(const Viewport& view, std::size_t i) noexcept;
	const Viewport& getViewport(std::size_t i) const noexcept;

	MaterialPtr createMaterial(const std::string& name) noexcept;
	void setMaterialPass(MaterialPassPtr pass) noexcept;
	MaterialPassPtr getMaterialPass() noexcept;

	void setWindowResolution(std::uint32_t w, std::uint32_t h) noexcept;
	void getWindowResolution(std::uint32_t& w, std::uint32_t& h) const noexcept;

	void addRenderData(RenderQueue queue, RenderPass pass, RenderObjectPtr object) noexcept;
	RenderObjects& getRenderData(RenderQueue queue, RenderPass pass) noexcept;

	GraphicsRenderTexturePtr createRenderTexture(const GraphicsRenderTextureDesc& desc) noexcept;
	void setRenderTexture(GraphicsRenderTexturePtr target) noexcept;
	void clearRenderTexture(GraphicsClearFlags flags, const Vector4& color, float depth, std::int32_t stencil) noexcept;
	void clearRenderTexture(GraphicsClearFlags flags, const Vector4& color, float depth, std::int32_t stencil, std::size_t i) noexcept;
	void discradRenderTexture() noexcept;
	void readRenderTexture(GraphicsRenderTexturePtr target, GraphicsFormat pfd, std::size_t w, std::size_t h, void* data) noexcept;
	void blitRenderTexture(GraphicsRenderTexturePtr srcTarget, const Viewport& src, GraphicsRenderTexturePtr destTarget, const Viewport& dest) noexcept;

	GraphicsTexturePtr createTexture(const std::string& name) noexcept;
	GraphicsTexturePtr createTexture(const GraphicsTextureDesc& desc) noexcept;
	GraphicsTexturePtr createTexture(std::uint32_t w, std::uint32_t h, GraphicsTextureDim dim, GraphicsFormat format) noexcept;

	GraphicsInputLayoutPtr createInputLayout(const GraphicsInputLayoutDesc& desc) noexcept;

	GraphicsDataPtr createGraphicsData(const GraphicsDataDesc& desc) noexcept;
	bool updateBuffer(GraphicsDataPtr& data, void* str, std::size_t cnt) noexcept;
	void* mapBuffer(GraphicsDataPtr& data, std::uint32_t access) noexcept;
	void unmapBuffer(GraphicsDataPtr& data) noexcept;

	RenderBufferPtr createRenderBuffer(GraphicsDataPtr vb, GraphicsDataPtr ib) noexcept;
	RenderBufferPtr createRenderBuffer(const MeshProperty& mesh) noexcept;
	RenderBufferPtr createRenderBuffer(const MeshPropertys& mesh) noexcept;

	void setRenderBuffer(RenderBufferPtr buffer) noexcept;
	void drawRenderBuffer(const GraphicsIndirect& renderable) noexcept;
	void drawRenderBuffer(const GraphicsIndirect renderable[], std::size_t first, std::size_t count) noexcept;
	
	void drawCone(MaterialPassPtr pass, const float4x4& transform) noexcept;
	void drawSphere(MaterialPassPtr pass, const float4x4& transform) noexcept;
	void drawScreenQuad(MaterialPassPtr pass) noexcept;
	void drawMesh(MaterialPassPtr pass, RenderBufferPtr mesh, const GraphicsIndirect& renderable) noexcept;
	void drawRenderQueue(RenderQueue type, RenderPass pass, MaterialPassPtr material = nullptr, GraphicsRenderTexturePtr target = nullptr) noexcept;
	void drawPostProcess(RenderQueue queue, GraphicsTexturePtr source, GraphicsRenderTexturePtr swap, GraphicsRenderTexturePtr dest) noexcept;

	void addPostProcess(RenderPostProcessPtr postprocess) noexcept;
	void removePostProcess(RenderPostProcessPtr postprocess) noexcept;

protected:
	virtual void onRenderObjectPre(RenderObject& object, RenderQueue queue, RenderPass type, MaterialPassPtr pass) noexcept;
	virtual void onRenderObject(RenderObject& object, RenderQueue queue, RenderPass type, MaterialPassPtr pass) noexcept;
	virtual void onRenderObjectPost(RenderObject& object, RenderQueue queue, RenderPass type, MaterialPassPtr pass) noexcept;

private:
	RenderPipelineContext(const RenderPipelineContext&) = delete;
	RenderPipelineContext& operator=(const RenderPipelineContext&) = delete;

private:
	RenderPipeline& _pipeline;
	GraphicsContextPtr _graphicsContext;

	MaterialPassPtr _materialPass;
	RenderDataManagerPtr _dataManager;
};

_NAME_END

#endif