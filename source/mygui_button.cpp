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
#include <ray/mygui_button.h>

_NAME_BEGIN

__ImplementSubClass(MyGuiButton, GuiButton, "MyGuiButton")
__ImplementSubClass(MyGuiButtonImpl, MyGuiWidget, "MyGuiButton")

MyGuiButtonImpl::MyGuiButtonImpl() noexcept
	: _button(nullptr)
	, _parent(nullptr)
{
}

MyGuiButtonImpl::MyGuiButtonImpl(MyGUI::Widget* parent) noexcept
	: _button(nullptr)
	, _parent(parent)
{
}

MyGuiButtonImpl::~MyGuiButtonImpl() noexcept
{
	this->destroy();
}

bool
MyGuiButtonImpl::create() except
{
	assert(!_button);
	if (_parent)
		_button = _parent->createWidget<MyGUI::Button>("Button", 0, 0, 0, 0, MyGUI::Align::Default, "");
	else
		_button = MyGUI::Gui::getInstance().createWidget<MyGUI::Button>("Button", 0, 0, 0, 0, MyGUI::Align::Default, "Main", "");
	this->setWidget(_button);
	
	return _button ? true : false;
}


Viewport
MyGuiButtonImpl::getTextRegion() noexcept
{
	auto region = _button->getTextRegion();
	return Viewport(region.left, region.top, region.width, region.height);
}

void
MyGuiButtonImpl::getTextSize(int& w, int& h) noexcept
{
	auto size = _button->getTextSize();
	w = size.width;
	h = size.height;
}

void
MyGuiButtonImpl::setCaption(const std::string& value) noexcept
{
	_caption = value;
	_button->setCaption(value);
}

const std::string&
MyGuiButtonImpl::getCaption() const noexcept
{
	return _caption;
}

void
MyGuiButtonImpl::setFontName(const std::string& value) noexcept
{
	_button->setFontName(value);
}


const std::string&
MyGuiButtonImpl::getFontName() noexcept
{
	return _button->getFontName();
}

void
MyGuiButtonImpl::setFontHeight(int _value) noexcept
{
	_button->setFontHeight(_value);
}

int
MyGuiButtonImpl::getFontHeight() noexcept
{
	return _button->getFontHeight();
}

void
MyGuiButtonImpl::setTextAlign(GuiWidgetAlign _value) noexcept
{
	_button->setTextAlign(GuiAlignToMyGui(_value));
}

GuiWidgetAlign
MyGuiButtonImpl::getTextAlign() noexcept
{
	return MyGuiToGuiAlign(_button->getTextAlign());
}

void
MyGuiButtonImpl::setTextColour(const float4& _value) noexcept
{
	MyGUI::Colour color;
	color.red = _value.x;
	color.green = _value.y;
	color.blue = _value.z;
	color.alpha = _value.w;
	_button->setTextColour(color);
}

float4
MyGuiButtonImpl::getTextColour() noexcept
{
	MyGUI::Colour colour = _button->getTextColour();

	float4 color;
	color.x = colour.red;
	color.y = colour.green;
	color.z = colour.blue;
	color.w = colour.alpha;
	return color;
}

void
MyGuiButtonImpl::setCaptionWithReplacing(const std::string& _value) noexcept
{
	_button->setCaptionWithReplacing(_value);
}

void
MyGuiButtonImpl::setTextShadowColour(const float4& value) noexcept
{
	MyGUI::Colour color;
	color.red = value.x;
	color.green = value.y;
	color.blue = value.z;
	color.alpha = value.w;
	_button->setTextShadowColour(color);
}

float4
MyGuiButtonImpl::getTextShadowColour() noexcept
{
	auto colour = _button->getTextShadowColour();

	float4 color;
	color.x = colour.red;
	color.y = colour.green;
	color.z = colour.blue;
	color.w = colour.alpha;
	return color;
}

void
MyGuiButtonImpl::setTextShadow(bool _value) noexcept
{
	_button->setTextShadow(_value);
}

bool
MyGuiButtonImpl::getTextShadow() noexcept
{
	return _button->getTextShadow();
}

void 
MyGuiButtonImpl::setStateSelected(bool value) noexcept
{
	assert(!_button);
	_button->setStateSelected(value);
}

bool 
MyGuiButtonImpl::getStateSelected() const noexcept
{
	return _button->getStateSelected();
}

void 
MyGuiButtonImpl::setModeImage(bool value) noexcept
{
	_button->setModeImage(value);
}

bool 
MyGuiButtonImpl::getModeImage() const noexcept
{
	return _button->getModeImage();
}

void 
MyGuiButtonImpl::setImageResource(const std::string& name) noexcept
{
	_button->setImageResource(name);
}

void 
MyGuiButtonImpl::setImageGroup(const std::string& name) noexcept
{
	_button->setImageGroup(name);
}

void 
MyGuiButtonImpl::setImageName(const std::string& name) noexcept
{
	_button->setImageName(name);
}

MyGuiButton::MyGuiButton() noexcept
	: GuiButton(_impl)
{
}

MyGuiButton::MyGuiButton(MyGUI::Widget* parent) noexcept
	: GuiButton(_impl)
	, _impl(parent)
{
}

MyGuiButton::~MyGuiButton() noexcept
{
}

void 
MyGuiButton::setStateSelected(bool value) noexcept
{
	_impl.setStateSelected(value);
}

bool 
MyGuiButton::getStateSelected() const noexcept
{
	return _impl.getStateSelected();
}

void 
MyGuiButton::setModeImage(bool value) noexcept
{
	_impl.setModeImage(value);
}

bool 
MyGuiButton::getModeImage() const noexcept
{
	return _impl.getModeImage();
}

void 
MyGuiButton::setImageResource(const std::string& name) noexcept
{
	_impl.setImageResource(name);
}

void 
MyGuiButton::setImageGroup(const std::string& name) noexcept
{
	_impl.setImageGroup(name);
}

void 
MyGuiButton::setImageName(const std::string& name) noexcept
{
	_impl.setImageName(name);
}

Viewport
MyGuiButton::getTextRegion() noexcept
{
	return _impl.getTextRegion();
}

void
MyGuiButton::getTextSize(int& w, int& h) noexcept
{
	_impl.getTextSize(w, h);
}

void
MyGuiButton::setCaption(const std::string& _value) noexcept
{
	_impl.setCaption(_value);
}

const string&
MyGuiButton::getCaption() const noexcept
{
	return _impl.getCaption();
}

void
MyGuiButton::setFontName(const std::string& _value) noexcept
{
	_impl.setFontName(_value);
}

const std::string&
MyGuiButton::getFontName() noexcept
{
	return _impl.getFontName();
}

void
MyGuiButton::setFontHeight(int _value) noexcept
{
	_impl.setFontHeight(_value);
}

int
MyGuiButton::getFontHeight() noexcept
{
	return _impl.getFontHeight();
}

void
MyGuiButton::setTextAlign(GuiWidgetAlign _value) noexcept
{
	_impl.setTextAlign(_value);
}

GuiWidgetAlign
MyGuiButton::getTextAlign() noexcept
{
	return _impl.getTextAlign();
}

void
MyGuiButton::setTextColour(const float4& value) noexcept
{
	_impl.setTextColour(value);
}

float4
MyGuiButton::getTextColour() noexcept
{
	return _impl.getTextColour();
}

void
MyGuiButton::setCaptionWithReplacing(const std::string& _value) noexcept
{
	_impl.setCaptionWithReplacing(_value);
}

void
MyGuiButton::setTextShadowColour(const float4& value) noexcept
{
	_impl.setTextShadowColour(value);
}

float4
MyGuiButton::getTextShadowColour() noexcept
{
	return _impl.getTextShadowColour();
}

void
MyGuiButton::setTextShadow(bool _value) noexcept
{
	_impl.setTextShadow(_value);
}

bool
MyGuiButton::getTextShadow() noexcept
{
	return _impl.getTextShadow();
}

_NAME_END
#endif