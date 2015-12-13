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
#ifndef _H_RENDER_DEVICE_H_
#define _H_RENDER_DEVICE_H_

#include <ray/render_types.h>

_NAME_BEGIN

class EXPORT RenderDevice
{
public:
	RenderDevice() except;
	virtual ~RenderDevice() noexcept;

	virtual bool open(WindHandle window) except = 0;
	virtual void close() noexcept = 0;

	virtual void renderBegin() except = 0;
	virtual void renderEnd() except = 0;

	virtual RenderWindowPtr createRenderWindow() const noexcept = 0;
	virtual void setRenderWindow(RenderWindowPtr window) except = 0;
	virtual RenderWindowPtr getRenderWindow() const noexcept = 0;

	virtual void setWireframeMode(bool enable) except = 0;
	virtual bool getWireframeMode() const noexcept = 0;

	virtual void setViewport(const Viewport& viewport, std::size_t i = 0) except = 0;
	virtual const Viewport& getViewport(std::size_t i = 0) const noexcept = 0;

	virtual void setSwapInterval(SwapInterval interval) except = 0;
	virtual SwapInterval getSwapInterval() const noexcept = 0;

	virtual RenderBufferPtr createRenderBuffer() noexcept = 0;
	virtual IndexBufferDataPtr createIndexBufferData() noexcept = 0;
	virtual VertexBufferDataPtr createVertexBufferData() noexcept = 0;
	virtual void setRenderBuffer(RenderBufferPtr buffer) except = 0;
	virtual void updateRenderBuffer(RenderBufferPtr buffer) except = 0;
	virtual void drawRenderBuffer(const RenderIndirect& renderable) except = 0;
	virtual void drawRenderBuffer(const RenderIndirects& renderable) except = 0;
	virtual RenderBufferPtr getRenderBuffer() const noexcept = 0;

	virtual TexturePtr createTexture() noexcept = 0;
	virtual RenderTexturePtr createRenderTexture() noexcept = 0;
	virtual MultiRenderTexturePtr createMultiRenderTexture() noexcept = 0;
	virtual void setRenderTexture(RenderTexturePtr target) except = 0;
	virtual void setRenderTextureLayer(RenderTexturePtr target, std::int32_t layer) except = 0;
	virtual void setMultiRenderTexture(MultiRenderTexturePtr target) except = 0;
	virtual void clearRenderTexture(ClearFlags flags, const Vector4& color, float depth, std::int32_t stencil) except = 0;
	virtual void clearRenderTexture(ClearFlags flags, const Vector4& color, float depth, std::int32_t stencil, std::size_t i) except = 0;
	virtual void discardRenderTexture() except = 0;
	virtual void blitRenderTexture(RenderTexturePtr src, const Viewport& v1, RenderTexturePtr dest, const Viewport& v2) except = 0;
	virtual void readRenderTexture(RenderTexturePtr source, TextureFormat pfd, std::size_t w, std::size_t h, void* data) except = 0;
	virtual RenderTexturePtr getRenderTexture() const noexcept = 0;
	virtual MultiRenderTexturePtr getMultiRenderTexture() const noexcept = 0;

	virtual RenderStatePtr createRenderState() noexcept = 0;
	virtual void setRenderState(RenderStatePtr state) except = 0;
	virtual RenderStatePtr getRenderState() const noexcept = 0;

	virtual ShaderPtr createShader() noexcept = 0;
	virtual ShaderObjectPtr createShaderObject() noexcept = 0;
	virtual void setShaderObject(ShaderObjectPtr shader) except = 0;
	virtual ShaderObjectPtr getShaderObject() const noexcept = 0;

private:
	RenderDevice(const RenderDevice&) noexcept = delete;
	RenderDevice& operator=(const RenderDevice&) noexcept = delete;
};

_NAME_END

#endif