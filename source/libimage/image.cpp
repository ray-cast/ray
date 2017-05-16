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
#include <ray/image.h>
#include <ray/imagutil.h>
#include <ray/ioserver.h>
#include <ray/math.h>
#include "imagall.h"

_NAME_BEGIN

namespace image
{
Image::Image() noexcept
{
	this->_init();
}

Image::Image(Image&& move) noexcept
	: _format(move._format)
	, _width(move._width)
	, _height(move._height)
	, _depth(move._depth)
	, _mipLevel(move._mipLevel)
	, _mipBase(move._mipBase)
	, _layerBase(move._layerBase)
	, _layerLevel(move._layerLevel)
	, _size(move._size)
	, _data(std::move(move._data))
{
}

Image::~Image() noexcept
{
	this->clear();
}

bool
Image::create(std::uint32_t width, std::uint32_t height, format_t format, bool clear) noexcept
{
	return this->create(width, height, 1, format, clear);
}

bool
Image::create(std::uint32_t width, std::uint32_t height, std::uint32_t depth, format_t format, bool clear) noexcept
{
	return this->create(width, height, depth, format, 1, 1, 0, 0, clear);
}

bool 
Image::create(std::uint32_t width, std::uint32_t height, std::uint32_t depth, format_t format, std::uint32_t mipLevel, std::uint32_t layerLevel, std::uint32_t mipBase, std::uint32_t layerBase, bool clear) noexcept
{
	assert(mipLevel >= 1);
	assert(layerLevel >= 1);
	assert(width >= 1 && height >= 1 && depth >= 1);
	assert(format >= format_t::BeginRange && format <= format_t::EndRange);

	this->clear();

	std::uint32_t w = width;
	std::uint32_t h = height;

	std::size_t destLength = 0;

	switch (this->value_type(format))
	{
	case value_t::SNorm:
	case value_t::UNorm:
	case value_t::SInt:
	case value_t::UInt:
	case value_t::SScaled:
	case value_t::UScaled:
	case value_t::SRGB:
	case value_t::Float:
	{
		auto channel = this->channel(format);
		auto type_size = this->type_size(format);

		std::uint32_t pixelSize = type_size * channel;

		for (std::uint32_t mip = mipBase; mip < mipBase + mipLevel; mip++)
		{
			std::size_t mipSize = w * h * depth * pixelSize;

			destLength += mipSize * layerLevel;

			w = std::max(w >> 1, (std::uint32_t)1);
			h = std::max(h >> 1, (std::uint32_t)1);
		}
	}
	break;
	case value_t::Compressed:
	{
		std::uint32_t blockSize = 16;
		if (format == format_t::BC1RGBUNormBlock ||
			format == format_t::BC1RGBSRGBBlock ||
			format == format_t::BC1RGBAUNormBlock ||
			format == format_t::BC1RGBASRGBBlock)
		{
			blockSize = 8;
		}

		for (std::uint32_t mip = mipBase; mip < mipBase + mipLevel; mip++)
		{
			auto mipSize = ((w + 3) / 4) * ((h + 3) / 4) * depth * blockSize;

			destLength += mipSize * layerLevel;

			w = std::max(w >> 1, (std::uint32_t)1);
			h = std::max(h >> 1, (std::uint32_t)1);
		}
	}
	break;
	case value_t::UNorm5_6_5:
	case value_t::UNorm5_5_5_1:
	case value_t::UNorm1_5_5_5:
	case value_t::UNorm2_10_10_10:
	case value_t::UFloatB10G11R11Pack32:
	case value_t::UFloatE5B9G9R9Pack32:
	case value_t::D16UNorm_S8UInt:
	case value_t::D24UNorm_S8UInt:
	case value_t::D24UNormPack32:
	case value_t::D32_SFLOAT_S8UInt:
	default:
		assert(false);
		return 0;
	}

	if (destLength == 0)
		return false;

	_format = format;

	_width = width;
	_height = height;
	_depth = depth;
	_size = destLength;
	_mipBase = mipBase;
	_mipLevel = mipLevel;
	_layerBase = layerBase;
	_layerLevel = layerLevel;
	_data = std::make_unique<std::uint8_t[]>(destLength);

	if (clear)
		std::memset(_data.get(), 0, (std::size_t)this->size());

	return true;
}

bool
Image::create(const Image& image, format_t format) noexcept
{
	assert(format >= format_t::BeginRange && format <= format_t::EndRange);

	if (_format != format && format != format_t::Undefined)
	{
		if (!this->create(image.width(), image.height(), image.depth(), format, image.mipLevel(), image.layerLevel(), image.mipBase(), image.layerBase(), true))
			return false;

		if (_format == format_t::R32G32B32SFloat && format == format_t::R8G8B8UInt)
			rgb32f_to_rgb8uint(image, *this);
		else if (_format == format_t::R32G32B32A32SFloat && format == format_t::R8G8B8A8UInt)
			rgba32f_to_rgba8uint(image, *this);
		else if (_format == format_t::R64G64B64A64SFloat && format == format_t::R8G8B8UInt)
			rgb64f_to_rgb8uint(image, *this);
		else if (_format == format_t::R64G64B64A64SFloat && format == format_t::R8G8B8A8UInt)
			rgba64f_to_rgba8uint(image, *this);
		else if (_format == format_t::R32G32B32SFloat && format == format_t::R8G8B8SInt)
			rgb32f_to_rgb8sint(image, *this);
		else if (_format == format_t::R32G32B32A32SFloat && format == format_t::R8G8B8A8SInt)
			rgba32f_to_rgba8sint(image, *this);
		else if (_format == format_t::R64G64B64A64SFloat && format == format_t::R8G8B8SInt)
			rgb64f_to_rgb8sint(image, *this);
		else if (_format == format_t::R64G64B64A64SFloat && format == format_t::R8G8B8A8SInt)
			rgba64f_to_rgba8sint(image, *this);
		else
			return false;

		return true;
	}
	else
	{
		if (!this->create(image.width(), image.height(), image.depth(), image.format(), image.mipLevel(), image.layerLevel(), image.mipBase(), image.layerBase(), true))
			return false;

		std::memcpy((char*)this->data(), image.data(), image.size());

		return true;
	}
}

void
Image::_init() noexcept
{
	_width = 0;
	_height = 0;
	_depth = 0;
	_size = 0;
	_mipBase = 0;
	_mipLevel = 0;
	_layerBase = 0;
	_layerLevel = 0;
	_data = nullptr;

	_format = format_t::Undefined;
}

void
Image::clear() noexcept
{
	_data.reset();
	this->_init();
}

bool 
Image::empty() const noexcept 
{
	return _data == nullptr; 
}

std::uint32_t 
Image::width() const noexcept
{
	return _width; 
}

std::uint32_t 
Image::height() const noexcept 
{
	return _height; 
}

std::uint32_t 
Image::depth() const noexcept 
{
	return _depth; 
}

std::size_t 
Image::size() const noexcept 
{
	return _size; 
}

format_t
Image::format() const noexcept
{
	return _format;
}

std::uint32_t
Image::mipBase() const noexcept
{
	return _mipBase;
}

std::uint32_t
Image::mipLevel() const noexcept
{
	return _mipLevel;
}

std::uint32_t 
Image::layerBase() const noexcept
{
	return _layerBase;
}

std::uint32_t 
Image::layerLevel() const noexcept
{
	return _layerLevel;
}

const char*
Image::data() const noexcept
{
	return (char*)_data.get();
}

bool
Image::load(StreamReader& stream, const char* type) noexcept
{
	ImageHandlerPtr impl = image::findHandler(stream, type);
	if (image::findHandler(stream, type))
	{
		if (impl->doLoad(stream, *this))
			return true;
	}

	return false;
}

bool
Image::load(const std::string& filename, const char* type) noexcept
{
	StreamReaderPtr stream;
	if (IoServer::instance()->openFile(stream, filename))
		return this->load(*stream, type);
	return false;
}

bool
Image::save(const std::string& filename, const char* type) noexcept
{
	StreamWritePtr stream;
	if (IoServer::instance()->openFile(stream, filename))
		return this->save(*stream, type);

	return false;
}

bool
Image::save(StreamWrite& stream, const char* type) noexcept
{
	if (stream.good())
	{
		ImageHandlerPtr impl = image::findHandler(type);
		if (impl)
		{
			if (!impl->doSave(stream, *this))
				return false;

			return true;
		}
	}

	return false;
}

std::uint8_t 
Image::channel(format_t format) noexcept
{
	switch (format)
	{
	case format_t::L8UNorm:
	case format_t::L8SNorm:
	case format_t::L8UScaled:
	case format_t::L8SScaled:
	case format_t::L8UInt:
	case format_t::L8SInt:
	case format_t::L8SRGB:
	case format_t::A8UNorm:
	case format_t::A8SNorm:
	case format_t::A8UScaled:
	case format_t::A8SScaled:
	case format_t::A8UInt:
	case format_t::A8SInt:
	case format_t::A8SRGB:
	case format_t::R8UNorm:
	case format_t::R8SNorm:
	case format_t::R8UScaled:
	case format_t::R8SScaled:
	case format_t::R8UInt:
	case format_t::R8SInt:
	case format_t::R8SRGB:
	case format_t::L16UNorm:
	case format_t::L16SNorm:
	case format_t::L16UScaled:
	case format_t::L16SScaled:
	case format_t::L16UInt:
	case format_t::L16SInt:
	case format_t::L16SFloat:
	case format_t::A16UNorm:
	case format_t::A16SNorm:
	case format_t::A16UScaled:
	case format_t::A16SScaled:
	case format_t::A16UInt:
	case format_t::A16SInt:
	case format_t::A16SFloat:
	case format_t::R16UNorm:
	case format_t::R16SNorm:
	case format_t::R16UScaled:
	case format_t::R16SScaled:
	case format_t::R16UInt:
	case format_t::R16SInt:
	case format_t::R16SFloat:
	case format_t::R32UInt:
	case format_t::R32SInt:
	case format_t::R32SFloat:
	case format_t::R64UInt:
	case format_t::R64SInt:
	case format_t::R64SFloat:
	case format_t::X8_D24UNormPack32:
	case format_t::D16UNorm:
	case format_t::D32_SFLOAT:
	case format_t::S8UInt:
	case format_t::D32_SFLOAT_S8UInt:
		return 1;
	case format_t::R4G4UNormPack8:
	case format_t::L8A8UNorm:
	case format_t::L8A8SNorm:
	case format_t::L8A8UScaled:
	case format_t::L8A8SScaled:
	case format_t::L8A8UInt:
	case format_t::L8A8SInt:
	case format_t::L8A8SRGB:
	case format_t::R8G8UNorm:
	case format_t::R8G8SNorm:
	case format_t::R8G8UScaled:
	case format_t::R8G8SScaled:
	case format_t::R8G8UInt:
	case format_t::R8G8SInt:
	case format_t::R8G8SRGB:
	case format_t::L16A16UNorm:
	case format_t::L16A16SNorm:
	case format_t::L16A16UScaled:
	case format_t::L16A16SScaled:
	case format_t::L16A16UInt:
	case format_t::L16A16SInt:
	case format_t::L16A16SRGB:
	case format_t::R16G16UNorm:
	case format_t::R16G16SNorm:
	case format_t::R16G16UScaled:
	case format_t::R16G16SScaled:
	case format_t::R16G16UInt:
	case format_t::R16G16SInt:
	case format_t::R16G16SFloat:
	case format_t::R32G32UInt:
	case format_t::R32G32SInt:
	case format_t::R32G32SFloat:
	case format_t::R64G64UInt:
	case format_t::R64G64SInt:
	case format_t::R64G64SFloat:
	case format_t::D16UNorm_S8UInt:
	case format_t::D24UNorm_S8UInt:
		return 2;
	case format_t::R5G6B5UNormPack16:
	case format_t::B5G6R5UNormPack16:
	case format_t::R8G8B8UNorm:
	case format_t::R8G8B8SNorm:
	case format_t::R8G8B8UScaled:
	case format_t::R8G8B8SScaled:
	case format_t::R8G8B8UInt:
	case format_t::R8G8B8SInt:
	case format_t::R8G8B8SRGB:
	case format_t::B8G8R8UNorm:
	case format_t::B8G8R8SNorm:
	case format_t::B8G8R8UScaled:
	case format_t::B8G8R8SScaled:
	case format_t::B8G8R8UInt:
	case format_t::B8G8R8SInt:
	case format_t::B8G8R8SRGB:
	case format_t::R16G16B16UNorm:
	case format_t::R16G16B16SNorm:
	case format_t::R16G16B16UScaled:
	case format_t::R16G16B16SScaled:
	case format_t::R16G16B16UInt:
	case format_t::R16G16B16SInt:
	case format_t::R16G16B16SFloat:
	case format_t::R32G32B32UInt:
	case format_t::R32G32B32SInt:
	case format_t::R32G32B32SFloat:
	case format_t::R64G64B64UInt:
	case format_t::R64G64B64SInt:
	case format_t::R64G64B64SFloat:
	case format_t::B10G11R11UFloatPack32:
	case format_t::E5B9G9R9UFloatPack32:
		return 3;
	case format_t::R4G4B4A4UNormPack16:
	case format_t::B4G4R4A4UNormPack16:
	case format_t::R5G5B5A1UNormPack16:
	case format_t::B5G5R5A1UNormPack16:
	case format_t::A1R5G5B5UNormPack16:
	case format_t::R8G8B8A8UNorm:
	case format_t::R8G8B8A8SNorm:
	case format_t::R8G8B8A8UScaled:
	case format_t::R8G8B8A8SScaled:
	case format_t::R8G8B8A8UInt:
	case format_t::R8G8B8A8SInt:
	case format_t::R8G8B8A8SRGB:
	case format_t::B8G8R8A8UNorm:
	case format_t::B8G8R8A8SNorm:
	case format_t::B8G8R8A8UScaled:
	case format_t::B8G8R8A8SScaled:
	case format_t::B8G8R8A8UInt:
	case format_t::B8G8R8A8SInt:
	case format_t::B8G8R8A8SRGB:
	case format_t::A8B8G8R8UNormPack32:
	case format_t::A8B8G8R8SNormPack32:
	case format_t::A8B8G8R8UScaledPack32:
	case format_t::A8B8G8R8SScaledPack32:
	case format_t::A8B8G8R8UIntPack32:
	case format_t::A8B8G8R8SIntPack32:
	case format_t::A8B8G8R8SRGBPack32:
	case format_t::A2R10G10B10UNormPack32:
	case format_t::A2R10G10B10SNormPack32:
	case format_t::A2R10G10B10UScaledPack32:
	case format_t::A2R10G10B10SScaledPack32:
	case format_t::A2R10G10B10UIntPack32:
	case format_t::A2R10G10B10SIntPack32:
	case format_t::A2B10G10R10UNormPack32:
	case format_t::A2B10G10R10SNormPack32:
	case format_t::A2B10G10R10UScaledPack32:
	case format_t::A2B10G10R10SScaledPack32:
	case format_t::A2B10G10R10UIntPack32:
	case format_t::A2B10G10R10SIntPack32:
	case format_t::R16G16B16A16UNorm:
	case format_t::R16G16B16A16SNorm:
	case format_t::R16G16B16A16UScaled:
	case format_t::R16G16B16A16SScaled:
	case format_t::R16G16B16A16UInt:
	case format_t::R16G16B16A16SInt:
	case format_t::R16G16B16A16SFloat:
	case format_t::R32G32B32A32UInt:
	case format_t::R32G32B32A32SInt:
	case format_t::R32G32B32A32SFloat:
	case format_t::R64G64B64A64UInt:
	case format_t::R64G64B64A64SInt:
	case format_t::R64G64B64A64SFloat:
		return 4;
	case format_t::BC1RGBUNormBlock:
	case format_t::BC1RGBSRGBBlock:
	case format_t::BC1RGBAUNormBlock:
	case format_t::BC1RGBASRGBBlock:
	case format_t::BC2UNormBlock:
	case format_t::BC2SRGBBlock:
	case format_t::BC3UNormBlock:
	case format_t::BC3SRGBBlock:
	case format_t::BC4UNormBlock:
	case format_t::BC4SNormBlock:
	case format_t::BC5UNormBlock:
	case format_t::BC5SNormBlock:
	case format_t::BC6HUFloatBlock:
	case format_t::BC6HSFloatBlock:
	case format_t::BC7UNormBlock:
	case format_t::BC7SRGBBlock:
	case format_t::ETC2R8G8B8UNormBlock:
	case format_t::ETC2R8G8B8SRGBBlock:
	case format_t::ETC2R8G8B8A1UNormBlock:
	case format_t::ETC2R8G8B8A1SRGBBlock:
	case format_t::ETC2R8G8B8A8UNormBlock:
	case format_t::ETC2R8G8B8A8SRGBBlock:
	case format_t::EACR11UNormBlock:
	case format_t::EACR11SNormBlock:
	case format_t::EACR11G11UNormBlock:
	case format_t::EACR11G11SNormBlock:
	case format_t::ASTC4x4UNormBlock:
	case format_t::ASTC4x4SRGBBlock:
	case format_t::ASTC5x4UNormBlock:
	case format_t::ASTC5x4SRGBBlock:
	case format_t::ASTC5x5UNormBlock:
	case format_t::ASTC5x5SRGBBlock:
	case format_t::ASTC6x5UNormBlock:
	case format_t::ASTC6x5SRGBBlock:
	case format_t::ASTC6x6UNormBlock:
	case format_t::ASTC6x6SRGBBlock:
	case format_t::ASTC8x5UNormBlock:
	case format_t::ASTC8x5SRGBBlock:
	case format_t::ASTC8x6UNormBlock:
	case format_t::ASTC8x6SRGBBlock:
	case format_t::ASTC8x8UNormBlock:
	case format_t::ASTC8x8SRGBBlock:
	case format_t::ASTC10x5UNormBlock:
	case format_t::ASTC10x5SRGBBlock:
	case format_t::ASTC10x6UNormBlock:
	case format_t::ASTC10x6SRGBBlock:
	case format_t::ASTC10x8UNormBlock:
	case format_t::ASTC10x8SRGBBlock:
	case format_t::ASTC10x10UNormBlock:
	case format_t::ASTC10x10SRGBBlock:
	case format_t::ASTC12x10UNormBlock:
	case format_t::ASTC12x10SRGBBlock:
	case format_t::ASTC12x12UNormBlock:
	case format_t::ASTC12x12SRGBBlock:
	default:
		assert(false);
		return 0;
	}
}

std::uint8_t 
Image::channel() const noexcept
{
	return channel(_format);
}

std::uint8_t 
Image::type_size(format_t format) noexcept
{
	switch (format)
	{
	case format_t::R4G4UNormPack8:
	case format_t::R5G6B5UNormPack16:
	case format_t::B5G6R5UNormPack16:
	case format_t::L8UNorm:
	case format_t::L8SNorm:
	case format_t::L8UScaled:
	case format_t::L8SScaled:
	case format_t::L8UInt:
	case format_t::L8SInt:
	case format_t::L8SRGB:
	case format_t::A8UNorm:
	case format_t::A8SNorm:
	case format_t::A8UScaled:
	case format_t::A8SScaled:
	case format_t::A8UInt:
	case format_t::A8SInt:
	case format_t::A8SRGB:
	case format_t::R8UNorm:
	case format_t::R8SNorm:
	case format_t::R8UScaled:
	case format_t::R8SScaled:
	case format_t::R8UInt:
	case format_t::R8SInt:
	case format_t::R8SRGB:
	case format_t::S8UInt:
	case format_t::L8A8UNorm:
	case format_t::L8A8SNorm:
	case format_t::L8A8UScaled:
	case format_t::L8A8SScaled:
	case format_t::L8A8UInt:
	case format_t::L8A8SInt:
	case format_t::L8A8SRGB:
	case format_t::R8G8UNorm:
	case format_t::R8G8SNorm:
	case format_t::R8G8UScaled:
	case format_t::R8G8SScaled:
	case format_t::R8G8UInt:
	case format_t::R8G8SInt:
	case format_t::R8G8SRGB:
	case format_t::R8G8B8UNorm:
	case format_t::R8G8B8SNorm:
	case format_t::R8G8B8UScaled:
	case format_t::R8G8B8SScaled:
	case format_t::R8G8B8UInt:
	case format_t::R8G8B8SInt:
	case format_t::R8G8B8SRGB:
	case format_t::B8G8R8UNorm:
	case format_t::B8G8R8SNorm:
	case format_t::B8G8R8UScaled:
	case format_t::B8G8R8SScaled:
	case format_t::B8G8R8UInt:
	case format_t::B8G8R8SInt:
	case format_t::B8G8R8SRGB:
	case format_t::R8G8B8A8UNorm:
	case format_t::R8G8B8A8SNorm:
	case format_t::R8G8B8A8UScaled:
	case format_t::R8G8B8A8SScaled:
	case format_t::R8G8B8A8UInt:
	case format_t::R8G8B8A8SInt:
	case format_t::R8G8B8A8SRGB:
	case format_t::B8G8R8A8UNorm:
	case format_t::B8G8R8A8SNorm:
	case format_t::B8G8R8A8UScaled:
	case format_t::B8G8R8A8SScaled:
	case format_t::B8G8R8A8UInt:
	case format_t::B8G8R8A8SInt:
	case format_t::B8G8R8A8SRGB:
	case format_t::A8B8G8R8UNormPack32:
	case format_t::A8B8G8R8SNormPack32:
	case format_t::A8B8G8R8UScaledPack32:
	case format_t::A8B8G8R8SScaledPack32:
	case format_t::A8B8G8R8UIntPack32:
	case format_t::A8B8G8R8SIntPack32:
	case format_t::A8B8G8R8SRGBPack32:
		return 1;
	case format_t::L16UNorm:
	case format_t::L16SNorm:
	case format_t::L16UScaled:
	case format_t::L16SScaled:
	case format_t::L16UInt:
	case format_t::L16SInt:
	case format_t::L16SFloat:
	case format_t::A16UNorm:
	case format_t::A16SNorm:
	case format_t::A16UScaled:
	case format_t::A16SScaled:
	case format_t::A16UInt:
	case format_t::A16SInt:
	case format_t::A16SFloat:
	case format_t::R16UNorm:
	case format_t::R16SNorm:
	case format_t::R16UScaled:
	case format_t::R16SScaled:
	case format_t::R16UInt:
	case format_t::R16SInt:
	case format_t::R16SFloat:
	case format_t::D16UNorm:
	case format_t::L16A16UNorm:
	case format_t::L16A16SNorm:
	case format_t::L16A16UScaled:
	case format_t::L16A16SScaled:
	case format_t::L16A16UInt:
	case format_t::L16A16SInt:
	case format_t::L16A16SRGB:
	case format_t::R16G16UNorm:
	case format_t::R16G16SNorm:
	case format_t::R16G16UScaled:
	case format_t::R16G16SScaled:
	case format_t::R16G16UInt:
	case format_t::R16G16SInt:
	case format_t::R16G16SFloat:
	case format_t::R16G16B16UNorm:
	case format_t::R16G16B16SNorm:
	case format_t::R16G16B16UScaled:
	case format_t::R16G16B16SScaled:
	case format_t::R16G16B16UInt:
	case format_t::R16G16B16SInt:
	case format_t::R16G16B16SFloat:
	case format_t::R16G16B16A16UNorm:
	case format_t::R16G16B16A16SNorm:
	case format_t::R16G16B16A16UScaled:
	case format_t::R16G16B16A16SScaled:
	case format_t::R16G16B16A16UInt:
	case format_t::R16G16B16A16SInt:
	case format_t::R16G16B16A16SFloat:
		return 2;
	case format_t::R32UInt:
	case format_t::R32SInt:
	case format_t::R32SFloat:
	case format_t::R32G32UInt:
	case format_t::R32G32SInt:
	case format_t::R32G32SFloat:
	case format_t::R32G32B32UInt:
	case format_t::R32G32B32SInt:
	case format_t::R32G32B32SFloat:
	case format_t::R32G32B32A32UInt:
	case format_t::R32G32B32A32SInt:
	case format_t::R32G32B32A32SFloat:
		return 4;
	case format_t::R64UInt:
	case format_t::R64SInt:
	case format_t::R64SFloat:
	case format_t::R64G64UInt:
	case format_t::R64G64SInt:
	case format_t::R64G64SFloat:
	case format_t::R64G64B64UInt:
	case format_t::R64G64B64SInt:
	case format_t::R64G64B64SFloat:
	case format_t::R64G64B64A64UInt:
	case format_t::R64G64B64A64SInt:
	case format_t::R64G64B64A64SFloat:
		return 8;
	case format_t::R4G4B4A4UNormPack16:
	case format_t::B4G4R4A4UNormPack16:
	case format_t::R5G5B5A1UNormPack16:
	case format_t::B5G5R5A1UNormPack16:
	case format_t::A1R5G5B5UNormPack16:
	case format_t::X8_D24UNormPack32:
	case format_t::D16UNorm_S8UInt:
	case format_t::D24UNorm_S8UInt:
	case format_t::D32_SFLOAT:
	case format_t::D32_SFLOAT_S8UInt:
	case format_t::B10G11R11UFloatPack32:
	case format_t::E5B9G9R9UFloatPack32:
	case format_t::A2R10G10B10UNormPack32:
	case format_t::A2R10G10B10SNormPack32:
	case format_t::A2R10G10B10UScaledPack32:
	case format_t::A2R10G10B10SScaledPack32:
	case format_t::A2R10G10B10UIntPack32:
	case format_t::A2R10G10B10SIntPack32:
	case format_t::A2B10G10R10UNormPack32:
	case format_t::A2B10G10R10SNormPack32:
	case format_t::A2B10G10R10UScaledPack32:
	case format_t::A2B10G10R10SScaledPack32:
	case format_t::A2B10G10R10UIntPack32:
	case format_t::A2B10G10R10SIntPack32:
	case format_t::BC1RGBUNormBlock:
	case format_t::BC1RGBSRGBBlock:
	case format_t::BC1RGBAUNormBlock:
	case format_t::BC1RGBASRGBBlock:
	case format_t::BC2UNormBlock:
	case format_t::BC2SRGBBlock:
	case format_t::BC3UNormBlock:
	case format_t::BC3SRGBBlock:
	case format_t::BC4UNormBlock:
	case format_t::BC4SNormBlock:
	case format_t::BC5UNormBlock:
	case format_t::BC5SNormBlock:
	case format_t::BC6HUFloatBlock:
	case format_t::BC6HSFloatBlock:
	case format_t::BC7UNormBlock:
	case format_t::BC7SRGBBlock:
	case format_t::ETC2R8G8B8UNormBlock:
	case format_t::ETC2R8G8B8SRGBBlock:
	case format_t::ETC2R8G8B8A1UNormBlock:
	case format_t::ETC2R8G8B8A1SRGBBlock:
	case format_t::ETC2R8G8B8A8UNormBlock:
	case format_t::ETC2R8G8B8A8SRGBBlock:
	case format_t::EACR11UNormBlock:
	case format_t::EACR11SNormBlock:
	case format_t::EACR11G11UNormBlock:
	case format_t::EACR11G11SNormBlock:
	case format_t::ASTC4x4UNormBlock:
	case format_t::ASTC4x4SRGBBlock:
	case format_t::ASTC5x4UNormBlock:
	case format_t::ASTC5x4SRGBBlock:
	case format_t::ASTC5x5UNormBlock:
	case format_t::ASTC5x5SRGBBlock:
	case format_t::ASTC6x5UNormBlock:
	case format_t::ASTC6x5SRGBBlock:
	case format_t::ASTC6x6UNormBlock:
	case format_t::ASTC6x6SRGBBlock:
	case format_t::ASTC8x5UNormBlock:
	case format_t::ASTC8x5SRGBBlock:
	case format_t::ASTC8x6UNormBlock:
	case format_t::ASTC8x6SRGBBlock:
	case format_t::ASTC8x8UNormBlock:
	case format_t::ASTC8x8SRGBBlock:
	case format_t::ASTC10x5UNormBlock:
	case format_t::ASTC10x5SRGBBlock:
	case format_t::ASTC10x6UNormBlock:
	case format_t::ASTC10x6SRGBBlock:
	case format_t::ASTC10x8UNormBlock:
	case format_t::ASTC10x8SRGBBlock:
	case format_t::ASTC10x10UNormBlock:
	case format_t::ASTC10x10SRGBBlock:
	case format_t::ASTC12x10UNormBlock:
	case format_t::ASTC12x10SRGBBlock:
	case format_t::ASTC12x12UNormBlock:
	case format_t::ASTC12x12SRGBBlock:
	default:
		assert(false);
		return 0;
	}
}

std::uint8_t 
Image::type_size() const noexcept
{
	return type_size(_format);
}

Image::value_t
Image::value_type(format_t format) noexcept
{
	switch (format)
	{
	case format_t::R4G4UNormPack8:
	case format_t::R4G4B4A4UNormPack16:
	case format_t::B4G4R4A4UNormPack16:
	case format_t::L8UNorm:
	case format_t::A8UNorm:
	case format_t::R8UNorm:
	case format_t::L8A8UNorm:
	case format_t::R8G8UNorm:
	case format_t::R8G8B8UNorm:
	case format_t::B8G8R8UNorm:
	case format_t::R8G8B8A8UNorm:
	case format_t::B8G8R8A8UNorm:
	case format_t::A8B8G8R8UNormPack32:
	case format_t::L16UNorm:
	case format_t::A16UNorm:
	case format_t::R16UNorm:
	case format_t::L16A16UNorm:
	case format_t::R16G16UNorm:
	case format_t::R16G16B16UNorm:
	case format_t::R16G16B16A16UNorm:
	case format_t::D16UNorm:
		return value_t::UNorm;
	case format_t::R5G6B5UNormPack16:
	case format_t::B5G6R5UNormPack16:
		return value_t::UNorm5_6_5;
	case format_t::R5G5B5A1UNormPack16:
	case format_t::B5G5R5A1UNormPack16:
		return value_t::UNorm5_5_5_1;
	case format_t::A1R5G5B5UNormPack16:
		return value_t::UNorm1_5_5_5;
	case format_t::B10G11R11UFloatPack32:
		return value_t::UFloatB10G11R11Pack32;
	case format_t::E5B9G9R9UFloatPack32:
		return value_t::UFloatE5B9G9R9Pack32;
	case format_t::L8SNorm:
	case format_t::A8SNorm:
	case format_t::R8SNorm:
	case format_t::L8A8SNorm:
	case format_t::R8G8SNorm:
	case format_t::R8G8B8SNorm:
	case format_t::B8G8R8SNorm:
	case format_t::R8G8B8A8SNorm:
	case format_t::B8G8R8A8SNorm:
	case format_t::A8B8G8R8SNormPack32:
	case format_t::L16SNorm:
	case format_t::A16SNorm:
	case format_t::R16SNorm:
	case format_t::L16A16SNorm:
	case format_t::R16G16SNorm:
	case format_t::R16G16B16SNorm:
	case format_t::R16G16B16A16SNorm:
		return value_t::SNorm;
	case format_t::A2R10G10B10UNormPack32:
	case format_t::A2B10G10R10UNormPack32:
	case format_t::A2R10G10B10SNormPack32:
	case format_t::A2B10G10R10SNormPack32:
		return value_t::UNorm2_10_10_10;
	case format_t::X8_D24UNormPack32:
		return value_t::D24UNormPack32;
	case format_t::D16UNorm_S8UInt:
		return value_t::D16UNorm_S8UInt;
	case format_t::D24UNorm_S8UInt:
		return value_t::D24UNorm_S8UInt;
	case format_t::D32_SFLOAT_S8UInt:
		return value_t::D32_SFLOAT_S8UInt;
	case format_t::L8UScaled:
	case format_t::A8UScaled:
	case format_t::R8UScaled:
	case format_t::L8A8UScaled:
	case format_t::R8G8UScaled:
	case format_t::R8G8B8UScaled:
	case format_t::B8G8R8UScaled:
	case format_t::R8G8B8A8UScaled:
	case format_t::B8G8R8A8UScaled:
	case format_t::A8B8G8R8UScaledPack32:
	case format_t::A2R10G10B10UScaledPack32:
	case format_t::A2B10G10R10UScaledPack32:
	case format_t::L16UScaled:
	case format_t::A16UScaled:
	case format_t::R16UScaled:
	case format_t::L16A16UScaled:
	case format_t::R16G16UScaled:
	case format_t::R16G16B16UScaled:
	case format_t::R16G16B16A16UScaled:
	case format_t::L8SScaled:
	case format_t::A8SScaled:
	case format_t::R8SScaled:
	case format_t::L8A8SScaled:
	case format_t::R8G8SScaled:
	case format_t::R8G8B8SScaled:
	case format_t::B8G8R8SScaled:
	case format_t::R8G8B8A8SScaled:
	case format_t::B8G8R8A8SScaled:
	case format_t::A8B8G8R8SScaledPack32:
	case format_t::A2R10G10B10SScaledPack32:
	case format_t::A2B10G10R10SScaledPack32:
	case format_t::L16SScaled:
	case format_t::A16SScaled:
	case format_t::R16SScaled:
	case format_t::L16A16SScaled:
	case format_t::R16G16SScaled:
	case format_t::R16G16B16SScaled:
	case format_t::R16G16B16A16SScaled:
		return value_t::UScaled;
	case format_t::L8UInt:
	case format_t::A8UInt:
	case format_t::R8UInt:
	case format_t::L8A8UInt:
	case format_t::R8G8UInt:
	case format_t::R8G8B8UInt:
	case format_t::B8G8R8UInt:
	case format_t::R8G8B8A8UInt:
	case format_t::B8G8R8A8UInt:
	case format_t::A8B8G8R8UIntPack32:
	case format_t::A2R10G10B10UIntPack32:
	case format_t::A2B10G10R10UIntPack32:
	case format_t::L16UInt:
	case format_t::A16UInt:
	case format_t::R16UInt:
	case format_t::L16A16UInt:
	case format_t::R16G16UInt:
	case format_t::R16G16B16UInt:
	case format_t::R16G16B16A16UInt:
	case format_t::R32UInt:
	case format_t::R32G32UInt:
	case format_t::R32G32B32UInt:
	case format_t::R32G32B32A32UInt:
	case format_t::R64UInt:
	case format_t::R64G64UInt:
	case format_t::R64G64B64UInt:
	case format_t::R64G64B64A64UInt:
	case format_t::S8UInt:
		return value_t::UInt;
	case format_t::L8SRGB:
	case format_t::A8SRGB:
	case format_t::R8SRGB:
	case format_t::L8A8SRGB:
	case format_t::R8G8SRGB:
	case format_t::R8G8B8SRGB:
	case format_t::B8G8R8SRGB:
	case format_t::R8G8B8A8SRGB:
	case format_t::B8G8R8A8SRGB:
	case format_t::A8B8G8R8SRGBPack32:
	case format_t::L16A16SRGB:
		return value_t::SRGB;
	case format_t::L8SInt:
	case format_t::A8SInt:
	case format_t::R8SInt:
	case format_t::L8A8SInt:
	case format_t::R8G8SInt:
	case format_t::R8G8B8SInt:
	case format_t::B8G8R8SInt:
	case format_t::R8G8B8A8SInt:
	case format_t::B8G8R8A8SInt:
	case format_t::A8B8G8R8SIntPack32:
	case format_t::A2R10G10B10SIntPack32:
	case format_t::A2B10G10R10SIntPack32:
	case format_t::L16SInt:
	case format_t::A16SInt:
	case format_t::R16SInt:
	case format_t::L16A16SInt:
	case format_t::R16G16SInt:
	case format_t::R16G16B16SInt:
	case format_t::R16G16B16A16SInt:
	case format_t::R32SInt:
	case format_t::R32G32SInt:
	case format_t::R32G32B32SInt:
	case format_t::R32G32B32A32SInt:
	case format_t::R64SInt:
	case format_t::R64G64SInt:
	case format_t::R64G64B64SInt:
	case format_t::R64G64B64A64SInt:
		return value_t::SInt;
	case format_t::L16SFloat:
	case format_t::A16SFloat:
	case format_t::R16SFloat:
	case format_t::R16G16SFloat:
	case format_t::R16G16B16SFloat:
	case format_t::R16G16B16A16SFloat:
	case format_t::R32SFloat:
	case format_t::R32G32SFloat:
	case format_t::R32G32B32SFloat:
	case format_t::R32G32B32A32SFloat:
	case format_t::R64SFloat:
	case format_t::R64G64SFloat:
	case format_t::R64G64B64SFloat:
	case format_t::R64G64B64A64SFloat:
	case format_t::D32_SFLOAT:
		return value_t::Float;
	case format_t::BC1RGBUNormBlock:
	case format_t::BC1RGBAUNormBlock:
	case format_t::BC1RGBSRGBBlock:
	case format_t::BC1RGBASRGBBlock:
	case format_t::BC2UNormBlock:
	case format_t::BC3UNormBlock:
	case format_t::BC4UNormBlock:
	case format_t::BC4SNormBlock:
	case format_t::BC5UNormBlock:
	case format_t::BC5SNormBlock:
	case format_t::BC6HSFloatBlock:
	case format_t::BC7UNormBlock:
	case format_t::ETC2R8G8B8UNormBlock:
	case format_t::ETC2R8G8B8A1UNormBlock:
	case format_t::ETC2R8G8B8A8UNormBlock:
	case format_t::EACR11UNormBlock:
	case format_t::EACR11G11UNormBlock:
	case format_t::EACR11SNormBlock:
	case format_t::EACR11G11SNormBlock:
	case format_t::ASTC4x4UNormBlock:
	case format_t::ASTC5x4UNormBlock:
	case format_t::ASTC5x5UNormBlock:
	case format_t::ASTC6x5UNormBlock:
	case format_t::ASTC6x6UNormBlock:
	case format_t::ASTC8x5UNormBlock:
	case format_t::ASTC8x6UNormBlock:
	case format_t::ASTC8x8UNormBlock:
	case format_t::ASTC10x5UNormBlock:
	case format_t::ASTC10x6UNormBlock:
	case format_t::ASTC10x8UNormBlock:
	case format_t::ASTC10x10UNormBlock:
	case format_t::ASTC12x10UNormBlock:
	case format_t::ASTC12x12UNormBlock:
	case format_t::BC2SRGBBlock:
	case format_t::BC3SRGBBlock:
	case format_t::BC6HUFloatBlock:
	case format_t::BC7SRGBBlock:
	case format_t::ETC2R8G8B8SRGBBlock:
	case format_t::ETC2R8G8B8A1SRGBBlock:
	case format_t::ETC2R8G8B8A8SRGBBlock:
	case format_t::ASTC4x4SRGBBlock:
	case format_t::ASTC5x4SRGBBlock:
	case format_t::ASTC5x5SRGBBlock:
	case format_t::ASTC6x5SRGBBlock:
	case format_t::ASTC6x6SRGBBlock:
	case format_t::ASTC8x5SRGBBlock:
	case format_t::ASTC8x6SRGBBlock:
	case format_t::ASTC8x8SRGBBlock:
	case format_t::ASTC10x5SRGBBlock:
	case format_t::ASTC10x6SRGBBlock:
	case format_t::ASTC10x8SRGBBlock:
	case format_t::ASTC10x10SRGBBlock:
	case format_t::ASTC12x10SRGBBlock:
	case format_t::ASTC12x12SRGBBlock:
		return value_t::Compressed;
	default:
		assert(false);
		return value_t::Null;
	}
}

value_t 
Image::value_type() const noexcept
{
	return value_type(_format);
}

Image::swizzle_t
Image::swizzle_type(format_t format) noexcept
{
	switch (format)
	{
	case format_t::R8UNorm:
	case format_t::R8SNorm:
	case format_t::R8UScaled:
	case format_t::R8SScaled:
	case format_t::R8UInt:
	case format_t::R8SInt:
	case format_t::R8SRGB:
	case format_t::R16UNorm:
	case format_t::R16SNorm:
	case format_t::R16UScaled:
	case format_t::R16SScaled:
	case format_t::R16UInt:
	case format_t::R16SInt:
	case format_t::R16SFloat:
	case format_t::R32UInt:
	case format_t::R32SInt:
	case format_t::R32SFloat:
	case format_t::R64UInt:
	case format_t::R64SInt:
	case format_t::R64SFloat:
		return swizzle_t::R;
	case format_t::R4G4UNormPack8:
	case format_t::R8G8UNorm:
	case format_t::R8G8SNorm:
	case format_t::R8G8UScaled:
	case format_t::R8G8SScaled:
	case format_t::R8G8UInt:
	case format_t::R8G8SInt:
	case format_t::R8G8SRGB:
	case format_t::R16G16UNorm:
	case format_t::R16G16SNorm:
	case format_t::R16G16UScaled:
	case format_t::R16G16SScaled:
	case format_t::R16G16UInt:
	case format_t::R16G16SInt:
	case format_t::R16G16SFloat:
	case format_t::R32G32UInt:
	case format_t::R32G32SInt:
	case format_t::R32G32SFloat:
	case format_t::R64G64UInt:
	case format_t::R64G64SInt:
	case format_t::R64G64SFloat:
		return swizzle_t::RG;
	case format_t::R5G6B5UNormPack16:
	case format_t::R8G8B8UNorm:
	case format_t::R8G8B8SNorm:
	case format_t::R8G8B8UScaled:
	case format_t::R8G8B8SScaled:
	case format_t::R8G8B8UInt:
	case format_t::R8G8B8SInt:
	case format_t::R8G8B8SRGB:
	case format_t::R16G16B16UNorm:
	case format_t::R16G16B16SNorm:
	case format_t::R16G16B16UScaled:
	case format_t::R16G16B16SScaled:
	case format_t::R16G16B16UInt:
	case format_t::R16G16B16SInt:
	case format_t::R16G16B16SFloat:
	case format_t::R32G32B32UInt:
	case format_t::R32G32B32SInt:
	case format_t::R32G32B32SFloat:
	case format_t::R64G64B64UInt:
	case format_t::R64G64B64SInt:
	case format_t::R64G64B64SFloat:
		return swizzle_t::RGB;
	case format_t::B5G6R5UNormPack16:
	case format_t::B8G8R8UNorm:
	case format_t::B8G8R8SNorm:
	case format_t::B8G8R8UScaled:
	case format_t::B8G8R8SScaled:
	case format_t::B8G8R8UInt:
	case format_t::B8G8R8SInt:
	case format_t::B8G8R8SRGB:
	case format_t::B10G11R11UFloatPack32:
		return swizzle_t::BGR;
	case format_t::R4G4B4A4UNormPack16:
	case format_t::R5G5B5A1UNormPack16:
	case format_t::A1R5G5B5UNormPack16:
	case format_t::R8G8B8A8UNorm:
	case format_t::R8G8B8A8SNorm:
	case format_t::R8G8B8A8UScaled:
	case format_t::R8G8B8A8SScaled:
	case format_t::R8G8B8A8UInt:
	case format_t::R8G8B8A8SInt:
	case format_t::R8G8B8A8SRGB:
	case format_t::A2R10G10B10UNormPack32:
	case format_t::A2R10G10B10SNormPack32:
	case format_t::A2R10G10B10UScaledPack32:
	case format_t::A2R10G10B10SScaledPack32:
	case format_t::A2R10G10B10UIntPack32:
	case format_t::A2R10G10B10SIntPack32:
	case format_t::R16G16B16A16UNorm:
	case format_t::R16G16B16A16SNorm:
	case format_t::R16G16B16A16UScaled:
	case format_t::R16G16B16A16SScaled:
	case format_t::R16G16B16A16UInt:
	case format_t::R16G16B16A16SInt:
	case format_t::R16G16B16A16SFloat:
	case format_t::R32G32B32A32UInt:
	case format_t::R32G32B32A32SInt:
	case format_t::R32G32B32A32SFloat:
	case format_t::R64G64B64A64UInt:
	case format_t::R64G64B64A64SInt:
	case format_t::R64G64B64A64SFloat:
		return swizzle_t::RGBA;
	case format_t::B4G4R4A4UNormPack16:
	case format_t::B5G5R5A1UNormPack16:
	case format_t::B8G8R8A8UNorm:
	case format_t::B8G8R8A8SNorm:
	case format_t::B8G8R8A8UScaled:
	case format_t::B8G8R8A8SScaled:
	case format_t::B8G8R8A8UInt:
	case format_t::B8G8R8A8SInt:
	case format_t::B8G8R8A8SRGB:
	case format_t::A2B10G10R10UNormPack32:
	case format_t::A2B10G10R10SNormPack32:
	case format_t::A2B10G10R10UScaledPack32:
	case format_t::A2B10G10R10SScaledPack32:
	case format_t::A2B10G10R10UIntPack32:
	case format_t::A2B10G10R10SIntPack32:
		return swizzle_t::BGRA;
	case format_t::A8B8G8R8UNormPack32:
	case format_t::A8B8G8R8SNormPack32:
	case format_t::A8B8G8R8UScaledPack32:
	case format_t::A8B8G8R8SScaledPack32:
	case format_t::A8B8G8R8UIntPack32:
	case format_t::A8B8G8R8SIntPack32:
	case format_t::A8B8G8R8SRGBPack32:
		return swizzle_t::ABGR;
	case format_t::D16UNorm:
	case format_t::X8_D24UNormPack32:
	case format_t::D32_SFLOAT:
		return swizzle_t::Depth;
	case format_t::S8UInt:
		return swizzle_t::Stencil;
	case format_t::D16UNorm_S8UInt:
	case format_t::D24UNorm_S8UInt:
	case format_t::D32_SFLOAT_S8UInt:
		return swizzle_t::DepthStencil;
	default:
		assert(false);
		return swizzle_t::DepthStencil;
	}
}

Image::swizzle_t
Image::swizzle_type() const noexcept
{
	return swizzle_type(_format);
}

}

_NAME_END