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
#include "mygui_listbox.h"

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
MyGuiListBoxImpl::getItemCount() const noexcept
{
	return _listBox->getItemCount();
}

void
MyGuiListBoxImpl::insertItemAt(std::size_t index, const std::string& name) noexcept
{
	_listBox->insertItemAt(index, name);
}

void
MyGuiListBoxImpl::addItem(const std::string& name) noexcept
{
	_listBox->addItem(name);
}

void
MyGuiListBoxImpl::removeItemAt(std::size_t index) noexcept
{
	_listBox->removeItemAt(index);
}

void
MyGuiListBoxImpl::removeAllItems() noexcept
{
	_listBox->removeAllItems();
}

void
MyGuiListBoxImpl::swapItemsAt(std::size_t index1, std::size_t index2) noexcept
{
	_listBox->swapItemsAt(index1, index2);
}

std::size_t
MyGuiListBoxImpl::findItemIndexWith(const std::string& name) noexcept
{
	return _listBox->findItemIndexWith(name);
}

std::size_t
MyGuiListBoxImpl::getIndexSelected() const noexcept
{
	return _listBox->getIndexSelected();
}

void
MyGuiListBoxImpl::setIndexSelected(std::size_t index) noexcept
{
	_listBox->setIndexSelected(index);
}

void
MyGuiListBoxImpl::clearIndexSelected() noexcept
{
	_listBox->clearIndexSelected();
}

void
MyGuiListBoxImpl::setItemNameAt(std::size_t index, const std::string& name) noexcept
{
	_listBox->setItemNameAt(index, name);
}

void
MyGuiListBoxImpl::clearItemDataAt(std::size_t index) noexcept
{
	_listBox->clearItemDataAt(index);
}

std::string
MyGuiListBoxImpl::getItemNameAt(std::size_t index) const noexcept
{
	return _listBox->getItemNameAt(index);
}

void
MyGuiListBoxImpl::beginToItemAt(std::size_t index) noexcept
{
	_listBox->beginToItemAt(index);
}

void
MyGuiListBoxImpl::beginToItemFirst() noexcept
{
	_listBox->beginToItemFirst();
}

void
MyGuiListBoxImpl::beginToItemLast() noexcept
{
	_listBox->beginToItemLast();
}

void
MyGuiListBoxImpl::beginToItemSelected() noexcept
{
	_listBox->beginToItemSelected();
}

bool
MyGuiListBoxImpl::isItemVisibleAt(std::size_t index, bool fill) noexcept
{
	return _listBox->isItemVisibleAt(index, fill);
}

bool
MyGuiListBoxImpl::isItemSelectedVisible(bool fill) noexcept
{
	return _listBox->isItemSelectedVisible(fill);
}

void
MyGuiListBoxImpl::setScrollVisible(bool visible) noexcept
{
	_listBox->setScrollVisible(visible);
}

void
MyGuiListBoxImpl::setScrollPosition(std::size_t position) noexcept
{
	_listBox->setScrollPosition(position);
}

int
MyGuiListBoxImpl::getOptimalHeight() noexcept
{
	return _listBox->getOptimalHeight();
}

void
MyGuiListBoxImpl::setActivateOnClick(bool activateOnClick) noexcept
{
	_listBox->setActivateOnClick(activateOnClick);
}

void
MyGuiListBoxImpl::addListSelectAcceptListener(std::function<void()>& func) noexcept
{
	assert(_listBox);
	assert(_onListSelectAccept.find(func));
	if (_onListSelectAccept.empty())
		_listBox->eventListSelectAccept += MyGUI::newDelegate(this, &MyGuiListBoxImpl::onListSelectAccept);
	_onListSelectAccept.attach(func);
}

void
MyGuiListBoxImpl::addListChangePositionListener(std::function<void()>& func) noexcept
{
	assert(_listBox);
	assert(_onListChangePosition.find(func));
	if (_onListChangePosition.empty())
		_listBox->eventListChangePosition += MyGUI::newDelegate(this, &MyGuiListBoxImpl::onListChangePosition);
	_onListChangePosition.attach(func);
}

void
MyGuiListBoxImpl::addListMouseItemActivateListener(std::function<void()>& func) noexcept
{
	assert(_listBox);
	assert(_onListMouseItemActivate.find(func));
	if (_onListMouseItemActivate.empty())
		_listBox->eventListMouseItemActivate += MyGUI::newDelegate(this, &MyGuiListBoxImpl::onListMouseItemActivate);
	_onListMouseItemActivate.attach(func);
}

void
MyGuiListBoxImpl::addListMouseItemFocusListener(std::function<void()>& func) noexcept
{
	assert(_listBox);
	assert(_onListMouseItemFocus.find(func));
	if (_onListMouseItemFocus.empty())
		_listBox->eventListMouseItemFocus += MyGUI::newDelegate(this, &MyGuiListBoxImpl::onListMouseItemFocus);
	_onListMouseItemFocus.attach(func);
}

void
MyGuiListBoxImpl::addListChangeScrollListener(std::function<void()>& func) noexcept
{
	assert(_listBox);
	assert(_onListChangeScroll.find(func));
	_onListChangeScroll.attach(func);
}

void
MyGuiListBoxImpl::removeListSelectAcceptListener(std::function<void()>& func) noexcept
{
	assert(_listBox);
	assert(_onListSelectAccept.find(func));
	_onListSelectAccept.remove(func);
}

void
MyGuiListBoxImpl::removeListChangePositionListener(std::function<void()>& func) noexcept
{
	assert(_listBox);
	assert(_onListChangePosition.find(func));
	_onListChangePosition.remove(func);
}

void
MyGuiListBoxImpl::removeListMouseItemActivateListener(std::function<void()>& func) noexcept
{
	assert(_listBox);
	assert(_onListMouseItemActivate.find(func));
	_onListMouseItemActivate.remove(func);
}

void
MyGuiListBoxImpl::removeListMouseItemFocusListener(std::function<void()>& func) noexcept
{
	assert(_listBox);
	assert(_onListMouseItemFocus.find(func));
	_onListMouseItemFocus.remove(func);
}

void
MyGuiListBoxImpl::removeListChangeScrollListener(std::function<void()>& func) noexcept
{
	assert(_listBox);
	assert(_onListChangeScroll.find(func));
	_onListChangeScroll.remove(func);
}

void
MyGuiListBoxImpl::onListSelectAccept(MyGUI::Widget*, std::size_t) except
{
	_onListSelectAccept.run();
}

void
MyGuiListBoxImpl::onListChangePosition(MyGUI::Widget*, std::size_t) except
{
	_onListChangePosition.run();
}

void
MyGuiListBoxImpl::onListMouseItemActivate(MyGUI::Widget*, std::size_t) except
{
	_onListMouseItemActivate.run();
}

void
MyGuiListBoxImpl::onListMouseItemFocus(MyGUI::Widget*, std::size_t) except
{
	_onListMouseItemFocus.run();
}

void
MyGuiListBoxImpl::onListChangeScroll(MyGUI::Widget*, std::size_t) except
{
	_onListChangeScroll.run();
}

MyGuiListBox::MyGuiListBox() noexcept
	: GuiListBox(_impl)
{
}

MyGuiListBox::~MyGuiListBox() noexcept
{
}

std::size_t
MyGuiListBox::getItemCount() const noexcept
{
	return _impl.getItemCount();
}

void
MyGuiListBox::insertItemAt(std::size_t index, const std::string& name) noexcept
{
	_impl.insertItemAt(index, name);
}

void
MyGuiListBox::addItem(const std::string& name) noexcept
{
	_impl.addItem(name);
}

void
MyGuiListBox::removeItemAt(std::size_t index) noexcept
{
	_impl.removeItemAt(index);
}

void
MyGuiListBox::removeAllItems() noexcept
{
	_impl.removeAllItems();
}

void
MyGuiListBox::swapItemsAt(std::size_t index1, std::size_t index2) noexcept
{
	_impl.swapItemsAt(index1, index2);
}

std::size_t
MyGuiListBox::findItemIndexWith(const std::string& name) noexcept
{
	return _impl.findItemIndexWith(name);
}

std::size_t
MyGuiListBox::getIndexSelected() const noexcept
{
	return _impl.getIndexSelected();
}

void
MyGuiListBox::setIndexSelected(std::size_t index) noexcept
{
	_impl.setIndexSelected(index);
}

void
MyGuiListBox::clearIndexSelected() noexcept
{
	_impl.clearIndexSelected();
}

void
MyGuiListBox::setItemNameAt(std::size_t index, const std::string& name) noexcept
{
	_impl.setItemNameAt(index, name);
}

void
MyGuiListBox::clearItemDataAt(std::size_t index) noexcept
{
	_impl.clearItemDataAt(index);
}

std::string
MyGuiListBox::getItemNameAt(std::size_t index) const noexcept
{
	return _impl.getItemNameAt(index);
}

void
MyGuiListBox::beginToItemAt(std::size_t index) noexcept
{
	_impl.beginToItemAt(index);
}

void
MyGuiListBox::beginToItemFirst() noexcept
{
	_impl.beginToItemFirst();
}

void
MyGuiListBox::beginToItemLast() noexcept
{
	_impl.beginToItemLast();
}

void
MyGuiListBox::beginToItemSelected() noexcept
{
	_impl.beginToItemSelected();
}

bool
MyGuiListBox::isItemVisibleAt(std::size_t index, bool fill) noexcept
{
	return _impl.isItemVisibleAt(index, fill);
}

bool
MyGuiListBox::isItemSelectedVisible(bool fill) noexcept
{
	return _impl.isItemSelectedVisible(fill);
}

void
MyGuiListBox::setScrollVisible(bool visible) noexcept
{
	_impl.setScrollVisible(visible);
}

void
MyGuiListBox::setScrollPosition(std::size_t position) noexcept
{
	_impl.setScrollPosition(position);
}

int
MyGuiListBox::getOptimalHeight() noexcept
{
	return _impl.getOptimalHeight();
}

void
MyGuiListBox::setActivateOnClick(bool activateOnClick) noexcept
{
	_impl.setActivateOnClick(activateOnClick);
}

void
MyGuiListBox::addListSelectAcceptListener(std::function<void()>& func) noexcept
{
	_impl.addListSelectAcceptListener(func);
}

void
MyGuiListBox::addListChangePositionListener(std::function<void()>& func) noexcept
{
	_impl.addListChangePositionListener(func);
}

void
MyGuiListBox::addListMouseItemActivateListener(std::function<void()>& func) noexcept
{
	_impl.addListMouseItemActivateListener(func);
}

void
MyGuiListBox::addListMouseItemFocusListener(std::function<void()>& func) noexcept
{
	_impl.addListMouseItemFocusListener(func);
}

void
MyGuiListBox::addListChangeScrollListener(std::function<void()>& func) noexcept
{
	_impl.addListChangeScrollListener(func);
}

void
MyGuiListBox::removeListSelectAcceptListener(std::function<void()>& func) noexcept
{
	_impl.removeListSelectAcceptListener(func);
}

void
MyGuiListBox::removeListChangePositionListener(std::function<void()>& func) noexcept
{
	_impl.removeListChangePositionListener(func);
}

void
MyGuiListBox::removeListMouseItemActivateListener(std::function<void()>& func) noexcept
{
	_impl.removeListMouseItemActivateListener(func);
}

void
MyGuiListBox::removeListMouseItemFocusListener(std::function<void()>& func) noexcept
{
	_impl.removeListMouseItemFocusListener(func);
}

void
MyGuiListBox::removeListChangeScrollListener(std::function<void()>& func) noexcept
{
	_impl.removeListChangeScrollListener(func);
}

_NAME_END

#endif