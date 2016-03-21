// +----------------------------------------------------------------------
// | Project : ray.
// | All rights reserved.
// +----------------------------------------------------------------------
// | Copyright (c) 2013-2016.
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
#ifndef _H_FONT_BITMAP_H_
#define _H_FONT_BITMAP_H_

#include <ray/font.h>

_NAME_BEGIN

class EXPORT FontGlyph
{
public:
	std::uint16_t glyph;

	std::uint16_t advanceX;
	std::uint16_t advanceY;

	std::uint16_t offsetX;
	std::uint16_t offsetY;

	float left;
	float top;
	float width;
	float height;
};

typedef std::vector<FontGlyph> FontGlyphs;
typedef std::vector<std::uint8_t> FontBitmaps;

class EXPORT FontBitmap : public Font
{
public:
	FontBitmap() noexcept;
	virtual ~FontBitmap() noexcept;

	virtual void createFontMapping(StreamReader& stream, const std::wstring& charsets, std::size_t fontSize, std::size_t distanceSize, std::size_t numThreads) = 0;
	virtual void createFontMapping(const std::string& fontpath, const std::wstring& charsets, std::size_t fontSize, std::size_t distanceSize, std::size_t numThreads) = 0;

	virtual void clear() noexcept = 0;
};

_NAME_END

#endif