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
#include <ray/graphics_texture.h>

_NAME_BEGIN

__ImplementSubInterface(GraphicsTexture, GraphicsResource, "GraphicsTexture")

GraphicsTextureDesc::GraphicsTextureDesc() noexcept
	: _size(0, 0, 0)
	, _format(TextureFormat::R8G8B8A8)
	, _dim(TextureDim::DIM_2D)
	, _mipmap(false)
	, _multisample(false)
	, _mipLevel(0)
	, _mipSize(0)
	, _texop(SamplerOp::Add)
	, _filter(SamplerFilter::Linear)
	, _wrap(SamplerWrap::ClampToEdge)
	, _anis(SamplerAnis::Anis0)
	, _data(nullptr)
{
}

GraphicsTextureDesc::~GraphicsTextureDesc() noexcept
{
}

void
GraphicsTextureDesc::setTexMipmap(bool enable) noexcept
{
	_mipmap = enable;
}

void
GraphicsTextureDesc::setTexFormat(TextureFormat format) noexcept
{
	_format = format;
}

void
GraphicsTextureDesc::setTexDim(TextureDim map) noexcept
{
	_dim = map;
}

void
GraphicsTextureDesc::setTexOp(SamplerOp op) noexcept
{
	_texop = op;
}

void
GraphicsTextureDesc::setSamplerWrap(SamplerWrap wrap) noexcept
{
	_wrap = wrap;
}

void
GraphicsTextureDesc::setSamplerFilter(SamplerFilter filter) noexcept
{
	_filter = filter;
}

void
GraphicsTextureDesc::setSamplerAnis(SamplerAnis anis) noexcept
{
	_anis = anis;
}

void
GraphicsTextureDesc::setMipLevel(std::uint8_t level) noexcept
{
	_mipLevel = level;
}

void
GraphicsTextureDesc::setMipSize(std::uint32_t size) noexcept
{
	_mipSize = size;
}

void
GraphicsTextureDesc::setMultisample(bool enable) noexcept
{
	_multisample = enable;
}

void
GraphicsTextureDesc::setWidth(int w) noexcept
{
	_size.x = w;
}

void
GraphicsTextureDesc::setHeight(int h) noexcept
{
	_size.y = h;
}

void
GraphicsTextureDesc::setDepth(int d) noexcept
{
	_size.z = d;
}

void
GraphicsTextureDesc::setSize(int w, int h, int depth) noexcept
{
	_size.x = w;
	_size.y = h;
	_size.z = depth;
}

void
GraphicsTextureDesc::setStream(void* data) noexcept
{
	_data = data;
}

SamplerOp
GraphicsTextureDesc::getTexOp() const noexcept
{
	return _texop;
}

TextureFormat
GraphicsTextureDesc::getTexFormat() const noexcept
{
	return _format;
}

TextureDim
GraphicsTextureDesc::getTexDim() const noexcept
{
	return _dim;
}

SamplerWrap
GraphicsTextureDesc::getSamplerWrap() const noexcept
{
	return _wrap;
}

SamplerFilter
GraphicsTextureDesc::getSamplerFilter() const noexcept
{
	return _filter;
}

SamplerAnis
GraphicsTextureDesc::getSamplerAnis() const noexcept
{
	return _anis;
}

std::int32_t
GraphicsTextureDesc::getMipLevel() const noexcept
{
	return _mipLevel;
}

std::uint32_t
GraphicsTextureDesc::getMipSize() const noexcept
{
	return _mipSize;
}

int
GraphicsTextureDesc::getWidth() const noexcept
{
	return _size.x;
}

int
GraphicsTextureDesc::getHeight() const noexcept
{
	return _size.y;
}

int
GraphicsTextureDesc::getDepth() const noexcept
{
	return _size.z;
}

const int3&
GraphicsTextureDesc::getSize() const noexcept
{
	return _size;
}

void*
GraphicsTextureDesc::getStream() const noexcept
{
	return _data;
}

bool
GraphicsTextureDesc::isMipmap() const noexcept
{
	return _mipmap;
}

bool
GraphicsTextureDesc::isMultiSample() const noexcept
{
	return _multisample;
}

GraphicsTexture::GraphicsTexture() noexcept
{
}

GraphicsTexture::~GraphicsTexture() noexcept
{
}

_NAME_END