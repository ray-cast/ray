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
#ifndef _H_INPUT_MOUSE_H_
#define _H_INPUT_MOUSE_H_

#include <ray/input_mouse_base.h>

_NAME_BEGIN

class EXPORT DefaultInputMouse : public InputMouse
{
	__DeclareSubClass(DefaultInputMouse, InputMouse)
public:
	DefaultInputMouse() noexcept;
	virtual ~DefaultInputMouse() noexcept;

	void lockMouse() noexcept;
	void unlockMouse() noexcept;
	bool isLockedMouse() const noexcept;

	void showMouse() noexcept;
	void hideMouse() noexcept;
	bool isShowMouse() noexcept;

	float getAxis(InputAxis::Code axis) const noexcept;

	void setPosition(InputButton::mouse_t x, InputButton::mouse_t y) noexcept;
	void getPosition(InputButton::mouse_t& x, InputButton::mouse_t& y) const noexcept;

	bool isButtonDown(InputButton::Code key) const noexcept;
	bool isButtonUp(InputButton::Code key) const noexcept;
	bool isButtonPressed(InputButton::Code key) const noexcept;

	InputMousePtr clone() const noexcept;

protected:
	virtual void onFrameBegin() noexcept;
	virtual void onFrameEnd() noexcept;

	virtual void onObtainCapture() noexcept;
	virtual void onReleaseCapture() noexcept;

	virtual void onReset() noexcept;

	virtual void onInputEvent(const InputEvent& event) noexcept;

	virtual void onShowMouse() noexcept;
	virtual void onHideMouse() noexcept;

	virtual void onChangePosition(InputButton::mouse_t x, InputButton::mouse_t y) noexcept;

private:
	DefaultInputMouse(const DefaultInputMouse&) noexcept = delete;
	DefaultInputMouse& operator=(const DefaultInputMouse&) noexcept = delete;

protected:

	bool _isMouseLock;
	bool _isMouseLocked;
	bool _isMouseHide;

	float _mouseAxisX;
	float _mouseAxisY;

	InputButton::mouse_t _lastX;
	InputButton::mouse_t _lastY;

	InputButton::mouse_t _mouseX;
	InputButton::mouse_t _mouseY;

	InputButton::mouse_t _centerX;
	InputButton::mouse_t _centerY;

	struct ButtonState
	{
		bool down;
		bool up;
		bool pressed;
		bool doubleClick;
	};

	ButtonState _buttonState[InputButton::NumButtonCodes];
};

_NAME_END

#endif