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
#include "forward_render_pipeline.h"

#include <ray/render_pipeline.h>
#include <ray/camera.h>

_NAME_BEGIN

__ImplementSubClass(ForwardRenderPipeline, RenderPipelineController, "ForwardRenderPipeline")

ForwardRenderPipeline::ForwardRenderPipeline() noexcept
{
}

ForwardRenderPipeline::~ForwardRenderPipeline() noexcept
{
	this->close();
}

bool
ForwardRenderPipeline::setup(RenderPipelinePtr pipeline) noexcept
{
	assert(pipeline);
	_pipeline = pipeline;
	return true;
}

void
ForwardRenderPipeline::close() noexcept
{
}

void
ForwardRenderPipeline::render2DEnvMap(RenderPipeline& pipeline) noexcept
{
	assert(pipeline.getCamera());

	pipeline.setFramebuffer(pipeline.getCamera()->getFramebuffer());

	if (pipeline.getCamera()->getClearFlags() & GraphicsClearFlagBits::GraphicsClearFlagColorBit)
		pipeline.clearFramebuffer(0, GraphicsClearFlagBits::GraphicsClearFlagColorBit, pipeline.getCamera()->getClearColor(), 1.0, 0);

	if (pipeline.getCamera()->getClearFlags() & GraphicsClearFlagBits::GraphicsClearFlagDepthBit ||
		pipeline.getCamera()->getClearFlags() & GraphicsClearFlagBits::GraphicsClearFlagStencilBit)
	{
		if (pipeline.getCamera()->getClearFlags() & GraphicsClearFlagBits::GraphicsClearFlagDepthStencilBit)
			pipeline.clearFramebuffer(1, GraphicsClearFlagBits::GraphicsClearFlagDepthStencilBit, pipeline.getCamera()->getClearColor(), 1.0, 0);
		else if (pipeline.getCamera()->getClearFlags() & GraphicsClearFlagBits::GraphicsClearFlagDepthBit)
			pipeline.clearFramebuffer(1, GraphicsClearFlagBits::GraphicsClearFlagDepthBit, pipeline.getCamera()->getClearColor(), 1.0, 0);
		else if (pipeline.getCamera()->getClearFlags() & GraphicsClearFlagBits::GraphicsClearFlagStencilBit)
			pipeline.clearFramebuffer(1, GraphicsClearFlagBits::GraphicsClearFlagStencilBit, pipeline.getCamera()->getClearColor(), 1.0, 0);
	}
		
	pipeline.drawRenderQueue(RenderQueue::RenderQueueOpaque);
}

void
ForwardRenderPipeline::onResolutionChange() noexcept
{
}

void
ForwardRenderPipeline::onRenderPipeline(const CameraPtr& camera) noexcept
{
	assert(camera);
	assert(camera->getCameraOrder() == CameraOrder::CameraOrder2D);
	_pipeline->setCamera(camera);
	this->render2DEnvMap(*_pipeline);
}

void
ForwardRenderPipeline::onRenderPre() noexcept
{
}

void
ForwardRenderPipeline::onRenderPost() noexcept
{
}

_NAME_END