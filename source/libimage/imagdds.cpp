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

enum DXGI_FORMAT 
{
	DXGI_FORMAT_UNKNOWN = 0,
	DXGI_FORMAT_R32G32B32A32_TYPELESS = 1,
	DXGI_FORMAT_R32G32B32A32_FLOAT = 2,
	DXGI_FORMAT_R32G32B32A32_UINT = 3,
	DXGI_FORMAT_R32G32B32A32_SINT = 4,
	DXGI_FORMAT_R32G32B32_TYPELESS = 5,
	DXGI_FORMAT_R32G32B32_FLOAT = 6,
	DXGI_FORMAT_R32G32B32_UINT = 7,
	DXGI_FORMAT_R32G32B32_SINT = 8,
	DXGI_FORMAT_R16G16B16A16_TYPELESS = 9,
	DXGI_FORMAT_R16G16B16A16_FLOAT = 10,
	DXGI_FORMAT_R16G16B16A16_UNORM = 11,
	DXGI_FORMAT_R16G16B16A16_UINT = 12,
	DXGI_FORMAT_R16G16B16A16_SNORM = 13,
	DXGI_FORMAT_R16G16B16A16_SINT = 14,
	DXGI_FORMAT_R32G32_TYPELESS = 15,
	DXGI_FORMAT_R32G32_FLOAT = 16,
	DXGI_FORMAT_R32G32_UINT = 17,
	DXGI_FORMAT_R32G32_SINT = 18,
	DXGI_FORMAT_R32G8X24_TYPELESS = 19,
	DXGI_FORMAT_D32_FLOAT_S8X24_UINT = 20,
	DXGI_FORMAT_R32_FLOAT_X8X24_TYPELESS = 21,
	DXGI_FORMAT_X32_TYPELESS_G8X24_UINT = 22,
	DXGI_FORMAT_R10G10B10A2_TYPELESS = 23,
	DXGI_FORMAT_R10G10B10A2_UNORM = 24,
	DXGI_FORMAT_R10G10B10A2_UINT = 25,
	DXGI_FORMAT_R11G11B10_FLOAT = 26,
	DXGI_FORMAT_R8G8B8A8_TYPELESS = 27,
	DXGI_FORMAT_R8G8B8A8_UNORM = 28,
	DXGI_FORMAT_R8G8B8A8_UNORM_SRGB = 29,
	DXGI_FORMAT_R8G8B8A8_UINT = 30,
	DXGI_FORMAT_R8G8B8A8_SNORM = 31,
	DXGI_FORMAT_R8G8B8A8_SINT = 32,
	DXGI_FORMAT_R16G16_TYPELESS = 33,
	DXGI_FORMAT_R16G16_FLOAT = 34,
	DXGI_FORMAT_R16G16_UNORM = 35,
	DXGI_FORMAT_R16G16_UINT = 36,
	DXGI_FORMAT_R16G16_SNORM = 37,
	DXGI_FORMAT_R16G16_SINT = 38,
	DXGI_FORMAT_R32_TYPELESS = 39,
	DXGI_FORMAT_D32_FLOAT = 40,
	DXGI_FORMAT_R32_FLOAT = 41,
	DXGI_FORMAT_R32_UINT = 42,
	DXGI_FORMAT_R32_SINT = 43,
	DXGI_FORMAT_R24G8_TYPELESS = 44,
	DXGI_FORMAT_D24_UNORM_S8_UINT = 45,
	DXGI_FORMAT_R24_UNORM_X8_TYPELESS = 46,
	DXGI_FORMAT_X24_TYPELESS_G8_UINT = 47,
	DXGI_FORMAT_R8G8_TYPELESS = 48,
	DXGI_FORMAT_R8G8_UNORM = 49,
	DXGI_FORMAT_R8G8_UINT = 50,
	DXGI_FORMAT_R8G8_SNORM = 51,
	DXGI_FORMAT_R8G8_SINT = 52,
	DXGI_FORMAT_R16_TYPELESS = 53,
	DXGI_FORMAT_R16_FLOAT = 54,
	DXGI_FORMAT_D16_UNORM = 55,
	DXGI_FORMAT_R16_UNORM = 56,
	DXGI_FORMAT_R16_UINT = 57,
	DXGI_FORMAT_R16_SNORM = 58,
	DXGI_FORMAT_R16_SINT = 59,
	DXGI_FORMAT_R8_TYPELESS = 60,
	DXGI_FORMAT_R8_UNORM = 61,
	DXGI_FORMAT_R8_UINT = 62,
	DXGI_FORMAT_R8_SNORM = 63,
	DXGI_FORMAT_R8_SINT = 64,
	DXGI_FORMAT_A8_UNORM = 65,
	DXGI_FORMAT_R1_UNORM = 66,
	DXGI_FORMAT_R9G9B9E5_SHAREDEXP = 67,
	DXGI_FORMAT_R8G8_B8G8_UNORM = 68,
	DXGI_FORMAT_G8R8_G8B8_UNORM = 69,
	DXGI_FORMAT_BC1_TYPELESS = 70,
	DXGI_FORMAT_BC1_UNORM = 71,
	DXGI_FORMAT_BC1_UNORM_SRGB = 72,
	DXGI_FORMAT_BC2_TYPELESS = 73,
	DXGI_FORMAT_BC2_UNORM = 74,
	DXGI_FORMAT_BC2_UNORM_SRGB = 75,
	DXGI_FORMAT_BC3_TYPELESS = 76,
	DXGI_FORMAT_BC3_UNORM = 77,
	DXGI_FORMAT_BC3_UNORM_SRGB = 78,
	DXGI_FORMAT_BC4_TYPELESS = 79,
	DXGI_FORMAT_BC4_UNORM = 80,
	DXGI_FORMAT_BC4_SNORM = 81,
	DXGI_FORMAT_BC5_TYPELESS = 82,
	DXGI_FORMAT_BC5_UNORM = 83,
	DXGI_FORMAT_BC5_SNORM = 84,
	DXGI_FORMAT_B5G6R5_UNORM = 85,
	DXGI_FORMAT_B5G5R5A1_UNORM = 86,
	DXGI_FORMAT_B8G8R8A8_UNORM = 87,
	DXGI_FORMAT_B8G8R8X8_UNORM = 88,
	DXGI_FORMAT_R10G10B10_XR_BIAS_A2_UNORM = 89,
	DXGI_FORMAT_B8G8R8A8_TYPELESS = 90,
	DXGI_FORMAT_B8G8R8A8_UNORM_SRGB = 91,
	DXGI_FORMAT_B8G8R8X8_TYPELESS = 92,
	DXGI_FORMAT_B8G8R8X8_UNORM_SRGB = 93,
	DXGI_FORMAT_BC6H_TYPELESS = 94,
	DXGI_FORMAT_BC6H_UF16 = 95,
	DXGI_FORMAT_BC6H_SF16 = 96,
	DXGI_FORMAT_BC7_TYPELESS = 97,
	DXGI_FORMAT_BC7_UNORM = 98,
	DXGI_FORMAT_BC7_UNORM_SRGB = 99,
	DXGI_FORMAT_AYUV = 100,
	DXGI_FORMAT_Y410 = 101,
	DXGI_FORMAT_Y416 = 102,
	DXGI_FORMAT_NV12 = 103,
	DXGI_FORMAT_P010 = 104,
	DXGI_FORMAT_P016 = 105,
	DXGI_FORMAT_420_OPAQUE = 106,
	DXGI_FORMAT_YUY2 = 107,
	DXGI_FORMAT_Y210 = 108,
	DXGI_FORMAT_Y216 = 109,
	DXGI_FORMAT_NV11 = 110,
	DXGI_FORMAT_AI44 = 111,
	DXGI_FORMAT_IA44 = 112,
	DXGI_FORMAT_P8 = 113,
	DXGI_FORMAT_A8P8 = 114,
	DXGI_FORMAT_B4G4R4A4_UNORM = 115,
	DXGI_FORMAT_P208 = 130,
	DXGI_FORMAT_V208 = 131,
	DXGI_FORMAT_V408 = 132,
	DXGI_FORMAT_FORCE_UINT = 0xffffffff
};

enum D3D10_RESOURCE_DIMENSION
{
	D3D10_RESOURCE_DIMENSION_UNKNOWN = 0,
	D3D10_RESOURCE_DIMENSION_BUFFER = 1,
	D3D10_RESOURCE_DIMENSION_TEXTURE1D = 2,
	D3D10_RESOURCE_DIMENSION_TEXTURE2D = 3,
	D3D10_RESOURCE_DIMENSION_TEXTURE3D = 4
};

enum
{
	DDSD_CAPS = 0x1,
	DDSD_HEIGHT = 0x2,
	DDSD_WIDTH = 0x4,
	DDSD_PITCH = 0x8,
	DDSD_PIXELFORMAT = 0x1000,
	DDSD_MIPMAPCOUNT = 0x20000,
	DDSD_LINEARSIZE = 0x80000,
	DDSD_DEPTH = 0x800000
};

enum
{
	DDPF_ALPHAPIXELS = 0x1,
	DDPF_ALPHA = 0x2,
	DDPF_FOURCC = 0x4,
	DDPF_RGB = 0x40,
	DDPF_YUV = 0x200,
	DDPF_LUMINANCE = 0x20000,
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

struct DDS_MAGIC
{
	dds_ubyte   header[4];
};

struct DDS_HEADER
{
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

struct DDS_HEADER_DXT10 
{
	DXGI_FORMAT              dxgiFormat;
	D3D10_RESOURCE_DIMENSION resourceDimension;
	UINT                     miscFlag;
	UINT                     arraySize;
	UINT                     miscFlags2;
};

#define DDS_MAKE_FOURCC(ch0, ch1, ch2, ch3) \
        ((dds_uint)( \
        (((dds_uint)(dds_ubyte)(ch3) << 24) & 0xFF000000) | \
        (((dds_uint)(dds_ubyte)(ch2) << 16) & 0x00FF0000) | \
        (((dds_uint)(dds_ubyte)(ch1) <<  8) & 0x0000FF00) | \
         ((dds_uint)(dds_ubyte)(ch0)        & 0x000000FF) ))

#define DDS_FOURCC_DXT1 DDS_MAKE_FOURCC('D', 'X', 'T', '1')
#define DDS_FOURCC_DXT2 DDS_MAKE_FOURCC('D', 'X', 'T', '2')
#define DDS_FOURCC_DXT3 DDS_MAKE_FOURCC('D', 'X', 'T', '3')
#define DDS_FOURCC_DXT4 DDS_MAKE_FOURCC('D', 'X', 'T', '4')
#define DDS_FOURCC_DXT5 DDS_MAKE_FOURCC('D', 'X', 'T', '5')
#define DDS_FOURCC_BC4U DDS_MAKE_FOURCC('B', 'C', '4', 'U')
#define DDS_FOURCC_BC4S DDS_MAKE_FOURCC('B', 'C', '4', 'S')
#define DDS_FOURCC_BC5U DDS_MAKE_FOURCC('B', 'C', '5', 'U')
#define DDS_FOURCC_BC5S DDS_MAKE_FOURCC('B', 'C', '5', 'S')
#define DDS_FOURCC_RGBG DDS_MAKE_FOURCC('R', 'G', 'B', 'G')
#define DDS_FOURCC_GRGB DDS_MAKE_FOURCC('G', 'R', 'G', 'B')
#define DDS_FOURCC_ATI2 DDS_MAKE_FOURCC('A', 'T', 'I', '2')
#define DDS_FOURCC_DX10 DDS_MAKE_FOURCC('D', 'X', '1', '0')
#define DDS_FOURCC_UYVY DDS_MAKE_FOURCC('U', 'Y', 'V', 'Y')
#define DDS_FOURCC_YUY2 DDS_MAKE_FOURCC('Y', 'U', 'Y', '2')
#define DDS_FOURCC_YUY2 DDS_MAKE_FOURCC(117, 0, 0,  0)

#define DXGI_FORMAT_R16G16B16A16_UNORM 36
#define DXGI_FORMAT_R16G16B16A16_SNORM 110
#define DXGI_FORMAT_R16_FLOAT 111
#define DXGI_FORMAT_R16G16_FLOAT 112
#define DXGI_FORMAT_R16G16B16A16_FLOAT 113
#define DXGI_FORMAT_R32_FLOAT 114
#define DXGI_FORMAT_R32G32_FLOAT 115
#define DXGI_FORMAT_R32G32B32A32_FLOAT 116

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
	DDS_MAGIC magic;
	if (!stream.read((char*)&magic, sizeof(magic)))
		return false;

	DDS_HEADER info;
	if (!stream.read((char*)&info, sizeof(info)))
		return false;

	std::size_t length = stream.size() - sizeof(info);

	ImageFormat format = ImageFormat::ImageFormatUnknow;
	if (info.format.flags & DDPF_FOURCC)
	{
		switch (info.format.fourcc)
		{
		case DDS_FOURCC_DXT1:
			format = ImageFormat::ImageFormatBC1RGBU;
			break;
		case DDS_FOURCC_DXT2:
			format = ImageFormat::ImageFormatBC2U;
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
		case DXGI_FORMAT_R16G16B16A16_FLOAT:
		{
			info.format.bpp = 64;
			format = ImageFormat::ImageFormatR16G16B16A16F;
		}
		break;
		case DXGI_FORMAT_R32G32B32A32_FLOAT:
		{
			info.format.bpp = 128;
			format = ImageFormat::ImageFormatR32G32B32A32F;
		}
		break;
		case DDS_FOURCC_DX10:
		{
			DDS_HEADER_DXT10 dx10;
			if (!stream.read((char*)&dx10, sizeof(dx10)))
				return false;

			length -= sizeof(dx10);

			if (info.format.flags == 65)
			{
				if (info.format.blue_mask > info.format.red_mask)
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
					else if (info.format.bpp == 32)
						format = ImageFormat::ImageFormatB8G8R8A8;
					else
						return false;
				}
			}
			else
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
		}
		break;
		}
	}
	else
	{
		if (info.format.flags & DDPF_LUMINANCE)
		{
			format = ImageFormat::ImageFormatR8;
		}
		else if (info.format.flags & DDPF_RGB)
		{
			if (info.format.flags & DDPF_ALPHAPIXELS)
			{
				if (info.format.blue_mask > info.format.red_mask)
					format = ImageFormat::ImageFormatR8G8B8A8;
				else
					format = ImageFormat::ImageFormatB8G8R8A8;
			}
			else
			{
				if (info.format.blue_mask > info.format.red_mask)
					format = ImageFormat::ImageFormatR8G8B8;
				else
					format = ImageFormat::ImageFormatB8G8R8;
			}
		}
	}

	if (format == ImageFormat::ImageFormatUnknow)
		return false;

	auto data = make_scope<char[]>(length);
	if (!stream.read((char*)data.get(), length))
		return false;

	image.setMipLevel(info.mip_level);
	image.setImageType(ImageType::ImageTypeDDS);

	if (info.mip_level > 1 && info.depth == 0 && info.flags & DDSD_PITCH)
	{
		auto swap = make_scope<char[]>(length);
		if (!DDStoCubeMap(swap.get(), 0, info.mip_level, info.width, info.height, 6, info.format.bpp, data.get()))
			return false;

		if (!image.create(info.width, info.height, info.depth, format, length, (std::uint8_t*)swap.get(), false))
			return false;

		swap.dismiss();
	}
	else
	{
		if (!image.create(info.width, info.height, info.depth, format, length, (std::uint8_t*)data.get(), false))
			return false;

		data.dismiss();
	}

	return true;
}

bool
DDSHandler::doSave(Image& image, StreamWrite& stream) noexcept
{
	return false;
}

_NAME_END