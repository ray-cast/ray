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
#include <ray/gui_scrollbar_component.h>
#include <ray/gui_scrollbar.h>
#include <ray/gui_system.h>

_NAME_BEGIN

__ImplementSubClass(GuiScrollBarComponent, GuiWidgetComponent, "GuiScrollBar")

GuiScrollBarComponent::GuiScrollBarComponent() noexcept
{
	_scrollBar = GuiSystem::instance()->createWidget<GuiScrollBar>();
	_scrollBar->create();

	this->setGuiWidget(_scrollBar);
}

GuiScrollBarComponent::~GuiScrollBarComponent() noexcept
{
}

void
GuiScrollBarComponent::setVerticalAlignment(bool value) noexcept
{
    _scrollBar->setVerticalAlignment(value);
}

bool
GuiScrollBarComponent::getVerticalAlignment() const noexcept
{
    return _scrollBar->getVerticalAlignment();
}

void
GuiScrollBarComponent::setScrollRange(std::size_t value) noexcept
{
    _scrollBar->setScrollRange(value);
}

std::size_t
GuiScrollBarComponent::getScrollRange() const noexcept
{
    return _scrollBar->getScrollRange();
}

void
GuiScrollBarComponent::setScrollPosition(std::size_t value) noexcept
{
    _scrollBar->setScrollPosition(value);
}

std::size_t
GuiScrollBarComponent::getScrollPosition() const noexcept
{
    return _scrollBar->getScrollPosition();
}

void
GuiScrollBarComponent::setScrollPage(std::size_t value) noexcept
{
    _scrollBar->setScrollPage(value);
}

std::size_t
GuiScrollBarComponent::getScrollPage() const noexcept
{
    return _scrollBar->getScrollPage();
}

void
GuiScrollBarComponent::setScrollViewPage(std::size_t value) noexcept
{
    _scrollBar->setScrollViewPage(value);
}

std::size_t
GuiScrollBarComponent::getScrollViewPage() const noexcept
{
    return _scrollBar->getScrollViewPage();
}

void
GuiScrollBarComponent::setScrollWheelPage(std::size_t value) noexcept
{
    _scrollBar->setScrollWheelPage(value);
}

std::size_t
GuiScrollBarComponent::getScrollWheelPage() const noexcept
{
    return _scrollBar->getScrollWheelPage();
}

int
GuiScrollBarComponent::getLineSize() const noexcept
{
    return _scrollBar->getLineSize();
}

void
GuiScrollBarComponent::setTrackSize(int value) noexcept
{
    _scrollBar->setTrackSize(value);
}

int
GuiScrollBarComponent::getTrackSize() const noexcept
{
    return _scrollBar->getTrackSize();
}

void
GuiScrollBarComponent::setMinTrackSize(int value) noexcept
{
    _scrollBar->setMinTrackSize(value);
}

int
GuiScrollBarComponent::getMinTrackSize() const noexcept
{
    return _scrollBar->getMinTrackSize();
}

void
GuiScrollBarComponent::setMoveToClick(bool value) noexcept
{
    _scrollBar->setMoveToClick(value);
}

bool
GuiScrollBarComponent::getMoveToClick() const noexcept
{
    return _scrollBar->getMoveToClick();
}

void
GuiScrollBarComponent::setRepeatEnabled(bool enabled) noexcept
{
    _scrollBar->setRepeatEnabled(enabled);
}

bool
GuiScrollBarComponent::getRepeatEnabled() const noexcept
{
    return _scrollBar->getRepeatEnabled();
}

void
GuiScrollBarComponent::setRepeatTriggerTime(float time) noexcept
{
    _scrollBar->setRepeatTriggerTime(time);
}

float
GuiScrollBarComponent::getRepeatTriggerTime(float time) const noexcept
{
    return _scrollBar->getRepeatTriggerTime(time);
}

void
GuiScrollBarComponent::setRepeatStepTime(float time) noexcept
{
    _scrollBar->setRepeatStepTime(time);
}

float
GuiScrollBarComponent::getRepeatStepTime(float time) const noexcept
{
    return _scrollBar->getRepeatStepTime(time);
}

void
GuiScrollBarComponent::addScrollPositionChange(std::function<void()>& func) noexcept
{
	assert(_scrollBar);
	_scrollBar->addScrollPositionChange(func);
}

void
GuiScrollBarComponent::removeScrollPositionChange(std::function<void()>& func) noexcept
{
	assert(_scrollBar);
	_scrollBar->removeScrollPositionChange(func);
}

void
GuiScrollBarComponent::load(iarchive& reader) noexcept
{
	GuiWidgetComponent::load(reader);
}

void
GuiScrollBarComponent::save(oarchive& write) noexcept
{
	GuiWidgetComponent::save(write);
}

GameComponentPtr
GuiScrollBarComponent::clone() const except
{
	return std::make_shared<GuiScrollBarComponent>();
}

_NAME_END