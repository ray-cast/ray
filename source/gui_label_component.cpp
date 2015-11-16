// +----------------------------------------------------------------------
// | Project : ray.
// | All rights reserved.
// +----------------------------------------------------------------------
// | Copyright (c) 2013-2014.
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
#include <ray/gui_label_component.h>
#include <ray/font_distance_field.h>
#include <ray/render_factory.h>
#include <ray/render_texture.h>
#include <ray/utf8.h>

_NAME_BEGIN

__ImplementSubClass(GUILabelComponent, GUIBehaviourComponent, "GUILabel")

GUILabelComponent::GUILabelComponent() noexcept
{
	_color = Vector4(1.0, 1.0, 1.0, 1.0);
}

GUILabelComponent::~GUILabelComponent() noexcept
{
}

GameComponentPtr 
GUILabelComponent::clone() const except
{
	auto other = std::make_shared<GUILabelComponent>();
	other->_color = _color;
	other->_text = _text;
	other->_fontName = _fontName;
	other->_materialName = _materialName;
	other->_texture = _texture;
	other->_material = _material;
	other->_fontSize = _fontSize;
	other->_bitmapSize = _bitmapSize;
	other->_glyphs = _glyphs;
	return other;
}

void 
GUILabelComponent::setText(const std::string& text) noexcept
{
	_text = text;
	this->needUpdate(true);
}

const std::string&
GUILabelComponent::getText() const noexcept
{
	return _text;
}

void 
GUILabelComponent::setFont(const std::string& font) noexcept
{
	_fontName = font;
	if (this->getActive())
		_buildFontBitmap();
	this->needUpdate(true);
}

const std::string& 
GUILabelComponent::getFont() const noexcept
{
	return _fontName;
}

void 
GUILabelComponent::setMaterial(MaterialPtr material) noexcept
{
	_material = material;
	if (this->getActive())
		_buildMaterial();
	this->needUpdate(true);
}

MaterialPtr 
GUILabelComponent::getMaterial() const noexcept
{
	return _material;
}

void 
GUILabelComponent::load(iarchive& reader) noexcept
{
	GameComponent::load(reader);

	std::string font;
	std::string text;
	std::string material;

	reader >> make_name(text);
	reader >> make_name(font);
	reader >> make_name(material);

	_text = text;
	_fontName = font;
	_materialName = material;
}

void 
GUILabelComponent::save(oarchive& write) noexcept
{
}

void
GUILabelComponent::_buildMaterial() noexcept
{
	if (!_materialName.empty())
		_material = RenderFactory::createMaterial(_materialName);
	else
	{
		_material = RenderFactory::createMaterial("dlc:UI/materials/font.mat");
		_material->getParameter("decal")->assign(_texture);
	}
}

void 
GUILabelComponent::_buildFontBitmap() noexcept
{
	FontDistanceField font;
	if (font.load(_fontName))
	{
		auto& glyphs = font.getBitmapGlyphs();
		for (auto& glyph : glyphs)
		{
			_glyphs[glyph.glyph] = std::make_unique<FontGlyph>(glyph);
		}

		_fontSize = font.getFontSize();
		_bitmapSize = font.getBitmapSize();

		auto texture = RenderFactory::createTexture();
		texture->setMipLevel(0);
		texture->setMipSize(0);
		texture->setSize(font.getBitmapSize(), font.getBitmapSize());
		texture->setTexDim(TextureDim::DIM_2D);
		texture->setTexFormat(PixelFormat::R8);
		texture->setStream((void*)font.getBitmapData().data());
		texture->setup();

		_texture = texture;
	}
}

void
GUILabelComponent::onActivate() except
{
	_buildFontBitmap();
	_buildMaterial();
}

void 
GUILabelComponent::buildUIControl(GUILayoutComponentPtr layout) noexcept
{
	std::vector<wchar_t> data(_text.size());
	UTF8toUNICODE(data.data(), data.size(), _text.data(), _text.size());

	Vector2 advance(0, 0);
	Vector3 center = this->getGameObject()->getTranslate();

	float size = (float) _fontSize / _bitmapSize;
	float scale = 2;

	std::vector<float4> rects;
	std::vector<float4> uvs;
	std::vector<float4> colors;

	for (auto& it : data)
	{
		if (it == 0)
			continue;
		else if (it == L' ')
		{
			advance.x += 1;
			continue;
		}			

		auto& glyph = _glyphs[it];
		if (glyph)
		{
			Vector4 rect;
			rect.x = center.x + advance.x;
			rect.y = center.y + advance.y;
			rect.z = center.x + advance.x + scale;
			rect.w = center.y + advance.y + scale;

			Vector4 uv;
			uv.x = glyph->offsetX * size;
			uv.y = glyph->offsetY * size;
			uv.z = glyph->offsetX * size + size;
			uv.w = glyph->offsetY * size + size;

			advance.x += (glyph->left + glyph->left + glyph->width) * scale;

			rects.push_back(rect);
			uvs.push_back(uv);
			colors.push_back(_color);
		}
	}

	auto materialPass = _material->getTech(RenderQueue::RQ_OPAQUE)->getPass(RenderPass::RP_OPAQUES);

	layout->setMaterialPass(materialPass);
	layout->drawQuadsWithUV(rects, uvs, colors);

	this->needUpdate(false);
}

GameComponentPtr 
GUILabelComponent::hitTest(const Vector3& raycast) noexcept
{
	return nullptr;
}

_NAME_END