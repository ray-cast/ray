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

#include <ray/render_canvas.h>
#include <ray/render_texture.h>
#include <ray/render_state.h>
#include <ray/render_buffer.h>
#include <ray/shader.h>

_NAME_BEGIN

class EXPORT RenderDevice
{
public:
    RenderDevice() noexcept;
    virtual ~RenderDevice() noexcept;

    virtual bool open(const GPUfbconfig& fbconfig, const GPUctxconfig& ctxconfig) noexcept = 0;
    virtual void close() noexcept = 0;

    virtual void setSwapInterval(SwapInterval interval) noexcept = 0;
    virtual SwapInterval getSwapInterval() const noexcept = 0;

    virtual void setBlendState(const RenderBlendState& state) noexcept = 0;
    virtual void setRasterState(const RenderRasterState& state) noexcept = 0;
    virtual void setDepthState(const RenderDepthState& state) noexcept = 0;
    virtual void setStencilState(const RenderStencilState& state) noexcept = 0;
    virtual void setRenderState(RenderStatePtr state) noexcept = 0;

    virtual RenderCanvasPtr createRenderCanvas(WindHandle hwnd) noexcept = 0;
    virtual void destroyRenderCanvas(RenderCanvasPtr canvas) noexcept = 0;
    virtual void setRenderCanvas(RenderCanvasPtr handle) noexcept = 0;

    virtual bool createConstantBuffer(ShaderConstantBuffer& buffer) noexcept = 0;
    virtual void setShaderConstantBuffer(ShaderConstantBufferPtr buffer) noexcept = 0;
    virtual void destroyConstantBuffer(ShaderConstantBuffer& buffer) noexcept = 0;

    virtual bool createRenderBuffer(RenderBuffer& buffer) noexcept = 0;
    virtual void destroyRenderBuffer(RenderBuffer& buffer) noexcept = 0;
    virtual void setRenderBuffer(RenderBufferPtr buffer) noexcept = 0;
    virtual void updateRenderBuffer(RenderBufferPtr buffer) noexcept = 0;
    virtual void drawRenderBuffer(const Renderable& renderable) noexcept = 0;

    virtual bool createRenderTexture(RenderTexture& target) noexcept = 0;
    virtual void destroyRenderTexture(RenderTexture& target) noexcept = 0;
    virtual void setRenderTexture(RenderTexturePtr target) noexcept = 0;
    virtual void copyRenderTexture(RenderTexturePtr src, const Viewport& v1, RenderTexturePtr dest, const Viewport& v2) noexcept = 0;
    virtual void readRenderTexture(RenderTexturePtr source, PixelFormat pfd, std::size_t w, std::size_t h, void* data) noexcept = 0;

    virtual bool createMultiRenderTexture(MultiRenderTexture& desc) noexcept = 0;
    virtual void destroyMultiRenderTexture(MultiRenderTexture& target) noexcept = 0;
    virtual void setMultiRenderTexture(MultiRenderTexturePtr target) noexcept = 0;

    virtual bool createTexture(Texture& texture) noexcept = 0;
    virtual void destroyTexture(Texture& texture) noexcept = 0;

    virtual ShaderProgramPtr createShaderProgram(std::vector<ShaderPtr>& shaders) noexcept = 0;
    virtual void destroyShaderProgram(ShaderProgramPtr shader) noexcept = 0;
    virtual void setShaderProgram(ShaderProgramPtr shader) noexcept = 0;

    virtual void renderBegin() noexcept = 0;
    virtual void renderEnd() noexcept = 0;

    virtual void present(RenderCanvasPtr canvas) noexcept = 0;

private:
    RenderDevice(const RenderDevice&) noexcept = delete;
    RenderDevice& operator=(const RenderDevice&) noexcept = delete;
};

_NAME_END

#endif