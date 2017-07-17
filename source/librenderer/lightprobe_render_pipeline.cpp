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
#include "lightprobe_render_pipeline.h"

#include <ray/render_pipeline.h>
#include <ray/render_pipeline_framebuffer.h>
#include <ray/camera.h>

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
LightProbeRenderPipeline::renderLightProbes(RenderPipeline& pipeline) noexcept
{
	assert(pipeline.getCamera());

	auto& v = pipeline.getCamera()->getPixelViewportDPI();

	pipeline.setFramebuffer(pipeline.getCamera()->getRenderPipelineFramebuffer()->getFramebuffer());
	pipeline.setViewport(0, Viewport(v.x, v.y, v.z, v.w));

	if (pipeline.getCamera()->getClearFlags() & CameraClearFlagBits::CameraClearColorBit)
		pipeline.clearFramebuffer(0, CameraClearFlagBits::CameraClearColorBit, pipeline.getCamera()->getClearColor(), 1.0, 0);

	if (pipeline.getCamera()->getClearFlags() & CameraClearFlagBits::CameraClearDepthBit ||
		pipeline.getCamera()->getClearFlags() & CameraClearFlagBits::CameraClearStencilBit)
	{
		if (pipeline.getCamera()->getClearFlags() & CameraClearFlagBits::CameraClearDepthStencilBit)
			pipeline.clearFramebuffer(1, CameraClearFlagBits::CameraClearDepthStencilBit, pipeline.getCamera()->getClearColor(), 1.0, 0);
		else if (pipeline.getCamera()->getClearFlags() & CameraClearFlagBits::CameraClearDepthBit)
			pipeline.clearFramebuffer(1, CameraClearFlagBits::CameraClearDepthBit, pipeline.getCamera()->getClearColor(), 1.0, 0);
		else if (pipeline.getCamera()->getClearFlags() & CameraClearFlagBits::CameraClearStencilBit)
			pipeline.clearFramebuffer(1, CameraClearFlagBits::CameraClearStencilBit, pipeline.getCamera()->getClearColor(), 1.0, 0);
	}

	pipeline.drawRenderQueue(RenderQueue::RenderQueueOpaque);
}

void
LightProbeRenderPipeline::onRenderPipeline(const CameraPtr& camera) noexcept
{
	assert(camera);
	assert(camera->getCameraOrder() == CameraOrder::CameraOrder2D);
	_pipeline->setCamera(camera);
	this->renderLightProbes(*_pipeline);
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