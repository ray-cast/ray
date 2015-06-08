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
#include <ray/imagpcx.h>

_NAME_BEGIN

struct PCXTargaHeader
{
    char    manufacturer;
    char    version;
    char    encoding;
    char    bits_per_pixel;
    unsigned short    xmin, ymin, xmax, ymax;
    unsigned short    hres, vres;
    unsigned char    palette[48];
    char    reserved;
    char    color_planes;
    unsigned short    bytes_per_line;
    unsigned short    palette_type;
    char    filler[58];
    unsigned char    data;            // unbounded
};

bool
PCXHandler::doCanRead(istream& stream) const noexcept
{
    static std::uint8_t magic[] = { 0xFF, 0xD8 };

    std::uint8_t hdr[sizeof(magic)];

    if (stream.read((char*)hdr, sizeof(hdr)))
    {
        return std::memcmp(hdr, magic, sizeof(magic)) == 0;
    }

    return false;
}

bool
PCXHandler::doLoad(Image& /*image*/, istream& stream) noexcept
{
    PCXTargaHeader header;

    stream.read((char*)&header, sizeof(PCXTargaHeader));

    if (header.manufacturer != 0x0a ||
        header.version != 5 ||
        header.encoding != 1 ||
        header.bits_per_pixel != 8 ||
        header.xmax >= 65535 ||
        header.ymax >= 65535)
    {
    }

    return true;
}

bool
PCXHandler::doSave(Image& /*image*/, ostream& /*stream*/) noexcept
{
    return false;
}

_NAME_END