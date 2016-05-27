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
#include "post_render_pipeline.h"
#include "ssss.h"

#include <ray/render_pipeline.h>
#include <ray/render_object_manager.h>

#include <ray/light.h>
#include <ray/camera.h>
#include <ray/material.h>

_NAME_BEGIN

PostRenderPipeline::PostRenderPipeline() noexcept
	: _enableSSSS(false)
{
}

PostRenderPipeline::~PostRenderPipeline() noexcept
{
}

bool
PostRenderPipeline::enableSSSS(bool enable) noexcept
{
	if (_enableSSSS != enable)
	{
		if (enable)
		{
			_SSSS = std::make_shared<SSSS>();
			_SSSS->setup(*this->getRenderPipeline());
		}
		else
		{
			_SSSS.reset();
		}

		_enableSSSS = enable;
	}

	return true;
}

bool
PostRenderPipeline::isEnableSSSS() const noexcept
{
	return _SSSS ? true : false;
}

void 
PostRenderPipeline::onActivate(RenderPipeline& pipeline) noexcept
{
	if (_enableSSSS)
	{
		_SSSS = std::make_shared<SSSS>();
		_SSSS->setup(pipeline);
	}
}

void 
PostRenderPipeline::onDeactivate(RenderPipeline& pipeline) noexcept
{
}

void 
PostRenderPipeline::onResolutionChangeBefore(RenderPipeline& pipeline) noexcept
{
}

void 
PostRenderPipeline::onResolutionChangeAfter(RenderPipeline& pipeline) noexcept
{
}

void 
PostRenderPipeline::onRenderPre(RenderPipeline& pipeline) noexcept
{
}

void 
PostRenderPipeline::onRenderPost(RenderPipeline& pipeline) noexcept
{
}

bool 
PostRenderPipeline::onRender(RenderPipeline& pipeline, RenderQueue queue, GraphicsFramebufferPtr& source, GraphicsFramebufferPtr swap) noexcept
{
	if (queue != RenderQueue::RenderQueuePostprocess)
		return false;

	_depthMap = pipeline.getSemanticParam(GlobalSemanticType::GlobalSemanticTypeDepthMap)->getTexture();
	_depthLinearMap = pipeline.getSemanticParam(GlobalSemanticType::GlobalSemanticTypeDepthLinearMap)->getTexture();
	_diffuseMap = pipeline.getSemanticParam(GlobalSemanticType::GlobalSemanticTypeDiffuseMap)->getTexture();
	_normalMap = pipeline.getSemanticParam(GlobalSemanticType::GlobalSemanticTypeNormalMap)->getTexture();
	_lightMap = pipeline.getSemanticParam(GlobalSemanticType::GlobalSemanticTypeLightingMap)->getTexture();

	if (_SSSS)
	{
		std::size_t shadowIndex = 0;

		auto& lights = pipeline.getCamera()->getRenderDataManager()->getRenderData(RenderQueue::RenderQueueLighting);
		for (auto& it : lights)
		{
			auto light = it->downcast<Light>();
			if (light->getShadowType() != LightShadowType::LightShadowTypeNone && light->getSubsurfaceScattering())
			{
				_SSSS->applyTranslucency(pipeline, source, _diffuseMap, _normalMap, *light, _depthLinearMap, light->getShadowMap());
			}

			if (light->getSoftShadow())
				shadowIndex++;
		}

		_SSSS->applyGuassBlur(pipeline, source, _depthLinearMap, swap);
	}

	return false;
}

_NAME_END