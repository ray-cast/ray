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
// +-----------------------------------------------------------------------
#include "fimic.h"
#include <ray/material.h>

#include <ray/graphics_framebuffer.h>
#include <ray/graphics_texture.h>
#include <ray/render_pipeline.h>

_NAME_BEGIN

#define SAMPLE_COUNT 6
#define SAMPLE_LOG_SIZE 32
#define SAMPLE_LOG_COUNT SAMPLE_LOG_SIZE * SAMPLE_LOG_SIZE

float EyeAdaption(float lum)
{
	return math::lerp(0.2f, lum, 0.5f);
}

float ToneExposure(float avgLum)
{
	return 3 / (std::max(0.1f, 1 + 10 * EyeAdaption(avgLum)));
}

FimicToneMapping::Setting::Setting() noexcept
	: bloomThreshold(0.4)
	, bloomIntensity(10.0)
	, lumKey(0.98)
	, lumDelta(50.0)
	, lumExposure(1.6)
{
}

FimicToneMapping::FimicToneMapping() noexcept
	: _lumAdapt(0)
{
}

FimicToneMapping::~FimicToneMapping() noexcept
{
}

void
FimicToneMapping::setSetting(const Setting& setting) noexcept
{
	_bloomThreshold->uniform1f(_setting.bloomThreshold);
	_bloomIntensity->uniform1f(_setting.bloomIntensity);

	_toneLumExposure->uniform1f(setting.lumExposure);

	_setting = setting;
}

const FimicToneMapping::Setting&
FimicToneMapping::getSetting() const noexcept
{
	return _setting;
}

void
FimicToneMapping::measureLuminance(RenderPipeline& pipeline, GraphicsFramebufferPtr source) noexcept
{
	_timer->update();

	float lum = 0;
	float data[SAMPLE_LOG_COUNT];
	float delta = _timer->delta();

	pipeline.readFramebuffer(source, GraphicsFormat::GraphicsFormatR16SFloat, SAMPLE_LOG_SIZE, SAMPLE_LOG_SIZE, SAMPLE_LOG_COUNT * 4, data);

	for (std::size_t i = 0; i < SAMPLE_LOG_COUNT; ++i)
		lum += data[i];

	lum /= SAMPLE_LOG_COUNT;
	lum = std::exp(lum);

	_lumAdapt = _lumAdapt +((lum - _lumAdapt) * (1.0f - pow(_setting.lumKey, _setting.lumDelta * delta)));

	_toneLumExposure->uniform1f(_setting.lumExposure * ToneExposure(_lumAdapt));
}

void
FimicToneMapping::sunLum(RenderPipeline& pipeline, GraphicsTexturePtr source, GraphicsFramebufferPtr dest) noexcept
{
	_texSource->uniformTexture(source);
	_texSourceSizeInv->uniform2f(float2(1.0 / source->getGraphicsTextureDesc().getWidth(), 1.0 / source->getGraphicsTextureDesc().getHeight()));

	pipeline.setFramebuffer(dest);
	pipeline.discradRenderTexture();
	pipeline.drawScreenQuad(_sunLum);
}

void
FimicToneMapping::sunLumLog(RenderPipeline& pipeline, GraphicsTexturePtr source, GraphicsFramebufferPtr dest) noexcept
{
	_texSource->uniformTexture(source);
	_texSourceSizeInv->uniform2f(float2(1.0 / source->getGraphicsTextureDesc().getWidth(), 1.0 / source->getGraphicsTextureDesc().getHeight()));

	pipeline.setFramebuffer(dest);
	pipeline.discradRenderTexture();
	pipeline.drawScreenQuad(_sunLumLog);
}

void
FimicToneMapping::generateBloom(RenderPipeline& pipeline, GraphicsTexturePtr source, GraphicsFramebufferPtr dest) noexcept
{
	_texSource->uniformTexture(source);

	pipeline.setFramebuffer(dest);
	pipeline.discradRenderTexture();
	pipeline.drawScreenQuad(_bloom);
}

void
FimicToneMapping::blurh(RenderPipeline& pipeline, GraphicsTexturePtr source, GraphicsFramebufferPtr dest) noexcept
{
	_texSource->uniformTexture(source);

	pipeline.setFramebuffer(dest);
	pipeline.discradRenderTexture();
	pipeline.drawScreenQuad(_blurh);
}

void
FimicToneMapping::blurv(RenderPipeline& pipeline, GraphicsTexturePtr source, GraphicsFramebufferPtr dest) noexcept
{
	_texSource->uniformTexture(source);

	pipeline.setFramebuffer(dest);
	pipeline.discradRenderTexture();
	pipeline.drawScreenQuad(_blurv);
}

void
FimicToneMapping::generateToneMapping(RenderPipeline& pipeline, GraphicsTexturePtr bloom, GraphicsTexturePtr source, GraphicsFramebufferPtr dest) noexcept
{
	_texSource->uniformTexture(source);
	_toneBloom->uniformTexture(bloom);

	pipeline.setFramebuffer(dest);
	pipeline.discradRenderTexture();
	pipeline.drawScreenQuad(_tone);
}

void
FimicToneMapping::onActivate(RenderPipeline& pipeline) noexcept
{
	std::uint32_t width, height;
	pipeline.getWindowResolution(width, height);

	_texSample4Map = pipeline.createTexture(width / 2.0f, height / 2.0f, GraphicsTextureDim::GraphicsTextureDim2D, GraphicsFormat::GraphicsFormatR8G8B8A8UNorm);
	_texSample8Map = pipeline.createTexture(width / 4.0f, width / 4.0f, GraphicsTextureDim::GraphicsTextureDim2D, GraphicsFormat::GraphicsFormatR8G8B8A8UNorm);
	_texCombieMap = pipeline.createTexture(width / 4.0f, height / 4.0f, GraphicsTextureDim::GraphicsTextureDim2D, GraphicsFormat::GraphicsFormatR8G8B8A8UNorm);

	_texSampleLogMap = pipeline.createTexture(SAMPLE_LOG_SIZE, SAMPLE_LOG_SIZE, GraphicsTextureDim::GraphicsTextureDim2D, GraphicsFormat::GraphicsFormatR16SFloat);

	GraphicsFramebufferLayoutDesc framebufferLayoutDesc;
	framebufferLayoutDesc.addComponent(GraphicsAttachmentDesc(GraphicsViewLayout::GraphicsViewLayoutColorAttachmentOptimal, GraphicsFormat::GraphicsFormatR8G8B8A8UNorm, 0));
	_sampleViewLayout = pipeline.createFramebufferLayout(framebufferLayoutDesc);

	GraphicsFramebufferLayoutDesc framebufferLogLayoutDesc;
	framebufferLogLayoutDesc.addComponent(GraphicsAttachmentDesc(GraphicsViewLayout::GraphicsViewLayoutColorAttachmentOptimal, GraphicsFormat::GraphicsFormatR16SFloat, 0));
	_sampleLogViewLayout = pipeline.createFramebufferLayout(framebufferLogLayoutDesc);

	GraphicsFramebufferDesc sample4ViewDesc;
	sample4ViewDesc.setWidth(width / 2.0f);
	sample4ViewDesc.setHeight(height / 2.0f);
	sample4ViewDesc.attach(_texSample4Map);
	sample4ViewDesc.setGraphicsFramebufferLayout(_sampleViewLayout);
	_texSample4View = pipeline.createFramebuffer(sample4ViewDesc);

	GraphicsFramebufferDesc sample8ViewDesc;
	sample8ViewDesc.setWidth(width / 4.0f);
	sample8ViewDesc.setHeight(width / 4.0f);
	sample8ViewDesc.attach(_texSample8Map);
	sample8ViewDesc.setGraphicsFramebufferLayout(_sampleViewLayout);
	_texSample8View = pipeline.createFramebuffer(sample8ViewDesc);

	GraphicsFramebufferDesc sampleLogViewDesc;
	sampleLogViewDesc.setWidth(SAMPLE_LOG_SIZE);
	sampleLogViewDesc.setHeight(SAMPLE_LOG_SIZE);
	sampleLogViewDesc.attach(_texSampleLogMap);
	sampleLogViewDesc.setGraphicsFramebufferLayout(_sampleLogViewLayout);
	_texSampleLogView = pipeline.createFramebuffer(sampleLogViewDesc);

	GraphicsFramebufferDesc sampleCombieViewDesc;
	sampleCombieViewDesc.setWidth(width / 4.0f);
	sampleCombieViewDesc.setHeight(height / 4.0f);
	sampleCombieViewDesc.attach(_texCombieMap);
	sampleCombieViewDesc.setGraphicsFramebufferLayout(_sampleViewLayout);
	_texCombieView = pipeline.createFramebuffer(sampleCombieViewDesc);

	_fimic = pipeline.createMaterial("sys:fx/fimic.fxml.o");

	_sunLum = _fimic->getTech("SumLum");
	_sunLumLog = _fimic->getTech("SumLumLog");
	_bloom = _fimic->getTech("GenerateBloom");
	_blurh = _fimic->getTech("BlurBloomh");
	_blurv = _fimic->getTech("BlurBloomv");
	_tone = _fimic->getTech("FimicTongMapping");

	_bloomThreshold = _fimic->getParameter("bloomThreshold");
	_bloomIntensity = _fimic->getParameter("bloomIntensity");
	_bloomOffset = _fimic->getParameter("bloomOffset");
	_bloomWeight = _fimic->getParameter("bloomWeight");

	_toneBloom = _fimic->getParameter("texBloom");
	_toneLumExposure = _fimic->getParameter("exposure");

	_texSource = _fimic->getParameter("texSource");
	_texSourceSizeInv = _fimic->getParameter("texSourceSizeInv");

	_timer = std::make_shared<Timer>();

	float2 offset[] = {
		float2(-5.0f / (width / 4.0f), -5.0f / (height / 4.0f)),
		float2(-4.0f / (width / 4.0f), -4.0f / (height / 4.0f)),
		float2(-3.0f / (width / 4.0f), -3.0f / (height / 4.0f)),
		float2(-2.0f / (width / 4.0f), -2.0f / (height / 4.0f)),
		float2(-1.0f / (width / 4.0f), -1.0f / (height / 4.0f)),
		float2(1.0f / (width / 4.0f), 1.0f / (height / 4.0f)),
		float2(2.0f / (width / 4.0f), 2.0f / (height / 4.0f)),
		float2(3.0f / (width / 4.0f), 3.0f / (height / 4.0f)),
		float2(4.0f / (width / 4.0f), 4.0f / (height / 4.0f)),
		float2(5.0f / (width / 4.0f), 5.0f / (height / 4.0f)),
	};
	
	float weight[] = { 0.2,0.02,0.044,0.0716,0.1046,0.1686,0.1686,0.1046,0.0716,0.044,0.02 };

	_bloomOffset->uniform2fv(sizeof(offset) / sizeof(offset[0]), (float*)offset);
	_bloomWeight->uniform1fv(sizeof(weight) / sizeof(weight[0]), weight);

	this->setSetting(_setting);
}

void
FimicToneMapping::onDeactivate(RenderPipeline& pipeline) noexcept
{
	_texSample4Map.reset();
	_texSample8Map.reset();
	_texSampleLogMap.reset();
	_texCombieMap.reset();

	_texSample4View.reset();
	_texSample8View.reset();
	_texSampleLogView.reset();
	_texCombieView.reset();
}

bool
FimicToneMapping::onRender(RenderPipeline& pipeline, GraphicsFramebufferPtr source, GraphicsFramebufferPtr dest) noexcept
{
	auto texture = source->getGraphicsFramebufferDesc().getTextures().front();

	this->sunLum(pipeline, texture, _texSample4View);
	this->sunLum(pipeline, _texSample4Map, _texSample8View);
	this->sunLumLog(pipeline, _texSample8Map, _texSampleLogView);

	this->measureLuminance(pipeline, _texSampleLogView);

	this->generateBloom(pipeline, _texSample8Map, _texCombieView);

	this->blurh(pipeline, _texCombieMap, _texSample8View);
	this->blurv(pipeline, _texSample8Map, _texCombieView);

	this->generateToneMapping(pipeline, _texCombieMap, texture, dest);

	return true;
}

_NAME_END