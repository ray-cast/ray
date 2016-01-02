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
#include "mygui_menu.h"

_NAME_BEGIN

__ImplementSubClass(MyGuiMenuImpl, MyGuiWidget, "MyGuiMenuImpl")
__ImplementSubClass(MyGuiMenu, GuiMenu, "MyGuiMenu")

MyGuiMenuImpl::MyGuiMenuImpl() noexcept
	: _parent(nullptr)
	, _menuBar(nullptr)
{
}

MyGuiMenuImpl::~MyGuiMenuImpl() noexcept
{
	this->destroy();
}

bool
MyGuiMenuImpl::create() except
{
	assert(!_menuBar);
	if (_parent)
		_menuBar = _parent->createWidget<MyGUI::MenuBar>("", 0, 0, 0, 0, MyGUI::Align::Default, "");
	else
		_menuBar = MyGUI::Gui::getInstance().createWidget<MyGUI::MenuBar>("", 0, 0, 0, 0, MyGUI::Align::Default, "Main", "");

	this->setWidget(_menuBar);

	return _menuBar ? true : false;
}

void
MyGuiMenuImpl::setVisibleSmooth(bool value) noexcept
{
	assert(_menuBar);
	_menuBar->setVisibleSmooth(value);
}

std::size_t
MyGuiMenuImpl::getItemCount() const noexcept
{
	assert(_menuBar);
	return _menuBar->getItemCount();
}

void
MyGuiMenuImpl::removeItemAt(std::size_t index) noexcept
{
	assert(_menuBar);
	_menuBar->removeItemAt(index);
}

void
MyGuiMenuImpl::removeAllItems() noexcept
{
	assert(_menuBar);
	_menuBar->removeAllItems();
}

void
MyGuiMenuImpl::setItemDataAt(std::size_t index, const std::string& data) noexcept
{
	assert(_menuBar);
	_menuBar->setItemDataAt(index, data);
}

void
MyGuiMenuImpl::clearItemDataAt(std::size_t index) noexcept
{
	assert(_menuBar);
	_menuBar->clearItemDataAt(index);
}

void
MyGuiMenuImpl::setItemIdAt(std::size_t index, const std::string& id) noexcept
{
	assert(_menuBar);
	_menuBar->setItemIdAt(index, id);
}

std::string
MyGuiMenuImpl::getItemIdAt(std::size_t index) noexcept
{
	assert(_menuBar);
	return _menuBar->getItemIdAt(index);
}

void
MyGuiMenuImpl::setItemNameAt(std::size_t index, const std::string& name) noexcept
{
	assert(_menuBar);
	_menuBar->setItemNameAt(index, name);
}

std::string
MyGuiMenuImpl::getItemNameAt(size_t index) noexcept
{
	assert(_menuBar);
	return _menuBar->getItemNameAt(index);
}

std::size_t
MyGuiMenuImpl::findItemIndexWith(const std::string& name) noexcept
{
	assert(_menuBar);
	return _menuBar->findItemIndexWith(name);
}

void
MyGuiMenuImpl::setItemChildVisibleAt(std::size_t index, bool visible) noexcept
{
	assert(_menuBar);
	_menuBar->setItemChildVisibleAt(index, visible);
}

void
MyGuiMenuImpl::removeItemChildAt(std::size_t index) noexcept
{
	assert(_menuBar);
	_menuBar->removeItemChildAt(index);
}

void
MyGuiMenuImpl::setItemTypeAt(std::size_t index, GuiMenuItemType type) noexcept
{
	assert(_menuBar);
	_menuBar->setItemTypeAt(index, convert(type));
}

GuiMenuItemType
MyGuiMenuImpl::getItemTypeAt(std::size_t index) noexcept
{
	assert(_menuBar);
	return convert(_menuBar->getItemTypeAt(index));
}

void
MyGuiMenuImpl::setPopupAccept(bool value) noexcept
{
	assert(_menuBar);
	_menuBar->setPopupAccept(value);
}

bool
MyGuiMenuImpl::getPopupAccept() const noexcept
{
	assert(_menuBar);
	return _menuBar->getPopupAccept();
}

void
MyGuiMenuImpl::setVerticalAlignment(bool value) noexcept
{
	assert(_menuBar);
	_menuBar->setVerticalAlignment(value);
}

bool
MyGuiMenuImpl::getVerticalAlignment() const noexcept
{
	assert(_menuBar);
	return _menuBar->getVerticalAlignment();
}

void 
MyGuiMenuImpl::addMenuCtrlCloseListener(std::function<void()>& func) noexcept
{
	assert(_menuBar);
	assert(!_onMenuCtrlClose.find(func));
	if (_onMenuCtrlClose.empty())
		_menuBar->eventMenuCtrlClose += MyGUI::newDelegate(this, &MyGuiMenuImpl::onMenuCtrlClose);
	_onMenuCtrlClose.attach(func);
}

void 
MyGuiMenuImpl::addMenuCtrlAcceptListener(std::function<void()>& func) noexcept
{
	assert(_menuBar);
	assert(!_onMenuCtrlAccept.find(func));
	if (_onMenuCtrlAccept.empty())
		_menuBar->eventMenuCtrlAccept += MyGUI::newDelegate(this, &MyGuiMenuImpl::onMenuCtrlAccept);
	_onMenuCtrlAccept.attach(func);
}

void 
MyGuiMenuImpl::removeMenuCtrlCloseListener(std::function<void()>& func) noexcept
{
	assert(_onMenuCtrlClose.find(func));
	_onMenuCtrlClose.remove(func);
}

void 
MyGuiMenuImpl::removeMenuCtrlAcceptListener(std::function<void()>& func) noexcept
{
	assert(_onMenuCtrlAccept.find(func));
	_onMenuCtrlAccept.remove(func);
}

void 
MyGuiMenuImpl::onMenuCtrlClose(MyGUI::MenuControl*) noexcept
{
	_onMenuCtrlClose.run();
}

void 
MyGuiMenuImpl::onMenuCtrlAccept(MyGUI::MenuControl*, MyGUI::MenuItem*) noexcept
{
	_onMenuCtrlAccept.run();
}

MyGuiMenu::MyGuiMenu() noexcept
	: GuiMenu(_impl)
{
}

MyGuiMenu::~MyGuiMenu() noexcept
{
}

void
MyGuiMenu::setVisibleSmooth(bool value) noexcept
{
	_impl.setVisibleSmooth(value);
}

std::size_t
MyGuiMenu::getItemCount() const noexcept
{
	return _impl.getItemCount();
}

void
MyGuiMenu::removeItemAt(std::size_t index) noexcept
{
	_impl.removeItemAt(index);
}

void
MyGuiMenu::removeAllItems() noexcept
{
	_impl.removeAllItems();
}

void
MyGuiMenu::setItemDataAt(std::size_t index, const std::string& data) noexcept
{
	_impl.setItemDataAt(index, data);
}

void
MyGuiMenu::clearItemDataAt(std::size_t index) noexcept
{
	_impl.clearItemDataAt(index);
}

void
MyGuiMenu::setItemIdAt(std::size_t index, const std::string& id) noexcept
{
	_impl.setItemIdAt(index, id);
}

std::string
MyGuiMenu::getItemIdAt(std::size_t index) noexcept
{
	return _impl.getItemIdAt(index);
}

void
MyGuiMenu::setItemNameAt(std::size_t index, const std::string& name) noexcept
{
	_impl.setItemNameAt(index, name);
}

std::string
MyGuiMenu::getItemNameAt(size_t index) noexcept
{
	return _impl.getItemNameAt(index);
}

std::size_t
MyGuiMenu::findItemIndexWith(const std::string& name) noexcept
{
	return _impl.findItemIndexWith(name);
}

void
MyGuiMenu::setItemChildVisibleAt(std::size_t index, bool visible) noexcept
{
	_impl.setItemChildVisibleAt(index, visible);
}

void
MyGuiMenu::removeItemChildAt(std::size_t index) noexcept
{
	_impl.removeItemChildAt(index);
}

void
MyGuiMenu::setItemTypeAt(std::size_t index, GuiMenuItemType type) noexcept
{
	_impl.setItemTypeAt(index, type);
}

GuiMenuItemType
MyGuiMenu::getItemTypeAt(std::size_t index) noexcept
{
	return _impl.getItemTypeAt(index);
}

void
MyGuiMenu::setPopupAccept(bool value) noexcept
{
	_impl.setPopupAccept(value);
}

bool
MyGuiMenu::getPopupAccept() const noexcept
{
	return _impl.getPopupAccept();
}

void
MyGuiMenu::setVerticalAlignment(bool value) noexcept
{
	_impl.setVerticalAlignment(value);
}

bool
MyGuiMenu::getVerticalAlignment() const noexcept
{
	return _impl.getVerticalAlignment();
}

void
MyGuiMenu::addMenuCtrlCloseListener(std::function<void()>& func) noexcept
{
	_impl.addMenuCtrlCloseListener(func);
}

void 
MyGuiMenu::addMenuCtrlAcceptListener(std::function<void()>& func) noexcept
{
	_impl.addMenuCtrlAcceptListener(func);
}

void 
MyGuiMenu::removeMenuCtrlCloseListener(std::function<void()>& func) noexcept
{
	_impl.removeMenuCtrlCloseListener(func);
}

void 
MyGuiMenu::removeMenuCtrlAcceptListener(std::function<void()>& func) noexcept
{
	_impl.removeMenuCtrlAcceptListener(func);
}

_NAME_END

#endif