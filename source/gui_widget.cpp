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
#include <ray/gui_widget.h>

_NAME_BEGIN

__ImplementSubInterface(GuiWidget, rtti::Interface, "GuiWidgetControl")
__ImplementSubInterface(GuiWidgetImpl, rtti::Interface, "GuiWidgetControlImpl")

GuiWidgetImpl::GuiWidgetImpl() noexcept
{
}

GuiWidgetImpl::~GuiWidgetImpl() noexcept
{
}

GuiWidget::GuiWidget(GuiWidgetImpl& impl) noexcept
	: _impl(impl)
{
}

GuiWidget::~GuiWidget() noexcept
{
}

bool
GuiWidget::create() except
{
	return _impl.create();
}

void
GuiWidget::destroy() noexcept
{
	_impl.destroy();
}

void 
GuiWidget::setParent(GuiWidgetPtr parent) noexcept
{
	if (_parent.lock() != parent)
	{
		if (parent)
			_impl.setParent(&parent->_impl);
		else
			_impl.setParent(nullptr);
		_parent = parent;
	}
}

GuiWidgetPtr
GuiWidget::getParent() const noexcept
{
	return _parent.lock();
}

void 
GuiWidget::setAlign(GuiWidgetAlign align) noexcept
{
	_impl.setAlign(align);
}

GuiWidgetAlign 
GuiWidget::getAlign() noexcept
{
	return _impl.getAlign();
}

void 
GuiWidget::changeWidgetSkin(const std::string& skin) except
{
	_impl.changeWidgetSkin(skin);
}

void 
GuiWidget::setViewport(const Rect& view) except
{
	_impl.setViewport(view);
}

void
GuiWidget::getViewport(Rect& view) const noexcept
{
	_impl.getViewport(view);
}

_NAME_END