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
	, _multisample(false)
	, _layerBase(0)
	, _layer(1)
	, _mipBase(0)
	, _mipLevel(1)
	, _format(GraphicsFormat::GraphicsFormatUndefined)
	, _dim(GraphicsTextureDim::GraphicsTextureDim2D)
	, _filter(GraphicsSamplerFilter::GraphicsSamplerFilterLinear)
	, _wrap(GraphicsSamplerWrap::GraphicsSamplerWrapRepeat)
	, _anis(GraphicsSamplerAnis::GraphicsSamplerAnis1)
	, _textureUsage(GraphicsViewUsageFlagBits::GraphicsViewUsageFlagBitsSampledBit)
	, _tiling(GraphicsImageTiling::GraphicsImageTilingOptimal)
	, _data(nullptr)
	, _dataSize(0)
{
}

GraphicsTextureDesc::~GraphicsTextureDesc() noexcept
{
}

void
GraphicsTextureDesc::setWidth(std::uint32_t w) noexcept
{
	_size.x = w;
}

void
GraphicsTextureDesc::setHeight(std::uint32_t h) noexcept
{
	_size.y = h;
}

void
GraphicsTextureDesc::setDepth(std::uint32_t d) noexcept
{
	_size.z = d;
}

void
GraphicsTextureDesc::setSize(std::uint32_t w, std::uint32_t h, std::uint32_t depth) noexcept
{
	_size.x = w;
	_size.y = h;
	_size.z = depth;
}

std::uint32_t
GraphicsTextureDesc::getWidth() const noexcept
{
	return _size.x;
}

std::uint32_t
GraphicsTextureDesc::getHeight() const noexcept
{
	return _size.y;
}

std::uint32_t
GraphicsTextureDesc::getDepth() const noexcept
{
	return _size.z;
}

const uint3&
GraphicsTextureDesc::getSize() const noexcept
{
	return _size;
}

void
GraphicsTextureDesc::setTexFormat(GraphicsFormat format) noexcept
{
	assert(format >= GraphicsFormat::GraphicsFormatBeginRange && format <= GraphicsFormat::GraphicsFormatEndRange);
	_format = format;
}

void
GraphicsTextureDesc::setTexDim(GraphicsTextureDim dim) noexcept
{
	assert(dim >= GraphicsTextureDim::GraphicsTextureDimBeginRange && dim <= GraphicsTextureDim::GraphicsTextureDimEndRange);
	_dim = dim;
}

void
GraphicsTextureDesc::setTexTiling(GraphicsImageTiling tiling) noexcept
{
	_tiling = tiling;
}

void
GraphicsTextureDesc::setTexUsage(std::uint32_t flags) noexcept
{
	_textureUsage = flags;
}

GraphicsFormat
GraphicsTextureDesc::getTexFormat() const noexcept
{
	return _format;
}

GraphicsTextureDim
GraphicsTextureDesc::getTexDim() const noexcept
{
	return _dim;
}

std::uint32_t
GraphicsTextureDesc::getTexUsage() const noexcept
{
	return _textureUsage;
}

GraphicsImageTiling
GraphicsTextureDesc::getTexTiling() const noexcept
{
	return _tiling;
}

void
GraphicsTextureDesc::setSamplerWrap(GraphicsSamplerWrap wrap) noexcept
{
	_wrap = wrap;
}

void
GraphicsTextureDesc::setSamplerFilter(GraphicsSamplerFilter filter) noexcept
{
	_filter = filter;
}

void
GraphicsTextureDesc::setSamplerAnis(GraphicsSamplerAnis anis) noexcept
{
	_anis = anis;
}

GraphicsSamplerWrap
GraphicsTextureDesc::getSamplerWrap() const noexcept
{
	return _wrap;
}

GraphicsSamplerFilter
GraphicsTextureDesc::getSamplerFilter() const noexcept
{
	return _filter;
}

GraphicsSamplerAnis
GraphicsTextureDesc::getSamplerAnis() const noexcept
{
	return _anis;
}

void
GraphicsTextureDesc::setMultisample(bool enable) noexcept
{
	_multisample = enable;
}

bool
GraphicsTextureDesc::isMultiSample() const noexcept
{
	return _multisample;
}

void
GraphicsTextureDesc::setLayerNums(std::uint32_t layer) noexcept
{
	_layer = layer;
}

std::uint32_t
GraphicsTextureDesc::getLayerNums() const noexcept
{
	return _layer;
}

void
GraphicsTextureDesc::setLayerBase(std::uint32_t minLayer) noexcept
{
	_layerBase = minLayer;
}

std::uint32_t
GraphicsTextureDesc::getLayerBase() const noexcept
{
	return _layerBase;
}

void
GraphicsTextureDesc::setMipLevel(std::uint32_t level) noexcept
{
	_mipLevel = level;
}

void
GraphicsTextureDesc::setMipBase(std::uint32_t level) noexcept
{
	_mipBase = level;
}

std::uint32_t
GraphicsTextureDesc::getMipLevel() const noexcept
{
	return _mipLevel;
}

std::uint32_t
GraphicsTextureDesc::getMipBase() const noexcept
{
	return _mipBase;
}

void
GraphicsTextureDesc::setStream(void* data) noexcept
{
	_data = data;
}

void
GraphicsTextureDesc::setStreamSize(std::uint32_t size) noexcept
{
	_dataSize = size;
}

void*
GraphicsTextureDesc::getStream() const noexcept
{
	return _data;
}

std::uint32_t
GraphicsTextureDesc::getStreamSize() const noexcept
{
	return _dataSize;
}

GraphicsTexture::GraphicsTexture() noexcept
{
}

GraphicsTexture::~GraphicsTexture() noexcept
{
}

_NAME_END