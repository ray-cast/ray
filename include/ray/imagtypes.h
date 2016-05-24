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

enum ImageType
{
	ImageTypeUnknown,
	ImageTypeBMP,
	ImageTypeJPEG,
	ImageTypePNG,
	ImageTypeTGA,
	ImageTypeDDS,
};

enum ImageFormat
{
	ImageFormatUnknow,
	ImageFormatR8,
	ImageFormatR8G8,
	ImageFormatR8G8B8,
	ImageFormatR8G8B8A8,
	ImageFormatB8G8R8,
	ImageFormatB8G8R8A8,
	ImageFormatR16F,
	ImageFormatR16G16F,
	ImageFormatR16G16B16F,
	ImageFormatR16G16B16A16F,
	ImageFormatR32F,
	ImageFormatR32G32F,
	ImageFormatR32G32B32F,
	ImageFormatR32G32B32A32F,
	ImageFormatBC1RGBU,
	ImageFormatBC1RGBSRGB,
	ImageFormatBC1RGBAU,
	ImageFormatBC1RGBASRGB,
	ImageFormatBC2U,
	ImageFormatBC2SRGB,
	ImageFormatBC3U,
	ImageFormatBC3SRGB,
	ImageFormatBC4U,
	ImageFormatBC4S,
	ImageFormatBC5U,
	ImageFormatBC5S,
	ImageFormatBC6HUFloat,
	ImageFormatBC6HSFloat,
	ImageFormatBC7U,
	ImageFormatBC7SRGB,
	ImageFormatATI2,
};

_NAME_END

#endif