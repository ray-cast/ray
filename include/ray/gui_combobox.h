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
#ifndef _H_GUI_COMBOBOX_H_
#define _H_GUI_COMBOBOX_H_

#include <ray/gui_widget.h>

_NAME_BEGIN

class EXPORT GuiComboBox : public GuiWidget
{
	__DeclareSubInterface(GuiComboBox, GuiWidget)
public:
	GuiComboBox(GuiWidgetImpl& impl) noexcept;
	virtual ~GuiComboBox() noexcept;

    virtual std::size_t getItemCount() const = 0;
    virtual void insertItemAt(std::size_t index, const std::string& name, std::string data) = 0;
    virtual void addItem(const std::string& name, const std::string& data) = 0;
    virtual void removeItemAt(std::size_t index) = 0;
    virtual void removeAllItems() = 0;
    virtual std::size_t findItemIndexWith(const std::string& name) = 0;

    virtual void setIndexSelected(std::size_t index) = 0;
    virtual std::size_t getIndexSelected() const = 0;
    virtual void clearIndexSelected() = 0;

    virtual void setItemDataAt(std::size_t index, const std::string& data) = 0;
    virtual void clearItemDataAt(std::size_t index) = 0;
    virtual std::string* getItemDataAt(std::size_t index, bool _throw = true) const = 0;

    virtual void setItemNameAt(std::size_t index, const std::string& name) = 0;
    virtual std::string getItemNameAt(std::size_t index) const = 0;

    virtual void beginToItemAt(std::size_t index) = 0;
    virtual void beginToItemFirst() = 0;
    virtual void beginToItemLast() = 0;
    virtual void beginToItemSelected() = 0;

    virtual void setComboModeDrop(bool value) = 0;
    virtual bool getComboModeDrop() const = 0;

    virtual void setSmoothShow(bool value) = 0;
    virtual bool getSmoothShow() const = 0;

    virtual void setMaxListLength(int value) = 0;
    virtual int getMaxListLength() const = 0;

    virtual void setFlowDirection(GuiFlowDirection value) = 0;
    virtual GuiFlowDirection getFlowDirection() const = 0;

	virtual GuiEditBoxPtr getGuiEditBox() const noexcept = 0;

private:
	GuiComboBox(const GuiComboBox&) noexcept = delete;
	GuiComboBox& operator=(const GuiComboBox&) noexcept = delete;
};

_NAME_END

#endif