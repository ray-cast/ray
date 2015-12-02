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
#ifndef _H_GUI_WIDGET_H_
#define _H_GUI_WIDGET_H_

#include <ray/gui_align.h>

_NAME_BEGIN

class EXPORT GuiWidgetImpl : public rtti::Interface
{
	__DeclareSubInterface(GuiWidget, rtti::Interface)
public:
	GuiWidgetImpl() noexcept;
	virtual ~GuiWidgetImpl() noexcept;

	virtual bool create() except = 0;
	virtual void destroy() noexcept = 0;

	virtual void setParent(GuiWidgetImpl* parent) noexcept = 0;
	virtual GuiWidgetImpl* getParent() const noexcept = 0;

	virtual void setAlign(GuiWidgetAlign align) noexcept = 0;
	virtual GuiWidgetAlign getAlign() noexcept = 0;

	virtual void changeWidgetSkin(const std::string& skin) except = 0;

	virtual void setViewport(const Rect& view) except = 0;
	virtual void getViewport(Rect& view) const noexcept = 0;

	virtual void addKeySetFocus(function<void()>& func) noexcept = 0;
	virtual void addKeyLostFocus(function<void()>& func) noexcept = 0;
	virtual void addKeyButtonPressed(function<void()>& func) noexcept = 0;
	virtual void addKeyButtonReleased(function<void()>& func) noexcept = 0;

	virtual void addMouseMove(function<void()>& func) noexcept = 0;
	virtual void addMouseDrag(function<void()>& func) noexcept = 0;
	virtual void addMouseWheel(function<void()>& func) noexcept = 0;

	virtual void addMouseSetFocusListener(function<void()>& func) noexcept = 0;
	virtual void addMouseLostFocusListener(function<void()>& func) noexcept = 0;
	virtual void addMouseButtonPressedListener(function<void()>& func) noexcept = 0;
	virtual void addMouseButtonReleasedListener(function<void()>& func) noexcept = 0;
	virtual void addMouseButtonClickListener(function<void()>& func) noexcept = 0;
	virtual void addMouseButtonDoubleClickListener(function<void()>& func) noexcept = 0;

	virtual void removeKeySetFocus(function<void()>& func) noexcept = 0;
	virtual void removeKeyLostFocus(function<void()>& func) noexcept = 0;
	virtual void removeKeyButtonPressed(function<void()>& func) noexcept = 0;
	virtual void removeKeyButtonReleased(function<void()>& func) noexcept = 0;

	virtual void removeMouseMove(function<void()>& func) noexcept = 0;
	virtual void removeMouseDrag(function<void()>& func) noexcept = 0;
	virtual void removeMouseWheel(function<void()>& func) noexcept = 0;

	virtual void removeMouseSetFocusListener(function<void()>& func) noexcept = 0;
	virtual void removeMouseLostFocusListener(function<void()>& func) noexcept = 0;
	virtual void removeMouseButtonPressedListener(function<void()>& func) noexcept = 0;
	virtual void removeMouseButtonReleasedListener(function<void()>& func) noexcept = 0;
	virtual void removeMouseButtonClickListener(function<void()>& func) noexcept = 0;
	virtual void removeMouseButtonDoubleClickListener(function<void()>& func) noexcept = 0;
private:
	GuiWidgetImpl(const GuiWidgetImpl&) noexcept = delete;
	GuiWidgetImpl& operator=(const GuiWidgetImpl&) noexcept = delete;
};

class EXPORT GuiWidget : public rtti::Interface
{
	__DeclareSubInterface(GuiWidget, rtti::Interface)
public:
	GuiWidget(GuiWidgetImpl& impl) noexcept;
	virtual ~GuiWidget() noexcept;

	bool create() except;
	void destroy() noexcept;

	void setParent(GuiWidgetPtr parent) noexcept;
	GuiWidgetPtr getParent() const noexcept;

	void setAlign(GuiWidgetAlign align) noexcept;
	GuiWidgetAlign getAlign() noexcept;

	void changeWidgetSkin(const std::string& skin) except;

	void setViewport(const Rect& view) except;
	void getViewport(Rect& view) const noexcept;

	void addKeySetFocus(function<void()>& func) noexcept;
	void addKeyLostFocus(function<void()>& func) noexcept;
	void addKeyButtonPressed(function<void()>& func) noexcept;
	void addKeyButtonReleased(function<void()>& func) noexcept;

	void addMouseMove(function<void()>& func) noexcept;
	void addMouseDrag(function<void()>& func) noexcept;
	void addMouseWheel(function<void()>& func) noexcept;

	void addMouseSetFocusListener(function<void()>& func) noexcept;
	void addMouseLostFocusListener(function<void()>& func) noexcept;
	void addMouseButtonPressedListener(function<void()>& func) noexcept;
	void addMouseButtonReleasedListener(function<void()>& func) noexcept;
	void addMouseButtonClickListener(function<void()>& func) noexcept;
	void addMouseButtonDoubleClickListener(function<void()>& func) noexcept;

	void removeKeySetFocus(function<void()>& func) noexcept;
	void removeKeyLostFocus(function<void()>& func) noexcept;
	void removeKeyButtonPressed(function<void()>& func) noexcept;
	void removeKeyButtonReleased(function<void()>& func) noexcept;

	void removeMouseMove(function<void()>& func) noexcept;
	void removeMouseDrag(function<void()>& func) noexcept;
	void removeMouseWheel(function<void()>& func) noexcept;

	void removeMouseSetFocusListener(function<void()>& func) noexcept;
	void removeMouseLostFocusListener(function<void()>& func) noexcept;
	void removeMouseButtonPressedListener(function<void()>& func) noexcept;
	void removeMouseButtonReleasedListener(function<void()>& func) noexcept;
	void removeMouseButtonClickListener(function<void()>& func) noexcept;
	void removeMouseButtonDoubleClickListener(function<void()>& func) noexcept;

private:
	GuiWidget(const GuiWidget&) noexcept = delete;
	GuiWidget& operator=(const GuiWidget&) noexcept = delete;
private:

	GuiWidgetImpl& _impl;
	GuiWidgetWeakPtr _parent;
};

_NAME_END

#endif