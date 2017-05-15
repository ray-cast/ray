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
	this->destroy();
}

bool
Image::create(std::uint32_t width, std::uint32_t height, ImageFormat format, bool clear) noexcept
{
	return this->create(width, height, 1, format, clear);
}

bool
Image::create(std::uint32_t width, std::uint32_t height, std::uint32_t depth, ImageFormat format, bool clear) noexcept
{
	return this->create(width, height, depth, format, 1, 1, 0, 0, clear);
}

bool 
Image::create(std::uint32_t width, std::uint32_t height, std::uint32_t depth, ImageFormat format, std::uint32_t mipLevel, std::uint32_t layerLevel, std::uint32_t mipBase, std::uint32_t layerBase, bool clear) noexcept
{
	assert(mipLevel >= 1);
	assert(layerLevel >= 1);
	assert(width >= 1 && height >= 1 && depth >= 1);
	assert(format >= ImageFormat::ImageFormatBeginRange && format <= ImageFormat::ImageFormatEndRange);

	this->destroy();

	std::uint32_t w = width;
	std::uint32_t h = height;
	std::size_t destLength = 0;

	auto type = image::type(format);
	if (type == ImageType::ImageTypeCompressed)
	{
		std::uint32_t blockSize = 16;
		if (format == ImageFormat::BC1RGBUNormBlock ||
			format == ImageFormat::BC1RGBSRGBBlock ||
			format == ImageFormat::BC1RGBAUNormBlock ||
			format == ImageFormat::BC1RGBASRGBBlock)
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
	else
	{
		std::uint32_t bpp;
		if (format == ImageFormat::R8UNorm)
			bpp = 8;
		else if (format == ImageFormat::R8G8UNorm)
			bpp = 16;
		else if (format == ImageFormat::R8G8B8UNorm || format == ImageFormat::B8G8R8UNorm ||
				format == ImageFormat::R8G8B8SNorm || format == ImageFormat::B8G8R8SNorm ||
				format == ImageFormat::R8G8B8SInt || format == ImageFormat::B8G8R8SInt ||
				format == ImageFormat::R8G8B8UInt || format == ImageFormat::B8G8R8UInt)
			bpp = 24;
		else if (format == ImageFormat::R8G8B8A8UNorm || format == ImageFormat::B8G8R8A8UNorm)
			bpp = 32;
		else if (format == ImageFormat::R16SFloat)
			bpp = 16;
		else if (format == ImageFormat::R16G16SFloat)
			bpp = 32;
		else if (format == ImageFormat::R16G16B16SFloat)
			bpp = 48;
		else if (format == ImageFormat::R16G16B16A16SFloat)
			bpp = 64;
		else if (format == ImageFormat::R32SFloat)
			bpp = 32;
		else if (format == ImageFormat::R32G32SFloat)
			bpp = 64;
		else if (format == ImageFormat::R32G32B32SFloat)
			bpp = 96;
		else if (format == ImageFormat::R32G32B32A32SFloat)
			bpp = 128;
		else
			return false;

		std::uint32_t pixelSize = (bpp >> 3);

		for (std::uint32_t mip = mipBase; mip < mipBase + mipLevel; mip++)
		{
			std::size_t mipSize = w * h * depth * pixelSize;

			destLength += mipSize * layerLevel;

			w = std::max(w >> 1, (std::uint32_t)1);
			h = std::max(h >> 1, (std::uint32_t)1);
		}
	}

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
	{
		std::memset(_data.get(), 0, (std::size_t)this->size());
	}

	return true;
}

bool
Image::create(const Image& image, ImageFormat format) noexcept
{
	assert(format >= ImageFormat::ImageFormatBeginRange && format <= ImageFormat::ImageFormatEndRange);

	if (_format != format && format != ImageFormat::Undefined)
	{
		if (!this->create(image.width(), image.height(), image.depth(), format, image.mipLevel(), image.layerLevel(), image.mipBase(), image.layerBase(), true))
			return false;

		Image image;
		if (!image.create(_width, _height, _depth, format, _mipLevel, _layerLevel, _mipBase, _layerBase, true))
			return false;

		if (_format == ImageFormat::R32G32B32SFloat && format == ImageFormat::R8G8B8UInt)
		{
			auto src = (float3*)this->data();
			auto dst = (Vector3t<std::uint8_t>*)image.data();

			for (std::uint32_t i = 0; i < _width * _height * _depth; i++)
			{
				dst[i] = math::clamp<std::uint8_t>(Vector3t<std::uint8_t>(src[i] * 255.0f), 0, 255);
			}
		}

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

	_format = ImageFormat::ImageFormatMaxEnum;
}

void
Image::destroy() noexcept
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

std::uint32_t 
Image::size() const noexcept 
{
	return _size; 
}

ImageFormat
Image::format() const noexcept
{
	return _format;
}

std::uint32_t
Image::channel() const noexcept
{
	switch (_format)
	{
	case ImageFormat::L8UNorm:
	case ImageFormat::L8SNorm:
	case ImageFormat::L8UScaled:
	case ImageFormat::L8SScaled:
	case ImageFormat::L8UInt:
	case ImageFormat::L8SInt:
	case ImageFormat::L8SRGB:
	case ImageFormat::A8UNorm:
	case ImageFormat::A8SNorm:
	case ImageFormat::A8UScaled:
	case ImageFormat::A8SScaled:
	case ImageFormat::A8UInt:
	case ImageFormat::A8SInt:
	case ImageFormat::A8SRGB:
	case ImageFormat::R8UNorm:
	case ImageFormat::R8SNorm:
	case ImageFormat::R8UScaled:
	case ImageFormat::R8SScaled:
	case ImageFormat::R8UInt:
	case ImageFormat::R8SInt:
	case ImageFormat::R8SRGB:
	case ImageFormat::L16UNorm:
	case ImageFormat::L16SNorm:
	case ImageFormat::L16UScaled:
	case ImageFormat::L16SScaled:
	case ImageFormat::L16UInt:
	case ImageFormat::L16SInt:
	case ImageFormat::L16SFloat:
	case ImageFormat::A16UNorm:
	case ImageFormat::A16SNorm:
	case ImageFormat::A16UScaled:
	case ImageFormat::A16SScaled:
	case ImageFormat::A16UInt:
	case ImageFormat::A16SInt:
	case ImageFormat::A16SFloat:
	case ImageFormat::R16UNorm:
	case ImageFormat::R16SNorm:
	case ImageFormat::R16UScaled:
	case ImageFormat::R16SScaled:
	case ImageFormat::R16UInt:
	case ImageFormat::R16SInt:
	case ImageFormat::R16SFloat:
	case ImageFormat::R32UInt:
	case ImageFormat::R32SInt:
	case ImageFormat::R32SFloat:
	case ImageFormat::R64UInt:
	case ImageFormat::R64SInt:
	case ImageFormat::R64SFloat:
	case ImageFormat::X8_D24UNormPack32:
	case ImageFormat::D16UNorm:
	case ImageFormat::D32_SFLOAT:
	case ImageFormat::S8UInt:
	case ImageFormat::D32_SFLOAT_S8UInt:
		return 1;
	case ImageFormat::R4G4UNormPack8:
	case ImageFormat::L8A8UNorm:
	case ImageFormat::L8A8SNorm:
	case ImageFormat::L8A8UScaled:
	case ImageFormat::L8A8SScaled:
	case ImageFormat::L8A8UInt:
	case ImageFormat::L8A8SInt:
	case ImageFormat::L8A8SRGB:
	case ImageFormat::R8G8UNorm:
	case ImageFormat::R8G8SNorm:
	case ImageFormat::R8G8UScaled:
	case ImageFormat::R8G8SScaled:
	case ImageFormat::R8G8UInt:
	case ImageFormat::R8G8SInt:
	case ImageFormat::R8G8SRGB:
	case ImageFormat::L16A16UNorm:
	case ImageFormat::L16A16SNorm:
	case ImageFormat::L16A16UScaled:
	case ImageFormat::L16A16SScaled:
	case ImageFormat::L16A16UInt:
	case ImageFormat::L16A16SInt:
	case ImageFormat::L16A16SRGB:
	case ImageFormat::R16G16UNorm:
	case ImageFormat::R16G16SNorm:
	case ImageFormat::R16G16UScaled:
	case ImageFormat::R16G16SScaled:
	case ImageFormat::R16G16UInt:
	case ImageFormat::R16G16SInt:
	case ImageFormat::R16G16SFloat:
	case ImageFormat::R32G32UInt:
	case ImageFormat::R32G32SInt:
	case ImageFormat::R32G32SFloat:
	case ImageFormat::R64G64UInt:
	case ImageFormat::R64G64SInt:
	case ImageFormat::R64G64SFloat:
	case ImageFormat::D16UNorm_S8UInt:
	case ImageFormat::D24UNorm_S8UInt:
		return 2;
	case ImageFormat::R5G6B5UNormPack16:
	case ImageFormat::B5G6R5UNormPack16:
	case ImageFormat::R8G8B8UNorm:
	case ImageFormat::R8G8B8SNorm:
	case ImageFormat::R8G8B8UScaled:
	case ImageFormat::R8G8B8SScaled:
	case ImageFormat::R8G8B8UInt:
	case ImageFormat::R8G8B8SInt:
	case ImageFormat::R8G8B8SRGB:
	case ImageFormat::B8G8R8UNorm:
	case ImageFormat::B8G8R8SNorm:
	case ImageFormat::B8G8R8UScaled:
	case ImageFormat::B8G8R8SScaled:
	case ImageFormat::B8G8R8UInt:
	case ImageFormat::B8G8R8SInt:
	case ImageFormat::B8G8R8SRGB:
	case ImageFormat::R16G16B16UNorm:
	case ImageFormat::R16G16B16SNorm:
	case ImageFormat::R16G16B16UScaled:
	case ImageFormat::R16G16B16SScaled:
	case ImageFormat::R16G16B16UInt:
	case ImageFormat::R16G16B16SInt:
	case ImageFormat::R16G16B16SFloat:
	case ImageFormat::R32G32B32UInt:
	case ImageFormat::R32G32B32SInt:
	case ImageFormat::R32G32B32SFloat:
	case ImageFormat::R64G64B64UInt:
	case ImageFormat::R64G64B64SInt:
	case ImageFormat::R64G64B64SFloat:
	case ImageFormat::B10G11R11UFloatPack32:
	case ImageFormat::E5B9G9R9UFloatPack32:
		return 3;
	case ImageFormat::R4G4B4A4UNormPack16:
	case ImageFormat::B4G4R4A4UNormPack16:
	case ImageFormat::R5G5B5A1UNormPack16:
	case ImageFormat::B5G5R5A1UNormPack16:
	case ImageFormat::A1R5G5B5UNormPack16:
	case ImageFormat::R8G8B8A8UNorm:
	case ImageFormat::R8G8B8A8SNorm:
	case ImageFormat::R8G8B8A8UScaled:
	case ImageFormat::R8G8B8A8SScaled:
	case ImageFormat::R8G8B8A8UInt:
	case ImageFormat::R8G8B8A8SInt:
	case ImageFormat::R8G8B8A8SRGB:
	case ImageFormat::B8G8R8A8UNorm:
	case ImageFormat::B8G8R8A8SNorm:
	case ImageFormat::B8G8R8A8UScaled:
	case ImageFormat::B8G8R8A8SScaled:
	case ImageFormat::B8G8R8A8UInt:
	case ImageFormat::B8G8R8A8SInt:
	case ImageFormat::B8G8R8A8SRGB:
	case ImageFormat::A8B8G8R8UNormPack32:
	case ImageFormat::A8B8G8R8SNormPack32:
	case ImageFormat::A8B8G8R8UScaledPack32:
	case ImageFormat::A8B8G8R8SScaledPack32:
	case ImageFormat::A8B8G8R8UIntPack32:
	case ImageFormat::A8B8G8R8SIntPack32:
	case ImageFormat::A8B8G8R8SRGBPack32:
	case ImageFormat::A2R10G10B10UNormPack32:
	case ImageFormat::A2R10G10B10SNormPack32:
	case ImageFormat::A2R10G10B10UScaledPack32:
	case ImageFormat::A2R10G10B10SScaledPack32:
	case ImageFormat::A2R10G10B10UIntPack32:
	case ImageFormat::A2R10G10B10SIntPack32:
	case ImageFormat::A2B10G10R10UNormPack32:
	case ImageFormat::A2B10G10R10SNormPack32:
	case ImageFormat::A2B10G10R10UScaledPack32:
	case ImageFormat::A2B10G10R10SScaledPack32:
	case ImageFormat::A2B10G10R10UIntPack32:
	case ImageFormat::A2B10G10R10SIntPack32:
	case ImageFormat::R16G16B16A16UNorm:
	case ImageFormat::R16G16B16A16SNorm:
	case ImageFormat::R16G16B16A16UScaled:
	case ImageFormat::R16G16B16A16SScaled:
	case ImageFormat::R16G16B16A16UInt:
	case ImageFormat::R16G16B16A16SInt:
	case ImageFormat::R16G16B16A16SFloat:
	case ImageFormat::R32G32B32A32UInt:
	case ImageFormat::R32G32B32A32SInt:
	case ImageFormat::R32G32B32A32SFloat:
	case ImageFormat::R64G64B64A64UInt:
	case ImageFormat::R64G64B64A64SInt:
	case ImageFormat::R64G64B64A64SFloat:
		return 4;
	case ImageFormat::BC1RGBUNormBlock:
	case ImageFormat::BC1RGBSRGBBlock:
	case ImageFormat::BC1RGBAUNormBlock:
	case ImageFormat::BC1RGBASRGBBlock:
	case ImageFormat::BC2UNormBlock:
	case ImageFormat::BC2SRGBBlock:
	case ImageFormat::BC3UNormBlock:
	case ImageFormat::BC3SRGBBlock:
	case ImageFormat::BC4UNormBlock:
	case ImageFormat::BC4SNormBlock:
	case ImageFormat::BC5UNormBlock:
	case ImageFormat::BC5SNormBlock:
	case ImageFormat::BC6HUFloatBlock:
	case ImageFormat::BC6HSFloatBlock:
	case ImageFormat::BC7UNormBlock:
	case ImageFormat::BC7SRGBBlock:
	case ImageFormat::ETC2R8G8B8UNormBlock:
	case ImageFormat::ETC2R8G8B8SRGBBlock:
	case ImageFormat::ETC2R8G8B8A1UNormBlock:
	case ImageFormat::ETC2R8G8B8A1SRGBBlock:
	case ImageFormat::ETC2R8G8B8A8UNormBlock:
	case ImageFormat::ETC2R8G8B8A8SRGBBlock:
	case ImageFormat::EACR11UNormBlock:
	case ImageFormat::EACR11SNormBlock:
	case ImageFormat::EACR11G11UNormBlock:
	case ImageFormat::EACR11G11SNormBlock:
	case ImageFormat::ASTC4x4UNormBlock:
	case ImageFormat::ASTC4x4SRGBBlock:
	case ImageFormat::ASTC5x4UNormBlock:
	case ImageFormat::ASTC5x4SRGBBlock:
	case ImageFormat::ASTC5x5UNormBlock:
	case ImageFormat::ASTC5x5SRGBBlock:
	case ImageFormat::ASTC6x5UNormBlock:
	case ImageFormat::ASTC6x5SRGBBlock:
	case ImageFormat::ASTC6x6UNormBlock:
	case ImageFormat::ASTC6x6SRGBBlock:
	case ImageFormat::ASTC8x5UNormBlock:
	case ImageFormat::ASTC8x5SRGBBlock:
	case ImageFormat::ASTC8x6UNormBlock:
	case ImageFormat::ASTC8x6SRGBBlock:
	case ImageFormat::ASTC8x8UNormBlock:
	case ImageFormat::ASTC8x8SRGBBlock:
	case ImageFormat::ASTC10x5UNormBlock:
	case ImageFormat::ASTC10x5SRGBBlock:
	case ImageFormat::ASTC10x6UNormBlock:
	case ImageFormat::ASTC10x6SRGBBlock:
	case ImageFormat::ASTC10x8UNormBlock:
	case ImageFormat::ASTC10x8SRGBBlock:
	case ImageFormat::ASTC10x10UNormBlock:
	case ImageFormat::ASTC10x10SRGBBlock:
	case ImageFormat::ASTC12x10UNormBlock:
	case ImageFormat::ASTC12x10SRGBBlock:
	case ImageFormat::ASTC12x12UNormBlock:
	case ImageFormat::ASTC12x12SRGBBlock:
	default:
		assert(false);
		return 0;
	}
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

ImageType type(ImageFormat format) noexcept
{
	switch (format)
	{
	case ImageFormat::R4G4UNormPack8:
	case ImageFormat::R4G4B4A4UNormPack16:
	case ImageFormat::B4G4R4A4UNormPack16:
	case ImageFormat::L8UNorm:
	case ImageFormat::A8UNorm:
	case ImageFormat::R8UNorm:
	case ImageFormat::L8A8UNorm:
	case ImageFormat::R8G8UNorm:
	case ImageFormat::R8G8B8UNorm:
	case ImageFormat::B8G8R8UNorm:
	case ImageFormat::R8G8B8A8UNorm:
	case ImageFormat::B8G8R8A8UNorm:
	case ImageFormat::A8B8G8R8UNormPack32:
	case ImageFormat::L16UNorm:
	case ImageFormat::A16UNorm:
	case ImageFormat::R16UNorm:
	case ImageFormat::L16A16UNorm:
	case ImageFormat::R16G16UNorm:
	case ImageFormat::R16G16B16UNorm:
	case ImageFormat::R16G16B16A16UNorm:
	case ImageFormat::D16UNorm:
		return ImageType::ImageTypeUNorm;
	case ImageFormat::R5G6B5UNormPack16:
	case ImageFormat::B5G6R5UNormPack16:
		return ImageType::ImageTypeUNorm5_6_5;
	case ImageFormat::R5G5B5A1UNormPack16:
	case ImageFormat::B5G5R5A1UNormPack16:
		return ImageType::ImageTypeUNorm5_5_5_1;
	case ImageFormat::A1R5G5B5UNormPack16:
		return ImageType::ImageTypeUNorm1_5_5_5;
	case ImageFormat::L8SNorm:
	case ImageFormat::A8SNorm:
	case ImageFormat::R8SNorm:
	case ImageFormat::L8A8SNorm:
	case ImageFormat::R8G8SNorm:
	case ImageFormat::R8G8B8SNorm:
	case ImageFormat::B8G8R8SNorm:
	case ImageFormat::R8G8B8A8SNorm:
	case ImageFormat::B8G8R8A8SNorm:
	case ImageFormat::A8B8G8R8SNormPack32:
	case ImageFormat::L16SNorm:
	case ImageFormat::A16SNorm:
	case ImageFormat::R16SNorm:
	case ImageFormat::L16A16SNorm:
	case ImageFormat::R16G16SNorm:
	case ImageFormat::R16G16B16SNorm:
	case ImageFormat::R16G16B16A16SNorm:
		return ImageType::ImageTypeSNorm;
	case ImageFormat::A2R10G10B10UNormPack32:
	case ImageFormat::A2B10G10R10UNormPack32:
	case ImageFormat::A2R10G10B10SNormPack32:
	case ImageFormat::A2B10G10R10SNormPack32:
		return ImageType::ImageTypeUNorm2_10_10_10;
	case ImageFormat::X8_D24UNormPack32:
		return ImageType::ImageTypeD24UNormPack32;
	case ImageFormat::D16UNorm_S8UInt:
		return ImageType::ImageTypeD16UNorm_S8UInt;
	case ImageFormat::D24UNorm_S8UInt:
		return ImageType::ImageTypeD24UNorm_S8UInt;
	case ImageFormat::D32_SFLOAT_S8UInt:
		return ImageType::ImageTypeD32_SFLOAT_S8UInt;
	case ImageFormat::L8UScaled:
	case ImageFormat::A8UScaled:
	case ImageFormat::R8UScaled:
	case ImageFormat::L8A8UScaled:
	case ImageFormat::R8G8UScaled:
	case ImageFormat::R8G8B8UScaled:
	case ImageFormat::B8G8R8UScaled:
	case ImageFormat::R8G8B8A8UScaled:
	case ImageFormat::B8G8R8A8UScaled:
	case ImageFormat::A8B8G8R8UScaledPack32:
	case ImageFormat::A2R10G10B10UScaledPack32:
	case ImageFormat::A2B10G10R10UScaledPack32:
	case ImageFormat::L16UScaled:
	case ImageFormat::A16UScaled:
	case ImageFormat::R16UScaled:
	case ImageFormat::L16A16UScaled:
	case ImageFormat::R16G16UScaled:
	case ImageFormat::R16G16B16UScaled:
	case ImageFormat::R16G16B16A16UScaled:
	case ImageFormat::L8SScaled:
	case ImageFormat::A8SScaled:
	case ImageFormat::R8SScaled:
	case ImageFormat::L8A8SScaled:
	case ImageFormat::R8G8SScaled:
	case ImageFormat::R8G8B8SScaled:
	case ImageFormat::B8G8R8SScaled:
	case ImageFormat::R8G8B8A8SScaled:
	case ImageFormat::B8G8R8A8SScaled:
	case ImageFormat::A8B8G8R8SScaledPack32:
	case ImageFormat::A2R10G10B10SScaledPack32:
	case ImageFormat::A2B10G10R10SScaledPack32:
	case ImageFormat::L16SScaled:
	case ImageFormat::A16SScaled:
	case ImageFormat::R16SScaled:
	case ImageFormat::L16A16SScaled:
	case ImageFormat::R16G16SScaled:
	case ImageFormat::R16G16B16SScaled:
	case ImageFormat::R16G16B16A16SScaled:
		return ImageType::ImageTypeUScaled;
	case ImageFormat::L8UInt:
	case ImageFormat::A8UInt:
	case ImageFormat::R8UInt:
	case ImageFormat::L8A8UInt:
	case ImageFormat::R8G8UInt:
	case ImageFormat::R8G8B8UInt:
	case ImageFormat::B8G8R8UInt:
	case ImageFormat::R8G8B8A8UInt:
	case ImageFormat::B8G8R8A8UInt:
	case ImageFormat::A8B8G8R8UIntPack32:
	case ImageFormat::A2R10G10B10UIntPack32:
	case ImageFormat::A2B10G10R10UIntPack32:
	case ImageFormat::L16UInt:
	case ImageFormat::A16UInt:
	case ImageFormat::R16UInt:
	case ImageFormat::L16A16UInt:
	case ImageFormat::R16G16UInt:
	case ImageFormat::R16G16B16UInt:
	case ImageFormat::R16G16B16A16UInt:
	case ImageFormat::R32UInt:
	case ImageFormat::R32G32UInt:
	case ImageFormat::R32G32B32UInt:
	case ImageFormat::R32G32B32A32UInt:
	case ImageFormat::R64UInt:
	case ImageFormat::R64G64UInt:
	case ImageFormat::R64G64B64UInt:
	case ImageFormat::R64G64B64A64UInt:
	case ImageFormat::S8UInt:
	case ImageFormat::L8SRGB:
	case ImageFormat::A8SRGB:
	case ImageFormat::R8SRGB:
	case ImageFormat::L8A8SRGB:
	case ImageFormat::R8G8SRGB:
	case ImageFormat::R8G8B8SRGB:
	case ImageFormat::B8G8R8SRGB:
	case ImageFormat::R8G8B8A8SRGB:
	case ImageFormat::B8G8R8A8SRGB:
	case ImageFormat::A8B8G8R8SRGBPack32:
	case ImageFormat::L16A16SRGB:
		return ImageType::ImageTypeUInt;
	case ImageFormat::L8SInt:
	case ImageFormat::A8SInt:
	case ImageFormat::R8SInt:
	case ImageFormat::L8A8SInt:
	case ImageFormat::R8G8SInt:
	case ImageFormat::R8G8B8SInt:
	case ImageFormat::B8G8R8SInt:
	case ImageFormat::R8G8B8A8SInt:
	case ImageFormat::B8G8R8A8SInt:
	case ImageFormat::A8B8G8R8SIntPack32:
	case ImageFormat::A2R10G10B10SIntPack32:
	case ImageFormat::A2B10G10R10SIntPack32:
	case ImageFormat::L16SInt:
	case ImageFormat::A16SInt:
	case ImageFormat::R16SInt:
	case ImageFormat::L16A16SInt:
	case ImageFormat::R16G16SInt:
	case ImageFormat::R16G16B16SInt:
	case ImageFormat::R16G16B16A16SInt:
	case ImageFormat::R32SInt:
	case ImageFormat::R32G32SInt:
	case ImageFormat::R32G32B32SInt:
	case ImageFormat::R32G32B32A32SInt:
	case ImageFormat::R64SInt:
	case ImageFormat::R64G64SInt:
	case ImageFormat::R64G64B64SInt:
	case ImageFormat::R64G64B64A64SInt:
	case ImageFormat::L16SFloat:
	case ImageFormat::A16SFloat:
	case ImageFormat::R16SFloat:
	case ImageFormat::R16G16SFloat:
	case ImageFormat::R16G16B16SFloat:
	case ImageFormat::R16G16B16A16SFloat:
	case ImageFormat::R32SFloat:
	case ImageFormat::R32G32SFloat:
	case ImageFormat::R32G32B32SFloat:
	case ImageFormat::R32G32B32A32SFloat:
	case ImageFormat::R64SFloat:
	case ImageFormat::R64G64SFloat:
	case ImageFormat::R64G64B64SFloat:
	case ImageFormat::R64G64B64A64SFloat:
	case ImageFormat::BC6HSFloatBlock:
	case ImageFormat::B10G11R11UFloatPack32:
	case ImageFormat::E5B9G9R9UFloatPack32:
	case ImageFormat::D32_SFLOAT:
		return ImageType::ImageTypeFloat;
	case ImageFormat::BC1RGBUNormBlock:
	case ImageFormat::BC1RGBAUNormBlock:
	case ImageFormat::BC1RGBSRGBBlock:
	case ImageFormat::BC1RGBASRGBBlock:
	case ImageFormat::BC2UNormBlock:
	case ImageFormat::BC3UNormBlock:
	case ImageFormat::BC4UNormBlock:
	case ImageFormat::BC4SNormBlock:
	case ImageFormat::BC5UNormBlock:
	case ImageFormat::BC5SNormBlock:
	case ImageFormat::BC7UNormBlock:
	case ImageFormat::ETC2R8G8B8UNormBlock:
	case ImageFormat::ETC2R8G8B8A1UNormBlock:
	case ImageFormat::ETC2R8G8B8A8UNormBlock:
	case ImageFormat::EACR11UNormBlock:
	case ImageFormat::EACR11G11UNormBlock:
	case ImageFormat::EACR11SNormBlock:
	case ImageFormat::EACR11G11SNormBlock:
	case ImageFormat::ASTC4x4UNormBlock:
	case ImageFormat::ASTC5x4UNormBlock:
	case ImageFormat::ASTC5x5UNormBlock:
	case ImageFormat::ASTC6x5UNormBlock:
	case ImageFormat::ASTC6x6UNormBlock:
	case ImageFormat::ASTC8x5UNormBlock:
	case ImageFormat::ASTC8x6UNormBlock:
	case ImageFormat::ASTC8x8UNormBlock:
	case ImageFormat::ASTC10x5UNormBlock:
	case ImageFormat::ASTC10x6UNormBlock:
	case ImageFormat::ASTC10x8UNormBlock:
	case ImageFormat::ASTC10x10UNormBlock:
	case ImageFormat::ASTC12x10UNormBlock:
	case ImageFormat::ASTC12x12UNormBlock:
	case ImageFormat::BC2SRGBBlock:
	case ImageFormat::BC3SRGBBlock:
	case ImageFormat::BC6HUFloatBlock:
	case ImageFormat::BC7SRGBBlock:
	case ImageFormat::ETC2R8G8B8SRGBBlock:
	case ImageFormat::ETC2R8G8B8A1SRGBBlock:
	case ImageFormat::ETC2R8G8B8A8SRGBBlock:
	case ImageFormat::ASTC4x4SRGBBlock:
	case ImageFormat::ASTC5x4SRGBBlock:
	case ImageFormat::ASTC5x5SRGBBlock:
	case ImageFormat::ASTC6x5SRGBBlock:
	case ImageFormat::ASTC6x6SRGBBlock:
	case ImageFormat::ASTC8x5SRGBBlock:
	case ImageFormat::ASTC8x6SRGBBlock:
	case ImageFormat::ASTC8x8SRGBBlock:
	case ImageFormat::ASTC10x5SRGBBlock:
	case ImageFormat::ASTC10x6SRGBBlock:
	case ImageFormat::ASTC10x8SRGBBlock:
	case ImageFormat::ASTC10x10SRGBBlock:
	case ImageFormat::ASTC12x10SRGBBlock:
	case ImageFormat::ASTC12x12SRGBBlock:
		return ImageType::ImageTypeCompressed;
	default:
		assert(false);
		return ImageType::ImageTypeMaxEnum;
	}
}

ImageType type(const Image& image) noexcept
{
	return type(image.format());
}

ImageOrder order(ImageFormat format) noexcept
{
	switch (format)
	{
	case ImageFormat::R8UNorm:
	case ImageFormat::R8SNorm:
	case ImageFormat::R8UScaled:
	case ImageFormat::R8SScaled:
	case ImageFormat::R8UInt:
	case ImageFormat::R8SInt:
	case ImageFormat::R8SRGB:
	case ImageFormat::R16UNorm:
	case ImageFormat::R16SNorm:
	case ImageFormat::R16UScaled:
	case ImageFormat::R16SScaled:
	case ImageFormat::R16UInt:
	case ImageFormat::R16SInt:
	case ImageFormat::R16SFloat:
	case ImageFormat::R32UInt:
	case ImageFormat::R32SInt:
	case ImageFormat::R32SFloat:
	case ImageFormat::R64UInt:
	case ImageFormat::R64SInt:
	case ImageFormat::R64SFloat:
		return ImageOrder::ImageOrderR;
	case ImageFormat::R4G4UNormPack8:
	case ImageFormat::R8G8UNorm:
	case ImageFormat::R8G8SNorm:
	case ImageFormat::R8G8UScaled:
	case ImageFormat::R8G8SScaled:
	case ImageFormat::R8G8UInt:
	case ImageFormat::R8G8SInt:
	case ImageFormat::R8G8SRGB:
	case ImageFormat::R16G16UNorm:
	case ImageFormat::R16G16SNorm:
	case ImageFormat::R16G16UScaled:
	case ImageFormat::R16G16SScaled:
	case ImageFormat::R16G16UInt:
	case ImageFormat::R16G16SInt:
	case ImageFormat::R16G16SFloat:
	case ImageFormat::R32G32UInt:
	case ImageFormat::R32G32SInt:
	case ImageFormat::R32G32SFloat:
	case ImageFormat::R64G64UInt:
	case ImageFormat::R64G64SInt:
	case ImageFormat::R64G64SFloat:
		return ImageOrder::ImageOrderRG;
	case ImageFormat::R5G6B5UNormPack16:
	case ImageFormat::R8G8B8UNorm:
	case ImageFormat::R8G8B8SNorm:
	case ImageFormat::R8G8B8UScaled:
	case ImageFormat::R8G8B8SScaled:
	case ImageFormat::R8G8B8UInt:
	case ImageFormat::R8G8B8SInt:
	case ImageFormat::R8G8B8SRGB:
	case ImageFormat::R16G16B16UNorm:
	case ImageFormat::R16G16B16SNorm:
	case ImageFormat::R16G16B16UScaled:
	case ImageFormat::R16G16B16SScaled:
	case ImageFormat::R16G16B16UInt:
	case ImageFormat::R16G16B16SInt:
	case ImageFormat::R16G16B16SFloat:
	case ImageFormat::R32G32B32UInt:
	case ImageFormat::R32G32B32SInt:
	case ImageFormat::R32G32B32SFloat:
	case ImageFormat::R64G64B64UInt:
	case ImageFormat::R64G64B64SInt:
	case ImageFormat::R64G64B64SFloat:
		return ImageOrder::ImageOrderRGB;
	case ImageFormat::B5G6R5UNormPack16:
	case ImageFormat::B8G8R8UNorm:
	case ImageFormat::B8G8R8SNorm:
	case ImageFormat::B8G8R8UScaled:
	case ImageFormat::B8G8R8SScaled:
	case ImageFormat::B8G8R8UInt:
	case ImageFormat::B8G8R8SInt:
	case ImageFormat::B8G8R8SRGB:
	case ImageFormat::B10G11R11UFloatPack32:
		return ImageOrder::ImageOrderBGR;
	case ImageFormat::R4G4B4A4UNormPack16:
	case ImageFormat::R5G5B5A1UNormPack16:
	case ImageFormat::A1R5G5B5UNormPack16:
	case ImageFormat::R8G8B8A8UNorm:
	case ImageFormat::R8G8B8A8SNorm:
	case ImageFormat::R8G8B8A8UScaled:
	case ImageFormat::R8G8B8A8SScaled:
	case ImageFormat::R8G8B8A8UInt:
	case ImageFormat::R8G8B8A8SInt:
	case ImageFormat::R8G8B8A8SRGB:
	case ImageFormat::A2R10G10B10UNormPack32:
	case ImageFormat::A2R10G10B10SNormPack32:
	case ImageFormat::A2R10G10B10UScaledPack32:
	case ImageFormat::A2R10G10B10SScaledPack32:
	case ImageFormat::A2R10G10B10UIntPack32:
	case ImageFormat::A2R10G10B10SIntPack32:
	case ImageFormat::R16G16B16A16UNorm:
	case ImageFormat::R16G16B16A16SNorm:
	case ImageFormat::R16G16B16A16UScaled:
	case ImageFormat::R16G16B16A16SScaled:
	case ImageFormat::R16G16B16A16UInt:
	case ImageFormat::R16G16B16A16SInt:
	case ImageFormat::R16G16B16A16SFloat:
	case ImageFormat::R32G32B32A32UInt:
	case ImageFormat::R32G32B32A32SInt:
	case ImageFormat::R32G32B32A32SFloat:
	case ImageFormat::R64G64B64A64UInt:
	case ImageFormat::R64G64B64A64SInt:
	case ImageFormat::R64G64B64A64SFloat:
		return ImageOrder::ImageOrderRGBA;
	case ImageFormat::B4G4R4A4UNormPack16:
	case ImageFormat::B5G5R5A1UNormPack16:
	case ImageFormat::B8G8R8A8UNorm:
	case ImageFormat::B8G8R8A8SNorm:
	case ImageFormat::B8G8R8A8UScaled:
	case ImageFormat::B8G8R8A8SScaled:
	case ImageFormat::B8G8R8A8UInt:
	case ImageFormat::B8G8R8A8SInt:
	case ImageFormat::B8G8R8A8SRGB:
	case ImageFormat::A2B10G10R10UNormPack32:
	case ImageFormat::A2B10G10R10SNormPack32:
	case ImageFormat::A2B10G10R10UScaledPack32:
	case ImageFormat::A2B10G10R10SScaledPack32:
	case ImageFormat::A2B10G10R10UIntPack32:
	case ImageFormat::A2B10G10R10SIntPack32:
		return ImageOrder::ImageOrderBGRA;
	case ImageFormat::A8B8G8R8UNormPack32:
	case ImageFormat::A8B8G8R8SNormPack32:
	case ImageFormat::A8B8G8R8UScaledPack32:
	case ImageFormat::A8B8G8R8SScaledPack32:
	case ImageFormat::A8B8G8R8UIntPack32:
	case ImageFormat::A8B8G8R8SIntPack32:
	case ImageFormat::A8B8G8R8SRGBPack32:
		return ImageOrder::ImageOrderABGR;
	case ImageFormat::D16UNorm:
	case ImageFormat::X8_D24UNormPack32:
	case ImageFormat::D32_SFLOAT:
		return ImageOrder::ImageOrderDepth;
	case ImageFormat::S8UInt:
		return ImageOrder::ImageOrderStencil;
	case ImageFormat::D16UNorm_S8UInt:
	case ImageFormat::D24UNorm_S8UInt:
	case ImageFormat::D32_SFLOAT_S8UInt:
		return ImageOrder::ImageOrderDepthStencil;
	default:
		assert(false);
		return ImageOrder::ImageOrderDepthStencil;
	}
}

ImageOrder order(const Image& image) noexcept
{
	return order(image.format());
}

}

_NAME_END