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
#if defined(_BUILD_MYGUI)
#include <ray/mygui_textbox.h>

_NAME_BEGIN

__ImplementSubClass(MyGuiTextBox, GuiTextBox, "MyGuiTextBox")
__ImplementSubClass(MyGuiTextBoxImpl, MyGuiWidget, "MyGuiTextBoxImpl")

MyGuiTextBoxImpl::MyGuiTextBoxImpl() noexcept
	: _table(nullptr)
	, _parent(nullptr)
	, _destroy(true)
{
}

MyGuiTextBoxImpl::MyGuiTextBoxImpl(MyGUI::TextBox* self, bool destroy) noexcept
	: _table(self)
	, _parent(nullptr)
	, _destroy(destroy)
{
}

MyGuiTextBoxImpl::~MyGuiTextBoxImpl() noexcept
{
	if (_destroy)
		this->destroy();
}

bool
MyGuiTextBoxImpl::create() except
{
	assert(!_table);
	if (_parent)
		_table = _parent->createWidget<MyGUI::TextBox>("TextBox", 0, 0, 0, 0, MyGUI::Align::Center, "");
	else
		_table = MyGUI::Gui::getInstance().createWidget<MyGUI::TextBox>("TextBox", 0, 0, 0, 0, MyGUI::Align::Center, "Main", "");
	this->setWidget(_table);

	return _table ? true : false;
}

void
MyGuiTextBoxImpl::destroy() noexcept
{
	if (_destroy)
	{
		if (_table)
		{
			MyGUI::Gui::getInstance().destroyWidget(_table);
			_table = nullptr;
		}
	}
}

Viewport
MyGuiTextBoxImpl::getTextRegion() noexcept
{
	auto region = _table->getTextRegion();
	return Viewport(region.left, region.top, region.width, region.height);
}

void
MyGuiTextBoxImpl::getTextSize(int& w, int& h) noexcept
{
	auto size = _table->getTextSize();
	w = size.width;
	h = size.height;
}

void
MyGuiTextBoxImpl::setText(const std::string& value) noexcept
{
	_caption = value;
	_table->setCaption(value);
}

const std::string&
MyGuiTextBoxImpl::getText() const noexcept
{
	return _caption;
}

void
MyGuiTextBoxImpl::setFontName(const std::string& value) noexcept
{
	_table->setFontName(value);
}


const std::string&
MyGuiTextBoxImpl::getFontName() noexcept
{
	return _table->getFontName();
}

void
MyGuiTextBoxImpl::setFontHeight(int _value) noexcept
{
	_table->setFontHeight(_value);
}

int
MyGuiTextBoxImpl::getFontHeight() noexcept
{
	return _table->getFontHeight();
}

void
MyGuiTextBoxImpl::setTextAlign(GuiWidgetAlign _value) noexcept
{
	_table->setTextAlign(GuiAlignToMyGui(_value));
}

GuiWidgetAlign
MyGuiTextBoxImpl::getTextAlign() noexcept
{
	return MyGuiToGuiAlign(_table->getTextAlign());
}

void
MyGuiTextBoxImpl::setTextColour(const float4& _value) noexcept
{
	MyGUI::Colour color;
	color.red = _value.x;
	color.green = _value.y;
	color.blue = _value.z;
	color.alpha = _value.w;
	_table->setTextColour(color);
}

float4
MyGuiTextBoxImpl::getTextColour() noexcept
{
	MyGUI::Colour colour = _table->getTextColour();

	float4 color;
	color.x = colour.red;
	color.y = colour.green;
	color.z = colour.blue;
	color.w = colour.alpha;
	return color;
}

void
MyGuiTextBoxImpl::setTextWithReplacing(const std::string& _value) noexcept
{
	_table->setCaptionWithReplacing(_value);
}

void
MyGuiTextBoxImpl::setTextShadowColour(const float4& value) noexcept
{
	MyGUI::Colour color;
	color.red = value.x;
	color.green = value.y;
	color.blue = value.z;
	color.alpha = value.w;
	_table->setTextShadowColour(color);
}

float4
MyGuiTextBoxImpl::getTextShadowColour() noexcept
{
	auto colour = _table->getTextShadowColour();

	float4 color;
	color.x = colour.red;
	color.y = colour.green;
	color.z = colour.blue;
	color.w = colour.alpha;
	return color;
}

void
MyGuiTextBoxImpl::setTextShadow(bool _value) noexcept
{
	_table->setTextShadow(_value);
}

bool
MyGuiTextBoxImpl::getTextShadow() const noexcept
{
	return _table->getTextShadow();
}

MyGuiTextBox::MyGuiTextBox() noexcept
	: GuiTextBox(_impl)
{
}

MyGuiTextBox::MyGuiTextBox(MyGUI::TextBox* self, bool destroy) noexcept
	: GuiTextBox(_impl)
	, _impl(self, destroy)
{
}

MyGuiTextBox::~MyGuiTextBox() noexcept
{
}

Viewport
MyGuiTextBox::getTextRegion() noexcept
{
	return _impl.getTextRegion();
}

void
MyGuiTextBox::getTextSize(int& w, int& h) noexcept
{
	_impl.getTextSize(w, h);
}

void
MyGuiTextBox::setText(const std::string& _value) noexcept
{
	_impl.setText(_value);
}

const string&
MyGuiTextBox::getText() const noexcept
{
	return _impl.getText();
}

void
MyGuiTextBox::setFontName(const std::string& _value) noexcept
{
	_impl.setFontName(_value);
}

const std::string&
MyGuiTextBox::getFontName() noexcept
{
	return _impl.getFontName();
}

void
MyGuiTextBox::setFontHeight(int _value) noexcept
{
	_impl.setFontHeight(_value);
}

int
MyGuiTextBox::getFontHeight() noexcept
{
	return _impl.getFontHeight();
}

void
MyGuiTextBox::setTextAlign(GuiWidgetAlign _value) noexcept
{
	_impl.setTextAlign(_value);
}

GuiWidgetAlign
MyGuiTextBox::getTextAlign() noexcept
{
	return _impl.getTextAlign();
}

void
MyGuiTextBox::setTextColour(const float4& value) noexcept
{
	_impl.setTextColour(value);
}

float4
MyGuiTextBox::getTextColour() noexcept
{
	return _impl.getTextColour();
}

void
MyGuiTextBox::setTextWithReplacing(const std::string& _value) noexcept
{
	_impl.setTextWithReplacing(_value);
}

void
MyGuiTextBox::setTextShadowColour(const float4& value) noexcept
{
	_impl.setTextShadowColour(value);
}

float4
MyGuiTextBox::getTextShadowColour() noexcept
{
	return _impl.getTextShadowColour();
}

void
MyGuiTextBox::setTextShadow(bool _value) noexcept
{
	_impl.setTextShadow(_value);
}

bool
MyGuiTextBox::getTextShadow() const noexcept
{
	return _impl.getTextShadow();
}

_NAME_END
#endif