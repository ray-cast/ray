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
#if defined(_BUILD_PLATFORM_WINDOWS)
#include <ray/ray.h>

#include <ray/game_application.h>

#include <ray/fcntl.h>

#include <ray/input_event.h>
#include <ray/input_feature.h>

#define GLFW_EXPOSE_NATIVE_WGL
#define GLFW_EXPOSE_NATIVE_WIN32

#include <GLFW\glfw3.h>
#include <GLFW\glfw3native.h>

RAY_C_LINKAGE int RAY_CALL ray_main(int argc, const char* argv[]);

GLFWwindow* _window = nullptr;
ray::GameApplicationPtr _gameApp;
ray::util::string _gameRootPath;
ray::util::string _gameScenePath;

void onWindowResize(GLFWwindow* glfwWindow, int w, int h)
{
	if (_gameApp)
	{
		ray::InputEvent inputEvent;
		inputEvent.event = ray::InputEvent::SizeChange;
		inputEvent.change.w = w;
		inputEvent.change.h = h;
		inputEvent.change.windowID = (std::uint64_t)::glfwGetWin32Window(_window);
		inputEvent.change.timestamp = ::glfwGetTimerFrequency();
		_gameApp->sendInputEvent(inputEvent);
	}
}

void onWindowClose(GLFWwindow* glfwWindow)
{
	if (_gameApp)
	{
		ray::InputEvent inputEvent;
		inputEvent.event = ray::InputEvent::AppQuit;
		_gameApp->sendInputEvent(inputEvent);
	}
}

void onWindowFocus(GLFWwindow* window, int focus)
{
	if (_gameApp)
	{
		if (focus)
		{
			ray::InputEvent inputEvent;
			inputEvent.event = ray::InputEvent::GetFocus;
			_gameApp->sendInputEvent(inputEvent);
		}
		else
		{
			ray::InputEvent inputEvent;
			inputEvent.event = ray::InputEvent::LostFocus;
			_gameApp->sendInputEvent(inputEvent);
		}
	}
}

void onWindowMouseButton(GLFWwindow* window, int button, int state, int)
{
	if (_gameApp)
	{
		double mouseX, mouseY;
		::glfwGetCursorPos(window, &mouseX, &mouseY);

		ray::InputEvent event;
		event.event = state == GLFW_PRESS ? ray::InputEvent::MouseButtonDown : ray::InputEvent::MouseButtonUp;
		event.button.button = ray::InputButton::MOUSE0 + button;
		event.button.clicks = true;
		event.button.x = mouseX;
		event.button.y = mouseY;
		event.button.timestamp = glfwGetTimerFrequency();

		_gameApp->sendInputEvent(event);
	}
}

void onWindowMouseMotion(GLFWwindow* window, double x, double y)
{
	if (_gameApp)
	{
		ray::InputEvent event;
		event.event = ray::InputEvent::MouseMotion;
		event.motion.x = event.motion.xrel = x;
		event.motion.x = event.motion.yrel = y;
		event.motion.timestamp = glfwGetTimerFrequency();

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

	::glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	
	_window = ::glfwCreateWindow(w, h, title, nullptr, nullptr);
	if (_window)
	{
		::glfwSetWindowUserPointer(_window, nullptr);
		::glfwSetWindowFocusCallback(_window, &onWindowFocus);
		::glfwSetWindowCloseCallback(_window, &onWindowClose);
		::glfwSetWindowSizeCallback(_window, &onWindowResize);
		//::glfwSetMouseButtonCallback(_window, &onWindowMouseButton);
		//::glfwSetCursorPosCallback(_window, &onWindowMouseMotion);

		HWND hwnd = ::glfwGetWin32Window(_window);

		_gameApp = std::make_shared<ray::GameApplication>();
		_gameApp->setFileService(true);
		_gameApp->setFileServicePath(_gameRootPath);

		if (!_gameApp->open(hwnd, w, h))
			return false;

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

	if (::glfwWindowShouldClose(_window) || _gameApp->isQuitRequest())
		return true;

	return false;
}

void RAY_CALL rayUpdate() noexcept
{
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

#endif