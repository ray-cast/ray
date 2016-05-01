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

SSSS::SSSS() noexcept
	: _sssScale(15.0f)
	, _sssStrength(1.0f)
	, _sssCorrection(1.0f)
{
}

SSSS::~SSSS() noexcept
{
}

bool
SSSS::setup(RenderPipeline& pipeline) noexcept
{
	std::uint32_t width, height;
	pipeline.getWindowResolution(width, height);

	_material = pipeline.createMaterial("sys:fx/ssss.fxml");
	if (!_material)
		return false;

	_sssTranslucency = _material->getTech("translucency");
	_sssGuassBlur = _material->getTech("blur");

	_sssFactor = _material->getParameter("blurFactor");
	_sssSource = _material->getParameter("texSource");

	_texMRT0 = _material->getParameter("texMRT0");
	_texMRT1 = _material->getParameter("texMRT1");
	_texDepthLinear = _material->getParameter("texDepthLinear");

	_lightColor = _material->getParameter("lightColor");
	_lightEyePosition = _material->getParameter("lightEyePosition");
	_lightEyeProjInfo = _material->getParameter("eyeProjInfo");

	_shadowMap = _material->getParameter("shadowMap");
	_shadowFactor = _material->getParameter("shadowFactor");
	_shadowEye2LightView = _material->getParameter("shadowEye2LightView");
	_shadowEye2LightViewProject = _material->getParameter("shadowEye2LightViewProject");

	return true;
}

void
SSSS::close() noexcept
{
}

void
SSSS::setScale(float scale) noexcept
{
	_sssScale = scale;
}

void
SSSS::setStrength(float strength) noexcept
{
	_sssStrength = strength;
}

void
SSSS::setCorrection(float correction) noexcept
{
	_sssCorrection = correction;
}

float
SSSS::getScale() const noexcept
{
	return _sssScale;
}

float
SSSS::getStrength() const noexcept
{
	return _sssStrength;
}

float
SSSS::getCorrection() const noexcept
{
	return _sssCorrection;
}

void
SSSS::applyTranslucency(RenderPipeline& pipeline, GraphicsFramebufferPtr source, LightPtr light, GraphicsTexturePtr linearDepth, GraphicsTexturePtr shaodwMap) noexcept
{
	assert(light && shaodwMap && source);
	assert(linearDepth);

	_texDepthLinear->uniformTexture(linearDepth);

	_lightColor->uniform3f(light->getLightColor() * light->getIntensity());
	_lightEyePosition->uniform3f(light->getTranslate() * pipeline.getCamera()->getView());
	_lightEyeProjInfo->uniform4f(pipeline.getCamera()->getProjConstant());

	_shadowMap->uniformTexture(shaodwMap);
	_shadowFactor->uniform4f(float4(light->getShadowCamera()->getClipConstant().xyz(), _sssScale));
	_shadowEye2LightView->uniform4fmat((pipeline.getCamera()->getViewInverse() * light->getShadowCamera()->getView()));
	_shadowEye2LightViewProject->uniform4fmat(pipeline.getCamera()->getViewInverse() * light->getShadowCamera()->getViewProject());

	pipeline.setFramebuffer(source);
	pipeline.drawScreenQuad(_sssTranslucency);
}

void
SSSS::applyGuassBlur(RenderPipeline& pipeline, GraphicsFramebufferPtr source, GraphicsTexturePtr MRT0, GraphicsTexturePtr MRT1, GraphicsTexturePtr linearDepth, GraphicsFramebufferPtr swap) noexcept
{
	std::uint32_t widght = source->getGraphicsFramebufferDesc().getWidth();
	std::uint32_t height = source->getGraphicsFramebufferDesc().getHeight();

	_texMRT0->uniformTexture(MRT0);
	_texMRT1->uniformTexture(MRT1);
	_texDepthLinear->uniformTexture(linearDepth);

	_sssSource->uniformTexture(source->getGraphicsFramebufferDesc().getTextures().front());
	_sssFactor->uniform3f(float3(float2(_sssStrength / widght, 0.0), _sssCorrection));

	pipeline.setFramebuffer(swap);
	pipeline.drawScreenQuad(_sssGuassBlur);

	_sssFactor->uniform3f(float3(float2(0.0, _sssStrength / height), _sssCorrection));

	pipeline.setFramebuffer(source);
	pipeline.drawScreenQuad(_sssGuassBlur);
}

_NAME_END