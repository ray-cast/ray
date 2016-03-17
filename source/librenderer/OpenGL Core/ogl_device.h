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
#ifndef _H_OGL_DEVICE_H_
#define _H_OGL_DEVICE_H_

#include "ogl_canvas.h"

_NAME_BEGIN

class OGLDevice final : public GraphicsDevice
{
	__DeclareSubClass(OGLDevice, GraphicsDevice)
public:
	OGLDevice() noexcept;
	virtual ~OGLDevice() noexcept;

	virtual bool open(WindHandle hwnd) noexcept;
	virtual void close() noexcept;

	virtual GraphicsContextPtr createGraphicsContext(WindHandle window) noexcept;
	virtual void setGraphicsContext(GraphicsContextPtr context) noexcept;
	virtual GraphicsContextPtr getGraphicsContext() const noexcept;

	virtual void renderBegin() noexcept;
	virtual void renderEnd() noexcept;

	virtual void setWireframeMode(bool enable) noexcept;
	virtual bool getWireframeMode() const noexcept;

	virtual void setViewport(const Viewport& viewport, std::size_t i) noexcept;
	virtual const Viewport& getViewport(std::size_t i) const noexcept;

	virtual void setSwapInterval(SwapInterval interval) noexcept;
	virtual SwapInterval getSwapInterval() const noexcept;

	virtual GraphicsLayoutPtr createGraphicsLayout(const GraphicsLayoutDesc& desc) noexcept;
	virtual void setGraphicsLayout(GraphicsLayoutPtr data) noexcept;
	virtual GraphicsLayoutPtr getGraphicsLayout() const noexcept;

	virtual GraphicsDataPtr createGraphicsData(const GraphicsDataDesc& desc) noexcept;
	virtual bool updateBuffer(GraphicsDataPtr& data, void* str, std::size_t cnt) noexcept;
	virtual void* mapBuffer(GraphicsDataPtr& data, std::uint32_t access) noexcept;
	virtual void unmapBuffer(GraphicsDataPtr& data) noexcept;

	virtual void setIndexBufferData(GraphicsDataPtr data) noexcept;
	virtual GraphicsDataPtr getIndexBufferData() const noexcept;

	virtual void setVertexBufferData(GraphicsDataPtr data) noexcept;
	virtual GraphicsDataPtr getVertexBufferData() const noexcept;

	virtual GraphicsTexturePtr createGraphicsTexture(const GraphicsTextureDesc& desc) noexcept;
	virtual void setGraphicsTexture(GraphicsTexturePtr texture, std::uint32_t slot) noexcept;
	virtual void setGraphicsTexture(GraphicsTexturePtr texture[], std::uint32_t first, std::uint32_t count) noexcept;

	virtual GraphicsSamplerPtr createGraphicsSampler(const GraphicsSamplerDesc& desc) noexcept;
	virtual void setGraphicsSampler(GraphicsSamplerPtr sampler, std::uint32_t slot) noexcept;
	virtual void setGraphicsSampler(GraphicsSamplerPtr sampler[], std::uint32_t first, std::uint32_t count) noexcept;

	virtual GraphicsRenderTexturePtr createRenderTexture(const GraphicsRenderTextureDesc& desc) noexcept;
	virtual void setRenderTexture(GraphicsRenderTexturePtr target) noexcept;
	virtual void setRenderTextureLayer(GraphicsRenderTexturePtr target, std::int32_t layer) noexcept;
	virtual void clearRenderTexture(GraphicsClearFlags flags, const Vector4& color, float depth, std::int32_t stencil) noexcept;
	virtual void discardRenderTexture() noexcept;
	virtual void blitRenderTexture(GraphicsRenderTexturePtr src, const Viewport& v1, GraphicsRenderTexturePtr dest, const Viewport& v2) noexcept;
	virtual void readRenderTexture(GraphicsRenderTexturePtr source, GraphicsFormat pfd, std::size_t w, std::size_t h, void* data) noexcept;
	virtual GraphicsRenderTexturePtr getRenderTexture() const noexcept;

	virtual GraphicsMultiRenderTexturePtr createMultiRenderTexture(const GraphicsMultiRenderTextureDesc& desc) noexcept;
	virtual void setMultiRenderTexture(GraphicsMultiRenderTexturePtr target) noexcept;
	virtual void clearRenderTexture(GraphicsClearFlags flags, const Vector4& color, float depth, std::int32_t stencil, std::size_t i) noexcept;
	virtual GraphicsMultiRenderTexturePtr getMultiRenderTexture() const noexcept;

	virtual GraphicsStatePtr createGraphicsState(const GraphicsStateDesc& desc) noexcept;
	virtual void setGraphicsState(GraphicsStatePtr state) noexcept;
	virtual GraphicsStatePtr getGraphicsState() const noexcept;

	virtual GraphicsShaderPtr createShader(const ShaderDesc& desc) noexcept;
	virtual GraphicsProgramPtr createShaderProgram(const ShaderObjectDesc& desc) noexcept;
	virtual void setGraphicsProgram(GraphicsProgramPtr shader) noexcept;
	virtual GraphicsProgramPtr getGraphicsProgram() const noexcept;

	virtual void drawRenderBuffer(const RenderIndirect& renderable) noexcept;
	virtual void drawRenderBuffer(const RenderIndirect renderable[], std::size_t first, std::size_t count) noexcept;

private:
	OGLDevice(const OGLDevice&) noexcept = delete;
	OGLDevice& operator=(const OGLDevice&) noexcept = delete;

private:
	OGLDeviceContextPtr _glcontext;
};

_NAME_END

#endif