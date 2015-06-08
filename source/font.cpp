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
#include <ray/font.h>
#include <ray/fontall.h>
#include <ray/material.h>
#include <ray/fstream.h>

_NAME_BEGIN

Font::Font()
    : _width(10)
    , _height(10)
    , _material(nullptr)
    , _handler(nullptr)
{
};

Font::Font(const std::string& name, font_size w, font_size h)
    : _width(10)
    , _height(10)
    , _material(nullptr)
    , _handler(nullptr)
{
    this->create(name, w, h);
}

Font::Font(const std::string& name, font_size w, font_size h, bool Bold, bool Italic, bool Descender, bool Smooth)
    : _width(10)
    , _height(10)
    , _material(nullptr)
    , _handler(nullptr)
{
    this->create(name, w, h, Bold, Italic, Descender, Smooth);
}

Font::~Font() noexcept
{
    for (auto& it : _font_map)
    {
        delete it.second;
    }

    delete _handler;
    delete _material;
};

Font&
Font::create(const std::string& name, font_size w, font_size h)
{
    return this->create(name, w, h, false, false, false, false);
}

Font&
Font::create(const std::string& name, font_size w, font_size h, bool bold, bool italic, bool descender, bool smooth)
{
    ifstream stream;
    if (stream.open(name))
    {
        if (emptyHandler())
            GetFontInstanceList(*this);

        if (_handler)
        {
            if (_handler->doCanRead(stream))
            {
                this->setName(name);
                this->setWidth(w);
                this->setHeight(h);
                this->setBold(bold);
                this->setItalic(italic);
                this->setSmooth(smooth);
                this->setDescender(descender);

                if (_handler->doLoad(*this, stream))
                {
                }
            }
        }
    }

    return *this;
}

void
Font::setName(const std::string& name) noexcept
{
    _name = name;
}

const std::string&
Font::getName() const noexcept
{
    return _name;
}

void
Font::addGlyph(GlyphDesc* glyph)
{
}

bool
Font::getGlyph(wchar_t chr, const GlyphDesc& desc)
{
    return false;
}

bool
Font::getGlyphs(std::wstring text, std::vector<const GlyphDesc*>& desc)
{
    for (auto it : text)
    {
        GlyphDesc* info = _font_map[it];
        if (!info)
        {
            if (!this->getGlyph(it, *info))
            {
                return false;
            }

            _font_map[it] = info;
        }

        desc.push_back(info);
    }

    return true;
}

bool
Font::emptyHandler() const noexcept
{
    return _handler ? false : true;
}

void
Font::setHandler(FontHandler* handler) noexcept
{
    if (_handler)
        delete _handler;
    _handler = handler;
}

void
Font::setMaterial(Material* material) noexcept
{
    if (_material)
        delete _material;
    _material = material;
}

Material*
Font::getMaterial() const noexcept
{
    return _material;
}

_NAME_END