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
#include <ray/imagcfg.h>

#if _BUILD_BMP_HANDLER
#include <ray/imagbmp.h>
#endif
#if _BUILD_PNG_HANDLER
#include <ray/imagpng.h>
#endif
#if _BUILD_GIF_HANDLER
#include <ray/imaggif.h>
#endif
#if _BUILD_DDS_HANDLER
#include <ray/imagdds.h>
#endif
#if _BUILD_IFF_HANDLER
#include <ray/imagiff.h>
#endif
#if _BUILD_JPEG_HANDLER
#include <ray/imagjpeg.h>
#endif
#if _BUILD_PCX_HANDLER
#include <ray/imagpcx.h>
#endif
#if _BUILD_TGA_HANDLER
#include <ray/imagtga.h>
#endif
#if _BUILD_TIFF_HANDLER
#include <ray/imagtiff.h>
#endif
#if _BUILD_XPM_HANDLER
#include <ray/imagxpm.h>
#endif

_NAME_BEGIN

#if _BUILD_BMP_HANDLER
std::shared_ptr<ImageHandler> bmp = std::make_shared<BMPHandler>();
#endif
#if _BUILD_DDS_HANDLER
std::shared_ptr<ImageHandler> dds = std::make_shared<DDSHandler>();
#endif
#if _BUILD_JPEG_HANDLER
std::shared_ptr<ImageHandler> jpeg = std::make_shared<JPEGHandler>();
#endif
#if _BUILD_PNG_HANDLER
std::shared_ptr<ImageHandler> png = std::make_shared<PNGHandler>();
#endif
#if _BUILD_PCX_HANDLER
std::shared_ptr<ImageHandler> pcx = std::make_shared<PCXHandler>();
#endif
#if _BUILD_TGA_HANDLER
std::shared_ptr<ImageHandler> tga = std::make_shared<TGAHandler>();
#endif

void GetImageInstanceList(Image& image)
{
#if _BUILD_PNG_HANDLER
    image.add(png);
#endif
#if _BUILD_TGA_HANDLER
    image.add(tga);
#endif
#if _BUILD_JPEG_HANDLER
    image.add(jpeg);
#endif
#if _BUILD_BMP_HANDLER
    image.add(bmp);
#endif
#if _BUILD_DDS_HANDLER
	image.add(dds);
#endif
}

_NAME_END