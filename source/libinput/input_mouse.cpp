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
#include <ray/input_mouse.h>

_NAME_BEGIN

__ImplementSubInterface(DefaultInputMouse, InputMouse, "DefaultInputMouse")

DefaultInputMouse::DefaultInputMouse() noexcept
	: _mouseX(0)
	, _mouseY(0)
	, _isMouseLock(false)
	, _isMouseLocked(false)
	, _isMouseHide(false)
{
	std::memset(_buttonState, 0, sizeof(_buttonState));
}

DefaultInputMouse::~DefaultInputMouse() noexcept
{
}

void
DefaultInputMouse::lockMouse() noexcept
{
	if (!isLockedMouse())
	{
		this->hideMouse();

		_lockX = _mouseX;
		_lockY = _mouseY;

		_isMouseLock = true;
		_isMouseLocked = true;
	}
}

void
DefaultInputMouse::unlockMouse() noexcept
{
	if (isLockedMouse())
	{
		this->showMouse();

		_isMouseLock = false;
		_isMouseLocked = false;
	}
}

bool
DefaultInputMouse::isLockedMouse() const noexcept
{
	return _isMouseLocked;
}

void
DefaultInputMouse::showMouse() noexcept
{
	if (_isMouseHide)
	{
		this->onShowMouse();
		_isMouseHide = false;
	}
}

void
DefaultInputMouse::hideMouse() noexcept
{
	if (!_isMouseHide)
	{
		this->onHideMouse();
		_isMouseHide = true;
	}
}

bool
DefaultInputMouse::isShowMouse() noexcept
{
	return !_isMouseHide;
}

void
DefaultInputMouse::setPosition(int x, int y) noexcept
{
	_mouseX = x;
	_mouseY = y;
	this->onChangePosition(x, y);
}

void
DefaultInputMouse::getPosition(int& x, int& y) const noexcept
{
	x = _mouseX;
	y = _mouseY;
}

bool
DefaultInputMouse::getButtonDown(InputButton::Code key) const noexcept
{
	return _buttonState[key].down;
}

bool
DefaultInputMouse::getButtonUp(InputButton::Code key) const noexcept
{
	return _buttonState[key].up;
}

bool
DefaultInputMouse::getButton(InputButton::Code key) const noexcept
{
	return _buttonState[key].pressed;
}

void
DefaultInputMouse::onFrameBegin() noexcept
{
	for (auto& button : _buttonState)
	{
		if (button.up)
		{
			button.pressed = false;
		}

		button.up = false;
		button.down = false;
	}
}

void
DefaultInputMouse::onFrameEnd() noexcept
{
	if (_isMouseLocked)
	{
		this->setPosition(_lockX, _lockY);
	}
}

void
DefaultInputMouse::onObtainCapture() noexcept
{
	if (_isMouseLock)
	{
		this->hideMouse();
		_isMouseLocked = true;
	}
}

void
DefaultInputMouse::onReleaseCapture() noexcept
{
	if (_isMouseLocked)
	{
		this->showMouse();
		_isMouseLocked = false;
	}
}

void
DefaultInputMouse::onInputEvent(const InputEvent& event) noexcept
{
	switch (event.event)
	{
	case InputEvent::MouseMotion:
	{
		_mouseX = event.motion.x;
		_mouseY = event.motion.y;
	}
	break;
	case InputEvent::MouseWheelDown:
	case InputEvent::MouseButtonDown:
	{
		auto& key = this->_buttonState[event.button.button];
		if (!key.pressed)
		{
			key.down = true;
			key.pressed = true;
		}
	}
	break;
	case InputEvent::MouseWheelUp:
	case InputEvent::MouseButtonUp:
	{
		auto& key = this->_buttonState[event.button.button];
		key.up = true;
		key.pressed = false;
		key.down = false;
	}
	break;
	case InputEvent::MouseButtonDoubleClick:
	{
		auto& key = this->_buttonState[event.button.button];
		key.click = true;
	}
	break;
	case InputEvent::GetFocus:
		this->obtainCapture();
		break;
	case InputEvent::LostFocus:
		this->releaseCapture();
		break;
	case InputEvent::Reset:
		this->onReset();
		break;
	default:
		break;
	}
}

_NAME_END