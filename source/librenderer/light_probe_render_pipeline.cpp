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
#include "light_probe_render_pipeline.h"

#include <ray/render_pipeline.h>
#include <ray/render_pipeline_framebuffer.h>
#include <ray/render_object_manager.h>

#include <ray/camera.h>
#include <ray/light_probe.h>

_NAME_BEGIN

__ImplementSubClass(LightProbeRenderPipeline, RenderPipelineController, "LightProbeRenderPipeline")

LightProbeRenderPipeline::LightProbeRenderPipeline() noexcept
{
}

LightProbeRenderPipeline::~LightProbeRenderPipeline() noexcept
{
	this->close();
}

bool
LightProbeRenderPipeline::setup(const RenderPipelinePtr& pipeline) noexcept
{
	assert(pipeline);
	_pipeline = pipeline;
	return true;
}

void
LightProbeRenderPipeline::close() noexcept
{
	_pipeline = nullptr;
}

void
LightProbeRenderPipeline::onRenderPipeline(const CameraPtr& mainCamera) noexcept
{
	assert(mainCamera);

	_pipeline->setCamera(mainCamera);

	const auto& lightProbes = mainCamera->getRenderDataManager()->getRenderData(RenderQueue::RenderQueueLightProbes);
	for (auto& it : lightProbes)
	{
		auto lightProbe = it->downcast<LightProbe>();
		lightProbe->onGenProbeBefore(*mainCamera);

		auto& camera = lightProbe->getCamera();
		if (camera)
		{
			camera->onRenderBefore(*camera);

			for (std::uint8_t i = 0; i < 6; i++)
			{
				_pipeline->setCamera(camera);
				_pipeline->setFramebuffer(camera->getRenderPipelineFramebuffer()->getFramebuffer());

				if (camera->getClearFlags() & CameraClearFlagBits::CameraClearColorBit)
					_pipeline->clearFramebuffer(0, CameraClearFlagBits::CameraClearColorBit, camera->getClearColor());

				if (camera->getClearFlags() & CameraClearFlagBits::CameraClearDepthBit ||
					camera->getClearFlags() & CameraClearFlagBits::CameraClearStencilBit)
				{
					if (camera->getClearFlags() & CameraClearFlagBits::CameraClearDepthStencilBit)
						_pipeline->clearFramebuffer(1, CameraClearFlagBits::CameraClearDepthStencilBit, camera->getClearColor());
					else if (camera->getClearFlags() & CameraClearFlagBits::CameraClearDepthBit)
						_pipeline->clearFramebuffer(1, CameraClearFlagBits::CameraClearDepthBit, camera->getClearColor());
					else if (camera->getClearFlags() & CameraClearFlagBits::CameraClearStencilBit)
						_pipeline->clearFramebuffer(1, CameraClearFlagBits::CameraClearStencilBit, camera->getClearColor());
				}

				_pipeline->drawRenderQueue(RenderQueue::RenderQueueOpaque);
			}

			camera->onRenderAfter(*camera);
		}

		lightProbe->onGenProbeAfter(*camera);
	}
}

void
LightProbeRenderPipeline::onRenderBefore() noexcept
{
}

void
LightProbeRenderPipeline::onRenderAfter() noexcept
{
}

_NAME_END