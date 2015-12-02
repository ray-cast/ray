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
#ifndef _H_MYGUI_COMBOBOX_H_
#define _H_MYGUI_COMBOBOX_H_

#include <ray/mygui_widget.h>

_NAME_BEGIN

class MyGuiComboBoxImpl final : public MyGuiWidget
{
	__DeclareSubClass(MyGuiComboBoxImpl, MyGuiWidget)
public:
	MyGuiComboBoxImpl() noexcept;
	virtual ~MyGuiComboBoxImpl() noexcept;

	bool create() except;

	std::size_t getItemCount() const;
	void insertItemAt(std::size_t index, const std::string& name, std::string data);
	void addItem(const std::string& name, const std::string& data);
	void removeItemAt(std::size_t index);
	void removeAllItems();
	std::size_t findItemIndexWith(const std::string& name);

	void setIndexSelected(std::size_t index);
	std::size_t getIndexSelected() const;
	void clearIndexSelected();

	void setItemDataAt(std::size_t index, const std::string& data);
	void clearItemDataAt(std::size_t index);
	std::string* getItemDataAt(std::size_t index, bool _throw = true) const;

	void setItemNameAt(std::size_t index, const std::string& name);
	std::string getItemNameAt(std::size_t index) const;

	void beginToItemAt(std::size_t index);
	void beginToItemFirst();
	void beginToItemLast();
	void beginToItemSelected();

	void setComboModeDrop(bool _value);
	bool getComboModeDrop() const;

	void setSmoothShow(bool _value);
	bool getSmoothShow() const;

	void setMaxListLength(int _value);
	int getMaxListLength() const;

	void setFlowDirection(GuiFlowDirection _value);
	GuiFlowDirection getFlowDirection() const;

	GuiEditBoxPtr getGuiEditBox() const noexcept;

private:
	MyGuiComboBoxImpl(const MyGuiComboBoxImpl&) noexcept = delete;
	MyGuiComboBoxImpl& operator=(const MyGuiComboBoxImpl&) noexcept = delete;

private:

	std::string _caption;
	MyGUI::ComboBox* _comboBox;
	MyGUI::Widget* _parent;

	GuiEditBoxPtr _editBox;
};

class MyGuiComboBox final : public GuiComboBox
{
	__DeclareSubClass(MyGuiComboBox, GuiComboBox)
public:
	MyGuiComboBox() noexcept;
	virtual ~MyGuiComboBox() noexcept;

	GuiEditBoxPtr getGuiEditBox() const noexcept;

	std::size_t getItemCount() const;
	void insertItemAt(std::size_t index, const std::string& name, std::string data);
	void addItem(const std::string& name, const std::string& data);
	void removeItemAt(std::size_t index);
	void removeAllItems();
	std::size_t findItemIndexWith(const std::string& name);

	void setIndexSelected(std::size_t index);
	std::size_t getIndexSelected() const;
	void clearIndexSelected();

	void setItemDataAt(std::size_t index, const std::string& data);
	void clearItemDataAt(std::size_t index);
	std::string* getItemDataAt(std::size_t index, bool _throw = true) const;

	void setItemNameAt(std::size_t index, const std::string& name);
	std::string getItemNameAt(std::size_t index) const;

	void beginToItemAt(std::size_t index);
	void beginToItemFirst();
	void beginToItemLast();
	void beginToItemSelected();

	void setComboModeDrop(bool _value);
	bool getComboModeDrop() const;

	void setSmoothShow(bool _value);
	bool getSmoothShow() const;

	void setMaxListLength(int _value);
	int getMaxListLength() const;

	void setFlowDirection(GuiFlowDirection _value);
	GuiFlowDirection getFlowDirection() const;

private:
	MyGuiComboBox(const MyGuiComboBox&) noexcept = delete;
	MyGuiComboBox& operator=(const MyGuiComboBox&) noexcept = delete;

private:

	MyGuiComboBoxImpl _impl;
};

_NAME_END

#endif