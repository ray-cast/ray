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
#ifndef _H_IMAGE_BASE_H_
#define _H_IMAGE_BASE_H_

#include <ray/imagdef.h>

_NAME_BEGIN

class EXPORT ImageBase
{
public:
    enum _ImageType {};
    static const _ImageType unknown = (_ImageType)UNKNOWN_HANDLER;
#if _BUILD_BMP_HANDLER
    static const _ImageType bmp = (_ImageType)BMP_HANDLER;
#endif
#if _BUILD_JPEG_HANDLER
    static const _ImageType jpeg = (_ImageType)JPEG_HANDLER;
#endif
#if _BUILD_PNG_HANDLER
    static const _ImageType png = (_ImageType)PNG_HANDLER;
#endif
#if _BUILD_TGA_HANDLER
    static const _ImageType tga = (_ImageType)TGA_HANDLER;
#endif
#if _BUILD_DDS_HANDLER
	static const _ImageType dds1 = (_ImageType)DDS1_HANDLER;
	static const _ImageType dds3 = (_ImageType)DDS3_HANDLER;
	static const _ImageType dds5 = (_ImageType)DDS5_HANDLER;
	static const _ImageType bc4u = (_ImageType)BC4U_HANDLER;
	static const _ImageType bc4s = (_ImageType)BC4S_HANDLER;
	static const _ImageType bc5s = (_ImageType)BC5S_HANDLER;
	static const _ImageType ati2 = (_ImageType)ATI2_HANDLER;
#endif
    typedef unsigned int color_type;
    typedef unsigned int image_type;

    typedef std::uint8_t pass_val;
    typedef std::uint8_t* image_buf;
    typedef std::uint16_t bpp_type;

    typedef int size_type;
    typedef std::uint32_t flag_type;
    typedef std::uint32_t delay_type;

protected:
    ImageBase() noexcept {}

private:
    ImageBase(const ImageBase&);
    const ImageBase& operator=(const ImageBase&);
};

_NAME_END

#endif