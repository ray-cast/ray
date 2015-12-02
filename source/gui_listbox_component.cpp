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
#if defined(_BUILD_GUI)
#include <ray/gui_listbox_component.h>
#include <ray/gui_listbox.h>
#include <ray/gui_system.h>

_NAME_BEGIN

__ImplementSubInterface(GuiListBoxComponent, GameComponent, "GuiListBox")

GuiListBoxComponent::GuiListBoxComponent() noexcept
{
	_listBox = GuiSystem::instance()->createWidget<GuiListBox>();
	_listBox->create();

	this->setGuiWidget(_listBox);
}

GuiListBoxComponent::~GuiListBoxComponent() noexcept
{
}

std::size_t
GuiListBoxComponent::getItemCount() const
{
    return _listBox->getItemCount();
}

void
GuiListBoxComponent::insertItemAt(std::size_t index, const std::string& name)
{
    _listBox->insertItemAt(index, name);
}

void
GuiListBoxComponent::addItem(const std::string& name)
{
    _listBox->addItem(name);
}

void
GuiListBoxComponent::removeItemAt(std::size_t index)
{
    _listBox->removeItemAt(index);
}

void
GuiListBoxComponent::removeAllItems()
{
    _listBox->removeAllItems();
}

void
GuiListBoxComponent::swapItemsAt(std::size_t index1, std::size_t index2)
{
    _listBox->swapItemsAt(index1, index2);
}

std::size_t
GuiListBoxComponent::findItemIndexWith(const std::string& name)
{
    return _listBox->findItemIndexWith(name);
}

std::size_t
GuiListBoxComponent::getIndexSelected() const
{
    return _listBox->getIndexSelected();
}

void
GuiListBoxComponent::setIndexSelected(std::size_t index)
{
    _listBox->setIndexSelected(index);
}

void
GuiListBoxComponent::clearIndexSelected()
{
    _listBox->clearIndexSelected();
}

void
GuiListBoxComponent::setItemNameAt(std::size_t index, const std::string& name)
{
    _listBox->setItemNameAt(index, name);
}

void
GuiListBoxComponent::clearItemDataAt(std::size_t index)
{
    _listBox->clearItemDataAt(index);
}

std::string
GuiListBoxComponent::getItemNameAt(std::size_t index) const
{
    return _listBox->getItemNameAt(index);
}

void
GuiListBoxComponent::beginToItemAt(std::size_t index)
{
    _listBox->beginToItemAt(index);
}

void
GuiListBoxComponent::beginToItemFirst()
{
    _listBox->beginToItemFirst();
}

void
GuiListBoxComponent::beginToItemLast()
{
    _listBox->beginToItemLast();
}

void
GuiListBoxComponent::beginToItemSelected()
{
    _listBox->beginToItemSelected();
}

bool
GuiListBoxComponent::isItemVisibleAt(std::size_t index, bool fill)
{
    return _listBox->isItemVisibleAt(index, fill);
}

bool
GuiListBoxComponent::isItemSelectedVisible(bool fill)
{
    return _listBox->isItemSelectedVisible(fill);
}

void
GuiListBoxComponent::setScrollVisible(bool visible)
{
    _listBox->setScrollVisible(visible);
}

void
GuiListBoxComponent::setScrollPosition(std::size_t position)
{
    _listBox->setScrollPosition(position);
}

int
GuiListBoxComponent::getOptimalHeight()
{
    return _listBox->getOptimalHeight();
}

void
GuiListBoxComponent::setActivateOnClick(bool activateOnClick)
{
    _listBox->setActivateOnClick(activateOnClick);
}

void
GuiListBoxComponent::load(iarchive& reader) noexcept
{
	GuiWidgetComponent::load(reader);
}

void 
GuiListBoxComponent::save(oarchive& write) noexcept
{
	GuiWidgetComponent::save(write);
}

GameComponentPtr 
GuiListBoxComponent::clone() const except
{
	return std::make_shared<GuiListBoxComponent>();
}

_NAME_END

#endif