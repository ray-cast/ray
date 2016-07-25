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
		event.key.keysym.sym = key;
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

		ray::WindHandle hwnd = (ray::WindHandle)::glfwGetWinHandle(_window);

		_gameApp = std::make_shared<ray::GameApplication>();
		_gameApp->setFileService(true);
		_gameApp->setFileServicePath(_gameRootPath);

		if (!_gameApp->open(hwnd, w, h))
			return false;

		onWindowFocus(_window, true);
		onWindowResize(_window, w, h);

		if (!_gameScenePath.empty())
		{
			if (!_gameApp->openScene(_gameScenePath))
				return false;
		}

		return _gameApp->start();
	}

	return false;
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
	if (argc != 0)
	{
		if (ray::fcntl::access(argv[0], 0) == 0)
			_gameRootPath = ray::util::directory(argv[0]);
	}

	return ray_main(argc, argv);
}