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
#include <ray/mygui_textbox.h>

_NAME_BEGIN

__ImplementSubClass(MyGuiButton, GuiButton, "MyGuiButton")
__ImplementSubClass(MyGuiButtonImpl, MyGuiWidget, "MyGuiButton")

MyGuiButtonImpl::MyGuiButtonImpl() noexcept
	: _button(nullptr)
	, _parent(nullptr)
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

	_textbox = std::make_shared<MyGuiTextBox>(_button, false);

	this->setWidget(_button);
	
	return _button ? true : false;
}

GuiTextBoxPtr
MyGuiButtonImpl::getGuiTextBox() const noexcept
{
	return _textbox;
}

void 
MyGuiButtonImpl::setStateSelected(bool value) noexcept
{
	assert(_button);
	_button->setStateSelected(value);
}

bool 
MyGuiButtonImpl::getStateSelected() const noexcept
{
	assert(_button);
	return _button->getStateSelected();
}

void 
MyGuiButtonImpl::setModeImage(bool value) noexcept
{
	assert(_button);
	_button->setModeImage(value);
}

bool 
MyGuiButtonImpl::getModeImage() const noexcept
{
	assert(_button);
	return _button->getModeImage();
}

void 
MyGuiButtonImpl::setImageResource(const std::string& name) noexcept
{
	assert(_button);
	_button->setImageResource(name);
}

void 
MyGuiButtonImpl::setImageGroup(const std::string& name) noexcept
{
	assert(_button);
	_button->setImageGroup(name);
}

void 
MyGuiButtonImpl::setImageName(const std::string& name) noexcept
{
	assert(_button);
	_button->setImageName(name);
}

MyGuiButton::MyGuiButton() noexcept
	: GuiButton(_impl)
{
}

MyGuiButton::~MyGuiButton() noexcept
{
}

GuiTextBoxPtr
MyGuiButton::getGuiTextBox() const noexcept
{
	return _impl.getGuiTextBox();
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

_NAME_END
#endif