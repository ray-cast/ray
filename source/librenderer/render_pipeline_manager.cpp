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

#include "deferred_render_pipeline.h"
#include "forward_render_pipeline.h"

#include "atmospheric.h"
#include "ssao.h"
#include "ssgi.h"
#include "ssss.h"
#include "ssr.h"
#include "dof.h"
#include "fog.h"
#include "fimic.h"
#include "fxaa.h"
#include "light_shaft.h"
#include "color_grading.h"

_NAME_BEGIN

__ImplementSubClass(RenderPipelineManager, rtti::Interface, "RenderPipelineManager")

RenderPipelineManager::RenderPipelineManager() noexcept
{
	std::memset(&_setting, 0, sizeof(_setting));
}

RenderPipelineManager::~RenderPipelineManager() noexcept
{
	this->close();
}

bool
RenderPipelineManager::setup(const RenderSetting& setting) noexcept
{
	assert(setting.window);
	assert(setting.width > 0 && setting.height > 0);

	_pipelineDevice = std::make_shared<RenderPipelineDevice>();
	if (!_pipelineDevice->open(setting.deviceType))
		return false;

	_pipeline = _pipelineDevice->createRenderPipeline(setting.window, setting.width, setting.height);
	if (!_pipeline)
		return false;

	auto deferredLighting = std::make_shared<DeferredRenderPipeline>();
	deferredLighting->_setRenderPipeline(_pipeline);
	deferredLighting->setup(*_pipeline);
	_deferredLighting = deferredLighting;

	auto forwardShading = std::make_shared<ForwardRenderPipeline>();
	forwardShading->_setRenderPipeline(_pipeline);
	forwardShading->setup(*_pipeline);
	_forwardShading = forwardShading;

	this->setRenderSetting(setting);
	return true;
}

void 
RenderPipelineManager::close() noexcept
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
	_deferredLighting.reset();
	_forwardShading.reset();
	_pipeline.reset();
}

void
RenderPipelineManager::setRenderSetting(const RenderSetting& setting) noexcept
{
	if (_setting.enableSAT != setting.enableSAT)
	{
		if (setting.enableSAT)
		{
			_SAT = std::make_shared<Atmospheric>();
			this->addPostProcess(_SAT);
		}
		else if (_SAT)
		{
			this->removePostProcess(_SAT);
			_SAT.reset();
		}
	}

	if (_setting.enableSSAO != setting.enableSSAO)
	{
		if (setting.enableSSAO)
		{
			_SSAO = std::make_shared<SSAO>();
			this->addPostProcess(_SSAO);
		}
		else if (_SSAO)
		{
			this->removePostProcess(_SSAO);
			_SSAO.reset();
		}
	}

	if (_setting.enableSSSS != setting.enableSSSS)
	{
		if (setting.enableSSSS)
			_deferredLighting->downcast<DeferredRenderPipeline>()->enableSSSS(true);
		else if (_SSSS)
			_deferredLighting->downcast<DeferredRenderPipeline>()->enableSSSS(false);
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

	if (_setting.enableSSGI != setting.enableSSGI)
	{
		if (setting.enableSSGI)
		{
			_SSGI = std::make_shared<SSGI>();
			this->addPostProcess(_SSGI);
		}
		else if (_SSGI)
		{
			this->removePostProcess(_SSGI);
			_SSGI.reset();
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

	if (_setting.enableFimic != setting.enableFimic)
	{
		if (setting.enableFimic)
		{
			_fimicToneMapping = std::make_shared<FimicToneMapping>();
			this->addPostProcess(_fimicToneMapping);
		}
		else if (_fimicToneMapping)
		{
			this->removePostProcess(_fimicToneMapping);
			_fimicToneMapping.reset();
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

	if (_setting.swapInterval != setting.swapInterval)
	{
		_pipeline->setSwapInterval(setting.swapInterval);
	}

	_setting = setting;
}

const RenderSetting&
RenderPipelineManager::getRenderSetting() const noexcept
{
	return _setting;
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
		_deferredLighting->onRenderPre(*_pipeline);

		RenderListener* renderListener = camera->getOwnerListener();
		if (renderListener)
			renderListener->onRenderObjectPre(*_pipeline);

		_deferredLighting->onRenderPipeline(*_pipeline, camera);

		if (renderListener)
			renderListener->onRenderObjectPost(*_pipeline);

		_deferredLighting->onRenderPost(*_pipeline);
	}
}

void 
RenderPipelineManager::renderEnd() noexcept
{
	_pipeline->present();
	_pipeline->renderEnd();
}

void
RenderPipelineManager::setCamera(CameraPtr camera) noexcept
{
	_pipeline->setCamera(camera);
}

CameraPtr
RenderPipelineManager::getCamera() const noexcept
{
	return _pipeline->getCamera();
}

void
RenderPipelineManager::addRenderData(RenderQueue queue, RenderPass pass, RenderObjectPtr object) noexcept
{
	assert(_pipeline);
	_pipeline->addRenderData(queue, pass, object);
}

const RenderObjects&
RenderPipelineManager::getRenderData(RenderQueue queue, RenderPass pass) const noexcept
{
	assert(_pipeline);
	return _pipeline->getRenderData(queue, pass);
}

void
RenderPipelineManager::setViewport(const Viewport& view) noexcept
{
	assert(_pipeline);
	_pipeline->setViewport(view);
}

const Viewport&
RenderPipelineManager::getViewport() const noexcept
{
	assert(_pipeline);
	return _pipeline->getViewport();
}

void
RenderPipelineManager::setTransform(const float4x4& transform) noexcept
{
	assert(_pipeline);
	_pipeline->setTransform(transform);
}

void
RenderPipelineManager::setTransformInverse(const float4x4& transform) noexcept
{
	assert(_pipeline);
	_pipeline->setTransformInverse(transform);
}

void
RenderPipelineManager::setTransformInverseTranspose(const float4x4& transform) noexcept
{
	assert(_pipeline);
	_pipeline->setTransformInverseTranspose(transform);
}

void
RenderPipelineManager::setFramebuffer(GraphicsFramebufferPtr target) noexcept
{
	assert(_pipeline);
	_pipeline->setFramebuffer(target);
}

void
RenderPipelineManager::clearFramebuffer(GraphicsClearFlags flags, const float4& color, float depth, std::int32_t stencil) noexcept
{
	assert(_pipeline);
	_pipeline->clearFramebuffer(flags, color, depth, stencil);
}

void
RenderPipelineManager::discradRenderTexture() noexcept
{
}

void
RenderPipelineManager::readFramebuffer(GraphicsFramebufferPtr texture, GraphicsFormat pfd, std::size_t w, std::size_t h, void* data) noexcept
{
	assert(_pipeline);
	_pipeline->readFramebuffer(texture, pfd, w, h, data);
}

void
RenderPipelineManager::blitFramebuffer(GraphicsFramebufferPtr srcTarget, const Viewport& src, GraphicsFramebufferPtr destTarget, const Viewport& dest) noexcept
{
	assert(_pipeline);
	_pipeline->blitFramebuffer(srcTarget, src, destTarget, dest);
}

bool
RenderPipelineManager::updateBuffer(GraphicsDataPtr& data, void* str, std::size_t cnt) noexcept
{
	assert(_pipeline);
	return _pipeline->updateBuffer(data, str, cnt);
}

void*
RenderPipelineManager::mapBuffer(GraphicsDataPtr& data, std::uint32_t access) noexcept
{
	assert(_pipeline);
	return _pipeline->mapBuffer(data, access);
}

void
RenderPipelineManager::unmapBuffer(GraphicsDataPtr& data) noexcept
{
	assert(_pipeline);
	return _pipeline->unmapBuffer(data);
}

void
RenderPipelineManager::drawCone(MaterialPassPtr pass) noexcept
{
	assert(_pipeline);
	_pipeline->drawCone(pass);
}

void
RenderPipelineManager::drawSphere(MaterialPassPtr pass) noexcept
{
	assert(_pipeline);
	_pipeline->drawSphere(pass);
}

void
RenderPipelineManager::drawScreenQuad(MaterialPassPtr pass) noexcept
{
	assert(_pipeline);
	_pipeline->drawScreenQuad(pass);
}

void
RenderPipelineManager::drawMesh(MaterialPassPtr pass, RenderBufferPtr mesh, const GraphicsIndirect& renderable) noexcept
{
	assert(_pipeline);
	_pipeline->drawMesh(pass, mesh, renderable);
}

void
RenderPipelineManager::setWindowResolution(std::uint32_t width, std::uint32_t height) noexcept
{
	if (_setting.width != width || _setting.height != height)
	{
		_deferredLighting->onResolutionChangeBefore(*_pipeline);
		_forwardShading->onResolutionChangeBefore(*_pipeline);

		_pipeline->setWindowResolution(width, height);

		_deferredLighting->onResolutionChangeAfter(*_pipeline);
		_forwardShading->onResolutionChangeAfter(*_pipeline);

		_setting.width = width;
		_setting.height = height;
	}
}

void
RenderPipelineManager::getWindowResolution(std::uint32_t& w, std::uint32_t& h) const noexcept
{
	w = _setting.width;
	h = _setting.height;
}

void
RenderPipelineManager::addPostProcess(RenderPostProcessPtr postprocess) noexcept
{
	assert(_pipeline);
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

RenderPipelinePtr 
RenderPipelineManager::createRenderPipeline(WindHandle window, std::uint32_t w, std::uint32_t h) noexcept
{
	assert(_pipelineDevice);
	return _pipelineDevice->createRenderPipeline(window, w, h);
}

GraphicsSwapchainPtr 
RenderPipelineManager::createSwapchain(const GraphicsSwapchainDesc& desc) noexcept
{
	assert(_pipelineDevice);
	return _pipelineDevice->createSwapchain(desc);
}

GraphicsContextPtr 
RenderPipelineManager::createDeviceContext(const GraphicsContextDesc& desc) noexcept
{
	assert(_pipelineDevice);
	return _pipelineDevice->createDeviceContext(desc);
}

GraphicsFramebufferPtr 
RenderPipelineManager::createFramebuffer(const GraphicsFramebufferDesc& desc) noexcept
{
	assert(_pipelineDevice);
	return _pipelineDevice->createFramebuffer(desc);
}

GraphicsFramebufferLayoutPtr 
RenderPipelineManager::createFramebufferLayout(const GraphicsFramebufferLayoutDesc& desc) noexcept
{
	assert(_pipelineDevice);
	return _pipelineDevice->createFramebufferLayout(desc);
}

GraphicsTexturePtr 
RenderPipelineManager::createTexture(const GraphicsTextureDesc& desc) noexcept
{
	assert(_pipelineDevice);
	return _pipelineDevice->createTexture(desc);
}

GraphicsTexturePtr
RenderPipelineManager::createTexture(std::uint32_t w, std::uint32_t h, GraphicsTextureDim dim, GraphicsFormat format) noexcept
{
	assert(_pipelineDevice);
	return _pipelineDevice->createTexture(w, h, dim, format);
}

GraphicsTexturePtr 
RenderPipelineManager::createTexture(const std::string& name) noexcept
{
	assert(_pipelineDevice);
	return _pipelineDevice->createTexture(name);
}

MaterialPtr 
RenderPipelineManager::createMaterial(const std::string& name) noexcept
{
	assert(_pipelineDevice);
	return _pipelineDevice->createMaterial(name);
}

void
RenderPipelineManager::destroyMaterial(MaterialPtr material) noexcept
{
	_pipelineDevice->destroyMaterial(material);
}

GraphicsInputLayoutPtr 
RenderPipelineManager::createInputLayout(const GraphicsInputLayoutDesc& desc) noexcept
{
	assert(_pipelineDevice);
	return _pipelineDevice->createInputLayout(desc);
}

GraphicsDataPtr 
RenderPipelineManager::createGraphicsData(const GraphicsDataDesc& desc) noexcept
{
	assert(_pipelineDevice);
	return _pipelineDevice->createGraphicsData(desc);
}

RenderBufferPtr 
RenderPipelineManager::createRenderBuffer(GraphicsDataPtr vb, GraphicsDataPtr ib) noexcept
{
	assert(_pipelineDevice);
	return _pipelineDevice->createRenderBuffer(vb, ib);
}

RenderBufferPtr
RenderPipelineManager::createRenderBuffer(const MeshProperty& mesh) noexcept
{
	assert(_pipelineDevice);
	return _pipelineDevice->createRenderBuffer(mesh);
}

RenderBufferPtr
RenderPipelineManager::createRenderBuffer(const MeshPropertys& mesh) noexcept
{
	assert(_pipelineDevice);
	return _pipelineDevice->createRenderBuffer(mesh);
}

_NAME_END