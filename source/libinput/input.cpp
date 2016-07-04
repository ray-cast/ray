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

#if defined(_BUILD_PLATFORM_WINDOWS)
#	include "msw_input_device.h"
#	include "msw_input_keyboard.h"
#	include "msw_input_mouse.h"
#endif

_NAME_BEGIN

__ImplementSubClass(DefaultInput, Input, "DefaultInput")

DefaultInput::DefaultInput() noexcept
{
}

DefaultInput::~DefaultInput() noexcept
{
	this->close();
}

bool 
DefaultInput::open() noexcept
{
#if defined(_BUILD_PLATFORM_WINDOWS)
	_inputDevice = std::make_shared<MSWInputDevice>();
	this->obtainMouseCapture(std::make_shared<MSWInputMouse>());
	this->obtainKeyboardCapture(std::make_shared<MSWInputKeyboard>());
#endif
	return true;
}

bool
DefaultInput::open(InputDevicePtr& device) noexcept
{
#if defined(_BUILD_PLATFORM_WINDOWS)
	_inputDevice = device ? device : std::make_shared<MSWInputDevice>();
	this->obtainMouseCapture(std::make_shared<MSWInputMouse>());
	this->obtainKeyboardCapture(std::make_shared<MSWInputKeyboard>());
#endif
	return true;
}

bool
DefaultInput::open(InputDevicePtr&& device) noexcept
{
#if defined(_BUILD_PLATFORM_WINDOWS)
	_inputDevice = device ? std::move(device) : std::make_shared<MSWInputDevice>();
	this->obtainMouseCapture(std::make_shared<MSWInputMouse>());
	this->obtainKeyboardCapture(std::make_shared<MSWInputKeyboard>());
#endif
	return true;
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
		_mouseCaptureDevice->releaseCapture();
		_mouseCaptureDevice.reset();
		_mouseCaptureDevice = nullptr;
	}

	if (_keyboardCaptureDevice)
	{
		_keyboardCaptureDevice->releaseCapture();
		_keyboardCaptureDevice.reset();
		_keyboardCaptureDevice = nullptr;
	}
}

void
DefaultInput::setCaptureObject(CaptureObject window) noexcept
{
	if (_inputDevice)
		_inputDevice->setCaptureObject(window);
}

CaptureObject
DefaultInput::getCaptureObject() const noexcept
{
	if (_inputDevice)
		return _inputDevice->getCaptureObject();
	return nullptr;
}

float 
DefaultInput::getAxisX() const noexcept
{
	if (_mouseCaptureDevice)
		return _mouseCaptureDevice->getAxisX();
	return 0.0f;
}

float 
DefaultInput::getAxisY() const noexcept
{
	if (_mouseCaptureDevice)
		return _mouseCaptureDevice->getAxisY();
	return 0.0f;
}

void
DefaultInput::setMousePos(int x, int y) noexcept
{
	if (_mouseCaptureDevice)
		_mouseCaptureDevice->setPosition(x, y);
}

void
DefaultInput::getMousePos(int& x, int& y) const noexcept
{
	if (_mouseCaptureDevice)
		_mouseCaptureDevice->getPosition(x, y);
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
DefaultInput::obtainMouseCapture(InputMousePtr& mouse) noexcept
{
	if (_mouseCaptureDevice != mouse)
	{
		if (_mouseCaptureDevice)
		{
			_mouseCaptureDevice->releaseCapture();

			if (_inputDevice)
				_inputDevice->removeInputListener(_mouseCaptureDevice);
		}

		_mouseCaptureDevice = mouse;

		if (_mouseCaptureDevice)
		{
			_mouseCaptureDevice->obtainCapture();

			if (_inputDevice)
				_inputDevice->addInputListener(_mouseCaptureDevice);
		}			
	}
}

void
DefaultInput::obtainMouseCapture(InputMousePtr&& mouse) noexcept
{
	if (_mouseCaptureDevice != mouse)
	{
		if (_mouseCaptureDevice)
		{
			_mouseCaptureDevice->releaseCapture();

			if (_inputDevice)
				_inputDevice->removeInputListener(_mouseCaptureDevice);
		}

		_mouseCaptureDevice = std::move(mouse);

		if (_mouseCaptureDevice)
		{
			_mouseCaptureDevice->obtainCapture();

			if (_inputDevice)
				_inputDevice->addInputListener(_mouseCaptureDevice);
		}
	}
}

void
DefaultInput::obtainKeyboardCapture(InputKeyboardPtr& keyboard) noexcept
{
	if (_keyboardCaptureDevice != keyboard)
	{
		if (_keyboardCaptureDevice)
		{
			_keyboardCaptureDevice->releaseCapture();

			if (_inputDevice)
				_inputDevice->removeInputListener(keyboard);
		}

		_keyboardCaptureDevice = keyboard;

		if (_keyboardCaptureDevice)
		{
			_keyboardCaptureDevice->obtainCapture();

			if (_inputDevice)
				_inputDevice->addInputListener(_keyboardCaptureDevice);
		}			
	}
}

void
DefaultInput::obtainKeyboardCapture(InputKeyboardPtr&& keyboard) noexcept
{
	if (_keyboardCaptureDevice != keyboard)
	{
		if (_keyboardCaptureDevice)
		{
			_keyboardCaptureDevice->releaseCapture();

			if (_inputDevice)
				_inputDevice->removeInputListener(keyboard);
		}

		_keyboardCaptureDevice = std::move(keyboard);

		if (_keyboardCaptureDevice)
		{
			_keyboardCaptureDevice->obtainCapture();

			if (_inputDevice)
				_inputDevice->addInputListener(_keyboardCaptureDevice);
		}
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
DefaultInput::addInputListener(InputListenerPtr& listener) noexcept
{
	if (_inputDevice)
		_inputDevice->addInputListener(listener);
}

void
DefaultInput::addInputListener(InputListenerPtr&& listener) noexcept
{
	if (_inputDevice)
		_inputDevice->addInputListener(std::move(listener));
}

void
DefaultInput::removeInputListener(InputListenerPtr& listener) noexcept
{
	if (_inputDevice)
		_inputDevice->removeInputListener(listener);
}

void
DefaultInput::removeInputListener(InputListenerPtr&& listener) noexcept
{
	if (_inputDevice)
		_inputDevice->removeInputListener(std::move(listener));
}

void
DefaultInput::clearInputListener() noexcept
{
	if (_inputDevice)
		_inputDevice->clearInputListener();
}

bool
DefaultInput::sendInputEvent(const InputEvent& event) noexcept
{
	if (_inputDevice)
		return _inputDevice->sendEvent(event);
	return false;
}

bool
DefaultInput::postInputEvent(const InputEvent& event) noexcept
{
	if (_inputDevice)
		return _inputDevice->postEvent(event);
	return true;
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
		this->sendInputEvent(event);
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