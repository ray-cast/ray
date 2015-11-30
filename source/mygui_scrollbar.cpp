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
#include <ray/mygui_scrollbar.h>

_NAME_BEGIN

__ImplementSubClass(MyGuiScrollBar, GuiScrollBar, "MyGuiScrollBar")
__ImplementSubClass(MyGuiScrollBarImpl, MyGuiWidget, "MyGuiScrollBarImpl")

MyGuiScrollBarImpl::MyGuiScrollBarImpl() noexcept
{
}

MyGuiScrollBarImpl::~MyGuiScrollBarImpl() noexcept
{
}

bool
MyGuiScrollBarImpl::create() except
{
	assert(!_scrollBar);
	if (_parent)
		_scrollBar = _parent->createWidget<MyGUI::ScrollBar>("", 0, 0, 0, 0, MyGUI::Align::Default, "");
	else
		_scrollBar = MyGUI::Gui::getInstance().createWidget<MyGUI::ScrollBar>("", 0, 0, 0, 0, MyGUI::Align::Default, "Main", "");

	this->setWidget(_scrollBar);

	return _scrollBar ? true : false;
}

void
MyGuiScrollBarImpl::setVerticalAlignment(bool value)
{
    _scrollBar->setVerticalAlignment(value);
}

bool
MyGuiScrollBarImpl::getVerticalAlignment() const
{
    return _scrollBar->getVerticalAlignment();
}

void
MyGuiScrollBarImpl::setScrollRange(std::size_t value)
{
    _scrollBar->setScrollRange(value);
}

std::size_t
MyGuiScrollBarImpl::getScrollRange() const
{
    return _scrollBar->getScrollRange();
}

void
MyGuiScrollBarImpl::setScrollPosition(std::size_t value)
{
    _scrollBar->setScrollPosition(value);
}

std::size_t
MyGuiScrollBarImpl::getScrollPosition() const
{
    return _scrollBar->getScrollPosition();
}

void
MyGuiScrollBarImpl::setScrollPage(std::size_t value)
{
    _scrollBar->setScrollPage(value);
}

std::size_t
MyGuiScrollBarImpl::getScrollPage() const
{
    return _scrollBar->getScrollPage();
}

void
MyGuiScrollBarImpl::setScrollViewPage(std::size_t value)
{
    _scrollBar->setScrollViewPage(value);
}

std::size_t
MyGuiScrollBarImpl::getScrollViewPage() const
{
    return _scrollBar->getScrollViewPage();
}

void
MyGuiScrollBarImpl::setScrollWheelPage(std::size_t value)
{
    _scrollBar->setScrollWheelPage(value);
}

std::size_t
MyGuiScrollBarImpl::getScrollWheelPage() const
{
    return _scrollBar->getScrollWheelPage();
}

int
MyGuiScrollBarImpl::getLineSize() const
{
    return _scrollBar->getLineSize();
}

void
MyGuiScrollBarImpl::setTrackSize(int value)
{
    _scrollBar->setTrackSize(value);
}

int
MyGuiScrollBarImpl::getTrackSize() const
{
    return _scrollBar->getTrackSize();
}

void
MyGuiScrollBarImpl::setMinTrackSize(int value)
{
    _scrollBar->setMinTrackSize(value);
}

int
MyGuiScrollBarImpl::getMinTrackSize() const
{
	return _scrollBar->getMinTrackSize();
}

void
MyGuiScrollBarImpl::setMoveToClick(bool value)
{
    _scrollBar->setMoveToClick(value);
}

bool
MyGuiScrollBarImpl::getMoveToClick() const
{
	return _scrollBar->getMoveToClick();
}

void
MyGuiScrollBarImpl::setRepeatEnabled(bool enabled)
{
    _scrollBar->setRepeatEnabled(enabled);
}

bool
MyGuiScrollBarImpl::getRepeatEnabled() const
{
	return _scrollBar->getRepeatEnabled();
}

void
MyGuiScrollBarImpl::setRepeatTriggerTime(float time)
{
    _scrollBar->setRepeatTriggerTime(time);
}

float
MyGuiScrollBarImpl::getRepeatTriggerTime(float time) const
{
	return _scrollBar->getRepeatTriggerTime(time);
}

void
MyGuiScrollBarImpl::setRepeatStepTime(float time)
{
	return _scrollBar->setRepeatStepTime(time);
}

float
MyGuiScrollBarImpl::getRepeatStepTime(float time) const
{
	return _scrollBar->getRepeatStepTime(time);
}

MyGuiScrollBar::MyGuiScrollBar() noexcept
	: GuiScrollBar(_impl)
{
}

MyGuiScrollBar::~MyGuiScrollBar() noexcept
{
}

void
MyGuiScrollBar::setVerticalAlignment(bool value)
{
    _impl.setVerticalAlignment(value);
}

bool
MyGuiScrollBar::getVerticalAlignment() const
{
    return _impl.getVerticalAlignment();
}

void
MyGuiScrollBar::setScrollRange(std::size_t value)
{
    _impl.setScrollRange(value);
}

std::size_t
MyGuiScrollBar::getScrollRange() const
{
    return _impl.getScrollRange();
}

void
MyGuiScrollBar::setScrollPosition(std::size_t value)
{
    _impl.setScrollPosition(value);
}

std::size_t
MyGuiScrollBar::getScrollPosition() const
{
    return _impl.getScrollPosition();
}

void
MyGuiScrollBar::setScrollPage(std::size_t value)
{
    _impl.setScrollPage(value);
}

std::size_t
MyGuiScrollBar::getScrollPage() const
{
    return _impl.getScrollPage();
}

void
MyGuiScrollBar::setScrollViewPage(std::size_t value)
{
    _impl.setScrollViewPage(value);
}

std::size_t
MyGuiScrollBar::getScrollViewPage() const
{
    return _impl.getScrollViewPage();
}

void
MyGuiScrollBar::setScrollWheelPage(std::size_t value)
{
    _impl.setScrollWheelPage(value);
}

std::size_t
MyGuiScrollBar::getScrollWheelPage() const
{
    return _impl.getScrollWheelPage();
}

int
MyGuiScrollBar::getLineSize() const
{
    return _impl.getLineSize();
}

void
MyGuiScrollBar::setTrackSize(int value)
{
    _impl.setTrackSize(value);
}

int
MyGuiScrollBar::getTrackSize() const
{
    return _impl.getTrackSize();
}

void
MyGuiScrollBar::setMinTrackSize(int value)
{
    _impl.setMinTrackSize(value);
}

int
MyGuiScrollBar::getMinTrackSize() const
{
    return _impl.getMinTrackSize();
}

void
MyGuiScrollBar::setMoveToClick(bool value)
{
    _impl.setMoveToClick(value);
}

bool
MyGuiScrollBar::getMoveToClick() const
{
	return _impl.getMoveToClick();
}

void
MyGuiScrollBar::setRepeatEnabled(bool enabled)
{
    _impl.setRepeatEnabled(enabled);
}

bool
MyGuiScrollBar::getRepeatEnabled() const
{
	return _impl.getRepeatEnabled();
}

void
MyGuiScrollBar::setRepeatTriggerTime(float time)
{
    _impl.setRepeatTriggerTime(time);
}

float
MyGuiScrollBar::getRepeatTriggerTime(float time) const
{
	return _impl.getRepeatTriggerTime(time);
}

void
MyGuiScrollBar::setRepeatStepTime(float time)
{
    _impl.setRepeatStepTime(time);
}

float
MyGuiScrollBar::getRepeatStepTime(float time) const
{
	return _impl.getRepeatStepTime(time);
}

_NAME_END