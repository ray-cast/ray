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
#include <ray/ray.h>
#include <ray/ray_main.h>

#include <ray/game_application.h>

#include <ray/fcntl.h>

#include <ray/input_event.h>
#include <ray/input_feature.h>

#include <GLFW/glfw3.h>
#include <GLFW/glfw3native.h>

#if defined(GLFW_EXPOSE_NATIVE_WIN32)
#define glfwGetWinHandle(window) glfwGetWin32Window(_window);
#elif defined(GLFW_EXPOSE_NATIVE_X11)
#define glfwGetWinHandle(window) glfwGetX11Window(_window);
#elif defined(GLFW_EXPOSE_NATIVE_EGL)
#define glfwGetWinHandle(window) glfwGetEGLSurface(_window);
#elif defined(GLFW_EXPOSE_NATIVE_NSGL)
#define glfwGetWinHandle(window) glfwGetCocoaWindow(_window);
#endif

GLFWwindow* _window = nullptr;
ray::GameApplicationPtr _gameApp;
ray::InputMousePtr _inputMessage;

ray::util::string _gameRootPath;
ray::util::string _gameScenePath;

ray::InputKey::Code KeyCodetoInputKey(int key) noexcept
{
	switch (key)
	{
		case GLFW_KEY_UNKNOWN: return ray::InputKey::Code::None;
		case GLFW_KEY_SPACE: return ray::InputKey::Code::Space;
		case GLFW_KEY_APOSTROPHE: return ray::InputKey::Code::Apostrophe;
		case GLFW_KEY_COMMA: return ray::InputKey::Code::Comma;
		case GLFW_KEY_MINUS: return ray::InputKey::Code::Minus;
		case GLFW_KEY_PERIOD: return ray::InputKey::Code::Period;
		case GLFW_KEY_SLASH: return ray::InputKey::Code::Slash;
		case GLFW_KEY_0: return ray::InputKey::Code::Key0;
		case GLFW_KEY_1: return ray::InputKey::Code::Key1;
		case GLFW_KEY_2: return ray::InputKey::Code::Key2;
		case GLFW_KEY_3: return ray::InputKey::Code::Key3;
		case GLFW_KEY_4: return ray::InputKey::Code::Key4;
		case GLFW_KEY_5: return ray::InputKey::Code::Key5;
		case GLFW_KEY_6: return ray::InputKey::Code::Key6;
		case GLFW_KEY_7: return ray::InputKey::Code::Key7;
		case GLFW_KEY_8: return ray::InputKey::Code::Key8;
		case GLFW_KEY_9: return ray::InputKey::Code::Key9;
		case GLFW_KEY_SEMICOLON: return ray::InputKey::Code::Semicolon;
		case GLFW_KEY_EQUAL: return ray::InputKey::Code::Equal;
		case GLFW_KEY_A: return ray::InputKey::Code::A;
		case GLFW_KEY_B: return ray::InputKey::Code::B;
		case GLFW_KEY_C: return ray::InputKey::Code::C;
		case GLFW_KEY_D: return ray::InputKey::Code::D;
		case GLFW_KEY_E: return ray::InputKey::Code::E;
		case GLFW_KEY_F: return ray::InputKey::Code::F;
		case GLFW_KEY_G: return ray::InputKey::Code::G;
		case GLFW_KEY_H: return ray::InputKey::Code::H;
		case GLFW_KEY_I: return ray::InputKey::Code::I;
		case GLFW_KEY_J: return ray::InputKey::Code::J;
		case GLFW_KEY_K: return ray::InputKey::Code::K;
		case GLFW_KEY_L: return ray::InputKey::Code::L;
		case GLFW_KEY_M: return ray::InputKey::Code::M;
		case GLFW_KEY_N: return ray::InputKey::Code::N;
		case GLFW_KEY_O: return ray::InputKey::Code::O;
		case GLFW_KEY_P: return ray::InputKey::Code::P;
		case GLFW_KEY_Q: return ray::InputKey::Code::Q;
		case GLFW_KEY_R: return ray::InputKey::Code::R;
		case GLFW_KEY_S: return ray::InputKey::Code::S;
		case GLFW_KEY_T: return ray::InputKey::Code::T;
		case GLFW_KEY_U: return ray::InputKey::Code::U;
		case GLFW_KEY_V: return ray::InputKey::Code::V;
		case GLFW_KEY_W: return ray::InputKey::Code::W;
		case GLFW_KEY_X: return ray::InputKey::Code::X;
		case GLFW_KEY_Y: return ray::InputKey::Code::Y;
		case GLFW_KEY_Z: return ray::InputKey::Code::Z;
		case GLFW_KEY_LEFT_BRACKET: return ray::InputKey::Code::LeftBracket;
		case GLFW_KEY_BACKSLASH: return ray::InputKey::Code::Backslash;
		case GLFW_KEY_RIGHT_BRACKET: return ray::InputKey::Code::RightBracket;
		case GLFW_KEY_GRAVE_ACCENT: return ray::InputKey::Code::GraveAccent;
		case GLFW_KEY_WORLD_1: return ray::InputKey::Code::World1;
		case GLFW_KEY_WORLD_2: return ray::InputKey::Code::World2;
		case GLFW_KEY_ESCAPE: return ray::InputKey::Code::Escape;
		case GLFW_KEY_ENTER: return ray::InputKey::Code::Enter;
		case GLFW_KEY_TAB: return ray::InputKey::Code::Tab;
		case GLFW_KEY_BACKSPACE: return ray::InputKey::Code::Backspace;
		case GLFW_KEY_INSERT: return ray::InputKey::Code::Insert;
		case GLFW_KEY_DELETE: return ray::InputKey::Code::Delete;
		case GLFW_KEY_RIGHT: return ray::InputKey::Code::Right;
		case GLFW_KEY_LEFT: return ray::InputKey::Code::Left;
		case GLFW_KEY_DOWN: return ray::InputKey::Code::Down;
		case GLFW_KEY_UP: return ray::InputKey::Code::Up;
		case GLFW_KEY_PAGE_UP: return ray::InputKey::Code::PageUp;
		case GLFW_KEY_PAGE_DOWN: return ray::InputKey::Code::PageDown;
		case GLFW_KEY_HOME: return ray::InputKey::Code::Home;
		case GLFW_KEY_END: return ray::InputKey::Code::End;
		case GLFW_KEY_CAPS_LOCK: return ray::InputKey::Code::CapsLock;
		case GLFW_KEY_SCROLL_LOCK: return ray::InputKey::Code::ScrollLock;
		case GLFW_KEY_NUM_LOCK: return ray::InputKey::Code::NumLock;
		case GLFW_KEY_PRINT_SCREEN: return ray::InputKey::Code::PrintScreen;
		case GLFW_KEY_PAUSE: return ray::InputKey::Code::Pause;
		case GLFW_KEY_F1: return ray::InputKey::Code::F1;
		case GLFW_KEY_F2: return ray::InputKey::Code::F2;
		case GLFW_KEY_F3: return ray::InputKey::Code::F3;
		case GLFW_KEY_F4: return ray::InputKey::Code::F4;
		case GLFW_KEY_F5: return ray::InputKey::Code::F5;
		case GLFW_KEY_F6: return ray::InputKey::Code::F6;
		case GLFW_KEY_F7: return ray::InputKey::Code::F7;
		case GLFW_KEY_F8: return ray::InputKey::Code::F8;
		case GLFW_KEY_F9: return ray::InputKey::Code::F9;
		case GLFW_KEY_F10: return ray::InputKey::Code::F10;
		case GLFW_KEY_F11: return ray::InputKey::Code::F11;
		case GLFW_KEY_F12: return ray::InputKey::Code::F12;
		case GLFW_KEY_F13: return ray::InputKey::Code::F13;
		case GLFW_KEY_F14: return ray::InputKey::Code::F14;
		case GLFW_KEY_F15: return ray::InputKey::Code::F15;
		case GLFW_KEY_F16: return ray::InputKey::Code::F16;
		case GLFW_KEY_F17: return ray::InputKey::Code::F17;
		case GLFW_KEY_F18: return ray::InputKey::Code::F18;
		case GLFW_KEY_F19: return ray::InputKey::Code::F19;
		case GLFW_KEY_F20: return ray::InputKey::Code::F20;
		case GLFW_KEY_F21: return ray::InputKey::Code::F21;
		case GLFW_KEY_F22: return ray::InputKey::Code::F22;
		case GLFW_KEY_F23: return ray::InputKey::Code::F23;
		case GLFW_KEY_F24: return ray::InputKey::Code::F24;
		case GLFW_KEY_F25: return ray::InputKey::Code::F25;
		case GLFW_KEY_KP_0: return ray::InputKey::Code::KP_0;
		case GLFW_KEY_KP_1: return ray::InputKey::Code::KP_1;
		case GLFW_KEY_KP_2: return ray::InputKey::Code::KP_2;
		case GLFW_KEY_KP_3: return ray::InputKey::Code::KP_3;
		case GLFW_KEY_KP_4: return ray::InputKey::Code::KP_4;
		case GLFW_KEY_KP_5: return ray::InputKey::Code::KP_5;
		case GLFW_KEY_KP_6: return ray::InputKey::Code::KP_6;
		case GLFW_KEY_KP_7: return ray::InputKey::Code::KP_7;
		case GLFW_KEY_KP_8: return ray::InputKey::Code::KP_8;
		case GLFW_KEY_KP_9: return ray::InputKey::Code::KP_9;
		case GLFW_KEY_KP_DECIMAL: return ray::InputKey::Code::KP_Decimal;
		case GLFW_KEY_KP_DIVIDE: return ray::InputKey::Code::KP_Divide;
		case GLFW_KEY_KP_MULTIPLY: return ray::InputKey::Code::KP_Multiply;
		case GLFW_KEY_KP_SUBTRACT: return ray::InputKey::Code::KP_Subtract;
		case GLFW_KEY_KP_ADD: return ray::InputKey::Code::KP_Add;
		case GLFW_KEY_KP_ENTER: return ray::InputKey::Code::KP_Enter;
		case GLFW_KEY_KP_EQUAL: return ray::InputKey::Code::KP_Equal;
		case GLFW_KEY_LEFT_SHIFT: return ray::InputKey::Code::LeftShift;
		case GLFW_KEY_LEFT_CONTROL: return ray::InputKey::Code::LeftControl;
		case GLFW_KEY_LEFT_ALT: return ray::InputKey::Code::LeftAlt;
		case GLFW_KEY_LEFT_SUPER: return ray::InputKey::Code::LeftSuper;
		case GLFW_KEY_RIGHT_SHIFT: return ray::InputKey::Code::LeftSuper;
		case GLFW_KEY_RIGHT_CONTROL: return ray::InputKey::Code::RightControl;
		case GLFW_KEY_RIGHT_ALT: return ray::InputKey::Code::RightAlt;
		case GLFW_KEY_RIGHT_SUPER: return ray::InputKey::Code::RightSuper;
		case GLFW_KEY_MENU: return ray::InputKey::Code::Menu;
		default:
			return ray::InputKey::Code::None;
	}
}

void onWindowResize(GLFWwindow* window, int w, int h)
{
	if (_gameApp)
	{
		ray::InputEvent event;
		event.event = ray::InputEvent::SizeChange;
		event.change.w = w;
		event.change.h = h;
		event.change.windowID = (std::uint64_t)::glfwGetWinHandle(_window);
		event.change.timestamp = ::glfwGetTimerFrequency();
		_gameApp->sendInputEvent(event);
	}
}

void onWindowFramebufferResize(GLFWwindow* window, int w, int h)
{
	if (_gameApp)
	{
		ray::InputEvent event;
		event.event = ray::InputEvent::SizeChangeDPI;
		event.change.w = w;
		event.change.h = h;
		event.change.windowID = (std::uint64_t)::glfwGetWinHandle(_window);
		event.change.timestamp = ::glfwGetTimerFrequency();
		_gameApp->sendInputEvent(event);
	}
}

void onWindowClose(GLFWwindow* window)
{
	if (_gameApp)
	{
		ray::InputEvent event;
		event.event = ray::InputEvent::AppQuit;
		event.window.windowID = (std::uint64_t)::glfwGetWinHandle(_window);
		event.window.timestamp = ::glfwGetTimerFrequency();
		_gameApp->sendInputEvent(event);
	}
}

void onWindowFocus(GLFWwindow* window, int focus)
{
	if (_gameApp)
	{
		ray::InputEvent event;
		event.event = focus ? ray::InputEvent::GetFocus : ray::InputEvent::LostFocus;
		event.window.windowID = (std::uint64_t)::glfwGetWinHandle(_window);
		event.window.timestamp = ::glfwGetTimerFrequency();
		_gameApp->sendInputEvent(event);
	}
}

void onWindowKey(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if (_gameApp)
	{
		ray::InputEvent event;
		event.event = (action == GLFW_PRESS || action == GLFW_REPEAT) ? ray::InputEvent::KeyDown : ray::InputEvent::KeyUp;
		event.key.windowID = (std::uint64_t)::glfwGetWinHandle(_window);
		event.key.timestamp = ::glfwGetTimerFrequency();
		event.key.padding2 = 0;
		event.key.padding3 = 0;
		event.key.repeat = (action == GLFW_REPEAT) ? true : false;
		event.key.state = ::glfwGetKey(_window, key) == GLFW_PRESS ? true : false;
		event.key.keysym.raw = scancode;
		event.key.keysym.sym = KeyCodetoInputKey(key);
		event.key.keysym.mod = mods;
		event.key.keysym.unicode = 0;

		_gameApp->sendInputEvent(event);
	}
}

void onWindowKeyChar(GLFWwindow* window, unsigned int unicode, int mods)
{
	if (_gameApp)
	{
		ray::InputEvent event;
		event.event = ray::InputEvent::Character;
		event.key.windowID = (std::uint64_t)::glfwGetWinHandle(_window);
		event.key.timestamp = ::glfwGetTimerFrequency();
		event.key.padding2 = 0;
		event.key.padding3 = 0;
		event.key.repeat = 0;
		event.key.state = true;
		event.key.keysym.raw = 0;
		event.key.keysym.sym = 0;
		event.key.keysym.mod = mods;
		event.key.keysym.unicode = unicode;

		_gameApp->sendInputEvent(event);
	}
}

void onWindowMouseButton(GLFWwindow* window, int button, int action, int mods)
{
	if (_gameApp)
	{
		double mouseX, mouseY;
		::glfwGetCursorPos(window, &mouseX, &mouseY);

		ray::InputEvent event;
		event.event = action == GLFW_PRESS ? ray::InputEvent::MouseButtonDown : ray::InputEvent::MouseButtonUp;
		event.button.button = ray::InputButton::MOUSE0 + button;
		event.button.clicks = action == GLFW_PRESS ? true : false;
		event.button.x = mouseX;
		event.button.y = mouseY;
		event.button.timestamp = glfwGetTimerFrequency();
		event.button.windowID = (std::uint64_t)::glfwGetWinHandle(_window);
		event.button.padding1 = 0;
		event.button.which = 0;

		_gameApp->sendInputEvent(event);

		if (action == GLFW_RELEASE)
		{
			static auto clicks = false;
			static auto before = std::chrono::system_clock::now();

			if (!clicks)
			{
				before = std::chrono::system_clock::now();
				clicks = true;
			}
			else
			{
				auto now = std::chrono::system_clock::now();
				double diff_ms = std::chrono::duration <double, std::milli>(now - before).count();
				if (diff_ms > 10 && diff_ms < 200)
				{
					ray::InputEvent doubleClick;
					doubleClick.event = ray::InputEvent::MouseButtonDoubleClick;
					doubleClick.button.button = ray::InputButton::MOUSE0 + button;
					doubleClick.button.clicks = true;
					doubleClick.button.x = mouseX;
					doubleClick.button.y = mouseY;
					doubleClick.button.timestamp = glfwGetTimerFrequency();
					doubleClick.button.windowID = (std::uint64_t)::glfwGetWinHandle(_window);
					doubleClick.button.padding1 = 0;
					doubleClick.button.which = 0;

					_gameApp->sendInputEvent(doubleClick);
				}

				clicks = false;
			}
		}
	}
}

void onWindowMouseMotion(GLFWwindow* window, double x, double y)
{
	if (_gameApp)
	{
		ray::InputEvent event;
		event.event = ray::InputEvent::MouseMotion;
		event.motion.x = event.motion.xrel = x;
		event.motion.y = event.motion.yrel = y;
		event.motion.timestamp = glfwGetTimerFrequency();
		event.motion.state = false;
		event.motion.windowID = (std::uint64_t)::glfwGetWinHandle(_window);

#if defined(GLFW_EXPOSE_NATIVE_WIN32)
		POINT pt;
		GetCursorPos(&pt);

		event.motion.xrel = pt.x;
		event.motion.yrel = pt.y;
#endif

		_gameApp->sendInputEvent(event);
	}
}

void RAY_CALL rayInit(const char* gamedir, const char* scenename) noexcept
{
	if (gamedir)
	{
		if (ray::fcntl::access(gamedir, 0) == 0)
			_gameRootPath = ray::util::directory(gamedir);
	}

	if (scenename)
		_gameScenePath = scenename;
}

bool RAY_CALL rayOpenWindow(const char* title, int w, int h) noexcept
{
	assert(!_gameApp && !_window);

	try
	{
		if (::glfwInit() == GL_FALSE)
			return false;

#if defined(GLFW_EXPOSE_NATIVE_X11)
		::glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
		::glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
		::glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#else
		::glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
#endif

		_window = ::glfwCreateWindow(w, h, title, nullptr, nullptr);
		if (_window)
		{
			::glfwSetWindowUserPointer(_window, nullptr);
			::glfwSetWindowFocusCallback(_window, &onWindowFocus);
			::glfwSetWindowCloseCallback(_window, &onWindowClose);
			::glfwSetWindowSizeCallback(_window, &onWindowResize);
			::glfwSetFramebufferSizeCallback(_window, &onWindowFramebufferResize);
			::glfwSetMouseButtonCallback(_window, &onWindowMouseButton);
			::glfwSetCursorPosCallback(_window, &onWindowMouseMotion);
			::glfwSetKeyCallback(_window, &onWindowKey);
			::glfwSetCharModsCallback(_window, &onWindowKeyChar);

			int dpi_w, dpi_h;
			::glfwGetFramebufferSize(_window, &dpi_w, &dpi_h);

			ray::WindHandle hwnd = (ray::WindHandle)::glfwGetWinHandle(_window);

			_gameApp = std::make_shared<ray::GameApplication>();
			_gameApp->setFileService(true);
			_gameApp->setFileServicePath(_gameRootPath);

			if (!_gameApp->open(hwnd, w, h, dpi_w, dpi_h))
			{
				rayCloseWindow();
				return false;
			}

			onWindowFocus(_window, true);
			onWindowResize(_window, w, h);
			onWindowFramebufferResize(_window, dpi_w, dpi_h);

			if (!_gameScenePath.empty())
			{
				if (!_gameApp->openScene(_gameScenePath))
				{
					rayCloseWindow();
					return false;
				}
			}

			return _gameApp->start();
		}

		return true;
	}
	catch (...)
	{
		rayCloseWindow();
		return false;
	}
}

void RAY_CALL rayCloseWindow() noexcept
{
	if (_gameApp)
	{
		_gameApp.reset();
		_gameApp = nullptr;
	}

	if (_window)
	{
		::glfwDestroyWindow(_window);
		_window = nullptr;
	}

	::glfwTerminate();
}

bool RAY_CALL rayIsQuitRequest() noexcept
{
	if (!_gameApp)
		return true;

	if (glfwWindowShouldClose(_window) || _gameApp->isQuitRequest())
		return true;

	return false;
}

void RAY_CALL rayUpdate() noexcept
{
	::glfwPollEvents();

	if (_gameApp)
		_gameApp->update();
}

void RAY_CALL rayTerminate() noexcept
{
	rayCloseWindow();
}

int main(int argc, const char* argv[]) noexcept
{
	try
	{
		if (argc != 0)
		{
			if (ray::fcntl::access(argv[0], 0) == 0)
				_gameRootPath = ray::util::directory(argv[0]);
		}

		return ray_main(argc, argv);
	}
	catch (const std::exception& e)
	{
		std::cerr << e.what();
		return 1;
	}
}