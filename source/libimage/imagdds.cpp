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
#include "imagdds.h"

_NAME_BEGIN

typedef int             dds_int;
typedef unsigned int    dds_uint;
typedef char            dds_byte;
typedef unsigned char   dds_ubyte;
typedef long            dds_long;
typedef unsigned long   dds_ulong;
typedef void            dds_void;
typedef void*           dds_ptr;

enum dds_bool
{
	DDS_FALSE = 0,
	DDS_TRUE
};

typedef dds_uint DDSCaps[4];

struct DDPixelFormat
{
	dds_uint    size;
	dds_uint    flags;
	dds_uint    fourcc;
	dds_uint    bpp;
	dds_uint    red_mask;
	dds_uint    green_mask;
	dds_uint    blue_mask;
	dds_uint    alpha_mask;
};

struct DDColorKey
{
	dds_uint    low;
	dds_uint    high;
};

struct DDSTextureInfo
{
	dds_ubyte   header[4];
	dds_uint    size;
	dds_uint    flags;
	dds_uint    height;
	dds_uint    width;
	dds_uint    pitch;
	dds_uint    depth;
	dds_uint    mip_level;
	dds_uint    alpha_bit_depth;
	dds_uint    reserved;
	dds_uint    surface;
	DDColorKey  dest_overlay;
	DDColorKey  dest_blit;
	DDColorKey  src_overlay;
	DDColorKey  src_blit;
	DDPixelFormat format;
	DDSCaps     caps;
	dds_uint    texture_stage;
};

struct DDSRawPixelData
{
	dds_uint        height;
	dds_uint        width;
	dds_ubyte*      texels;
	dds_bool        ok;
};

#define DDS_MAKE_FOURCC(ch0, ch1, ch2, ch3) \
        ((dds_uint)( \
        (((dds_uint)(dds_ubyte)(ch3) << 24) & 0xFF000000) | \
        (((dds_uint)(dds_ubyte)(ch2) << 16) & 0x00FF0000) | \
        (((dds_uint)(dds_ubyte)(ch1) <<  8) & 0x0000FF00) | \
         ((dds_uint)(dds_ubyte)(ch0)        & 0x000000FF) ))

#define DDS_FOURCC_DXT1 DDS_MAKE_FOURCC('D', 'X', 'T', '1')
#define DDS_FOURCC_DXT3 DDS_MAKE_FOURCC('D', 'X', 'T', '3')
#define DDS_FOURCC_DXT5 DDS_MAKE_FOURCC('D', 'X', 'T', '5')
#define DDS_FOURCC_BC4U DDS_MAKE_FOURCC('B', 'C', '4', 'U')
#define DDS_FOURCC_BC4S DDS_MAKE_FOURCC('B', 'C', '4', 'S')
#define DDS_FOURCC_BC5U DDS_MAKE_FOURCC('B', 'C', '5', 'U')
#define DDS_FOURCC_BC5S DDS_MAKE_FOURCC('B', 'C', '5', 'S')
#define DDS_FOURCC_ATI2 DDS_MAKE_FOURCC('A', 'T', 'I', '2')
#define DDS_FOURCC_DX10 DDS_MAKE_FOURCC('D', 'X', '1', '0')

#define DDS_FOURCE_HALF_FLOAT 113
#define DDS_FOURCE_FLOAT 116

#define DDS_OK                  0
#define DDS_CANNOT_OPEN         1
#define DDS_INVALID_FILE        2
#define DDS_BAD_COMPRESSION     3
#define DDS_NO_GL_SUPPORT       4
#define DDS_CANNOT_WRITE_FILE   5
#define DDS_BAD_TEXINFO         6

#define DDS_MAX(a, b) (a > b ? a : b)
#define DDS_MIN(a, b) (a > b ? b : a)

bool DDStoCubeMap(char* buffer, std::size_t mipBase, std::size_t mipLevel, std::size_t width, std::size_t height, std::size_t depth, std::size_t bpp, char* stream)
{
	std::size_t offset1 = 0;
	std::size_t offset2 = 0;
	std::size_t allLayerSize = 0;

	std::size_t w = width;
	std::size_t h = height;

	std::size_t pixelSize = 0;
	if (bpp == 24)
		pixelSize = 3;
	else if (bpp == 32)
		pixelSize = 4;
	else if (bpp == 64)
		pixelSize = 8;
	else if (bpp == 128)
		pixelSize = 16;
	else
		return false;

	for (std::size_t mip = mipBase; mip < mipBase + mipLevel; mip++)
	{
		std::size_t mipSize = w * h * pixelSize;

		w = std::max(w >> 1, (std::size_t)1);
		h = std::max(h >> 1, (std::size_t)1);

		allLayerSize += mipSize;
	}

	w = width;
	h = height;

	for (std::size_t mip = mipBase; mip < mipBase + mipLevel; mip++)
	{
		std::size_t mipSize = w * h * pixelSize;

		for (std::size_t i = 0; i < depth; i++)
		{
			std::size_t offset = allLayerSize * i + offset1;
			std::memcpy(buffer + offset2, (char*)stream + offset, mipSize);
			offset2 += mipSize;
		}

		w = std::max(w >> 1, (std::size_t)1);
		h = std::max(h >> 1, (std::size_t)1);

		offset1 += mipSize;
	}

	return true;
}

DDSHandler::DDSHandler() noexcept
{
}

DDSHandler::~DDSHandler() noexcept
{
}

bool
DDSHandler::doCanRead(StreamReader& stream) const noexcept
{
	static const std::uint8_t magic[] = { 'D', 'D', 'S', ' ' };

	std::uint8_t hdr[sizeof(magic)];

	if (stream.read((char*)hdr, sizeof(hdr)))
		return std::memcmp(hdr, magic, sizeof(magic)) == 0;

	return false;
}

bool
DDSHandler::doLoad(Image& image, StreamReader& stream) noexcept
{
	DDSTextureInfo info;

	if (!stream.read((char*)&info, sizeof(info)))
		return false;

	auto size = stream.size() - sizeof(info);
	auto data = make_scope<char[]>(size);
	if (!stream.read((char*)data.get(), size))
		return false;

	image.setMipLevel(info.mip_level);
	image.setImageType(ImageType::ImageTypeDDS);

	ImageFormat format = ImageFormat::ImageFormatUnknow;
	switch (info.format.fourcc)
	{
	case DDS_FOURCC_DXT1:
	case DDS_FOURCC_DXT3:
	case DDS_FOURCC_DXT5:
	case DDS_FOURCC_BC4U:
	case DDS_FOURCC_BC4S:
	case DDS_FOURCC_BC5S:
	case DDS_FOURCC_ATI2:
	{
		switch (info.format.fourcc)
		{
		case DDS_FOURCC_DXT1:
			format = ImageFormat::ImageFormatBC1RGBU;
			break;
		case DDS_FOURCC_DXT3:
			format = ImageFormat::ImageFormatBC3U;
			break;
		case DDS_FOURCC_DXT5:
			format = ImageFormat::ImageFormatBC5U;
			break;
		case DDS_FOURCC_BC4U:
			format = ImageFormat::ImageFormatBC4U;
			break;
		case DDS_FOURCC_BC4S:
			format = ImageFormat::ImageFormatBC4S;
			break;
		case DDS_FOURCC_BC5S:
			format = ImageFormat::ImageFormatBC5S;
			break;
		case DDS_FOURCC_ATI2:
			format = ImageFormat::ImageFormatATI2;
			break;
		}

		if (!image.create(info.width, info.height, info.depth, format, size, (std::uint8_t*)data.get(), false))
			return false;

		data.dismiss();
	}
	break;
	case DDS_FOURCC_DX10:
	{
		info.format.bpp = info.format.size * 2;

		if (info.format.size == 8)
			format = ImageFormat::ImageFormatR16F;
		else if (info.format.size == 16)
			format = ImageFormat::ImageFormatR16G16F;
		else if (info.format.size == 24)
			format = ImageFormat::ImageFormatR16G16B16F;
		else if (info.format.size == 32)
			format = ImageFormat::ImageFormatR16G16B16A16F;
		else
			return false;
		assert(false);
	}
	break;
	default:
	{
		if (info.format.blue_mask == 0 &&
			info.format.red_mask == 0 &&
			info.format.blue_mask == 0)
		{
			if (info.format.fourcc == DDS_FOURCE_FLOAT)
			{
				info.format.bpp = info.format.size * 4;

				if (info.format.size == 8)
					format = ImageFormat::ImageFormatR32F;
				else if (info.format.size == 16)
					format = ImageFormat::ImageFormatR32G32F;
				else if (info.format.size == 24)
					format = ImageFormat::ImageFormatR32G32B32F;
				else if (info.format.size == 32)
					format = ImageFormat::ImageFormatR32G32B32A32F;
				else
					return false;
			}
			else if (info.format.fourcc == DDS_FOURCE_HALF_FLOAT)
			{
				info.format.bpp = info.format.size * 2;

				if (info.format.size == 8)
					format = ImageFormat::ImageFormatR16F;
				else if (info.format.size == 16)
					format = ImageFormat::ImageFormatR16G16F;
				else if (info.format.size == 24)
					format = ImageFormat::ImageFormatR16G16B16F;
				else if (info.format.size == 32)
					format = ImageFormat::ImageFormatR16G16B16A16F;
				else
					return false;
			}
			else
				return false;
		}
		else if (info.format.blue_mask > info.format.red_mask)
		{
			if (info.format.bpp == 24)
				format = ImageFormat::ImageFormatR8G8B8;
			else if (info.format.bpp == 32)
				format = ImageFormat::ImageFormatR8G8B8A8;
			else
				return false;
		}
		else
		{
			if (info.format.bpp == 24)
				format = ImageFormat::ImageFormatB8G8R8;
			if (info.format.bpp == 32)
				format = ImageFormat::ImageFormatB8G8R8A8;
			else
				return false;
		}

		if (info.mip_level > 1)
		{
			if (info.format.size == 0)
				return false;

			auto swap = make_scope<char[]>(size);
			if (!DDStoCubeMap(swap.get(), 0, info.mip_level, info.width, info.height, 6, info.format.size, data.get()))
				return false;

			if (!image.create(info.width, info.height, info.depth, format, size, (std::uint8_t*)swap.get(), false))
				return false;

			swap.dismiss();
		}
		else
		{
			if (!image.create(info.width, info.height, info.depth, format, size, (std::uint8_t*)data.get(), false))
				return false;

			data.dismiss();
		}
	}
	break;
	}

	return true;
}

bool
DDSHandler::doSave(Image& image, StreamWrite& stream) noexcept
{
	return false;
}

_NAME_END