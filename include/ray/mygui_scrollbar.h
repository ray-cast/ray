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
#ifndef _H_MYGUI_SCROLLBAR_H_
#define _H_MYGUI_SCROLLBAR_H_

#include <ray/mygui_widget.h>

_NAME_BEGIN

class MyGuiScrollBarImpl final : public MyGuiWidget
{
	__DeclareSubClass(MyGuiScrollBarImpl, MyGuiWidget)
public:
	MyGuiScrollBarImpl() noexcept;
	~MyGuiScrollBarImpl() noexcept;

	bool create() except;

	void setVerticalAlignment(bool value) noexcept;
	bool getVerticalAlignment() const noexcept;

	void setScrollRange(std::size_t value) noexcept;
	std::size_t getScrollRange() const noexcept;

	void setScrollPosition(std::size_t value) noexcept;
	std::size_t getScrollPosition() const noexcept;

	void setScrollPage(std::size_t value) noexcept;
	std::size_t getScrollPage() const noexcept;

	void setScrollViewPage(std::size_t value) noexcept;
	std::size_t getScrollViewPage() const noexcept;

	void setScrollWheelPage(std::size_t value) noexcept;
	std::size_t getScrollWheelPage() const noexcept;

	int getLineSize() const noexcept;

	void setTrackSize(int value) noexcept;
	int getTrackSize() const noexcept;

	void setMinTrackSize(int value) noexcept;
	int getMinTrackSize() const noexcept;

	void setMoveToClick(bool value) noexcept;
	bool getMoveToClick() const noexcept;

	void setRepeatEnabled(bool enabled) noexcept;
	bool getRepeatEnabled() const noexcept;

	void setRepeatTriggerTime(float time) noexcept;
	float getRepeatTriggerTime(float time) const noexcept;

	void setRepeatStepTime(float time) noexcept;
	float getRepeatStepTime(float time) const noexcept;

	void addScrollPositionChange(std::function<void()>& func) noexcept;
	void removeScrollPositionChange(std::function<void()>& func) noexcept;

private:
	void onScrollPositionChange(MyGUI::ScrollBar*, std::size_t) except;

private:
	MyGuiScrollBarImpl(const MyGuiScrollBarImpl&) noexcept = delete;
	MyGuiScrollBarImpl& operator=(const MyGuiScrollBarImpl&) noexcept = delete;

private:

	MyGUI::Widget* _parent;
	MyGUI::ScrollBar* _scrollBar;

	delegate<void()> _onScrollPositionChange;
};

class MyGuiScrollBar final : public GuiScrollBar
{
	__DeclareSubClass(MyGuiScrollBar, GuiScrollBar)
public:
	MyGuiScrollBar() noexcept;
	~MyGuiScrollBar() noexcept;

	void setVerticalAlignment(bool value) noexcept;
	bool getVerticalAlignment() const noexcept;

	void setScrollRange(std::size_t value) noexcept;
	std::size_t getScrollRange() const noexcept;

	void setScrollPosition(std::size_t value) noexcept;
	std::size_t getScrollPosition() const noexcept;

	void setScrollPage(std::size_t value) noexcept;
	std::size_t getScrollPage() const noexcept;

	void setScrollViewPage(std::size_t value) noexcept;
	std::size_t getScrollViewPage() const noexcept;

	void setScrollWheelPage(std::size_t value) noexcept;
	std::size_t getScrollWheelPage() const noexcept;

	int getLineSize() const noexcept;

	void setTrackSize(int value) noexcept;
	int getTrackSize() const noexcept;

	void setMinTrackSize(int value) noexcept;
	int getMinTrackSize() const noexcept;

	void setMoveToClick(bool value) noexcept;
	bool getMoveToClick() const noexcept;

	void setRepeatEnabled(bool enabled) noexcept;
	bool getRepeatEnabled() const noexcept;

	void setRepeatTriggerTime(float time) noexcept;
	float getRepeatTriggerTime(float time) const noexcept;

	void setRepeatStepTime(float time) noexcept;
	float getRepeatStepTime(float time) const noexcept;

	void addScrollPositionChange(std::function<void()>& func) noexcept;
	void removeScrollPositionChange(std::function<void()>& func) noexcept;

private:
	MyGuiScrollBar(const MyGuiScrollBar&) noexcept = delete;
	MyGuiScrollBar& operator=(const MyGuiScrollBar&) noexcept = delete;

private:

	MyGuiScrollBarImpl _impl;
};

_NAME_END

#endif