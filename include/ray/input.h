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
#ifndef _H_INPUT_H_
#define _H_INPUT_H_

#include <ray/input_base.h>

_NAME_BEGIN

class EXPORT DefaultInput final : public Input
{
	__DeclareSubClass(DefaultInput, Input)
public:
	DefaultInput() noexcept;
	~DefaultInput() noexcept;

	virtual bool open() noexcept;
	virtual bool open(InputDevicePtr& device) noexcept;
	virtual bool open(InputDevicePtr&& device) noexcept;
	virtual void close() noexcept;

	virtual void setCaptureObject(CaptureObject window) noexcept;
	virtual CaptureObject getCaptureObject() const noexcept;

	virtual float getAxisX() const noexcept;
	virtual float getAxisY() const noexcept;

	virtual void setMousePos(int x, int y) noexcept;
	virtual void getMousePos(int& x, int& y) const noexcept;

	virtual bool getKeyDown(InputKey::Code key) const noexcept;
	virtual bool getKeyUp(InputKey::Code key) const noexcept;
	virtual bool getKey(InputKey::Code key) const noexcept;

	virtual bool getButtonDown(InputButton::Code key) const noexcept;
	virtual bool getButtonUp(InputButton::Code key) const noexcept;
	virtual bool getButton(InputButton::Code key) const noexcept;

	virtual void showCursor(bool show) noexcept;
	virtual bool isShowCursor() const noexcept;

	virtual void lockCursor(bool lock) noexcept;
	virtual bool isLockedCursor() const noexcept;

	virtual void obtainMouseCapture() noexcept;
	virtual void obtainKeyboardCapture() noexcept;

	virtual void obtainMouseCapture(InputMousePtr& mouse) noexcept;
	virtual void obtainMouseCapture(InputMousePtr&& mouse) noexcept;
	virtual void obtainKeyboardCapture(InputKeyboardPtr& key) noexcept;
	virtual void obtainKeyboardCapture(InputKeyboardPtr&& key) noexcept;
	virtual void obtainCapture() noexcept;

	virtual void releaseMouseCapture() noexcept;
	virtual void releaseKeyboardCapture() noexcept;
	virtual void releaseCapture() noexcept;

	virtual void reset() noexcept;

	virtual void addInputListener(InputListenerPtr& listener) noexcept;
	virtual void addInputListener(InputListenerPtr&& listener) noexcept;
	virtual void removeInputListener(InputListenerPtr& listener) noexcept;
	virtual void removeInputListener(InputListenerPtr&& listener) noexcept;
	virtual void clearInputListener() noexcept;

	virtual bool sendInputEvent(const InputEvent& event) noexcept;
	virtual bool postInputEvent(const InputEvent& event) noexcept;

	virtual void updateBegin() noexcept;
	virtual void update() noexcept;
	virtual void updateEnd() noexcept;

	virtual InputPtr clone() const noexcept;

private:
	DefaultInput(const DefaultInput&) noexcept = delete;
	DefaultInput& operator=(const DefaultInput&) noexcept = delete;

private:
	InputDevicePtr _inputDevice;
	InputMousePtr _mouseCaptureDevice;
	InputKeyboardPtr _keyboardCaptureDevice;
};

_NAME_END

#endif