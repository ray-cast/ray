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
#include "imagall.h"

#if _BUILD_BMP_HANDLER
#include "imagbmp.h"
#endif
#if _BUILD_PNG_HANDLER
#include "imagpng.h"
#endif
#if _BUILD_GIF_HANDLER
#include "imaggif.h"
#endif
#if _BUILD_DDS_HANDLER
#include "imagdds.h"
#endif
#if _BUILD_JPEG_HANDLER
#include "imagjpeg.h"
#endif
#if _BUILD_TGA_HANDLER
#include "imagtga.h"
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

/*"Unable to remove custom importer: I can't find you ...",
"No suitable reader found for the file format of stream .",
"Unknown bitdepth in file.",
"Unknown encoding in file.",
"The file extension is already in use",
"Encoding doesn't match bitdepth.",
"invalid image",
"Image width too large",
"Image height too large",
"File size too large"*/

_NAME_END