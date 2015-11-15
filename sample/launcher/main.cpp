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
#include <ray/xmlreader.h>
#include <ray/archive.h>

#define GLFW_EXPOSE_NATIVE_WGL
#define GLFW_EXPOSE_NATIVE_WIN32

#define WINDOW_DEFAULT_WIDTH 1376
#define WINDOW_DEFAULT_HEIGHT 768
#define WINDOW_MAX_WIDTH 4096
#define WINDOW_MAX_HEIGHT 4096

#include <GLFW\glfw3.h>
#include <GLFW\glfw3native.h>

static void setWindowCloseCallback(GLFWwindow* window)
{
	ray::GameApplication* engine = (ray::GameApplication*)glfwGetWindowUserPointer(window);
	if (engine)
	{
		ray::AppQuitEvent quit;
		engine->sendMessage(&quit);
	}
}

static void setWindowFocusCallback(GLFWwindow* window, int focus)
{
	ray::GameApplication* engine = (ray::GameApplication*)glfwGetWindowUserPointer(window);
	if (engine)
	{
		if (focus)
		{
			ray::GetFocusEvent focus;
			engine->sendMessage(&focus);
		}
		else
		{
			ray::LostFocusEvent focus;
			engine->sendMessage(&focus);
		}
	}
}

static void setWindowSizeChangeCallback(GLFWwindow* window, int w, int h)
{
	ray::GameApplication* engine = (ray::GameApplication*)glfwGetWindowUserPointer(window);
	if (engine)
	{
		ray::SizeChangeEvent event;
		event.window.w = w;
		event.window.h = h;
		engine->sendMessage(&event);
	}
}

int main(int argc, char *argv[])
{
	try
	{
		ray::GameApplication app;
		app.initialize(argc, argv);

		int width = WINDOW_DEFAULT_WIDTH;
		int height = WINDOW_DEFAULT_HEIGHT;
		std::string title = "Launcher";
		std::string scene = "";

		for (;;)
		{
			std::cout << "Enter your main file path : ";
			std::cin >> scene;
			std::cout << std::endl;

			ray::XMLReader reader;
			if (!reader.open(scene))
				continue;

			if (reader.getCurrentNodeName() != "main")
				continue;

			reader.setToFirstChild();

			reader >> rtti_name(width);
			reader >> rtti_name(height);
			reader >> rtti_name(title);
			reader >> rtti_name(scene);

			if (width < 0 || width > WINDOW_MAX_WIDTH)
				width = WINDOW_DEFAULT_WIDTH;
			if (height < 0 || height > WINDOW_MAX_HEIGHT)
				height = WINDOW_DEFAULT_HEIGHT;
			if (title.empty() || scene.empty())
				continue;
			break;
		}

		glfwInit();
		auto _window = glfwCreateWindow(width, height, title.c_str(), nullptr, nullptr);
		if (_window)
		{
			glfwSetWindowUserPointer(_window, &app);
			glfwSetWindowFocusCallback(_window, &setWindowFocusCallback);
			glfwSetWindowCloseCallback(_window, &setWindowCloseCallback);
			glfwSetWindowSizeCallback(_window, &setWindowSizeChangeCallback);

			HWND hwnd = glfwGetWin32Window(_window);

			if (!app.open(hwnd, width, height))
				throw ray::failure("App::open() fail");

			while (!app.openScene(scene))
			{
				std::cout << "Enter your scene file path : ";
				std::cin >> scene;
				std::cout << std::endl;
			}

			while (!glfwWindowShouldClose(_window))
			{
				app.update();
			}
		}		
	}
	catch (const ray::exception& e)
	{
		std::cout << e.what();
		std::system("pause");
	}

	glfwTerminate();
	return 0;
}