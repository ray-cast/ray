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

	virtual void addKeySetFocus(std::function<void()>& func) noexcept = 0;
	virtual void addKeyLostFocus(std::function<void()>& func) noexcept = 0;
	virtual void addKeyButtonPressed(std::function<void()>& func) noexcept = 0;
	virtual void addKeyButtonReleased(std::function<void()>& func) noexcept = 0;

	virtual void addMouseMove(std::function<void()>& func) noexcept = 0;
	virtual void addMouseDrag(std::function<void()>& func) noexcept = 0;
	virtual void addMouseWheel(std::function<void()>& func) noexcept = 0;

	virtual void addMouseSetFocusListener(std::function<void()>& func) noexcept = 0;
	virtual void addMouseLostFocusListener(std::function<void()>& func) noexcept = 0;
	virtual void addMouseButtonPressedListener(std::function<void()>& func) noexcept = 0;
	virtual void addMouseButtonReleasedListener(std::function<void()>& func) noexcept = 0;
	virtual void addMouseButtonClickListener(std::function<void()>& func) noexcept = 0;
	virtual void addMouseButtonDoubleClickListener(std::function<void()>& func) noexcept = 0;

	virtual void removeKeySetFocus(std::function<void()>& func) noexcept = 0;
	virtual void removeKeyLostFocus(std::function<void()>& func) noexcept = 0;
	virtual void removeKeyButtonPressed(std::function<void()>& func) noexcept = 0;
	virtual void removeKeyButtonReleased(std::function<void()>& func) noexcept = 0;

	virtual void removeMouseMove(std::function<void()>& func) noexcept = 0;
	virtual void removeMouseDrag(std::function<void()>& func) noexcept = 0;
	virtual void removeMouseWheel(std::function<void()>& func) noexcept = 0;

	virtual void removeMouseSetFocusListener(std::function<void()>& func) noexcept = 0;
	virtual void removeMouseLostFocusListener(std::function<void()>& func) noexcept = 0;
	virtual void removeMouseButtonPressedListener(std::function<void()>& func) noexcept = 0;
	virtual void removeMouseButtonReleasedListener(std::function<void()>& func) noexcept = 0;
	virtual void removeMouseButtonClickListener(std::function<void()>& func) noexcept = 0;
	virtual void removeMouseButtonDoubleClickListener(std::function<void()>& func) noexcept = 0;
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

	void addKeySetFocus(std::function<void()>& func) noexcept;
	void addKeyLostFocus(std::function<void()>& func) noexcept;
	void addKeyButtonPressed(std::function<void()>& func) noexcept;
	void addKeyButtonReleased(std::function<void()>& func) noexcept;

	void addMouseMove(std::function<void()>& func) noexcept;
	void addMouseDrag(std::function<void()>& func) noexcept;
	void addMouseWheel(std::function<void()>& func) noexcept;

	void addMouseSetFocusListener(std::function<void()>& func) noexcept;
	void addMouseLostFocusListener(std::function<void()>& func) noexcept;
	void addMouseButtonPressedListener(std::function<void()>& func) noexcept;
	void addMouseButtonReleasedListener(std::function<void()>& func) noexcept;
	void addMouseButtonClickListener(std::function<void()>& func) noexcept;
	void addMouseButtonDoubleClickListener(std::function<void()>& func) noexcept;

	void removeKeySetFocus(std::function<void()>& func) noexcept;
	void removeKeyLostFocus(std::function<void()>& func) noexcept;
	void removeKeyButtonPressed(std::function<void()>& func) noexcept;
	void removeKeyButtonReleased(std::function<void()>& func) noexcept;

	void removeMouseMove(std::function<void()>& func) noexcept;
	void removeMouseDrag(std::function<void()>& func) noexcept;
	void removeMouseWheel(std::function<void()>& func) noexcept;

	void removeMouseSetFocusListener(std::function<void()>& func) noexcept;
	void removeMouseLostFocusListener(std::function<void()>& func) noexcept;
	void removeMouseButtonPressedListener(std::function<void()>& func) noexcept;
	void removeMouseButtonReleasedListener(std::function<void()>& func) noexcept;
	void removeMouseButtonClickListener(std::function<void()>& func) noexcept;
	void removeMouseButtonDoubleClickListener(std::function<void()>& func) noexcept;

private:
	GuiWidget(const GuiWidget&) noexcept = delete;
	GuiWidget& operator=(const GuiWidget&) noexcept = delete;
private:

	GuiWidgetImpl& _impl;
	GuiWidgetWeakPtr _parent;
};

_NAME_END

#endif