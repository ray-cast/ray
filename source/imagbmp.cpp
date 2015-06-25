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
#include <ray/imagbmp.h>

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

bool
BMPHandler::doCanRead(istream& stream) const noexcept
{
    std::uint8_t hdr[2];

    if (stream.read((char*)hdr, sizeof(hdr)))
    {
        return hdr[0] == 'B' && hdr[1] == 'M';
    }

    return false;
}

bool
BMPHandler::doLoad(Image& image, istream& stream) noexcept
{
    auto size = (std::size_t)stream.size();

    BITMAPINFO info;
    if (!stream.read((char*)&info, sizeof(info)))
        return false;

    if (info.header.size != size ||
        info.info.size != sizeof(BITMAPINFOHEADER) ||
        info.header.reserved1 != 0 ||
        info.header.reserved2 != 0 ||
        info.info.width == 0 ||
        info.info.height == 0 ||
        info.info.planes != 1 ||
        info.info.bpp != 1 && info.info.bpp != 4 &&
        info.info.bpp != 8 && info.info.bpp != 16 &&
        info.info.bpp != 24 && info.info.bpp != 32)
    {
        return false;
    }

    if (info.info.comp == BI_RGB)
    {
        return this->loadDIB(image, stream, info);
    }
    else
    {
        if (info.info.comp == BI_RLE4 && info.info.bpp != 4 ||
            info.info.comp == BI_RLE8 && info.info.bpp != 8 ||
            info.info.comp == BI_BITFIELDS && info.info.bpp != 16 && info.info.bpp != 32)
        {
            this->error(image_error::ENCODING_DOESNT_MATCH_BITDEPTH);
            return false;
        }

        return this->decode(image, stream, info);
    }
}

bool
BMPHandler::doSave(Image&, ostream&) noexcept
{
    return false;
}

bool
BMPHandler::decode(Image&, istream&, const BITMAPINFO&)
{
    return false;
}

bool
BMPHandler::encode(Image&, istream&, const BITMAPINFO&)
{
    return false;
}

bool
BMPHandler::loadDIB(Image& image, istream& stream, const BITMAPINFO& info)
{
    size_type columns = (size_type)info.info.width;
    size_type rows = (size_type)(info.info.height < 0 ? -info.info.height : info.info.height);
    size_type nums = columns * rows;

    std::size_t length = (std::size_t)(nums * info.info.bpp / 8);

    std::vector<pass_val> buffers(length);
    image_buf buf = (image_buf)buffers.data();

    if (!stream.read((char*)buf, (std::streamsize)length))
        return false;

    if (!image.create(columns, rows, (bpp_type)info.info.bpp))
        return false;

    if (info.info.bpp == BMP_32BPP)
    {
        RGBA* rgba = (RGBA*)image.data();

        for (size_type i = 0; i < nums; i++)
        {
            rgba->b = *buf++;
            rgba->g = *buf++;
            rgba->r = *buf++;
            rgba->a = *buf++;
            rgba++;
        }
    }
    else if (info.info.bpp == BMP_24BPP)
    {
        RGB* rgb = (RGB*)image.data();

        for (size_type i = 0; i < nums; i++)
        {
            rgb->b = *buf++;
            rgb->g = *buf++;
            rgb->r = *buf++;
            rgb++;
        }
    }
    else if (info.info.bpp == BMP_16BPP)
    {
        image_buf data = image.data();
        switch (info.info.comp)
        {
        case BI_RGB:
        {
            std::size_t pos = 0;
            for (size_type i = 0; i < nums; i++)
            {
                WORD word = *buf;

                buf += sizeof(WORD);

                data[pos++] = (pass_val)(((word & BI_RGB5_MASK_R) >> BI_RGB5_SHIFT_R) << BI_RGB5_BIT_R);
                data[pos++] = (pass_val)(((word & BI_RGB5_MASK_G) >> BI_RGB5_SHIFT_G) << BI_RGB5_BIT_G);
                data[pos++] = (pass_val)(((word & BI_RGB5_MASK_B) >> BI_RGB5_SHIFT_B) << BI_RGB5_BIT_B);
            }
        }
        break;
        case BI_BITFIELDS:
        {
            BITMAPMASK mask = (BITMAPMASK&)*buf;

            buf += sizeof(BITMAPMASK);

            RGBt<int> shift;
            for (int bit = info.info.bpp - 1; bit >= 0; bit--)
            {
                if (mask.b & (1 << bit))
                    shift.b = bit;
                if (mask.g & (1 << bit))
                    shift.g = bit;
                if (mask.r & (1 << bit))
                    shift.r = bit;
            }

            RGBt<int> bits;
            for (int bit = 0; bit < info.info.bpp; bit++)
            {
                if (mask.b & (1 << bit))
                    bits.b = bit - shift.b + 1;
                if (mask.g & (1 << bit))
                    bits.g = bit - shift.g + 1;
                if (mask.r & (1 << bit))
                    bits.r = bit - shift.r + 1;
            }

            std::size_t pos = 0;
            for (size_type i = 0; i < nums; i++)
            {
                WORD word;
                stream.read((char*)&word, sizeof(word));

                data[pos++] = (pass_val)(((word & mask.r) >> shift.r) << (8 - bits.r));
                data[pos++] = (pass_val)(((word & mask.g) >> shift.g) << (8 - bits.g));
                data[pos++] = (pass_val)(((word & mask.b) >> shift.b) << (8 - bits.b));
            }
        }
        break;
        }
    }
    else
    {
        if (info.info.bpp == BMP_8BPP ||
            info.info.bpp == BMP_4BPP ||
            info.info.bpp == BMP_1BPP)
        {
            DWORD colors = info.info.clr_used;
            if (colors == 0)
                colors = (DWORD)1 << info.info.bpp;

            //std::shared_ptr<BMPPalette[]> cmap = std::make_shared<BMPPalette[]>(colors);

            for (std::size_t i = 0; i < colors; i++)
            {
            }
        }
    }

    return true;
}

_NAME_END