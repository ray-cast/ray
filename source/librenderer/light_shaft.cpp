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
#include <ray/light_shaft.h>
#include <ray/light.h>
#include <ray/camera.h>

_NAME_BEGIN

LightShaft::LightShaft() noexcept
	: illuminationNumber(60)
	, illuminationWeight(1 / 60)
	, illuminationDecay(0.98)
{
}

LightShaft::~LightShaft() noexcept
{
}

void
LightShaft::onActivate(RenderPipeline& pipeline) except
{
	std::uint32_t width, height;
	pipeline.getWindowResolution(width, height);

	_material = pipeline.createMaterial("sys:fx/light_shaft.glsl");
	_texSample = pipeline.createRenderTexture(width*0.5, height*0.5, TextureDim::DIM_2D, TextureFormat::R11G11B10F);

	_lightShaft = _material->getTech(RenderQueue::RQ_POSTPROCESS)->getPass("LightScatter");
	_lightShaftCopy = _material->getTech(RenderQueue::RQ_POSTPROCESS)->getPass("LightScatterCopy");

	_illuminationSample = _material->getParameter("illuminationSample");
	_illuminationPosition = _material->getParameter("illuminationPosition");
	_illuminationSource = _material->getParameter("illuminationSource");
	_illuminationRadio = _material->getParameter("illuminationRadio");

	float4 sample;
	sample.x = illuminationNumber;
	sample.y = 1 / illuminationNumber;
	sample.z = illuminationWeight;
	sample.w = illuminationDecay;

	_illuminationSample->assign(sample);
}

void
LightShaft::onDeactivate(RenderPipeline& pipeline) except
{
}

void
LightShaft::onRender(RenderPipeline& pipeline, GraphicsRenderTexturePtr source, GraphicsRenderTexturePtr dest) except
{
	pipeline.setRenderTexture(_texSample);
	pipeline.clearRenderTexture(ClearFlags::All, Vector4::Zero, 1.0, 0);

	std::uint32_t width, height;
	pipeline.getWindowResolution(width, height);

	_illuminationRadio->assign((float)width / height);

	auto lights = pipeline.getRenderData(RenderQueue::RQ_LIGHTING, RenderPass::RP_LIGHTS);
	for (auto& it : lights)
	{
		auto light = std::dynamic_pointer_cast<Light>(it);

		if (light->getLightType() == LightType::LT_SUN)
		{
			auto sun = pipeline.getCamera()->getTranslate() + light->getTransform().getTranslate();
			auto view = pipeline.getCamera()->worldToProject(sun);
			view.x = view.x * 0.5 + 0.5;
			view.y = view.y * 0.5 + 0.5;

			if (view.x >= -0.5f && view.x <= 2.0f &&
				view.y >= -0.5f && view.y <= 2.0f && view.z < 1.0f)
			{
				_illuminationPosition->assign(Vector2(view.x, view.y));
				_illuminationSource->assign(source->getResolveTexture());

				pipeline.drawScreenQuad(_lightShaft);
			}
		}
	}

	_illuminationSource->assign(_texSample->getResolveTexture());

	pipeline.setRenderTexture(source);
	pipeline.drawScreenQuad(_lightShaftCopy);
}

_NAME_END