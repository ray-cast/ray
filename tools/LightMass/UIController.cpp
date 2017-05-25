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
#include "UIController.h"
#include "UIView.h"

#include <chrono>
#include <ctime>
#include <cinttypes>
#include <iomanip>
#include <sstream>
#include <ios>

#include <glfw/glfw3.h>

#include <ray/fcntl.h>
#include <ray/except.h>
#include <ray/ioserver.h>
#include <ray/mstream.h>
#include <ray/jsonreader.h>

#include <ray/image.h>
#include <ray/imagutil.h>
#include <ray/imagcubemap.h>

#include <ray/SH.h>

struct AppParams
{
	std::uint32_t chart;
	float stretch;
	float margin;
	ray::LightMassParams lightMass;
};

class AppMapListener : public ray::LightMapListener
{
public:
	AppMapListener() noexcept
		: _lastProgress(0)
	{
	}

	~AppMapListener() noexcept {}

	void onUvmapperStart()
	{
		_startTime = std::time(nullptr);
		std::cout << "Calculating the Light map pack of the model : ";
		std::cout << std::put_time(std::localtime(&_startTime), "start time %Y-%m-%d %H.%M.%S") << "." << std::endl;
	}

	void onUvmapperProgressing(float progress)
	{
		if (_lastProgress != progress)
		{
			std::cout.precision(2);
			std::cout << "Processing : " << progress * 100 << "%" << std::fixed;
			std::cout << "\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b";
			_lastProgress = progress;
		}
	}

	void onUvmapperEnd()
	{
		_endTime = std::time(nullptr);
		std::cout << "Processing : " << "100.00%" << std::fixed << std::endl;
		std::cout << "Calculated the Light map pack of the model : ";
		std::cout << std::put_time(std::localtime(&_endTime), "end time %Y-%m-%d %H.%M.%S") << "." << std::endl;
	}

	virtual void onMessage(const ray::util::string& message) noexcept
	{
		std::cout << message << std::endl;
	}

private:
	float _lastProgress;
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

	virtual void onMessage(const ray::util::string& message) noexcept
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

	bool load(const ray::util::string& path, ray::PMX& model)
	{
		ray::PMXHandler modelLoader;

		if (_lightMassListener)
			_lightMassListener->onMessage("loading model : " + path);

		ray::util::string error;
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

	bool saveLightMass(const ray::util::string& path, float* data, std::uint32_t w, std::uint32_t h, std::uint32_t channel, std::uint32_t margin)
	{
		assert(channel == 1 || channel == 3 || channel == 4);

		bool isGreyscale = channel == 1;
		bool hasAlpha = channel == 1 ? false : true;

		if (margin > 0)
		{
			std::unique_ptr<float[]> lightmapTemp = std::make_unique<float[]>(w * h * channel);
			std::memset(lightmapTemp.get(), 0, w * h * channel * sizeof(float));

			for (std::uint32_t j = 0; j < std::max<std::uint32_t>(margin >> 1, 1); j++)
			{
				ray::image::smoothFilter(data, lightmapTemp.get(), w, h, channel);
				ray::image::dilateFilter(lightmapTemp.get(), data, w, h, channel);
			}
		}

		ray::image::format_t format = isGreyscale ? ray::image::format_t::R8SRGB : ray::image::format_t::R8G8B8SRGB;
		if (hasAlpha)
			format = ray::image::format_t::R8G8B8A8SRGB;

		ray::image::Image image;
		if (!image.create(w, h, format))
		{
			std::cout << "Failed to create image : " << path << std::endl;
			return false;
		}

		auto temp = (std::uint8_t*)image.data();

		if (channel == 1)
			ray::image::r32f_to_r8uint(data, temp, w, h, channel);
		else
			ray::image::rgb32f_to_rgbt8(data, temp, w, h, channel);

		ray::image::flipHorizontal(temp, w, h, channel);

		if (!isGreyscale)
		{
			for (std::size_t i = 0; i < w * h * channel; i += channel)
				std::swap(temp[i], temp[i + 2]);
		}

		image.save(path);

		return true;
	}

	bool run(ray::util::string path)
	{
		while (path.empty())
		{
			std::cout << "Input a path to your pmx model : ";
			char paths[PATHLIMIT];
			std::cin.getline(paths, PATHLIMIT);
			path.append(paths);
		}

		AppParams params;

		ray::PMX model;
		if (!this->load(path, model))
			return false;

		auto lightMass = std::make_shared<ray::LightMass>(_lightMassListener);
		if (!lightMass->open())
			return false;

		ray::LightMapData lightMap;
		if (!lightMass->baking(params.lightMass, model, lightMap))
			return false;

		ray::util::string outputPath = ray::util::directory(path) + "ao.tga";
		std::cout << "Save as image : " << outputPath << std::endl;

		if (!this->saveLightMass(outputPath, lightMap.data.get(), lightMap.width, lightMap.height, lightMap.channel, params.lightMass.lightMap.margin))
			return false;

		return true;
	}

private:
	ray::LightMapListenerPtr _lightMapListener;
	ray::LightMassListenerPtr _lightMassListener;
};

__ImplementSubClass(GuiControllerComponent, ray::GameComponent, "GuiController")

GuiControllerComponent::GuiControllerComponent() noexcept
	: _lightMapListener(std::make_shared<AppMapListener>())
	, _lightMassListener(std::make_shared<AppMassListener>())
{
}

GuiControllerComponent::~GuiControllerComponent() noexcept
{
}

bool
GuiControllerComponent::onModelImport(ray::util::string::const_pointer path, ray::util::string& error) noexcept
{
	ray::StreamReaderPtr stream;
	if (!ray::IoServer::instance()->openFile(stream, path))
		return false;

	ray::PMXHandler header;
	if (!header.doCanRead(*stream))
		return false;

	auto model = std::make_unique<ray::PMX>();
	if (!header.doLoad(*stream, *model))
		return false;

	_model = std::move(model);

	return true;
}

bool
GuiControllerComponent::onModelSaveAs(ray::util::string::const_pointer path, ray::util::string& error) noexcept
{
	if (!_model)
		return false;

	ray::StreamWritePtr stream;
	if (!ray::IoServer::instance()->openFile(stream, path))
		return false;

	ray::PMXHandler header;
	if (!header.doSave(*stream, *_model))
		return false;

	return true;
}

bool
GuiControllerComponent::onUVMapperWillStart(const GuiParams& params) noexcept
{
	return _model ? true : false;
}

bool
GuiControllerComponent::onUVMapperStart(const GuiParams& params) noexcept
{
	std::uint32_t size = 512;
	if (params.lightmass.imageSize == 1)
		size = 1024;
	else if (params.lightmass.imageSize == 2)
		size = 2048;
	else if (params.lightmass.imageSize == 3)
		size = 4096;
	else if (params.lightmass.imageSize == 4)
		size = 8192;

	ray::LightMapPack lightPack(_lightMapListener);
	if (!lightPack.atlasUV1(*_model, size, size, params.uvmapper.chart, params.uvmapper.stretch, params.uvmapper.margin))
		return false;

	return true;
}

ray::GameComponentPtr
GuiControllerComponent::clone() const noexcept
{
	return std::make_shared<GuiControllerComponent>();
}

void
GuiControllerComponent::onAttachComponent(ray::GameComponentPtr& component) except
{
	if (component->isInstanceOf<GuiViewComponent>())
	{
		auto view = component->downcast<GuiViewComponent>();
		view->setModelImportListener(std::bind(&GuiControllerComponent::onModelImport, this, std::placeholders::_1, std::placeholders::_2));
		view->setModelSaveAsListener(std::bind(&GuiControllerComponent::onModelSaveAs, this, std::placeholders::_1, std::placeholders::_2));

		view->setUVMapperStartListener(std::bind(&GuiControllerComponent::onUVMapperStart, this, std::placeholders::_1));
		view->setUVMapperWillStartListener(std::bind(&GuiControllerComponent::onUVMapperWillStart, this, std::placeholders::_1));
	}
}

void
GuiControllerComponent::onDetachComponent(ray::GameComponentPtr& component) noexcept
{
	if (component->isInstanceOf<GuiViewComponent>())
	{
		auto view = component->downcast<GuiViewComponent>();
		view->setModelImportListener(nullptr);
		view->setModelSaveAsListener(nullptr);

		view->setUVMapperStartListener(nullptr);
		view->setUVMapperWillStartListener(nullptr);
	}
}