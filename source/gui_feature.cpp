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
#include <ray/game_server.h>
#include <ray/timer.h>
#include <ray/image.h>
#include <ray/input_event.h>

_NAME_BEGIN

__ImplementSubClass(GuiFeature, GameFeature, "GuiFeature")

Gui::GuiButton::Code ButtonCodeToGuiButton(InputButton::Code button) noexcept
{
	switch (button)
	{
	case ray::InputButton::LEFT:
		return Gui::GuiButton::Code::Left;
	case ray::InputButton::RIGHT:
		return Gui::GuiButton::Code::Right;
	case ray::InputButton::MIDDLE:
		return Gui::GuiButton::Code::Middle;
	case ray::InputButton::MOUSE3:
		return Gui::GuiButton::Code::Button3;
	case ray::InputButton::MOUSE4:
		return Gui::GuiButton::Code::Button4;
	case ray::InputButton::MOUSE5:
		return Gui::GuiButton::Code::Button5;
	case ray::InputButton::MOUSE6:
		return Gui::GuiButton::Code::Button6;
	case ray::InputButton::MOUSE7:
		return Gui::GuiButton::Code::Button7;
	}

	return Gui::GuiButton::Code::None;
};

Gui::GuiKey::Code KeyCodetoGuiKey(InputKey::Code key) noexcept
{
	switch (key)
	{
	case InputKey::BACKSPACE:
		return Gui::GuiKey::Code::Backspace;
	case InputKey::TAB:
		return Gui::GuiKey::Code::Tab;
	case InputKey::ENTER:
		return Gui::GuiKey::Code::Return;
	case InputKey::PAUSE:
		return Gui::GuiKey::Code::Pause;
	case InputKey::ESCAPE:
		return Gui::GuiKey::Code::Escape;
	case InputKey::SPACE:
		return Gui::GuiKey::Code::Space;
	case InputKey::PGDN:
		return Gui::GuiKey::Code::NextTrack;
	case InputKey::END:
		return Gui::GuiKey::Code::End;
	case InputKey::HOME:
		return Gui::GuiKey::Code::Home;
	case InputKey::LEFT:
		return Gui::GuiKey::Code::ArrowLeft;
	case InputKey::RIGHT:
		return Gui::GuiKey::Code::ArrowRight;
	case InputKey::UP:
		return Gui::GuiKey::Code::ArrowUp;
	case InputKey::DOWN:
		return Gui::GuiKey::Code::ArrowDown;
	case InputKey::INSERT:
		return Gui::GuiKey::Code::Insert;
	case InputKey::DELETE:
		return Gui::GuiKey::Code::Delete;
	case InputKey::NP_0:
		return Gui::GuiKey::Code::Numpad0;
	case InputKey::NP_1:
		return Gui::GuiKey::Code::Numpad1;
	case InputKey::NP_2:
		return Gui::GuiKey::Code::Numpad2;
	case InputKey::NP_3:
		return Gui::GuiKey::Code::Numpad3;
	case InputKey::NP_4:
		return Gui::GuiKey::Code::Numpad4;
	case InputKey::NP_5:
		return Gui::GuiKey::Code::Numpad5;
	case InputKey::NP_6:
		return Gui::GuiKey::Code::Numpad6;
	case InputKey::NP_7:
		return Gui::GuiKey::Code::Numpad7;
	case InputKey::NP_8:
		return Gui::GuiKey::Code::Numpad8;
	case InputKey::NP_9:
		return Gui::GuiKey::Code::Numpad9;
	case InputKey::MULTIPLY:
		return Gui::GuiKey::Code::Multiply;
	case InputKey::ADD:
		return Gui::GuiKey::Code::Add;
	case InputKey::SUBTRACT:
		return Gui::GuiKey::Code::Subtract;
	case InputKey::DECIMAL:
		return Gui::GuiKey::Code::Decimal;
	case InputKey::DIVIDE:
		return Gui::GuiKey::Code::Divide;
	case InputKey::F1:
		return Gui::GuiKey::Code::F1;
	case InputKey::F2:
		return Gui::GuiKey::Code::F2;
	case InputKey::F3:
		return Gui::GuiKey::Code::F3;
	case InputKey::F4:
		return Gui::GuiKey::Code::F4;
	case InputKey::F5:
		return Gui::GuiKey::Code::F5;
	case InputKey::F6:
		return Gui::GuiKey::Code::F6;
	case InputKey::F7:
		return Gui::GuiKey::Code::F7;
	case InputKey::F8:
		return Gui::GuiKey::Code::F8;
	case InputKey::F9:
		return Gui::GuiKey::Code::F9;
	case InputKey::F10:
		return Gui::GuiKey::Code::F10;
	case InputKey::F11:
		return Gui::GuiKey::Code::F11;
	case InputKey::F12:
		return Gui::GuiKey::Code::F12;
	case InputKey::F13:
		return Gui::GuiKey::Code::F13;
	case InputKey::F14:
		return Gui::GuiKey::Code::F14;
	case InputKey::F15:
		return Gui::GuiKey::Code::F15;
	case InputKey::NUMLOCK:
		return Gui::GuiKey::Code::NumLock;
	case InputKey::SCROLLLOCK:
		return Gui::GuiKey::Code::ScrollLock;
	case InputKey::SEMICOLON:
		return Gui::GuiKey::Code::Semicolon;
	case InputKey::SLASH:
		return Gui::GuiKey::Code::Slash;
	case InputKey::GRAVE:
		return Gui::GuiKey::Code::Grave;
	case InputKey::LBRACKET:
		return Gui::GuiKey::Code::LeftBracket;
	case InputKey::RBRACKET:
		return Gui::GuiKey::Code::RightBracket;
	case InputKey::BACKSLASH:
		return Gui::GuiKey::Code::Backslash;
	case InputKey::COMMA:
		return Gui::GuiKey::Code::Comma;
	case InputKey::UNDERLINE:
		return Gui::GuiKey::Code::Minus;
	case InputKey::PERIOD:
		return Gui::GuiKey::Code::Period;
	case InputKey::EQUALS:
		return Gui::GuiKey::Code::Equals;
	case InputKey::LSHIFT:
		return Gui::GuiKey::Code::LeftShift;
	case InputKey::RSHIFT:
		return Gui::GuiKey::Code::RightShift;
	case InputKey::LCTRL:
		return Gui::GuiKey::Code::LeftControl;
	case InputKey::RCTRL:
		return Gui::GuiKey::Code::RightControl;
	case InputKey::LWIN:
		return Gui::GuiKey::Code::LeftWindows;
	case InputKey::RWIN:
		return Gui::GuiKey::Code::RightWindow;
	case InputKey::VOLUMEDOWN:
		return Gui::GuiKey::Code::VolumeDown;
	case InputKey::VOLUMEUP:
		return Gui::GuiKey::Code::VolumeUp;
	case InputKey::Key0:
		return Gui::GuiKey::Code::Zero;
	case InputKey::Key1:
		return Gui::GuiKey::Code::One;
	case InputKey::Key2:
		return Gui::GuiKey::Code::Two;
	case InputKey::Key3:
		return Gui::GuiKey::Code::Three;
	case InputKey::Key4:
		return Gui::GuiKey::Code::Four;
	case InputKey::Key5:
		return Gui::GuiKey::Code::Five;
	case InputKey::Key6:
		return Gui::GuiKey::Code::Six;
	case InputKey::Key7:
		return Gui::GuiKey::Code::Seven;
	case InputKey::Key8:
		return Gui::GuiKey::Code::Eight;
	case InputKey::Key9:
		return Gui::GuiKey::Code::Nine;
	case InputKey::A:
		return Gui::GuiKey::Code::A;
	case InputKey::B:
		return Gui::GuiKey::Code::B;
	case InputKey::C:
		return Gui::GuiKey::Code::C;
	case InputKey::D:
		return Gui::GuiKey::Code::D;
	case InputKey::E:
		return Gui::GuiKey::Code::E;
	case InputKey::F:
		return Gui::GuiKey::Code::F;
	case InputKey::G:
		return Gui::GuiKey::Code::G;
	case InputKey::H:
		return Gui::GuiKey::Code::H;
	case InputKey::I:
		return Gui::GuiKey::Code::I;
	case InputKey::J:
		return Gui::GuiKey::Code::J;
	case InputKey::K:
		return Gui::GuiKey::Code::K;
	case InputKey::L:
		return Gui::GuiKey::Code::L;
	case InputKey::M:
		return Gui::GuiKey::Code::M;
	case InputKey::N:
		return Gui::GuiKey::Code::N;
	case InputKey::O:
		return Gui::GuiKey::Code::O;
	case InputKey::P:
		return Gui::GuiKey::Code::P;
	case InputKey::Q:
		return Gui::GuiKey::Code::Q;
	case InputKey::R:
		return Gui::GuiKey::Code::R;
	case InputKey::S:
		return Gui::GuiKey::Code::S;
	case InputKey::T:
		return Gui::GuiKey::Code::T;
	case InputKey::U:
		return Gui::GuiKey::Code::U;
	case InputKey::V:
		return Gui::GuiKey::Code::V;
	case InputKey::W:
		return Gui::GuiKey::Code::W;
	case InputKey::X:
		return Gui::GuiKey::Code::X;
	case InputKey::Y:
		return Gui::GuiKey::Code::Y;
	case InputKey::Z:
		return Gui::GuiKey::Code::Z;
	}

	return Gui::GuiKey::Code::None;
}

class ImageLoader : public Gui::GuiImageLoader
{
public:
	bool loadImage(int& _width, int& _height, Gui::PixelFormat& _format, const std::string& _filename, MemoryStream& stream)
	{
		ray::Image image;
		if (image.load(_filename))
		{
			_width = image.width();
			_height = image.height();

			if (image.bpp() == 32)
				_format = Gui::PixelFormat::R8G8B8A8;
			else if (image.bpp() == 24)
				_format = Gui::PixelFormat::R8G8B8;
			else
			{
				assert(false);
			}

			size_t size = image.size();

			stream.resize(size);
			stream.write((char*)image.data(), size);
			return true;
		}

		return false;
	}

	void saveImage(int _width, int _height, Gui::PixelFormat _format, void* _texture, const std::string& _filename)
	{
	}

};

GuiFeature::GuiFeature() noexcept
	: _width(0)
	, _height(0)
{
}

GuiFeature::GuiFeature(std::uint32_t w, std::uint32_t h) noexcept
	: _width(w)
	, _height(h)
{
}

GuiFeature::~GuiFeature() noexcept
{
}

void
GuiFeature::render() except
{
	if (_platform)
		_platform->render(this->getGameServer()->getTimer()->delta());
}

void
GuiFeature::onActivate() except
{
	_platform = std::make_shared<Gui::MyGuiSystem>();
	_platform->open();
	_platform->setImageLoader(std::make_shared<ImageLoader>());
	_platform->setCoreProfile("sys:media/ui/MyGUI_Core.xml");
	_platform->setViewport(_width, _height);
}

void
GuiFeature::onDeactivate() except
{
	if (_platform)
	{
		_platform.reset();
		_platform = nullptr;
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