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
#include <ray/texture.h>
#include <ray/render_impl.h>

_NAME_BEGIN

TextureDesc::TextureDesc() noexcept
    : size(0, 0, 0)
    , format(PixelFormat::R8G8B8A8)
    , dim(TextureDim::DIM_2D)
    , mipmap(false)
    , multisample(false)
    , level(0)
    , texop(TextureOp::OP_ADD)
    , filter(TextureFilter::GPU_LINEAR)
    , wrap(TextureWrap::CLAMP_TO_EDGE)
    , anis(Anisotropy::ANISOTROPY_0)
    , data(nullptr)
{
};

Texture::Texture() noexcept
{
}

Texture::~Texture() noexcept
{
    this->close();
}

bool
Texture::setup(std::size_t w, std::size_t h, void* data, TextureDim dim, PixelFormat format) noexcept
{
    TextureDesc desc;
    desc.size.x = w;
    desc.size.y = h;
    desc.data = data;
    desc.dim = dim;
    desc.format = format;

    this->setTextureDesc(desc);

    return RenderImpl::instance()->createTexture(*this);
}

bool
Texture::setup(const TextureDesc& desc) noexcept
{
    this->setTextureDesc(desc);

    return RenderImpl::instance()->createTexture(*this);
}

void
Texture::close() noexcept
{
    RenderImpl::instance()->destroyTexture(*this);
}

void
Texture::setTexMipmap(bool enable) noexcept
{
    _setting.mipmap = enable;
}

void
Texture::setTexFormat(PixelFormat format) noexcept
{
    _setting.format = format;
}

void
Texture::setTexOp(TextureOp op) noexcept
{
    _setting.texop = op;
}

void
Texture::setTexWrap(TextureWrap wrap) noexcept
{
    _setting.wrap = wrap;
}

void
Texture::setTexFilter(TextureFilter filter) noexcept
{
    _setting.filter = filter;
}

void
Texture::setTexDim(TextureDim map) noexcept
{
    _setting.dim = map;
}

void
Texture::setLevel(int level) noexcept
{
    _setting.level = level;
}

void
Texture::setWidth(int w) noexcept
{
    _setting.size.x = w;
}

void
Texture::setHeight(int h) noexcept
{
    _setting.size.y = h;
}

void
Texture::setDepth(int d) noexcept
{
    _setting.size.z = d;
}

void
Texture::setSize(int w, int h, int depth) noexcept
{
    _setting.size.x = w;
    _setting.size.y = h;
    _setting.size.z = depth;
}

void
Texture::setStream(void* data) noexcept
{
    _setting.data = data;
}

TextureOp
Texture::getTexOp() const noexcept
{
    return _setting.texop;
}

PixelFormat
Texture::getTexFormat() const noexcept
{
    return _setting.format;
}

TextureDim
Texture::getTexDim() const noexcept
{
    return _setting.dim;
}

TextureWrap
Texture::getTexWrap() const noexcept
{
    return _setting.wrap;
}

TextureFilter
Texture::getTexFilter() const noexcept
{
    return _setting.filter;
}

Anisotropy
Texture::getTexAnisotropy() const noexcept
{
    return _setting.anis;
}

int
Texture::getLevel() const noexcept
{
    return _setting.level;
}

int
Texture::getWidth() const noexcept
{
    return _setting.size.x;
}

int
Texture::getHeight() const noexcept
{
    return _setting.size.y;
}

int
Texture::getDepth() const noexcept
{
    return _setting.size.z;
}

const int3&
Texture::getSize() const noexcept
{
    return _setting.size;
}

void*
Texture::getStream() const noexcept
{
    return _setting.data;
}

void
Texture::setTextureDesc(const TextureDesc& desc) noexcept
{
    _setting = desc;
}

const TextureDesc&
Texture::getTextureDesc() const noexcept
{
    return _setting;
}

bool
Texture::isMipmap() const noexcept
{
    return _setting.mipmap;
}

bool
Texture::isMultiSample() const noexcept
{
    return _setting.multisample;
}

void
Texture::copy(Texture* other) noexcept
{
    this->setWidth(other->getWidth());
    this->setHeight(other->getHeight());
    this->setTexMipmap(other->isMipmap());
    this->setTexFormat(other->getTexFormat());
    this->setTexFilter(other->getTexFilter());
    this->setTexWrap(other->getTexWrap());
    this->setTexOp(other->getTexOp());
    this->setTexDim(other->getTexDim());
}

void
Texture::copy(TexturePtr other) noexcept
{
    this->copy(other.get());
}

TexturePtr
Texture::clone() const noexcept
{
    return std::make_shared<Texture>();
}

_NAME_END