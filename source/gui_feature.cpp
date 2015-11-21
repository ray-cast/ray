// +----------------------------------------------------------------------
// | Project : ray.
// | All rights reserved.
// +----------------------------------------------------------------------
// | Copyright (c) 2013-2015.
// +----------------------------------------------------------------------
// | * Redistribution and use of _renderThread software in source and binary forms,
// |   with or without modification, are permitted provided that the following
// |   conditions are met:
// |
// | * Redistributions of source code must retain the above
// |   copyright notice, _renderThread list of conditions and the
// |   following disclaimer.
// |
// | * Redistributions in binary form must reproduce the above
// |   copyright notice, _renderThread list of conditions and the
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
#if defined(_BUILD_GUI)
#include <ray/gui_feature.h>
#include <ray/gui_system.h>
#include <ray/input_event.h>

_NAME_BEGIN

GuiButton::Code ButtonCodeToGuiButton(InputButton::Code button) noexcept
{
	switch (button)
	{
	case ray::InputButton::LEFT:
		return GuiButton::Code::Left;
	case ray::InputButton::RIGHT:
		return GuiButton::Code::Right;
	case ray::InputButton::MIDDLE:
		return GuiButton::Code::Middle;
	case ray::InputButton::MOUSE3:
		return GuiButton::Code::Button3;
	case ray::InputButton::MOUSE4:
		return GuiButton::Code::Button4;
	case ray::InputButton::MOUSE5:
		return GuiButton::Code::Button5;
	case ray::InputButton::MOUSE6:
		return GuiButton::Code::Button6;
	case ray::InputButton::MOUSE7:
		return GuiButton::Code::Button7;
	}

	return GuiButton::Code::None;
};

GuiKey::Code KeyCodetoGuiKey(InputKey::Code key) noexcept
{
	switch (key)
	{
	case InputKey::BACKSPACE:
		return GuiKey::Code::Backspace;
	case InputKey::TAB:
		return GuiKey::Code::Tab;
	case InputKey::ENTER:
		return GuiKey::Code::Return;
	case InputKey::PAUSE:
		return GuiKey::Code::Pause;
	case InputKey::ESCAPE:
		return GuiKey::Code::Escape;
	case InputKey::SPACE:
		return GuiKey::Code::Space;
	case InputKey::PGDN:
		return GuiKey::Code::NextTrack;
	case InputKey::END:
		return GuiKey::Code::End;
	case InputKey::HOME:
		return GuiKey::Code::Home;
	case InputKey::LEFT:
		return GuiKey::Code::ArrowLeft;
	case InputKey::RIGHT:
		return GuiKey::Code::ArrowRight;
	case InputKey::UP:
		return GuiKey::Code::ArrowUp;
	case InputKey::DOWN:
		return GuiKey::Code::ArrowDown;
	case InputKey::INSERT:
		return GuiKey::Code::Insert;
	case InputKey::DELETE:
		return GuiKey::Code::Delete;
	case InputKey::NP_0:
		return GuiKey::Code::Numpad0;
	case InputKey::NP_1:
		return GuiKey::Code::Numpad1;
	case InputKey::NP_2:
		return GuiKey::Code::Numpad2;
	case InputKey::NP_3:
		return GuiKey::Code::Numpad3;
	case InputKey::NP_4:
		return GuiKey::Code::Numpad4;
	case InputKey::NP_5:
		return GuiKey::Code::Numpad5;
	case InputKey::NP_6:
		return GuiKey::Code::Numpad6;
	case InputKey::NP_7:
		return GuiKey::Code::Numpad7;
	case InputKey::NP_8:
		return GuiKey::Code::Numpad8;
	case InputKey::NP_9:
		return GuiKey::Code::Numpad9;
	case InputKey::MULTIPLY:
		return GuiKey::Code::Multiply;
	case InputKey::ADD:
		return GuiKey::Code::Add;
	case InputKey::SUBTRACT:
		return GuiKey::Code::Subtract;
	case InputKey::DECIMAL:
		return GuiKey::Code::Decimal;
	case InputKey::DIVIDE:
		return GuiKey::Code::Divide;
	case InputKey::F1:
		return GuiKey::Code::F1;
	case InputKey::F2:
		return GuiKey::Code::F2;
	case InputKey::F3:
		return GuiKey::Code::F3;
	case InputKey::F4:
		return GuiKey::Code::F4;
	case InputKey::F5:
		return GuiKey::Code::F5;
	case InputKey::F6:
		return GuiKey::Code::F6;
	case InputKey::F7:
		return GuiKey::Code::F7;
	case InputKey::F8:
		return GuiKey::Code::F8;
	case InputKey::F9:
		return GuiKey::Code::F9;
	case InputKey::F10:
		return GuiKey::Code::F10;
	case InputKey::F11:
		return GuiKey::Code::F11;
	case InputKey::F12:
		return GuiKey::Code::F12;
	case InputKey::F13:
		return GuiKey::Code::F13;
	case InputKey::F14:
		return GuiKey::Code::F14;
	case InputKey::F15:
		return GuiKey::Code::F15;
	case InputKey::NUMLOCK:
		return GuiKey::Code::NumLock;
	case InputKey::SCROLLLOCK:
		return GuiKey::Code::ScrollLock;
	case InputKey::SEMICOLON:
		return GuiKey::Code::Semicolon;
	case InputKey::SLASH:
		return GuiKey::Code::Slash;
	case InputKey::GRAVE:
		return GuiKey::Code::Grave;
	case InputKey::LBRACKET:
		return GuiKey::Code::LeftBracket;
	case InputKey::RBRACKET:
		return GuiKey::Code::RightBracket;
	case InputKey::BACKSLASH:
		return GuiKey::Code::Backslash;
	case InputKey::COMMA:
		return GuiKey::Code::Comma;
	case InputKey::UNDERLINE:
		return GuiKey::Code::Minus;
	case InputKey::PERIOD:
		return GuiKey::Code::Period;
	case InputKey::EQUALS:
		return GuiKey::Code::Equals;
	case InputKey::LSHIFT:
		return GuiKey::Code::LeftShift;
	case InputKey::RSHIFT:
		return GuiKey::Code::RightShift;
	case InputKey::LCTRL:
		return GuiKey::Code::LeftControl;
	case InputKey::RCTRL:
		return GuiKey::Code::RightControl;
	case InputKey::LWIN:
		return GuiKey::Code::LeftWindows;
	case InputKey::RWIN:
		return GuiKey::Code::RightWindow;
	case InputKey::VOLUMEDOWN:
		return GuiKey::Code::VolumeDown;
	case InputKey::VOLUMEUP:
		return GuiKey::Code::VolumeUp;
	case InputKey::Key0:
		return GuiKey::Code::Zero;
	case InputKey::Key1:
		return GuiKey::Code::One;
	case InputKey::Key2:
		return GuiKey::Code::Two;
	case InputKey::Key3:
		return GuiKey::Code::Three;
	case InputKey::Key4:
		return GuiKey::Code::Four;
	case InputKey::Key5:
		return GuiKey::Code::Five;
	case InputKey::Key6:
		return GuiKey::Code::Six;
	case InputKey::Key7:
		return GuiKey::Code::Seven;
	case InputKey::Key8:
		return GuiKey::Code::Eight;
	case InputKey::Key9:
		return GuiKey::Code::Nine;
	case InputKey::A:
		return GuiKey::Code::A;
	case InputKey::B:
		return GuiKey::Code::B;
	case InputKey::C:
		return GuiKey::Code::C;
	case InputKey::D:
		return GuiKey::Code::D;
	case InputKey::E:
		return GuiKey::Code::E;
	case InputKey::F:
		return GuiKey::Code::F;
	case InputKey::G:
		return GuiKey::Code::G;
	case InputKey::H:
		return GuiKey::Code::H;
	case InputKey::I:
		return GuiKey::Code::I;
	case InputKey::J:
		return GuiKey::Code::J;
	case InputKey::K:
		return GuiKey::Code::K;
	case InputKey::L:
		return GuiKey::Code::L;
	case InputKey::M:
		return GuiKey::Code::M;
	case InputKey::N:
		return GuiKey::Code::N;
	case InputKey::O:
		return GuiKey::Code::O;
	case InputKey::P:
		return GuiKey::Code::P;
	case InputKey::Q:
		return GuiKey::Code::Q;
	case InputKey::R:
		return GuiKey::Code::R;
	case InputKey::S:
		return GuiKey::Code::S;
	case InputKey::T:
		return GuiKey::Code::T;
	case InputKey::U:
		return GuiKey::Code::U;
	case InputKey::V:
		return GuiKey::Code::V;
	case InputKey::W:
		return GuiKey::Code::W;
	case InputKey::X:
		return GuiKey::Code::X;
	case InputKey::Y:
		return GuiKey::Code::Y;
	case InputKey::Z:
		return GuiKey::Code::Z;
	}

	return GuiKey::Code::None;
}

GuiFeature::GuiFeature(std::uint32_t w, std::uint32_t h) noexcept
	: _width(w)
	, _height(h)
	, _platform(nullptr)
	, _loader(nullptr)
{
}

GuiFeature::~GuiFeature() noexcept
{
}

void
GuiFeature::render() except
{
	if (_platform)
		_platform->render();
}

void
GuiFeature::onActivate() except
{
	_loader = new GuiImageLoader;

	_platform = new MyGuiSystem();
	_platform->open(_loader);
	_platform->setViewport(_width, _height);
}

void
GuiFeature::onDeactivate() except
{
	if (_platform)
	{
		delete _platform;
		_platform = nullptr;
	}

	if (_loader)
	{
		delete _loader;
		_loader = nullptr;
	}
}

void
GuiFeature::onMessage(const MessagePtr& message) except
{
	if (message->isInstanceOf<InputEvent>())
	{
		InputEventPtr event = std::dynamic_pointer_cast<InputEvent>(message);
		if (event)
		{
			switch (event->event)
			{
			case InputEvent::MouseMotion:
				_platform->injectMouseMove(event->motion.x, event->motion.y, 0);
				break;
			case InputEvent::MouseButtonDown:
				_platform->injectMousePress(event->button.x, event->button.y, ButtonCodeToGuiButton((InputButton::Code)event->button.button));
				break;
			case InputEvent::MouseButtonUp:
				_platform->injectMouseRelease(event->button.x, event->button.y, ButtonCodeToGuiButton((InputButton::Code)event->button.button));
				break;
			case InputEvent::KeyDown:
				_platform->injectKeyPress(KeyCodetoGuiKey((InputKey::Code)event->key.keysym.sym));
				break;
			case InputEvent::KeyUp:
				_platform->injectKeyRelease(KeyCodetoGuiKey((InputKey::Code)event->key.keysym.sym));
				break;
			}
		}
	}
}

_NAME_END
#endif