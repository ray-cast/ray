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
#include "imagtga.h"

_NAME_BEGIN

#pragma pack(push)
#pragma pack(1)

struct TGAHeader
{
	std::uint8_t  id_length;
	std::uint8_t  colormap_type;
	std::uint8_t  image_type;
	std::uint16_t colormap_index;
	std::uint16_t colormap_length;
	std::uint8_t  colormap_size;
	std::uint16_t x_origin;
	std::uint16_t y_origin;
	std::uint16_t width;
	std::uint16_t height;
	std::uint8_t  pixel_size;
	std::uint8_t  attributes;
};

#pragma pack(pop)

TGAHandler::TGAHandler() noexcept
{
}

TGAHandler::~TGAHandler() noexcept
{
}

bool
TGAHandler::doCanRead(StreamReader& stream) const noexcept
{
	TGAHeader hdr;

	if (!stream.read((char*)&hdr, sizeof(hdr)))
		return false;

	if (hdr.image_type != 0 &&
		hdr.image_type != 2 &&
		hdr.image_type != 3 &&
		hdr.image_type != 10)
	{
		return false;
	}

	if (hdr.colormap_type != 0)
		return false;

	if (hdr.id_length != 0)
		return false;

	if (hdr.pixel_size != 32 &&
		hdr.pixel_size != 24 &&
		hdr.pixel_size != 16)
	{
		return false;
	}

	return true;
}

bool
TGAHandler::doLoad(Image& image, StreamReader& stream) noexcept
{
	TGAHeader hdr;
	if (!stream.read((char*)&hdr, sizeof(hdr))) return false;

	std::uint32_t columns = hdr.width;
	std::uint32_t rows = hdr.height;
	std::uint32_t nums = columns * rows;
	std::uint32_t length = nums * hdr.pixel_size / 8;

	if (hdr.id_length != 0)
		stream.seekg(hdr.id_length, ios_base::cur);

	switch (hdr.image_type)
	{
	case 2:
	{
		ImageFormat format = ImageFormat::ImageFormatUnknow;
		if (hdr.pixel_size == 8)
			format = ImageFormat::ImageFormatR8;
		else if (hdr.pixel_size == 24)
			format = ImageFormat::ImageFormatB8G8R8;
		else if (hdr.pixel_size == 32)
			format = ImageFormat::ImageFormatB8G8R8A8;
		else
			return false;

		if (!image.create(columns, rows, ImageType::ImageTypeTGA, format))
			return false;

		if (!stream.read((char*)image.data(), length))
			return false;
	}
	break;
	case 10:
	{
		if (!image.create(columns, rows, hdr.pixel_size))
			return false;

		image.setImageType(ImageType::ImageTypeTGA);

		std::vector<std::uint8_t> buffers(stream.size() - sizeof(TGAHeader));
		std::uint8_t* buf = (std::uint8_t*)buffers.data();

		if (!stream.read((char*)buf, buffers.size()))
			return false;

		switch (hdr.pixel_size)
		{
		case 16:
		{
		}
		break;

		case 24:
		{
			RGB* rgb = (RGB*)image.data();
			RGB* end = (RGB*)(image.data() + image.size());

			while (rgb < end)
			{
				std::uint8_t packe = *buf++;
				if (packe & 0x80)
				{
					std::uint8_t length = (std::uint8_t)(1 + (packe & 0x7f));

					BGR bgr;
					bgr.b = *buf++;
					bgr.g = *buf++;
					bgr.r = *buf++;

					for (std::uint8_t i = 0; i < length; i++)
					{
						rgb->r = bgr.r;
						rgb->g = bgr.g;
						rgb->b = bgr.b;
						rgb++;
					}
				}
				else
				{
					std::uint8_t length = ++packe;

					for (std::uint8_t i = 0; i < length; i++)
					{
						rgb->b = *buf++;
						rgb->g = *buf++;
						rgb->r = *buf++;
						rgb++;
					}
				}
			}
		}
		break;
		case 32:
		{
			image.setImageFormat(ImageFormat::ImageFormatB8G8R8A8);

			std::uint32_t* rgba = (std::uint32_t*)image.data();
			std::uint32_t* end = (std::uint32_t*)(image.data() + image.size());

			while (rgba < end)
			{
				std::uint8_t packe = *buf++;
				if (packe & 0x80)
				{
					std::uint8_t length = (std::uint8_t)(1 + (packe & 0x7f));

					for (std::uint8_t i = 0; i < length; i++)
						*rgba++ = *(std::uint32_t*)buf;

					buf += 4;						
				}
				else
				{
					std::uint8_t length = ++packe;
					for (std::uint8_t i = 0; i < length; i++)
						*rgba++ = *((std::uint32_t*&)buf)++;
				}
			}
		}
		break;
		}
	}
	break;
	default:
		assert(false);
	}

	if (hdr.y_origin == 0)
	{
		if (hdr.pixel_size == 24)
		{
			std::size_t half = image.height() >> 1;
			for (std::size_t i = 0; i < half; i++)
			{
				RGB* dest = (RGB*)image.data() + image.width() * (image.height() - i - 1);
				RGB* src = (RGB*)image.data() + image.width() * i;
				for (int j = 0; j < image.width(); j++)
				{
					std::swap(*dest, *src);
					dest++;
					src++;
				}
			}
		}
		else if (hdr.pixel_size == 32)
		{
			std::size_t half = image.height() >> 1;
			for (std::size_t i = 0; i < half; i++)
			{
				RGBA* dest = (RGBA*)image.data() + image.width() * (image.height() - i - 1);
				RGBA* src = (RGBA*)image.data() + image.width() * i;
				for (int j = 0; j < image.width(); j++)
				{
					std::swap(*dest, *src);
					dest++;
					src++;
				}
			}
		}
	}

	return true;
}

bool
TGAHandler::doSave(Image&, StreamWrite&) noexcept
{
	return false;
}

_NAME_END