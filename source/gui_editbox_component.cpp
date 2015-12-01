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
#include <ray/gui_editbox_component.h>
#include <ray/gui_label_component.h>
#include <ray/gui_editbox.h>
#include <ray/gui_system.h>

_NAME_BEGIN

__ImplementSubClass(GuiEditBoxComponent, GuiWidgetComponent, "GuiEditBox")

GuiEditBoxComponent::GuiEditBoxComponent() noexcept
{
	_editBox = GuiSystem::instance()->createWidget<GuiEditBox>();
	_editBox->create();
}

GuiEditBoxComponent::~GuiEditBoxComponent() noexcept
{
}

void
GuiEditBoxComponent::setTextIntervalColour(std::size_t start, std::size_t count, const float4& colour)
{
    _editBox->setTextIntervalColour(start, count, colour);
}

std::size_t
GuiEditBoxComponent::getTextSelectionStart() const
{
    return _editBox->getTextSelectionStart();
}

std::size_t
GuiEditBoxComponent::getTextSelectionEnd() const
{
    return _editBox->getTextSelectionEnd();
}

std::size_t
GuiEditBoxComponent::getTextSelectionLength() const
{
    return _editBox->getTextSelectionLength();
}

std::string
GuiEditBoxComponent::getTextInterval(std::size_t start, std::size_t count)
{
    return _editBox->getTextInterval(start, count);
}

void
GuiEditBoxComponent::setTextSelection(std::size_t start, std::size_t end)
{
    return _editBox->setTextSelection(start, end);
}

void
GuiEditBoxComponent::deleteTextSelection()
{
    _editBox->deleteTextSelection();
}

std::string
GuiEditBoxComponent::getTextSelection()
{
    return _editBox->getTextSelection();
}

bool
GuiEditBoxComponent::isTextSelection() const
{
    return _editBox->isTextSelection();
}

void
GuiEditBoxComponent::setTextSelectionColour(const float4& value)
{
    _editBox->setTextSelectionColour(value);
}

void
GuiEditBoxComponent::setTextCursor(std::size_t index)
{
    _editBox->setTextCursor(index);
}

std::size_t
GuiEditBoxComponent::getTextCursor() const
{
    return _editBox->getTextCursor();
}

void
GuiEditBoxComponent::setOnlyText(const std::string& value)
{
    _editBox->setOnlyText(value);
}

std::string
GuiEditBoxComponent::getOnlyText()
{
    return _editBox->getOnlyText();
}

std::size_t
GuiEditBoxComponent::getTextLength() const
{
    return _editBox->getTextLength();
}

void
GuiEditBoxComponent::setOverflowToTheLeft(bool value)
{
    _editBox->setOverflowToTheLeft(value);
}

bool
GuiEditBoxComponent::getOverflowToTheLeft() const
{
    return _editBox->getOverflowToTheLeft();
}

void
GuiEditBoxComponent::setMaxTextLength(std::size_t value)
{
    _editBox->setMaxTextLength(value);
}

std::size_t
GuiEditBoxComponent::getMaxTextLength() const
{
    return _editBox->getMaxTextLength();
}

void
GuiEditBoxComponent::insertText(const std::string& text, std::size_t index)
{
    _editBox->insertText(text, index);
}

void
GuiEditBoxComponent::addText(const std::string& text)
{
    _editBox->addText(text);
}

void
GuiEditBoxComponent::eraseText(std::size_t start, std::size_t count)
{
    _editBox->eraseText(start, count);
}

void
GuiEditBoxComponent::setEditReadOnly(bool value)
{
    _editBox->setEditReadOnly(value);
}

bool
GuiEditBoxComponent::getEditReadOnly() const
{
    return _editBox->getEditReadOnly();
}

void
GuiEditBoxComponent::setEditPassword(bool value)
{
    _editBox->setEditPassword(value);
}

bool
GuiEditBoxComponent::getEditPassword() const
{
    return _editBox->getEditPassword();
}

void
GuiEditBoxComponent::setEditMultiLine(bool value)
{
    _editBox->setEditMultiLine(value);
}

bool
GuiEditBoxComponent::getEditMultiLine() const
{
    return _editBox->getEditMultiLine();
}

void
GuiEditBoxComponent::setEditStatic(bool value)
{
    _editBox->setEditStatic(value);
}

bool
GuiEditBoxComponent::getEditStatic() const
{
    return _editBox->getEditStatic();
}

void
GuiEditBoxComponent::setPasswordChar(GuiInputChar value)
{
    _editBox->setPasswordChar(value);
}

void
GuiEditBoxComponent::setPasswordChar(const std::string& value)
{
    _editBox->setPasswordChar(value);
}

GuiInputChar
GuiEditBoxComponent::getPasswordChar() const
{
    return _editBox->getPasswordChar();
}

void
GuiEditBoxComponent::setEditWordWrap(bool value)
{
    _editBox->setEditWordWrap(value);
}

bool
GuiEditBoxComponent::getEditWordWrap() const
{
    return _editBox->getEditWordWrap();
}

void
GuiEditBoxComponent::setTabPrinting(bool value)
{
    _editBox->setTabPrinting(value);
}

bool
GuiEditBoxComponent::getTabPrinting() const
{
    return _editBox->getTabPrinting();
}

bool
GuiEditBoxComponent::getInvertSelected()
{
    return _editBox->getInvertSelected();
}

void
GuiEditBoxComponent::setInvertSelected(bool value)
{
    _editBox->setInvertSelected(value);
}

std::size_t
GuiEditBoxComponent::getVScrollRange() const
{
    return _editBox->getVScrollRange();
}

std::size_t
GuiEditBoxComponent::getVScrollPosition()
{
    return _editBox->getVScrollPosition();
}

void
GuiEditBoxComponent::setVScrollPosition(std::size_t index)
{
    _editBox->setVScrollPosition(index);
}

std::size_t
GuiEditBoxComponent::getHScrollRange() const
{
    return _editBox->getHScrollRange();
}

std::size_t
GuiEditBoxComponent::getHScrollPosition()
{
    return _editBox->getHScrollPosition();
}

void
GuiEditBoxComponent::setHScrollPosition(std::size_t index)
{
    _editBox->setHScrollPosition(index);
}

void
GuiEditBoxComponent::onAttach() except
{
	if (_label)
		_label->setParent(this->getGameObject());
}

void
GuiEditBoxComponent::onDetach() except
{
	if (_label)
		_label->setParent(nullptr);
}

void
GuiEditBoxComponent::load(iarchive& reader) noexcept
{
	auto label = std::make_shared<GuiLabelComponent>(_editBox->getGuiTextBox());
	label->load(reader);

	_label = std::make_shared<GameObject>();
	_label->addComponent(label);
}

void
GuiEditBoxComponent::save(oarchive& write) noexcept
{
	GuiWidgetComponent::save(write);
}

GameComponentPtr
GuiEditBoxComponent::clone() const except
{
	return std::make_shared<GuiEditBoxComponent>();
}

void
GuiEditBoxComponent::setGuiWidget(GuiWidgetPtr widget) noexcept
{
	_editBox = widget->downcast<GuiEditBox>();
}

GuiWidgetPtr
GuiEditBoxComponent::getGuiWidget() const noexcept
{
	return _editBox;
}

_NAME_END

#endif