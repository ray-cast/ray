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
#ifndef _H_FONT_H_
#define _H_FONT_H_

#include <ray/gdicmn.h>
#include <ray/reference.h>
#include <ray/istream.h>
#include <map>

_NAME_BEGIN

class Font;
class Material;
class Image;
class EXPORT FontHandler
{
public:
    virtual ~FontHandler() noexcept {};

    virtual bool doCanRead(istream& stream) const noexcept = 0;
    virtual bool doLoad(Font& font, istream& stream) noexcept = 0;
    virtual bool doGetGlyph(Font& font, wchar_t text) noexcept = 0;
};

class EXPORT GlyphDesc
{
public:
    // 字符
    wchar_t glyph;

    // 字体推进
    int                advance_x;
    int                advance_y;

    int                left;
    int                top;
    int             rows;
    int             width;
    int             pitch;
    unsigned char*  buffer;
    short           num_grays;
    char            pixel_mode;
    char            palette_mode;
    void*           palette;
};

class EXPORT Font final : public Reference<Font>
{
public:
    typedef std::wstring::value_type value_type;
    typedef std::wstring::size_type size_type;

    typedef unsigned int font_size;

    static const size_type none = (size_type)0;

public:
    Font();
    Font(const std::string& name, font_size w, font_size h);
    Font(const std::string& name, font_size w, font_size h, bool bold, bool italic, bool descender, bool smooth);
    ~Font() noexcept;

    Font& create(const std::string& name, font_size w, font_size h);
    Font& create(const std::string& name, font_size w, font_size h, bool bold, bool italic, bool descender, bool smooth);

    void setName(const std::string& name) noexcept;
    const std::string& getName() const noexcept;

    void setItalic(bool italic) noexcept { _is_italic = italic; }
    void setBold(bool bold)     noexcept { _is_bold = bold; }
    void setSmooth(bool smooth) noexcept { _is_smooth = smooth; }
    void setWidth(font_size w)  noexcept { _width = w; }
    void setHeight(font_size h) noexcept { _height = h; }
    void setDescender(bool descender) noexcept { _is_dscender = descender; }
    void setSize(font_size w, font_size h) noexcept
    {
        _width = w;
        _height = h;
    }

    bool isBold()     const noexcept { return _is_bold; }
    bool isItalic()   const noexcept { return _is_italic; }
    bool isDscender() const noexcept { return _is_dscender; }
    bool isSmooth()   const noexcept { return _is_smooth; }
    bool isShadow()   const noexcept { return _is_shadow; }

    font_size getWidth()  const noexcept { return _width; }
    font_size getHeight() const noexcept { return _height; }

    float getNormalWeight() const noexcept { return _normal_weight; }
    float getItalicWeight() const noexcept { return _italic_weight; }
    float getBoldWeight()   const noexcept { return _bold_weight; }

    Size getNewLineAdvance()   const noexcept { return _new_line_advance; }
    Size getDescenderAdvance() const noexcept { return _descender_advance; }
    Size getTabAdvance()       const noexcept { return _tab_advance; }
    Size getSpaceAdvance()     const noexcept { return _space_advance; }

    void addGlyph(GlyphDesc* glyph);
    bool getGlyph(wchar_t chr, const GlyphDesc& desc);
    bool getGlyphs(std::wstring text, std::vector<const GlyphDesc*>& desc);

    void setMaterial(Material* material) noexcept;
    Material* getMaterial() const noexcept;

public:
    bool emptyHandler() const noexcept;
    void setHandler(FontHandler* handler) noexcept;

private:
    std::string _name;

    bool _is_italic;   // 斜体
    bool _is_bold;     // 加粗
    bool _is_smooth;   // 平滑
    bool _is_shadow;   // 阴影
    bool _is_dscender; // 下划线

    font_size _width;  // 字体宽度
    font_size _height; // 字体高度

    float _normal_weight; // 常规加粗
    float _bold_weight;   // 粗体加粗
    float _italic_weight; // 倾斜度

    Size _space_advance;     // 空格推进
    Size _tab_advance;       // Tab推进
    Size _new_line_advance;  // 回车推进
    Size _descender_advance; // 下划线位置

    float _gamma;         // gamma
    float _contrast;      // 对比度
    float _concentration; // 渲染浓度

    std::map<wchar_t, GlyphDesc*> _font_map;

    std::shared_ptr<Image> _image;

    Material* _material;

    FontHandler* _handler;
};

_NAME_END

#endif