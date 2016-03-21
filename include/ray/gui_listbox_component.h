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
#ifndef _H_GUI_BEHAVIOUR_COMPONENT_H_
#define _H_GUI_BEHAVIOUR_COMPONENT_H_

#include <ray/gui_widget_component.h>

_NAME_BEGIN

class GuiListBoxComponent final : public GuiWidgetComponent
{
	__DeclareSubInterface(GUIBehaviourComponent, GameComponent)
public:
	GuiListBoxComponent() noexcept;
	~GuiListBoxComponent() noexcept;

    std::size_t getItemCount() const noexcept;
    void insertItemAt(std::size_t index, const std::string& name) noexcept;
    void addItem(const std::string& name) noexcept;
    void removeItemAt(std::size_t index) noexcept;
    void removeAllItems() noexcept;
    void swapItemsAt(std::size_t index1, std::size_t index2) noexcept;

    std::size_t findItemIndexWith(const std::string& name) noexcept;
    std::size_t getIndexSelected() const noexcept;
    void setIndexSelected(std::size_t index) noexcept;
    void clearIndexSelected() noexcept;
    void setItemNameAt(std::size_t index, const std::string& name) noexcept;
    void clearItemDataAt(std::size_t index) noexcept;

    std::string getItemNameAt(std::size_t index) const noexcept;

    void beginToItemAt(std::size_t index) noexcept;
    void beginToItemFirst() noexcept;
    void beginToItemLast() noexcept;
    void beginToItemSelected() noexcept;

    bool isItemVisibleAt(std::size_t index, bool fill = true) noexcept;
    bool isItemSelectedVisible(bool fill = true) noexcept;

    void setScrollVisible(bool visible) noexcept;
    void setScrollPosition(std::size_t position) noexcept;

    int getOptimalHeight() noexcept;

    void setActivateOnClick(bool activateOnClick) noexcept;

    void addListSelectAcceptListener(std::function<void()> func) noexcept;
    void addListChangePositionListener(std::function<void()> func) noexcept;
    void addListMouseItemActivateListener(std::function<void()> func) noexcept;
    void addListMouseItemFocusListener(std::function<void()> func) noexcept;
    void addListChangeScrollListener(std::function<void()> func) noexcept;

    void removeListSelectAcceptListener(std::function<void()> func) noexcept;
    void removeListChangePositionListener(std::function<void()> func) noexcept;
    void removeListMouseItemActivateListener(std::function<void()> func) noexcept;
    void removeListMouseItemFocusListener(std::function<void()> func) noexcept;
    void removeListChangeScrollListener(std::function<void()> func) noexcept;

	void load(iarchive& reader) noexcept;
	void save(oarchive& write) noexcept;

	GameComponentPtr clone() const noexcept;

private:
    GuiListBoxComponent(const GuiListBoxComponent&) noexcept = delete;
    GuiListBoxComponent& operator=(const GuiListBoxComponent&) noexcept = delete;

private:
    GuiListBoxPtr _listBox;
};

_NAME_END

#endif