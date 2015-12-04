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
#include <ray/gui_menu_component.h>
#include <ray/gui_menu.h>
#include <ray/gui_system.h>

_NAME_BEGIN

__ImplementSubClass(GuiMenuComponent, GuiWidgetComponent, "GuiMenuItem")

GuiMenuComponent::GuiMenuComponent() noexcept
{
	_menu = GuiSystem::instance()->createWidget<GuiMenu>();
	_menu->create();

	this->setGuiWidget(_menu);
}

GuiMenuComponent::~GuiMenuComponent() noexcept
{
}

void
GuiMenuComponent::setVisibleSmooth(bool value) noexcept
{
    assert(_menu);
    _menu->setVisibleSmooth(value);
}

std::size_t
GuiMenuComponent::getItemCount() const noexcept
{
    assert(_menu);
    return _menu->getItemCount();
}

void
GuiMenuComponent::removeItemAt(std::size_t index) noexcept
{
    assert(_menu);
    _menu->removeItemAt(index);
}

void
GuiMenuComponent::removeAllItems() noexcept
{
    assert(_menu);
    _menu->removeAllItems();
}

void
GuiMenuComponent::setItemDataAt(std::size_t index, const std::string& data) noexcept
{
    assert(_menu);
    _menu->setItemDataAt(index, data);
}

void
GuiMenuComponent::clearItemDataAt(std::size_t index) noexcept
{
    assert(_menu);
    _menu->clearItemDataAt(index);
}

void
GuiMenuComponent::setItemIdAt(std::size_t index, const std::string& id) noexcept
{
    assert(_menu);
    _menu->setItemIdAt(index, id);
}

std::string
GuiMenuComponent::getItemIdAt(std::size_t index) noexcept
{
    assert(_menu);
    return _menu->getItemIdAt(index);
}

void
GuiMenuComponent::setItemNameAt(std::size_t index, const std::string& name) noexcept
{
    assert(_menu);
    _menu->setItemNameAt(index, name);
}

std::string
GuiMenuComponent::getItemNameAt(size_t index) noexcept
{
    assert(_menu);
    return _menu->getItemNameAt(index);
}

std::size_t
GuiMenuComponent::findItemIndexWith(const std::string& name) noexcept
{
    assert(_menu);
    return _menu->findItemIndexWith(name);
}

void
GuiMenuComponent::setItemChildVisibleAt(std::size_t index, bool visible) noexcept
{
    assert(_menu);
    _menu->setItemChildVisibleAt(index, visible);
}

void
GuiMenuComponent::removeItemChildAt(std::size_t index) noexcept
{
    assert(_menu);
    _menu->removeItemChildAt(index);
}

void
GuiMenuComponent::setItemTypeAt(std::size_t index, GuiMenuItemType type) noexcept
{
    assert(_menu);
    _menu->setItemTypeAt(index, type);
}

GuiMenuItemType
GuiMenuComponent::getItemTypeAt(std::size_t index) noexcept
{
    assert(_menu);
    return _menu->getItemTypeAt(index);
}

void
GuiMenuComponent::setPopupAccept(bool value) noexcept
{
    assert(_menu);
    _menu->setPopupAccept(value);
}

bool
GuiMenuComponent::getPopupAccept() const noexcept
{
    assert(_menu);
    return _menu->getPopupAccept();
}

void
GuiMenuComponent::setVerticalAlignment(bool value) noexcept
{
    assert(_menu);
    _menu->setVerticalAlignment(value);
}

bool
GuiMenuComponent::getVerticalAlignment() const noexcept
{
    assert(_menu);
    return _menu->getVerticalAlignment();
}

void
GuiMenuComponent::addMenuCtrlCloseListener(std::function<void()> func) noexcept
{
	assert(_menu);
	return _menu->addMenuCtrlCloseListener(func);
}

void 
GuiMenuComponent::addMenuCtrlAcceptListener(std::function<void()> func) noexcept
{
	assert(_menu);
	return _menu->addMenuCtrlAcceptListener(func);
}

void 
GuiMenuComponent::removeMenuCtrlCloseListener(std::function<void()> func) noexcept
{
	assert(_menu);
	return _menu->removeMenuCtrlCloseListener(func);
}

void 
GuiMenuComponent::removeMenuCtrlAcceptListener(std::function<void()> func) noexcept
{
	assert(_menu);
	return _menu->removeMenuCtrlAcceptListener(func);
}

void
GuiMenuComponent::load(iarchive& reader) noexcept
{
}

void
GuiMenuComponent::save(oarchive& write) noexcept
{
}

GameComponentPtr
GuiMenuComponent::clone() const except
{
	return std::make_shared<GuiMenuComponent>();
}

_NAME_END

#endif