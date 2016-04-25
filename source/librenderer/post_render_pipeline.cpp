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
#include <ray/light.h>

_NAME_BEGIN

PostRenderPipeline::PostRenderPipeline() noexcept
{
}

PostRenderPipeline::~PostRenderPipeline() noexcept
{
}

bool
PostRenderPipeline::enableSSSS(RenderPipeline& pipeline, bool enable) noexcept
{
	if (!_SSSS)
	{
		_SSSS = std::make_shared<SSSS>();
		_SSSS->setup(pipeline);
	}

	return true;
}

bool
PostRenderPipeline::isEnableSSSS() const noexcept
{
	return _SSSS ? true : false;
}

void
PostRenderPipeline::render(RenderPipeline& pipeline) noexcept
{
	/*if (_SSSS)
	{
		std::size_t shadowIndex = 0;

		auto& lights = pipeline.getRenderData(RenderQueue::RenderQueueLighting);
		for (auto& it : lights)
		{
			auto light = it->downcast<Light>();
			if (light->getShadow() && light->getSubsurfaceScattering())
			{
				if (light->getSoftShadow())
					_SSSS->applyTranslucency(pipeline, _deferredShadingView, light, _deferredDepthLinearMap, _softShadowMaps[shadowIndex]);
				else
					_SSSS->applyTranslucency(pipeline, _deferredShadingView, light, _deferredDepthLinearMap, light->getShadowMap());
			}

			if (light->getSoftShadow())
				shadowIndex++;
		}

		_SSSS->applyGuassBlur(pipeline, _deferredShadingView, _deferredGraphicsMap, _deferredNormalMap, _deferredDepthLinearMap, _deferredSwapView);
	}*/
}

_NAME_END