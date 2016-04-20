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
#include "shadow_render_pipeline.h"

#include <ray/render_pipeline.h>
#include <ray/camera.h>
#include <ray/light.h>
#include <ray/material.h>

#include <ray/graphics_texture.h>
#include <ray/graphics_framebuffer.h>

_NAME_BEGIN

__ImplementSubClass(ShadowRenderPipeline, RenderPipelineController, "ShadowRenderPipeline")

ShadowRenderPipeline::ShadowRenderPipeline() noexcept
{
}

ShadowRenderPipeline::~ShadowRenderPipeline() noexcept
{
	this->close();
}

bool
ShadowRenderPipeline::setup(RenderPipeline& pipeline) noexcept
{
	return true;
}

void
ShadowRenderPipeline::close() noexcept
{
}

void
ShadowRenderPipeline::renderShadowMap(RenderPipeline& pipeline) noexcept
{
	const auto lights = pipeline.getRenderData(RenderQueue::RenderQueueShadow);

	std::size_t numSoftLight = 0;
	for (auto& it : lights)
	{
		auto light = it->downcast<Light>();
		if (light->getSoftShadow())
			numSoftLight++;
	}

	if (_softShadowViews.size() < numSoftLight)
	{
		auto num = numSoftLight - _softShadowViews.size();
		for (std::size_t i = 0; i < num; i++)
		{
			GraphicsTextureDesc textureDesc;
			textureDesc.setWidth(_shadowMapSize);
			textureDesc.setHeight(_shadowMapSize);
			textureDesc.setTexDim(GraphicsTextureDim::GraphicsTextureDim2D);
			textureDesc.setTexFormat(_shadowFormat);
			auto blurShadowMap = pipeline.createTexture(textureDesc);
			if (!blurShadowMap)
				return;

			GraphicsFramebufferDesc shadowViewDesc;
			shadowViewDesc.setWidth(_shadowMapSize);
			shadowViewDesc.setHeight(_shadowMapSize);
			shadowViewDesc.attach(blurShadowMap);
			shadowViewDesc.setGraphicsFramebufferLayout(_softShadowViewLayout);
			auto blurShadowView = pipeline.createFramebuffer(shadowViewDesc);
			if (!blurShadowView)
				return;

			_softShadowMaps.push_back(blurShadowMap);
			_softShadowViews.push_back(blurShadowView);
		}
	}

	numSoftLight = 0;

	for (auto& it : lights)
	{
		auto light = it->downcast<Light>();
		pipeline.setCamera(light->getShadowCamera());
		pipeline.setFramebuffer(light->getShadowCamera()->getFramebuffer());
		pipeline.clearFramebuffer(GraphicsClearFlags::GraphicsClearFlagsAll, float4::Zero, 1.0, 0);
		pipeline.drawRenderQueue(RenderQueue::RenderQueueOpaque, _softGenShadowMap);

		if (light->getSoftShadow())
		{
			//_softBlurShadowSource->uniformTexture(light->getShadowMap());
			//_softBlurShadowSourceInv->uniform2f(float2(1.0f, 0.0f) / light->getShadowSize());
			_softClipConstant->uniform4f(float4(light->getShadowCamera()->getClipConstant().xy(), 1.0));
			pipeline.setFramebuffer(_softShadowViewTemp);
			pipeline.drawScreenQuad(_softBlurShadowX);

			_softBlurShadowSource->uniformTexture(_softShadowMapTemp);
			_softBlurShadowSourceInv->uniform2f(float2(0.0f, 1.0f) / _shadowMapSize);
			pipeline.setFramebuffer(_softShadowViews[numSoftLight]);
			pipeline.drawScreenQuad(_softBlurShadowY);

			numSoftLight++;
		}
	}
}

void
ShadowRenderPipeline::onResolutionChangeBefore(RenderPipeline& pipeline) noexcept
{
}

void
ShadowRenderPipeline::onResolutionChangeAfter(RenderPipeline& pipeline) noexcept
{
}

void
ShadowRenderPipeline::onRenderPipeline(RenderPipeline& pipeline, const CameraPtr& camera) noexcept
{
	if (!camera)
		return;

	auto cameraOrder = camera->getCameraOrder();
	switch (cameraOrder)
	{
	case CameraOrder::CameraOrderShadow:
		this->renderShadowMap(pipeline);
		break;
	default:
		assert(false);
		break;
	}
}

void
ShadowRenderPipeline::onRenderPre(RenderPipeline& pipeline) noexcept
{
}

void
ShadowRenderPipeline::onRenderPost(RenderPipeline& pipeline) noexcept
{
}

_NAME_END