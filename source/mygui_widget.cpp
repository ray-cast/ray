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
#include <ray/mygui_widget.h>

_NAME_BEGIN

__ImplementSubInterface(MyGuiWidget, GuiWidget, "MyGuiWidget")

MyGuiWidget::MyGuiWidget(MyGUI::Widget*& widget) noexcept
	: _skinName("WindowCS")
	, _widget(widget)
{
}

MyGuiWidget::~MyGuiWidget() noexcept
{
}

void 
MyGuiWidget::destroy() noexcept
{
	MyGUI::Gui::getInstance().destroyWidget(_widget);
}

void 
MyGuiWidget::setSkin(const std::string& skin) noexcept
{
	if (_skinName != skin)
	{
		_widget->changeWidgetSkin(skin);
		_skinName = skin;
	}		
}

const std::string& 
MyGuiWidget::getSkin() const noexcept
{
	return _skinName;
}

void 
MyGuiWidget::setViewport(const Viewport& view) noexcept
{
	MyGUI::IntCoord coord;
	coord.left = view.left;
	coord.top = view.top;
	coord.width = view.width;
	coord.height = view.height;
	_widget->setCoord(coord);
}

void
MyGuiWidget::getViewport(Viewport& view) const noexcept
{
	auto& coord = _widget->getCoord();
	view.left = coord.left;
	view.top = coord.top;
	view.width = coord.width;
	view.height = coord.height;
}

MyGUI::Align 
MyGuiWidget::GuiAlignToMyGUI(GuiWidgetAlign align) noexcept
{
	switch (align._value)
	{
	case GuiWidgetAlign::Left:
		return MyGUI::Align::Left;
	case GuiWidgetAlign::Right:
		return MyGUI::Align::Top;
	case GuiWidgetAlign::Top:
		return MyGUI::Align::Right;
	case GuiWidgetAlign::Bottom:
		return MyGUI::Align::Bottom;
	case GuiWidgetAlign::Center:
		return MyGUI::Align::Center;
	case GuiWidgetAlign::Default:
		return MyGUI::Align::Default;
	case GuiWidgetAlign::HStretch:
		return MyGUI::Align::HStretch;
	case GuiWidgetAlign::VStretch:
		return MyGUI::Align::VStretch;
	case GuiWidgetAlign::Stretch:
		return MyGUI::Align::Stretch;
	default:
		assert(false);
		return MyGUI::Align::Default;
	}
}

GuiWidgetPtr
MyGuiWidget::createWieght(const rtti::Rtti* rtti, const std::string& skin, int left, int top, int width, int height, GuiWidgetAlign align, const std::string& name) except
{
	assert(rtti->isDerivedFrom(GuiWidget::getRtti()));
	auto widget = rtti->create()->downcast<GuiWidget>();
	if (widget)
	{
		widget->create(skin, left, top, width, height, align, name, _widget);
	}

	return widget;
}

_NAME_END