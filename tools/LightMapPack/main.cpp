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

#include <chrono>
#include <ctime>
#include <cinttypes>
#include <iomanip>
#include <sstream>
#include <ios>

#include <glfw/glfw3.h>

class AppListener : public ray::LightMassListener
{
public:
	AppListener() noexcept {}
	~AppListener() noexcept {}

	virtual void onBakingStart() noexcept
	{
		_startTime = std::time(nullptr);
		std::cout << "Calculating the ambient occlusion of the model : ";
		std::cout << std::put_time(std::localtime(&_startTime), "start time %Y-%m-%d %H.%M.%S") << "." << std::endl;
	}

	virtual void onBakingEnd() noexcept
	{
		_endTime = std::time(nullptr);
		std::cout << "Processing : " << "100.00%" << std::fixed << std::endl;
		std::cout << "Calculated the ambient occlusion of the model : ";
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
			std::cout << "Input the image size (512, 1024, 2048, 4096): ";
			char paths[MAX_PATH];
			std::cin.getline(paths, MAX_PATH);
			size = atoi(paths);
		}

		return std::floor(size / 2) * 2;
	}

	std::uint32_t GetImageChannel()
	{
		std::uint32_t channel = 0;
		while (!channel)
		{
			std::cout << "Enable Backface Culling (Y/N, 1/0) :";
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
			std::cout << "Input the sample size (16, 32, 64, 128, 256, 512): ";
			char paths[MAX_PATH];
			std::cin.getline(paths, MAX_PATH);
			size = atoi(paths);
			if (size == 16 || size == 32 || size == 64 || size == 128 || size == 256, size == 512)
				break;
		}

		return size;
	}

	bool baking(std::string path)
	{
		while (path.empty())
		{
			std::cout << "Input the path to your pmx model : ";
			char paths[PATHLIMIT];
			std::cin.getline(paths, PATHLIMIT);
			path.append(paths);
		}

		std::uint32_t imageSize = GetImageSize();

		std::unique_ptr<float[]> lightmap = std::make_unique<float[]>(imageSize * imageSize * 4);
		std::memset(lightmap.get(), 0, imageSize * imageSize * sizeof(float) * 4);

		ray::LightMassParams params;
		params.lightMap.width = params.lightMap.height = imageSize;
		params.lightMap.channel = 4;
		params.lightMap.data = lightmap.get();
		params.baking.hemisphereSize = GetSampleCount();

		_lightMass = std::make_shared<ray::LightMass>();
		_lightMass->setLightMassListener(std::make_shared<AppListener>());
		if (!_lightMass->load(path))
			return false;

		if (!_lightMass->baking(params))
			return false;

		std::string outputPath = ray::util::directory(path) + "ao.tga";
		std::cout << "Save as image : " << outputPath << std::endl;
		
		if (!_lightMass->saveAsTGA(outputPath, params.lightMap.data, params.lightMap.width, params.lightMap.height, params.lightMap.channel))
			std::cout << "Failed to save image : " << outputPath << std::endl;

		return true;
	}

private:
	ray::LightMassPtr _lightMass;
};

bool domain_main(int argc, char** argv)
{
	auto app = std::make_shared<Application>();
	return app->baking(argc > 1 ? argv[1] : "");
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