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
#include "msw_input_keyboard.h"

_NAME_BEGIN

__ImplementSubClass(MSWInputKeyboard, DefaultInputKeyboard, "MSWInputKeyboard")

MSWInputKeyboard::MSWInputKeyboard() noexcept
{
}

MSWInputKeyboard::~MSWInputKeyboard() noexcept
{
}

bool
MSWInputKeyboard::getKeyState(InputKey::Code key) const noexcept
{
	int virtualKey = 0;

	switch (key)
	{
	case ray::InputKey::ESCAPE:
		virtualKey = VK_ESCAPE;
		break;
	case ray::InputKey::MINUS:
		virtualKey = VK_OEM_MINUS;
		break;
	case ray::InputKey::EQUALS:
		virtualKey = VK_OEM_NEC_EQUAL;
		break;
	case ray::InputKey::BACKSPACE:
		virtualKey = VK_BACK;
		break;
	case ray::InputKey::TAB:
		virtualKey = VK_TAB;
		break;
	case ray::InputKey::SUB:
		virtualKey = VK_SUBTRACT;
		break;
	case ray::InputKey::DIVIDE:
		break;
	case ray::InputKey::MULTIPLY:
		break;
	case ray::InputKey::SUBTRACT:
		break;
	case ray::InputKey::ADD:
		virtualKey = VK_ADD;
		break;
	case ray::InputKey::DECIMAL:
		break;
	case ray::InputKey::CLEAR:
		break;
	case ray::InputKey::GRAVE:
		break;
	case ray::InputKey::COMMA:
		break;
	case ray::InputKey::PERIOD:
		break;
	case ray::InputKey::SLASH:
		break;
	case ray::InputKey::LCTRL:
		break;
	case ray::InputKey::RCTRL:
		break;
	case ray::InputKey::LALT:
		break;
	case ray::InputKey::RALT:
		break;
	case ray::InputKey::LSHIFT:
		break;
	case ray::InputKey::RSHIFT:
		break;
	case ray::InputKey::LWIN:
		break;
	case ray::InputKey::RWIN:
		break;
	case ray::InputKey::SPACE:
		virtualKey = VK_SPACE;
		break;
	case ray::InputKey::ENTER:
		break;
	case ray::InputKey::LBRACKET:
		break;
	case ray::InputKey::RBRACKET:
		break;
	case ray::InputKey::BACKSLASH:
		break;
	case ray::InputKey::COLON:
		break;
	case ray::InputKey::SEMICOLON:
		break;
	case ray::InputKey::UNDERLINE:
		break;
	case ray::InputKey::TILDE:
		break;
	case ray::InputKey::NP_PERIOD:
		break;
	case ray::InputKey::NP_DIVIDE:
		break;
	case ray::InputKey::PRINT:
		break;
	case ray::InputKey::PAUSE:
		break;
	case ray::InputKey::HOME:
		break;
	case ray::InputKey::UP:
		break;
	case ray::InputKey::PGUP:
		break;
	case ray::InputKey::LEFT:
		break;
	case ray::InputKey::RIGHT:
		break;
	case ray::InputKey::END:
		break;
	case ray::InputKey::DOWN:
		break;
	case ray::InputKey::PGDN:
		break;
	case ray::InputKey::INSERT:
		break;
	case ray::InputKey::DELETE:
		break;
	case ray::InputKey::APPS:
		break;
	case ray::InputKey::OEM_102:
		break;
	case ray::InputKey::NUMLOCK:
		virtualKey = VK_NUMLOCK;
		break;
	case ray::InputKey::CAPSLOCK:
		break;
	case ray::InputKey::SCROLLLOCK:
		break;
	case ray::InputKey::NP_MULTIPLY:
		break;
	case ray::InputKey::NP_SUBSTRACT:
		break;
	case ray::InputKey::NP_ADD:
		break;
	case ray::InputKey::NP_ENTER:
		break;
	default:
		break;
	}

	if (key >= InputKey::Code::A && key <= InputKey::Code::Z)
	{
		virtualKey = 'a' + (key - InputKey::Code::A);
	}

	if (key >= InputKey::Code::Key0 && key <= InputKey::Code::Key1)
	{
		virtualKey = '0' + (key - InputKey::Code::Key0);
	}

	if (key >= InputKey::Code::NP_0 && key <= InputKey::Code::NP_9)
	{
		virtualKey = VK_NUMPAD0 + (key - InputKey::Code::NP_0);
	}

	if (key >= InputKey::Code::F1 && key <= InputKey::Code::F24)
	{
		virtualKey = VK_F1 + (key - InputKey::Code::F1);
	}

	//if (virtualKey != 0)
		//return ::GetAsyncKeyState(virtualKey) && 0x8000;

	return true;
}

InputKeyboardPtr
MSWInputKeyboard::clone() const noexcept
{
	return std::make_shared<MSWInputKeyboard>();
}

_NAME_END