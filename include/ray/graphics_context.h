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
#ifndef _H_GRAPHICS_DEVICE_CONTEXT_H_
#define _H_GRAPHICS_DEVICE_CONTEXT_H_

#include <ray/graphics_child.h>

_NAME_BEGIN

class GraphicsContext : public GraphicsChild
{
	__DeclareSubInterface(GraphicsContext, GraphicsChild)
public:
	GraphicsContext() noexcept;
	virtual ~GraphicsContext() noexcept;

	virtual void renderBegin() noexcept = 0;
	virtual void renderEnd() noexcept = 0;

	virtual void setWireframeMode(bool enable) noexcept = 0;
	virtual bool getWireframeMode() const noexcept = 0;

	virtual void setViewport(const Viewport& viewport, std::size_t i = 0) noexcept = 0;
	virtual const Viewport& getViewport(std::size_t i = 0) const noexcept = 0;

	virtual void setSwapInterval(SwapInterval interval) noexcept = 0;
	virtual SwapInterval getSwapInterval() const noexcept = 0;

	virtual void setGraphicsLayout(GraphicsLayoutPtr data) noexcept = 0;
	virtual GraphicsLayoutPtr getGraphicsLayout() const noexcept = 0;

	virtual bool updateBuffer(GraphicsDataPtr& data, void* str, std::size_t cnt) noexcept = 0;
	virtual void* mapBuffer(GraphicsDataPtr& data, std::uint32_t access) noexcept = 0;
	virtual void unmapBuffer(GraphicsDataPtr& data) noexcept = 0;

	virtual void setIndexBufferData(GraphicsDataPtr data) noexcept = 0;
	virtual GraphicsDataPtr getIndexBufferData() const noexcept = 0;

	virtual void setVertexBufferData(GraphicsDataPtr data) noexcept = 0;
	virtual GraphicsDataPtr getVertexBufferData() const noexcept = 0;

	virtual void setTexture(TexturePtr texture, std::uint32_t slot) noexcept = 0;
	virtual void setGraphicsSampler(GraphicsSamplerPtr sampler, std::uint32_t slot) noexcept = 0;

	virtual void setRenderTexture(RenderTexturePtr target) noexcept = 0;
	virtual void setRenderTextureLayer(RenderTexturePtr target, std::int32_t layer) noexcept = 0;
	virtual void setMultiRenderTexture(MultiRenderTexturePtr target) noexcept = 0;
	virtual void clearRenderTexture(ClearFlags flags, const Vector4& color, float depth, std::int32_t stencil) noexcept = 0;
	virtual void clearRenderTexture(ClearFlags flags, const Vector4& color, float depth, std::int32_t stencil, std::size_t i) noexcept = 0;
	virtual void discardRenderTexture() noexcept = 0;
	virtual void blitRenderTexture(RenderTexturePtr src, const Viewport& v1, RenderTexturePtr dest, const Viewport& v2) noexcept = 0;
	virtual void readRenderTexture(RenderTexturePtr source, TextureFormat pfd, std::size_t w, std::size_t h, void* data) noexcept = 0;
	virtual RenderTexturePtr getRenderTexture() const noexcept = 0;
	virtual MultiRenderTexturePtr getMultiRenderTexture() const noexcept = 0;

	virtual void setGraphicsState(GraphicsStatePtr state) noexcept = 0;
	virtual GraphicsStatePtr getGraphicsState() const noexcept = 0;

	virtual void setShaderObject(ShaderObjectPtr shader) noexcept = 0;
	virtual ShaderObjectPtr getShaderObject() const noexcept = 0;

	virtual void drawRenderBuffer(const RenderIndirect& renderable) noexcept = 0;
	virtual void drawRenderBuffer(const RenderIndirects& renderable) noexcept = 0;

	virtual void present() noexcept = 0;

private:
	GraphicsContext(const GraphicsContext&) noexcept = delete;
	GraphicsContext& operator=(const GraphicsContext&) noexcept = delete;
};

_NAME_END

#endif