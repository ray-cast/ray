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
#if defined(_BUILD_MYGUI)
#include "mygui_system.h"
#include "mygui_renderer.h"
#include "mygui_manager.h"
#include "mygui_button.h"
#include "mygui_combobox.h"
#include "mygui_window.h"
#include "mygui_editbox.h"
#include "mygui_imagebox.h"
#include "mygui_textbox.h"
#include "mygui_scrollbar.h"
#include "mygui_scrollview.h"

_NAME_BEGIN

__ImplementSubClass(MyGuiSystem, GuiSystemBase, "MyGuiSystem")

MyGUI::MouseButton GuiButtonToMyGUI(GuiInputButton::Code button)
{
	switch (button)
	{
	case ray::GuiInputButton::None:
		return MyGUI::MouseButton::None;
		break;
	case ray::GuiInputButton::Button0:
		return MyGUI::MouseButton::Button0;
		break;
	case ray::GuiInputButton::Button1:
		return MyGUI::MouseButton::Button1;
		break;
	case ray::GuiInputButton::Button2:
		return MyGUI::MouseButton::Button2;
		break;
	case ray::GuiInputButton::Button3:
		return MyGUI::MouseButton::Button3;
		break;
	case ray::GuiInputButton::Button4:
		return MyGUI::MouseButton::Button4;
		break;
	case ray::GuiInputButton::Button5:
		return MyGUI::MouseButton::Button5;
		break;
	case ray::GuiInputButton::Button6:
		return MyGUI::MouseButton::Button6;
		break;
	case ray::GuiInputButton::Button7:
		return MyGUI::MouseButton::Button7;
		break;
	default:
		break;
	}

	return MyGUI::MouseButton::None;
}

MyGUI::KeyCode GuiKeyToMyGUI(GuiInputKey::Code key)
{
	switch (key)
	{
	case GuiInputKey::Code::None: return MyGUI::KeyCode::None;
	case GuiInputKey::Code::Escape: return MyGUI::KeyCode::Escape;
	case GuiInputKey::Code::One: return MyGUI::KeyCode::One;
	case GuiInputKey::Code::Two: return MyGUI::KeyCode::Two;
	case GuiInputKey::Code::Three: return MyGUI::KeyCode::Three;
	case GuiInputKey::Code::Four: return MyGUI::KeyCode::Four;
	case GuiInputKey::Code::Five: return MyGUI::KeyCode::Five;
	case GuiInputKey::Code::Six: return MyGUI::KeyCode::Six;
	case GuiInputKey::Code::Seven: return MyGUI::KeyCode::Seven;
	case GuiInputKey::Code::Eight: return MyGUI::KeyCode::Eight;
	case GuiInputKey::Code::Nine: return MyGUI::KeyCode::Nine;
	case GuiInputKey::Code::Zero: return MyGUI::KeyCode::Zero;
	case GuiInputKey::Code::Minus: return MyGUI::KeyCode::Minus;
	case GuiInputKey::Code::Equals: return MyGUI::KeyCode::Equals;
	case GuiInputKey::Code::Backspace: return MyGUI::KeyCode::Backspace;
	case GuiInputKey::Code::Tab: return MyGUI::KeyCode::Tab;
	case GuiInputKey::Code::Q: return MyGUI::KeyCode::Q;
	case GuiInputKey::Code::W: return MyGUI::KeyCode::W;
	case GuiInputKey::Code::E: return MyGUI::KeyCode::E;
	case GuiInputKey::Code::R: return MyGUI::KeyCode::R;
	case GuiInputKey::Code::T: return MyGUI::KeyCode::T;
	case GuiInputKey::Code::Y: return MyGUI::KeyCode::Y;
	case GuiInputKey::Code::U: return MyGUI::KeyCode::U;
	case GuiInputKey::Code::I: return MyGUI::KeyCode::I;
	case GuiInputKey::Code::O: return MyGUI::KeyCode::O;
	case GuiInputKey::Code::P: return MyGUI::KeyCode::P;
	case GuiInputKey::Code::LeftBracket: return MyGUI::KeyCode::LeftBracket;
	case GuiInputKey::Code::RightBracket: return MyGUI::KeyCode::RightBracket;
	case GuiInputKey::Code::Return: return MyGUI::KeyCode::Return;
	case GuiInputKey::Code::LeftControl: return MyGUI::KeyCode::LeftControl;
	case GuiInputKey::Code::A: return MyGUI::KeyCode::A;
	case GuiInputKey::Code::S: return MyGUI::KeyCode::S;
	case GuiInputKey::Code::D: return MyGUI::KeyCode::D;
	case GuiInputKey::Code::F: return MyGUI::KeyCode::F;
	case GuiInputKey::Code::G: return MyGUI::KeyCode::G;
	case GuiInputKey::Code::H: return MyGUI::KeyCode::H;
	case GuiInputKey::Code::J: return MyGUI::KeyCode::J;
	case GuiInputKey::Code::K: return MyGUI::KeyCode::K;
	case GuiInputKey::Code::L: return MyGUI::KeyCode::L;
	case GuiInputKey::Code::Semicolon: return MyGUI::KeyCode::Semicolon;
	case GuiInputKey::Code::Apostrophe: return MyGUI::KeyCode::Apostrophe;
	case GuiInputKey::Code::GraveAccent: return MyGUI::KeyCode::Grave;
	case GuiInputKey::Code::LeftShift: return MyGUI::KeyCode::LeftShift;
	case GuiInputKey::Code::Backslash: return MyGUI::KeyCode::Backslash;
	case GuiInputKey::Code::Z: return MyGUI::KeyCode::Z;
	case GuiInputKey::Code::X: return MyGUI::KeyCode::X;
	case GuiInputKey::Code::C: return MyGUI::KeyCode::C;
	case GuiInputKey::Code::V: return MyGUI::KeyCode::V;
	case GuiInputKey::Code::B: return MyGUI::KeyCode::B;
	case GuiInputKey::Code::N: return MyGUI::KeyCode::N;
	case GuiInputKey::Code::M: return MyGUI::KeyCode::M;
	case GuiInputKey::Code::Comma: return MyGUI::KeyCode::Comma;
	case GuiInputKey::Code::Period: return MyGUI::KeyCode::Period;
	case GuiInputKey::Code::Slash: return MyGUI::KeyCode::Slash;
	case GuiInputKey::Code::RightShift: return MyGUI::KeyCode::RightShift;
	case GuiInputKey::Code::Multiply: return MyGUI::KeyCode::Multiply;
	case GuiInputKey::Code::LeftAlt: return MyGUI::KeyCode::LeftAlt;
	case GuiInputKey::Code::Space: return MyGUI::KeyCode::Space;
	case GuiInputKey::Code::Capital: return MyGUI::KeyCode::Capital;
	case GuiInputKey::Code::F1: return MyGUI::KeyCode::F1;
	case GuiInputKey::Code::F2: return MyGUI::KeyCode::F2;
	case GuiInputKey::Code::F3: return MyGUI::KeyCode::F3;
	case GuiInputKey::Code::F4: return MyGUI::KeyCode::F4;
	case GuiInputKey::Code::F5: return MyGUI::KeyCode::F5;
	case GuiInputKey::Code::F6: return MyGUI::KeyCode::F6;
	case GuiInputKey::Code::F7: return MyGUI::KeyCode::F7;
	case GuiInputKey::Code::F8: return MyGUI::KeyCode::F8;
	case GuiInputKey::Code::F9: return MyGUI::KeyCode::F9;
	case GuiInputKey::Code::F10: return MyGUI::KeyCode::F10;
	case GuiInputKey::Code::NumLock: return MyGUI::KeyCode::NumLock;
	case GuiInputKey::Code::ScrollLock: return MyGUI::KeyCode::ScrollLock;
	case GuiInputKey::Code::Numpad7: return MyGUI::KeyCode::Numpad7;
	case GuiInputKey::Code::Numpad8: return MyGUI::KeyCode::Numpad8;
	case GuiInputKey::Code::Numpad9: return MyGUI::KeyCode::Numpad9;
	case GuiInputKey::Code::Subtract: return MyGUI::KeyCode::Subtract;
	case GuiInputKey::Code::Numpad4: return MyGUI::KeyCode::Numpad4;
	case GuiInputKey::Code::Numpad5: return MyGUI::KeyCode::Numpad5;
	case GuiInputKey::Code::Numpad6: return MyGUI::KeyCode::Numpad6;
	case GuiInputKey::Code::Add: return MyGUI::KeyCode::Add;
	case GuiInputKey::Code::Numpad1: return MyGUI::KeyCode::Numpad1;
	case GuiInputKey::Code::Numpad2: return MyGUI::KeyCode::Numpad2;
	case GuiInputKey::Code::Numpad3: return MyGUI::KeyCode::Numpad3;
	case GuiInputKey::Code::Numpad0: return MyGUI::KeyCode::Numpad0;
	case GuiInputKey::Code::Decimal: return MyGUI::KeyCode::Decimal;
	case GuiInputKey::Code::OEM_102: return MyGUI::KeyCode::OEM_102;
	case GuiInputKey::Code::F11: return MyGUI::KeyCode::F11;
	case GuiInputKey::Code::F12: return MyGUI::KeyCode::F12;
	case GuiInputKey::Code::F13: return MyGUI::KeyCode::F13;
	case GuiInputKey::Code::F14: return MyGUI::KeyCode::F14;
	case GuiInputKey::Code::F15: return MyGUI::KeyCode::F15;
	case GuiInputKey::Code::Kana: return MyGUI::KeyCode::Kana;
	case GuiInputKey::Code::ABNT_C1: return MyGUI::KeyCode::ABNT_C1;
	case GuiInputKey::Code::Convert: return MyGUI::KeyCode::Convert;
	case GuiInputKey::Code::NoConvert: return MyGUI::KeyCode::NoConvert;
	case GuiInputKey::Code::Yen: return MyGUI::KeyCode::Yen;
	case GuiInputKey::Code::ABNT_C2: return MyGUI::KeyCode::ABNT_C2;
	case GuiInputKey::Code::NumpadEquals: return MyGUI::KeyCode::NumpadEquals;
	case GuiInputKey::Code::PrevTrack: return MyGUI::KeyCode::PrevTrack;
	case GuiInputKey::Code::At: return MyGUI::KeyCode::At;
	case GuiInputKey::Code::Colon: return MyGUI::KeyCode::Colon;
	case GuiInputKey::Code::Underline: return MyGUI::KeyCode::Underline;
	case GuiInputKey::Code::Kanji: return MyGUI::KeyCode::Kanji;
	case GuiInputKey::Code::Stop: return MyGUI::KeyCode::Stop;
	case GuiInputKey::Code::AX: return MyGUI::KeyCode::AX;
	case GuiInputKey::Code::Unlabeled: return MyGUI::KeyCode::Unlabeled;
	case GuiInputKey::Code::NextTrack: return MyGUI::KeyCode::NextTrack;
	case GuiInputKey::Code::NumpadEnter: return MyGUI::KeyCode::NumpadEnter;
	case GuiInputKey::Code::RightControl: return MyGUI::KeyCode::RightControl;
	case GuiInputKey::Code::Mute: return MyGUI::KeyCode::Mute;
	case GuiInputKey::Code::Calculator: return MyGUI::KeyCode::Calculator;
	case GuiInputKey::Code::PlayPause: return MyGUI::KeyCode::PlayPause;
	case GuiInputKey::Code::MediaStop: return MyGUI::KeyCode::MediaStop;
	case GuiInputKey::Code::VolumeDown: return MyGUI::KeyCode::VolumeDown;
	case GuiInputKey::Code::VolumeUp: return MyGUI::KeyCode::VolumeUp;
	case GuiInputKey::Code::WebHome: return MyGUI::KeyCode::WebHome;
	case GuiInputKey::Code::NumpadComma: return MyGUI::KeyCode::NumpadComma;
	case GuiInputKey::Code::Divide: return MyGUI::KeyCode::Divide;
	case GuiInputKey::Code::SysRq: return MyGUI::KeyCode::SysRq;
	case GuiInputKey::Code::RightAlt: return MyGUI::KeyCode::RightAlt;
	case GuiInputKey::Code::Pause: return MyGUI::KeyCode::Pause;
	case GuiInputKey::Code::Home: return MyGUI::KeyCode::Home;
	case GuiInputKey::Code::ArrowUp: return MyGUI::KeyCode::ArrowUp;
	case GuiInputKey::Code::PageUp: return MyGUI::KeyCode::PageUp;
	case GuiInputKey::Code::ArrowLeft: return MyGUI::KeyCode::ArrowLeft;
	case GuiInputKey::Code::ArrowRight: return MyGUI::KeyCode::ArrowRight;
	case GuiInputKey::Code::End: return MyGUI::KeyCode::End;
	case GuiInputKey::Code::ArrowDown: return MyGUI::KeyCode::ArrowDown;
	case GuiInputKey::Code::PageDown: return MyGUI::KeyCode::PageDown;
	case GuiInputKey::Code::Insert: return MyGUI::KeyCode::Insert;
	case GuiInputKey::Code::Delete: return MyGUI::KeyCode::Delete;
	case GuiInputKey::Code::LeftWindows: return MyGUI::KeyCode::LeftWindows;
	case GuiInputKey::Code::RightWindow: return MyGUI::KeyCode::RightWindow;
	case GuiInputKey::Code::AppMenu: return MyGUI::KeyCode::AppMenu;
	case GuiInputKey::Code::Power: return MyGUI::KeyCode::Power;
	case GuiInputKey::Code::Sleep: return MyGUI::KeyCode::Sleep;
	case GuiInputKey::Code::Wake: return MyGUI::KeyCode::Wake;
	case GuiInputKey::Code::WebSearch: return MyGUI::KeyCode::WebSearch;
	case GuiInputKey::Code::WebFavorites: return MyGUI::KeyCode::WebFavorites;
	case GuiInputKey::Code::WebRefresh: return MyGUI::KeyCode::WebRefresh;
	case GuiInputKey::Code::WebStop: return MyGUI::KeyCode::WebStop;
	case GuiInputKey::Code::WebForward: return MyGUI::KeyCode::WebForward;
	case GuiInputKey::Code::WebBack: return MyGUI::KeyCode::WebBack;
	case GuiInputKey::Code::MyComputer: return MyGUI::KeyCode::MyComputer;
	case GuiInputKey::Code::Mail: return MyGUI::KeyCode::Mail;
	case GuiInputKey::Code::MediaSelect: return MyGUI::KeyCode::MediaSelect;
	}

	return MyGUI::KeyCode::None;
}

MyGuiSystem::MyGuiSystem() noexcept
	: _isInitialise(false)
	, _renderer(nullptr)
	, _resLoader(nullptr)
	, _logManager(nullptr)
{
}

MyGuiSystem::~MyGuiSystem() noexcept
{
	this->close();
}

bool
MyGuiSystem::open() except
{
	assert(!_isInitialise);

	_logManager = std::make_unique<MyGUI::LogManager>();
	_logManager->createDefaultSource(MYGUI_PLATFORM_LOG_FILENAME);

	_renderer = std::make_unique<MyGuiRenderer>();
	_renderer->open();

	_resLoader = std::make_unique<MyGuiResManager>();
	_resLoader->open();

	_isInitialise = true;

	return true;
}

void
MyGuiSystem::close() noexcept
{
	if (_isInitialise)
	{
		_isInitialise = false;

		_renderer->close();
		_resLoader->close();

		_gui.reset();
		_renderer.reset();
		_resLoader.reset();
		_logManager.reset();
	}
}

void
MyGuiSystem::setCoreProfile(const std::string& core) except
{
	if (_coreProfile != core)
	{
		_gui = std::make_unique<MyGUI::Gui>();
		_gui->initialise(core);
		_gui->setVisiblePointer(false);

		_coreProfile = core;
	}
}

const std::string&
MyGuiSystem::getCoreProfile() const noexcept
{
	return _coreProfile;
}

void
MyGuiSystem::setImageLoader(GuiImageLoaderPtr loader) noexcept
{
	assert(_renderer);
	_renderer->setImageLoader(loader);
}

GuiImageLoaderPtr
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
MyGuiSystem::injectMousePress(int _absx, int _absy, GuiInputButton::Code _id) noexcept
{
	return MyGUI::InputManager::getInstance().injectMousePress(_absx, _absy, GuiButtonToMyGUI(_id));
}

bool
MyGuiSystem::injectMouseRelease(int _absx, int _absy, GuiInputButton::Code _id) noexcept
{
	return MyGUI::InputManager::getInstance().injectMouseRelease(_absx, _absy, GuiButtonToMyGUI(_id));
}

bool
MyGuiSystem::injectKeyPress(GuiInputKey::Code _key, GuiInputChar _char) noexcept
{
	return MyGUI::InputManager::getInstance().injectKeyPress(GuiKeyToMyGUI(_key), _char);
}

bool
MyGuiSystem::injectKeyRelease(GuiInputKey::Code _key) noexcept
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
MyGuiSystem::setViewport(std::uint32_t w, std::uint32_t h) noexcept
{
	assert(_renderer);
	_renderer->setViewport(w, h);
}

void
MyGuiSystem::getViewport(std::uint32_t& w, std::uint32_t& h) noexcept
{
	assert(_renderer);
	_renderer->getViewport(w, h);
}

void 
MyGuiSystem::setFramebufferScale(std::uint32_t w, std::uint32_t h) noexcept
{
	assert(_renderer);
	_renderer->setFramebufferScale(w, h);
}

void 
MyGuiSystem::getFramebufferScale(std::uint32_t& w, std::uint32_t& h) noexcept
{
	assert(_renderer);
	_renderer->getFramebufferScale(w, h);
}

GuiWidgetPtr
MyGuiSystem::createWidget(const rtti::Rtti* rtti) except
{
	assert(rtti);
	assert(rtti->isDerivedFrom(GuiWidget::getRtti()));

	GuiWidgetPtr widget;
	if (rtti == GuiButton::getRtti())
		widget = std::make_shared<MyGuiButton>();
	else if (rtti == GuiWindow::getRtti())
		widget = std::make_shared<MyGuiWindow>();
	else if (rtti == GuiEditBox::getRtti())
		widget = std::make_shared<MyGuiEditBox>();
	else if (rtti == GuiTextBox::getRtti())
		widget = std::make_shared<MyGuiTextBox>();
	else if (rtti == GuiImageBox::getRtti())
		widget = std::make_shared<MyGuiImageBox>();
	else if (rtti == GuiScrollBar::getRtti())
		widget = std::make_shared<MyGuiScrollBar>();
	else if (rtti == GuiScrollView::getRtti())
		widget = std::make_shared<MyGuiScrollView>();
	else if (rtti == GuiComboBox::getRtti())
		widget = std::make_shared<MyGuiComboBox>();
	else
	{
		assert(false);
	}

	return widget;
}

void
MyGuiSystem::render(float delta) noexcept
{
	assert(_renderer);
	_renderer->drawOneFrame(delta);
}

_NAME_END

#endif