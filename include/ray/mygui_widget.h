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
#ifndef _H_MY_GUI_WIDGET_H_
#define _H_MY_GUI_WIDGET_H_

#include <ray/mygui_types.h>

_NAME_BEGIN

class MyGuiWidget : public GuiWidgetImpl
{
	__DeclareSubInterface(MyGuiWidget, GuiWidget)
public:
	MyGuiWidget() noexcept;
	virtual ~MyGuiWidget() noexcept;

	virtual void destroy() noexcept;

	virtual void setParent(GuiWidgetImpl* parent) noexcept;
	virtual GuiWidgetImpl* getParent() const noexcept;

	virtual void setAlign(GuiWidgetAlign align) noexcept;
	virtual GuiWidgetAlign getAlign() noexcept;

	virtual void changeWidgetSkin(const std::string& skin) noexcept;

	virtual void setViewport(const Rect& view) noexcept;
	virtual void getViewport(Rect& view) const noexcept;

	void addKeySetFocus(binder<void()>& func) noexcept;
	void addKeyLostFocus(binder<void()>& func) noexcept;
	void addKeyButtonPressed(binder<void()>& func) noexcept;
	void addKeyButtonReleased(binder<void()>& func) noexcept;

	void addMouseSetFocusListener(binder<void()>& func) noexcept;
	void addMouseLostFocusListener(binder<void()>& func) noexcept;
	void addMouseButtonPressedListener(binder<void()>& func) noexcept;
	void addMouseButtonReleasedListener(binder<void()>& func) noexcept;
	void addMouseButtonClickListener(binder<void()>& func) noexcept;
	void addMouseButtonDoubleClickListener(binder<void()>& func) noexcept;

	void removeKeySetFocus(binder<void()>& func) noexcept;
	void removeKeyLostFocus(binder<void()>& func) noexcept;
	void removeKeyButtonPressed(binder<void()>& func) noexcept;
	void removeKeyButtonReleased(binder<void()>& func) noexcept;

	void removeMouseSetFocusListener(binder<void()>& func) noexcept;
	void removeMouseLostFocusListener(binder<void()>& func) noexcept;
	void removeMouseButtonPressedListener(binder<void()>& func) noexcept;
	void removeMouseButtonReleasedListener(binder<void()>& func) noexcept;
	void removeMouseButtonClickListener(binder<void()>& func) noexcept;
	void removeMouseButtonDoubleClickListener(binder<void()>& func) noexcept;

private:
	void onKeySetFocus(MyGUI::Widget*, MyGUI::Widget*);
	void onKeyLostFocus(MyGUI::Widget*, MyGUI::Widget*);
	void onKeyButtonPressed(MyGUI::Widget* _sender, MyGUI::KeyCode _key, MyGUI::Char _char);
	void onKeyButtonReleased(MyGUI::Widget* _sender, MyGUI::KeyCode _key);

	void onMouseSetFocus(MyGUI::Widget*, MyGUI::Widget*);
	void onMouseLostFocus(MyGUI::Widget*, MyGUI::Widget*);
	void onMouseButtonPressed(MyGUI::Widget*, int x, int y, MyGUI::MouseButton button);
	void onMouseButtonReleased(MyGUI::Widget*, int x, int y, MyGUI::MouseButton button);
	void onMouseButtonClick(MyGUI::Widget* _sender);
	void onMouseButtonDoubleClick(MyGUI::Widget* _sender);

protected:
	void setWidget(MyGUI::Widget* widget) noexcept;
	MyGUI::Widget* getWidget() const noexcept;

protected:

	static MyGUI::IntPoint convert(const Point& pt) noexcept
	{
		return MyGUI::IntPoint(pt.x, pt.y);
	}

	static MyGUI::IntSize convert(const Size& sz) noexcept
	{
		return MyGUI::IntSize(sz.x, sz.y);
	}

	static MyGUI::IntCoord convert(const Rect& rect) noexcept
	{
		return MyGUI::IntCoord(rect.x, rect.y, rect.w, rect.h);
	}

	static MyGUI::Colour convert(const float4& color) noexcept
	{
		return MyGUI::Colour(color.x, color.y, color.z, color.w);
	}

	static Point convert(const MyGUI::IntPoint& pt) noexcept
	{
		return Point(pt.left, pt.top);
	}

	static Size convert(const MyGUI::IntSize& sz) noexcept
	{
		return Size(sz.width, sz.height);
	}

	static Rect convert(const MyGUI::IntCoord& rect) noexcept
	{
		return Rect(rect.left, rect.top, rect.width, rect.height);
	}

	static float4 convert(const MyGUI::Colour& color) noexcept
	{
		return float4(color.red, color.green, color.blue, color.alpha);
	}

	static MyGUI::FlowDirection convert(GuiFlowDirection flow) noexcept;
	static GuiFlowDirection convert(MyGUI::FlowDirection flow) noexcept;

	static MyGUI::Align GuiAlignToMyGui(GuiWidgetAlign align) noexcept;
	static GuiWidgetAlign MyGuiToGuiAlign(MyGUI::Align align) noexcept;

private:
	MyGuiWidget(const MyGuiWidget&) noexcept = delete;
	MyGuiWidget& operator=(const MyGuiWidget&) noexcept = delete;

private:
	GuiWidgetImpl* _parent;
	MyGUI::Widget* _widget;

	delegate<void()> _onKeySetFocus;
	delegate<void()> _onKeyLostFocus;
	delegate<void()> _onKeyButtonPressed;
	delegate<void()> _onKeyButtonReleased;

	delegate<void()> _onMouseSetFocus;
	delegate<void()> _onMouseLostFocus;
	delegate<void()> _onMouseButtonPressed;
	delegate<void()> _onMouseButtonReleased;
	delegate<void()> _onMouseButtonClick;
	delegate<void()> _onMouseButtonDoubleClick;
};

_NAME_END

#endif