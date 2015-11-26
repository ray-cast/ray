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
#include <ray/render_system.h>
#include <ray/render_scene.h>
#include <ray/render_window.h>
#include <ray/render_pipeline.h>
#include <ray/render_pipeline_manager.h>
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

RenderSystem::RenderSystem() noexcept
{
}

RenderSystem::~RenderSystem() noexcept
{
	this->close();
}

void
RenderSystem::open(WindHandle window, std::size_t w, std::size_t h) except
{
	assert(!_renderPipeline);

	_renderPipeline = std::make_shared<DefaultRenderPipeline>();
	_renderPipeline->open(window, w, h);

	RenderSetting setting;
	setting.enableSSAO = true;
	setting.enableFXAA = true;
	setting.enableFimic = true;
	this->setRenderSetting(setting);
}

void
RenderSystem::close() noexcept
{
	if (_SAT)
	{
		_SAT.reset();
		_SAT = nullptr;
	}

	if (_SSAO)
	{
		_SSAO.reset();
		_SSAO = nullptr;
	}

	if (_SSGI)
	{
		_SSGI.reset();
		_SSGI = nullptr;
	}

	if (_SSR)
	{
		_SSR.reset();
		_SSR = nullptr;
	}

	if (_SSSS)
	{
		_SSSS.reset();
		_SSSS = nullptr;
	}

	if (_fog)
	{
		_fog.reset();
		_fog = nullptr;
	}

	if (_DOF)
	{
		_DOF.reset();
		_DOF = nullptr;
	}

	if (_fimicToneMapping)
	{
		_fimicToneMapping.reset();
		_fimicToneMapping = nullptr;
	}

	if (_FXAA)
	{
		_FXAA.reset();
		_FXAA = nullptr;
	}

	if (_renderPipeline)
	{
		_renderPipeline.reset();
		_renderPipeline = nullptr;
	}
}

void
RenderSystem::setRenderSetting(const RenderSetting& setting) except
{
	if (_setting.enableSAT != setting.enableSAT)
	{
		if (setting.enableSAT)
		{
			_SAT = std::make_shared<Atmospheric>();
			_renderPipeline->addPostProcess(_SAT);
		}
		else if (_SAT)
		{
			_renderPipeline->removePostProcess(_SAT);
			_SAT.reset();
		}
	}

	if (_setting.enableSSAO != setting.enableSSAO)
	{
		if (setting.enableSSAO)
		{
			_SSAO = std::make_shared<SSAO>();
			_renderPipeline->addPostProcess(_SSAO);
		}
		else if (_SSAO)
		{
			_renderPipeline->removePostProcess(_SSAO);
			_SSAO.reset();
		}
	}

	if (_setting.enableSSSS != setting.enableSSSS)
	{
		if (setting.enableSSSS)
		{
			_SSSS = std::make_shared<SSSS>();
			_renderPipeline->addPostProcess(_SSSS);
		}
		else if (_SSSS)
		{
			_renderPipeline->removePostProcess(_SSSS);
			_SSSS.reset();
		}
	}

	if (_setting.enableFog != setting.enableFog)
	{
		if (setting.enableFog)
		{
			_fog = std::make_shared<Fog>();
			_renderPipeline->addPostProcess(_fog);
		}
		else if (_fog)
		{
			_renderPipeline->removePostProcess(_fog);
			_fog.reset();
		}
	}

	if (_setting.enableSSGI != setting.enableSSGI)
	{
		if (setting.enableSSGI)
		{
			_SSGI = std::make_shared<SSGI>();
			_renderPipeline->addPostProcess(_SSGI);
		}
		else if (_SSGI)
		{
			_renderPipeline->removePostProcess(_SSGI);
			_SSGI.reset();
		}
	}

	if (_setting.enableSSR != setting.enableSSR)
	{
		if (setting.enableSSR)
		{
			_SSR = std::make_shared<SSR>();
			_renderPipeline->addPostProcess(_SSR);
		}
		else if (_SSR)
		{
			_renderPipeline->removePostProcess(_SSR);
			_SSR.reset();
		}
	}

	if (_setting.enableDOF != setting.enableDOF)
	{
		if (setting.enableDOF)
		{
			_DOF = std::make_shared<DepthOfField>();
			_renderPipeline->addPostProcess(_DOF);
		}
		else if (_DOF)
		{
			_renderPipeline->removePostProcess(_DOF);
			_DOF.reset();
		}
	}

	if (_setting.enableLightShaft != setting.enableLightShaft)
	{
		if (setting.enableLightShaft)
		{
			_lightShaft = std::make_shared<LightShaft>();
			_renderPipeline->addPostProcess(_lightShaft);
		}
		else if (_lightShaft)
		{
			_renderPipeline->removePostProcess(_lightShaft);
			_lightShaft.reset();
		}
	}

	if (_setting.enableFimic != setting.enableFimic)
	{
		if (setting.enableFimic)
		{
			_fimicToneMapping = std::make_shared<FimicToneMapping>();
			_renderPipeline->addPostProcess(_fimicToneMapping);
		}
		else if (_fimicToneMapping)
		{
			_renderPipeline->removePostProcess(_fimicToneMapping);
			_fimicToneMapping.reset();
		}
	}

	if (_setting.enableFXAA != setting.enableFXAA)
	{
		if (setting.enableFXAA)
		{
			_FXAA = std::make_shared<FXAA>();
			_renderPipeline->addPostProcess(_FXAA);
		}
		else if (_FXAA)
		{
			_renderPipeline->removePostProcess(_FXAA);
			_FXAA.reset();
		}
	}

	if (_setting.enableColorGrading != setting.enableColorGrading)
	{
		if (setting.enableColorGrading)
		{
			_colorGrading = std::make_shared<ColorGrading>();
			_renderPipeline->addPostProcess(_colorGrading);
		}
		else if (_colorGrading)
		{
			_renderPipeline->removePostProcess(_colorGrading);
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
RenderSystem::setWireframeMode(bool enable) noexcept
{
	assert(_renderPipeline);
	_renderPipeline->setWireframeMode(enable);
}

bool 
RenderSystem::getWireframeMode() const noexcept
{
	assert(_renderPipeline);
	return _renderPipeline->getWireframeMode();
}

void 
RenderSystem::setWindowResolution(std::uint32_t w, std::uint32_t h) except
{
	assert(_renderPipeline);
	_renderPipeline->setWindowResolution(w, h);
}

void 
RenderSystem::getWindowResolution(std::uint32_t& w, std::uint32_t& h) const noexcept
{
	assert(_renderPipeline);
	_renderPipeline->getWindowResolution(w, h);
}

void 
RenderSystem::setRenderPipeline(RenderPipelinePtr pipeline) except
{
	assert(_renderPipeline);
	_renderPipeline = pipeline;
}

RenderPipelinePtr 
RenderSystem::getRenderPipeline() const noexcept
{
	assert(_renderPipeline);
	return _renderPipeline;
}

void 
RenderSystem::setSwapInterval(SwapInterval interval) except
{
	assert(_renderPipeline);
	_renderPipeline->setSwapInterval(interval);
}

SwapInterval 
RenderSystem::getSwapInterval() const noexcept
{
	assert(_renderPipeline);
	return _renderPipeline->getSwapInterval();
}

bool
RenderSystem::addRenderScene(RenderScenePtr scene) noexcept
{
	auto it = std::find(_sceneList.begin(), _sceneList.end(), scene);
	if (it == _sceneList.end())
	{
		scene->registerAddCameraDelegate(make_delegate(&RenderSystem::onAddCamera, this));

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

TexturePtr 
RenderSystem::createTexture() noexcept
{
	assert(_renderPipeline);
	return _renderPipeline->createTexture();
}

TexturePtr 
RenderSystem::createTexture(const std::string& name) except
{
	assert(_renderPipeline);
	return _renderPipeline->createTexture(name);
}

MaterialPtr
RenderSystem::createMaterial(const std::string& name) noexcept
{
	assert(_renderPipeline);
	return _renderPipeline->createMaterial(name);
}

RenderTexturePtr 
RenderSystem::createRenderTexture() noexcept
{
	assert(_renderPipeline);
	return _renderPipeline->createRenderTexture();
}

MultiRenderTexturePtr 
RenderSystem::createMultiRenderTexture() noexcept
{
	assert(_renderPipeline);
	return _renderPipeline->createMultiRenderTexture();
}

RenderStatePtr
RenderSystem::createRenderState() noexcept
{
	assert(_renderPipeline);
	return _renderPipeline->createRenderState();
}

ShaderPtr 
RenderSystem::createShader() noexcept
{
	assert(_renderPipeline);
	return _renderPipeline->createShader();
}

ShaderObjectPtr 
RenderSystem::createShaderObject() noexcept
{
	assert(_renderPipeline);
	return _renderPipeline->createShaderObject();
}

RenderBufferPtr 
RenderSystem::createRenderBuffer() noexcept
{
	assert(_renderPipeline);
	return _renderPipeline->createRenderBuffer();
}

RenderBufferPtr 
RenderSystem::createRenderBuffer(const MeshProperty& mesh) except
{
	assert(_renderPipeline);
	return _renderPipeline->createRenderBuffer(mesh);
}

RenderBufferPtr 
RenderSystem::createRenderBuffer(const MeshPropertys& meshes) except
{
	assert(_renderPipeline);
	return _renderPipeline->createRenderBuffer(meshes);
}

void
RenderSystem::render() noexcept
{
	assert(_renderPipeline);

	_renderPipeline->renderBegin();

	for (auto& scene : _sceneList)
		_renderPipeline->render(*scene);

	for (auto& scene : _sceneList)
	{
		auto& cameras = scene->getCameraList();
		for (auto& camera : cameras)
		{
			auto widnow = camera->getRenderWindow();
			if (widnow)
			{
				widnow->present();
			}
		}
	}

	_renderPipeline->renderEnd();
}

void 
RenderSystem::onAddCamera(CameraPtr camera)
{
}

_NAME_END