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
#if defined(_BUILD_MYGUI)
#include <ray/mygui_imagebox.h>

_NAME_BEGIN

__ImplementSubClass(MyGuiImageBox, GuiImageBox, "MyGuiImageBox")
__ImplementSubClass(MyGuiImageBoxImpl, GuiWidgetImpl, "MyGuiImageBoxImpl")

MyGuiImageBoxImpl::MyGuiImageBoxImpl() noexcept
	: _imagebox(nullptr)
	, _parent(nullptr)
{
}

MyGuiImageBoxImpl::~MyGuiImageBoxImpl() noexcept
{
	this->destroy();
}

bool
MyGuiImageBoxImpl::create() except
{
	assert(!_imagebox);
	if (_parent)
		_imagebox = _parent->createWidget<MyGUI::ImageBox>("", 0, 0, 0, 0, MyGUI::Align::Default, "");
	else
		_imagebox = MyGUI::Gui::getInstance().createWidget<MyGUI::ImageBox>("", 0, 0, 0, 0, MyGUI::Align::Default, "Main", "");

	this->setWidget(_imagebox);

	return _imagebox ? true : false;
}

void
MyGuiImageBoxImpl::setImageInfo(const std::string& texture, const Rect& view, const Size& tile)
{
	assert(_imagebox);
	_imagebox->setImageInfo(texture, convert(view), convert(tile));
}

void
MyGuiImageBoxImpl::setImageTexture(const std::string& value)
{
	assert(_imagebox);
	_imagebox->setImageTexture(value);
}

void
MyGuiImageBoxImpl::setImageCoord(const Rect& value)
{
	assert(_imagebox);
	_imagebox->setImageCoord(convert(value));
}

void
MyGuiImageBoxImpl::setImageTile(const Size& value)
{
	assert(_imagebox);
	_imagebox->setImageTile(convert(value));
}

void
MyGuiImageBoxImpl::setImageIndex(std::size_t index)
{
	assert(_imagebox);
	_imagebox->setImageIndex(index);
}

std::size_t
MyGuiImageBoxImpl::getImageIndex() const
{
	assert(_imagebox);
	return _imagebox->getImageIndex();
}

Size
MyGuiImageBoxImpl::getImageSize() const
{
	assert(_imagebox);
	return convert(_imagebox->getImageSize());
}

std::size_t
MyGuiImageBoxImpl::getItemCount() const
{
	assert(_imagebox);
	return _imagebox->getItemCount();
}

void
MyGuiImageBoxImpl::setItemSelect(std::size_t index)
{
	assert(_imagebox);
	_imagebox->setItemSelect(index);
}

std::size_t
MyGuiImageBoxImpl::getItemSelect() const
{
	assert(_imagebox);
	return _imagebox->getItemSelect();
}

void
MyGuiImageBoxImpl::resetItemSelect()
{
	assert(_imagebox);
	_imagebox->resetItemSelect();
}

void
MyGuiImageBoxImpl::insertItem(std::size_t index, const Rect& value)
{
	assert(_imagebox);
	_imagebox->insertItem(index, convert(value));
}

void
MyGuiImageBoxImpl::addItem(const Rect& value)
{
	assert(_imagebox);
	_imagebox->addItem(convert(value));
}

void
MyGuiImageBoxImpl::setItem(std::size_t index, const Rect& value)
{
	assert(_imagebox);
	_imagebox->setItem(index, convert(value));
}

void
MyGuiImageBoxImpl::deleteAllItems()
{
	assert(_imagebox);
	_imagebox->deleteAllItems();
}

void
MyGuiImageBoxImpl::addItemFrame(std::size_t index, const Rect& value)
{
	assert(_imagebox);
	_imagebox->addItemFrame(index, convert(value));
}

void
MyGuiImageBoxImpl::insertItemFrame(std::size_t index, std::size_t indexFrame, const Rect& value)
{
	assert(_imagebox);
	_imagebox->insertItemFrame(index, indexFrame, convert(value));
}

void
MyGuiImageBoxImpl::addItemFrameDuplicate(std::size_t index, std::size_t indexSourceFrame)
{
	assert(_imagebox);
	_imagebox->addItemFrameDuplicate(index, indexSourceFrame);
}

void
MyGuiImageBoxImpl::insertItemFrameDuplicate(std::size_t index, std::size_t indexFrame, std::size_t indexSourceFrame)
{
	assert(_imagebox);
	_imagebox->insertItemFrameDuplicate(index, indexFrame, indexSourceFrame);
}

void
MyGuiImageBoxImpl::setItemFrame(std::size_t index, std::size_t indexFrame, const Rect& value)
{
	assert(_imagebox);
	_imagebox->setItemFrame(index, indexFrame, convert(value));
}

void
MyGuiImageBoxImpl::deleteItemFrame(std::size_t index, std::size_t indexFrame)
{
	assert(_imagebox);
	_imagebox->deleteItemFrame(index, indexFrame);
}

void
MyGuiImageBoxImpl::deleteAllItemFrames(std::size_t index)
{
	assert(_imagebox);
	_imagebox->deleteAllItemFrames(index);
}

void
MyGuiImageBoxImpl::setItemFrameRate(std::size_t index, float rate)
{
	assert(_imagebox);
	_imagebox->setItemFrameRate(index, rate);
}

float
MyGuiImageBoxImpl::getItemFrameRate(std::size_t index)
{
	assert(_imagebox);
	return _imagebox->getItemFrameRate(index);
}

bool
MyGuiImageBoxImpl::setItemResource(const std::string& value)
{
	assert(_imagebox);
	return _imagebox->setItemResource(value);
}

void
MyGuiImageBoxImpl::setItemGroup(const std::string& value)
{
	assert(_imagebox);
	_imagebox->setItemGroup(value);
}

void
MyGuiImageBoxImpl::setItemName(const std::string& value)
{
	assert(_imagebox);
	_imagebox->setItemName(value);
}

MyGuiImageBox::MyGuiImageBox() noexcept
	: GuiImageBox(_impl)
{
}

MyGuiImageBox::~MyGuiImageBox() noexcept
{
}

void
MyGuiImageBox::setImageInfo(const std::string& texture, const Rect& coord, const Size& tile)
{
	_impl.setImageInfo(texture, coord, tile);
}

void
MyGuiImageBox::setImageTexture(const std::string& value)
{
	_impl.setImageTexture(value);
}

void
MyGuiImageBox::setImageCoord(const Rect& value)
{
	_impl.setImageCoord(value);
}

void
MyGuiImageBox::setImageTile(const Size& value)
{
	_impl.setImageTile(value);
}

void
MyGuiImageBox::setImageIndex(std::size_t index)
{
	_impl.setImageIndex(index);
}

std::size_t
MyGuiImageBox::getImageIndex() const
{
	return _impl.getImageIndex();
}

Size
MyGuiImageBox::getImageSize() const
{
	return _impl.getImageSize();
}

std::size_t
MyGuiImageBox::getItemCount() const
{
	return _impl.getItemCount();
}

void
MyGuiImageBox::setItemSelect(std::size_t index)
{
	_impl.setItemSelect(index);
}

std::size_t
MyGuiImageBox::getItemSelect() const
{
	return _impl.getItemSelect();
}

void
MyGuiImageBox::resetItemSelect()
{
	_impl.resetItemSelect();
}

void
MyGuiImageBox::insertItem(std::size_t index, const Rect& item)
{
	_impl.insertItem(index, item);
}

void
MyGuiImageBox::addItem(const Rect& item)
{
	_impl.addItem(item);
}

void
MyGuiImageBox::setItem(std::size_t index, const Rect& item)
{
	_impl.setItem(index, item);
}

void
MyGuiImageBox::deleteAllItems()
{
	_impl.deleteAllItems();
}

void
MyGuiImageBox::addItemFrame(std::size_t index, const Rect& item)
{
	_impl.addItemFrame(index, item);
}

void
MyGuiImageBox::insertItemFrame(std::size_t index, std::size_t indexFrame, const Rect& item)
{
	_impl.insertItemFrame(index, indexFrame, item);
}

void
MyGuiImageBox::addItemFrameDuplicate(std::size_t index, std::size_t indexSourceFrame)
{
	_impl.addItemFrameDuplicate(index, indexSourceFrame);
}

void
MyGuiImageBox::insertItemFrameDuplicate(std::size_t index, std::size_t indexFrame, std::size_t indexSourceFrame)
{
	_impl.insertItemFrameDuplicate(index, indexFrame, indexSourceFrame);
}

void
MyGuiImageBox::setItemFrame(std::size_t index, std::size_t indexFrame, const Rect& item)
{
	_impl.setItemFrame(index, indexFrame, item);
}

void
MyGuiImageBox::deleteItemFrame(std::size_t index, std::size_t indexFrame)
{
	_impl.deleteItemFrame(index, indexFrame);
}

void
MyGuiImageBox::deleteAllItemFrames(std::size_t index)
{
	_impl.deleteAllItemFrames(index);
}

void
MyGuiImageBox::setItemFrameRate(std::size_t index, float rate)
{
	_impl.setItemFrameRate(index, rate);
}

float
MyGuiImageBox::getItemFrameRate(std::size_t index)
{
	return _impl.getItemFrameRate(index);
}

bool
MyGuiImageBox::setItemResource(const std::string& name)
{
	return _impl.setItemResource(name);
}

void
MyGuiImageBox::setItemGroup(const std::string& value)
{
	_impl.setItemGroup(value);
}

void
MyGuiImageBox::setItemName(const std::string& value)
{
	_impl.setItemName(value);
}

_NAME_END

#endif