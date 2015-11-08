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
#ifndef _H_IMAGEBMP_H_
#define _H_IMAGEBMP_H_

#include <ray/image.h>

_NAME_BEGIN

#ifndef __WINDOWS__
/* constants for the biCompression field */
#   define BI_RGB        0L
#   define BI_RLE8       1L
#   define BI_RLE4       2L
#   define BI_BITFIELDS  3L
#   define BI_JPEG       4L
#   define BI_PNG        5L
#endif // __WINDOWS__

#ifndef __WINDOWS__
typedef std::uint16_t WORD;
typedef std::uint32_t DWORD;
typedef std::int32_t LONG;
#endif

enum
{
	BMP_32BPP = 32,

	// default, do not need to set
	BMP_24BPP = 24,

	BMP_16BPP = 16,

	// 8bpp, quantized colors
	BMP_8BPP = 8,

	// 8bpp, rgb averaged to greys
	BMP_8BPP_GREY = 9,

	// 8bpp, rgb averaged to grays
	BMP_8BPP_GRAY = 9,

	// 8bpp, red used as greyscale
	BMP_8BPP_RED = 10,

	// 8bpp, use the wxImage's palette
	BMP_8BPP_PALETTE = 11,

	// 4bpp, quantized colors
	BMP_4BPP = 4,

	// 1bpp, quantized "colors"
	BMP_1BPP = 1,

	// 1bpp, black & white from red
	BMP_1BPP_BW = 2
};

#define BI_RGB5_MASK_R 0x7C00
#define BI_RGB5_MASK_G 0x03E0
#define BI_RGB5_MASK_B 0x001F
#define BI_RGB5_SHIFT_R 0xA
#define BI_RGB5_SHIFT_G 0x5
#define BI_RGB5_SHIFT_B 0x0
#define BI_RGB5_BIT_R 0x3
#define BI_RGB5_BIT_G 0x3
#define BI_RGB5_BIT_B 0x3

#pragma pack(push)
#pragma pack(1)

struct BITMAPFILEHEADER
{
	WORD type;
	DWORD size;
	WORD reserved1;
	WORD reserved2;
	DWORD off_bits;
};

struct BITMAPINFOHEADER
{
	DWORD size;
	LONG width;
	LONG height;
	WORD planes;
	WORD bpp;
	DWORD comp;
	DWORD size_image;
	LONG pels_per_mater_x;
	LONG pels_per_mater_y;
	DWORD clr_used;
	DWORD clr_important;
};

struct BITMAPMASK
{
	DWORD r;
	DWORD g;
	DWORD b;
};

struct BITMAPINFO
{
	BITMAPFILEHEADER header;
	BITMAPINFOHEADER info;
};

#pragma pack(pop)

typedef RGB BMPPalette;

class BMPHandler : public ImageHandler
{
private:
	bool doCanRead(istream& stream) const noexcept;
	bool doLoad(Image& image, istream& stream) noexcept;
	bool doSave(Image& image, ostream& stream) noexcept;

	bool decode(Image& image, istream& stream, const struct BITMAPINFO& info);
	bool encode(Image& image, istream& stream, const struct BITMAPINFO& info);
	bool loadDIB(Image& image, istream& stream, const struct BITMAPINFO& info);
};

_NAME_END

#endif