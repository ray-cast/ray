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
#ifndef _H_GUI_MENU_H_
#define _H_GUI_MENU_H_

#include <ray/gui_widget.h>

_NAME_BEGIN

class EXPORT GuiMenu : public GuiWidget
{
	__DeclareSubInterface(GuiMenu, GuiWidget)
public:
	GuiMenu(GuiWidgetImpl& impl) noexcept;
	virtual ~GuiMenu() noexcept;

	virtual void setVisibleSmooth(bool value) noexcept = 0;
	virtual std::size_t getItemCount() const noexcept = 0;

	virtual void removeItemAt(std::size_t index) noexcept = 0;
	virtual void removeAllItems() noexcept = 0;

	virtual void setItemDataAt(std::size_t index, const std::string& data) noexcept = 0;
	virtual void clearItemDataAt(std::size_t index) noexcept = 0;
	virtual void setItemIdAt(std::size_t index, const std::string& _id) noexcept = 0;
	virtual std::string getItemIdAt(std::size_t index) noexcept = 0;

	virtual void setItemNameAt(std::size_t index, const std::string& name) noexcept = 0;
	virtual std::string getItemNameAt(std::size_t index) noexcept = 0;

	virtual std::size_t findItemIndexWith(const std::string& name) noexcept = 0;
	virtual void setItemChildVisibleAt(std::size_t index, bool visible) noexcept = 0;

	virtual void removeItemChildAt(std::size_t index) noexcept = 0;

	virtual void setItemTypeAt(std::size_t index, GuiMenuItemType type) noexcept = 0;
	virtual GuiMenuItemType getItemTypeAt(std::size_t index) noexcept = 0;

	virtual void setPopupAccept(bool value) noexcept = 0;
	virtual bool getPopupAccept() const noexcept = 0;

	virtual void setVerticalAlignment(bool value) noexcept = 0;
	virtual bool getVerticalAlignment() const noexcept = 0;

	virtual void addMenuCtrlCloseListener(std::function<void()>* func) noexcept = 0;
	virtual void addMenuCtrlAcceptListener(std::function<void()>* func) noexcept = 0;

	virtual void removeMenuCtrlCloseListener(std::function<void()>* func) noexcept = 0;
	virtual void removeMenuCtrlAcceptListener(std::function<void()>* func) noexcept = 0;

private:
	GuiMenu(const GuiMenu&) noexcept = delete;
	GuiMenu& operator=(const GuiMenu&) noexcept = delete;
};

_NAME_END

#endif