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
#ifndef _H_RENDER_IMPL_H_
#define _H_RENDER_IMPL_H_

#include <ray/render_device.h>

_NAME_BEGIN

class RenderImpl
{
    __DeclareSingleton(RenderImpl)
public:
    RenderImpl() noexcept;
    ~RenderImpl() noexcept;

    bool open(RenderDevicePtr renderDevice) except;
    void close() noexcept;

    void setSwapInterval(SwapInterval interval) noexcept;
    SwapInterval getSwapInterval() const noexcept;

    void setRenderState(RenderStatePtr state) noexcept;

    bool createShaderVariant(ShaderVariant& buffer) noexcept;
    void destroyShaderVariant(ShaderVariant& buffer) noexcept;
    void setShaderVariant(ShaderVariantPtr buffer, ShaderUniformPtr uniform) noexcept;

    bool createRenderBuffer(RenderBuffer& buffer) noexcept;
    void destroyRenderBuffer(RenderBuffer& buffer) noexcept;
    void setRenderBuffer(RenderBufferPtr buffer) noexcept;
    void updateRenderBuffer(RenderBufferPtr buffer) noexcept;
    void drawRenderBuffer(const Renderable& renderable) noexcept;

    bool createRenderTexture(RenderTexture& target) noexcept;
    void destroyRenderTexture(RenderTexture& target) noexcept;
    void setRenderTexture(RenderTexturePtr target) noexcept;
    void copyRenderTexture(RenderTexturePtr srcTarget, const Viewport& src, RenderTexturePtr destTarget, const Viewport& dest) noexcept;
    void readRenderTexture(RenderTexturePtr target, PixelFormat pfd, std::size_t w, std::size_t h, void* data) noexcept;

    bool createMultiRenderTexture(MultiRenderTexture& desc) noexcept;
    void destroyMultiRenderTexture(MultiRenderTexture& target) noexcept;
    void setMultiRenderTexture(MultiRenderTexturePtr target) noexcept;

    bool createTexture(Texture& texture) noexcept;
    void destroyTexture(Texture& texture) noexcept;

    ShaderProgramPtr createShaderProgram(std::vector<ShaderPtr>& shaders) noexcept;
    void destroyShaderProgram(ShaderProgramPtr program) noexcept;
    void setShaderProgram(ShaderProgramPtr program) noexcept;

    void renderBegin() noexcept;
    void renderEnd() noexcept;

private:
    RenderImpl(const RenderImpl&) noexcept = delete;
    RenderImpl& operator=(const RenderImpl&)noexcept = delete;

private:

    RenderDevicePtr _renderDevice;
};

_NAME_END

#endif