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
#include <ray/mygui_listbox.h>

_NAME_BEGIN

__ImplementSubClass(MyGuiListBox, GuiListBox, "MyGuiListBox")
__ImplementSubClass(MyGuiListBoxImpl, MyGuiWidget, "MyGuiListBoxImpl")

MyGuiListBoxImpl::MyGuiListBoxImpl() noexcept
	: _listBox(nullptr)
	, _parent(nullptr)
{
}

MyGuiListBoxImpl::~MyGuiListBoxImpl() noexcept
{
	this->destroy();
}

bool
MyGuiListBoxImpl::create() except
{
	assert(!_listBox);
	if (_parent)
		_listBox = _parent->createWidget<MyGUI::ListBox>("", 0, 0, 0, 0, MyGUI::Align::Default, "");
	else
		_listBox = MyGUI::Gui::getInstance().createWidget<MyGUI::ListBox>("", 0, 0, 0, 0, MyGUI::Align::Default, "Main", "");

	this->setWidget(_listBox);

	return _listBox ? true : false;
}

std::size_t
MyGuiListBoxImpl::getItemCount() const
{
	return _listBox->getItemCount();
}

void
MyGuiListBoxImpl::insertItemAt(std::size_t index, const std::string& name)
{
	_listBox->insertItemAt(index, name);
}

void
MyGuiListBoxImpl::addItem(const std::string& name)
{
	_listBox->addItem(name);
}

void
MyGuiListBoxImpl::removeItemAt(std::size_t index)
{
	_listBox->removeItemAt(index);
}

void
MyGuiListBoxImpl::removeAllItems()
{
	_listBox->removeAllItems();
}

void
MyGuiListBoxImpl::swapItemsAt(std::size_t index1, std::size_t index2)
{
	_listBox->swapItemsAt(index1, index2);
}

std::size_t
MyGuiListBoxImpl::findItemIndexWith(const std::string& name)
{
	return _listBox->findItemIndexWith(name);
}

std::size_t
MyGuiListBoxImpl::getIndexSelected() const
{
	return _listBox->getIndexSelected();
}

void
MyGuiListBoxImpl::setIndexSelected(std::size_t index)
{
	_listBox->setIndexSelected(index);
}

void
MyGuiListBoxImpl::clearIndexSelected()
{
	_listBox->clearIndexSelected();
}

void
MyGuiListBoxImpl::setItemNameAt(std::size_t index, const std::string& name)
{
	_listBox->setItemNameAt(index, name);
}

void
MyGuiListBoxImpl::clearItemDataAt(std::size_t index)
{
	_listBox->clearItemDataAt(index);
}

const std::string&
MyGuiListBoxImpl::getItemNameAt(std::size_t index)
{
	return _listBox->getItemNameAt(index);
}

void
MyGuiListBoxImpl::beginToItemAt(std::size_t index)
{
	_listBox->beginToItemAt(index);
}

void
MyGuiListBoxImpl::beginToItemFirst()
{
	_listBox->beginToItemFirst();
}

void
MyGuiListBoxImpl::beginToItemLast()
{
	_listBox->beginToItemLast();
}

void
MyGuiListBoxImpl::beginToItemSelected()
{
	_listBox->beginToItemSelected();
}

bool
MyGuiListBoxImpl::isItemVisibleAt(std::size_t index, bool fill)
{
	return _listBox->isItemVisibleAt(index, fill);
}

bool
MyGuiListBoxImpl::isItemSelectedVisible(bool fill)
{
	return _listBox->isItemSelectedVisible(fill);
}

void
MyGuiListBoxImpl::setScrollVisible(bool visible)
{
	_listBox->setScrollVisible(visible);
}

void
MyGuiListBoxImpl::setScrollPosition(std::size_t position)
{
	_listBox->setScrollPosition(position);
}

int
MyGuiListBoxImpl::getOptimalHeight()
{
	return _listBox->getOptimalHeight();
}

void
MyGuiListBoxImpl::setActivateOnClick(bool activateOnClick)
{
	_listBox->setActivateOnClick(activateOnClick);
}

MyGuiListBox::MyGuiListBox() noexcept
	: GuiListBox(_impl)
{
}

MyGuiListBox::~MyGuiListBox() noexcept
{
}

std::size_t
MyGuiListBox::getItemCount() const
{
	return _impl.getItemCount();
}

void
MyGuiListBox::insertItemAt(std::size_t index, const std::string& name)
{
	_impl.insertItemAt(index, name);
}

void
MyGuiListBox::addItem(const std::string& name)
{
	_impl.addItem(name);
}

void
MyGuiListBox::removeItemAt(std::size_t index)
{
	_impl.removeItemAt(index);
}

void
MyGuiListBox::removeAllItems()
{
	_impl.removeAllItems();
}

void
MyGuiListBox::swapItemsAt(std::size_t index1, std::size_t index2)
{
	_impl.swapItemsAt(index1, index2);
}

std::size_t
MyGuiListBox::findItemIndexWith(const std::string& name)
{
	return _impl.findItemIndexWith(name);
}

std::size_t
MyGuiListBox::getIndexSelected() const
{
	return _impl.getIndexSelected();
}

void
MyGuiListBox::setIndexSelected(std::size_t index)
{
	_impl.setIndexSelected(index);
}

void
MyGuiListBox::clearIndexSelected()
{
	_impl.clearIndexSelected();
}

void
MyGuiListBox::setItemNameAt(std::size_t index, const std::string& name)
{
	_impl.setItemNameAt(index, name);
}

void
MyGuiListBox::clearItemDataAt(std::size_t index)
{
	_impl.clearItemDataAt(index);
}

const std::string&
MyGuiListBox::getItemNameAt(std::size_t index)
{
	return _impl.getItemNameAt(index);
}

void
MyGuiListBox::beginToItemAt(std::size_t index)
{
	_impl.beginToItemAt(index);
}

void
MyGuiListBox::beginToItemFirst()
{
	_impl.beginToItemFirst();
}

void
MyGuiListBox::beginToItemLast()
{
	_impl.beginToItemLast();
}

void
MyGuiListBox::beginToItemSelected()
{
	_impl.beginToItemSelected();
}

bool
MyGuiListBox::isItemVisibleAt(std::size_t index, bool fill)
{
	return _impl.isItemVisibleAt(index, fill);
}

bool
MyGuiListBox::isItemSelectedVisible(bool fill)
{
	return _impl.isItemSelectedVisible(fill);
}

void
MyGuiListBox::setScrollVisible(bool visible)
{
	_impl.setScrollVisible(visible);
}

void
MyGuiListBox::setScrollPosition(std::size_t position)
{
	_impl.setScrollPosition(position);
}

int
MyGuiListBox::getOptimalHeight()
{
	return _impl.getOptimalHeight();
}

void
MyGuiListBox::setActivateOnClick(bool activateOnClick)
{
	_impl.setActivateOnClick(activateOnClick);
}

_NAME_END
#endif