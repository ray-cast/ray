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
#if defined(_BUILD_GUI)
#include <ray/gui_label_component.h>
#include <ray/gui_textbox.h>
#include <ray/gui_system.h>

_NAME_BEGIN

__ImplementSubClass(GuiLabelComponent, GuiWidgetComponent, "GuiLabel")

GuiLabelComponent::GuiLabelComponent() noexcept
{
	_label = GuiSystem::instance()->createWidget<GuiTextBox>();
	_label->create();

	this->setGuiWidget(_label);
}

GuiLabelComponent::GuiLabelComponent(GuiTextBoxPtr label) noexcept
	: _label(label)
{
	this->setGuiWidget(_label);
}

GuiLabelComponent::~GuiLabelComponent() noexcept
{
}

Rect
GuiLabelComponent::getTextRegion() noexcept
{
	assert(_label);
	return _label->getTextRegion();
}

void 
GuiLabelComponent::getTextSize(int& w, int& h) noexcept
{
	assert(_label);
	return _label->getTextSize(w, h);
}

void 
GuiLabelComponent::setText(const std::string& value) noexcept
{
	assert(_label);
	_label->setText(value);
}

const std::string& 
GuiLabelComponent::getText() const noexcept
{
	assert(_label);
	return _label->getText();
}

void 
GuiLabelComponent::setFontName(const std::string& value) noexcept
{
	assert(_label);
	_label->setFontName(value);
}

const std::string& 
GuiLabelComponent::getFontName() noexcept
{
	assert(_label);
	return _label->getFontName();
}

void 
GuiLabelComponent::setFontHeight(int value) noexcept
{
	assert(_label);
	_label->setFontHeight(value);
}

int 
GuiLabelComponent::getFontHeight() noexcept
{
	assert(_label);
	return _label->getFontHeight();
}

void 
GuiLabelComponent::setTextAlign(GuiWidgetAlign value) noexcept
{
	assert(_label);
	_label->setTextAlign(value);
}

GuiWidgetAlign 
GuiLabelComponent::getTextAlign() noexcept
{
	assert(_label);
	return _label->getTextAlign();
}

void 
GuiLabelComponent::setTextColour(const float4& value) noexcept
{
	assert(_label);
	_label->setTextColour(value);
}

float4 
GuiLabelComponent::getTextColour() noexcept
{
	assert(_label);
	return _label->getTextColour();
}

void 
GuiLabelComponent::setTextWithReplacing(const std::string& value) noexcept
{
	assert(_label);
	_label->setTextWithReplacing(value);
}

void 
GuiLabelComponent::setTextShadowColour(const float4& value) noexcept
{
	assert(_label);
	_label->setTextShadowColour(value);
}

float4 
GuiLabelComponent::getTextShadowColour() noexcept
{
	assert(_label);
	return _label->getTextShadowColour();
}

void 
GuiLabelComponent::setTextShadow(bool value) noexcept
{
	assert(_label);
	_label->setTextShadow(value);
}

bool 
GuiLabelComponent::getTextShadow() const noexcept
{
	assert(_label);
	return _label->getTextShadow();
}

void
GuiLabelComponent::load(iarchive& reader) noexcept
{
	GuiWidgetComponent::load(reader);

	std::string font;
	int fontSize = 12;
	std::string text;
	std::string textAlign;
	bool textShadow = false;
	float4 textColor(0, 0, 0, 0);
	float4 textShadowColor(0, 0, 0, 0);

	reader >> make_archive(font, "font");
	reader >> make_archive(fontSize, "fontSize");
	reader >> make_archive(text, "text");
	reader >> make_archive(textAlign, "textAlign");
	reader >> make_archive(textShadow, "textShadow");
	reader >> make_archive(textColor, "textColor");
	reader >> make_archive(textShadowColor, "textShadowColor");

	this->setText(text);
	this->setTextColour(textColor);
	this->setTextShadow(textShadow);
	this->setTextShadowColour(textShadowColor);

	if (!textAlign.empty())
		this->setTextAlign(GuiWidgetAlign::parse(textAlign));

	if (!font.empty())
		this->setFontName(font);

	this->setFontHeight(fontSize);
}

void 
GuiLabelComponent::save(oarchive& write) noexcept
{
}

GameComponentPtr 
GuiLabelComponent::clone() const noexcept
{
	auto other = std::make_shared<GuiLabelComponent>();
	return other;
}

_NAME_END
#endif