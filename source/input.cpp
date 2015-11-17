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
#include <ray/input.h>

_NAME_BEGIN

__ImplementSubClass(DefaultInput, Input, "DefaultInput")

DefaultInput::DefaultInput() noexcept
{
}

DefaultInput::~DefaultInput() noexcept
{
	this->close();
}

void
DefaultInput::open(InputDevicePtr device) noexcept
{
	assert(device);
	_inputDevice = device;
}

void
DefaultInput::close() noexcept
{
	if (_inputDevice)
	{
		_inputDevice.reset();
		_inputDevice = nullptr;
	}

	if (_mouseCaptureDevice)
	{
		_mouseCaptureDevice.reset();
		_mouseCaptureDevice = nullptr;
	}

	if (_keyboardCaptureDevice)
	{
		_keyboardCaptureDevice.reset();
		_keyboardCaptureDevice = nullptr;
	}
}

void
DefaultInput::setMousePosX(int x) noexcept
{
	if (_mouseCaptureDevice)
		_mouseCaptureDevice->setPositionX(x);
}

void
DefaultInput::setMousePosY(int y) noexcept
{
	if (_mouseCaptureDevice)
		_mouseCaptureDevice->setPositionY(y);
}

void
DefaultInput::setMousePos(int x, int y) noexcept
{
	if (_mouseCaptureDevice)
		_mouseCaptureDevice->setPosition(x, y);
}

int
DefaultInput::getMousePosX() const noexcept
{
	if (_mouseCaptureDevice)
		return _mouseCaptureDevice->getPositionX();

	return 0;
}

int
DefaultInput::getMousePosY() const noexcept
{
	if (_mouseCaptureDevice)
		return _mouseCaptureDevice->getPositionY();

	return 0;
}

bool
DefaultInput::getKeyDown(InputKey::Code key) const noexcept
{
	if (_keyboardCaptureDevice)
		return _keyboardCaptureDevice->getKeyDown(key);

	return false;
}

bool
DefaultInput::getKeyUp(InputKey::Code key) const noexcept
{
	if (_keyboardCaptureDevice)
		return _keyboardCaptureDevice->getKeyUp(key);

	return false;
}

bool
DefaultInput::getKey(InputKey::Code key) const noexcept
{
	if (_keyboardCaptureDevice)
		return _keyboardCaptureDevice->getKey(key);

	return false;
}

bool
DefaultInput::getButtonDown(InputButton::Code key) const noexcept
{
	if (_mouseCaptureDevice)
		return _mouseCaptureDevice->getButtonDown(key);

	return false;
}

bool
DefaultInput::getButtonUp(InputButton::Code key) const noexcept
{
	if (_mouseCaptureDevice)
		return _mouseCaptureDevice->getButtonUp(key);

	return false;
}

bool
DefaultInput::getButton(InputButton::Code key) const noexcept
{
	if (_mouseCaptureDevice)
		return _mouseCaptureDevice->getButton(key);

	return false;
}

void
DefaultInput::showCursor(bool show) noexcept
{
	if (_mouseCaptureDevice)
	{
		if (show)
			_mouseCaptureDevice->showMouse();
		else
			_mouseCaptureDevice->hideMouse();
	}
}

bool
DefaultInput::isShowCursor() const noexcept
{
	if (_mouseCaptureDevice)
		return _mouseCaptureDevice->isShowMouse();
	return false;
}

void
DefaultInput::lockCursor(bool lock) noexcept
{
	if (_mouseCaptureDevice)
	{
		if (lock)
			_mouseCaptureDevice->lockMouse();
		else
			_mouseCaptureDevice->unlockMouse();
	}
}

bool
DefaultInput::isLockedCursor() const noexcept
{
	if (_mouseCaptureDevice)
		return _mouseCaptureDevice->isLockedMouse();
	return false;
}

void
DefaultInput::obtainMouseCapture() noexcept
{
	if (_mouseCaptureDevice && !_mouseCaptureDevice->capture())
		_mouseCaptureDevice->obtainCapture();
}

void
DefaultInput::obtainKeyboardCapture() noexcept
{
	if (_keyboardCaptureDevice && !_keyboardCaptureDevice->capture())
		_keyboardCaptureDevice->obtainCapture();
}

void
DefaultInput::obtainMouseCapture(InputMousePtr mouse) noexcept
{
	if (_mouseCaptureDevice != mouse)
	{
		if (_mouseCaptureDevice)
			_mouseCaptureDevice->onReleaseCapture();

		_mouseCaptureDevice = mouse;

		if (_mouseCaptureDevice)
			_mouseCaptureDevice->onObtainCapture();
	}
}

void
DefaultInput::obtainKeyboardCapture(InputKeyboardPtr keyboard) noexcept
{
	if (_keyboardCaptureDevice != keyboard)
	{
		if (_keyboardCaptureDevice)
			_keyboardCaptureDevice->onReleaseCapture();

		_keyboardCaptureDevice = keyboard;

		if (_keyboardCaptureDevice)
			_keyboardCaptureDevice->onObtainCapture();
	}
}

void
DefaultInput::obtainCapture() noexcept
{
	this->obtainMouseCapture();
	this->obtainKeyboardCapture();
}

void
DefaultInput::releaseMouseCapture() noexcept
{
	if (_mouseCaptureDevice && _mouseCaptureDevice->capture())
		_mouseCaptureDevice->releaseCapture();
}

void
DefaultInput::releaseKeyboardCapture() noexcept
{
	if (_keyboardCaptureDevice && _keyboardCaptureDevice->capture())
		_keyboardCaptureDevice->releaseCapture();
}

void
DefaultInput::releaseCapture() noexcept
{
	this->releaseMouseCapture();
	this->releaseKeyboardCapture();
}

void
DefaultInput::reset() noexcept
{
	if (_mouseCaptureDevice)
		_mouseCaptureDevice->onReset();

	if (_keyboardCaptureDevice)
		_keyboardCaptureDevice->onReset();
}

void
DefaultInput::addInputListener(InputListenerPtr listener) noexcept
{
	assert(listener);
	auto it = std::find(_inputListeners.begin(), _inputListeners.end(), listener);
	if (it == _inputListeners.end())
	{
		_inputListeners.push_back(listener);
	}
}

void
DefaultInput::removeInputListener(InputListenerPtr listener) noexcept
{
	assert(listener);
	auto it = std::find(_inputListeners.begin(), _inputListeners.end(), listener);
	if (it != _inputListeners.end())
	{
		_inputListeners.erase(it);
	}
}

void 
DefaultInput::clearInputListener() noexcept
{
	_inputListeners.clear();
}

void
DefaultInput::updateBegin() noexcept
{
	if (_mouseCaptureDevice)
		_mouseCaptureDevice->onFrameBegin();

	if (_keyboardCaptureDevice)
		_keyboardCaptureDevice->onFrameBegin();
}

void
DefaultInput::update() noexcept
{
	if (!_inputDevice)
		return;

	InputEvent event;
	while (_inputDevice->pollEvents(event))
	{
		switch (event.event)
		{
		case InputEvent::KeyDown:
		case InputEvent::KeyUp:
		{
			if (_keyboardCaptureDevice && _keyboardCaptureDevice->capture())
				_keyboardCaptureDevice->onEvent(event);
			break;
		}
		case InputEvent::MouseMotion:
		case InputEvent::MouseWheelUp:
		case InputEvent::MouseWheelDown:
		case InputEvent::MouseButtonUp:
		case InputEvent::MouseButtonDown:
		case InputEvent::MouseButtonDoubleClick:
		{
			if (_mouseCaptureDevice && _mouseCaptureDevice->capture())
				_mouseCaptureDevice->onEvent(event);
		}
		break;
		default:
			break;
		}

		for (auto& it : _inputListeners)
			it->onInputEvent(event);
	}
}

void
DefaultInput::updateEnd() noexcept
{
	if (_mouseCaptureDevice && _mouseCaptureDevice->capture())
		_mouseCaptureDevice->onFrameEnd();

	if (_keyboardCaptureDevice && _keyboardCaptureDevice->capture())
		_keyboardCaptureDevice->onFrameEnd();
}

InputPtr
DefaultInput::clone() const noexcept
{
	auto input = std::make_shared<DefaultInput>();
	if (_inputDevice)
		input->open(_inputDevice->clone());
	if (_keyboardCaptureDevice)
		input->obtainKeyboardCapture(_keyboardCaptureDevice->clone());
	if (_mouseCaptureDevice)
		input->obtainMouseCapture(_mouseCaptureDevice->clone());
	return input;
}

_NAME_END