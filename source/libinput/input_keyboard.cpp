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
#include <ray/input_keyboard.h>

_NAME_BEGIN

__ImplementSubInterface(DefaultInputKeyboard, InputKeyboard, "DefaultInputKeyboard")

DefaultInputKeyboard::DefaultInputKeyboard() noexcept
{
	std::memset(_keyState, 0, sizeof(_keyState));
}

DefaultInputKeyboard::~DefaultInputKeyboard() noexcept
{
}

bool
DefaultInputKeyboard::isKeyDown(InputKey::Code key) const noexcept
{
	assert(key < InputKey::Code::NumKeyCodes);
	return _keyState[key].down;
}

bool
DefaultInputKeyboard::isKeyUp(InputKey::Code key) const noexcept
{
	assert(key < InputKey::Code::NumKeyCodes);
	return _keyState[key].up;
}

bool
DefaultInputKeyboard::isKeyPressed(InputKey::Code key) const noexcept
{
	assert(key < InputKey::Code::NumKeyCodes);
	return _keyState[key].pressed;
}

InputKeyboardPtr
DefaultInputKeyboard::clone() const noexcept
{
	return std::make_shared<DefaultInputKeyboard>();
}

void
DefaultInputKeyboard::onFrameEnd() noexcept
{
	for (std::size_t i = 0; i < InputKey::NumKeyCodes; i++)
	{
		auto& key = _keyState[i];

		if (key.up)
			key.pressed = false;

		key.up = false;
		key.down = false;
	}
}

void
DefaultInputKeyboard::onObtainCapture() noexcept
{
	for (auto& it : _keyState)
	{
		it.down = false;
		it.pressed = false;
		it.up = true;
	}
}

void
DefaultInputKeyboard::onReset() noexcept
{
	for (auto& it : _keyState)
	{
		it.down = false;
		it.pressed = false;
		it.up = true;
	}
}

void
DefaultInputKeyboard::onInputEvent(const InputEvent& event) noexcept
{
	switch (event.event)
	{
	case InputEvent::KeyDown:
	{
		auto& key = this->_keyState[event.key.keysym.sym];
		if (!key.pressed)
		{
			key.down = true;
			key.pressed = true;
		}
	}
	break;
	case InputEvent::KeyUp:
	{
		auto& key = this->_keyState[event.key.keysym.sym];
		key.up = true;
		key.pressed = false;
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