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
#include "light_shaft.h"
#include <ray/light.h>
#include <ray/camera.h>
#include <ray/material.h>
#include <ray/graphics_framebuffer.h>
#include <ray/graphics_texture.h>
#include <ray/render_pipeline.h>

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
LightShaft::onActivate(RenderPipeline& pipeline) noexcept
{
	std::uint32_t width, height;
	pipeline.getWindowResolution(width, height);

	_material = pipeline.createMaterial("sys:fx/light_shaft.glsl");

	_lightShaft = _material->getTech("LightScatter");
	_lightShaftCopy = _material->getTech("LightScatterCopy");

	_illuminationSample = _material->getParameter("illuminationSample");
	_illuminationPosition = _material->getParameter("illuminationPosition");
	_illuminationSource = _material->getParameter("illuminationSource");
	_illuminationRadio = _material->getParameter("illuminationRadio");

	float4 sample;
	sample.x = illuminationNumber;
	sample.y = 1 / illuminationNumber;
	sample.z = illuminationWeight;
	sample.w = illuminationDecay;

	_illuminationSample->uniform4f(sample);

	_sampleMap = pipeline.createTexture(width / 2, height / 2, GraphicsTextureDim::GraphicsTextureDim2D, GraphicsFormat::GraphicsFormatB10G11R11UFloatPack32);

	GraphicsFramebufferDesc sampleViewDesc;
	sampleViewDesc.attach(_sampleMap);
	_sampleView = pipeline.createFramebuffer(sampleViewDesc);
}

void
LightShaft::onDeactivate(RenderPipeline& pipeline) noexcept
{
	_material.reset();
	_lightShaft.reset();
	_lightShaftCopy.reset();
	_illuminationPosition.reset();
	_illuminationSample.reset();
	_illuminationSource.reset();
	_illuminationRadio.reset();
	_sampleMap.reset();
	_sampleView.reset();
}

bool
LightShaft::onRender(RenderPipeline& pipeline, GraphicsFramebufferPtr source, GraphicsFramebufferPtr dest) noexcept
{
	auto texture = source->getGraphicsFramebufferDesc().getTextures().front();

	pipeline.setFramebuffer(_sampleView);
	pipeline.clearFramebuffer(GraphicsClearFlags::GraphicsClearFlagsAll, float4::Zero, 1.0, 0);

	std::uint32_t width, height;
	pipeline.getWindowResolution(width, height);

	_illuminationRadio->uniform1f((float)width / height);

	auto lights = pipeline.getRenderData(RenderQueue::RenderQueueLighting);
	for (auto& it : lights)
	{
		auto light = std::dynamic_pointer_cast<Light>(it);

		if (light->getLightType() == LightType::LightTypeSun)
		{
			auto sun = pipeline.getCamera()->getTranslate() + light->getTransform().getTranslate();
			auto view = pipeline.getCamera()->worldToProject(sun);
			view.x = view.x * 0.5 + 0.5;
			view.y = view.y * 0.5 + 0.5;

			if (view.x >= -0.5f && view.x <= 2.0f &&
				view.y >= -0.5f && view.y <= 2.0f && view.z < 1.0f)
			{
				_illuminationPosition->uniform2f(Vector2(view.x, view.y));
				_illuminationSource->uniformTexture(texture);

				pipeline.drawScreenQuad(_lightShaft);
			}
		}
	}

	_illuminationSource->uniformTexture(_sampleMap);

	pipeline.setFramebuffer(dest);
	pipeline.drawScreenQuad(_lightShaftCopy);

	return true;
}

_NAME_END