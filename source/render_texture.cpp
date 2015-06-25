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
#include <ray/render_factory.h>

_NAME_BEGIN

RenderTarget::RenderTarget() noexcept
    : _width(0)
    , _height(0)
    , _depth(0)
    , _isMipmap(false)
    , _isMultiSample(false)
    , _attachment(Attachment::COLOR0)
    , _viewport(0, 0, 0, 0)
    , _clearFlags(ClearFlags::CLEAR_ALL)
    , _clearStencil(0)
    , _clearDepth(1.0)
    , _clearColor(0.0, 0.0, 0.0, 1.0)
    , _resolveTexOp(TextureOp::OP_ADD)
    , _resolveFormat(PixelFormat::R8G8B8A8)
    , _resolveFilter(TextureFilter::GPU_NEAREST)
    , _resolveDim(TextureDim::DIM_2D)
    , _resolveWrap(TextureWrap::CLAMP_TO_EDGE)
    , _resolveTexture(0)
    , _sharedDepthTexture(0)
    , _sharedStencilTexture(0)
{
}

RenderTarget::~RenderTarget() noexcept
{
}

std::size_t
RenderTarget::getWidth() const noexcept
{
    return _width;
}

std::size_t
RenderTarget::getHeight() const noexcept
{
    return _height;
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
    _isMipmap = enable;
}

void
RenderTarget::setTexFormat(PixelFormat format) noexcept
{
    _resolveFormat = format;
}

void
RenderTarget::setTexFilter(TextureFilter filter) noexcept
{
    _resolveFilter = filter;
}

void
RenderTarget::setTexWrap(TextureWrap wrap) noexcept
{
    _resolveWrap = wrap;
}

void
RenderTarget::setTexOp(TextureOp texop) noexcept
{
    _resolveTexOp = texop;
}

void
RenderTarget::setTexDim(TextureDim mapping) noexcept
{
    _resolveDim = mapping;
}

void
RenderTarget::setMultiSample(bool multisample) noexcept
{
    _isMultiSample = multisample;
}

void
RenderTarget::setWidth(std::size_t w) noexcept
{
    _width = w;
}

void
RenderTarget::setHeight(std::size_t h) noexcept
{
    _height = h;
}

void
RenderTarget::setDepth(std::size_t depth) noexcept
{
    _depth = depth;
}

void
RenderTarget::setSize(std::size_t w, std::size_t h, std::size_t depth) noexcept
{
    _width = w;
    _height = h;
    _depth = depth;
}

void
RenderTarget::setViewport(const Viewport& view) noexcept
{
    _viewport = view;
}

TextureOp
RenderTarget::getTexOp() const noexcept
{
    return _resolveTexOp;
}

PixelFormat
RenderTarget::getTexFormat()  const noexcept
{
    return _resolveFormat;
}

TextureFilter
RenderTarget::getTexFilter() const noexcept
{
    return _resolveFilter;
}

TextureWrap
RenderTarget::getTexWrap() const noexcept
{
    return _resolveWrap;
}

TextureDim
RenderTarget::getTexDim() const noexcept
{
    return _resolveDim;
}

std::size_t
RenderTarget::getDepth() const noexcept
{
    return _depth;
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

const Viewport&
RenderTarget::getViewport() const noexcept
{
    return _viewport;
}

bool
RenderTarget::isMipmap() const noexcept
{
    return _isMipmap;
}

bool
RenderTarget::isMultiSample() const noexcept
{
    return _isMultiSample;
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
RenderTarget::setup(std::size_t w, std::size_t h, TextureDim dim, PixelFormat format, ClearFlags flags, const Color4& color, float depth, int stencil) noexcept
{
    _width = w;
    _height = h;

    _viewport = Viewport(0, 0, w, h);

    _clearFlags = flags;
    _clearColor = color;
    _clearDepth = depth;
    _clearStencil = stencil;

    _resolveFormat = format;
    _resolveDim = dim;
    _resolveFilter = TextureFilter::GPU_LINEAR;
    _resolveWrap = TextureWrap::CLAMP_TO_EDGE;

    _resolveTexture = RenderFactory::createTexture();
    _resolveTexture->setSize(this->getWidth(), this->getHeight());
    _resolveTexture->setTexMipmap(this->isMipmap());
    _resolveTexture->setTexFormat(this->getTexFormat());
    _resolveTexture->setTexFilter(this->getTexFilter());
    _resolveTexture->setTexWrap(this->getTexWrap());
    _resolveTexture->setTexOp(this->getTexOp());
    _resolveTexture->setTexDim(this->getTexDim());
    _resolveTexture->setup();

    this->setup();
}

MultiRenderTarget::MultiRenderTarget() noexcept
    : _active(false)
{
}

MultiRenderTarget::~MultiRenderTarget() noexcept
{
}

void
MultiRenderTarget::setActive(bool active) except
{
    if (_active != active)
    {
        if (active)
        {
            this->onActivate();
        }
        else
        {
            this->onDeactivate();
        }

        _active = active;
    }
}

bool
MultiRenderTarget::getActive() const noexcept
{
    return _active;
}

void
MultiRenderTarget::attach(RenderTargetPtr texture, Attachment location) noexcept
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
        RenderTargetBinding tex;
        tex.texture = texture;
        tex.location = location;

        _textures.push_back(tex);
    }
}

void
MultiRenderTarget::detach(RenderTargetPtr texture) noexcept
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

RenderTargetBindings&
MultiRenderTarget::getRenderTargets() noexcept
{
    return _textures;
}

const RenderTargetBindings&
MultiRenderTarget::getRenderTargets() const noexcept
{
    return _textures;
}

_NAME_END