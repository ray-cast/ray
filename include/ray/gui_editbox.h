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
#ifndef _H_GUI_EDITBOX_H_
#define _H_GUI_EDITBOX_H_

#include <ray/gui_widget.h>

_NAME_BEGIN

class EXPORT GuiEditBox : public GuiWidget
{
	__DeclareSubInterface(GuiButton, GuiWidget)
public:
	GuiEditBox(GuiWidgetImpl& impl) noexcept;
	virtual ~GuiEditBox() noexcept;

	virtual void setTextIntervalColour(std::size_t start, std::size_t count, const float4& colour) = 0;
	virtual std::size_t getTextSelectionStart() const = 0;
	virtual std::size_t getTextSelectionEnd() const = 0;
	virtual std::size_t getTextSelectionLength() const = 0;
	virtual std::string getTextInterval(std::size_t start, std::size_t count) = 0;

	virtual void setTextSelection(std::size_t start, std::size_t end) = 0;
	virtual void deleteTextSelection() = 0;
	virtual std::string getTextSelection() = 0;
	virtual bool isTextSelection() const = 0;
	virtual void setTextSelectionColour(const float4& value) = 0;

	virtual void setTextCursor(std::size_t index) = 0;
	virtual std::size_t getTextCursor() const = 0;

	virtual void setOnlyText(const std::string& value) = 0;
	virtual std::string getOnlyText() = 0;

	virtual std::size_t getTextLength() const = 0;

	virtual void setOverflowToTheLeft(bool value) = 0;
	virtual bool getOverflowToTheLeft() const = 0;

	virtual void setMaxTextLength(std::size_t value) = 0;
	virtual std::size_t getMaxTextLength() const = 0;

	virtual void insertText(const std::string& _text, std::size_t index) = 0;
	virtual void addText(const std::string& _text) = 0;
	virtual void eraseText(std::size_t start, std::size_t count = 1) = 0;

	virtual void setEditReadOnly(bool value) = 0;
	virtual bool getEditReadOnly() const = 0;

	virtual void setEditPassword(bool value) = 0;
	virtual bool getEditPassword() const = 0;

	virtual void setEditMultiLine(bool value) = 0;
	virtual bool getEditMultiLine() const = 0;

	virtual void setEditStatic(bool value) = 0;
	virtual bool getEditStatic() const = 0;

	virtual void setPasswordChar(GuiInputChar value) = 0;
	virtual void setPasswordChar(const std::string& value) = 0;
	virtual GuiInputChar getPasswordChar() const = 0;

	virtual void setEditWordWrap(bool value) = 0;
	virtual bool getEditWordWrap() const = 0;

	virtual void setTabPrinting(bool value) = 0;
	virtual bool getTabPrinting() const = 0;

	virtual bool getInvertSelected() = 0;
	virtual void setInvertSelected(bool value) = 0;

	virtual std::size_t getVScrollRange() const = 0;
	virtual std::size_t getVScrollPosition() = 0;
	virtual void setVScrollPosition(std::size_t index) = 0;

	virtual std::size_t getHScrollRange() const = 0;
	virtual std::size_t getHScrollPosition() = 0;
	virtual void setHScrollPosition(std::size_t index) = 0;

	virtual void addTextChangeListener(std::function<void()>& func) noexcept = 0;
	virtual void removeTextChangeListener(std::function<void()>& func) noexcept = 0;

	virtual void addSelectAccept(std::function<void()>& func) noexcept = 0;
	virtual void removeSelectAccept(std::function<void()>& func) noexcept = 0;

	virtual GuiTextBoxPtr getGuiTextBox() const noexcept = 0;

private:
	GuiEditBox(const GuiEditBox&) noexcept = delete;
	GuiEditBox& operator=(const GuiEditBox&) noexcept = delete;
};

_NAME_END

#endif