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
#ifndef _H_GUI_IMAGEBOX_H_
#define _H_GUI_IMAGEBOX_H_

#include <ray/gui_widget.h>

_NAME_BEGIN

class EXPORT GuiImageBox : public GuiWidget
{
	__DeclareSubInterface(GuiImageBox, GuiWidget)
public:
	GuiImageBox(GuiWidgetImpl& impl) noexcept;
	virtual ~GuiImageBox() noexcept;

	virtual void setImageInfo(const std::string& texture, const Viewport& coord, const Size& tile) = 0;

	virtual void setImageTexture(const std::string& value) = 0;
	virtual void setImageRect(const Viewport& value) = 0;
	virtual void setImageCoord(const Viewport& value) = 0;
	virtual void setImageTile(const int2& value) = 0;

	virtual void setImageIndex(std::size_t index) = 0;
	virtual std::size_t getImageIndex() const = 0;

	virtual Size getImageSize() const = 0;

	virtual std::size_t getItemCount() const = 0;

	virtual void setItemSelect(std::size_t index) = 0;
	virtual std::size_t getItemSelect() const = 0;
	virtual void resetItemSelect() = 0;

	virtual void insertItem(std::size_t index, const Viewport& _item) = 0;
	virtual void addItem(const Viewport& _item) = 0;
	virtual void setItem(std::size_t index, const Viewport& _item) = 0;

	virtual void deleteAllItems() = 0;

	virtual void addItemFrame(std::size_t index, const Viewport& _item) = 0;

	virtual void insertItemFrame(std::size_t index, std::size_t indexFrame, const Viewport& _item) = 0;

	virtual void addItemFrameDuplicate(std::size_t index, std::size_t indexSourceFrame) = 0;

	virtual void insertItemFrameDuplicate(std::size_t index, std::size_t indexFrame, std::size_t indexSourceFrame) = 0;

	virtual void setItemFrame(std::size_t index, std::size_t indexFrame, const Viewport& _item) = 0;

	virtual void deleteItemFrame(std::size_t index, std::size_t indexFrame) = 0;
	virtual void deleteAllItemFrames(std::size_t index) = 0;

	virtual void setItemFrameRate(std::size_t index, float _rate) = 0;
	virtual float getItemFrameRate(std::size_t index) = 0;

	virtual bool setItemResource(const std::string& _name) = 0;

	virtual void setItemGroup(const std::string& value) = 0;
	virtual void setItemName(const std::string& value) = 0;
};

_NAME_END

#endif