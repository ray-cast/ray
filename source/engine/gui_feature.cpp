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
#include <ray/input_feature.h>
#include <ray/input_event.h>
#include <ray/timer.h>
#include <ray/image.h>

#include <ray/game_server.h>
#include <ray/game_application.h>

_NAME_BEGIN

__ImplementSubClass(GuiFeature, GameFeature, "GuiFeature")

GuiInputButton::Code ButtonCodeToGuiButton(InputButton::Code button) noexcept
{
	switch (button)
	{
	case InputButton::LEFT:
		return GuiInputButton::Code::Left;
	case InputButton::RIGHT:
		return GuiInputButton::Code::Right;
	case InputButton::MIDDLE:
		return GuiInputButton::Code::Middle;
	case InputButton::MOUSE3:
		return GuiInputButton::Code::Button3;
	case InputButton::MOUSE4:
		return GuiInputButton::Code::Button4;
	case InputButton::MOUSE5:
		return GuiInputButton::Code::Button5;
	case InputButton::MOUSE6:
		return GuiInputButton::Code::Button6;
	case InputButton::MOUSE7:
		return GuiInputButton::Code::Button7;
	case InputButton::MOUSE8:
	case InputButton::MOUSELAST:
	case InputButton::MOUSEWHEEL:
	case InputButton::MOUSEX:
	case InputButton::MOUSEY:
	case InputButton::MOUSEZ:
		return GuiInputButton::Code::None;
	}

	return GuiInputButton::Code::None;
}

GuiInputKey::Code KeyCodetoGuiKey(InputKey::Code key) noexcept
{
	switch (key)
	{
	case InputKey::None:
		return GuiInputKey::Code::None;
	case InputKey::BACKSPACE:
		return GuiInputKey::Code::Backspace;
	case InputKey::TAB:
		return GuiInputKey::Code::Tab;
	case InputKey::ENTER:
		return GuiInputKey::Code::Return;
	case InputKey::PAUSE:
		return GuiInputKey::Code::Pause;
	case InputKey::ESCAPE:
		return GuiInputKey::Code::Escape;
	case InputKey::SPACE:
		return GuiInputKey::Code::Space;
	case InputKey::PGDN:
		return GuiInputKey::Code::NextTrack;
	case InputKey::END:
		return GuiInputKey::Code::End;
	case InputKey::HOME:
		return GuiInputKey::Code::Home;
	case InputKey::LEFT:
		return GuiInputKey::Code::ArrowLeft;
	case InputKey::RIGHT:
		return GuiInputKey::Code::ArrowRight;
	case InputKey::UP:
		return GuiInputKey::Code::ArrowUp;
	case InputKey::DOWN:
		return GuiInputKey::Code::ArrowDown;
	case InputKey::INSERT:
		return GuiInputKey::Code::Insert;
	case InputKey::DELETE:
		return GuiInputKey::Code::Delete;
	case InputKey::NP_0:
		return GuiInputKey::Code::Numpad0;
	case InputKey::NP_1:
		return GuiInputKey::Code::Numpad1;
	case InputKey::NP_2:
		return GuiInputKey::Code::Numpad2;
	case InputKey::NP_3:
		return GuiInputKey::Code::Numpad3;
	case InputKey::NP_4:
		return GuiInputKey::Code::Numpad4;
	case InputKey::NP_5:
		return GuiInputKey::Code::Numpad5;
	case InputKey::NP_6:
		return GuiInputKey::Code::Numpad6;
	case InputKey::NP_7:
		return GuiInputKey::Code::Numpad7;
	case InputKey::NP_8:
		return GuiInputKey::Code::Numpad8;
	case InputKey::NP_9:
		return GuiInputKey::Code::Numpad9;
	case InputKey::MULTIPLY:
		return GuiInputKey::Code::Multiply;
	case InputKey::ADD:
		return GuiInputKey::Code::Add;
	case InputKey::SUBTRACT:
		return GuiInputKey::Code::Subtract;
	case InputKey::DECIMAL:
		return GuiInputKey::Code::Decimal;
	case InputKey::DIVIDE:
		return GuiInputKey::Code::Divide;
	case InputKey::F1:
		return GuiInputKey::Code::F1;
	case InputKey::F2:
		return GuiInputKey::Code::F2;
	case InputKey::F3:
		return GuiInputKey::Code::F3;
	case InputKey::F4:
		return GuiInputKey::Code::F4;
	case InputKey::F5:
		return GuiInputKey::Code::F5;
	case InputKey::F6:
		return GuiInputKey::Code::F6;
	case InputKey::F7:
		return GuiInputKey::Code::F7;
	case InputKey::F8:
		return GuiInputKey::Code::F8;
	case InputKey::F9:
		return GuiInputKey::Code::F9;
	case InputKey::F10:
		return GuiInputKey::Code::F10;
	case InputKey::F11:
		return GuiInputKey::Code::F11;
	case InputKey::F12:
		return GuiInputKey::Code::F12;
	case InputKey::F13:
		return GuiInputKey::Code::F13;
	case InputKey::F14:
		return GuiInputKey::Code::F14;
	case InputKey::F15:
		return GuiInputKey::Code::F15;
	case InputKey::NUMLOCK:
		return GuiInputKey::Code::NumLock;
	case InputKey::SCROLLLOCK:
		return GuiInputKey::Code::ScrollLock;
	case InputKey::SEMICOLON:
		return GuiInputKey::Code::Semicolon;
	case InputKey::SLASH:
		return GuiInputKey::Code::Slash;
	case InputKey::GRAVE:
		return GuiInputKey::Code::Grave;
	case InputKey::LBRACKET:
		return GuiInputKey::Code::LeftBracket;
	case InputKey::RBRACKET:
		return GuiInputKey::Code::RightBracket;
	case InputKey::BACKSLASH:
		return GuiInputKey::Code::Backslash;
	case InputKey::COMMA:
		return GuiInputKey::Code::Comma;
	case InputKey::UNDERLINE:
		return GuiInputKey::Code::Minus;
	case InputKey::PERIOD:
		return GuiInputKey::Code::Period;
	case InputKey::EQUALS:
		return GuiInputKey::Code::Equals;
	case InputKey::LSHIFT:
		return GuiInputKey::Code::LeftShift;
	case InputKey::RSHIFT:
		return GuiInputKey::Code::RightShift;
	case InputKey::LCTRL:
		return GuiInputKey::Code::LeftControl;
	case InputKey::RCTRL:
		return GuiInputKey::Code::RightControl;
	case InputKey::LWIN:
		return GuiInputKey::Code::LeftWindows;
	case InputKey::RWIN:
		return GuiInputKey::Code::RightWindow;
	case InputKey::VOLUMEDOWN:
		return GuiInputKey::Code::VolumeDown;
	case InputKey::VOLUMEUP:
		return GuiInputKey::Code::VolumeUp;
	case InputKey::Key0:
		return GuiInputKey::Code::Zero;
	case InputKey::Key1:
		return GuiInputKey::Code::One;
	case InputKey::Key2:
		return GuiInputKey::Code::Two;
	case InputKey::Key3:
		return GuiInputKey::Code::Three;
	case InputKey::Key4:
		return GuiInputKey::Code::Four;
	case InputKey::Key5:
		return GuiInputKey::Code::Five;
	case InputKey::Key6:
		return GuiInputKey::Code::Six;
	case InputKey::Key7:
		return GuiInputKey::Code::Seven;
	case InputKey::Key8:
		return GuiInputKey::Code::Eight;
	case InputKey::Key9:
		return GuiInputKey::Code::Nine;
	case InputKey::A:
		return GuiInputKey::Code::A;
	case InputKey::B:
		return GuiInputKey::Code::B;
	case InputKey::C:
		return GuiInputKey::Code::C;
	case InputKey::D:
		return GuiInputKey::Code::D;
	case InputKey::E:
		return GuiInputKey::Code::E;
	case InputKey::F:
		return GuiInputKey::Code::F;
	case InputKey::G:
		return GuiInputKey::Code::G;
	case InputKey::H:
		return GuiInputKey::Code::H;
	case InputKey::I:
		return GuiInputKey::Code::I;
	case InputKey::J:
		return GuiInputKey::Code::J;
	case InputKey::K:
		return GuiInputKey::Code::K;
	case InputKey::L:
		return GuiInputKey::Code::L;
	case InputKey::M:
		return GuiInputKey::Code::M;
	case InputKey::N:
		return GuiInputKey::Code::N;
	case InputKey::O:
		return GuiInputKey::Code::O;
	case InputKey::P:
		return GuiInputKey::Code::P;
	case InputKey::Q:
		return GuiInputKey::Code::Q;
	case InputKey::R:
		return GuiInputKey::Code::R;
	case InputKey::S:
		return GuiInputKey::Code::S;
	case InputKey::T:
		return GuiInputKey::Code::T;
	case InputKey::U:
		return GuiInputKey::Code::U;
	case InputKey::V:
		return GuiInputKey::Code::V;
	case InputKey::W:
		return GuiInputKey::Code::W;
	case InputKey::X:
		return GuiInputKey::Code::X;
	case InputKey::Y:
		return GuiInputKey::Code::Y;
	case InputKey::Z:
		return GuiInputKey::Code::Z;
	default:
		return GuiInputKey::Code::None;
	}
}

class ImageLoader : public GuiImageLoader
{
public:
	bool loadImage(int& _width, int& _height, PixelFormat& _format, const std::string& _filename, MemoryStream& stream)
	{
		ray::Image image;
		if (image.load(_filename))
		{
			_width = image.width();
			_height = image.height();

			if (image.bpp() == 32)
				_format = PixelFormat::R8G8B8A8;
			else if (image.bpp() == 24)
				_format = PixelFormat::R8G8B8;
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

	void saveImage(int _width, int _height, PixelFormat _format, void* _texture, const std::string& _filename)
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
GuiFeature::setCoreProfile(const std::string& core) except
{
	assert(_platform);
	_platform->setCoreProfile(core);
}

const std::string& 
GuiFeature::getCoreProfile() const noexcept
{
	assert(_platform);
	return _platform->getCoreProfile();
}

void 
GuiFeature::setImageLoader(GuiImageLoaderPtr loader) noexcept
{
	assert(_platform);
	_platform->setImageLoader(loader);
}

GuiImageLoaderPtr 
GuiFeature::getImageLoader() const noexcept
{
	assert(_platform);
	return _platform->getImageLoader();
}

void 
GuiFeature::setViewport(int w, int h) noexcept
{
	assert(_platform);
	_platform->setViewport(w, h);
}

void 
GuiFeature::getViewport(int& w, int& h) noexcept
{
	assert(_platform);
	_platform->getViewport(w, h);
}

void
GuiFeature::onActivate() except
{
	_platform = GuiSystem::instance();
	if (!_platform->open())
		throw failure("GuiSystem::open() fail");

	_platform->setImageLoader(std::make_shared<ImageLoader>());
	_platform->setCoreProfile("sys:media/ui/MyGUI_Core.xml");
	_platform->setViewport(_width, _height);
}

void
GuiFeature::onDeactivate() except
{
	if (_platform)
		_platform = nullptr;
}

void
GuiFeature::onWindowSizeChange() except
{
	std::uint32_t w, h;
	this->getGameServer()->getGameApp()->getWindowResolution(w, h);
	_platform->setViewport(w, h);
}

void
GuiFeature::onMessage(const MessagePtr& message) except
{
	assert(_platform);

	if (message->isInstanceOf<InputMessage>())
	{
		auto inputMessage = message->downcast<InputMessage>();
		if (inputMessage)
		{
			InputEvent event = inputMessage->getEvent();
			switch (event.event)
			{
			case InputEvent::MouseMotion:
				_platform->injectMouseMove(event.motion.x, event.motion.y, 0);
				break;
			case InputEvent::MouseButtonDown:
				_platform->injectMousePress(event.button.x, event.button.y, ButtonCodeToGuiButton((InputButton::Code)event.button.button));
				break;
			case InputEvent::MouseButtonUp:
				_platform->injectMouseRelease(event.button.x, event.button.y, ButtonCodeToGuiButton((InputButton::Code)event.button.button));
				break;
			case InputEvent::KeyDown:
				_platform->injectKeyPress(KeyCodetoGuiKey((InputKey::Code)event.key.keysym.sym), event.key.keysym.unicode);
				break;
			case InputEvent::KeyUp:
				_platform->injectKeyRelease(KeyCodetoGuiKey((InputKey::Code)event.key.keysym.sym));
				break;
			case InputEvent::Character:
				_platform->injectKeyPress(KeyCodetoGuiKey(InputKey::Code::None), event.key.keysym.unicode);
			}
		}
	}
}

_NAME_END
#endif