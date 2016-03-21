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
#include <ray/ssss.h>
#include <ray/camera.h>
#include <ray/light.h>

#include <ray/graphics_view.h>
#include <ray/graphics_texture.h>

_NAME_BEGIN

SSSS::SSSS() noexcept
	: _sssStrength(0.6f)
	, _gaussianWidth(6.0f)
{
	this->setRenderQueue(RenderQueue::RenderQueuePostprocess);
}

SSSS::~SSSS() noexcept
{
}

void
SSSS::blurX(RenderPipeline& pipeline, GraphicsTexturePtr source, GraphicsRenderTexturePtr dest) noexcept
{
	std::uint32_t widght = source->getGraphicsTextureDesc().getWidth();

	_sssSource->assign(source);
	_sssStep->assign(float2(1.0 / widght, 0.0) * _sssStrength * _gaussianWidth);

	pipeline.setRenderTexture(dest);
	pipeline.discradRenderTexture();
	pipeline.drawScreenQuad(_blurX);
}

void 
SSSS::blurY(RenderPipeline& pipeline, GraphicsTexturePtr source, GraphicsRenderTexturePtr dest) noexcept
{
	std::uint32_t height = source->getGraphicsTextureDesc().getHeight();

	_sssSource->assign(source);
	_sssStep->assign(float2(0.0, 1.0 / height) * _sssStrength * _gaussianWidth);

	pipeline.setRenderTexture(dest);
	pipeline.discradRenderTexture();
	pipeline.drawScreenQuad(_blurY);
}

void
SSSS::translucency(RenderPipeline& pipeline, GraphicsRenderTexturePtr dest) noexcept
{
	pipeline.setRenderTexture(dest);

	auto lights = pipeline.getRenderData(RenderQueue::RenderQueueLighting, RenderPass::RenderPassLights);
	for (auto& it : lights)
	{
		auto light = it->downcast<Light>();
		if (light->getShadow())
		{
			_lightColor->assign(light->getLightColor() * light->getIntensity());
			_lightShadowMap->assign(light->getShadowMap());
			_lightShadowMatrix->assign(light->getShadowCamera()->getViewProject());
			_lightDirection->assign(-light->getForward() * float3x3(pipeline.getCamera()->getView()));

			if (light->getLightType() == LightType::LightTypeSun)
			{
				pipeline.drawScreenQuad(_translucency);
			}
		}
	}
}

void
SSSS::onActivate(RenderPipeline& pipeline) except
{
	_material = pipeline.createMaterial("sys:fx/ssss.fxml.o");

	_translucency = _material->getTech(RenderQueue::RenderQueuePostprocess)->getPass("translucency");
	_blurX = _material->getTech(RenderQueue::RenderQueuePostprocess)->getPass("blurX");
	_blurY = _material->getTech(RenderQueue::RenderQueuePostprocess)->getPass("blurY");

	std::uint32_t width, height;
	pipeline.getWindowResolution(width, height);

	_SSSSMap = pipeline.createTexture(width, height, GraphicsTextureDim::GraphicsTextureDim2D, GraphicsFormat::GraphicsFormatR8G8B8A8UNorm);

	float sssLevel = 0.125f * float(47) / (100 - 0);
	float sssStrength = 8.25 * (1.0 - 0.83) / sssLevel;

	_sssWidth = _material->getParameter("sssWidth");
	_sssStep = _material->getParameter("sssStep");
	_sssCorrection = _material->getParameter("sssCorrection");
	_sssSource = _material->getParameter("texSource");

	_lightColor = _material->getParameter("lightColor");
	_lightDirection = _material->getParameter("lightDirection");
	_lightShadowMap = _material->getParameter("lightShadowMap");
	_lightShadowMatrix = _material->getParameter("lightShadowMatrix");

	_sssWidth->assign(sssStrength);
	_sssCorrection->assign(sssLevel);
}

void
SSSS::onDeactivate(RenderPipeline& pipeline) except
{
}

void
SSSS::onRender(RenderPipeline& pipeline, GraphicsTexturePtr source, GraphicsRenderTexturePtr dest) noexcept
{
	this->blurX(pipeline, source, _SSSSView);
	this->blurY(pipeline, _SSSSMap, dest);

	this->translucency(pipeline, dest);
}

_NAME_END