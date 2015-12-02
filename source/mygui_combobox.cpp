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
#include <ray/mygui_combobox.h>
#include <ray/mygui_editbox.h>

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
MyGuiComboBoxImpl::getItemCount() const
{
	return _comboBox->getItemCount();
}

void
MyGuiComboBoxImpl::insertItemAt(std::size_t index, const std::string& name, std::string data)
{
	_comboBox->insertItemAt(index, name, data);
}

void
MyGuiComboBoxImpl::addItem(const std::string& name, const std::string& data)
{
	_comboBox->addItem(name, data);
}

void
MyGuiComboBoxImpl::removeItemAt(std::size_t index)
{
	_comboBox->removeItemAt(index);
}

void
MyGuiComboBoxImpl::removeAllItems()
{
	_comboBox->removeAllItems();
}

std::size_t
MyGuiComboBoxImpl::findItemIndexWith(const std::string& name)
{
	return _comboBox->findItemIndexWith(name);
}

void
MyGuiComboBoxImpl::setIndexSelected(std::size_t index)
{
	_comboBox->setIndexSelected(index);
}

std::size_t
MyGuiComboBoxImpl::getIndexSelected() const
{
	return _comboBox->getIndexSelected();
}

void
MyGuiComboBoxImpl::clearIndexSelected()
{
	_comboBox->clearIndexSelected();
}

void
MyGuiComboBoxImpl::setItemDataAt(std::size_t index, const std::string& data)
{
	_comboBox->setItemDataAt(index, data);
}

void
MyGuiComboBoxImpl::clearItemDataAt(std::size_t index)
{
	_comboBox->clearItemDataAt(index);
}

std::string*
MyGuiComboBoxImpl::getItemDataAt(std::size_t index, bool _throw) const
{
	return _comboBox->getItemDataAt<std::string>(index, _throw);
}

void
MyGuiComboBoxImpl::setItemNameAt(std::size_t index, const std::string& name)
{
	_comboBox->setItemNameAt(index, name);
}

std::string
MyGuiComboBoxImpl:: getItemNameAt(std::size_t index) const
{
	return _comboBox->getItemNameAt(index);
}

void
MyGuiComboBoxImpl::beginToItemAt(std::size_t index)
{
	_comboBox->beginToItemAt(index);
}

void
MyGuiComboBoxImpl::beginToItemFirst()
{
	_comboBox->beginToItemFirst();
}

void
MyGuiComboBoxImpl::beginToItemLast()
{
	_comboBox->beginToItemLast();
}

void
MyGuiComboBoxImpl::beginToItemSelected()
{
	_comboBox->beginToItemSelected();
}

void
MyGuiComboBoxImpl::setComboModeDrop(bool value)
{
	_comboBox->setComboModeDrop(value);
}

bool
MyGuiComboBoxImpl::getComboModeDrop() const
{
	return _comboBox->getComboModeDrop();
}

void
MyGuiComboBoxImpl::setSmoothShow(bool value)
{
	_comboBox->setSmoothShow(value);
}

bool
MyGuiComboBoxImpl::getSmoothShow() const
{
	return _comboBox->getSmoothShow();
}

void
MyGuiComboBoxImpl::setMaxListLength(int value)
{
	_comboBox->setMaxListLength(value);
}

int
MyGuiComboBoxImpl::getMaxListLength() const
{
	return _comboBox->getMaxListLength();
}

void
MyGuiComboBoxImpl::setFlowDirection(GuiFlowDirection value)
{
	_comboBox->setFlowDirection(convert(value));
}

GuiFlowDirection
MyGuiComboBoxImpl::getFlowDirection() const
{
	return convert(_comboBox->getFlowDirection());
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
MyGuiComboBox::getItemCount() const
{
	return _impl.getItemCount();
}

void
MyGuiComboBox::insertItemAt(std::size_t index, const std::string& name, std::string data)
{
	_impl.insertItemAt(index, name, data);
}

void
MyGuiComboBox::addItem(const std::string& name, const std::string& data)
{
	_impl.addItem(name, data);
}

void
MyGuiComboBox::removeItemAt(std::size_t index)
{
	_impl.removeItemAt(index);
}

void
MyGuiComboBox::removeAllItems()
{
	_impl.removeAllItems();
}

std::size_t
MyGuiComboBox::findItemIndexWith(const std::string& name)
{
	return _impl.findItemIndexWith(name);
}

void
MyGuiComboBox::setIndexSelected(std::size_t index)
{
	_impl.setIndexSelected(index);
}

std::size_t
MyGuiComboBox::getIndexSelected() const
{
	return _impl.getIndexSelected();
}

void
MyGuiComboBox::clearIndexSelected()
{
	_impl.clearIndexSelected();
}

void
MyGuiComboBox::setItemDataAt(std::size_t index, const std::string& data)
{
	_impl.setItemDataAt(index, data);
}

void
MyGuiComboBox::clearItemDataAt(std::size_t index)
{
	_impl.clearItemDataAt(index);
}

std::string*
MyGuiComboBox::getItemDataAt(std::size_t index, bool _throw) const
{
	return _impl.getItemDataAt(index, _throw);
}

void
MyGuiComboBox::setItemNameAt(std::size_t index, const std::string& name)
{
	_impl.setItemNameAt(index, name);
}

std::string
MyGuiComboBox:: getItemNameAt(std::size_t index) const
{
	return _impl.getItemNameAt(index);
}

void
MyGuiComboBox::beginToItemAt(std::size_t index)
{
	_impl.beginToItemAt(index);
}

void
MyGuiComboBox::beginToItemFirst()
{
	_impl.beginToItemFirst();
}

void
MyGuiComboBox::beginToItemLast()
{
	_impl.beginToItemLast();
}

void
MyGuiComboBox::beginToItemSelected()
{
	_impl.beginToItemSelected();
}

void
MyGuiComboBox::setComboModeDrop(bool value)
{
	_impl.setComboModeDrop(value);
}

bool
MyGuiComboBox::getComboModeDrop() const
{
	return _impl.getComboModeDrop();
}

void
MyGuiComboBox::setSmoothShow(bool value)
{
	_impl.setSmoothShow(value);
}

bool
MyGuiComboBox::getSmoothShow() const
{
	return _impl.getSmoothShow();
}

void
MyGuiComboBox::setMaxListLength(int value)
{
	_impl.setMaxListLength(value);
}

int
MyGuiComboBox::getMaxListLength() const
{
	return _impl.getMaxListLength();
}

void
MyGuiComboBox::setFlowDirection(GuiFlowDirection value)
{
	_impl.setFlowDirection(value);
}

GuiFlowDirection
MyGuiComboBox::getFlowDirection() const
{
	return _impl.getFlowDirection();
}

_NAME_END

#endif