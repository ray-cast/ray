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
#include <ray/fontft.h>

_NAME_BEGIN
/*
void
FreeFace::setTransform(FT_Matrix* m, FT_Vector* dt)
{
    FT_Set_Transform(_face, m, dt);
}

FreeGlyph::FreeGlyph() noexcept
    :_glyph(nullptr)
{
}

FreeGlyph::~FreeGlyph() noexcept
{
    if (_glyph != (FT_Glyph)FT_GLYPH_SPACE &&
        _glyph != (FT_Glyph)FT_GLYPH_TAB &&
        _glyph != (FT_Glyph)FT_GLYPH_NEWLINE)
    {
        FT_Done_Glyph(_glyph);
    }
}

Font::advance_type
FreeGlyph::getAdvanceX(const Font& font)
{
    Font::advance_type result = 0;
    switch ((Font::size_type)_glyph)
    {
    case FT_KEY_SPACE:
        result = font.getSpaceAdvance();
        break;
    case FT_KEY_TAB:
        result = font.getTabAdvance();
        break;
    case FT_KEY_NEWLINE:
        break;
    default:
        result = _glyph->advance.x >> 16;
    }

    return result;
}

Font::advance_type
FreeGlyph::getAdvanceY(const Font& font)
{
    Font::advance_type result = 0;
    switch ((Font::size_type)_glyph)
    {
    case FT_KEY_SPACE:
        result = font.getSpaceAdvance();
        break;
    case FT_KEY_TAB:
        result = font.getTabAdvance();
        break;
    case FT_KEY_NEWLINE:
        break;
    default:
        result = _glyph->advance.y >> 16;
    }

    return result;
}

bool
FreeType::generate(const std::wstring& name) noexcept
{
    try
    {
        const Font& font = this->font();

        _face->setSize(font.getWidth(), font.getHeight());

        //_descender_advance = abs(_face->size->metrics.descender) >> 6;
        //_space_advance = size / 2;
        //_tab_advance = size * 2;
        //_new_line_advance = size * 15 / 10;

        if (font.isItalic())
        {
            float lean = font.getItalicWeight();

            FT_Matrix matrix;
            matrix.xx = 0x10000L;
            matrix.xy = (FT_Fixed)(lean * 0x10000L);
            matrix.yx = 0;
            matrix.yy = 0x10000L;

            _face->setTransform(&matrix, 0);
        }

        for (auto& it : name)
        {
            FreeGlyph glyph;

            if (_face->generateGlyph(it, font.getLoadMode(), font.getRenderMode(), glyph))
            {
                if (this->font().isBold())
                {
                    FT_BitmapGlyph BitmapGlyp = (FT_BitmapGlyph)glyph;
                    FT_Bitmap_Embolden(*_library, &BitmapGlyp->bitmap, 32, 32);
                }
            }
        }

        return true;
    }
    catch (...)
    {
        return false;
    }
}

FreeType::advance_type
FreeType::getAdvanceX(size_type index)
{
    return _glyphs[index].getAdvanceX(this->font());
}

FreeType::advance_type
FreeType::getAdvanceY(size_type index)
{
    return _glyphs[index].getAdvanceY(this->font());
}

void
FreeType::FONT_DrawGlyph(FT_Glyph Glyph)
{
    assert(nullptr != Glyph);

    auto FONT_GetGray2Bitmap = [](const std::uint8_t *buffer, int Offset)
    {
        std::uint8_t Array = buffer[Offset >> 3];
        int Index = 7 - (Offset % 8);
        return (Array >> Index) & 0x1;
    };

    const Font& font = this->font();

    glPushMatrix();
    glTranslatef((GLfloat)_advance.x, (GLfloat)_advance.y , 0.0f);

    switch((size_type)Glyph)
    {
    case FT_GLYPH_SPACE:
            _advance.x += font.getSpaceAdvance();
            break;
    case FT_GLYPH_TAB:
            _advance.x += font.getTabAdvance();
            break;
    case FT_GLYPH_NEWLINE:
            _advance.y -= font.getNewLineAdvance();
            _advance.x = 0;
            break;
    default:
        {
            FT_BitmapGlyph BitmapGlyp = (FT_BitmapGlyph)Glyph;
            FT_Bitmap* Bitmap = &BitmapGlyp->bitmap;
            glTranslatef((GLfloat)BitmapGlyp->left, (GLfloat)(BitmapGlyp->top - Bitmap->rows), 0.0f);

            if (font.isSmooth())
            {
                int Width = Bitmap->width;
                int Height = Bitmap->rows;
                unsigned char* pBuffer = Bitmap->buffer;

                glBegin(GL_POINTS);

                for (int i = Height; i > 0; i--)
                {
                    for (int j = 0; j < Width; j++)
                    {
                        unsigned int alpha = *pBuffer++;
                        if (alpha)
                        {
                            glVertex2i(j, i);
                        }
                    }
                }

                glEnd();
            }
            else
            {
                glBegin(GL_POINTS);

                int BitHeight = Bitmap->rows;
                int BitWidth = Bitmap->pitch << 3;
                int BitStep = Bitmap->pitch;
                const std::uint8_t* pbuffer = Bitmap->buffer;

                for (int i = BitHeight; i > 0; i--)
                {
                    for (int j = 0; j < BitWidth; j++)
                    {
                        int bit = FONT_GetGray2Bitmap(pbuffer, j);
                        if (bit)
                        {
                            glVertex2i(j, i);
                        }
                    }

                    pbuffer += BitStep;
                }

                glEnd();
            }

            _advance.x += Glyph->advance.x;
        }
        break;
    }

    glPopMatrix();
}
*/

FreeType::FreeType() noexcept
    : _library(nullptr)
    , _face(nullptr)
{
}

FreeType::~FreeType() noexcept
{
    this->destroy();
}

bool
FreeType::doCanRead(istream& stream) const noexcept
{
    return true;
}

bool
FreeType::doLoad(Font& font, istream& stream) noexcept
{
    this->destroy();

    stream.seekg(0, ios_base::end);
    auto size = (std::size_t)stream.tellg();
    stream.seekg(0, ios_base::beg);

    std::vector<char> buffer;
    buffer.resize(size);

    stream.read(buffer.data(), size);

    ::FT_Init_FreeType(&_library);

    FT_Error error = FT_New_Memory_Face(_library, (FT_Byte*)buffer.data(), size, 0, &_face);
    if (error)
    {
        error = ::FT_New_Face(_library, font.getName().c_str(), 0, &_face);
        if (error)
        {
            return false;
        }
    }

    if (!error)
    {
        if (::FT_Select_Charmap(_face, FT_ENCODING_UNICODE))
        {
            return false;
        }

        _load_mode = font.isSmooth() ? FT_LOAD_RENDER | FT_LOAD_CROP_BITMAP | FT_LOAD_FORCE_AUTOHINT : FT_LOAD_CROP_BITMAP;

        return ::FT_Set_Pixel_Sizes(_face, font.getWidth(), font.getHeight()) ? false : true;
    }

    return false;
}

bool
FreeType::doGetGlyph(Font& font, wchar_t chr) noexcept
{
    FT_Glyph glyph = {};

    GlyphDesc desc = {};

    switch (chr)
    {
    case FT_KEY_SPACE:
    {
        desc.glyph = FT_GLYPH_SPACE;
        desc.advance_x = font.getSpaceAdvance().x;
        desc.advance_y = font.getSpaceAdvance().y;
    }
    break;
    case FT_KEY_TAB:
    {
        desc.glyph = FT_GLYPH_TAB;
        desc.advance_x = font.getTabAdvance().x;
        desc.advance_y = font.getTabAdvance().y;
    }
    break;
    case FT_KEY_RETURN:
    case FT_KEY_NEWLINE:
    {
        desc.glyph = FT_GLYPH_NEWLINE;
        desc.advance_x = font.getNewLineAdvance().x;
        desc.advance_y = font.getNewLineAdvance().y;
    }
    break;
    default:
    {
        FT_UInt FT_Index = ::FT_Get_Char_Index(_face, chr);
        if (FT_Index)
        {
            if (::FT_Load_Glyph(_face, FT_Index, _load_mode))
            {
                return false;
            }

            if (::FT_Get_Glyph(_face->glyph, &glyph))
            {
                return false;
            }

            if (::FT_Glyph_To_Bitmap(&glyph, _render_mode, 0, true))
            {
                return false;
            }
        }

        desc.glyph = chr;

        desc.advance_x = glyph->advance.x;
        desc.advance_y = glyph->advance.y;

        FT_BitmapGlyph bitmapGlyp = (FT_BitmapGlyph)glyph;
        desc.left = bitmapGlyp->left;
        desc.top = bitmapGlyp->top;

        FT_Bitmap* bitmap = &bitmapGlyp->bitmap;
        desc.rows = bitmap->rows;
        desc.width = bitmap->width;
        desc.pitch = bitmap->pitch;
        desc.buffer = bitmap->buffer;
        desc.num_grays = bitmap->num_grays;
        desc.pixel_mode = bitmap->pixel_mode;
        desc.palette = bitmap->palette;
        desc.palette_mode = bitmap->palette_mode;
    }
    }

    return true;
}

void
FreeType::destroy() noexcept
{
    for (auto& glyph : _glyphs)
    {
        ::FT_Done_Glyph(glyph.second);
        glyph.second = nullptr;
    }

    _glyphs.clear();

    if (_library)
    {
        ::FT_Done_FreeType(_library);
        _library = nullptr;
    }

    if (_face)
    {
        FT_Done_Face(_face);
        _face = nullptr;
    }
}

_NAME_END