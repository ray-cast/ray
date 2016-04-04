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
#include <ray/render_scene.h>
#include <ray/camera.h>
#include <ray/graphics_context.h>

#include "deferred_render_pipeline.h"

_NAME_BEGIN

RenderPipelineManager::RenderPipelineManager() noexcept
{
}

RenderPipelineManager::~RenderPipelineManager() noexcept
{
}

bool
RenderPipelineManager::open(RenderPipelinePtr pipeline) noexcept
{
	_renderPipeline = pipeline;

	_deferredLighting = std::make_shared<DeferredRenderPipeline>();
	_deferredLighting->_setRenderPipeline(_renderPipeline);
	_deferredLighting->setActive(true);
	return true;
}

void 
RenderPipelineManager::close() noexcept
{
	if (_deferredLighting)
	{
		_deferredLighting.reset();
		_deferredLighting = nullptr;
	}

	if (_renderPipeline)
	{
		_renderPipeline.reset();
		_renderPipeline = nullptr;
	}
}

void
RenderPipelineManager::setRenderPipeline(RenderPipelinePtr pipeline) noexcept
{
	if (_renderPipeline != pipeline)
	{
		_deferredLighting->setActive(false);
		_deferredLighting->_setRenderPipeline(pipeline);
		_deferredLighting->setActive(true);

		_renderPipeline = pipeline;
	}
}

RenderPipelinePtr 
RenderPipelineManager::getRenderPipeline() noexcept
{
	return _renderPipeline;
}

void 
RenderPipelineManager::setWindowResolution(std::uint32_t w, std::uint32_t h) noexcept
{
	_renderPipeline->renderBegin();
	_deferredLighting->onResolutionChangeBefore(*_renderPipeline);
	_renderPipeline->setWindowResolution(w, h);
	_deferredLighting->onResolutionChangeAfter(*_renderPipeline);
	_renderPipeline->renderEnd();
}

void 
RenderPipelineManager::getWindowResolution(std::uint32_t& w, std::uint32_t& h) const noexcept
{
	_renderPipeline->getWindowResolution(w, h);
}

void
RenderPipelineManager::renderBegin() noexcept
{
}

void 
RenderPipelineManager::render(const RenderScene& scene) noexcept
{
	if (!_renderPipeline)
		return;

	_renderPipeline->renderBegin();

	auto& cameras = scene.getCameraList();
	for (auto& camera : cameras)
	{
		_deferredLighting->onRenderPre(*_renderPipeline);

		RenderListener* renderListener = camera->getOwnerListener();
		if (renderListener)
			renderListener->onWillRenderObject(*camera);

		_renderPipeline->setCamera(camera);

		_deferredLighting->onRenderPipeline(*_renderPipeline);

		if (renderListener)
			renderListener->onRenderObject(*_renderPipeline, *camera);

		_deferredLighting->onRenderPost(*_renderPipeline);
	}

	_renderPipeline->renderEnd();
}

void 
RenderPipelineManager::renderEnd() noexcept
{
}

_NAME_END