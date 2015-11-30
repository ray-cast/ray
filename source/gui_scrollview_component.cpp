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
#include <ray/gui_scrollview_component.h>
#include <ray/gui_scrollview.h>
#include <ray/gui_system.h>

_NAME_BEGIN

__ImplementSubClass(GuiScrollViewComponent, GuiWidgetComponent, "GuiScrollView")

GuiScrollViewComponent::GuiScrollViewComponent() noexcept
{
	_scrollView = GuiSystem::instance()->createWidget<GuiScrollView>();
	_scrollView->create();
}

GuiScrollViewComponent::~GuiScrollViewComponent() noexcept
{
}

void
GuiScrollViewComponent::setVisibleVScroll(bool value)
{
    _scrollView->setVisibleVScroll(value);
}

bool
GuiScrollViewComponent::isVisibleVScroll() const
{
    return _scrollView->isVisibleVScroll();
}

void
GuiScrollViewComponent::setVisibleHScroll(bool value)
{
    _scrollView->setVisibleHScroll(value);
}

bool
GuiScrollViewComponent::isVisibleHScroll() const
{
    return _scrollView->isVisibleHScroll();
}

void
GuiScrollViewComponent::setCanvasAlign(GuiWidgetAlign value)
{
    _scrollView->setCanvasAlign(value);
}

GuiWidgetAlign
GuiScrollViewComponent::getCanvasAlign() const
{
    return _scrollView->getCanvasAlign();
}

void
GuiScrollViewComponent::setCanvasSize(const Size& value)
{
    this->setCanvasSize(value.x, value.y);
}

void
GuiScrollViewComponent::setCanvasSize(int width, int height)
{
    _scrollView->setCanvasSize(width, height);
}

Size
GuiScrollViewComponent::getCanvasSize()
{
    return _scrollView->getCanvasSize();
}

void
GuiScrollViewComponent::setViewOffset(const Point& value)
{
    _scrollView->setViewOffset(value);
}

Point
GuiScrollViewComponent::getViewOffset() const
{
    return _scrollView->getViewOffset();
}

void 
GuiScrollViewComponent::load(iarchive& reader) noexcept
{
	GuiWidgetComponent::load(reader);

	bool scrollV = false;
	bool scrollH = false;
	int2 size(0, 0);
	int2 offset(0, 0);
	std::string align;

	reader >> make_archive(scrollV, "scrollV");
	reader >> make_archive(scrollH, "scrollH");
	reader >> make_archive(offset, "offset");
	reader >> make_archive(size, "canvasSize");
	reader >> make_archive(align, "canvasAlign");

	this->setVisibleVScroll(scrollV);
	this->setVisibleHScroll(scrollH);
	this->setCanvasSize(Size(size.x, size.y));
	this->setViewOffset(Point(offset.x, offset.y));
	
	if (!align.empty())
		this->setCanvasAlign(GuiWidgetAlign::parse(align));
}

void 
GuiScrollViewComponent::save(oarchive& write) noexcept
{
	bool scrollV = this->isVisibleVScroll();
	bool scrollH = this->isVisibleHScroll();

	write << make_archive(scrollV, "scrollV");
	write << make_archive(scrollH, "scrollH");
}

GameComponentPtr 
GuiScrollViewComponent::clone() const except
{
	return std::make_shared<GuiScrollViewComponent>();
}

void 
GuiScrollViewComponent::setGuiWidget(GuiWidgetPtr widget) noexcept
{
	_scrollView = widget->downcast<GuiScrollView>();
}

GuiWidgetPtr 
GuiScrollViewComponent::getGuiWidget() const noexcept
{
	return _scrollView;
}

_NAME_END