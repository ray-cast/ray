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

RenderTexture::~RenderTexture() noexcept
{
    this->close();
}

std::size_t
RenderTexture::getWidth() const noexcept
{
    return _width;
}

std::size_t
RenderTexture::getHeight() const noexcept
{
    return _height;
}

void
RenderTexture::setClearFlags(ClearFlags flags) noexcept
{
    _clearFlags = flags;
}

void
RenderTexture::setClearColor(const Color4& color) noexcept
{
    _clearColor = color;
}

void
RenderTexture::setClearDepth(float depth) noexcept
{
    _clearDepth = depth;
}

void
RenderTexture::setClearStencil(int stencil) noexcept
{
    _clearStencil = stencil;
}

void
RenderTexture::setTexMipmap(bool enable) noexcept
{
    _isMipmap = enable;
}

void
RenderTexture::setTexFormat(PixelFormat format) noexcept
{
    _resolveFormat = format;
}

void
RenderTexture::setTexFilter(TextureFilter filter) noexcept
{
    _resolveFilter = filter;
}

void
RenderTexture::setTexWrap(TextureWrap wrap) noexcept
{
    _resolveWrap = wrap;
}

void
RenderTexture::setTexOp(TextureOp texop) noexcept
{
    _resolveTexOp = texop;
}

void
RenderTexture::setTexDim(TextureDim mapping) noexcept
{
    _resolveDim = mapping;
}

void
RenderTexture::setMultiSample(bool multisample) noexcept
{
    _isMultiSample = multisample;
}

void
RenderTexture::setWidth(std::size_t w) noexcept
{
    _width = w;
}

void
RenderTexture::setHeight(std::size_t h) noexcept
{
    _height = h;
}

void
RenderTexture::setDepth(std::size_t depth) noexcept
{
    _depth = depth;
}

void
RenderTexture::setSize(std::size_t w, std::size_t h, std::size_t depth) noexcept
{
    _width = w;
    _height = h;
    _depth = depth;
}

void
RenderTexture::setViewport(const Viewport& view) noexcept
{
    _viewport = view;
}

TextureOp
RenderTexture::getTexOp() const noexcept
{
    return _resolveTexOp;
}

PixelFormat
RenderTexture::getTexFormat()  const noexcept
{
    return _resolveFormat;
}

TextureFilter
RenderTexture::getTexFilter() const noexcept
{
    return _resolveFilter;
}

TextureWrap
RenderTexture::getTexWrap() const noexcept
{
    return _resolveWrap;
}

TextureDim
RenderTexture::getTexDim() const noexcept
{
    return _resolveDim;
}

std::size_t
RenderTexture::getDepth() const noexcept
{
    return _depth;
}

ClearFlags
RenderTexture::getClearFlags() const noexcept
{
    return _clearFlags;
}

Color4
RenderTexture::getClearColor() const noexcept
{
    return _clearColor;
}

float
RenderTexture::getClearDepth() const noexcept
{
    return _clearDepth;
}

int
RenderTexture::getClearStencil() const noexcept
{
    return _clearStencil;
}

const Viewport&
RenderTexture::getViewport() const noexcept
{
    return _viewport;
}

bool
RenderTexture::isMipmap() const noexcept
{
    return _isMipmap;
}

bool
RenderTexture::isMultiSample() const noexcept
{
    return _isMultiSample;
}

void
RenderTexture::setSharedDepthTexture(RenderTexturePtr handle) noexcept
{
    _sharedDepthTexture = handle;
}

void
RenderTexture::setSharedStencilTexture(RenderTexturePtr handle) noexcept
{
    _sharedStencilTexture = handle;
}

TexturePtr
RenderTexture::getResolveTexture() const noexcept
{
    return _resolveTexture;
}

RenderTexturePtr
RenderTexture::getSharedDepthTexture() const noexcept
{
    return _sharedDepthTexture;
}

RenderTexturePtr
RenderTexture::getSharedStencilTexture() const noexcept
{
    return _sharedStencilTexture;
}

void
RenderTexture::setup(std::size_t w, std::size_t h, TextureDim dim, PixelFormat format, ClearFlags flags, const Color4& color, float depth, int stencil) noexcept
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

    TextureDesc param;
    param.size.x = this->getWidth();
    param.size.y = this->getHeight();
    param.mipmap = this->isMipmap();
    param.format = this->getTexFormat();
    param.filter = this->getTexFilter();
    param.wrap = this->getTexWrap();
    param.texop = this->getTexOp();
    param.dim = this->getTexDim();

    _resolveTexture = std::make_shared<Texture>();
    _resolveTexture->setup(param);

    RenderImpl::instance()->createRenderTexture(*this);
}

void
RenderTexture::close() noexcept
{
    if (_resolveTexture)
    {
        _resolveTexture.reset();
        _resolveTexture = nullptr;
    }

    RenderImpl::instance()->destroyRenderTexture(*this);
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