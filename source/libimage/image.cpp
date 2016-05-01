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

Image::Image(StreamReader& stream, ImageType type) noexcept
{
	this->_init();
	this->load(stream, type);
}

Image::Image(std::uint32_t width, std::uint32_t height, std::uint16_t bpp, bool clear) noexcept
{
	this->_init();
	this->create(width, height, bpp, clear);
}

Image::Image(std::uint32_t width, std::uint32_t height, std::uint16_t bpp, std::size_t dataSize, std::uint8_t* data, bool staticData, bool clear) noexcept
{
	this->_init();
	this->create(width, height, bpp, dataSize, data, staticData, clear);
}

Image::~Image() noexcept
{
	this->destroy();
}

bool
Image::create(std::uint32_t width, std::uint32_t height, std::uint16_t bpp, bool clear) noexcept
{
	std::size_t destLength = (width * height * bpp) >> 3;

	this->destroy();

	_width = width;
	_height = height;
	_depth = 1;
	_bpp = bpp;
	_size = destLength;
	_isStatic = false;
	_data = new std::uint8_t[destLength];

	this->setImageType(ImageType::ImageTypeUnknown);

	if (clear) this->clear();

	return true;
}

bool
Image::create(std::uint32_t width, std::uint32_t height, std::uint16_t bpp, std::size_t dataSize, std::uint8_t* data, bool staticData, bool clear) noexcept
{
	return this->create(width, height, 0, bpp, dataSize, data, staticData, clear);
}

bool
Image::create(std::uint32_t width, std::uint32_t height, std::uint32_t depth, std::uint16_t bpp, std::size_t dataSize, std::uint8_t* data, bool staticData, bool clear) noexcept
{
	assert(data);

	this->destroy();

	_width = width;
	_height = height;
	_depth = depth;
	_bpp = bpp;
	_data = data;
	_size = dataSize;
	_isStatic = staticData;

	if (clear) this->clear();

	return true;
}

bool 
Image::create(std::uint32_t width, std::uint32_t height, ImageType type, ImageFormat format, bool clear) noexcept
{
	std::uint16_t bpp = 0;
	if (format == ImageFormat::ImageFormatR8)
		bpp = 8;
	else if (format == ImageFormat::ImageFormatR8G8)
		bpp = 16;
	else if (format == ImageFormat::ImageFormatR8G8B8 || format == ImageFormat::ImageFormatB8G8R8)
		bpp = 24;
	else if (format == ImageFormat::ImageFormatR8G8B8A8 || format == ImageFormat::ImageFormatB8G8R8A8)
		bpp = 32;
	else
		return false;

	this->setImageType(type);

	return this->create(width, height, bpp, clear);
}

bool
Image::create(const Image& copy) noexcept
{
	return this->create(copy.width(), copy.height(), copy.bpp(), false);
}

void
Image::_init() noexcept
{
	_isStatic = false;

	_width = 0;
	_height = 0;
	_depth = 0;

	_bpp = 0;

	_size = 0;

	_imageFormat = ImageFormat::ImageFormatUnknow;
	_imageType = ImageType::ImageTypeUnknown;
	_data = nullptr;
	_mipLevel = 1;
}

void
Image::destroy() noexcept
{
	if (_data && !_isStatic)
	{
		delete[] _data;
		_data = nullptr;
	}
}

void
Image::setImageType(ImageType type) noexcept
{
	_imageType = type;
}

ImageType
Image::getImageType() const noexcept
{
	return _imageType;
}

void
Image::setImageFormat(ImageFormat format) noexcept
{
	_imageFormat = format;
}

ImageFormat
Image::getImageFormat() const noexcept
{
	return _imageFormat;
}

void
Image::setMipLevel(std::uint8_t level) noexcept
{
	_mipLevel = level;
}

std::uint8_t
Image::getMipLevel() const noexcept
{
	return _mipLevel;
}

std::uint32_t
Image::getFrameDelay() const noexcept
{
	return 0;
}

void
Image::setFrameDelay(std::uint32_t /*delay*/) const noexcept
{
}

void
Image::setPalette(PaletteData* pal) noexcept
{
	assert(pal);
}

bool
Image::load(StreamReader& stream, ImageType type) noexcept
{
	if (emptyHandler())
		GetImageInstanceList(*this);

	ImageHandlerPtr impl;

	this->setImageType(type);

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

void
Image::cmyk_to_rgb(std::uint8_t* rgb, const std::uint8_t* cmyk) noexcept
{
	int k = 255 - cmyk[3];
	int k2 = cmyk[3];
	int c;

	c = k + k2 * (255 - cmyk[0]) / 255;
	rgb[0] = (std::uint8_t)((c > 255) ? 0 : (255 - c));

	c = k + k2 * (255 - cmyk[1]) / 255;
	rgb[1] = (std::uint8_t)((c > 255) ? 0 : (255 - c));

	c = k + k2 * (255 - cmyk[2]) / 255;
	rgb[2] = (std::uint8_t)((c > 255) ? 0 : (255 - c));
}

void
Image::cmyk_to_rgba(std::uint8_t* rgba, const std::uint8_t* cmyk) noexcept
{
	int k = 255 - cmyk[3];
	int k2 = cmyk[3];
	int c;

	c = k + k2 * (255 - cmyk[0]) / 255;
	rgba[0] = (std::uint8_t)((c > 255) ? 0 : (255 - c));

	c = k + k2 * (255 - cmyk[1]) / 255;
	rgba[1] = (std::uint8_t)((c > 255) ? 0 : (255 - c));

	c = k + k2 * (255 - cmyk[2]) / 255;
	rgba[2] = (std::uint8_t)((c > 255) ? 0 : (255 - c));

	rgba[3] = 255;
}

void
Image::cmyk_to_rgb(RGB* texel, const std::uint8_t* cmyk) noexcept
{
	int k = 255 - cmyk[3];
	int k2 = cmyk[3];
	int c;

	c = k + k2 * (255 - cmyk[0]) / 255;
	texel->r = (std::uint8_t)((c > 255) ? 0 : (255 - c));

	c = k + k2 * (255 - cmyk[1]) / 255;
	texel->g = (std::uint8_t)((c > 255) ? 0 : (255 - c));

	c = k + k2 * (255 - cmyk[2]) / 255;
	texel->b = (std::uint8_t)((c > 255) ? 0 : (255 - c));
}

void
Image::cmyk_to_rgba(RGBA* texel, const std::uint8_t* cmyk) noexcept
{
	int k = 255 - cmyk[3];
	int k2 = cmyk[3];
	int c;

	c = k + k2 * (255 - cmyk[0]) / 255;
	texel->r = (std::uint8_t)((c > 255) ? 0 : (255 - c));

	c = k + k2 * (255 - cmyk[1]) / 255;
	texel->g = (std::uint8_t)((c > 255) ? 0 : (255 - c));

	c = k + k2 * (255 - cmyk[2]) / 255;
	texel->b = (std::uint8_t)((c > 255) ? 0 : (255 - c));

	texel->a = 255;
}

void
Image::hsv_to_rgb(RGB& rgb, float h, float s, float v) noexcept
{
	int i;
	float f;
	float p, q, t;

	h *= 5;

	i = (int)floor(h);
	f = h - i;

	p = v * (1 - s);
	q = v * (1 - s * f);
	t = v * (1 - s * (1 - f));

	switch (i)
	{
	case 0:
		rgb.r = (std::uint8_t)(255 * v);
		rgb.g = (std::uint8_t)(255 * t);
		rgb.b = (std::uint8_t)(255 * p);
		break;
	case 1:
		rgb.r = (std::uint8_t)(255 * q);
		rgb.g = (std::uint8_t)(255 * v);
		rgb.b = (std::uint8_t)(255 * p);
		break;
	case 2:
		rgb.r = (std::uint8_t)(255 * p);
		rgb.g = (std::uint8_t)(255 * v);
		rgb.b = (std::uint8_t)(255 * t);
		break;
	case 3:
		rgb.r = (std::uint8_t)(255 * p);
		rgb.g = (std::uint8_t)(255 * q);
		rgb.b = (std::uint8_t)(255 * v);
		break;
	case 4:
		rgb.r = (std::uint8_t)(255 * t);
		rgb.g = (std::uint8_t)(255 * p);
		rgb.b = (std::uint8_t)(255 * v);
		break;
	case 5:
		rgb.r = (std::uint8_t)(255 * v);
		rgb.g = (std::uint8_t)(255 * p);
		rgb.b = (std::uint8_t)(255 * q);
		break;
	}
}

void
Image::hsv_to_rgba(RGBA& rgb, float h, float s, float v) noexcept
{
	float f;
	float p, q, t;

	h *= 5;

	int i = (int)floor(h);
	f = h - i;

	p = v * (1 - s);
	q = v * (1 - s * f);
	t = v * (1 - s * (1 - f));

	switch (i)
	{
	case 0:
		rgb.r = (std::uint8_t)(255 * v);
		rgb.g = (std::uint8_t)(255 * t);
		rgb.b = (std::uint8_t)(255 * p);
		break;
	case 1:
		rgb.r = (std::uint8_t)(255 * q);
		rgb.g = (std::uint8_t)(255 * v);
		rgb.b = (std::uint8_t)(255 * p);
		break;
	case 2:
		rgb.r = (std::uint8_t)(255 * p);
		rgb.g = (std::uint8_t)(255 * v);
		rgb.b = (std::uint8_t)(255 * t);
		break;
	case 3:
		rgb.r = (std::uint8_t)(255 * p);
		rgb.g = (std::uint8_t)(255 * q);
		rgb.b = (std::uint8_t)(255 * v);
		break;
	case 4:
		rgb.r = (std::uint8_t)(255 * t);
		rgb.g = (std::uint8_t)(255 * p);
		rgb.b = (std::uint8_t)(255 * v);
		break;
	case 5:
		rgb.r = (std::uint8_t)(255 * v);
		rgb.g = (std::uint8_t)(255 * p);
		rgb.b = (std::uint8_t)(255 * q);
		break;
	}

	rgb.a = 255;
}

void
Image::flipImageVertical(char* data, std::size_t width, std::size_t height, std::size_t component) noexcept
{
	std::size_t size = width * height * component;
	std::size_t stride = sizeof(char)* width * component;

	char* _data = new char[sizeof(char)* size];

	for (std::size_t i = 0; i < height; i++)
	{
		std::size_t j = height - i - 1;
		memcpy(_data + j * stride, data + i * stride, stride);
	}

	memcpy(data, _data, size);

	delete[] _data;
}

_NAME_END