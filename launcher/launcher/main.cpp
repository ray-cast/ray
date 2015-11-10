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

int main(int argc, char *argv[])
{
	try
	{
		ray::GameApplication app;
		app.openIoServer();

		int width = 1376;
		int height = 768;
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

			if (width < 0 || width > 4096)
				width = 1376;
			if (height < 0 || height > 4096)
				height = 768;
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

			HWND hwnd = glfwGetWin32Window(_window);

			if (!app.openGameServer(hwnd, width, height))
				throw ray::failure("App::open() fail");

			for (;;)
			{
				if (app.openScene(scene))
					break;

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