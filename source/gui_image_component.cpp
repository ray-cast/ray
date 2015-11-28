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
#if defined(_BUILD_GUI)
#include <ray/gui_image_component.h>
#include <ray/gui_system.h>

_NAME_BEGIN

__ImplementSubClass(GuiImageComponent, GuiWidgetComponent, "GuiImage")

GuiImageComponent::GuiImageComponent() noexcept
{
	_imagebox = GuiSystem::instance()->createWidget<GuiImageBox>();
	_imagebox->create();
}

GuiImageComponent::~GuiImageComponent() noexcept
{
}

void
GuiImageComponent::setImageInfo(const std::string& texture, const Viewport& coord, const Size& tile)
{
    assert(_imagebox);
    _imagebox->setImageInfo(texture, coord, tile);
}

void
GuiImageComponent::setImageTexture(const std::string& value)
{
    assert(_imagebox);
    _imagebox->setImageTexture(value);
}

void
GuiImageComponent::setImageRect(const Viewport& value)
{
    assert(_imagebox);
    _imagebox->setImageRect(value);
}

void
GuiImageComponent::setImageCoord(const Viewport& value)
{
    assert(_imagebox);
    _imagebox->setImageCoord(value);
}

void
GuiImageComponent::setImageTile(const int2& value)
{
    assert(_imagebox);
    _imagebox->setImageTile(value);
}

void
GuiImageComponent::setImageIndex(std::size_t index)
{
    assert(_imagebox);
    _imagebox->setImageIndex(index);
}

std::size_t
GuiImageComponent::getImageIndex() const
{
    assert(_imagebox);
    return _imagebox->getImageIndex();
}

Size
GuiImageComponent::getImageSize() const
{
    assert(_imagebox);
    return _imagebox->getImageSize();
}

std::size_t
GuiImageComponent::getItemCount() const
{
    assert(_imagebox);
    return _imagebox->getItemCount();
}

void
GuiImageComponent::setItemSelect(std::size_t index)
{
    assert(_imagebox);
    _imagebox->setItemSelect(index);
}

std::size_t
GuiImageComponent::getItemSelect() const
{
    assert(_imagebox);
    return _imagebox->getItemSelect();
}

void
GuiImageComponent::resetItemSelect()
{
    assert(_imagebox);
    _imagebox->resetItemSelect();
}

void
GuiImageComponent::insertItem(std::size_t index, const Viewport& item)
{
    assert(_imagebox);
    _imagebox->insertItem(index, item);
}

void
GuiImageComponent::addItem(const Viewport& item)
{
    assert(_imagebox);
    _imagebox->addItem(item);
}

void
GuiImageComponent::setItem(std::size_t index, const Viewport& item)
{
    assert(_imagebox);
    _imagebox->setItem(index, item);
}

void
GuiImageComponent::deleteAllItems()
{
    assert(_imagebox);
    _imagebox->deleteAllItems();
}

void
GuiImageComponent::addItemFrame(std::size_t index, const Viewport& item)
{
    assert(_imagebox);
    _imagebox->addItemFrame(index, item);
}

void
GuiImageComponent::insertItemFrame(std::size_t index, std::size_t indexFrame, const Viewport& item)
{
    assert(_imagebox);
    _imagebox->insertItemFrame(index, indexFrame, item);
}

void
GuiImageComponent::addItemFrameDuplicate(std::size_t index, std::size_t indexSourceFrame)
{
    assert(_imagebox);
    _imagebox->addItemFrameDuplicate(index, indexSourceFrame);
}

void
GuiImageComponent::insertItemFrameDuplicate(std::size_t index, std::size_t indexFrame, std::size_t indexSourceFrame)
{
    assert(_imagebox);
    _imagebox->insertItemFrameDuplicate(index, indexFrame, indexSourceFrame);
}

void
GuiImageComponent::setItemFrame(std::size_t index, std::size_t indexFrame, const Viewport& item)
{
    assert(_imagebox);
    _imagebox->setItemFrame(index, indexFrame, item);
}

void
GuiImageComponent::deleteItemFrame(std::size_t index, std::size_t indexFrame)
{
    assert(_imagebox);
    _imagebox->deleteItemFrame(index, indexFrame);
}

void
GuiImageComponent::deleteAllItemFrames(std::size_t index)
{
    assert(_imagebox);
    _imagebox->deleteAllItemFrames(index);
}

void
GuiImageComponent::setItemFrameRate(std::size_t index, float rate)
{
    assert(_imagebox);
    _imagebox->setItemFrameRate(index, rate);
}

float
GuiImageComponent::getItemFrameRate(std::size_t index)
{
    assert(_imagebox);
    return _imagebox->getItemFrameRate(index);
}

bool
GuiImageComponent::setItemResource(const std::string& name)
{
    assert(_imagebox);
    return _imagebox->setItemResource(name);
}

void
GuiImageComponent::setItemGroup(const std::string& value)
{
    assert(_imagebox);
    _imagebox->setItemGroup(value);
}

void
GuiImageComponent::setItemName(const std::string& value)
{
    assert(_imagebox);
    _imagebox->setItemName(value);
}

void
GuiImageComponent::load(iarchive& reader) noexcept
{
	GuiWidgetComponent::load(reader);

	std::string texture;
	reader >> make_archive(texture, "texture");

	this->setImageTexture(texture);
}

void
GuiImageComponent::save(oarchive& write) noexcept
{
	GuiWidgetComponent::save(write);
}

GameComponentPtr
GuiImageComponent::clone() const except
{
	return std::make_shared<GuiImageComponent>();
}

void
GuiImageComponent::setGuiWidget(GuiWidgetPtr widget) noexcept
{
	_imagebox = widget->downcast<GuiImageBox>();
}

GuiWidgetPtr
GuiImageComponent::getGuiWidget() const noexcept
{
	return _imagebox;
}

_NAME_END

#endif