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

typedef std::shared_ptr<class ImageHandler> ImageHandlerPtr;

class EXPORT ImageBase
{
public:
    typedef unsigned int color_type;

    typedef std::uint8_t pass_val;
    typedef std::uint8_t* image_buf;
    typedef std::uint16_t bpp_type;

    typedef int size_type;
    typedef std::uint32_t flag_type;
    typedef std::uint32_t delay_type;

    ImageBase() noexcept {}

private:
    ImageBase(const ImageBase&) noexcept = delete;
    const ImageBase& operator=(const ImageBase&) noexcept = delete;
};

class ImageType
{
public:
	enum Type
	{
		unknown = UNKNOWN_HANDLER,
#if _BUILD_BMP_HANDLER
		bmp = BMP_HANDLER,
#endif
#if _BUILD_JPEG_HANDLER
		jpeg = JPEG_HANDLER,
#endif
#if _BUILD_PNG_HANDLER
		png = PNG_HANDLER,
#endif
#if _BUILD_TGA_HANDLER
		tga = TGA_HANDLER,
#endif
#if _BUILD_DDS_HANDLER
		dds1 = DDS1_HANDLER,
		dds3 = DDS3_HANDLER,
		dds5 = DDS5_HANDLER,
		bc4u = BC4U_HANDLER,
		bc4s = BC4S_HANDLER,
		bc5s = BC5S_HANDLER,
		ati2 = ATI2_HANDLER
#endif
	};

	ImageType()
		: _value(ImageType::unknown)
	{
	}

	ImageType(Type value)
		: _value(value)
	{
	}

	Type getValue() const noexcept
	{
		return _value;
	}

private:
	Type _value;
};

inline bool operator==(const ImageType& a, const ImageType& b) noexcept
{
	return a.getValue() == b.getValue();
}

inline bool operator!=(const ImageType& a, const ImageType& b) noexcept
{
	return a.getValue() != b.getValue();
}

inline bool operator==(const ImageType& a, ImageType::Type b) noexcept
{
	return a.getValue() == b;
}

inline bool operator!=(const ImageType& a, ImageType::Type b) noexcept
{
	return a.getValue() != b;
}

_NAME_END

#endif