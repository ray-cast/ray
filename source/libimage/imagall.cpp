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

namespace image
{

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

std::vector<ImageHandlerPtr> _handlers = {
#if _BUILD_PNG_HANDLER
	png,
#endif
#if _BUILD_TGA_HANDLER
	tga,
#endif
#if _BUILD_JPEG_HANDLER
	jpeg,
#endif
#if _BUILD_BMP_HANDLER
	bmp,
#endif
#if _BUILD_DDS_HANDLER
	dds,
#endif
};

const std::vector<ImageHandlerPtr>& GetImageInstanceList()
{
	return _handlers;
}


bool emptyHandler() noexcept
{
	return _handlers.empty();
}

bool addHandler(ImageHandlerPtr handler) noexcept
{
	assert(handler);
	auto it = std::find(_handlers.begin(), _handlers.end(), handler);
	if (it == _handlers.end())
	{
		_handlers.push_back(handler);
		return true;
	}

	return false;
}

bool removeHandler(ImageHandlerPtr handler) noexcept
{
	assert(handler);
	auto it = std::find(_handlers.begin(), _handlers.end(), handler);
	if (it != _handlers.end())
	{
		_handlers.erase(it);
		return true;
	}

	return false;
}

ImageHandlerPtr findHandler(const char* type) noexcept
{
	if (type)
	{
		for (auto& it : _handlers)
			if (it->doCanRead(type))
				return it;
	}

	return nullptr;
}

ImageHandlerPtr findHandler(StreamReader& stream) noexcept
{
	if (!stream.good())
		return nullptr;

	for (auto it : _handlers)
	{
		stream.seekg(0, std::ios_base::beg);

		if (it->doCanRead(stream))
		{
			stream.seekg(0, std::ios_base::beg);
			return it;
		}
	}

	return nullptr;
}

ImageHandlerPtr findHandler(StreamReader& stream, const char* type) noexcept
{
	ImageHandlerPtr result = findHandler(type);
	if (result)
		return result;
	return findHandler(stream, type);
}

}

_NAME_END