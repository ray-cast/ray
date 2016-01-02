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
#include "mygui_scrollview.h"

_NAME_BEGIN

__ImplementSubClass(MyGuiScrollView, GuiScrollView, "MyGuiScrollView")
__ImplementSubClass(MyGuiScrollViewImpl, MyGuiWidget, "MyGuiScrollViewImpl")

MyGuiScrollViewImpl::MyGuiScrollViewImpl() noexcept
	: _destroy(true)
	, _parent(nullptr)
	, _scrollView(nullptr)
{
}

MyGuiScrollViewImpl::MyGuiScrollViewImpl(MyGUI::ScrollView* self, bool destroy) noexcept
	: _destroy(false)
	, _parent(nullptr)
	, _scrollView(self)
{
	this->setWidget(self);
}

MyGuiScrollViewImpl::~MyGuiScrollViewImpl() noexcept
{
	this->destroy();
}

bool
MyGuiScrollViewImpl::create() except
{
	assert(!_scrollView);
	if (_parent)
		_scrollView = _parent->createWidget<MyGUI::ScrollView>("", 0, 0, 0, 0, MyGUI::Align::Default, "");
	else
		_scrollView = MyGUI::Gui::getInstance().createWidget<MyGUI::ScrollView>("", 0, 0, 0, 0, MyGUI::Align::Default, "Main", "");

	this->setWidget(_scrollView);

	return _scrollView ? true : false;
}

void
MyGuiScrollViewImpl::destroy() noexcept
{
	if (_destroy)
	{
		if (_scrollView)
		{
			MyGUI::Gui::getInstance().destroyWidget(_scrollView);
			_scrollView = nullptr;
		}
	}
}

void
MyGuiScrollViewImpl::setVisibleVScroll(bool value) noexcept
{
	_scrollView->setVisibleVScroll(value);
}

bool
MyGuiScrollViewImpl::isVisibleVScroll() const noexcept
{
	return _scrollView->isVisibleVScroll();
}

void
MyGuiScrollViewImpl::setVisibleHScroll(bool value) noexcept
{
	_scrollView->setVisibleHScroll(value);
}

bool
MyGuiScrollViewImpl::isVisibleHScroll() const noexcept
{
	return _scrollView->isVisibleHScroll();
}

void
MyGuiScrollViewImpl::setCanvasAlign(GuiWidgetAlign value) noexcept
{
	_scrollView->setCanvasAlign(GuiAlignToMyGui(value));
}

GuiWidgetAlign
MyGuiScrollViewImpl::getCanvasAlign() const noexcept
{
	return MyGuiToGuiAlign(_scrollView->getCanvasAlign());
}

void
MyGuiScrollViewImpl::setCanvasSize(const Size& value) noexcept
{
	this->setCanvasSize(value.x, value.y);
}

void
MyGuiScrollViewImpl::setCanvasSize(int width, int height) noexcept
{
	_scrollView->setCanvasSize(width, height);
}

Size
MyGuiScrollViewImpl::getCanvasSize() noexcept
{
	return convert(_scrollView->getSize());
}

Rect
MyGuiScrollViewImpl::getViewCoord() const noexcept
{
	return convert(_scrollView->getViewCoord());
}

void
MyGuiScrollViewImpl::setViewOffset(const Point& value) noexcept
{
	_scrollView->setViewOffset(convert(value));
}

Point
MyGuiScrollViewImpl::getViewOffset() const noexcept
{
	return convert(_scrollView->getViewOffset());
}

MyGuiScrollView::MyGuiScrollView() noexcept
	: GuiScrollView(_impl)
{
}

MyGuiScrollView::MyGuiScrollView(MyGUI::ScrollView* self, bool destroy) noexcept
	: GuiScrollView(_impl)
	, _impl(self, destroy)
{
}

MyGuiScrollView::~MyGuiScrollView() noexcept
{
}

void
MyGuiScrollView::setVisibleVScroll(bool value) noexcept
{
	_impl.setVisibleVScroll(value);
}

bool
MyGuiScrollView::isVisibleVScroll() const noexcept
{
	return _impl.isVisibleVScroll();
}

void
MyGuiScrollView::setVisibleHScroll(bool value) noexcept
{
	_impl.setVisibleHScroll(value);
}

bool
MyGuiScrollView::isVisibleHScroll() const noexcept
{
	return _impl.isVisibleHScroll();
}

void
MyGuiScrollView::setCanvasAlign(GuiWidgetAlign value) noexcept
{
	_impl.setCanvasAlign(value);
}

GuiWidgetAlign
MyGuiScrollView::getCanvasAlign() const noexcept
{
	return _impl.getCanvasAlign();
}

void
MyGuiScrollView::setCanvasSize(const Size& value) noexcept
{
	this->setCanvasSize(value.x, value.y);
}

void
MyGuiScrollView::setCanvasSize(int width, int height) noexcept
{
	_impl.setCanvasSize(width, height);
}

Size
MyGuiScrollView::getCanvasSize() noexcept
{
	return _impl.getCanvasSize();
}

Rect
MyGuiScrollView::getViewCoord() const noexcept
{
	return _impl.getViewCoord();
}

void
MyGuiScrollView::setViewOffset(const Point& value) noexcept
{
	_impl.setViewOffset(value);
}

Point
MyGuiScrollView::getViewOffset() const noexcept
{
	return _impl.getViewOffset();
}

_NAME_END