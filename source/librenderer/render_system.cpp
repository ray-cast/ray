// +----------------------------------------------------------------------
// | Project : ray.
// | All rights reserved.
// +----------------------------------------------------------------------
// | Copyright (c) 2013-2016.
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
#include <ray/render_system.h>
#include <ray/render_scene.h>
#include <ray/render_pipeline.h>
#include <ray/render_pipeline_manager.h>
#include <ray/graphics_context.h>
#include <ray/atmospheric.h>
#include <ray/ssao.h>
#include <ray/ssgi.h>
#include <ray/ssss.h>
#include <ray/ssr.h>
#include <ray/dof.h>
#include <ray/fog.h>
#include <ray/fimic.h>
#include <ray/fxaa.h>
#include <ray/light_shaft.h>
#include <ray/color_grading.h>

_NAME_BEGIN

__ImplementSingleton(RenderSystem)

RenderSetting::RenderSetting() noexcept
	: enableSSAO(false)
	, enableSSGI(false)
	, enableSAT(false)
	, enableSSR(false)
	, enableSSSS(false)
	, enableFog(false)
	, enableDOF(false)
	, enableFimic(false)
	, enableFXAA(false)
	, enableLightShaft(false)
	, enableColorGrading(false)
{
}

RenderSystem::RenderSystem() noexcept
{
}

RenderSystem::~RenderSystem() noexcept
{
	this->close();
}

bool
RenderSystem::open(WindHandle window, std::size_t w, std::size_t h) noexcept
{
	assert(!_pipeline);

	_pipeline = std::make_shared<RenderPipeline>();
	if (!_pipeline->open(window, w, h))
		return false;

	_pipelineManager = std::make_shared<RenderPipelineManager>();
	if (!_pipelineManager->open(_pipeline))
		return false;

	RenderSetting setting;
	setting.enableSSSS = false;
	setting.enableSSAO = false;
	setting.enableFimic = false;
	setting.enableFXAA = false;
	setting.enableColorGrading = true;
	this->setRenderSetting(setting);
	return true;
}

void
RenderSystem::close() noexcept
{
	_SAT.reset();
	_SSAO.reset();
	_SSGI.reset();
	_SSR.reset();
	_SSSS.reset();
	_fog.reset();
	_DOF.reset();
	_fimicToneMapping.reset();
	_FXAA.reset();

	_pipeline.reset();
	_pipelineManager.reset();
}

void
RenderSystem::setRenderSetting(const RenderSetting& setting) noexcept
{
	if (_setting.enableSAT != setting.enableSAT)
	{
		if (setting.enableSAT)
		{
			_SAT = std::make_shared<Atmospheric>();
			_pipeline->addPostProcess(_SAT);
		}
		else if (_SAT)
		{
			_pipeline->removePostProcess(_SAT);
			_SAT.reset();
		}
	}

	if (_setting.enableSSAO != setting.enableSSAO)
	{
		if (setting.enableSSAO)
		{
			_SSAO = std::make_shared<SSAO>();
			_pipeline->addPostProcess(_SSAO);
		}
		else if (_SSAO)
		{
			_pipeline->removePostProcess(_SSAO);
			_SSAO.reset();
		}
	}

	if (_setting.enableSSSS != setting.enableSSSS)
	{
		if (setting.enableSSSS)
		{
			_SSSS = std::make_shared<SSSS>();
			_pipeline->addPostProcess(_SSSS);
		}
		else if (_SSSS)
		{
			_pipeline->removePostProcess(_SSSS);
			_SSSS.reset();
		}
	}

	if (_setting.enableFog != setting.enableFog)
	{
		if (setting.enableFog)
		{
			_fog = std::make_shared<Fog>();
			_pipeline->addPostProcess(_fog);
		}
		else if (_fog)
		{
			_pipeline->removePostProcess(_fog);
			_fog.reset();
		}
	}

	if (_setting.enableSSGI != setting.enableSSGI)
	{
		if (setting.enableSSGI)
		{
			_SSGI = std::make_shared<SSGI>();
			_pipeline->addPostProcess(_SSGI);
		}
		else if (_SSGI)
		{
			_pipeline->removePostProcess(_SSGI);
			_SSGI.reset();
		}
	}

	if (_setting.enableSSR != setting.enableSSR)
	{
		if (setting.enableSSR)
		{
			_SSR = std::make_shared<SSR>();
			_pipeline->addPostProcess(_SSR);
		}
		else if (_SSR)
		{
			_pipeline->removePostProcess(_SSR);
			_SSR.reset();
		}
	}

	if (_setting.enableDOF != setting.enableDOF)
	{
		if (setting.enableDOF)
		{
			_DOF = std::make_shared<DepthOfField>();
			_pipeline->addPostProcess(_DOF);
		}
		else if (_DOF)
		{
			_pipeline->removePostProcess(_DOF);
			_DOF.reset();
		}
	}

	if (_setting.enableLightShaft != setting.enableLightShaft)
	{
		if (setting.enableLightShaft)
		{
			_lightShaft = std::make_shared<LightShaft>();
			_pipeline->addPostProcess(_lightShaft);
		}
		else if (_lightShaft)
		{
			_pipeline->removePostProcess(_lightShaft);
			_lightShaft.reset();
		}
	}

	if (_setting.enableFimic != setting.enableFimic)
	{
		if (setting.enableFimic)
		{
			_fimicToneMapping = std::make_shared<FimicToneMapping>();
			_pipeline->addPostProcess(_fimicToneMapping);
		}
		else if (_fimicToneMapping)
		{
			_pipeline->removePostProcess(_fimicToneMapping);
			_fimicToneMapping.reset();
		}
	}

	if (_setting.enableFXAA != setting.enableFXAA)
	{
		if (setting.enableFXAA)
		{
			_FXAA = std::make_shared<FXAA>();
			_pipeline->addPostProcess(_FXAA);
		}
		else if (_FXAA)
		{
			_pipeline->removePostProcess(_FXAA);
			_FXAA.reset();
		}
	}

	if (_setting.enableColorGrading != setting.enableColorGrading)
	{
		if (setting.enableColorGrading)
		{
			_colorGrading = std::make_shared<ColorGrading>();
			_pipeline->addPostProcess(_colorGrading);
		}
		else if (_colorGrading)
		{
			_pipeline->removePostProcess(_colorGrading);
			_colorGrading.reset();
		}
	}
}

const RenderSetting&
RenderSystem::getRenderSetting() const noexcept
{
	return _setting;
}

void 
RenderSystem::setWindowResolution(std::uint32_t w, std::uint32_t h) noexcept
{
	assert(_pipeline);
	_pipelineManager->setWindowResolution(w, h);
}

void 
RenderSystem::getWindowResolution(std::uint32_t& w, std::uint32_t& h) const noexcept
{
	assert(_pipeline);
	_pipelineManager->getWindowResolution(w, h);
}

void 
RenderSystem::setRenderPipeline(RenderPipelinePtr pipeline) noexcept
{
	assert(_pipeline);
	_pipeline = pipeline;
}

RenderPipelinePtr 
RenderSystem::getRenderPipeline() const noexcept
{
	assert(_pipeline);
	return _pipeline;
}

void 
RenderSystem::setSwapInterval(GraphicsSwapInterval interval) noexcept
{
	assert(_pipeline);
	_pipeline->setSwapInterval(interval);
}

GraphicsSwapInterval
RenderSystem::getSwapInterval() const noexcept
{
	assert(_pipeline);
	return _pipeline->getSwapInterval();
}

bool
RenderSystem::addRenderScene(RenderScenePtr scene) noexcept
{
	auto it = std::find(_sceneList.begin(), _sceneList.end(), scene);
	if (it == _sceneList.end())
	{
		_sceneList.push_back(scene);
		return true;
	}

	return false;
}

void
RenderSystem::removeRenderScene(RenderScenePtr scene) noexcept
{
	auto it = std::find(_sceneList.begin(), _sceneList.end(), scene);
	if (it != _sceneList.end())
	{
		_sceneList.erase(it);
	}
}

GraphicsTexturePtr
RenderSystem::createTexture(const GraphicsTextureDesc& desc) noexcept
{
	assert(_pipeline);
	return _pipeline->createTexture(desc);
}

GraphicsTexturePtr
RenderSystem::createTexture(std::uint32_t w, std::uint32_t h, GraphicsTextureDim dim, GraphicsFormat format) noexcept
{
	assert(_pipeline);
	return _pipeline->createTexture(w, h, dim, format);
}

GraphicsTexturePtr
RenderSystem::createTexture(const std::string& name) noexcept
{
	assert(_pipeline);
	return _pipeline->createTexture(name);
}

MaterialPtr
RenderSystem::createMaterial(const std::string& name) noexcept
{
	assert(_pipeline);
	return _pipeline->createMaterial(name);
}

GraphicsFramebufferPtr 
RenderSystem::createFramebuffer(const GraphicsFramebufferDesc& desc) noexcept
{
	assert(_pipeline);
	return _pipeline->createFramebuffer(desc);
}

GraphicsDataPtr
RenderSystem::createGraphicsData(const GraphicsDataDesc& desc) noexcept
{
	assert(_pipeline);
	return _pipeline->createGraphicsData(desc);
}

bool
RenderSystem::updateBuffer(GraphicsDataPtr& data, void* str, std::size_t cnt) noexcept
{
	assert(_pipeline);
	return _pipeline->updateBuffer(data, str, cnt);
}

void*
RenderSystem::mapBuffer(GraphicsDataPtr& data, std::uint32_t access) noexcept
{
	assert(_pipeline);
	return _pipeline->mapBuffer(data, access);
}

void 
RenderSystem::unmapBuffer(GraphicsDataPtr& data) noexcept
{
	assert(_pipeline);
	_pipeline->unmapBuffer(data);
}

GraphicsInputLayoutPtr
RenderSystem::createInputLayout(const GraphicsInputLayoutDesc& desc) noexcept
{
	assert(_pipeline);
	return _pipeline->createInputLayout(desc);
}

RenderBufferPtr 
RenderSystem::createRenderBuffer(GraphicsDataPtr vb, GraphicsDataPtr ib) noexcept
{
	assert(_pipeline);
	return _pipeline->createRenderBuffer(vb, ib);
}

RenderBufferPtr 
RenderSystem::createRenderBuffer(const MeshProperty& mesh) noexcept
{
	assert(_pipeline);
	return _pipeline->createRenderBuffer(mesh);
}

RenderBufferPtr 
RenderSystem::createRenderBuffer(const MeshPropertys& meshes) noexcept
{
	assert(_pipeline);
	return _pipeline->createRenderBuffer(meshes);
}

void
RenderSystem::renderBegin() noexcept
{
	assert(_pipelineManager);
	_pipelineManager->renderBegin();
}

void
RenderSystem::renderEnd() noexcept
{
	assert(_pipelineManager);
	_pipelineManager->renderEnd();
}

void
RenderSystem::render() noexcept
{
	assert(_pipeline);

	for (auto& scene : _sceneList)
		_pipelineManager->render(*scene);
}

_NAME_END