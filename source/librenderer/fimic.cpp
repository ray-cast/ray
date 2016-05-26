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

FimicToneMapping::Setting::Setting() noexcept
	: bloomThreshold(0.5f)
	, bloomIntensity(1.0f)
	, exposure(2.0f)
	, middleGray(0.5f)
{
}

FimicToneMapping::FimicToneMapping() noexcept
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

	_toneLumExposure->uniform1f(setting.exposure);

	_setting = setting;
}

const FimicToneMapping::Setting&
FimicToneMapping::getSetting() const noexcept
{
	return _setting;
}

void
FimicToneMapping::sunLum(RenderPipeline& pipeline, GraphicsTexturePtr source, GraphicsFramebufferPtr dest) noexcept
{
	float width = source->getGraphicsTextureDesc().getWidth() * 1.5;
	float height = source->getGraphicsTextureDesc().getHeight() * 1.5;

	_texSource->uniformTexture(source);
	_texSourceSizeInv->uniform2f(1.0f / width, 1.0f / height);

	pipeline.setFramebuffer(dest);
	pipeline.clearFramebuffer(0, GraphicsClearFlagBits::GraphicsClearFlagColorBit, ray::float4::Zero, 1.0, 0);
	pipeline.drawScreenQuad(*_sunLum);
}

void
FimicToneMapping::sunLumLog(RenderPipeline& pipeline, GraphicsTexturePtr source, GraphicsFramebufferPtr dest) noexcept
{
	float width = source->getGraphicsTextureDesc().getWidth() * 1.5;
	float height = source->getGraphicsTextureDesc().getHeight() * 1.5;

	_texSource->uniformTexture(source);
	_texSourceSizeInv->uniform2f(1.0f / width, 1.0f / height);

	pipeline.setFramebuffer(dest);
	pipeline.clearFramebuffer(0, GraphicsClearFlagBits::GraphicsClearFlagColorBit, ray::float4::Zero, 1.0, 0);
	pipeline.drawScreenQuad(*_sunLumLog);
}

void
FimicToneMapping::avgLuminance(RenderPipeline& pipeline, GraphicsTexturePtr adaptedLum, GraphicsTexturePtr source, GraphicsFramebufferPtr dest) noexcept
{
	float width = source->getGraphicsTextureDesc().getWidth() * 1.5;
	float height = source->getGraphicsTextureDesc().getHeight() * 1.5;

	_timer->update();

	_texSource->uniformTexture(source);
	_texSourceSizeInv->uniform2f(1.0f / width, 1.0f / height);

	_texLumAve->uniformTexture(adaptedLum);

	_delta->uniform1f(_timer->delta());

	pipeline.setFramebuffer(dest);
	pipeline.clearFramebuffer(0, GraphicsClearFlagBits::GraphicsClearFlagColorBit, ray::float4::Zero, 1.0, 0);
	pipeline.drawScreenQuad(*_avgLuminance);
}

void
FimicToneMapping::generateBloom(RenderPipeline& pipeline, GraphicsTexturePtr source, GraphicsFramebufferPtr dest) noexcept
{
	_texSource->uniformTexture(source);

	GraphicsAttachmentType attachment[] = { GraphicsAttachmentType::GraphicsAttachmentTypeColor0 };

	pipeline.setFramebuffer(dest);
	pipeline.discradRenderTexture(attachment, 1);
	pipeline.drawScreenQuad(*_bloom);
}

void
FimicToneMapping::blurh(RenderPipeline& pipeline, GraphicsTexturePtr source, GraphicsFramebufferPtr dest) noexcept
{
	_texSource->uniformTexture(source);

	GraphicsAttachmentType attachment[] = { GraphicsAttachmentType::GraphicsAttachmentTypeColor0 };

	pipeline.setFramebuffer(dest);
	pipeline.discradRenderTexture(attachment, 1);
	pipeline.drawScreenQuad(*_blurh);
}

void
FimicToneMapping::blurv(RenderPipeline& pipeline, GraphicsTexturePtr source, GraphicsFramebufferPtr dest) noexcept
{
	_texSource->uniformTexture(source);

	GraphicsAttachmentType attachment[] = { GraphicsAttachmentType::GraphicsAttachmentTypeColor0 };

	pipeline.setFramebuffer(dest);
	pipeline.discradRenderTexture(attachment, 1);
	pipeline.drawScreenQuad(*_blurv);
}

void
FimicToneMapping::generateToneMapping(RenderPipeline& pipeline, GraphicsTexturePtr bloom, GraphicsTexturePtr source, GraphicsFramebufferPtr dest) noexcept
{
	_texSource->uniformTexture(source);
	_toneBloom->uniformTexture(bloom);

	GraphicsAttachmentType attachment[] = { GraphicsAttachmentType::GraphicsAttachmentTypeColor0 };

	pipeline.setFramebuffer(dest);
	pipeline.discradRenderTexture(attachment, 1);
	pipeline.drawScreenQuad(*_tone);
}

void
FimicToneMapping::onActivate(RenderPipeline& pipeline) noexcept
{
	std::uint32_t width, height;
	pipeline.getWindowResolution(width, height);

	_texSample4Map = pipeline.createTexture(width / 2.0f, height / 2.0f, GraphicsTextureDim::GraphicsTextureDim2D, GraphicsFormat::GraphicsFormatR8G8B8UNorm);
	_texSample8Map = pipeline.createTexture(width / 4.0f, width / 4.0f, GraphicsTextureDim::GraphicsTextureDim2D, GraphicsFormat::GraphicsFormatR8G8B8UNorm);
	_texCombieMap = pipeline.createTexture(width / 4.0f, height / 4.0f, GraphicsTextureDim::GraphicsTextureDim2D, GraphicsFormat::GraphicsFormatR8G8B8UNorm);

	_texSampleLog64Map = pipeline.createTexture(64, 64, GraphicsTextureDim::GraphicsTextureDim2D, GraphicsFormat::GraphicsFormatR16SFloat);
	_texSampleLog16Map = pipeline.createTexture(16, 16, GraphicsTextureDim::GraphicsTextureDim2D, GraphicsFormat::GraphicsFormatR16SFloat);
	_texSampleLog4Map = pipeline.createTexture(4, 4, GraphicsTextureDim::GraphicsTextureDim2D, GraphicsFormat::GraphicsFormatR16SFloat);
	_texSampleLumMap = pipeline.createTexture(1, 1, GraphicsTextureDim::GraphicsTextureDim2D, GraphicsFormat::GraphicsFormatR16SFloat, GraphicsSamplerFilter::GraphicsSamplerFilterNearest);
	_texSampleLumMap2 = pipeline.createTexture(1, 1, GraphicsTextureDim::GraphicsTextureDim2D, GraphicsFormat::GraphicsFormatR16SFloat, GraphicsSamplerFilter::GraphicsSamplerFilterNearest);

	GraphicsFramebufferLayoutDesc framebufferLayoutDesc;
	framebufferLayoutDesc.addComponent(GraphicsAttachment(0, GraphicsImageLayout::GraphicsImageLayoutColorAttachmentOptimal, GraphicsFormat::GraphicsFormatR8G8B8UNorm));
	_sampleImageLayout = pipeline.createFramebufferLayout(framebufferLayoutDesc);

	GraphicsFramebufferLayoutDesc framebufferLogLayoutDesc;
	framebufferLogLayoutDesc.addComponent(GraphicsAttachment(0, GraphicsImageLayout::GraphicsImageLayoutColorAttachmentOptimal, GraphicsFormat::GraphicsFormatR16SFloat));
	_sampleLogImageLayout = pipeline.createFramebufferLayout(framebufferLogLayoutDesc);

	GraphicsFramebufferDesc sample4ViewDesc;
	sample4ViewDesc.setWidth(width / 2.0f);
	sample4ViewDesc.setHeight(height / 2.0f);
	sample4ViewDesc.attach(_texSample4Map);
	sample4ViewDesc.setGraphicsFramebufferLayout(_sampleImageLayout);
	_texSample4View = pipeline.createFramebuffer(sample4ViewDesc);

	GraphicsFramebufferDesc sample8ViewDesc;
	sample8ViewDesc.setWidth(width / 4.0f);
	sample8ViewDesc.setHeight(width / 4.0f);
	sample8ViewDesc.attach(_texSample8Map);
	sample8ViewDesc.setGraphicsFramebufferLayout(_sampleImageLayout);
	_texSample8View = pipeline.createFramebuffer(sample8ViewDesc);

	GraphicsFramebufferDesc sampleLog64ViewDesc;
	sampleLog64ViewDesc.setWidth(64);
	sampleLog64ViewDesc.setHeight(64);
	sampleLog64ViewDesc.attach(_texSampleLog64Map);
	sampleLog64ViewDesc.setGraphicsFramebufferLayout(_sampleLogImageLayout);
	_texSampleLog64View = pipeline.createFramebuffer(sampleLog64ViewDesc);

	GraphicsFramebufferDesc sampleLog16ViewDesc;
	sampleLog16ViewDesc.setWidth(16);
	sampleLog16ViewDesc.setHeight(16);
	sampleLog16ViewDesc.attach(_texSampleLog16Map);
	sampleLog16ViewDesc.setGraphicsFramebufferLayout(_sampleLogImageLayout);
	_texSampleLog16View = pipeline.createFramebuffer(sampleLog16ViewDesc);

	GraphicsFramebufferDesc sampleLog4ViewDesc;
	sampleLog4ViewDesc.setWidth(4);
	sampleLog4ViewDesc.setHeight(4);
	sampleLog4ViewDesc.attach(_texSampleLog4Map);
	sampleLog4ViewDesc.setGraphicsFramebufferLayout(_sampleLogImageLayout);
	_texSampleLog4View = pipeline.createFramebuffer(sampleLog4ViewDesc);

	GraphicsFramebufferDesc sampleLog1ViewDesc;
	sampleLog1ViewDesc.setWidth(1);
	sampleLog1ViewDesc.setHeight(1);
	sampleLog1ViewDesc.attach(_texSampleLumMap);
	sampleLog1ViewDesc.setGraphicsFramebufferLayout(_sampleLogImageLayout);
	_texSampleLumView = pipeline.createFramebuffer(sampleLog1ViewDesc);
	_texSampleLumView2 = pipeline.createFramebuffer(sampleLog1ViewDesc);

	GraphicsFramebufferDesc sampleCombieViewDesc;
	sampleCombieViewDesc.setWidth(width / 4.0f);
	sampleCombieViewDesc.setHeight(height / 4.0f);
	sampleCombieViewDesc.attach(_texCombieMap);
	sampleCombieViewDesc.setGraphicsFramebufferLayout(_sampleImageLayout);
	_texCombieView = pipeline.createFramebuffer(sampleCombieViewDesc);

	_fimic = pipeline.createMaterial("sys:fx/fimic.fxml");

	_sunLum = _fimic->getTech("SumLum");
	_sunLumLog = _fimic->getTech("SumLumLog");
	_avgLuminance = _fimic->getTech("AvgLuminance");
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

	_delta = _fimic->getParameter("delta");

	_texLumAve = _fimic->getParameter("texLumAve");
	_texSource = _fimic->getParameter("texSource");
	_texSourceSizeInv = _fimic->getParameter("texSourceSizeInv");

	_texLumAve->uniformTexture(_texSampleLumMap);
	_toneLumExposure->uniform1f(_setting.exposure);

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

	float weight[] = { 0.2f,0.02f,0.044f,0.0716f,0.1046f,0.1686f,0.1686f,0.1046f,0.0716f,0.044f,0.02f };

	_bloomOffset->uniform2fv(sizeof(offset) / sizeof(offset[0]), (float*)offset);
	_bloomWeight->uniform1fv(sizeof(weight) / sizeof(weight[0]), weight);

	_timer = std::make_shared<Timer>();
	_timer->open();

	this->setSetting(_setting);
}

void
FimicToneMapping::onDeactivate(RenderPipeline& pipeline) noexcept
{
	_texSample4Map.reset();
	_texSample8Map.reset();
	_texSampleLog64Map.reset();
	_texCombieMap.reset();

	_texSample4View.reset();
	_texSample8View.reset();
	_texSampleLog64View.reset();
	_texCombieView.reset();
}

bool
FimicToneMapping::onRender(RenderPipeline& pipeline, RenderQueue queue, GraphicsFramebufferPtr& source, GraphicsFramebufferPtr swap) noexcept
{
	if (queue != RenderQueue::RenderQueuePostprocess)
		return false;

	auto texture = source->getGraphicsFramebufferDesc().getTextures().front();

	this->sunLum(pipeline, texture, _texSample4View);
	this->sunLum(pipeline, _texSample4Map, _texSample8View);

	this->sunLumLog(pipeline, _texSample8Map, _texSampleLog64View);
	this->sunLum(pipeline, _texSampleLog64Map, _texSampleLog16View);
	this->sunLum(pipeline, _texSampleLog16Map, _texSampleLog4View);
	this->avgLuminance(pipeline, _texSampleLumMap, _texSampleLog4Map, _texSampleLumView);

	this->generateBloom(pipeline, _texSample8Map, _texCombieView);

	this->blurh(pipeline, _texCombieMap, _texSample8View);
	this->blurv(pipeline, _texSample8Map, _texCombieView);

	this->generateToneMapping(pipeline, _texCombieMap, texture, swap);

	return true;
}

_NAME_END