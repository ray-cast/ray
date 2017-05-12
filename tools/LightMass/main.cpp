// +----------------------------------------------------------------------
// | Project : ray.
// | All rights reserved.
// +----------------------------------------------------------------------
// | Copyright (c) 2013-2017.
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
#include "LightMass.h"
#include "LightMapPack.h"

#include <chrono>
#include <ctime>
#include <cinttypes>
#include <iomanip>
#include <sstream>
#include <ios>

#include <glfw/glfw3.h>

class AppMapListener : public ray::LightMapListener
{
public:
	AppMapListener() noexcept {}
	~AppMapListener() noexcept {}

	void onUvmapperStart()
	{
		_startTime = std::time(nullptr);
		std::cout << "Calculating the Light map pack of the model : ";
		std::cout << std::put_time(std::localtime(&_startTime), "start time %Y-%m-%d %H.%M.%S") << "." << std::endl;
	}

	void onUvmapperProgressing(float progress)
	{
		std::cout.precision(2);
		std::cout << "Processing : " << progress * 100 << "%" << std::fixed;
		std::cout << "\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b";
	}

	void onUvmapperEnd()
	{
		_endTime = std::time(nullptr);
		std::cout << "Processing : " << "100.00%" << std::fixed << std::endl;
		std::cout << "Calculated the Light map pack of the model : ";
		std::cout << std::put_time(std::localtime(&_endTime), "end time %Y-%m-%d %H.%M.%S") << "." << std::endl;
	}

	virtual void onMessage(const std::string& message) noexcept
	{
		std::cout << message << std::endl;
	}

private:
	std::time_t _startTime;
	std::time_t _endTime;
};

class AppMassListener : public ray::LightMassListener
{
public:
	AppMassListener() noexcept {}
	~AppMassListener() noexcept {}

	virtual void onBakingStart() noexcept
	{
		_startTime = std::time(nullptr);
		std::cout << "Calculating the radiosity of the model : ";
		std::cout << std::put_time(std::localtime(&_startTime), "start time %Y-%m-%d %H.%M.%S") << "." << std::endl;
	}

	virtual void onBakingEnd() noexcept
	{
		_endTime = std::time(nullptr);
		std::cout << "Processing : " << "100.00%" << std::fixed << std::endl;
		std::cout << "Calculated the radiosity of the model : ";
		std::cout << std::put_time(std::localtime(&_endTime), "end time %Y-%m-%d %H.%M.%S") << "." << std::endl;
	}

	virtual void onBakingProgressing(float progress) noexcept
	{
		std::cout.precision(2);
		std::cout << "Processing : " << progress * 100 << "%" << std::fixed;
		std::cout << "\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b";
	}

	virtual void onMessage(const std::string& message) noexcept
	{
		std::cout << message << std::endl;
	}

private:
	std::time_t _startTime;
	std::time_t _endTime;
};

class Application
{
public:
	Application() noexcept
		: _lightMapListener(std::make_shared<AppMapListener>())
		, _lightMassListener(std::make_shared<AppMassListener>())
	{
	}

	Application::~Application() noexcept
	{
	}

	std::uint32_t GetImageSize()
	{
		std::uint32_t size = 0;
		while (!size)
		{
			std::cout << "Enter the image size (512, 1024, 2048, 4096, 8192): ";
			char paths[MAX_PATH];
			std::cin.getline(paths, MAX_PATH);
			size = atoi(paths);
		}

		return std::floor(size / 2) * 2;
	}

	std::uint32_t GetImageMargin()
	{
		std::uint32_t size = 0;
		while (!size)
		{
			std::cout << "Enter the image margin (1, 2, 3, 4, 5 and above): ";
			char paths[MAX_PATH];
			std::cin.getline(paths, MAX_PATH);
			size = atoi(paths);
			if (size > 1)
				break;
		}

		return size;
	}

	std::uint32_t GetGIEnable()
	{
		std::uint32_t channel = 0;
		while (!channel)
		{
			std::cout << "Enable Global Illumination (Y/N, 1/0) :";
			char c;
			std::cin.get(c);
			if (c == 'y' || c == 'Y' || c == '1')
				channel = 4;
			if (c == 'n' || c == 'N' || c == '0')
				channel = 1;
		}

		return channel;
	}

	std::uint32_t GetSampleCount()
	{
		std::uint32_t size = 0;
		while (!size)
		{
			std::cout << "Enter the sample size (16, 32, 64, 128, 256, 512): ";
			char paths[MAX_PATH];
			std::cin.getline(paths, MAX_PATH);
			size = atoi(paths);
			if (size == 16 || size == 32 || size == 64 || size == 128 || size == 256, size == 512)
				break;
		}

		return size;
	}

	std::uint32_t GetSampleScale()
	{
		std::uint32_t size = 0;
		while (!size)
		{
			std::cout << "Enter the sample scale (1, 2, 3, 4, 5): ";
			char paths[MAX_PATH];
			std::cin.getline(paths, MAX_PATH);
			size = atoi(paths);
			if (size == 1 || size == 2 || size == 3 || size == 4 || size == 5)
				break;
		}

		return size;
	}

	bool load(const std::string& path, ray::PMX& model)
	{
		ray::PMXHandler modelLoader;

		if (_lightMassListener)
			_lightMassListener->onMessage("loading model : " + path);

		std::string error;
		if (!modelLoader.doLoad(path, model, error))
		{
			if (_lightMassListener)
				_lightMassListener->onMessage(error);

			return false;
		}

		if (_lightMassListener)
			_lightMassListener->onMessage("loaded model : " + path);

		return true;
	}

	bool save(const std::string& path, ray::PMX& model)
	{
		ray::PMXHandler modelLoader;

		if (_lightMassListener)
			_lightMassListener->onMessage("Save as model : " + path);

		std::string error;
		if (!modelLoader.doSave(path, model, error))
		{
			if (_lightMassListener)
				_lightMassListener->onMessage(error);

			return false;
		}

		return true;
	}

	bool run(std::string path)
	{
		while (path.empty())
		{
			std::cout << "Input a path to your pmx model : ";
			char paths[PATHLIMIT];
			std::cin.getline(paths, PATHLIMIT);
			path.append(paths);
		}

		ray::LightMassParams params;
		params.lightMap.width = params.lightMap.height = GetImageSize();
		params.lightMap.margin = GetImageMargin();
		params.baking.hemisphereSize = GetSampleCount();
		params.enableGI = GetGIEnable() == 1 ? false : true;

		ray::PMX model;
		if (!this->load(path, model))
			return false;

		if (model.header.addUVCount == 0)
		{
			ray::LightMapPack lightPack(_lightMapListener);
			if (!lightPack.atlasUV1(model, params.lightMap.width, params.lightMap.height, 0, 0.001, params.lightMap.margin))
				return false;

			if (!this->save(ray::util::directory(path) + "stage.pmx", model))
				return false;
		}

		auto lightMass = std::make_shared<ray::LightMass>(_lightMassListener);
		if (!lightMass->open())
			return false;

		ray::LightMapData lightMap;
		if (!lightMass->baking(params, model, lightMap))
			return false;

		std::string outputPath = ray::util::directory(path) + "ao.tga";
		std::cout << "Save as image : " << outputPath << std::endl;
		
		if (!lightMass->saveLightMass(outputPath, lightMap.data.get(), lightMap.width, lightMap.height, lightMap.channel, params.lightMap.margin))
		{
			std::cout << "Failed to save image : " << outputPath << std::endl;
			return false;
		}

		return true;
	}

private:
	ray::LightMapListenerPtr _lightMapListener;
	ray::LightMassListenerPtr _lightMassListener;
};

bool domain_main(int argc, char** argv)
{
	auto app = std::make_shared<Application>();
	return app->run(argc > 1 ? argv[1] : "");
}

int main(int argc, char** argv)
{
	if (::glfwInit() == GL_FALSE)
		return 1;

#if defined(GLFW_EXPOSE_NATIVE_X11)
	::glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	::glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	::glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#else
	::glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_API);
	::glfwWindowHint(GLFW_VISIBLE, false);
#endif
	::glfwSwapInterval(0);

	auto window = ::glfwCreateWindow(800, 600, "LightMass", nullptr, nullptr);
	if (window)
	{
		::glfwMakeContextCurrent(window);
		
		int code = domain_main(argc, argv);

		::glfwDestroyWindow(window);
		::glfwTerminate();

		std::system("pause");

		return code;
	}

	return 1;
}