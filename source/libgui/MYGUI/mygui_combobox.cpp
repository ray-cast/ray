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
#include "mygui_combobox.h"
#include "mygui_editbox.h"

_NAME_BEGIN

__ImplementSubClass(MyGuiComboBox, GuiComboBox, "MyGuiComboBox")
__ImplementSubClass(MyGuiComboBoxImpl, MyGuiWidget, "MyGuiComboBoxImpl")

MyGuiComboBoxImpl::MyGuiComboBoxImpl() noexcept
	: _comboBox(nullptr)
	, _parent(nullptr)
{
}

MyGuiComboBoxImpl::~MyGuiComboBoxImpl() noexcept
{
	this->destroy();
}

bool
MyGuiComboBoxImpl::create() except
{
	assert(!_comboBox);
	if (_parent)
		_comboBox = _parent->createWidget<MyGUI::ComboBox>("", 0, 0, 0, 0, MyGUI::Align::Default, "");
	else
		_comboBox = MyGUI::Gui::getInstance().createWidget<MyGUI::ComboBox>("", 0, 0, 0, 0, MyGUI::Align::Default, "Main", "");

	_editBox = std::make_shared<MyGuiEditBox>(_comboBox, false);

	this->setWidget(_comboBox);

	return _comboBox ? true : false;
}

GuiEditBoxPtr
MyGuiComboBoxImpl::getGuiEditBox() const noexcept
{
	return _editBox;
}

std::size_t
MyGuiComboBoxImpl::getItemCount() const noexcept
{
	assert(_comboBox);
	return _comboBox->getItemCount();
}

void
MyGuiComboBoxImpl::insertItemAt(std::size_t index, const std::string& name, std::string data) noexcept
{
	assert(_comboBox);
	_comboBox->insertItemAt(index, name, data);
}

void
MyGuiComboBoxImpl::addItem(const std::string& name, const std::string& data) noexcept
{
	assert(_comboBox);
	_comboBox->addItem(name, data);
}

void
MyGuiComboBoxImpl::removeItemAt(std::size_t index) noexcept
{
	assert(_comboBox);
	_comboBox->removeItemAt(index);
}

void
MyGuiComboBoxImpl::removeAllItems() noexcept
{
	assert(_comboBox);
	_comboBox->removeAllItems();
}

std::size_t
MyGuiComboBoxImpl::findItemIndexWith(const std::string& name) noexcept
{
	assert(_comboBox);
	return _comboBox->findItemIndexWith(name);
}

void
MyGuiComboBoxImpl::setIndexSelected(std::size_t index) noexcept
{
	assert(_comboBox);
	_comboBox->setIndexSelected(index);
}

std::size_t
MyGuiComboBoxImpl::getIndexSelected() const noexcept
{
	assert(_comboBox);
	return _comboBox->getIndexSelected();
}

void
MyGuiComboBoxImpl::clearIndexSelected() noexcept
{
	assert(_comboBox);
	_comboBox->clearIndexSelected();
}

void
MyGuiComboBoxImpl::setItemDataAt(std::size_t index, const std::string& data) noexcept
{
	assert(_comboBox);
	_comboBox->setItemDataAt(index, data);
}

void
MyGuiComboBoxImpl::clearItemDataAt(std::size_t index) noexcept
{
	assert(_comboBox);
	_comboBox->clearItemDataAt(index);
}

std::string*
MyGuiComboBoxImpl::getItemDataAt(std::size_t index, bool _throw) const noexcept
{
	assert(_comboBox);
	return _comboBox->getItemDataAt<std::string>(index, _throw);
}

void
MyGuiComboBoxImpl::setItemNameAt(std::size_t index, const std::string& name) noexcept
{
	assert(_comboBox);
	_comboBox->setItemNameAt(index, name);
}

std::string
MyGuiComboBoxImpl::getItemNameAt(std::size_t index) const noexcept
{
	assert(_comboBox);
	return _comboBox->getItemNameAt(index);
}

void
MyGuiComboBoxImpl::beginToItemAt(std::size_t index) noexcept
{
	assert(_comboBox);
	_comboBox->beginToItemAt(index);
}

void
MyGuiComboBoxImpl::beginToItemFirst() noexcept
{
	assert(_comboBox);
	_comboBox->beginToItemFirst();
}

void
MyGuiComboBoxImpl::beginToItemLast() noexcept
{
	assert(_comboBox);
	_comboBox->beginToItemLast();
}

void
MyGuiComboBoxImpl::beginToItemSelected() noexcept
{
	assert(_comboBox);
	_comboBox->beginToItemSelected();
}

void
MyGuiComboBoxImpl::setComboModeDrop(bool value) noexcept
{
	assert(_comboBox);
	_comboBox->setComboModeDrop(value);
}

bool
MyGuiComboBoxImpl::getComboModeDrop() const noexcept
{
	assert(_comboBox);
	return _comboBox->getComboModeDrop();
}

void
MyGuiComboBoxImpl::setSmoothShow(bool value) noexcept
{
	assert(_comboBox);
	_comboBox->setSmoothShow(value);
}

bool
MyGuiComboBoxImpl::getSmoothShow() const noexcept
{
	assert(_comboBox);
	return _comboBox->getSmoothShow();
}

void
MyGuiComboBoxImpl::setMaxListLength(int value) noexcept
{
	assert(_comboBox);
	_comboBox->setMaxListLength(value);
}

int
MyGuiComboBoxImpl::getMaxListLength() const noexcept
{
	assert(_comboBox);
	return _comboBox->getMaxListLength();
}

void
MyGuiComboBoxImpl::setFlowDirection(GuiFlowDirection value) noexcept
{
	assert(_comboBox);
	_comboBox->setFlowDirection(convert(value));
}

GuiFlowDirection
MyGuiComboBoxImpl::getFlowDirection() const noexcept
{
	assert(_comboBox);
	return convert(_comboBox->getFlowDirection());
}

void
MyGuiComboBoxImpl::addComboAcceptListener(std::function<void()>& func) noexcept
{
	assert(_comboBox);
	assert(!_onComboAccept.find(func));
	if (_onComboAccept.empty())
		_comboBox->eventComboAccept += MyGUI::newDelegate(this, &MyGuiComboBoxImpl::onComboAccept);
	_onComboAccept.attach(func);
}

void
MyGuiComboBoxImpl::addComboChangePositionListener(std::function<void()>& func) noexcept
{
	assert(_comboBox);
	assert(!_onComboChangePosition.find(func));
	if (_onComboChangePosition.empty())
		_comboBox->eventComboChangePosition += MyGUI::newDelegate(this, &MyGuiComboBoxImpl::onComboChangePosition);
	_onComboChangePosition.attach(func);
}

void
MyGuiComboBoxImpl::removeComboAcceptListener(std::function<void()>& func) noexcept
{
	assert(_onComboAccept.find(func));
	_onComboAccept.remove(func);
}

void
MyGuiComboBoxImpl::removeComboChangePositionListener(std::function<void()>& func) noexcept
{
	assert(_onComboChangePosition.find(func));
	_onComboChangePosition.remove(func);
}

void
MyGuiComboBoxImpl::onComboAccept(MyGUI::Widget*) noexcept
{
	_onComboAccept.run();
}

void
MyGuiComboBoxImpl::onComboChangePosition(MyGUI::Widget*, std::size_t) noexcept
{
	_onComboChangePosition.run();
}

MyGuiComboBox::MyGuiComboBox() noexcept
	: GuiComboBox(_impl)
{
}

MyGuiComboBox::~MyGuiComboBox() noexcept
{
}

GuiEditBoxPtr
MyGuiComboBox::getGuiEditBox() const noexcept
{
	return _impl.getGuiEditBox();
}

std::size_t
MyGuiComboBox::getItemCount() const noexcept
{
	return _impl.getItemCount();
}

void
MyGuiComboBox::insertItemAt(std::size_t index, const std::string& name, std::string data) noexcept
{
	_impl.insertItemAt(index, name, data);
}

void
MyGuiComboBox::addItem(const std::string& name, const std::string& data) noexcept
{
	_impl.addItem(name, data);
}

void
MyGuiComboBox::removeItemAt(std::size_t index) noexcept
{
	_impl.removeItemAt(index);
}

void
MyGuiComboBox::removeAllItems() noexcept
{
	_impl.removeAllItems();
}

std::size_t
MyGuiComboBox::findItemIndexWith(const std::string& name) noexcept
{
	return _impl.findItemIndexWith(name);
}

void
MyGuiComboBox::setIndexSelected(std::size_t index) noexcept
{
	_impl.setIndexSelected(index);
}

std::size_t
MyGuiComboBox::getIndexSelected() const noexcept
{
	return _impl.getIndexSelected();
}

void
MyGuiComboBox::clearIndexSelected() noexcept
{
	_impl.clearIndexSelected();
}

void
MyGuiComboBox::setItemDataAt(std::size_t index, const std::string& data) noexcept
{
	_impl.setItemDataAt(index, data);
}

void
MyGuiComboBox::clearItemDataAt(std::size_t index) noexcept
{
	_impl.clearItemDataAt(index);
}

std::string*
MyGuiComboBox::getItemDataAt(std::size_t index, bool _throw) const noexcept
{
	return _impl.getItemDataAt(index, _throw);
}

void
MyGuiComboBox::setItemNameAt(std::size_t index, const std::string& name) noexcept
{
	_impl.setItemNameAt(index, name);
}

std::string
MyGuiComboBox::getItemNameAt(std::size_t index) const noexcept
{
	return _impl.getItemNameAt(index);
}

void
MyGuiComboBox::beginToItemAt(std::size_t index) noexcept
{
	_impl.beginToItemAt(index);
}

void
MyGuiComboBox::beginToItemFirst() noexcept
{
	_impl.beginToItemFirst();
}

void
MyGuiComboBox::beginToItemLast() noexcept
{
	_impl.beginToItemLast();
}

void
MyGuiComboBox::beginToItemSelected() noexcept
{
	_impl.beginToItemSelected();
}

void
MyGuiComboBox::setComboModeDrop(bool value) noexcept
{
	_impl.setComboModeDrop(value);
}

bool
MyGuiComboBox::getComboModeDrop() const noexcept
{
	return _impl.getComboModeDrop();
}

void
MyGuiComboBox::setSmoothShow(bool value) noexcept
{
	_impl.setSmoothShow(value);
}

bool
MyGuiComboBox::getSmoothShow() const noexcept
{
	return _impl.getSmoothShow();
}

void
MyGuiComboBox::setMaxListLength(int value) noexcept
{
	_impl.setMaxListLength(value);
}

int
MyGuiComboBox::getMaxListLength() const noexcept
{
	return _impl.getMaxListLength();
}

void
MyGuiComboBox::setFlowDirection(GuiFlowDirection value) noexcept
{
	_impl.setFlowDirection(value);
}

GuiFlowDirection
MyGuiComboBox::getFlowDirection() const noexcept
{
	return _impl.getFlowDirection();
}

void
MyGuiComboBox::addComboAcceptListener(std::function<void()>& func) noexcept
{
	_impl.addComboAcceptListener(func);
}

void
MyGuiComboBox::addComboChangePositionListener(std::function<void()>& func) noexcept
{
	_impl.addComboChangePositionListener(func);
}

void
MyGuiComboBox::removeComboAcceptListener(std::function<void()>& func) noexcept
{
	_impl.removeComboAcceptListener(func);
}

void
MyGuiComboBox::removeComboChangePositionListener(std::function<void()>& func) noexcept
{
	_impl.removeComboChangePositionListener(func);
}

_NAME_END

#endif