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
#include <ray/texture.h>
#include <ray/render_factory.h>

_NAME_BEGIN

RenderTarget::RenderTarget() noexcept
    : _viewport(0, 0, 0, 0)
    , _clearFlags(ClearFlags::CLEAR_NONE)
    , _clearStencil(0)
    , _clearDepth(1.0)
    , _clearColor(0.0, 0.0, 0.0, 1.0)
    , _resolveTexture(0)
    , _sharedDepthTexture(0)
    , _sharedStencilTexture(0)
{
    _resolveTexture = RenderFactory::createTexture();
}

RenderTarget::~RenderTarget() noexcept
{
}

void
RenderTarget::setClearFlags(ClearFlags flags) noexcept
{
    _clearFlags = flags;
}

void
RenderTarget::setClearColor(const Color4& color) noexcept
{
    _clearColor = color;
}

void
RenderTarget::setClearDepth(float depth) noexcept
{
    _clearDepth = depth;
}

void
RenderTarget::setClearStencil(int stencil) noexcept
{
    _clearStencil = stencil;
}

void
RenderTarget::setTexMipmap(bool enable) noexcept
{
    _resolveTexture->setTexMipmap(enable);
}

void
RenderTarget::setTexFormat(PixelFormat format) noexcept
{
    _resolveTexture->setTexFormat(format);
}

void
RenderTarget::setTexFilter(TextureFilter filter) noexcept
{
    _resolveTexture->setTexFilter(filter);
}

void
RenderTarget::setTexWrap(TextureWrap wrap) noexcept
{
    _resolveTexture->setTexWrap(wrap);
}

void
RenderTarget::setTexOp(TextureOp texop) noexcept
{
    _resolveTexture->setTexOp(texop);
}

void
RenderTarget::setTexDim(TextureDim mapping) noexcept
{
    _resolveTexture->setTexDim(mapping);
}

void
RenderTarget::setMultiSample(bool multisample) noexcept
{
    _resolveTexture->setMultisample(multisample);
}

void
RenderTarget::setWidth(std::size_t w) noexcept
{
    _resolveTexture->setWidth(w);
}

void
RenderTarget::setHeight(std::size_t h) noexcept
{
    _resolveTexture->setHeight(h);
}

void
RenderTarget::setDepth(std::size_t depth) noexcept
{
    _resolveTexture->setDepth(depth);
}

void
RenderTarget::setSize(std::size_t w, std::size_t h, std::size_t depth) noexcept
{
    _resolveTexture->setSize(w, h, depth);
}

TextureOp
RenderTarget::getTexOp() const noexcept
{
    return _resolveTexture->getTexOp();
}

PixelFormat
RenderTarget::getTexFormat()  const noexcept
{
    return _resolveTexture->getTexFormat();
}

TextureFilter
RenderTarget::getTexFilter() const noexcept
{
    return _resolveTexture->getTexFilter();
}

TextureWrap
RenderTarget::getTexWrap() const noexcept
{
    return _resolveTexture->getTexWrap();
}

TextureDim
RenderTarget::getTexDim() const noexcept
{
    return _resolveTexture->getTexDim();
}

std::size_t
RenderTarget::getWidth() const noexcept
{
    return _resolveTexture->getWidth();
}

std::size_t
RenderTarget::getHeight() const noexcept
{
    return _resolveTexture->getHeight();
}

std::size_t
RenderTarget::getDepth() const noexcept
{
    return _resolveTexture->getDepth();
}

ClearFlags
RenderTarget::getClearFlags() const noexcept
{
    return _clearFlags;
}

Color4
RenderTarget::getClearColor() const noexcept
{
    return _clearColor;
}

float
RenderTarget::getClearDepth() const noexcept
{
    return _clearDepth;
}

int
RenderTarget::getClearStencil() const noexcept
{
    return _clearStencil;
}

bool
RenderTarget::isMipmap() const noexcept
{
    return _resolveTexture->isMipmap();
}

bool
RenderTarget::isMultiSample() const noexcept
{
    return _resolveTexture->isMultiSample();
}

void
RenderTarget::setSharedDepthTexture(RenderTargetPtr handle) noexcept
{
    _sharedDepthTexture = handle;
}

void
RenderTarget::setSharedStencilTexture(RenderTargetPtr handle) noexcept
{
    _sharedStencilTexture = handle;
}

TexturePtr
RenderTarget::getResolveTexture() const noexcept
{
    return _resolveTexture;
}

RenderTargetPtr
RenderTarget::getSharedDepthTexture() const noexcept
{
    return _sharedDepthTexture;
}

RenderTargetPtr
RenderTarget::getSharedStencilTexture() const noexcept
{
    return _sharedStencilTexture;
}

void
RenderTarget::setup(std::size_t w, std::size_t h, TextureDim dim, PixelFormat format) noexcept
{
    _resolveTexture->setWidth(w);
    _resolveTexture->setHeight(h);
    _resolveTexture->setTexDim(dim);
    _resolveTexture->setTexFormat(format);
    _resolveTexture->setup();

    this->setup();
}

MultiRenderTarget::MultiRenderTarget() noexcept
{
}

MultiRenderTarget::~MultiRenderTarget() noexcept
{
}

void
MultiRenderTarget::attach(RenderTargetPtr texture) noexcept
{
    assert(texture);
    assert(std::find(_textures.begin(), _textures.end(), texture) == _textures.end());

    _textures.push_back(texture);
}

void
MultiRenderTarget::detach(RenderTargetPtr texture) noexcept
{
    auto it = std::find(_textures.begin(), _textures.end(), texture);
    if (it != _textures.end())
    {
        _textures.erase(it);
    }
}

RenderTargets&
MultiRenderTarget::getRenderTargets() noexcept
{
    return _textures;
}

const RenderTargets&
MultiRenderTarget::getRenderTargets() const noexcept
{
    return _textures;
}

void
MultiRenderTarget::setSharedDepthTexture(RenderTargetPtr target) noexcept
{
    _sharedDepthTexture = target;
}

void
MultiRenderTarget::setSharedStencilTexture(RenderTargetPtr target) noexcept
{
    _sharedStencilTexture = target;
}

RenderTargetPtr
MultiRenderTarget::getSharedDepthTexture() const noexcept
{
    return _sharedDepthTexture;
}

RenderTargetPtr
MultiRenderTarget::getSharedStencilTexture() const noexcept
{
    return _sharedStencilTexture;
}

_NAME_END