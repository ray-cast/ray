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
#ifndef _H_GUI_MENUITEM_COMPONENT_H_
#define _H_GUI_MENUITEM_COMPONENT_H_

#include <ray/gui_widget_component.h>

_NAME_BEGIN

class GuiMenuItemComponent final : public GuiWidgetComponent
{
	__DeclareSubClass(GuiMenuItemComponent, GuiWidgetComponent)
public:
	GuiMenuItemComponent() noexcept;
	virtual ~GuiMenuItemComponent() noexcept;

	void setItemName(const std::string& value) noexcept;
	std::string getItemName() noexcept;

	void setItemData(const std::string& value) noexcept;
	std::string* getItemData() const noexcept;

	void removeItem() noexcept;
	void setItemId(const std::string& value) noexcept;
	const std::string& getItemId() noexcept;

	std::size_t getItemIndex() noexcept;

	void setItemType(GuiMenuItemType value) noexcept;
	GuiMenuItemType getItemType() noexcept;

	void setItemChildVisible(bool value) noexcept;

	bool getItemChecked() const noexcept;
	void setItemChecked(bool value) noexcept;

	void load(iarchive& reader) noexcept;
	void save(oarchive& write) noexcept;

	GameComponentPtr clone() const noexcept;

protected:
	virtual void onAttach() except;
	virtual void onDetach() noexcept;

private:
	GuiMenuItemComponent(const GuiMenuItemComponent&) noexcept = delete;
	GuiMenuItemComponent& operator=(const GuiMenuItemComponent&) noexcept = delete;

private:

	GuiMenuItemPtr _menuItem;

	GameComponentPtr _button;
	GameObjectPtr _buttonObject;
};

_NAME_END

#endif