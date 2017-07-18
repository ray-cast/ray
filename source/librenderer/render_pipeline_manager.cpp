// +----------------------------------------------------------------------
// | Project : ray.
// | All rights reserved.
// +----------------------------------------------------------------------
// | Copyright (c) 2013-2014.
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
#include <ray/render_pipeline_manager.h>
#include <ray/render_pipeline_device.h>
#include <ray/render_pipeline.h>
#include <ray/render_scene.h>
#include <ray/camera.h>
#include <ray/deferred_lighting_framebuffers.h>
#include <ray/except.h>

#include "deferred_lighting_pipeline.h"
#include "forward_render_pipeline.h"
#include "shadow_render_pipeline.h"
#include "lightprobe_render_pipeline.h"

#include "atmospheric.h"
#include "ssdo.h"
#include "ssss.h"
#include "ssr.h"
#include "dof.h"
#include "fog.h"
#include "postprocess_hdr.h"
#include "fxaa.h"
#include "light_shaft.h"
#include "color_grading.h"

_NAME_BEGIN

__ImplementSubClass(RenderPipelineManager, rtti::Interface, "RenderPipelineManager")

RenderPipelineManager::RenderPipelineManager() noexcept
{
	std::memset(&_setting, 0, sizeof(_setting));
}

RenderPipelineManager::RenderPipelineManager(const RenderSetting& setting) except
	: RenderPipelineManager()
{
	this->setup(setting);
}

RenderPipelineManager::~RenderPipelineManager() noexcept
{
	this->close();
}

void
RenderPipelineManager::setup(const RenderSetting& setting) except
{
	assert(setting.window);
	assert(setting.width > 0 && setting.height > 0);
	assert(setting.dpi_w > 0 && setting.dpi_h > 0);

	if (setting.deviceType == GraphicsDeviceType::GraphicsDeviceTypeOpenGLES2)
	{
		if (setting.pipelineType != RenderPipelineType::RenderPipelineTypeForward)
			throw failure("Unsupported pipeline type");
	}

	_pipelineDevice = std::make_shared<RenderPipelineDevice>();
	if (!_pipelineDevice->open(setting.deviceType))
		throw failure("Failed to open the pipeline device");

	_pipeline = _pipelineDevice->createRenderPipeline(setting.window, setting.width, setting.height, setting.dpi_w, setting.dpi_h, setting.swapInterval);
	if (!_pipeline)
		throw failure("Failed to create the pipeline");

	auto forwardShading = std::make_shared<ForwardRenderPipeline>();
	if (!forwardShading->setup(_pipeline))
		throw failure("Failed to create the ForwardRenderPipeline");

	_forward = forwardShading;

	auto lightProbeGen = std::make_shared<LightProbeRenderPipeline>();
	if (!lightProbeGen->setup(_pipeline))
		throw failure("Failed to create the LightProbeRenderPipeline");

	_lightProbeGen = lightProbeGen;

	this->setRenderSetting(setting);
}

void
RenderPipelineManager::close() noexcept
{
	_atmospheric.reset();
	_SSDO.reset();
	_SSGI.reset();
	_SSR.reset();
	_SSSS.reset();
	_fog.reset();
	_DOF.reset();
	_PostProcessHDR.reset();
	_FXAA.reset();
	_colorGrading.reset();

	_forward.reset();
	_lightProbeGen.reset();
	_shadowMapGen.reset();
	_deferredLighting.reset();

	_pipeline.reset();
	_pipelineDevice.reset();
}

void
RenderPipelineManager::setRenderPipeline(const RenderPipelinePtr& pipeline) noexcept
{
	_pipeline = pipeline;
}

const RenderPipelinePtr&
RenderPipelineManager::getRenderPipeline() const noexcept
{
	return _pipeline;
}

void
RenderPipelineManager::setRenderPipelineDevice(const RenderPipelineDevicePtr& device) noexcept
{
	_pipelineDevice = device;
}

const RenderPipelineDevicePtr&
RenderPipelineManager::getRenderPipelineDevice() const noexcept
{
	return _pipelineDevice;
}

bool
RenderPipelineManager::setRenderSetting(const RenderSetting& setting) noexcept
{
	_setting.enableGlobalIllumination = setting.enableGlobalIllumination;

	if (_setting.enableAtmospheric != setting.enableAtmospheric)
	{
		if (setting.enableAtmospheric)
		{
			Atmospheric::Setting createParams;
			createParams.earthRadius = setting.earthRadius;
			createParams.earthScaleHeight = setting.earthScaleHeight;
			createParams.minElevation = setting.minElevation;
			createParams.maxElevation = setting.maxElevation;
			createParams.rayleighAngularSctrCoeff = setting.rayleighAngularSctrCoeff;
			createParams.rayleighExtinctionCoeff = setting.rayleighExtinctionCoeff;
			createParams.mieAngularSctrCoeff = setting.mieAngularSctrCoeff;
			createParams.mieExtinctionCoeff = setting.mieExtinctionCoeff;
			createParams.mie = setting.mie;

			_atmospheric = std::make_shared<Atmospheric>(createParams);
			this->addPostProcess(_atmospheric);
		}
		else if (_atmospheric)
		{
			this->removePostProcess(_atmospheric);
			_atmospheric.reset();
		}
	}

	if (_setting.enableSSDO != setting.enableSSDO)
	{
		if (setting.enableSSDO)
		{
			_SSDO = std::make_shared<SSDO>();
			this->addPostProcess(_SSDO);
		}
		else if (_SSDO)
		{
			this->removePostProcess(_SSDO);
			_SSDO.reset();
		}
	}

	if (_setting.enableFog != setting.enableFog)
	{
		if (setting.enableFog)
		{
			_fog = std::make_shared<Fog>();
			this->addPostProcess(_fog);
		}
		else if (_fog)
		{
			this->removePostProcess(_fog);
			_fog.reset();
		}
	}

	if (_setting.enableSSSS != setting.enableSSSS)
	{
		if (setting.enableSSSS)
		{
			_SSSS = std::make_shared<SSSS>();
			this->addPostProcess(_SSSS);
		}
		else if (_SSSS)
		{
			this->removePostProcess(_SSSS);
			_SSSS.reset();
		}
	}

	if (_setting.enableSSR != setting.enableSSR)
	{
		if (setting.enableSSR)
		{
			_SSR = std::make_shared<SSR>();
			this->addPostProcess(_SSR);
		}
		else if (_SSR)
		{
			this->removePostProcess(_SSR);
			_SSR.reset();
		}
	}

	if (_setting.enableDOF != setting.enableDOF)
	{
		if (setting.enableDOF)
		{
			_DOF = std::make_shared<DepthOfField>();
			this->addPostProcess(_DOF);
		}
		else if (_DOF)
		{
			this->removePostProcess(_DOF);
			_DOF.reset();
		}
	}

	if (_setting.enableLightShaft != setting.enableLightShaft)
	{
		if (setting.enableLightShaft)
		{
			_lightShaft = std::make_shared<LightShaft>();
			this->addPostProcess(_lightShaft);
		}
		else if (_lightShaft)
		{
			this->removePostProcess(_lightShaft);
			_lightShaft.reset();
		}
	}

	if (_setting.enableHDR != setting.enableHDR)
	{
		if (setting.enableHDR)
		{
			_PostProcessHDR = std::make_shared<PostProcessHDR>();
			this->addPostProcess(_PostProcessHDR);
		}
		else if (_PostProcessHDR)
		{
			this->removePostProcess(_PostProcessHDR);
			_PostProcessHDR.reset();
		}
	}

	if (_setting.enableColorGrading != setting.enableColorGrading)
	{
		if (setting.enableColorGrading)
		{
			_colorGrading = std::make_shared<ColorGrading>();
			this->addPostProcess(_colorGrading);
		}
		else if (_colorGrading)
		{
			this->removePostProcess(_colorGrading);
			_colorGrading.reset();
		}
	}

	if (_setting.enableFXAA != setting.enableFXAA)
	{
		if (setting.enableFXAA)
		{
			_FXAA = std::make_shared<FXAA>();
			this->addPostProcess(_FXAA);
		}
		else if (_FXAA)
		{
			this->removePostProcess(_FXAA);
			_FXAA.reset();
		}
	}

	if (setting.shadowQuality != ShadowQuality::ShadowQualityNone)
	{
		if (!this->setupShadowRenderer(_pipeline, setting))
			return false;
	}

	if (setting.pipelineType == RenderPipelineType::RenderPipelineTypeDeferredLighting)
	{
		auto deferredLighting = std::make_shared<DeferredLightingPipeline>();
		if (!deferredLighting->setup(_pipeline, _setting.enableGlobalIllumination))
			return false;

		_deferredLighting = deferredLighting;
	}

	_pipeline->setSwapInterval(setting.swapInterval);

	_setting = setting;
	return true;
}

const RenderSetting&
RenderPipelineManager::getRenderSetting() const noexcept
{
	return _setting;
}

void
RenderPipelineManager::setWindowResolution(std::uint32_t width, std::uint32_t height) noexcept
{
	if (_setting.width != width || _setting.height != height)
	{
		_pipeline->setWindowResolution(width, height);

		_setting.width = width;
		_setting.height = height;

		_forward->onResolutionChange();

		if (_lightProbeGen)
			_lightProbeGen->onResolutionChange();

		if (_deferredLighting)
			_deferredLighting->onResolutionChange();
	}
}

void
RenderPipelineManager::getWindowResolution(std::uint32_t& w, std::uint32_t& h) const noexcept
{
	w = _setting.width;
	h = _setting.height;
}

void
RenderPipelineManager::setFramebufferSize(std::uint32_t w, std::uint32_t h) noexcept
{
	if (_setting.dpi_w != w || _setting.dpi_h != h)
	{
		_pipeline->setFramebufferSize(w, h);

		_setting.dpi_w = w;
		_setting.dpi_h = h;

		_forward->onResolutionChangeDPI();

		if (_lightProbeGen)
			_lightProbeGen->onResolutionChangeDPI();

		if (_deferredLighting)
			_deferredLighting->onResolutionChangeDPI();
	}
}

void
RenderPipelineManager::getFramebufferSize(std::uint32_t& w, std::uint32_t& h) const noexcept
{
	w = _setting.dpi_w;
	h = _setting.dpi_h;
}

void
RenderPipelineManager::addPostProcess(RenderPostProcessPtr postprocess) noexcept
{
	assert(_pipeline);
	postprocess->_setRenderPipeline(_pipeline.get());
	_pipeline->addPostProcess(postprocess);
}

void
RenderPipelineManager::removePostProcess(RenderPostProcessPtr postprocess) noexcept
{
	assert(_pipeline);
	_pipeline->removePostProcess(postprocess);
}

void
RenderPipelineManager::destroyPostProcess() noexcept
{
	assert(_pipeline);
	_pipeline->destroyPostProcess();
}

bool
RenderPipelineManager::setupShadowRenderer(RenderPipelinePtr pipeline, const RenderSetting& setting) noexcept
{
	auto shadowMapGen = std::make_shared<ShadowRenderPipeline>();
	shadowMapGen->setShadowMode(setting.shadowMode);
	shadowMapGen->setShadowQuality(setting.shadowQuality);

	if (!shadowMapGen->setup(pipeline))
		return false;

	_shadowMapGen = shadowMapGen;
	return true;
}

void
RenderPipelineManager::destroyShadowRenderer() noexcept
{
	_shadowMapGen.reset();
}

void
RenderPipelineManager::renderBegin() noexcept
{
	assert(_pipeline);
	_pipeline->renderBegin();
}

void
RenderPipelineManager::render(const RenderScene& scene) noexcept
{
	assert(_pipeline);

	auto& cameras = scene.getCameraList();
	for (auto& camera : cameras)
	{
		if (!camera)
			continue;

		if (!camera->getRenderPipelineFramebuffer())
			continue;

		if (!camera->getRenderDataManager())
			continue;

		camera->onRenderBefore(*camera);

		switch (camera->getCameraOrder())
		{
		case CameraOrder::CameraOrderShadow:
		{
			_shadowMapGen->onRenderBefore();
			_shadowMapGen->onRenderPipeline(camera);
			_shadowMapGen->onRenderAfter();
		}
		break;
		case CameraOrder::CameraOrderLightProbe:
		{
			_lightProbeGen->onRenderBefore();
			_lightProbeGen->onRenderPipeline(camera);
			_lightProbeGen->onRenderAfter();
		}
		break;
		case CameraOrder::CameraOrder2D:
		{
			_forward->onRenderBefore();
			_forward->onRenderPipeline(camera);
			_forward->onRenderAfter();
		}
		break;
		case CameraOrder::CameraOrder3D:
		{
			if (camera->getRenderPipelineFramebuffer()->isInstanceOf<DeferredLightingFramebuffers>())
			{
				if (_setting.pipelineType == RenderPipelineType::RenderPipelineTypeDeferredLighting)
				{
					_deferredLighting->onRenderBefore();
					_deferredLighting->onRenderPipeline(camera);
					_deferredLighting->onRenderAfter();
				}
			}
			else
			{
				_forward->onRenderBefore();
				_forward->onRenderPipeline(camera);
				_forward->onRenderAfter();
			}
		}
		break;
		default:
			assert(false);
			break;
		}

		camera->onRenderAfter(*camera);
	}
}

void
RenderPipelineManager::renderEnd() noexcept
{
	_pipeline->present();
	_pipeline->renderEnd();
}

_NAME_END