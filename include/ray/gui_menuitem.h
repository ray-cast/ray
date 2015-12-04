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
#ifndef _H_GUI_MENUITEM_H_
#define _H_GUI_MENUITEM_H_

#include <ray/gui_widget.h>

_NAME_BEGIN

class EXPORT GuiMenuItem : public GuiWidget
{
	__DeclareSubInterface(GuiMenuItem, GuiWidget)
public:
	GuiMenuItem(GuiWidgetImpl& impl) noexcept;
	virtual ~GuiMenuItem() noexcept;

	virtual GuiButtonPtr getGuiButton() const noexcept = 0;

	virtual void setItemName(const std::string& value) noexcept = 0;
	virtual std::string getItemName() noexcept = 0;

	virtual void setItemData(const std::string& value) noexcept = 0;
	virtual std::string* getItemData() const noexcept = 0;

	virtual void removeItem() noexcept = 0;
	virtual void setItemId(const std::string& value) noexcept = 0;
	virtual const std::string& getItemId() noexcept = 0;

	virtual std::size_t getItemIndex() noexcept = 0;

	virtual void setItemType(GuiMenuItemType value) noexcept = 0;
	virtual GuiMenuItemType getItemType() noexcept = 0;

	virtual void setItemChildVisible(bool value) noexcept = 0;

	virtual bool getItemChecked() const noexcept = 0;
	virtual void setItemChecked(bool value) noexcept = 0;

private:
	GuiMenuItem(const GuiMenuItem&) noexcept = delete;
	GuiMenuItem& operator=(const GuiMenuItem&) noexcept = delete;
};

_NAME_END

#endif