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
#ifndef _H_FREE_TYPE_H_
#define _H_FREE_TYPE_H_

#include <ray/font.h>

#include <ft2build.h>
#include <freetype.h>
#include <ftglyph.h>
#include <ftbitmap.h>
#include <ftstroke.h>

_NAME_BEGIN

#define FT_KEY_SPACE   0x20
#define FT_KEY_TAB     0x09
#define FT_KEY_RETURN  0x0d
#define FT_KEY_NEWLINE 0x0a

#define FT_GLYPH_SPACE   FT_KEY_SPACE
#define FT_GLYPH_TAB     FT_KEY_TAB
#define FT_GLYPH_NEWLINE FT_KEY_NEWLINE

class FreeType : public FontHandler
{
public:
    FreeType() noexcept;
    ~FreeType() noexcept;

    bool doCanRead(istream& stream) const noexcept;
    bool doLoad(Font& font, istream& stream) noexcept;
    bool doGetGlyph(Font& font, wchar_t chr) noexcept;

    void destroy() noexcept;

private:

    FT_Library _library;
    FT_Face _face;

    Size _advance;

    int _load_mode;
    FT_Render_Mode _render_mode;

    std::map<wchar_t, FT_Glyph> _glyphs;
};

_NAME_END

#endif