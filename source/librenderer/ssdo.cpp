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
#include "ssdo.h"
#include <ray/camera.h>
#include <ray/material.h>
#include <ray/graphics_framebuffer.h>
#include <ray/graphics_texture.h>
#include <ray/render_pipeline.h>

_NAME_BEGIN

SSDO::Setting::Setting() noexcept
	: radius(10.0f)
	, bias(0.002f)
	, intensity(1)
	, blur(true)
	, blurRadius(6)
	, blurScale(1.0f)
	, blurSharpness(4)
{
}

SSDO::SSDO() noexcept
{
}

SSDO::~SSDO() noexcept
{
}

void
SSDO::setSetting(const Setting& setting) noexcept
{
	const float blurSigma = _setting.blurRadius * 0.5f;
	const float blurFalloff = 1.0f / (2.0f * blurSigma * blurSigma);

	_blurFactor->uniform1f(blurFalloff);
	_blurSharpness->uniform1f(_setting.blurSharpness);

	_occlusionRadius->uniform1f(_setting.radius);
	_occlusionBias->uniform1f(_setting.bias);
	_occlusionIntensity->uniform1f(_setting.intensity);

	_setting = setting;
}

const SSDO::Setting&
SSDO::getSetting() const noexcept
{
	return _setting;
}

void
SSDO::computeRawAO(RenderPipeline& pipeline, GraphicsTexturePtr source, GraphicsFramebufferPtr dest) noexcept
{
	std::uint32_t width, height;
	pipeline.getWindowResolution(width, height);

	_occlusionSourceInv->uniform2f(1.0f / width, 1.0f / height);
	_cameraProjScale->uniform1f(pipeline.getCamera()->getFar());

	pipeline.setFramebuffer(dest);
	pipeline.discardFramebuffer(0);
	pipeline.drawScreenQuad(*_ambientOcclusionPass);
}

void
SSDO::blurHorizontal(RenderPipeline& pipeline, GraphicsTexturePtr source, GraphicsFramebufferPtr dest) noexcept
{
	GraphicsTextureDesc textureDesc = source->getGraphicsTextureDesc();

	float2 direction(_setting.blurScale, 0.0f);
	direction.x /= textureDesc.getWidth();

	_blurDirection->uniform2f(direction);
	_blurSource->uniformTexture(source);

	pipeline.setFramebuffer(dest);
	pipeline.discardFramebuffer(0);
	pipeline.drawScreenQuad(*_ambientOcclusionBlurXPass);
}

void
SSDO::blurVertical(RenderPipeline& pipeline, GraphicsTexturePtr source, GraphicsFramebufferPtr dest) noexcept
{
	GraphicsTextureDesc textureDesc = source->getGraphicsTextureDesc();

	float2 direction(0.0f, _setting.blurScale);
	direction.y /= textureDesc.getHeight();

	_blurDirection->uniform2f(direction);
	_blurSource->uniformTexture(source);

	pipeline.setFramebuffer(dest);
	pipeline.discardFramebuffer(0);
	pipeline.drawScreenQuad(*_ambientOcclusionBlurYPass);
}

void
SSDO::applySSDO(RenderPipeline& pipeline, GraphicsTexturePtr source, GraphicsFramebufferPtr dest) noexcept
{
	_blurSource->uniformTexture(source);

	pipeline.setFramebuffer(dest);
	pipeline.drawScreenQuad(*_ambientOcclusionApply);
}

void
SSDO::createSphereNoise() noexcept
{
	std::vector<float2> sphere;
	std::size_t numSample = _occlusionSampleNumber->getInt();

	for (std::size_t i = 0; i < numSample; i++)
	{
		float sampleAlpha = (i + 0.5f) * (1.0f / numSample);
		float angle = sampleAlpha * M_TWO_PI * 7.0f;

		float2 rotate;
		math::sinCos(&rotate.y, &rotate.x, angle);

		sphere.push_back(rotate);
	}

	_occlusionSphere->uniform2fv(sphere);
}

void
SSDO::onActivate(RenderPipeline& pipeline) noexcept
{
	std::uint32_t width, height;
	pipeline.getWindowResolution(width, height);

	_texAmbientMap = pipeline.createTexture(width, height, GraphicsTextureDim::GraphicsTextureDim2D, GraphicsFormat::GraphicsFormatR8UNorm);
	_texBlurMap = pipeline.createTexture(width, height, GraphicsTextureDim::GraphicsTextureDim2D, GraphicsFormat::GraphicsFormatR8UNorm);

	GraphicsFramebufferLayoutDesc framebufferLayoutDesc;
	framebufferLayoutDesc.addComponent(GraphicsAttachmentLayout(0, GraphicsImageLayout::GraphicsImageLayoutColorAttachmentOptimal, GraphicsFormat::GraphicsFormatR8UNorm));
	_framebufferLayout = pipeline.createFramebufferLayout(framebufferLayoutDesc);

	GraphicsFramebufferDesc ambientViewDesc;
	ambientViewDesc.setWidth(width);
	ambientViewDesc.setHeight(height);
	ambientViewDesc.addColorAttachment(GraphicsAttachmentBinding(_texAmbientMap, 0, 0));
	ambientViewDesc.setGraphicsFramebufferLayout(_framebufferLayout);
	_texAmbientView = pipeline.createFramebuffer(ambientViewDesc);

	GraphicsFramebufferDesc blurViewDesc;
	blurViewDesc.setWidth(width);
	blurViewDesc.setHeight(height);
	blurViewDesc.addColorAttachment(GraphicsAttachmentBinding(_texBlurMap, 0, 0));
	blurViewDesc.setGraphicsFramebufferLayout(_framebufferLayout);
	_texBlurView = pipeline.createFramebuffer(blurViewDesc);

	_ambientOcclusion = pipeline.createMaterial("sys:fx\\SSDO.fxml");
	assert(_ambientOcclusion);
	_ambientOcclusionPass = _ambientOcclusion->getTech("ComputeAO");
	_ambientOcclusionBlurXPass = _ambientOcclusion->getTech("BlurXAO");
	_ambientOcclusionBlurYPass = _ambientOcclusion->getTech("BlurYAO");
	_ambientOcclusionApply = _ambientOcclusion->getTech("Apply");

	_cameraProjScale = _ambientOcclusion->getParameter("projScale");

	_occlusionRadius = _ambientOcclusion->getParameter("radius");
	_occlusionBias = _ambientOcclusion->getParameter("bias");
	_occlusionIntensity = _ambientOcclusion->getParameter("intensity");
	_occlusionAmbient = _ambientOcclusion->getParameter("texOcclusion");
	_occlusionSphere = _ambientOcclusion->getParameter("sphere");
	_occlusionSourceInv = _ambientOcclusion->getParameter("texSourceInv");
	_occlusionSampleNumber = _ambientOcclusion->getMacro("NUM_SAMPLES");

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
SSDO::onDeactivate(RenderPipeline& pipeline) noexcept
{
	_ambientOcclusion.reset();

	_ambientOcclusionPass.reset();
	_ambientOcclusionBlurXPass.reset();
	_ambientOcclusionBlurYPass.reset();

	_cameraProjScale.reset();

	_occlusionRadius.reset();
	_occlusionBias.reset();
	_occlusionIntensity.reset();
	_occlusionAmbient.reset();
	_occlusionSphere.reset();
	_occlusionSampleNumber.reset();
	_occlusionSourceInv.reset();

	_blurSource.reset();
	_blurFactor.reset();
	_blurSharpness.reset();
	_blurDirection.reset();
	_blurGaussian.reset();
	_blurRadius.reset();

	_texBlurMap.reset();
	_texAmbientMap.reset();

	_framebufferLayout.reset();

	_texBlurView.reset();
	_texAmbientView.reset();
}

bool
SSDO::onRender(RenderPipeline& pipeline, RenderQueue queue, GraphicsFramebufferPtr& source, GraphicsFramebufferPtr swap) noexcept
{
	if (queue != RenderQueue::RenderQueueOpaqueLighting)
		return false;

	auto texture = source->getGraphicsFramebufferDesc().getColorAttachment(0).getBindingTexture();

	this->computeRawAO(pipeline, texture, _texAmbientView);

	if (_setting.blur)
	{
		this->blurHorizontal(pipeline, _texAmbientMap, _texBlurView);
		this->blurVertical(pipeline, _texBlurMap, _texAmbientView);
	}

	this->applySSDO(pipeline, _texAmbientMap, source);

	return false;
}

_NAME_END