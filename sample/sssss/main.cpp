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
#include <ray/game_application.h>
#include <ray/game_event.h>

#define GLFW_EXPOSE_NATIVE_WGL
#define GLFW_EXPOSE_NATIVE_WIN32

#include <GLFW\glfw3.h>
#include <GLFW\glfw3native.h>

class GameEngine : public ray::GameApplication
{
public:
	GameEngine() noexcept
		: _window(nullptr)
	{
	}

	void setup(std::size_t w, std::size_t h)
	{
		glfwInit();

		_window = glfwCreateWindow(w, h, "Screen Space Sub Surface Scattering", nullptr, nullptr);
		if (_window)
		{
			glfwSetWindowUserPointer(_window, this);
			glfwSetWindowFocusCallback(_window, &onWindowFocus);
			glfwSetWindowCloseCallback(_window, &onWindowClose);

			HWND hwnd = glfwGetWin32Window(_window);

			if (!this->open(hwnd, w, h))
				throw ray::failure("App::open() fail");

			if (!this->openScene("dlc:sssss\\scenes\\scene.map"))
				throw ray::failure("App::openScene('dlc:sssss\\scenes\\scene.map') fail");
		}
	}

	void run()
	{
		while (!glfwWindowShouldClose(_window))
		{
			this->update();

			glfwPostEmptyEvent();
		}
	}

	static void onWindowClose(GLFWwindow* window)
	{
		GameEngine* engine = (GameEngine*)glfwGetWindowUserPointer(window);
		if (engine)
		{
			auto event = ray::make_message<ray::InputEvent>();
			event->event = ray::InputEvent::AppQuit;
			engine->sendMessage(event);
		}
	}

	static void onWindowFocus(GLFWwindow* window, int focus)
	{
		GameEngine* engine = (GameEngine*)glfwGetWindowUserPointer(window);
		if (engine)
		{
			if (focus)
			{
				auto event = ray::make_message<ray::InputEvent>();
				event->event = ray::InputEvent::GetFocus;
				engine->sendMessage(event);
			}
			else
			{
				auto event = ray::make_message<ray::InputEvent>();
				event->event = ray::InputEvent::LostFocus;
				engine->sendMessage(event);
			}
		}
	}

private:

	GLFWwindow* _window;
};

int main(int argc, char *argv[])
{
	try
	{
		GameEngine engine;
		engine.initialize(argc, argv);
		engine.setup(1376, 768);
		engine.run();
	}
	catch (const ray::exception& e)
	{
		std::cout << e.what();
		std::system("pause");
	}

	glfwTerminate();
	return 0;
}