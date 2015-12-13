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
#include "mygui_widget.h"

_NAME_BEGIN

__ImplementSubInterface(MyGuiWidget, GuiWidget, "MyGuiWidget")

MyGuiWidget::MyGuiWidget() noexcept
	: _widget(nullptr)
{
}

MyGuiWidget::~MyGuiWidget() noexcept
{
}

void
MyGuiWidget::destroy() noexcept
{
	if (_widget)
	{
		MyGUI::Gui::getInstance().destroyWidget(_widget);
		_widget = nullptr;
	}
}

void
MyGuiWidget::setParent(GuiWidgetImpl* parent) noexcept
{
	assert(_widget);
	if (_parent != parent)
	{
		if (parent)
		{
			auto widget = dynamic_cast<MyGuiWidget*>(parent)->getWidget();
			if (widget)
				_widget->attachToWidget(widget);
		}
		_parent = parent;
	}
}

GuiWidgetImpl*
MyGuiWidget::getParent() const noexcept
{
	return _parent;
}

void
MyGuiWidget::setAlign(GuiWidgetAlign align) noexcept
{
	assert(_widget);
	_widget->setAlign(GuiAlignToMyGui(align));
}

GuiWidgetAlign
MyGuiWidget::getAlign() noexcept
{
	return MyGuiToGuiAlign(_widget->getAlign());
}

void
MyGuiWidget::changeWidgetSkin(const std::string& skin) noexcept
{
	assert(_widget);
	_widget->changeWidgetSkin(skin);
}

void
MyGuiWidget::setViewport(const Rect& view) noexcept
{
	assert(_widget);
	_widget->setCoord(convert(view));
}

void
MyGuiWidget::getViewport(Rect& rect) const noexcept
{
	assert(_widget);
	rect = convert(_widget->getCoord());
}

void
MyGuiWidget::addKeySetFocus(std::function<void()>& func) noexcept
{
	assert(!_onKeySetFocus.find(func));
	if (_onKeySetFocus.empty())
		_widget->eventKeySetFocus += MyGUI::newDelegate(this, &MyGuiWidget::onKeySetFocus);
	_onKeySetFocus.attach(func);
}

void
MyGuiWidget::addKeyLostFocus(std::function<void()>& func) noexcept
{
	assert(!_onKeyLostFocus.find(func));
	if (_onKeyLostFocus.empty())
		_widget->eventKeyLostFocus += MyGUI::newDelegate(this, &MyGuiWidget::onKeyLostFocus);
	_onKeyLostFocus.attach(func);
}

void
MyGuiWidget::addKeyButtonPressed(std::function<void()>& func) noexcept
{
	assert(!_onKeyButtonPressed.find(func));
	if (_onKeyButtonPressed.empty())
		_widget->eventKeyButtonPressed += MyGUI::newDelegate(this, &MyGuiWidget::onKeyButtonPressed);
	_onKeyButtonPressed.attach(func);
}

void
MyGuiWidget::addKeyButtonReleased(std::function<void()>& func) noexcept
{
	assert(!_onKeyButtonReleased.find(func));
	if (_onKeyButtonReleased.empty())
		_widget->eventKeyButtonReleased += MyGUI::newDelegate(this, &MyGuiWidget::onKeyButtonReleased);
	_onKeyButtonReleased.attach(func);
}

void
MyGuiWidget::addMouseMove(std::function<void()>& func) noexcept
{
	assert(!_onMouseMove.find(func));
	if (_onMouseMove.empty())
		_widget->eventMouseMove += MyGUI::newDelegate(this, &MyGuiWidget::onMouseMove);
	_onMouseMove.attach(func);
}

void
MyGuiWidget::addMouseDrag(std::function<void()>& func) noexcept
{
	assert(!_onMouseDrag.find(func));
	if (_onMouseDrag.empty())
		_widget->eventMouseDrag += MyGUI::newDelegate(this, &MyGuiWidget::onMouseDrag);
	_onMouseDrag.attach(func);
}

void
MyGuiWidget::addMouseWheel(std::function<void()>& func) noexcept
{
	assert(!_onMouseWheel.find(func));
	if (_onMouseWheel.empty())
		_widget->eventMouseWheel += MyGUI::newDelegate(this, &MyGuiWidget::onMouseWheel);
	_onMouseWheel.attach(func);
}

void
MyGuiWidget::addMouseLostFocusListener(std::function<void()>& func) noexcept
{
	assert(!_onMouseLostFocus.find(func));
	if (_onMouseLostFocus.empty())
		_widget->eventMouseLostFocus += MyGUI::newDelegate(this, &MyGuiWidget::onMouseLostFocus);
	_onMouseLostFocus.attach(func);
}

void
MyGuiWidget::addMouseSetFocusListener(std::function<void()>& func) noexcept
{
	assert(!_onMouseSetFocus.find(func));
	if (_onMouseSetFocus.empty())
		_widget->eventMouseSetFocus += MyGUI::newDelegate(this, &MyGuiWidget::onMouseSetFocus);
	_onMouseSetFocus.attach(func);
}

void
MyGuiWidget::addMouseButtonPressedListener(std::function<void()>& func) noexcept
{
	assert(!_onMouseButtonPressed.find(func));
	if (_onMouseButtonPressed.empty())
		_widget->eventMouseButtonPressed += MyGUI::newDelegate(this, &MyGuiWidget::onMouseButtonPressed);
	_onMouseButtonPressed.attach(func);
}

void
MyGuiWidget::addMouseButtonReleasedListener(std::function<void()>& func) noexcept
{
	assert(!_onMouseButtonPressed.find(func));
	if (_onMouseButtonPressed.empty())
		_widget->eventMouseButtonReleased += MyGUI::newDelegate(this, &MyGuiWidget::onMouseButtonReleased);
	_onMouseButtonReleased.attach(func);
}

void
MyGuiWidget::addMouseButtonClickListener(std::function<void()>& func) noexcept
{
	assert(!_onMouseButtonClick.find(func));
	if (_onMouseButtonClick.empty())
		_widget->eventMouseButtonClick += MyGUI::newDelegate(this, &MyGuiWidget::onMouseButtonClick);
	_onMouseButtonClick.attach(func);
}

void
MyGuiWidget::addMouseButtonDoubleClickListener(std::function<void()>& func) noexcept
{
	assert(!_onMouseButtonDoubleClick.find(func));
	if (_onMouseButtonDoubleClick.empty())
		_widget->eventMouseButtonDoubleClick += MyGUI::newDelegate(this, &MyGuiWidget::onMouseButtonDoubleClick);
	_onMouseButtonDoubleClick.attach(func);
}

void
MyGuiWidget::removeKeySetFocus(std::function<void()>& func) noexcept
{
	assert(_onKeySetFocus.find(func));
	_onKeySetFocus.remove(func);
}

void
MyGuiWidget::removeKeyLostFocus(std::function<void()>& func) noexcept
{
	assert(_onKeyLostFocus.find(func));
	_onKeyLostFocus.remove(func);
}

void
MyGuiWidget::removeKeyButtonPressed(std::function<void()>& func) noexcept
{
	assert(_onKeyButtonPressed.find(func));
	_onKeyButtonPressed.remove(func);
}

void
MyGuiWidget::removeKeyButtonReleased(std::function<void()>& func) noexcept
{
	assert(_onKeyButtonReleased.find(func));
	_onKeyButtonReleased.remove(func);
}

void
MyGuiWidget::removeMouseMove(std::function<void()>& func) noexcept
{
	assert(_onMouseMove.find(func));
	_onMouseMove.remove(func);
}

void
MyGuiWidget::removeMouseDrag(std::function<void()>& func) noexcept
{
	assert(_onMouseDrag.find(func));
	_onMouseDrag.remove(func);
}

void
MyGuiWidget::removeMouseWheel(std::function<void()>& func) noexcept
{
	assert(_onMouseWheel.find(func));
	_onMouseWheel.remove(func);
}

void
MyGuiWidget::removeMouseSetFocusListener(std::function<void()>& func) noexcept
{
	assert(_onMouseSetFocus.find(func));
	_onMouseSetFocus.remove(func);
}

void
MyGuiWidget::removeMouseLostFocusListener(std::function<void()>& func) noexcept
{
	assert(_onMouseLostFocus.find(func));
	_onMouseLostFocus.remove(func);
}

void
MyGuiWidget::removeMouseButtonPressedListener(std::function<void()>& func) noexcept
{
	assert(_onMouseButtonPressed.find(func));
	_onMouseButtonPressed.remove(func);
}

void
MyGuiWidget::removeMouseButtonReleasedListener(std::function<void()>& func) noexcept
{
	assert(_onMouseButtonReleased.find(func));
	_onMouseButtonReleased.remove(func);
}

void
MyGuiWidget::removeMouseButtonClickListener(std::function<void()>& func) noexcept
{
	assert(_onMouseButtonClick.find(func));
	_onMouseButtonClick.remove(func);
}

void
MyGuiWidget::removeMouseButtonDoubleClickListener(std::function<void()>& func) noexcept
{
	assert(_onMouseButtonDoubleClick.find(func));
	_onMouseButtonDoubleClick.remove(func);
}

void
MyGuiWidget::onKeySetFocus(MyGUI::Widget*, MyGUI::Widget*)
{
	_onKeySetFocus();
}

void
MyGuiWidget::onKeyLostFocus(MyGUI::Widget*, MyGUI::Widget*)
{
	_onKeyLostFocus();
}

void
MyGuiWidget::onMouseLostFocus(MyGUI::Widget*, MyGUI::Widget*)
{
	_onMouseLostFocus.run();
}

void
MyGuiWidget::onMouseMove(MyGUI::Widget*, int, int) noexcept
{
	_onMouseMove.run();
}

void
MyGuiWidget::onMouseDrag(MyGUI::Widget*, int, int, MyGUI::MouseButton) noexcept
{
	_onMouseDrag.run();
}

void
MyGuiWidget::onMouseWheel(MyGUI::Widget*, int) noexcept
{
	_onMouseWheel.run();
}

void
MyGuiWidget::onMouseSetFocus(MyGUI::Widget*, MyGUI::Widget*)
{
	_onMouseSetFocus.run();
}

void
MyGuiWidget::onKeyButtonPressed(MyGUI::Widget* _sender, MyGUI::KeyCode _key, MyGUI::Char _char)
{
	_onKeyButtonPressed.run();
}

void
MyGuiWidget::onKeyButtonReleased(MyGUI::Widget* _sender, MyGUI::KeyCode _key)
{
	_onKeyButtonReleased.run();
}

void
MyGuiWidget::onMouseButtonPressed(MyGUI::Widget*, int x, int y, MyGUI::MouseButton button)
{
	_onMouseButtonPressed.run();
}

void
MyGuiWidget::onMouseButtonReleased(MyGUI::Widget*, int x, int y, MyGUI::MouseButton button)
{
	_onMouseButtonReleased.run();
}

void
MyGuiWidget::onMouseButtonClick(MyGUI::Widget* _sender)
{
	_onMouseButtonClick.run();
}

void
MyGuiWidget::onMouseButtonDoubleClick(MyGUI::Widget* _sender)
{
	_onMouseButtonDoubleClick.run();
}

void
MyGuiWidget::setWidget(MyGUI::Widget* widget) noexcept
{
	assert(widget);
	assert(_widget == nullptr);
	_widget = widget;
}

MyGUI::Widget*
MyGuiWidget::getWidget() const noexcept
{
	return _widget;
}

MyGUI::FlowDirection
MyGuiWidget::convert(GuiFlowDirection flow) noexcept
{
	switch (flow.getValue())
	{
	case GuiFlowDirection::TopToBottom:
		return MyGUI::FlowDirection::TopToBottom;
	case GuiFlowDirection::BottomToTop:
		return MyGUI::FlowDirection::BottomToTop;
	case GuiFlowDirection::LeftToRight:
		return MyGUI::FlowDirection::LeftToRight;
	case GuiFlowDirection::RightToLeft:
		return MyGUI::FlowDirection::RightToLeft;
	default:
		assert(false);
		return MyGUI::FlowDirection::LeftToRight;
		break;
	}
}

GuiFlowDirection
MyGuiWidget::convert(MyGUI::FlowDirection flow) noexcept
{
	switch (flow.getValue())
	{
	case MyGUI::FlowDirection::TopToBottom:
		return GuiFlowDirection::TopToBottom;
	case MyGUI::FlowDirection::BottomToTop:
		return GuiFlowDirection::BottomToTop;
	case MyGUI::FlowDirection::LeftToRight:
		return GuiFlowDirection::LeftToRight;
	case MyGUI::FlowDirection::RightToLeft:
		return GuiFlowDirection::RightToLeft;
	default:
		assert(false);
		return GuiFlowDirection::LeftToRight;
	}
}

MyGUI::MenuItemType 
MyGuiWidget::convert(GuiMenuItemType type) noexcept
{
	switch (type.getValue())
	{
	case GuiMenuItemType::Normal:
		return MyGUI::MenuItemType::Normal;
	case GuiMenuItemType::Popup:
		return MyGUI::MenuItemType::Popup;
	case GuiMenuItemType::Separator:
		return MyGUI::MenuItemType::Separator;
	default:
		assert(false);
		return MyGUI::MenuItemType::Normal;
	}
}

GuiMenuItemType 
MyGuiWidget::convert(MyGUI::MenuItemType type) noexcept
{
	switch (type.getValue())
	{
	case MyGUI::MenuItemType::Normal:
		return GuiMenuItemType::Normal;
	case MyGUI::MenuItemType::Popup:
		return GuiMenuItemType::Popup;
	case MyGUI::MenuItemType::Separator:
		return GuiMenuItemType::Separator;
	default:
		assert(false);
		return GuiMenuItemType::Normal;
	}
}

MyGUI::Align
MyGuiWidget::GuiAlignToMyGui(GuiWidgetAlign align) noexcept
{
	switch (align.getValue())
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

GuiWidgetAlign
MyGuiWidget::MyGuiToGuiAlign(MyGUI::Align align) noexcept
{
	switch (align.getValue())
	{
	case MyGUI::Align::Left:
		return GuiWidgetAlign::Left;
	case MyGUI::Align::Right:
		return GuiWidgetAlign::Top;
	case MyGUI::Align::Top:
		return GuiWidgetAlign::Right;
	case MyGUI::Align::Bottom:
		return GuiWidgetAlign::Bottom;
	case MyGUI::Align::Center:
		return GuiWidgetAlign::Center;
	case MyGUI::Align::Default:
		return GuiWidgetAlign::Default;
	case MyGUI::Align::HStretch:
		return GuiWidgetAlign::HStretch;
	case MyGUI::Align::VStretch:
		return GuiWidgetAlign::VStretch;
	case MyGUI::Align::Stretch:
		return GuiWidgetAlign::Stretch;
	default:
		assert(false);
		return GuiWidgetAlign::Default;
	}
}

_NAME_END
#endif