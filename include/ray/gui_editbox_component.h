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
#ifndef _H_GUI_EDITBOX_COMPONENT_H_
#define _H_GUI_EDITBOX_COMPONENT_H_

#include <ray/gui_widget_component.h>

_NAME_BEGIN

class GuiEditBoxComponent final : public GuiWidgetComponent
{
	__DeclareSubClass(GuiEditBoxComponent, GuiWidgetComponent)
public:
	GuiEditBoxComponent() noexcept;
	GuiEditBoxComponent(GuiEditBoxPtr& edit) noexcept;
	~GuiEditBoxComponent() noexcept;

	void setTextIntervalColour(std::size_t start, std::size_t count, const float4& colour);
	std::size_t getTextSelectionStart() const;
	std::size_t getTextSelectionEnd() const;
	std::size_t getTextSelectionLength() const;
	std::string getTextInterval(std::size_t start, std::size_t count);

	void setTextSelection(std::size_t start, std::size_t end);
	void deleteTextSelection();
	std::string getTextSelection();
	bool isTextSelection() const;
	void setTextSelectionColour(const float4& value);

	void setTextCursor(std::size_t index);
	std::size_t getTextCursor() const;

	void setOnlyText(const std::string& value);
	std::string getOnlyText();

	std::size_t getTextLength() const;

	void setOverflowToTheLeft(bool value);
	bool getOverflowToTheLeft() const;

	void setMaxTextLength(std::size_t value);
	std::size_t getMaxTextLength() const;

	void insertText(const std::string& _text, std::size_t index);
	void addText(const std::string& _text);
	void eraseText(std::size_t start, std::size_t count = 1);

	void setEditReadOnly(bool value);
	bool getEditReadOnly() const;

	void setEditPassword(bool value);
	bool getEditPassword() const;

	void setEditMultiLine(bool value);
	bool getEditMultiLine() const;

	void setEditStatic(bool value);
	bool getEditStatic() const;

	void setPasswordChar(GuiInputChar value);
	void setPasswordChar(const std::string& value);
	GuiInputChar getPasswordChar() const;

	void setEditWordWrap(bool value);
	bool getEditWordWrap() const;

	void setTabPrinting(bool value);
	bool getTabPrinting() const;

	bool getInvertSelected();
	void setInvertSelected(bool value);

	std::size_t getVScrollRange() const;
	std::size_t getVScrollPosition();
	void setVScrollPosition(std::size_t index);

	std::size_t getHScrollRange() const;
	std::size_t getHScrollPosition();
	void setHScrollPosition(std::size_t index);

	void load(iarchive& reader) noexcept;
	void save(oarchive& write) noexcept;

	GameComponentPtr clone() const except;

protected:
	virtual void onAttach() except;
	virtual void onDetach() except;

private:
	GuiEditBoxComponent(const GuiEditBoxComponent&) noexcept = delete;
	GuiEditBoxComponent& operator=(const GuiEditBoxComponent&) noexcept = delete;

private:

	GuiEditBoxPtr _editBox;

	GameObjectPtr _label;
};

_NAME_END

#endif