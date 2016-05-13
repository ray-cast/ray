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
#include "ssao.h"
#include <ray/camera.h>
#include <ray/material.h>
#include <ray/graphics_framebuffer.h>
#include <ray/graphics_texture.h>
#include <ray/render_pipeline.h>

_NAME_BEGIN

SSAO::Setting::Setting() noexcept
	: radius(1.0f)
	, bias(0.002f)
	, intensity(2)
	, blur(true)
	, blurRadius(6)
	, blurScale(1.5f)
	, blurSharpness(4)
{
}

SSAO::SSAO() noexcept
{
	this->setRenderQueue(RenderQueue::RenderQueueOpaqueSpecific);
}

SSAO::~SSAO() noexcept
{
}

void
SSAO::setSetting(const Setting& setting) noexcept
{
	const float blurSigma = _setting.blurRadius * 0.5f;
	const float blurFalloff = 1.0f / (2.0f * blurSigma * blurSigma);

	_blurFactor->uniform1f(blurFalloff);
	_blurSharpness->uniform1f(_setting.blurSharpness);

	_occlusionRadius->uniform1f(_setting.radius);
	_occlusionRadius2->uniform1f(_setting.radius * _setting.radius);
	_occlusionBias->uniform1f(_setting.bias);
	_occlusionIntensity->uniform1f(_setting.intensity);

	_setting = setting;
}

const SSAO::Setting&
SSAO::getSetting() const noexcept
{
	return _setting;
}

void
SSAO::computeRawAO(RenderPipeline& pipeline, GraphicsTexturePtr source, GraphicsFramebufferPtr dest) noexcept
{
	std::uint32_t width, height;
	pipeline.getWindowResolution(width, height);

	_cameraProjInfo->uniform4f(pipeline.getCamera()->getProjConstant());
	_cameraProjScale->uniform1f(((float)width / height) * _setting.radius);

	pipeline.setFramebuffer(dest);
	pipeline.discradRenderTexture();
	pipeline.drawScreenQuad(_ambientOcclusionPass);
}

void
SSAO::blurHorizontal(RenderPipeline& pipeline, GraphicsTexturePtr source, GraphicsFramebufferPtr dest) noexcept
{
	GraphicsTextureDesc textureDesc = source->getGraphicsTextureDesc();

	float2 direction(_setting.blurScale, 0.0f);
	direction.x /= textureDesc.getWidth();

	_blurDirection->uniform2f(direction);
	_blurSource->uniformTexture(source);

	pipeline.setFramebuffer(dest);
	pipeline.discradRenderTexture();
	pipeline.drawScreenQuad(_ambientOcclusionBlurXPass);
}

void
SSAO::blurVertical(RenderPipeline& pipeline, GraphicsTexturePtr source, GraphicsFramebufferPtr dest) noexcept
{
	GraphicsTextureDesc textureDesc = source->getGraphicsTextureDesc();

	float2 direction(0.0f, _setting.blurScale);
	direction.y /= textureDesc.getHeight();

	_blurDirection->uniform2f(direction);
	_blurSource->uniformTexture(source);

	pipeline.setFramebuffer(dest);
	pipeline.discradRenderTexture();
	pipeline.drawScreenQuad(_ambientOcclusionBlurYPass);
}

void
SSAO::createSphereNoise() noexcept
{
	std::vector<float2> sphere;
	std::size_t numSample = _occlusionSampleNumber->getInt();

	for (std::size_t i = 0; i < numSample; i++)
	{
		float sampleAlpha = (i + 0.5) * (1.0 / numSample);
		float angle = sampleAlpha * M_TWO_PI * 7;

		float2 rotate;
		math::sinCos(&rotate.y, &rotate.x, angle);

		sphere.push_back(rotate);
	}

	_occlusionSphere->uniform2fv(sphere);
}

void
SSAO::onActivate(RenderPipeline& pipeline) noexcept
{
	std::uint32_t width, height;
	pipeline.getWindowResolution(width, height);

	width *= 0.5;
	height *= 0.5;

	_texAmbientMap = pipeline.createTexture(width, height, GraphicsTextureDim::GraphicsTextureDim2D, GraphicsFormat::GraphicsFormatR8UNorm);
	_texBlurMap = pipeline.createTexture(width, height, GraphicsTextureDim::GraphicsTextureDim2D, GraphicsFormat::GraphicsFormatR8UNorm);

	GraphicsFramebufferLayoutDesc framebufferLayoutDesc;
	framebufferLayoutDesc.addComponent(GraphicsAttachmentDesc(GraphicsViewLayout::GraphicsViewLayoutColorAttachmentOptimal, GraphicsFormat::GraphicsFormatR8UNorm, 0));
	_framebufferLayout = pipeline.createFramebufferLayout(framebufferLayoutDesc);

	GraphicsFramebufferDesc ambientViewDesc;
	ambientViewDesc.setWidth(width);
	ambientViewDesc.setHeight(height);
	ambientViewDesc.attach(_texAmbientMap);
	ambientViewDesc.setGraphicsFramebufferLayout(_framebufferLayout);
	_texAmbientView = pipeline.createFramebuffer(ambientViewDesc);

	GraphicsFramebufferDesc blurViewDesc;
	blurViewDesc.setWidth(width);
	blurViewDesc.setHeight(height);
	blurViewDesc.attach(_texBlurMap);
	blurViewDesc.setGraphicsFramebufferLayout(_framebufferLayout);
	_texBlurView = pipeline.createFramebuffer(blurViewDesc);

	_ambientOcclusion = pipeline.createMaterial("sys:fx\\ssao.fxml");
	_ambientOcclusionPass = _ambientOcclusion->getTech("ComputeAO");
	_ambientOcclusionBlurXPass = _ambientOcclusion->getTech("BlurXAO");
	_ambientOcclusionBlurYPass = _ambientOcclusion->getTech("BlurYAO");

	_cameraProjScale = _ambientOcclusion->getParameter("projScale");
	_cameraProjInfo = _ambientOcclusion->getParameter("projInfo");

	_occlusionRadius = _ambientOcclusion->getParameter("radius");
	_occlusionRadius2 = _ambientOcclusion->getParameter("radius2");
	_occlusionBias = _ambientOcclusion->getParameter("bias");
	_occlusionIntensity = _ambientOcclusion->getParameter("intensity");
	_occlusionAmbient = _ambientOcclusion->getParameter("texOcclusion");
	_occlusionSphere = _ambientOcclusion->getParameter("sphere");
	_occlusionSampleNumber = _ambientOcclusion->getMacro("NUM_SAMPLE");

	_blurSource = _ambientOcclusion->getParameter("texSource");
	_blurFactor = _ambientOcclusion->getParameter("blurFactor");
	_blurSharpness = _ambientOcclusion->getParameter("blurSharpness");
	_blurDirection = _ambientOcclusion->getParameter("blurDirection");
	_blurGaussian = _ambientOcclusion->getParameter("blurGaussian");
	_blurRadius = _ambientOcclusion->getMacro("BLUR_RADIUS");

	_setting.blurRadius = _blurRadius->getInt();

	this->createSphereNoise();

	this->setSetting(_setting);
}

void
SSAO::onDeactivate(RenderPipeline& pipeline) noexcept
{
}

bool
SSAO::onRender(RenderPipeline& pipeline, GraphicsFramebufferPtr source, GraphicsFramebufferPtr dest) noexcept
{
	auto texture = source->getGraphicsFramebufferDesc().getTextures().front();

	this->computeRawAO(pipeline, texture, _texAmbientView);
	this->blurHorizontal(pipeline, _texAmbientMap, _texBlurView);
	this->blurVertical(pipeline, _texBlurMap, dest);
	return true;
}

_NAME_END