// +----------------------------------------------------------------------
// | Project : ray.
// | All rights reserved.
// +----------------------------------------------------------------------
// | Copyright (c) 2013-2014.
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
#ifndef _H_RENDER_TEXTURE_H_
#define _H_RENDER_TEXTURE_H_

#include <ray/framebuffer.h>

_NAME_BEGIN

class EXPORT RenderTexture final : public Reference<RenderTexture>
{
    __DeclareSubClass(RenderTexture, Reference<RenderTexture>)
public:
    RenderTexture() noexcept;
    ~RenderTexture() noexcept;

    void setup(std::size_t w, std::size_t h, TextureDim mapping, PixelFormat format, ClearFlags flags = ClearFlags::CLEAR_ALL, const Color4& color = Color4::Black, float depth = 1.0, int stencil = 0) noexcept;
    void close() noexcept;

    std::size_t getWidth() const noexcept;
    std::size_t getHeight() const noexcept;

    TexturePtr getResolveTexture() const noexcept;

    void setShareDepthTarget(RenderTexturePtr rt);
    void setShareStencilTarget(RenderTexturePtr rt);

    FramebufferPtr getFramebuffer() const noexcept;

    FramebufferPtr getShareDepthTarget() const noexcept;
    FramebufferPtr getShareStencilTarget() const noexcept;

private:
    RenderTexture(RenderTexture&) noexcept = delete;
    RenderTexture& operator=(const RenderTexture&)noexcept = delete;

private:

    FramebufferPtr _renderTarget;
    FramebufferPtr _sharedDepthTarget;
    FramebufferPtr _sharedStencilTarget;
};

class MultiRenderTexture final : public Reference<MultiRenderTexture>
{
public:
    MultiRenderTexture() noexcept;
    ~MultiRenderTexture() noexcept;

    static MultiRenderTexturePtr create() noexcept;

    void setup(std::size_t w, std::size_t h, ClearFlags flags = ClearFlags::CLEAR_ALL, const Color4& color = Color4::Black, float depth = 1.0, int stencil = 0) noexcept;
    void close() noexcept;

    void attach(RenderTexturePtr texture, Attachment location) noexcept;
    void detach(RenderTexturePtr texture) noexcept;

    MultiFramebufferPtr getFramebuffer() const noexcept;

private:

    MultiFramebufferPtr _renderTarget;

    struct RenderTextures
    {
        RenderTexturePtr texture;
        Attachment location;
    };

    std::vector<RenderTextures> _textures;
};

_NAME_END

#endif