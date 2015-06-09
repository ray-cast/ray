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
#include <ray/framebuffer.h>

_NAME_BEGIN

FramebufferDesc::FramebufferDesc() noexcept
    : width(0)
    , height(0)
    , depth(0)
    , viewport(0, 0, 0, 0)
    , clearFlags(ClearFlags::CLEAR_ALL)
    , clearStencil(0)
    , clearDepth(1.0)
    , clearColor(0.0, 0.0, 0.0, 1.0)
    , resolveTexOp(TextureOp::OP_ADD)
    , resolveFormat(PixelFormat::R8G8B8A8)
    , resolveFilter(TextureFilter::GPU_NEAREST)
    , resolveMapping(TextureDim::DIM_2D)
    , resolveWrap(TextureWrap::CLAMP_TO_EDGE)
    , resolveMipmap(false)
    , sharedDepthTexture(0)
    , sharedStencilTexture(0)
{
}

Framebuffer::Framebuffer() noexcept
    : _width(0)
    , _height(0)
    , _depth(0)
    , _isMipmap(false)
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
    , _sharedDepth(0)
    , _sharedStencil(0)
{
}

Framebuffer::~Framebuffer() noexcept
{
}

void
Framebuffer::setClearFlags(ClearFlags flags) noexcept
{
    _clearFlags = flags;
}

void
Framebuffer::setClearColor(const Color4& color) noexcept
{
    _clearColor = color;
}

void
Framebuffer::setClearDepth(float depth) noexcept
{
    _clearDepth = depth;
}

void
Framebuffer::setClearStencil(int stencil) noexcept
{
    _clearStencil = stencil;
}

void
Framebuffer::setTexMipmap(bool enable) noexcept
{
    _isMipmap = enable;
}

void
Framebuffer::setTexFormat(PixelFormat format) noexcept
{
    _resolveFormat = format;
}

void
Framebuffer::setTexFilter(TextureFilter filter) noexcept
{
    _resolveFilter = filter;
}

void
Framebuffer::setTexWrap(TextureWrap wrap) noexcept
{
    _resolveWrap = wrap;
}

void
Framebuffer::setTexOp(TextureOp texop) noexcept
{
    _resolveTexOp = texop;
}

void
Framebuffer::setTexDim(TextureDim mapping) noexcept
{
    _resolveDim = mapping;
}

void
Framebuffer::setWidth(std::size_t w) noexcept
{
    _width = w;
}

void
Framebuffer::setHeight(std::size_t h) noexcept
{
    _height = h;
}

void
Framebuffer::setDepth(std::size_t depth) noexcept
{
    _depth = depth;
}

void
Framebuffer::setSize(std::size_t w, std::size_t h, std::size_t depth) noexcept
{
    _width = w;
    _height = h;
    _depth = depth;
}

void
Framebuffer::setViewport(const Viewport& view) noexcept
{
    _viewport = view;
}

void
Framebuffer::setAttachment(Attachment attachment) noexcept
{
    _attachment = attachment;
}

void
Framebuffer::setSharedDepthTexture(FramebufferPtr handle) noexcept
{
    _sharedDepth = handle;
}

void
Framebuffer::setSharedStencilTexture(FramebufferPtr handle) noexcept
{
    _sharedStencil = handle;
}

TextureOp
Framebuffer::getTexOp() const noexcept
{
    return _resolveTexOp;
}

PixelFormat
Framebuffer::getTexFormat()  const noexcept
{
    return _resolveFormat;
}

TextureFilter
Framebuffer::getTexFilter() const noexcept
{
    return _resolveFilter;
}

TextureWrap
Framebuffer::getTexWrap() const noexcept
{
    return _resolveWrap;
}

TextureDim
Framebuffer::getTexDim() const noexcept
{
    return _resolveDim;
}

std::size_t
Framebuffer::getWidth() const noexcept
{
    return _width;
}

std::size_t
Framebuffer::getHeight() const noexcept
{
    return _height;
}

std::size_t
Framebuffer::getDepth() const noexcept
{
    return _depth;
}

ClearFlags
Framebuffer::getClearFlags() const noexcept
{
    return _clearFlags;
}

Color4
Framebuffer::getClearColor() const noexcept
{
    return _clearColor;
}

float
Framebuffer::getClearDepth() const noexcept
{
    return _clearDepth;
}

int
Framebuffer::getClearStencil() const noexcept
{
    return _clearStencil;
}

const Viewport&
Framebuffer::getViewport() const noexcept
{
    return _viewport;
}

Attachment
Framebuffer::getAttachment() const noexcept
{
    return _attachment;
}

bool
Framebuffer::isMipmap() const noexcept
{
    return _isMipmap;
}

TexturePtr
Framebuffer::getResolveTexture() const noexcept
{
    return _resolveTexture;
}

FramebufferPtr
Framebuffer::getSharedDepthTexture() const noexcept
{
    return _sharedDepth;
}

FramebufferPtr
Framebuffer::getSharedStencilTexture() const noexcept
{
    return _sharedStencil;
}

MultiFramebuffer::MultiFramebuffer() noexcept
    : _viewport(0, 0, 0, 0)
{
}

MultiFramebuffer::~MultiFramebuffer() noexcept
{
}

void
MultiFramebuffer::setViewport(const Viewport& view) noexcept
{
    _viewport = view;
}

const Viewport&
MultiFramebuffer::getViewport() const noexcept
{
    return _viewport;
}

void
MultiFramebuffer::clear() noexcept
{
    _mrt.clear();
}

void
MultiFramebuffer::attach(FramebufferPtr rt) noexcept
{
    auto it = std::find(_mrt.begin(), _mrt.end(), rt);
    if (it == _mrt.end())
    {
        _mrt.push_back(rt);
    }
}

void
MultiFramebuffer::detach(FramebufferPtr rt) noexcept
{
    std::size_t count = _mrt.size();
    for (std::size_t i = 0; i < count; i++)
    {
        if (_mrt[i] == rt)
        {
            _mrt.erase(_mrt.begin() + i);
        }
    }
}

FramebufferPtr
MultiFramebuffer::getFramebuffer(Attachment attachment) const noexcept
{
    for (auto& it : _mrt)
    {
        if (it->getAttachment() == attachment)
        {
            return it;
        }
    }
}

Framebuffers
MultiFramebuffer::getFramebuffers() noexcept
{
    return _mrt;
}

_NAME_END