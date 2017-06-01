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
#include "LightMassAmbientOcclusion.h"
#include "LightMassGlobalIllumination.h"

_NAME_BEGIN

LightMass::LightMass() noexcept
	: _initialize(false)
	, _lightMassListener(std::make_shared<LightMassListener>())
	, _lightMapData(std::make_shared<LightMapData>())
	, _isStopped(true)
{
}

LightMass::LightMass(LightMassListenerPtr listener) noexcept
	: _initialize(false)
	, _lightMassListener(listener)
	, _lightMapData(std::make_shared<LightMapData>())
	, _isStopped(true)
{
}

LightMass::~LightMass() noexcept
{
	this->close();
}

bool
LightMass::open(const LightMassParams& params) noexcept
{
	assert(!_initialize);

	GraphicsDeviceDesc deviceDesc;
	deviceDesc.setDeviceType(ray::GraphicsDeviceType::GraphicsDeviceTypeOpenGL);
	_graphicsDevice = GraphicsSystem::instance()->createDevice(deviceDesc);
	if (!_graphicsDevice)
	{
		auto listener = this->getLightMassListener();
		if (listener)
			listener->onMessage("Could not initialize with GraphicsDevice.");

		return false;
	}

	ray::GraphicsSwapchainDesc swapchainDesc;
	swapchainDesc.setWindHandle(nullptr);
	swapchainDesc.setWidth(1);
	swapchainDesc.setHeight(1);
	swapchainDesc.setSwapInterval(GraphicsSwapInterval::GraphicsSwapIntervalFree);
	swapchainDesc.setImageNums(2);
	swapchainDesc.setColorFormat(GraphicsFormat::GraphicsFormatB8G8R8A8UNorm);
	swapchainDesc.setDepthStencilFormat(GraphicsFormat::GraphicsFormatD24UNorm_S8UInt);
	_graphicsSwapchain = _graphicsDevice->createSwapchain(swapchainDesc);
	if (!_graphicsSwapchain)
	{
		auto listener = this->getLightMassListener();
		if (listener)
			listener->onMessage("Could not initialize with GraphicsSwapchain.");

		return false;
	}

	ray::GraphicsContextDesc contextDesc;
	contextDesc.setSwapchain(_graphicsSwapchain);
	_graphicsContext = _graphicsDevice->createDeviceContext(contextDesc);
	if (!_graphicsContext)
	{
		auto listener = this->getLightMassListener();
		if (listener)
			listener->onMessage("Could not initialize with GraphicsContext.");

		return false;
	}

	LightBakingParams option;
	option.model = params.model;
	option.baking = params.baking;
	option.lightMap = _lightMapData;

	if (option.lightMap->channel == 4)
	{
		auto lightMass = std::make_shared<LightBakingGI>();
		lightMass->setLightMassListener(_lightMassListener);
		lightMass->open(option);
		_lightMassBaking = std::move(lightMass);
	}
	else
	{
		auto lightMass = std::make_shared<LightBakingAO>();
		lightMass->setLightMassListener(_lightMassListener);
		lightMass->open(option);
		_lightMassBaking = std::move(lightMass);
	}

	_initialize = true;

	return true;
}

void
LightMass::close() noexcept
{
	if (_lightMassBaking)
		_lightMassBaking->stop();

	if (_graphicsContext)
		_graphicsContext.reset();

	if (_graphicsSwapchain)
		_graphicsSwapchain.reset();

	if (_graphicsDevice)
		_graphicsDevice.reset();

	_initialize = false;
}

void
LightMass::setLightMassListener(LightMassListenerPtr pointer) noexcept
{
	if (_lightMassListener != pointer)
	{
		if (_lightMassListener)
			_lightMassListener->onListenerChangeBefore();

		_lightMassListener = pointer;

		if (_lightMassListener)
			_lightMassListener->onListenerChangeAfter();
	}
}

LightMassListenerPtr
LightMass::getLightMassListener() const noexcept
{
	return _lightMassListener;
}

void
LightMass::setLightMapData(LightMapDataPtr data) noexcept
{
	assert(data->data);
	assert(data->width >= 0 && data->height >= 0);
	assert(data->channel == 4 || data->channel == 1);

	_lightMapData = data;
}

LightMapDataPtr
LightMass::getLightMapData() const noexcept
{
	return _lightMapData;
}

bool
LightMass::start() noexcept
{
	if (_isStopped)
		_isStopped = false;

	if (this->getLightMassListener())
		this->getLightMassListener()->onBakingStart();

	if (!_lightMassBaking->start())
	{
		if (_lightMassListener)
			_lightMassListener->onMessage("Failed to baking the model");

		return false;
	}

	if (this->getLightMassListener())
		this->getLightMassListener()->onBakingEnd();

	return true;
}

void
LightMass::stop() noexcept
{
	if (!_isStopped)
	{
		if (_lightMassBaking)
			_lightMassBaking->stop();

		_isStopped = true;
	}
}

_NAME_END