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
#include "mygui_menuitem.h"
#include "mygui_button.h"

_NAME_BEGIN

__ImplementSubClass(MyGuiMenuItemImpl, MyGuiWidget, "MyGuiMenuItemImpl")
__ImplementSubClass(MyGuiMenuItem, GuiMenuItem, "MyGuiMenuItem")

MyGuiMenuItemImpl::MyGuiMenuItemImpl() noexcept
	: _parent(nullptr)
	, _menuItem(nullptr)
{
}

MyGuiMenuItemImpl::~MyGuiMenuItemImpl() noexcept
{
	this->destroy();
}

bool
MyGuiMenuItemImpl::create() except
{
	assert(!_menuItem);
	if (_parent)
		_menuItem = _parent->createWidget<MyGUI::MenuItem>("", 0, 0, 0, 0, MyGUI::Align::Default, "");
	else
		_menuItem = MyGUI::Gui::getInstance().createWidget<MyGUI::MenuItem>("", 0, 0, 0, 0, MyGUI::Align::Default, "Main", "");

	_button = std::make_shared<MyGuiButton>(_menuItem, false);

	this->setWidget(_menuItem);

	return _menuItem ? true : false;
}

GuiButtonPtr
MyGuiMenuItemImpl::getGuiButton() const noexcept
{
	return _button;
}

void
MyGuiMenuItemImpl::setItemName(const std::string& value) noexcept
{
	_menuItem->setItemName(value);
}

std::string
MyGuiMenuItemImpl::getItemName() noexcept
{
	return _menuItem->getItemName();
}

void
MyGuiMenuItemImpl::setItemData(const std::string& value) noexcept
{
	_menuItem->setItemData(value);
}

std::string*
MyGuiMenuItemImpl::getItemData() const noexcept
{
	return _menuItem->getItemData<std::string>();
}

void
MyGuiMenuItemImpl::removeItem() noexcept
{
	_menuItem->removeItem();
}

void
MyGuiMenuItemImpl::setItemId(const std::string& value) noexcept
{
	_menuItem->setItemId(value);
}

const std::string&
MyGuiMenuItemImpl::getItemId() noexcept
{
	return _menuItem->getItemId();
}

std::size_t
MyGuiMenuItemImpl::getItemIndex() noexcept
{
	return _menuItem->getItemIndex();
}

void
MyGuiMenuItemImpl::setItemType(GuiMenuItemType value) noexcept
{
	_menuItem->setItemType(convert(value));
}

GuiMenuItemType
MyGuiMenuItemImpl::getItemType() noexcept
{
	return convert(_menuItem->getItemType());
}

void
MyGuiMenuItemImpl::setItemChildVisible(bool value) noexcept
{
	_menuItem->setItemChildVisible(value);
}

bool
MyGuiMenuItemImpl::getItemChecked() const noexcept
{
	return _menuItem->getItemChecked();
}

void
MyGuiMenuItemImpl::setItemChecked(bool value) noexcept
{
	_menuItem->setItemChecked(value);
}

MyGuiMenuItem::MyGuiMenuItem() noexcept
	: GuiMenuItem(_impl)
{
}

MyGuiMenuItem::~MyGuiMenuItem() noexcept
{
}

GuiButtonPtr
MyGuiMenuItem::getGuiButton() const noexcept
{
	return _impl.getGuiButton();
}

void
MyGuiMenuItem::setItemName(const std::string& value) noexcept
{
	_impl.setItemName(value);
}

std::string
MyGuiMenuItem::getItemName() noexcept
{
	return _impl.getItemName();
}

void
MyGuiMenuItem::setItemData(const std::string& value) noexcept
{
	_impl.setItemData(value);
}

std::string*
MyGuiMenuItem::getItemData() const noexcept
{
	return _impl.getItemData();
}

void
MyGuiMenuItem::removeItem() noexcept
{
	_impl.removeItem();
}

void
MyGuiMenuItem::setItemId(const std::string& value) noexcept
{
	_impl.setItemId(value);
}

const std::string&
MyGuiMenuItem::getItemId() noexcept
{
	return _impl.getItemId();
}

std::size_t
MyGuiMenuItem::getItemIndex() noexcept
{
	return _impl.getItemIndex();
}

void
MyGuiMenuItem::setItemType(GuiMenuItemType value) noexcept
{
	_impl.setItemType(value);
}

GuiMenuItemType
MyGuiMenuItem::getItemType() noexcept
{
	return _impl.getItemType();
}

void
MyGuiMenuItem::setItemChildVisible(bool value) noexcept
{
	_impl.setItemChildVisible(value);
}

bool
MyGuiMenuItem::getItemChecked() const noexcept
{
	return _impl.getItemChecked();
}

void
MyGuiMenuItem::setItemChecked(bool value) noexcept
{
	_impl.setItemChecked(value);
}

_NAME_END

#endif