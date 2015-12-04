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
#ifndef _H_GUI_SCROLLBAR_H_
#define _H_GUI_SCROLLBAR_H_

#include <ray/gui_widget.h>

_NAME_BEGIN

class EXPORT GuiScrollBar : public GuiWidget
{
	__DeclareSubInterface(GuiScrollBar, GuiWidget)
public:
	GuiScrollBar(GuiWidgetImpl& impl) noexcept;
	virtual ~GuiScrollBar() noexcept;

	virtual void setVerticalAlignment(bool value) noexcept = 0;
	virtual bool getVerticalAlignment() const noexcept = 0;

	virtual void setScrollRange(std::size_t value) noexcept = 0;
	virtual std::size_t getScrollRange() const noexcept = 0;

	virtual void setScrollPosition(std::size_t value) noexcept = 0;
	virtual std::size_t getScrollPosition() const noexcept = 0;

	virtual void setScrollPage(std::size_t value) noexcept = 0;
	virtual std::size_t getScrollPage() const noexcept = 0;

	virtual void setScrollViewPage(std::size_t value) noexcept = 0;
	virtual std::size_t getScrollViewPage() const noexcept = 0;

	virtual void setScrollWheelPage(std::size_t value) noexcept = 0;
	virtual std::size_t getScrollWheelPage() const noexcept = 0;

	virtual int getLineSize() const noexcept = 0;

	virtual void setTrackSize(int value) noexcept = 0;
	virtual int getTrackSize() const noexcept = 0;

	virtual void setMinTrackSize(int value) noexcept = 0;
	virtual int getMinTrackSize() const noexcept = 0;

	virtual void setMoveToClick(bool value) noexcept = 0;
	virtual bool getMoveToClick() const noexcept = 0;

	virtual void setRepeatEnabled(bool enabled) noexcept = 0;
	virtual bool getRepeatEnabled() const noexcept = 0;

	virtual void setRepeatTriggerTime(float time) noexcept = 0;
	virtual float getRepeatTriggerTime(float time) const noexcept = 0;

	virtual void setRepeatStepTime(float time) noexcept = 0;
	virtual float getRepeatStepTime(float time) const noexcept = 0;

	virtual void addScrollPositionChange(std::function<void()>& func) noexcept = 0;
	virtual void removeScrollPositionChange(std::function<void()>& func) noexcept = 0;

private:
	GuiScrollBar(const GuiScrollBar&) noexcept = delete;
	GuiScrollBar& operator=(const GuiScrollBar&) noexcept = delete;
};

_NAME_END

#endif