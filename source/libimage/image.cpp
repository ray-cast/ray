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
#include "imagall.h"

_NAME_BEGIN

Image::Image() noexcept
{
	this->_init();
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

	if (format == ImageFormat::ImageFormatBC1RGBUNormBlock ||
		format == ImageFormat::ImageFormatBC1RGBSRGBBlock ||
		format == ImageFormat::ImageFormatBC1RGBAUNormBlock ||
		format == ImageFormat::ImageFormatBC1RGBASRGBBlock ||
		format == ImageFormat::ImageFormatBC2UNormBlock ||
		format == ImageFormat::ImageFormatBC2SRGBBlock ||
		format == ImageFormat::ImageFormatBC3UNormBlock ||
		format == ImageFormat::ImageFormatBC3SRGBBlock ||
		format == ImageFormat::ImageFormatBC4UNormBlock ||
		format == ImageFormat::ImageFormatBC4SNormBlock ||
		format == ImageFormat::ImageFormatBC5UNormBlock ||
		format == ImageFormat::ImageFormatBC5SNormBlock ||
		format == ImageFormat::ImageFormatBC6HUFloatBlock ||
		format == ImageFormat::ImageFormatBC6HSFloatBlock ||
		format == ImageFormat::ImageFormatBC7UNormBlock ||
		format == ImageFormat::ImageFormatBC7SRGBBlock)
	{
		std::uint32_t blockSize = 16;
		if (format == ImageFormat::ImageFormatBC1RGBUNormBlock ||
			format == ImageFormat::ImageFormatBC1RGBSRGBBlock ||
			format == ImageFormat::ImageFormatBC1RGBAUNormBlock ||
			format == ImageFormat::ImageFormatBC1RGBASRGBBlock)
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
		if (format == ImageFormat::ImageFormatR8UNorm)
			bpp = 8;
		else if (format == ImageFormat::ImageFormatR8G8UNorm)
			bpp = 16;
		else if (format == ImageFormat::ImageFormatR8G8B8UNorm || format == ImageFormat::ImageFormatB8G8R8UNorm)
			bpp = 24;
		else if (format == ImageFormat::ImageFormatR8G8B8A8UNorm || format == ImageFormat::ImageFormatB8G8R8A8UNorm)
			bpp = 32;
		else if (format == ImageFormat::ImageFormatR16SFloat)
			bpp = 16;
		else if (format == ImageFormat::ImageFormatR16G16SFloat)
			bpp = 32;
		else if (format == ImageFormat::ImageFormatR16G16B16SFloat)
			bpp = 48;
		else if (format == ImageFormat::ImageFormatR16G16B16A16SFloat)
			bpp = 64;
		else if (format == ImageFormat::ImageFormatR32SFloat)
			bpp = 32;
		else if (format == ImageFormat::ImageFormatR32G32SFloat)
			bpp = 64;
		else if (format == ImageFormat::ImageFormatR32G32B32SFloat)
			bpp = 96;
		else if (format == ImageFormat::ImageFormatR32G32B32A32SFloat)
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

	_imageFormat = format;

	_width = width;
	_height = height;
	_depth = depth;
	_size = destLength;
	_mipBase = mipBase;
	_mipLevel = mipLevel;
	_layerBase = layerBase;
	_layerLevel = layerLevel;
	_data = new std::uint8_t[destLength];

	if (clear)
	{
		std::memset(_data, 0, (std::size_t)this->size());
	}

	return true;
}

bool
Image::create(const Image& copy) noexcept
{
	return this->create(copy.width(), copy.height(), copy.getImageFormat(), false);
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

	_imageType = ImageType::ImageTypeUnknown;
	_imageFormat = ImageFormat::ImageFormatMaxEnum;

}

void
Image::destroy() noexcept
{
	if (_data)
	{
		delete[] _data;
		_data = nullptr;
	}

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
Image::getImageFormat() const noexcept
{
	return _imageFormat;
}

std::uint32_t
Image::getMipBase() const noexcept
{
	return _mipBase;
}

std::uint32_t
Image::getMipLevel() const noexcept
{
	return _mipLevel;
}

std::uint32_t 
Image::getLayerBase() const noexcept
{
	return _layerBase;
}

std::uint32_t 
Image::getLayerLevel() const noexcept
{
	return _layerLevel;
}

const char*
Image::data() const noexcept
{
	return (char*)_data;
}

bool
Image::load(StreamReader& stream, ImageType type) noexcept
{
	if (emptyHandler())
		GetImageInstanceList(*this);

	ImageHandlerPtr impl;

	if (this->find(stream, type, impl))
	{
		if (impl->doLoad(*this, stream))
		{
			return true;
		}
	}

	return false;
}

bool
Image::load(const std::string& filename, ImageType type) noexcept
{
	StreamReaderPtr stream;
	if (IoServer::instance()->openFile(stream, filename))
		return this->load(*stream, type);
	return false;
}

bool
Image::save(const std::string& filename, ImageType type) noexcept
{
	StreamWritePtr stream;
	if (IoServer::instance()->openFile(stream, filename))
		return this->save(*stream, type);
	return false;
}

bool
Image::save(StreamWrite& stream, ImageType type) noexcept
{
	if (stream.good())
	{
		ImageHandlerPtr impl;
		if (this->find(type, impl))
		{
			if (!impl->doSave(*this, stream))
				return false;

			return true;
		}
	}

	return false;
}

bool
Image::emptyHandler() const noexcept
{
	return this->_handlers.empty();
}

bool
Image::add(ImageHandlerPtr handler) noexcept
{
	assert(handler);
	auto it = std::find(_handlers.begin(), _handlers.end(), handler);
	if (it == _handlers.end())
	{
		_handlers.push_back(handler);
		return true;
	}

	return false;
}

bool
Image::remove(ImageHandlerPtr handler) noexcept
{
	assert(handler);
	auto it = std::find(_handlers.begin(), _handlers.end(), handler);
	if (it != _handlers.end())
	{
		_handlers.erase(it);
		return true;
	}

	return false;
}

bool
Image::find(StreamReader& stream, ImageHandlerPtr& out) const noexcept
{
	if (!stream.good())
		return false;

	for (auto it : _handlers)
	{
		stream.seekg(0, std::ios_base::beg);

		if (it->doCanRead(stream))
		{
			stream.seekg(0, std::ios_base::beg);

			out = it;
			return true;
		}
	}

	return false;
}

bool
Image::find(ImageType type, ImageHandlerPtr& out) const noexcept
{
	std::size_t index = type;
	if (_handlers.size() < index)
	{
		out = _handlers[index];
		return true;
	}

	return false;
}

bool
Image::find(StreamReader& stream, ImageType type, ImageHandlerPtr& out) const noexcept
{
	if (type != ImageType::ImageTypeUnknown)
	{
		if (this->find(type, out))
		{
			return true;
		}
	}

	return this->find(stream, out);
}

_NAME_END