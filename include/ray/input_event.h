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
#ifndef _H_INPUT_EVENT_H_
#define _H_INPUT_EVENT_H_

#include <ray/input_key.h>
#include <ray/input_button.h>

_NAME_BEGIN

struct InputKeysym
{
	std::uint16_t sym;
	std::uint16_t raw;
	std::uint16_t mod;
	std::uint16_t unicode;
};

struct KeyboardEvent
{
	std::uint64_t timestamp;
	std::uint64_t windowID;
	std::uint8_t state;
	std::uint8_t repeat;
	std::uint8_t padding2;
	std::uint8_t padding3;
	InputKeysym keysym;
};

struct MouseMotionEvent
{
	std::uint64_t timestamp;
	std::uint64_t windowID;
	std::uint8_t state;
	int x;
	int y;
	int xrel;
	int yrel;
};

struct MouseButtonEvent
{
	std::uint64_t timestamp;
	std::uint64_t windowID;
	std::uint32_t which;
	std::uint8_t button;
	std::uint8_t state;
	std::uint8_t clicks;
	std::uint8_t padding1;
	std::uint32_t x;
	std::uint32_t y;
};

struct MouseWheelEvent
{
	std::uint64_t timestamp;
	std::uint32_t windowID;
	std::uint8_t _state;
	int x;
	int y;
};

struct JoyAxisEvent {};
struct JoyBallEvent {};
struct JoyHatEvent {};
struct JoyButtonEvent {};
struct JoyDeviceEvent {};

class EXPORT InputEvent final
{
public:
	enum Type
	{
		KeyDown,
		KeyUp,
		Character,

		MouseMotion,
		MouseButtonDown,
		MouseButtonUp,
		MouseButtonDoubleClick,
		MouseWheelUp,
		MouseWheelDown,

		GamePadButtonDown,
		GamePadButtonUp,

		TouchMotionMove,
		TouchMotionDown,
		TouchMotionUp,
		TouchMotionCancel,

		GetFocus,
		LostFocus,

		Reset,

		AppQuit
	};

	Type event;
	KeyboardEvent key;
	MouseMotionEvent motion;
	MouseButtonEvent button;
	MouseWheelEvent wheel;
	JoyAxisEvent jaxis;
	JoyBallEvent jball;
	JoyHatEvent  jhat;
	JoyButtonEvent jbutton;
	JoyDeviceEvent jdevice;
};

_NAME_END

#endif