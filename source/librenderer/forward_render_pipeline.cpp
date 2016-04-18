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
ForwardRenderPipeline::setup(RenderPipeline& pipeline) noexcept
{
	return true;
}

void
ForwardRenderPipeline::close() noexcept
{
}

void
ForwardRenderPipeline::renderShadowMap(RenderPipeline& pipeline) noexcept
{
	pipeline.setFramebuffer(pipeline.getCamera()->getFramebuffer());
	pipeline.clearFramebuffer(GraphicsClearFlags::GraphicsClearFlagsDepth, float4::Zero, 1.0, 0);
	pipeline.drawRenderQueue(RenderQueue::RenderQueueOpaque, _depthOnly);
}

void
ForwardRenderPipeline::render2DEnvMap(RenderPipeline& pipeline) noexcept
{
	std::uint32_t width, height;
	pipeline.getWindowResolution(width, height);
	pipeline.setViewport(Viewport(0, 0, width, height, 0, 1.0));
	pipeline.clearFramebuffer(GraphicsClearFlags::GraphicsClearFlagsAll, pipeline.getCamera()->getClearColor(), 1.0, 0);
	pipeline.drawRenderQueue(RenderQueue::RenderQueueOpaque);
}

void
ForwardRenderPipeline::onResolutionChangeBefore(RenderPipeline& pipeline) noexcept
{
}

void
ForwardRenderPipeline::onResolutionChangeAfter(RenderPipeline& pipeline) noexcept
{
}

void
ForwardRenderPipeline::onRenderPipeline(RenderPipeline& pipeline, const CameraPtr& camera) noexcept
{
	if (!camera)
		return;

	auto cameraOrder = camera->getCameraOrder();
	switch (cameraOrder)
	{
	case CameraOrder::CameraOrderShadow:
		this->renderShadowMap(pipeline);
		break;
	case CameraOrder::CameraOrder2D:
		this->render2DEnvMap(pipeline);
		break;
	default:
		assert(false);
		break;
	}
}

void
ForwardRenderPipeline::onRenderPre(RenderPipeline& pipeline) noexcept
{
}

void
ForwardRenderPipeline::onRenderPost(RenderPipeline& pipeline) noexcept
{
}

_NAME_END