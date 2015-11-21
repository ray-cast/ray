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
#include <ray/gui_system.h>
#include <ray/gui_assert.h>
#include <ray/gui_renderer.h>
#include <ray/gui_manager.h>

#include <MyGUI.h>
#include <MyGUI_LogManager.h>

_NAME_BEGIN

using namespace Gui;

__ImplementSubClass(MyGuiSystem, GuiSystem, "MyGuiSystem")

MyGUI::MouseButton GuiButtonToMyGUI(GuiButton::Code button)
{
	switch (button)
	{
	case ray::GuiButton::None:
		return MyGUI::MouseButton::None;
		break;
	case ray::GuiButton::Button0:
		return MyGUI::MouseButton::Button0;
		break;
	case ray::GuiButton::Button1:
		return MyGUI::MouseButton::Button1;
		break;
	case ray::GuiButton::Button2:
		return MyGUI::MouseButton::Button2;
		break;
	case ray::GuiButton::Button3:
		return MyGUI::MouseButton::Button3;
		break;
	case ray::GuiButton::Button4:
		return MyGUI::MouseButton::Button4;
		break;
	case ray::GuiButton::Button5:
		return MyGUI::MouseButton::Button5;
		break;
	case ray::GuiButton::Button6:
		return MyGUI::MouseButton::Button6;
		break;
	case ray::GuiButton::Button7:
		return MyGUI::MouseButton::Button7;
		break;
	default:
		break;
	}

	return MyGUI::MouseButton::None;
}

MyGUI::KeyCode GuiKeyToMyGUI(GuiKey::Code key)
{
	switch (key)
	{
	case GuiKey::Code::None: return MyGUI::KeyCode::None;
	case GuiKey::Code::Escape: return MyGUI::KeyCode::Escape;
	case GuiKey::Code::One: return MyGUI::KeyCode::One;
	case GuiKey::Code::Two: return MyGUI::KeyCode::Two;
	case GuiKey::Code::Three: return MyGUI::KeyCode::Three;
	case GuiKey::Code::Four: return MyGUI::KeyCode::Four;
	case GuiKey::Code::Five: return MyGUI::KeyCode::Five;
	case GuiKey::Code::Six: return MyGUI::KeyCode::Six;
	case GuiKey::Code::Seven: return MyGUI::KeyCode::Seven;
	case GuiKey::Code::Eight: return MyGUI::KeyCode::Eight;
	case GuiKey::Code::Nine: return MyGUI::KeyCode::Nine;
	case GuiKey::Code::Zero: return MyGUI::KeyCode::Zero;
	case GuiKey::Code::Minus: return MyGUI::KeyCode::Minus;
	case GuiKey::Code::Equals: return MyGUI::KeyCode::Equals;
	case GuiKey::Code::Backspace: return MyGUI::KeyCode::Backspace;
	case GuiKey::Code::Tab: return MyGUI::KeyCode::Tab;
	case GuiKey::Code::Q: return MyGUI::KeyCode::Q;
	case GuiKey::Code::W: return MyGUI::KeyCode::W;
	case GuiKey::Code::E: return MyGUI::KeyCode::E;
	case GuiKey::Code::R: return MyGUI::KeyCode::R;
	case GuiKey::Code::T: return MyGUI::KeyCode::T;
	case GuiKey::Code::Y: return MyGUI::KeyCode::Y;
	case GuiKey::Code::U: return MyGUI::KeyCode::U;
	case GuiKey::Code::I: return MyGUI::KeyCode::I;
	case GuiKey::Code::O: return MyGUI::KeyCode::O;
	case GuiKey::Code::P: return MyGUI::KeyCode::P;
	case GuiKey::Code::LeftBracket: return MyGUI::KeyCode::LeftBracket;
	case GuiKey::Code::RightBracket: return MyGUI::KeyCode::RightBracket;
	case GuiKey::Code::Return: return MyGUI::KeyCode::Return;
	case GuiKey::Code::LeftControl: return MyGUI::KeyCode::LeftControl;
	case GuiKey::Code::A: return MyGUI::KeyCode::A;
	case GuiKey::Code::S: return MyGUI::KeyCode::S;
	case GuiKey::Code::D: return MyGUI::KeyCode::D;
	case GuiKey::Code::F: return MyGUI::KeyCode::F;
	case GuiKey::Code::G: return MyGUI::KeyCode::G;
	case GuiKey::Code::H: return MyGUI::KeyCode::H;
	case GuiKey::Code::J: return MyGUI::KeyCode::J;
	case GuiKey::Code::K: return MyGUI::KeyCode::K;
	case GuiKey::Code::L: return MyGUI::KeyCode::L;
	case GuiKey::Code::Semicolon: return MyGUI::KeyCode::Semicolon;
	case GuiKey::Code::Apostrophe: return MyGUI::KeyCode::Apostrophe;
	case GuiKey::Code::Grave: return MyGUI::KeyCode::Grave;
	case GuiKey::Code::LeftShift: return MyGUI::KeyCode::LeftShift;
	case GuiKey::Code::Backslash: return MyGUI::KeyCode::Backslash;
	case GuiKey::Code::Z: return MyGUI::KeyCode::Z;
	case GuiKey::Code::X: return MyGUI::KeyCode::X;
	case GuiKey::Code::C: return MyGUI::KeyCode::C;
	case GuiKey::Code::V: return MyGUI::KeyCode::V;
	case GuiKey::Code::B: return MyGUI::KeyCode::B;
	case GuiKey::Code::N: return MyGUI::KeyCode::N;
	case GuiKey::Code::M: return MyGUI::KeyCode::M;
	case GuiKey::Code::Comma: return MyGUI::KeyCode::Comma;
	case GuiKey::Code::Period: return MyGUI::KeyCode::Period;
	case GuiKey::Code::Slash: return MyGUI::KeyCode::Slash;
	case GuiKey::Code::RightShift: return MyGUI::KeyCode::RightShift;
	case GuiKey::Code::Multiply: return MyGUI::KeyCode::Multiply;
	case GuiKey::Code::LeftAlt: return MyGUI::KeyCode::LeftAlt;
	case GuiKey::Code::Space: return MyGUI::KeyCode::Space;
	case GuiKey::Code::Capital: return MyGUI::KeyCode::Capital;
	case GuiKey::Code::F1: return MyGUI::KeyCode::F1;
	case GuiKey::Code::F2: return MyGUI::KeyCode::F2;
	case GuiKey::Code::F3: return MyGUI::KeyCode::F3;
	case GuiKey::Code::F4: return MyGUI::KeyCode::F4;
	case GuiKey::Code::F5: return MyGUI::KeyCode::F5;
	case GuiKey::Code::F6: return MyGUI::KeyCode::F6;
	case GuiKey::Code::F7: return MyGUI::KeyCode::F7;
	case GuiKey::Code::F8: return MyGUI::KeyCode::F8;
	case GuiKey::Code::F9: return MyGUI::KeyCode::F9;
	case GuiKey::Code::F10: return MyGUI::KeyCode::F10;
	case GuiKey::Code::NumLock: return MyGUI::KeyCode::NumLock;
	case GuiKey::Code::ScrollLock: return MyGUI::KeyCode::ScrollLock;
	case GuiKey::Code::Numpad7: return MyGUI::KeyCode::Numpad7;
	case GuiKey::Code::Numpad8: return MyGUI::KeyCode::Numpad8;
	case GuiKey::Code::Numpad9: return MyGUI::KeyCode::Numpad9;
	case GuiKey::Code::Subtract: return MyGUI::KeyCode::Subtract;
	case GuiKey::Code::Numpad4: return MyGUI::KeyCode::Numpad4;
	case GuiKey::Code::Numpad5: return MyGUI::KeyCode::Numpad5;
	case GuiKey::Code::Numpad6: return MyGUI::KeyCode::Numpad6;
	case GuiKey::Code::Add: return MyGUI::KeyCode::Add;
	case GuiKey::Code::Numpad1: return MyGUI::KeyCode::Numpad1;
	case GuiKey::Code::Numpad2: return MyGUI::KeyCode::Numpad2;
	case GuiKey::Code::Numpad3: return MyGUI::KeyCode::Numpad3;
	case GuiKey::Code::Numpad0: return MyGUI::KeyCode::Numpad0;
	case GuiKey::Code::Decimal: return MyGUI::KeyCode::Decimal;
	case GuiKey::Code::OEM_102: return MyGUI::KeyCode::OEM_102;
	case GuiKey::Code::F11: return MyGUI::KeyCode::F11;
	case GuiKey::Code::F12: return MyGUI::KeyCode::F12;
	case GuiKey::Code::F13: return MyGUI::KeyCode::F13;
	case GuiKey::Code::F14: return MyGUI::KeyCode::F14;
	case GuiKey::Code::F15: return MyGUI::KeyCode::F15;
	case GuiKey::Code::Kana: return MyGUI::KeyCode::Kana;
	case GuiKey::Code::ABNT_C1: return MyGUI::KeyCode::ABNT_C1;
	case GuiKey::Code::Convert: return MyGUI::KeyCode::Convert;
	case GuiKey::Code::NoConvert: return MyGUI::KeyCode::NoConvert;
	case GuiKey::Code::Yen: return MyGUI::KeyCode::Yen;
	case GuiKey::Code::ABNT_C2: return MyGUI::KeyCode::ABNT_C2;
	case GuiKey::Code::NumpadEquals: return MyGUI::KeyCode::NumpadEquals;
	case GuiKey::Code::PrevTrack: return MyGUI::KeyCode::PrevTrack;
	case GuiKey::Code::At: return MyGUI::KeyCode::At;
	case GuiKey::Code::Colon: return MyGUI::KeyCode::Colon;
	case GuiKey::Code::Underline: return MyGUI::KeyCode::Underline;
	case GuiKey::Code::Kanji: return MyGUI::KeyCode::Kanji;
	case GuiKey::Code::Stop: return MyGUI::KeyCode::Stop;
	case GuiKey::Code::AX: return MyGUI::KeyCode::AX;
	case GuiKey::Code::Unlabeled: return MyGUI::KeyCode::Unlabeled;
	case GuiKey::Code::NextTrack: return MyGUI::KeyCode::NextTrack;
	case GuiKey::Code::NumpadEnter: return MyGUI::KeyCode::NumpadEnter;
	case GuiKey::Code::RightControl: return MyGUI::KeyCode::RightControl;
	case GuiKey::Code::Mute: return MyGUI::KeyCode::Mute;
	case GuiKey::Code::Calculator: return MyGUI::KeyCode::Calculator;
	case GuiKey::Code::PlayPause: return MyGUI::KeyCode::PlayPause;
	case GuiKey::Code::MediaStop: return MyGUI::KeyCode::MediaStop;
	case GuiKey::Code::VolumeDown: return MyGUI::KeyCode::VolumeDown;
	case GuiKey::Code::VolumeUp: return MyGUI::KeyCode::VolumeUp;
	case GuiKey::Code::WebHome: return MyGUI::KeyCode::WebHome;
	case GuiKey::Code::NumpadComma: return MyGUI::KeyCode::NumpadComma;
	case GuiKey::Code::Divide: return MyGUI::KeyCode::Divide;
	case GuiKey::Code::SysRq: return MyGUI::KeyCode::SysRq;
	case GuiKey::Code::RightAlt: return MyGUI::KeyCode::RightAlt;
	case GuiKey::Code::Pause: return MyGUI::KeyCode::Pause;
	case GuiKey::Code::Home: return MyGUI::KeyCode::Home;
	case GuiKey::Code::ArrowUp: return MyGUI::KeyCode::ArrowUp;
	case GuiKey::Code::PageUp: return MyGUI::KeyCode::PageUp;
	case GuiKey::Code::ArrowLeft: return MyGUI::KeyCode::ArrowLeft;
	case GuiKey::Code::ArrowRight: return MyGUI::KeyCode::ArrowRight;
	case GuiKey::Code::End: return MyGUI::KeyCode::End;
	case GuiKey::Code::ArrowDown: return MyGUI::KeyCode::ArrowDown;
	case GuiKey::Code::PageDown: return MyGUI::KeyCode::PageDown;
	case GuiKey::Code::Insert: return MyGUI::KeyCode::Insert;
	case GuiKey::Code::Delete: return MyGUI::KeyCode::Delete;
	case GuiKey::Code::LeftWindows: return MyGUI::KeyCode::LeftWindows;
	case GuiKey::Code::RightWindow: return MyGUI::KeyCode::RightWindow;
	case GuiKey::Code::AppMenu: return MyGUI::KeyCode::AppMenu;
	case GuiKey::Code::Power: return MyGUI::KeyCode::Power;
	case GuiKey::Code::Sleep: return MyGUI::KeyCode::Sleep;
	case GuiKey::Code::Wake: return MyGUI::KeyCode::Wake;
	case GuiKey::Code::WebSearch: return MyGUI::KeyCode::WebSearch;
	case GuiKey::Code::WebFavorites: return MyGUI::KeyCode::WebFavorites;
	case GuiKey::Code::WebRefresh: return MyGUI::KeyCode::WebRefresh;
	case GuiKey::Code::WebStop: return MyGUI::KeyCode::WebStop;
	case GuiKey::Code::WebForward: return MyGUI::KeyCode::WebForward;
	case GuiKey::Code::WebBack: return MyGUI::KeyCode::WebBack;
	case GuiKey::Code::MyComputer: return MyGUI::KeyCode::MyComputer;
	case GuiKey::Code::Mail: return MyGUI::KeyCode::Mail;
	case GuiKey::Code::MediaSelect: return MyGUI::KeyCode::MediaSelect;
	}

	return MyGUI::KeyCode::None;
}

MyGuiSystem::MyGuiSystem() noexcept
	: _isInitialise(false)
	, _logManager(nullptr)
	, _renderer(nullptr)
	, _resLoader(nullptr)
{
}

MyGuiSystem::~MyGuiSystem() noexcept
{
	this->close();
}

bool
MyGuiSystem::open(GuiImageLoader* loader) noexcept
{
	assert(!_isInitialise);

	_logManager = new MyGUI::LogManager();
	_logManager->createDefaultSource(MYGUI_PLATFORM_LOG_FILENAME);

	_renderer = new GuiRenderer();
	_renderer->initialise();
	_renderer->setImageLoader(loader);

	_resLoader = new GuiResManager();
	_resLoader->initialise();
	_resLoader->addResourceLocation("E:/libraries/mygui-master/Media", false);
	_resLoader->addResourceLocation("E:/libraries/mygui-master/Media/MyGUI_Media", false);
	_resLoader->addResourceLocation("E:/libraries/mygui-master/Media/Common/Base", false);
	_resLoader->addResourceLocation("E:/libraries/mygui-master/Media/Demos/Demo_ScrollView", false);
	_resLoader->addResourceLocation("E:/libraries/mygui-master/Media/Common/Demos", false);

	_gui = new MyGUI::Gui();
	_gui->initialise("MyGUI_Core.xml");

	_isInitialise = true;

	return true;
}

void
MyGuiSystem::close() noexcept
{
	if (_isInitialise)
	{
		_isInitialise = false;

		_renderer->shutdown();
		_resLoader->shutdown();

		delete _gui;
		delete _renderer;
		delete _resLoader;
		delete _logManager;
	}
}

void
MyGuiSystem::setImageLoader(Gui::GuiImageLoader* loader) noexcept
{
	assert(_renderer);
	_renderer->setImageLoader(loader);
}

Gui::GuiImageLoader*
MyGuiSystem::getImageLoader() const noexcept
{
	assert(_renderer);
	return _renderer->getImageLoader();
}

bool
MyGuiSystem::injectMouseMove(int _absx, int _absy, int _absz) noexcept
{
	return MyGUI::InputManager::getInstance().injectMouseMove(_absx, _absy, _absz);
}

bool
MyGuiSystem::injectMousePress(int _absx, int _absy, GuiButton::Code _id) noexcept
{
	return MyGUI::InputManager::getInstance().injectMousePress(_absx, _absy, GuiButtonToMyGUI(_id));
}

bool
MyGuiSystem::injectMouseRelease(int _absx, int _absy, GuiButton::Code _id) noexcept
{
	return MyGUI::InputManager::getInstance().injectMouseRelease(_absx, _absy, GuiButtonToMyGUI(_id));
}

bool
MyGuiSystem::injectKeyPress(GuiKey::Code _key) noexcept
{
	return MyGUI::InputManager::getInstance().injectKeyPress(GuiKeyToMyGUI(_key));
}

bool
MyGuiSystem::injectKeyRelease(GuiKey::Code _key) noexcept
{
	return MyGUI::InputManager::getInstance().injectKeyRelease(GuiKeyToMyGUI(_key));
}

bool
MyGuiSystem::isFocusMouse() const noexcept
{
	return MyGUI::InputManager::getInstance().isFocusMouse();
}

bool
MyGuiSystem::isFocusKey() const noexcept
{
	return MyGUI::InputManager::getInstance().isFocusKey();
}

bool
MyGuiSystem::isCaptureMouse() const noexcept
{
	return MyGUI::InputManager::getInstance().isCaptureMouse();
}

void
MyGuiSystem::setViewport(int w, int h) noexcept
{
	assert(_renderer);
	_renderer->setViewSize(w, h);
}

void 
MyGuiSystem::getViewport(int& w, int& h) noexcept
{
	assert(_renderer);
	auto size = _renderer->getViewSize();
	w = size.width;
	h = size.height;
}

void
MyGuiSystem::render() noexcept
{
	assert(_renderer);
	_renderer->drawOneFrame();
}

_NAME_END