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
#include "ssss.h"
#include <ray/camera.h>
#include <ray/light.h>
#include <ray/material.h>

#include <ray/graphics_framebuffer.h>
#include <ray/graphics_texture.h>
#include <ray/render_pipeline.h>

_NAME_BEGIN

__ImplementSubClass(SSSS, RenderPostProcess, "SSSS")

SSSS::SSSS() noexcept
	: _sssScale(100.0f)
	, _sssStrength(0.5f)
	, _sssWidth(0.0125f)
	, _gaussianWidth(6.0f)
{
	this->setRenderQueue(RenderQueue::RenderQueuePostprocess);
}

SSSS::~SSSS() noexcept
{
}

void
SSSS::blurX(RenderPipeline& pipeline, GraphicsTexturePtr source, GraphicsFramebufferPtr dest) noexcept
{
	std::uint32_t widght = source->getGraphicsTextureDesc().getWidth();

	_sssSource->assign(source);
	_sssStep->assign(float2(1.0 / widght, 0.0) * _sssStrength * _gaussianWidth);

	pipeline.setFramebuffer(dest);
	pipeline.drawScreenQuad(_blur);
}

void 
SSSS::blurY(RenderPipeline& pipeline, GraphicsTexturePtr source, GraphicsFramebufferPtr dest) noexcept
{
	std::uint32_t height = source->getGraphicsTextureDesc().getHeight();

	_sssSource->assign(source);
	_sssStep->assign(float2(0.0, 1.0 / height) * _sssStrength * _gaussianWidth);

	pipeline.setFramebuffer(dest);
	pipeline.drawScreenQuad(_blur);
}

void
SSSS::translucency(RenderPipeline& pipeline, LightPtr light, GraphicsTexturePtr shaodwMap, GraphicsFramebufferPtr dest) noexcept
{
	assert(light && shaodwMap && dest);
	assert(light->getSubsurfaceScattering());

	float3 clipConstant = light->getShadowCamera()->getClipConstant().xyz();
	float shadowFactor = _sssScale / (light->getShadowCamera()->getFar() - light->getShadowCamera()->getNear());

	_eyeProjInfo->assign(pipeline.getCamera()->getProjConstant());

	_lightColor->assign(light->getLightColor() * light->getIntensity());
	_lightEyePosition->assign(light->getTranslate() * pipeline.getCamera()->getView());

	_shadowMap->assign(shaodwMap);
	_shadowFactor->assign(float4(clipConstant, shadowFactor));
	_shadowEye2LightView->assign((pipeline.getCamera()->getViewInverse() * light->getShadowCamera()->getView().getAxisZ()));
	_shadowEye2LightViewProject->assign(pipeline.getCamera()->getViewInverse() * light->getShadowCamera()->getViewProject());

	pipeline.setFramebuffer(dest);
	pipeline.drawScreenQuad(_translucency);
}

void
SSSS::translucency(RenderPipeline& pipeline, GraphicsFramebufferPtr dest) noexcept
{
	pipeline.setFramebuffer(dest);

	auto lights = pipeline.getRenderData(RenderQueue::RenderQueueLighting);
	for (auto& it : lights)
	{
		auto light = it->downcast<Light>();
		if (light->getShadow() && light->getSubsurfaceScattering())
		{
			this->translucency(pipeline, light, light->getShadowMap(), dest);
		}
	}
}

void
SSSS::onActivate(RenderPipeline& pipeline) noexcept
{
	_material = pipeline.createMaterial("sys:fx/ssss.fxml.o");

	_translucency = _material->getTech("translucency");
	_blur = _material->getTech("blur");

	std::uint32_t width, height;
	pipeline.getWindowResolution(width, height);

	_SSSSMap = pipeline.createTexture(width, height, GraphicsTextureDim::GraphicsTextureDim2D, GraphicsFormat::GraphicsFormatR8G8B8UNorm);

	GraphicsFramebufferLayoutDesc framebufferLayoutDesc;
	framebufferLayoutDesc.addComponent(GraphicsAttachmentDesc(GraphicsViewLayout::GraphicsViewLayoutColorAttachmentOptimal, GraphicsFormat::GraphicsFormatR8G8B8UNorm, 0));
	_SSSSViewLayout = pipeline.createFramebufferLayout(framebufferLayoutDesc);

	GraphicsFramebufferDesc framebufferDesc;
	framebufferDesc.setWidth(width);
	framebufferDesc.setHeight(height);
	framebufferDesc.attach(_SSSSMap);
	framebufferDesc.setGraphicsFramebufferLayout(_SSSSViewLayout);
	_SSSSView = pipeline.createFramebuffer(framebufferDesc);

	_sssStep = _material->getParameter("sssStep");
	_sssCorrection = _material->getParameter("sssCorrection");
	_sssSource = _material->getParameter("texSource");

	_texDepthLinear = _material->getParameter("texDepthLinear");
	_eyeProjInfo = _material->getParameter("eyeProjInfo");

	_lightColor = _material->getParameter("lightColor");
	_lightEyePosition = _material->getParameter("lightEyePosition");

	_shadowMap = _material->getParameter("shadowMap");
	_shadowFactor = _material->getParameter("shadowFactor");
	_shadowEye2LightView = _material->getParameter("shadowEye2LightView");
	_shadowEye2LightViewProject = _material->getParameter("shadowEye2LightViewProject");

	_sssCorrection->assign(_sssWidth);
}

void
SSSS::onDeactivate(RenderPipeline& pipeline) noexcept
{
}

bool
SSSS::onRender(RenderPipeline& pipeline, GraphicsFramebufferPtr source, GraphicsFramebufferPtr dest) noexcept
{
	this->translucency(pipeline, source);
	this->blurX(pipeline, source->getGraphicsFramebufferDesc().getTextures().front(), _SSSSView);
	this->blurY(pipeline, _SSSSMap, dest);
	return true;
}

_NAME_END