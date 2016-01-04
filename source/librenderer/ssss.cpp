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
#include <ray/ssss.h>
#include <ray/camera.h>
#include <ray/light.h>

_NAME_BEGIN

SSSS::SSSS() noexcept
{
	this->setRenderQueue(RenderQueue::RQ_OPAQUE);
}

SSSS::~SSSS() noexcept
{
}

void
SSSS::onActivate(RenderPipeline& pipeline) except
{
	_material = pipeline.createMaterial("sys:fx/ssss.glsl");

	_translucency = _material->getTech(RenderQueue::RQ_POSTPROCESS)->getPass("translucency");
	_blurX = _material->getTech(RenderQueue::RQ_POSTPROCESS)->getPass("blurX");
	_blurY = _material->getTech(RenderQueue::RQ_POSTPROCESS)->getPass("blurY");

	std::uint32_t width, height;
	pipeline.getWindowResolution(width, height);

	_SSSS = pipeline.createRenderTexture(width, height, TextureDim::DIM_2D, TextureFormat::R16G16B16F);

	float sssLevel = 0.025f * float(47) / (100 - 0);
	float sssProject = 1.0 / tan(0.5 * radians(20.0)) / 3.0;

	_correction = _material->getParameter("correction");
	_strength = _material->getParameter("strength");
	_sssWidth = _material->getParameter("sssWidth");
	_sssColor = _material->getParameter("texColor");

	_clipInfo = _material->getParameter("clipInfo");

	_lightFarPlane = _material->getParameter("lightFarPlane");
	_lightDirection = _material->getParameter("lightDirection");

	_lightShadowMap = _material->getParameter("lightShadowMap");
	_lightShadowMatrix = _material->getParameter("lightShadowMatrix");

	_correction->assign(sssLevel);
	_strength->assign(sssLevel * sssProject);
	_sssWidth->assign(sssLevel);
}

void
SSSS::onDeactivate(RenderPipeline& pipeline) except
{
}

void
SSSS::onRender(RenderPipeline& pipeline, GraphicsRenderTexturePtr source, GraphicsRenderTexturePtr dest) except
{
	pipeline.setRenderTexture(source);

	/*auto lights = pipeline.getRenderData(RenderQueue::RQ_LIGHTING, RenderPass::RP_LIGHTS);
	for (auto& it : lights)
	{
		auto light = std::dynamic_pointer_cast<Light>(it);

		if (light->getShadowMap())
		{
			auto lightDirection = float3x3(pipeline.getCamera()->getView()) * ~(light->getTransform().getTranslate() - light->getLightLookat());

			_lightDirection->assign(lightDirection);
			_lightFarPlane->assign(light->getShadowCamera()->getFar());
			_lightShadowMap->assign(light->getShadowCamera()->getRenderTexture()->getResolveTexture());
			_lightShadowMatrix->assign(light->getShadowCamera()->getViewProject());

			if (light->getLightType() == LightType::LT_SUN)
			{
				pipeline.drawScreenQuad(_translucency);
			}
		}
	}*/

	_sssColor->assign(source->getResolveTexture());

	pipeline.setRenderTexture(_SSSS);
	pipeline.clearRenderTexture(ClearFlags::Default, Vector4::Zero, 1.0, 0);
	pipeline.drawScreenQuad(_blurX);

	_sssColor->assign(_SSSS->getResolveTexture());

	pipeline.setRenderTexture(source);
	pipeline.drawScreenQuad(_blurY);
}

_NAME_END