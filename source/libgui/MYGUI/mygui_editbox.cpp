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
#include "mygui_editbox.h"
#include "mygui_textbox.h"
#include "mygui_scrollview.h"

_NAME_BEGIN

__ImplementSubClass(MyGuiEditBox, GuiEditBox, "MyGuiEditBox")
__ImplementSubClass(MyGuiEditBoxImpl, MyGuiWidget, "MyGuiEditBoxImpl")

MyGuiEditBoxImpl::MyGuiEditBoxImpl() noexcept
	: _destroy(true)
	, _parent(nullptr)
	, _editBox(nullptr)
{
}

MyGuiEditBoxImpl::MyGuiEditBoxImpl(MyGUI::EditBox* self, bool destroy) noexcept
	: _destroy(destroy)
	, _parent(nullptr)
	, _editBox(self)
{
}

MyGuiEditBoxImpl::~MyGuiEditBoxImpl() noexcept
{
	this->destroy();
}

bool
MyGuiEditBoxImpl::create() except
{
	assert(!_editBox);
	if (_parent)
		_editBox = _parent->createWidget<MyGUI::EditBox>("", 0, 0, 0, 0, MyGUI::Align::Default, "");
	else
		_editBox = MyGUI::Gui::getInstance().createWidget<MyGUI::EditBox>("", 0, 0, 0, 0, MyGUI::Align::Default, "Main", "");
	
	_textbox = std::make_shared<MyGuiTextBox>(_editBox, false);
	
	this->setWidget(_editBox);

	return _editBox ? true : false;
}

void
MyGuiEditBoxImpl::destroy() noexcept
{
	if (_destroy)
	{
		if (_editBox)
		{
			MyGUI::Gui::getInstance().destroyWidget(_editBox);
			_editBox = nullptr;
		}
	}
}

GuiTextBoxPtr
MyGuiEditBoxImpl::getGuiTextBox() const noexcept
{
	return _textbox;
}

void
MyGuiEditBoxImpl::setTextIntervalColour(std::size_t start, std::size_t count, const float4& colour)
{
	_editBox->setTextIntervalColour(start, count, convert(colour));
}

std::size_t
MyGuiEditBoxImpl::getTextSelectionStart() const
{
	return _editBox->getTextSelectionStart();
}

std::size_t
MyGuiEditBoxImpl::getTextSelectionEnd() const
{
	return _editBox->getTextSelectionEnd();
}

std::size_t
MyGuiEditBoxImpl::getTextSelectionLength() const
{
	return _editBox->getTextSelectionLength();
}

std::string
MyGuiEditBoxImpl::getTextInterval(std::size_t start, std::size_t count)
{
	return _editBox->getTextInterval(start, count);
}

void
MyGuiEditBoxImpl::setTextSelection(std::size_t start, std::size_t end)
{
	return _editBox->setTextSelection(start, end);
}

void
MyGuiEditBoxImpl::deleteTextSelection()
{
	_editBox->deleteTextSelection();
}

std::string
MyGuiEditBoxImpl::getTextSelection()
{
	return _editBox->getTextSelection();
}

bool
MyGuiEditBoxImpl::isTextSelection() const
{
	return _editBox->isTextSelection();
}

void
MyGuiEditBoxImpl::setTextSelectionColour(const float4& value)
{
	_editBox->setTextSelectionColour(convert(value));
}

void
MyGuiEditBoxImpl::setTextCursor(std::size_t index)
{
	_editBox->setTextCursor(index);
}

std::size_t
MyGuiEditBoxImpl::getTextCursor() const
{
	return _editBox->getTextCursor();
}

void
MyGuiEditBoxImpl::setOnlyText(const std::string& value)
{
	_editBox->setOnlyText(value);
}

std::string
MyGuiEditBoxImpl::getOnlyText()
{
	return _editBox->getOnlyText();
}


std::size_t
MyGuiEditBoxImpl::getTextLength() const
{
	return _editBox->getTextLength();
}

void
MyGuiEditBoxImpl::setOverflowToTheLeft(bool value)
{
	_editBox->setOverflowToTheLeft(value);
}

bool
MyGuiEditBoxImpl::getOverflowToTheLeft() const
{
	return _editBox->getOverflowToTheLeft();
}

void
MyGuiEditBoxImpl::setMaxTextLength(std::size_t value)
{
	_editBox->setMaxTextLength(value);
}

std::size_t
MyGuiEditBoxImpl::getMaxTextLength() const
{
	return _editBox->getMaxTextLength();
}

void
MyGuiEditBoxImpl::insertText(const std::string& text, std::size_t index)
{
	_editBox->insertText(text, index);
}

void
MyGuiEditBoxImpl::addText(const std::string& text)
{
	_editBox->addText(text);
}

void
MyGuiEditBoxImpl::eraseText(std::size_t start, std::size_t count)
{
	_editBox->eraseText(start, count);
}

void
MyGuiEditBoxImpl::setEditReadOnly(bool value)
{
	_editBox->setEditReadOnly(value);
}

bool
MyGuiEditBoxImpl::getEditReadOnly() const
{
	return _editBox->getEditReadOnly();
}

void
MyGuiEditBoxImpl::setEditPassword(bool value)
{
	_editBox->setEditPassword(value);
}

bool
MyGuiEditBoxImpl::getEditPassword() const
{
	return _editBox->getEditPassword();
}

void
MyGuiEditBoxImpl::setEditMultiLine(bool value)
{
	_editBox->setEditMultiLine(value);
}

bool
MyGuiEditBoxImpl::getEditMultiLine() const
{
	return _editBox->getEditMultiLine();
}

void
MyGuiEditBoxImpl::setEditStatic(bool value)
{
	_editBox->setEditStatic(value);
}

bool
MyGuiEditBoxImpl::getEditStatic() const
{
	return _editBox->getEditStatic();
}

void
MyGuiEditBoxImpl::setPasswordChar(GuiInputChar value)
{
	_editBox->setPasswordChar(value);
}

void
MyGuiEditBoxImpl::setPasswordChar(const std::string& value)
{
	_editBox->setPasswordChar(value);
}

GuiInputChar
MyGuiEditBoxImpl::getPasswordChar() const
{
	return _editBox->getPasswordChar();
}

void
MyGuiEditBoxImpl::setEditWordWrap(bool value)
{
	_editBox->setEditWordWrap(value);
}

bool
MyGuiEditBoxImpl::getEditWordWrap() const
{
	return _editBox->getEditWordWrap();
}

void
MyGuiEditBoxImpl::setTabPrinting(bool value)
{
	_editBox->setTabPrinting(value);
}

bool
MyGuiEditBoxImpl::getTabPrinting() const
{
	return _editBox->getTabPrinting();
}

bool
MyGuiEditBoxImpl::getInvertSelected()
{
	return _editBox->getInvertSelected();
}

void
MyGuiEditBoxImpl::setInvertSelected(bool value)
{
	_editBox->setInvertSelected(value);
}

std::size_t
MyGuiEditBoxImpl::getVScrollRange() const
{
	return _editBox->getVScrollRange();
}

std::size_t
MyGuiEditBoxImpl::getVScrollPosition()
{
	return _editBox->getVScrollPosition();
}

void
MyGuiEditBoxImpl::setVScrollPosition(std::size_t index)
{
	_editBox->setVScrollPosition(index);
}

std::size_t
MyGuiEditBoxImpl::getHScrollRange() const
{
	return _editBox->getHScrollRange();
}

std::size_t
MyGuiEditBoxImpl::getHScrollPosition()
{
	return _editBox->getHScrollPosition();
}

void
MyGuiEditBoxImpl::setHScrollPosition(std::size_t index)
{
	_editBox->setHScrollPosition(index);
}

void 
MyGuiEditBoxImpl::addTextChangeListener(std::function<void()>& func) noexcept
{
	assert(!_onTextChange.find(func));
	if (_onTextChange.empty())
		_editBox->eventEditTextChange += MyGUI::newDelegate(this, &MyGuiEditBoxImpl::onTextChange);
	_onTextChange.attach(func);
}

void 
MyGuiEditBoxImpl::removeTextChangeListener(std::function<void()>& func) noexcept
{
	assert(_onTextChange.find(func));
	_onTextChange.remove(func);
}

void 
MyGuiEditBoxImpl::addSelectAccept(std::function<void()>& func) noexcept
{
	assert(!_onSelectAccept.find(func));
	if (_onTextChange.empty())
		_editBox->eventEditSelectAccept += MyGUI::newDelegate(this, &MyGuiEditBoxImpl::onSelectAccept);
	_onSelectAccept.attach(func);
}

void 
MyGuiEditBoxImpl::removeSelectAccept(std::function<void()>& func) noexcept
{
	assert(_onSelectAccept.find(func));
	_onSelectAccept.remove(func);
}

void 
MyGuiEditBoxImpl::onTextChange(MyGUI::Widget*) noexcept
{
	_onTextChange();
}

void 
MyGuiEditBoxImpl::onSelectAccept(MyGUI::Widget*) noexcept
{
	_onSelectAccept();
}

MyGuiEditBox::MyGuiEditBox() noexcept
	: GuiEditBox(_impl)
{
}

MyGuiEditBox::MyGuiEditBox(MyGUI::EditBox* self, bool destroy) noexcept
	: GuiEditBox(_impl)
	, _impl(self, destroy)
{
}

MyGuiEditBox::~MyGuiEditBox() noexcept
{
}

GuiTextBoxPtr
MyGuiEditBox::getGuiTextBox() const noexcept
{
	return _impl.getGuiTextBox();
}

void
MyGuiEditBox::setTextIntervalColour(std::size_t start, std::size_t count, const float4& colour)
{
	_impl.setTextIntervalColour(start, count, colour);
}

std::size_t
MyGuiEditBox::getTextSelectionStart() const
{
	return _impl.getTextSelectionStart();
}

std::size_t
MyGuiEditBox::getTextSelectionEnd() const
{
	return _impl.getTextSelectionEnd();
}

std::size_t
MyGuiEditBox::getTextSelectionLength() const
{
	return _impl.getTextSelectionLength();
}

std::string
MyGuiEditBox::getTextInterval(std::size_t start, std::size_t count)
{
	return _impl.getTextInterval(start, count);
}

void
MyGuiEditBox::setTextSelection(std::size_t start, std::size_t end)
{
	return _impl.setTextSelection(start, end);
}

void
MyGuiEditBox::deleteTextSelection()
{
	_impl.deleteTextSelection();
}

std::string
MyGuiEditBox::getTextSelection()
{
	return _impl.getTextSelection();
}

bool
MyGuiEditBox::isTextSelection() const
{
	return _impl.isTextSelection();
}

void
MyGuiEditBox::setTextSelectionColour(const float4& value)
{
	_impl.setTextSelectionColour(value);
}

void
MyGuiEditBox::setTextCursor(std::size_t index)
{
	_impl.setTextCursor(index);
}

std::size_t
MyGuiEditBox::getTextCursor() const
{
	return _impl.getTextCursor();
}

void
MyGuiEditBox::setOnlyText(const std::string& value)
{
	_impl.setOnlyText(value);
}

std::string
MyGuiEditBox::getOnlyText()
{
	return _impl.getOnlyText();
}

std::size_t
MyGuiEditBox::getTextLength() const
{
	return _impl.getTextLength();
}

void
MyGuiEditBox::setOverflowToTheLeft(bool value)
{
	_impl.setOverflowToTheLeft(value);
}

bool
MyGuiEditBox::getOverflowToTheLeft() const
{
	return _impl.getOverflowToTheLeft();
}

void
MyGuiEditBox::setMaxTextLength(std::size_t value)
{
	_impl.setMaxTextLength(value);
}

std::size_t
MyGuiEditBox::getMaxTextLength() const
{
	return _impl.getMaxTextLength();
}

void
MyGuiEditBox::insertText(const std::string& text, std::size_t index)
{
	_impl.insertText(text, index);
}

void
MyGuiEditBox::addText(const std::string& text)
{
	_impl.addText(text);
}

void
MyGuiEditBox::eraseText(std::size_t start, std::size_t count)
{
	_impl.eraseText(start, count);
}

void
MyGuiEditBox::setEditReadOnly(bool value)
{
	_impl.setEditReadOnly(value);
}

bool
MyGuiEditBox::getEditReadOnly() const
{
	return _impl.getEditReadOnly();
}

void
MyGuiEditBox::setEditPassword(bool value)
{
	_impl.setEditPassword(value);
}

bool
MyGuiEditBox::getEditPassword() const
{
	return _impl.getEditPassword();
}

void
MyGuiEditBox::setEditMultiLine(bool value)
{
	_impl.setEditMultiLine(value);
}

bool
MyGuiEditBox::getEditMultiLine() const
{
	return _impl.getEditMultiLine();
}

void
MyGuiEditBox::setEditStatic(bool value)
{
	_impl.setEditStatic(value);
}

bool
MyGuiEditBox::getEditStatic() const
{
	return _impl.getEditStatic();
}

void
MyGuiEditBox::setPasswordChar(GuiInputChar value)
{
	_impl.setPasswordChar(value);
}

void
MyGuiEditBox::setPasswordChar(const std::string& value)
{
	_impl.setPasswordChar(value);
}

GuiInputChar
MyGuiEditBox::getPasswordChar() const
{
	return _impl.getPasswordChar();
}

void
MyGuiEditBox::setEditWordWrap(bool value)
{
	_impl.setEditWordWrap(value);
}

bool
MyGuiEditBox::getEditWordWrap() const
{
	return _impl.getEditWordWrap();
}

void
MyGuiEditBox::setTabPrinting(bool value)
{
	_impl.setTabPrinting(value);
}

bool
MyGuiEditBox::getTabPrinting() const
{
	return _impl.getTabPrinting();
}

bool
MyGuiEditBox::getInvertSelected()
{
	return _impl.getInvertSelected();
}

void
MyGuiEditBox::setInvertSelected(bool value)
{
	_impl.setInvertSelected(value);
}

std::size_t
MyGuiEditBox::getVScrollRange() const
{
	return _impl.getVScrollRange();
}

std::size_t
MyGuiEditBox::getVScrollPosition()
{
	return _impl.getVScrollPosition();
}

void
MyGuiEditBox::setVScrollPosition(std::size_t index)
{
	_impl.setVScrollPosition(index);
}

std::size_t
MyGuiEditBox::getHScrollRange() const
{
	return _impl.getHScrollRange();
}

std::size_t
MyGuiEditBox::getHScrollPosition()
{
	return _impl.getHScrollPosition();
}

void
MyGuiEditBox::setHScrollPosition(std::size_t index)
{
	_impl.setHScrollPosition(index);
}

void
MyGuiEditBox::addTextChangeListener(std::function<void()>& func) noexcept
{
	_impl.addTextChangeListener(func);
}

void 
MyGuiEditBox::removeTextChangeListener(std::function<void()>& func) noexcept
{
	_impl.removeTextChangeListener(func);
}

void 
MyGuiEditBox::addSelectAccept(std::function<void()>& func) noexcept
{
	_impl.addSelectAccept(func);
}

void 
MyGuiEditBox::removeSelectAccept(std::function<void()>& func) noexcept
{
	_impl.removeSelectAccept(func);
}

_NAME_END

#endif