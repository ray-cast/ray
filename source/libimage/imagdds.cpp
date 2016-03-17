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

#define DDS_OK                  0
#define DDS_CANNOT_OPEN         1
#define DDS_INVALID_FILE        2
#define DDS_BAD_COMPRESSION     3
#define DDS_NO_GL_SUPPORT       4
#define DDS_CANNOT_WRITE_FILE   5
#define DDS_BAD_TEXINFO         6

#define DDS_MAX(a, b) (a > b ? a : b)
#define DDS_MIN(a, b) (a > b ? b : a)

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
	auto data = make_scope<std::uint8_t[]>(size);

	if (image.create(info.width, info.height, info.format.size, size, data.get(), false))
	{
		if (stream.read((char*)image.data(), size))
		{
			data.dismiss();

			switch (info.format.fourcc)
			{
			case DDS_FOURCC_DXT1:
				image.setImageType(ImageType::ImageTypeBC1RGBU);
				break;
			case DDS_FOURCC_DXT3:
				image.setImageType(ImageType::ImageTypeBC3U);
				break;
			case DDS_FOURCC_BC4U:
				image.setImageType(ImageType::ImageTypeBC4U);
				break;
			case DDS_FOURCC_BC4S:
				image.setImageType(ImageType::ImageTypeBC4S);
				break;
			case DDS_FOURCC_DXT5:
				image.setImageType(ImageType::ImageTypeBC5U);
				break;
			case DDS_FOURCC_BC5S:
				image.setImageType(ImageType::ImageTypeBC5S);
				break;
			case DDS_FOURCC_ATI2:
				image.setImageType(ImageType::ImageTypeATI2);
				break;
			default:
				assert(false);
				break;
			}

			image.setMipLevel(info.mip_level);

			return true;
		}
	}
	
	return false;
}

bool 
DDSHandler::doSave(Image& image, StreamWrite& stream) noexcept
{
	return false;
}

_NAME_END