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

_NAME_BEGIN

Texture::Texture() noexcept
	: _size(0, 0, 0)
	, _format(PixelFormat::R8G8B8A8)
	, _dim(TextureDim::DIM_2D)
	, _mipmap(false)
	, _multisample(false)
	, _level(0)
	, _texop(TextureOp::OP_ADD)
	, _filter(TextureFilter::GPU_LINEAR)
	, _wrap(TextureWrap::CLAMP_TO_EDGE)
	, _anis(Anisotropy::ANISOTROPY_0)
	, _data(nullptr)
{
}

Texture::~Texture() noexcept
{
}

void
Texture::setTexMipmap(bool enable) noexcept
{
	_mipmap = enable;
}

void
Texture::setTexFormat(PixelFormat format) noexcept
{
	_format = format;
}

void
Texture::setTexOp(TextureOp op) noexcept
{
	_texop = op;
}

void
Texture::setTexWrap(TextureWrap wrap) noexcept
{
	_wrap = wrap;
}

void
Texture::setTexFilter(TextureFilter filter) noexcept
{
	_filter = filter;
}

void
Texture::setTexDim(TextureDim map) noexcept
{
	_dim = map;
}

void
Texture::setLevel(int level) noexcept
{
	_level = level;
}

void
Texture::setAnisotropy(Anisotropy anis) noexcept
{
	_anis = anis;
}

void
Texture::setMultisample(bool enable) noexcept
{
	_multisample = enable;
}

void
Texture::setWidth(int w) noexcept
{
	_size.x = w;
}

void
Texture::setHeight(int h) noexcept
{
	_size.y = h;
}

void
Texture::setDepth(int d) noexcept
{
	_size.z = d;
}

void
Texture::setSize(int w, int h, int depth) noexcept
{
	_size.x = w;
	_size.y = h;
	_size.z = depth;
}

void
Texture::setStream(void* data) noexcept
{
	_data = data;
}

TextureOp
Texture::getTexOp() const noexcept
{
	return _texop;
}

PixelFormat
Texture::getTexFormat() const noexcept
{
	return _format;
}

TextureDim
Texture::getTexDim() const noexcept
{
	return _dim;
}

TextureWrap
Texture::getTexWrap() const noexcept
{
	return _wrap;
}

TextureFilter
Texture::getTexFilter() const noexcept
{
	return _filter;
}

Anisotropy
Texture::getTexAnisotropy() const noexcept
{
	return _anis;
}

int
Texture::getLevel() const noexcept
{
	return _level;
}

int
Texture::getWidth() const noexcept
{
	return _size.x;
}

int
Texture::getHeight() const noexcept
{
	return _size.y;
}

int
Texture::getDepth() const noexcept
{
	return _size.z;
}

const int3&
Texture::getSize() const noexcept
{
	return _size;
}

void*
Texture::getStream() const noexcept
{
	return _data;
}

bool
Texture::isMipmap() const noexcept
{
	return _mipmap;
}

bool
Texture::isMultiSample() const noexcept
{
	return _multisample;
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

_NAME_END