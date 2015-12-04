// +----------------------------------------------------------------------
// | Project : ray.
// | All rights reserved.
// +----------------------------------------------------------------------
// | Copyright (c) 2013-2015.
// +----------------------------------------------------------------------
// | * Redistribution and use of _renderThread software in source and binary forms,
// |   with or without modification, are permitted provided that the following
// |   conditions are met:
// |
// | * Redistributions of source code must retain the above
// |   copyright notice, _renderThread list of conditions and the
// |   following disclaimer.
// |
// | * Redistributions in binary form must reproduce the above
// |   copyright notice, _renderThread list of conditions and the
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
#include <ray/gui_menuitem_component.h>
#include <ray/gui_button_component.h>
#include <ray/gui_menuitem.h>
#include <ray/gui_system.h>

_NAME_BEGIN

__ImplementSubClass(GuiMenuItemComponent, GuiWidgetComponent, "GuiMenuItem")

GuiMenuItemComponent::GuiMenuItemComponent() noexcept
{
	_menuItem = GuiSystem::instance()->createWidget<GuiMenuItem>();
	_menuItem->create();

	_button = std::make_shared<GuiButtonComponent>(_menuItem->getGuiButton());
	_buttonObject = std::make_shared<GameObject>();
	_buttonObject->addComponent(_button);

	this->setGuiWidget(_menuItem);
}

GuiMenuItemComponent::~GuiMenuItemComponent() noexcept
{
}

void
GuiMenuItemComponent::setItemName(const std::string& value) noexcept
{
	assert(_menuItem);
	_menuItem->setItemName(value);
}

std::string
GuiMenuItemComponent::getItemName() noexcept
{
	assert(_menuItem);
	return _menuItem->getItemName();
}

void
GuiMenuItemComponent::setItemData(const std::string& value) noexcept
{
	assert(_menuItem);
	_menuItem->setItemData(value);
}

std::string*
GuiMenuItemComponent::getItemData() const noexcept
{
	assert(_menuItem);
	return _menuItem->getItemData();
}

void
GuiMenuItemComponent::removeItem() noexcept
{
	assert(_menuItem);
	_menuItem->removeItem();
}

void
GuiMenuItemComponent::setItemId(const std::string& value) noexcept
{
	assert(_menuItem);
	_menuItem->setItemId(value);
}

const std::string&
GuiMenuItemComponent::getItemId() noexcept
{
	assert(_menuItem);
	return _menuItem->getItemId();
}

std::size_t
GuiMenuItemComponent::getItemIndex() noexcept
{
	assert(_menuItem);
	return _menuItem->getItemIndex();
}

void
GuiMenuItemComponent::setItemType(GuiMenuItemType value) noexcept
{
	assert(_menuItem);
	_menuItem->setItemType(value);
}

GuiMenuItemType
GuiMenuItemComponent::getItemType() noexcept
{
	assert(_menuItem);
	return _menuItem->getItemType();
}

void
GuiMenuItemComponent::setItemChildVisible(bool value) noexcept
{
	assert(_menuItem);
	_menuItem->setItemChildVisible(value);
}

bool
GuiMenuItemComponent::getItemChecked() const noexcept
{
	assert(_menuItem);
	return _menuItem->getItemChecked();
}

void
GuiMenuItemComponent::setItemChecked(bool value) noexcept
{
	assert(_menuItem);
	_menuItem->setItemChecked(value);
}

void
GuiMenuItemComponent::onAttach() except
{
	assert(_buttonObject);
	_buttonObject->setParent(this->getGameObject());
}

void
GuiMenuItemComponent::onDetach() except
{
	assert(_buttonObject);
	_buttonObject->setParent(nullptr);
}

void
GuiMenuItemComponent::load(iarchive& reader) noexcept
{
	assert(_button);
	_button->load(reader);
}

void
GuiMenuItemComponent::save(oarchive& write) noexcept
{
	assert(_button);
	_button->save(write);
}

GameComponentPtr
GuiMenuItemComponent::clone() const except
{
	return std::make_shared<GuiMenuItemComponent>();
}

_NAME_END

#endif