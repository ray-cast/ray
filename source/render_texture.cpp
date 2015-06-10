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
#include <ray/render_texture.h>
#include <ray/render_impl.h>

_NAME_BEGIN

__ImplementSubClass(RenderTexture, Object<RenderTexture>)
__ImplementSubClass(MultiRenderTexture, Object<MultiRenderTexture>)

RenderTexture::RenderTexture() noexcept
{
}

RenderTexture::~RenderTexture() noexcept
{
    this->close();
}

std::size_t
RenderTexture::getWidth() const noexcept
{
    return _renderTarget->getWidth();
}

std::size_t
RenderTexture::getHeight() const noexcept
{
    return _renderTarget->getHeight();
}

TexturePtr
RenderTexture::getResolveTexture() const noexcept
{
    return _renderTarget->getResolveTexture();
}

void
RenderTexture::setup(std::size_t w, std::size_t h, TextureDim mapping, PixelFormat format, ClearFlags flags, const Color4& color, float depth, int stencil) noexcept
{
    FramebufferDesc desc;
    desc.width = w;
    desc.height = h;

    desc.viewport = Viewport(0, 0, w, h);

    desc.clearFlags = flags;
    desc.clearColor = color;
    desc.clearDepth = depth;
    desc.clearStencil = stencil;

    desc.resolveFormat = format;
    desc.resolveMapping = mapping;
    desc.resolveFilter = TextureFilter::GPU_LINEAR;
    desc.resolveWrap = TextureWrap::CLAMP_TO_EDGE;

    desc.sharedDepthTexture = _sharedDepthTarget;
    desc.sharedStencilTexture = _sharedStencilTarget;

    _renderTarget = RenderImpl::instance()->createFramebuffer(desc);
}

void
RenderTexture::close() noexcept
{
    if (_renderTarget)
    {
        RenderImpl::instance()->destroyFramebuffer(_renderTarget);
        _renderTarget.reset();
        _renderTarget = nullptr;
    }
}

FramebufferPtr
RenderTexture::getFramebuffer() const noexcept
{
    return _renderTarget;
}

void
RenderTexture::setShareDepthTarget(RenderTexturePtr rt)
{
    _sharedDepthTarget = rt->getFramebuffer();
}

void
RenderTexture::setShareStencilTarget(RenderTexturePtr rt)
{
    _sharedStencilTarget = rt->getFramebuffer();
}

FramebufferPtr
RenderTexture::getShareDepthTarget() const noexcept
{
    return _sharedDepthTarget;
}

FramebufferPtr
RenderTexture::getShareStencilTarget() const noexcept
{
    return _sharedStencilTarget;
}

RenderTexturePtr
RenderTexture::clone() const noexcept
{
    return std::make_shared<RenderTexture>();
}

MultiRenderTexture::MultiRenderTexture() noexcept
{
}

MultiRenderTexture::~MultiRenderTexture() noexcept
{
    this->close();
}

void
MultiRenderTexture::setup(std::size_t w, std::size_t h) noexcept
{
    assert(!_textures.empty());

    for (auto& it : _textures)
    {
        if (it.texture->getWidth() != w ||
            it.texture->getHeight() != h)
        {
            assert(false);
        }
    }

    this->setViewport(Viewport(0, 0, w, h));

    RenderImpl::instance()->createMultiRenderTexture(*this);
}

void
MultiRenderTexture::close() noexcept
{
    RenderImpl::instance()->createMultiRenderTexture(*this);
}

void
MultiRenderTexture::attach(RenderTexturePtr texture, Attachment location) noexcept
{
    auto it = _textures.begin();
    auto end = _textures.end();

    for (; it != end; ++it)
    {
        if (it->texture == texture)
            break;
    }

    if (it == _textures.end())
    {
        RenderTarget tex;
        tex.texture = texture;
        tex.location = location;

        _textures.push_back(tex);
    }
}

void
MultiRenderTexture::detach(RenderTexturePtr texture) noexcept
{
    auto it = _textures.begin();
    auto end = _textures.end();

    for (; it != end; ++it)
    {
        if (it->texture == texture)
            break;
    }

    if (it != _textures.end())
    {
        _textures.erase(it);
    }
}

void
MultiRenderTexture::setViewport(const Viewport& view) noexcept
{
    _viewport = view;
}

const Viewport&
MultiRenderTexture::getViewport() const noexcept
{
    return _viewport;
}

RenderTextures&
MultiRenderTexture::getRenderTextures() noexcept
{
    return _textures;
}

const RenderTextures&
MultiRenderTexture::getRenderTextures() const noexcept
{
    return _textures;
}

MultiRenderTexturePtr
MultiRenderTexture::clone() const noexcept
{
    return std::make_shared<MultiRenderTexture>();
}

_NAME_END