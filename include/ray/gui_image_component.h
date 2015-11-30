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
#ifndef _H_GUI_IMAGE_COMPONENT_H_
#define _H_GUI_IMAGE_COMPONENT_H_

#include <ray/gui_widget_component.h>

_NAME_BEGIN

class GuiImageComponent final : public GuiWidgetComponent
{
	__DeclareSubClass(GuiImageComponent, GuiWidgetComponent)
public:
	GuiImageComponent() noexcept;
	~GuiImageComponent() noexcept;

    void setImageInfo(const std::string& texture, const Rect& coord, const Size& tile);
    void setImageTexture(const std::string& value);
    void setImageCoord(const Rect& value);
    void setImageTile(const Size& value);

    void setImageIndex(std::size_t index);
    std::size_t getImageIndex() const;
    
	Size getImageSize() const;
    std::size_t getItemCount() const;
    
	void setItemSelect(std::size_t index);
    std::size_t getItemSelect() const;
    
	void resetItemSelect();
    void insertItem(std::size_t index, const Rect& _item);
    void addItem(const Rect& _item);
    void setItem(std::size_t index, const Rect& _item);
    void deleteAllItems();
    
	void addItemFrame(std::size_t index, const Rect& _item);
    void insertItemFrame(std::size_t index, std::size_t indexFrame, const Rect& _item);
    void addItemFrameDuplicate(std::size_t index, std::size_t indexSourceFrame);
    void insertItemFrameDuplicate(std::size_t index, std::size_t indexFrame, std::size_t indexSourceFrame);
    void setItemFrame(std::size_t index, std::size_t indexFrame, const Rect& _item);
    void deleteItemFrame(std::size_t index, std::size_t indexFrame);
    void deleteAllItemFrames(std::size_t index);
    
	void setItemFrameRate(std::size_t index, float _rate);
    float getItemFrameRate(std::size_t index);

    bool setItemResource(const std::string& _name);
    void setItemGroup(const std::string& value);
    void setItemName(const std::string& value);

	void load(iarchive& reader) noexcept;
	void save(oarchive& write) noexcept;

	GameComponentPtr clone() const except;

protected:
	virtual void setGuiWidget(GuiWidgetPtr widget) noexcept;
	virtual GuiWidgetPtr getGuiWidget() const noexcept;

private:

	GuiImageBoxPtr _imagebox;
};

_NAME_END

#endif