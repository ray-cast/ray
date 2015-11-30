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
#include <ray/mygui_window.h>

_NAME_BEGIN

__ImplementSubClass(MyGuiWindow, GuiWindow, "MyGuiWindow")
__ImplementSubClass(MyGuiWindowImpl, MyGuiWidget, "MyGuiWindowImpl")

MyGuiWindowImpl::MyGuiWindowImpl() noexcept
	: _window(nullptr)
	, _parent(nullptr)
{
}

MyGuiWindowImpl::~MyGuiWindowImpl() noexcept
{
	this->destroy();
}

bool
MyGuiWindowImpl::create() except
{
	assert(!_window);
	if (_parent)
		_window = _parent->createWidget<MyGUI::Window>("", 0, 0, 0, 0, MyGUI::Align::Default);
	else
		_window = MyGUI::Gui::getInstance().createWidget<MyGUI::Window>("", 0, 0, 0, 0, MyGUI::Align::Default, "Main");

	_textbox = std::make_shared<MyGuiTextBox>(_window, false);

	this->setWidget(_window);

	return _window ? true : false;
}

GuiTextBoxPtr 
MyGuiWindowImpl::getGuiTextBox() const noexcept
{
	return _textbox;
}

void 
MyGuiWindowImpl::setVisibleSmooth(bool _value) noexcept
{
	_window->setVisibleSmooth(_value);
}

void
MyGuiWindowImpl::destroySmooth() noexcept
{
	_window->destroySmooth();
}

void 
MyGuiWindowImpl::setAutoAlpha(bool _value) noexcept
{
	assert(_window);
	_window->setAutoAlpha(_value);
}

bool 
MyGuiWindowImpl::getAutoAlpha() const noexcept
{
	assert(_window);
	return _window->getAutoAlpha();
}

void 
MyGuiWindowImpl::setMinSize(int _width, int _height) noexcept
{
	assert(_window);
	_window->setMinSize(_width, _height);
}

void
MyGuiWindowImpl::getMinSize(int& w, int& h) const noexcept
{
	assert(_window);
	auto size = _window->getMinSize();
	w = size.width;
	h = size.height;
}

void 
MyGuiWindowImpl::setMaxSize(int _width, int _height) noexcept
{
	assert(_window);
	_window->setMaxSize(_width, _height);
}

void
MyGuiWindowImpl::getMaxSize(int& w, int& h) const noexcept
{
	assert(_window);
	auto size = _window->getMaxSize();
	w = size.width;
	h = size.height;
}

MyGuiWindow::MyGuiWindow() noexcept
	: GuiWindow(_impl)
{
}

MyGuiWindow::~MyGuiWindow() noexcept
{
}

GuiTextBoxPtr
MyGuiWindow::getGuiTextBox() const noexcept
{
	return _impl.getGuiTextBox();
}

void 
MyGuiWindow::setVisibleSmooth(bool _value) noexcept
{
	_impl.setVisibleSmooth(_value);
}

void 
MyGuiWindow::destroySmooth() noexcept
{
	_impl.destroySmooth();
}

void 
MyGuiWindow::setAutoAlpha(bool _value) noexcept
{
	_impl.setAutoAlpha(_value);
}

bool 
MyGuiWindow::getAutoAlpha() const noexcept
{
	return _impl.getAutoAlpha();
}

void 
MyGuiWindow::setMinSize(int _width, int _height) noexcept
{
	_impl.setMinSize(_width, _height);
}

void 
MyGuiWindow::getMinSize(int& w, int& h) const noexcept
{
	_impl.getMinSize(w, h);
}

void 
MyGuiWindow::setMaxSize(int _width, int _height) noexcept
{
	_impl.setMaxSize(_width, _height);
}

void 
MyGuiWindow::getMaxSize(int& w, int& h) const noexcept
{
	_impl.getMaxSize(w, h);
}

_NAME_END
#endif