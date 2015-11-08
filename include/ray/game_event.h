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
#ifndef _H_GAME_EVENT_H_
#define _H_GAME_EVENT_H_

#include <ray/game_message.h>

_NAME_BEGIN

struct App
{
	std::uint32_t timestamp;
};

struct QuitMessage
{
	std::uint32_t timestamp;
	std::int32_t  code;
};

struct WindowMessage
{
	std::uint32_t type;
	std::uint32_t timestamp;
	std::uint32_t id;
	std::int32_t x;
	std::int32_t y;
	std::int32_t w;
	std::int32_t h;
};

struct MouseEvent
{
	std::uint32_t timestamp;
	std::uint32_t windowID;
	std::uint32_t which;
	std::uint8_t button;
	std::uint8_t state;
	std::uint8_t clicks;
	std::uint8_t padding1;
	std::uint32_t x;
	std::uint32_t y;
	std::uint32_t xrel;
	std::uint32_t yrel;
};

class AppQuitEvent : public GameEvent
{
public:
	AppQuitEvent()
		:GameEvent(typeid(AppQuitEvent))
	{
	}

	QuitMessage quit;
};

class AppResetEvent : public GameEvent
{
public:
	AppResetEvent()
		:GameEvent(typeid(AppResetEvent))
	{
	}

	App app;
};

class GetFocusEvent : public GameEvent
{
public:
	GetFocusEvent()
		:GameEvent(typeid(GetFocusEvent))
	{
	}

	WindowMessage window;
};

class LostFocusEvent : public GameEvent
{
public:
	LostFocusEvent()
		:GameEvent(typeid(LostFocusEvent))
	{
	}

	WindowMessage window;
};

class SizeChangeEvent : public GameEvent
{
public:
	SizeChangeEvent()
		:GameEvent(typeid(SizeChangeEvent))
	{
	}

	WindowMessage window;
};

class MoveChange : public GameEvent
{
public:
	MoveChange()
		:GameEvent(typeid(MoveChange))
	{
	}

	WindowMessage window;
};

class MouseMotion : public GameEvent
{
public:
	MouseMotion()
		:GameEvent(typeid(MouseMotion))
	{
	}

	MouseEvent mouse;
};

class MouseDrag : public GameEvent
{
public:
	MouseDrag()
		:GameEvent(typeid(MouseDrag))
	{
	}

	MouseEvent mouse;
};

class MouseEnter : public GameEvent
{
public:
	MouseEnter()
		:GameEvent(typeid(MouseEnter))
	{
	}

	MouseEvent mouse;
};

class MouseOver : public GameEvent
{
public:
	MouseOver()
		:GameEvent(typeid(MouseOver))
	{
	}

	MouseEvent mouse;
};

class MouseButtonDown : public GameEvent
{
public:
	MouseButtonDown()
		:GameEvent(typeid(MouseButtonDown))
	{
	}

	MouseEvent mouse;
};

class MouseButtonUp : public GameEvent
{
public:
	MouseButtonUp()
		:GameEvent(typeid(MouseButtonUp))
	{
	}

	MouseEvent mouse;
};

class WillRenderObjectEvent : public GameEvent
{
public:
	WillRenderObjectEvent()
		:GameEvent(typeid(WillRenderObjectEvent))
	{
	}
};

class RenderObjectEvent : public GameEvent
{
public:
	RenderObjectEvent()
		:GameEvent(typeid(RenderObjectEvent))
	{
	}
};

_NAME_END

#endif