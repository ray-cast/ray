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

	virtual void addKeySetFocus(binder<void()>& func) noexcept = 0;
	virtual void addKeyLostFocus(binder<void()>& func) noexcept = 0;
	virtual void addKeyButtonPressed(binder<void()>& func) noexcept = 0;
	virtual void addKeyButtonReleased(binder<void()>& func) noexcept = 0;

	virtual void addMouseMove(binder<void()>& func) noexcept = 0;
	virtual void addMouseDrag(binder<void()>& func) noexcept = 0;
	virtual void addMouseWheel(binder<void()>& func) noexcept = 0;

	virtual void addMouseSetFocusListener(binder<void()>& func) noexcept = 0;
	virtual void addMouseLostFocusListener(binder<void()>& func) noexcept = 0;
	virtual void addMouseButtonPressedListener(binder<void()>& func) noexcept = 0;
	virtual void addMouseButtonReleasedListener(binder<void()>& func) noexcept = 0;
	virtual void addMouseButtonClickListener(binder<void()>& func) noexcept = 0;
	virtual void addMouseButtonDoubleClickListener(binder<void()>& func) noexcept = 0;

	virtual void removeKeySetFocus(binder<void()>& func) noexcept = 0;
	virtual void removeKeyLostFocus(binder<void()>& func) noexcept = 0;
	virtual void removeKeyButtonPressed(binder<void()>& func) noexcept = 0;
	virtual void removeKeyButtonReleased(binder<void()>& func) noexcept = 0;

	virtual void removeMouseMove(binder<void()>& func) noexcept = 0;
	virtual void removeMouseDrag(binder<void()>& func) noexcept = 0;
	virtual void removeMouseWheel(binder<void()>& func) noexcept = 0;

	virtual void removeMouseSetFocusListener(binder<void()>& func) noexcept = 0;
	virtual void removeMouseLostFocusListener(binder<void()>& func) noexcept = 0;
	virtual void removeMouseButtonPressedListener(binder<void()>& func) noexcept = 0;
	virtual void removeMouseButtonReleasedListener(binder<void()>& func) noexcept = 0;
	virtual void removeMouseButtonClickListener(binder<void()>& func) noexcept = 0;
	virtual void removeMouseButtonDoubleClickListener(binder<void()>& func) noexcept = 0;
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

	void addKeySetFocus(binder<void()>& func) noexcept;
	void addKeyLostFocus(binder<void()>& func) noexcept;
	void addKeyButtonPressed(binder<void()>& func) noexcept;
	void addKeyButtonReleased(binder<void()>& func) noexcept;

	void addMouseMove(binder<void()>& func) noexcept;
	void addMouseDrag(binder<void()>& func) noexcept;
	void addMouseWheel(binder<void()>& func) noexcept;

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

	void removeMouseMove(binder<void()>& func) noexcept;
	void removeMouseDrag(binder<void()>& func) noexcept;
	void removeMouseWheel(binder<void()>& func) noexcept;

	void removeMouseSetFocusListener(binder<void()>& func) noexcept;
	void removeMouseLostFocusListener(binder<void()>& func) noexcept;
	void removeMouseButtonPressedListener(binder<void()>& func) noexcept;
	void removeMouseButtonReleasedListener(binder<void()>& func) noexcept;
	void removeMouseButtonClickListener(binder<void()>& func) noexcept;
	void removeMouseButtonDoubleClickListener(binder<void()>& func) noexcept;

private:
	GuiWidget(const GuiWidget&) noexcept = delete;
	GuiWidget& operator=(const GuiWidget&) noexcept = delete;
private:

	GuiWidgetImpl& _impl;
	GuiWidgetWeakPtr _parent;
};

_NAME_END

#endif