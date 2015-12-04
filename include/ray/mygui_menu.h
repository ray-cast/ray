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
#ifndef _H_MYGUI_MENU_H_
#define _H_MYGUI_MENU_H_

#include <ray/mygui_widget.h>

_NAME_BEGIN

class MyGuiMenuImpl final : public MyGuiWidget
{
	__DeclareSubClass(MyGuiMenuImpl, MyGuiWidget)
public:
	MyGuiMenuImpl() noexcept;
	virtual ~MyGuiMenuImpl() noexcept;

	bool create() except;

	void setVisibleSmooth(bool value) noexcept;
	std::size_t getItemCount() const noexcept;

	void removeItemAt(std::size_t index) noexcept;
	void removeAllItems() noexcept;

	void setItemDataAt(std::size_t index, const std::string& data) noexcept;
	void clearItemDataAt(std::size_t index) noexcept;
	void setItemIdAt(std::size_t index, const std::string& _id) noexcept;
	std::string getItemIdAt(std::size_t index) noexcept;

	void setItemNameAt(std::size_t index, const std::string& name) noexcept;
	std::string getItemNameAt(size_t index) noexcept;

	std::size_t findItemIndexWith(const std::string& name) noexcept;
	void setItemChildVisibleAt(std::size_t index, bool visible) noexcept;

	void removeItemChildAt(std::size_t index) noexcept;

	void setItemTypeAt(std::size_t index, GuiMenuItemType type) noexcept;
	GuiMenuItemType getItemTypeAt(std::size_t index) noexcept;

	void setPopupAccept(bool value) noexcept;
	bool getPopupAccept() const noexcept;

	void setVerticalAlignment(bool value) noexcept;
	bool getVerticalAlignment() const noexcept;

	void addMenuCtrlCloseListener(std::function<void()>& func) noexcept;
	void addMenuCtrlAcceptListener(std::function<void()>& func) noexcept;

	void removeMenuCtrlCloseListener(std::function<void()>& func) noexcept;
	void removeMenuCtrlAcceptListener(std::function<void()>& func) noexcept;

private:
	void onMenuCtrlClose(MyGUI::MenuControl*) noexcept;
	void onMenuCtrlAccept(MyGUI::MenuControl*, MyGUI::MenuItem*) noexcept;

private:
	MyGuiMenuImpl(const MyGuiMenuImpl&) noexcept = delete;
	MyGuiMenuImpl& operator=(const MyGuiMenuImpl&) noexcept = delete;

private:

	MyGUI::Widget* _parent;
	MyGUI::MenuBar* _menuBar;
	GuiButtonPtr _button;

	delegate<void()> _onMenuCtrlClose;
	delegate<void()> _onMenuCtrlAccept;
};

class MyGuiMenu final : public GuiMenu
{
	__DeclareSubClass(MyGuiMenu, GuiMenu)
public:
	MyGuiMenu() noexcept;
	virtual ~MyGuiMenu() noexcept;

	void setVisibleSmooth(bool value) noexcept;
	std::size_t getItemCount() const noexcept;

	void removeItemAt(std::size_t index) noexcept;
	void removeAllItems() noexcept;

	void setItemDataAt(std::size_t index, const std::string& data) noexcept;
	void clearItemDataAt(std::size_t index) noexcept;
	void setItemIdAt(std::size_t index, const std::string& _id) noexcept;
	std::string getItemIdAt(std::size_t index) noexcept;

	void setItemNameAt(std::size_t index, const std::string& name) noexcept;
	std::string getItemNameAt(std::size_t index) noexcept;

	std::size_t findItemIndexWith(const std::string& name) noexcept;
	void setItemChildVisibleAt(std::size_t index, bool visible) noexcept;

	void removeItemChildAt(std::size_t index) noexcept;

	void setItemTypeAt(std::size_t index, GuiMenuItemType type) noexcept;
	GuiMenuItemType getItemTypeAt(std::size_t index) noexcept;

	void setPopupAccept(bool value) noexcept;
	bool getPopupAccept() const noexcept;

	void setVerticalAlignment(bool value) noexcept;
	bool getVerticalAlignment() const noexcept;

	void addMenuCtrlCloseListener(std::function<void()>& func) noexcept;
	void addMenuCtrlAcceptListener(std::function<void()>& func) noexcept;

	void removeMenuCtrlCloseListener(std::function<void()>& func) noexcept;
	void removeMenuCtrlAcceptListener(std::function<void()>& func) noexcept;

private:
	MyGuiMenu(const MyGuiMenu&) noexcept = delete;
	MyGuiMenu& operator=(const MyGuiMenu&) noexcept = delete;

private:

	MyGuiMenuImpl _impl;
};

_NAME_END

#endif