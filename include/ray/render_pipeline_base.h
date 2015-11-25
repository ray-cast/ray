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
#ifndef _H_RENDER_PIPELINE_BASE_H_
#define _H_RENDER_PIPELINE_BASE_H_

#include <ray/render_types.h>
#include <ray/render_buffer.h>
#include <ray/material.h>

_NAME_BEGIN

class EXPORT RenderPipeline
{
public:
	RenderPipeline() noexcept;
	virtual ~RenderPipeline() noexcept;

	virtual void open(WindHandle window, std::size_t w, std::size_t h) except = 0;
	virtual void close() noexcept = 0;

	virtual void setCamera(CameraPtr renderer) noexcept = 0;
	virtual CameraPtr getCamera() const noexcept = 0;

	virtual void setWireframeMode(bool enable) noexcept = 0;
	virtual bool getWireframeMode() const noexcept = 0;

	virtual void setSwapInterval(SwapInterval interval) except = 0;
	virtual SwapInterval getSwapInterval() const noexcept = 0;

	virtual RenderStatePtr createRenderState() noexcept = 0;
	virtual void setRenderState(RenderStatePtr state) noexcept = 0;
	virtual RenderStatePtr getRenderState() const noexcept = 0;

	virtual void setWindowResolution(std::size_t w, std::size_t h) noexcept = 0;
	virtual void getWindowResolution(std::size_t& w, std::size_t& h) const noexcept = 0;

	virtual void addRenderData(RenderQueue queue, RenderPass pass, RenderObjectPtr object) noexcept = 0;
	virtual RenderObjects& getRenderData(RenderQueue queue, RenderPass pass) noexcept = 0;

	virtual RenderTexturePtr createRenderTexture() noexcept = 0;
	virtual MultiRenderTexturePtr createMultiRenderTexture() noexcept = 0;
	virtual void setRenderTexture(RenderTexturePtr target) noexcept = 0;
	virtual void setMultiRenderTexture(MultiRenderTexturePtr target) noexcept = 0;
	virtual void setRenderTextureLayer(RenderTexturePtr target, int layer) noexcept = 0;
	virtual void clearRenderTexture(ClearFlags flags, const Vector4& color, float depth, std::int32_t stencil) noexcept = 0;
	virtual void clearRenderTexture(ClearFlags flags, const Vector4& color, float depth, std::int32_t stencil, std::size_t i) noexcept = 0;
	virtual void discradRenderTexture() noexcept = 0;
	virtual void readRenderTexture(RenderTexturePtr target, TextureFormat pfd, std::size_t w, std::size_t h, void* data) noexcept = 0;
	virtual void copyRenderTexture(RenderTexturePtr srcTarget, const Viewport& src, RenderTexturePtr destTarget, const Viewport& dest) noexcept = 0;

	virtual TexturePtr createTexture() noexcept = 0;
	virtual TexturePtr createTexture(const std::string& name) except = 0;

	virtual MaterialPtr createMaterial(const std::string& name) except = 0;
	virtual void setMaterialPass(MaterialPassPtr pass) noexcept = 0;
	virtual MaterialPassPtr getMaterialPass() const noexcept = 0;

	virtual ShaderPtr createShader() noexcept = 0;
	virtual ShaderObjectPtr createShaderObject() noexcept = 0;

	virtual RenderBufferPtr createRenderBuffer() noexcept = 0;
	virtual RenderBufferPtr createRenderBuffer(const MeshProperty& mesh) except = 0;
	virtual RenderBufferPtr createRenderBuffer(const MeshPropertys& mesh) except = 0;

	virtual void setRenderBuffer(RenderBufferPtr buffer) except = 0;
	virtual void drawRenderBuffer(const RenderIndirect& renderable) except = 0;
	virtual void drawRenderBuffer(const RenderIndirects& renderable) except = 0;
	virtual RenderBufferPtr getRenderBuffer() const noexcept = 0;

	virtual void drawCone() noexcept = 0;
	virtual void drawSphere() noexcept = 0;
	virtual void drawSceneQuad(MaterialPassPtr) noexcept = 0;
	virtual void drawRenderIndirect(RenderQueue type, RenderPass pass, MaterialPassPtr material = nullptr, RenderTexturePtr target = nullptr) noexcept = 0;

	virtual void drawMesh(RenderBufferPtr mesh, const RenderIndirect& renderable) noexcept = 0;
	virtual void updateMesh(RenderBufferPtr mesh, VertexBufferDataPtr vb, IndexBufferDataPtr ib) noexcept = 0;

	virtual void addPostProcess(RenderPostProcessPtr postprocess) except = 0;
	virtual void removePostProcess(RenderPostProcessPtr postprocess) noexcept = 0;
	virtual void renderPostProcess(RenderTexturePtr renderTexture) except = 0;

	virtual void renderBegin() noexcept = 0;
	virtual void render(const RenderScene& scene) noexcept = 0;
	virtual void renderEnd() noexcept = 0;

private:
	RenderPipeline(const RenderPipeline&) = delete;
	RenderPipeline& operator=(const RenderPipeline&) = delete;
};


_NAME_END

#endif