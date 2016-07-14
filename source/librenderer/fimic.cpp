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
	: bloomThreshold(0.8f)
	, bloomIntensity(1.0f)
	, exposure(1.5f)
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
	float width = source->getGraphicsTextureDesc().getWidth();
	float height = source->getGraphicsTextureDesc().getHeight();

	_texSource->uniformTexture(source);
	_texSourceSizeInv->uniform2f(1.0 / width, 1.0 / height);

	pipeline.setFramebuffer(dest);
	pipeline.discardFramebuffer(0);
	pipeline.drawScreenQuad(*_sunLum);
}

void
FimicToneMapping::sunLumLog(RenderPipeline& pipeline, GraphicsTexturePtr source, GraphicsFramebufferPtr dest) noexcept
{
	float width = source->getGraphicsTextureDesc().getWidth();
	float height = source->getGraphicsTextureDesc().getHeight();

	_texSource->uniformTexture(source);
	_texSourceSizeInv->uniform2f(1.0 / width, 1.0 / height);

	pipeline.setFramebuffer(dest);
	pipeline.discardFramebuffer(0);
	pipeline.drawScreenQuad(*_sunLumLog);
	pipeline.generateMipmap(dest->getGraphicsFramebufferDesc().getColorAttachment(0).getBindingTexture());
}

void
FimicToneMapping::avgLuminance(RenderPipeline& pipeline, GraphicsTexturePtr adaptedLum, GraphicsTexturePtr source, GraphicsFramebufferPtr dest) noexcept
{
	float width = source->getGraphicsTextureDesc().getWidth();
	float height = source->getGraphicsTextureDesc().getHeight();

	_timer->update();

	_texSource->uniformTexture(source);
	_texSourceSizeInv->uniform2f(1.0f / width, 1.0f / height);

	_texLumAve->uniformTexture(adaptedLum);

	_delta->uniform1f(_timer->delta());

	pipeline.setFramebuffer(dest);
	pipeline.drawScreenQuad(*_avgLuminance);
}

void
FimicToneMapping::generateBloom(RenderPipeline& pipeline, GraphicsTexturePtr source, GraphicsFramebufferPtr dest) noexcept
{
	_texSource->uniformTexture(source);

	pipeline.setFramebuffer(dest);
	pipeline.discardFramebuffer(0);
	pipeline.drawScreenQuad(*_bloom);
	pipeline.generateMipmap(dest->getGraphicsFramebufferDesc().getColorAttachment(0).getBindingTexture());
}

void
FimicToneMapping::blurh(RenderPipeline& pipeline, GraphicsTexturePtr source, GraphicsFramebufferPtr dest, std::uint32_t level) noexcept
{
	_texSource->uniformTexture(source);
	_texSourceLevel->uniform1i(level);
	_texSourceSizeInv->uniform2f(1.0f / dest->getGraphicsFramebufferDesc().getWidth(), 1.0f / dest->getGraphicsFramebufferDesc().getHeight());

	pipeline.setFramebuffer(dest);
	pipeline.discardFramebuffer(0);
	pipeline.drawScreenQuad(*_blurh);
}

void
FimicToneMapping::blurv(RenderPipeline& pipeline, GraphicsTexturePtr source, GraphicsFramebufferPtr dest, std::uint32_t level) noexcept
{
	_texSource->uniformTexture(source);
	_texSourceLevel->uniform1i(level);
	_texSourceSizeInv->uniform2f(1.0f / dest->getGraphicsFramebufferDesc().getWidth(), 1.0f / dest->getGraphicsFramebufferDesc().getHeight());

	pipeline.setFramebuffer(dest);
	pipeline.discardFramebuffer(0);
	pipeline.drawScreenQuad(*_blurv);
}

void
FimicToneMapping::generateToneMapping(RenderPipeline& pipeline, GraphicsTexturePtr bloom, GraphicsTexturePtr source, GraphicsFramebufferPtr dest) noexcept
{
	_texSource->uniformTexture(source);
	_toneBloom->uniformTexture(bloom);

	pipeline.setFramebuffer(dest);
	pipeline.discardFramebuffer(0);
	pipeline.drawScreenQuad(*_tone);
}

void
FimicToneMapping::onActivate(RenderPipeline& pipeline) noexcept
{
	std::uint32_t width, height;
	pipeline.getWindowResolution(width, height);

	GraphicsTextureDesc samplerBloomDesc;
	samplerBloomDesc.setWidth(width / 2);
	samplerBloomDesc.setHeight(height / 2);
	samplerBloomDesc.setTexDim(GraphicsTextureDim::GraphicsTextureDim2D);
	samplerBloomDesc.setTexFormat(GraphicsFormat::GraphicsFormatR8G8B8A8UNorm);
	samplerBloomDesc.setMipBase(0);
	samplerBloomDesc.setMipLevel(3);
	samplerBloomDesc.setSamplerWrap(GraphicsSamplerWrap::GraphicsSamplerWrapClampToEdge);
	samplerBloomDesc.setSamplerFilter(GraphicsSamplerFilter::GraphicsSamplerFilterLinearMipmapNearest);
	_texBloom1Map = pipeline.createTexture(samplerBloomDesc);
	_texBloom2Map = pipeline.createTexture(samplerBloomDesc);
	
	GraphicsTextureDesc samplerLogDesc;
	samplerLogDesc.setWidth(256);
	samplerLogDesc.setHeight(256);
	samplerLogDesc.setTexDim(GraphicsTextureDim::GraphicsTextureDim2D);
	samplerLogDesc.setTexFormat(GraphicsFormat::GraphicsFormatR16SFloat);
	samplerLogDesc.setMipBase(0);
	samplerLogDesc.setMipLevel(9);
	samplerLogDesc.setSamplerWrap(GraphicsSamplerWrap::GraphicsSamplerWrapClampToEdge);
	samplerLogDesc.setSamplerFilter(GraphicsSamplerFilter::GraphicsSamplerFilterNearestMipmapNearest);
	_texSampleLogMap = pipeline.createTexture(samplerLogDesc);

	GraphicsTextureDesc samplerLumDesc;
	samplerLumDesc.setWidth(1);
	samplerLumDesc.setHeight(1);
	samplerLumDesc.setTexDim(GraphicsTextureDim::GraphicsTextureDim2D);
	samplerLumDesc.setTexFormat(GraphicsFormat::GraphicsFormatR16SFloat);
	samplerLumDesc.setSamplerFilter(GraphicsSamplerFilter::GraphicsSamplerFilterNearest);
	samplerLumDesc.setSamplerWrap(GraphicsSamplerWrap::GraphicsSamplerWrapClampToEdge);
	_texSampleLumMap = pipeline.createTexture(samplerLumDesc);

	GraphicsFramebufferLayoutDesc framebufferBloomLayoutDesc;
	framebufferBloomLayoutDesc.addComponent(GraphicsAttachmentLayout(0, GraphicsImageLayout::GraphicsImageLayoutColorAttachmentOptimal, GraphicsFormat::GraphicsFormatR8G8B8UNorm));
	_sampleBloomImageLayout = pipeline.createFramebufferLayout(framebufferBloomLayoutDesc);

	GraphicsFramebufferLayoutDesc framebufferLogLayoutDesc;
	framebufferLogLayoutDesc.addComponent(GraphicsAttachmentLayout(0, GraphicsImageLayout::GraphicsImageLayoutColorAttachmentOptimal, GraphicsFormat::GraphicsFormatR16SFloat));
	_sampleLogImageLayout = pipeline.createFramebufferLayout(framebufferLogLayoutDesc);

	GraphicsFramebufferDesc sampleLogViewDesc;
	sampleLogViewDesc.setWidth(256);
	sampleLogViewDesc.setHeight(256);
	sampleLogViewDesc.addColorAttachment(GraphicsAttachmentBinding(_texSampleLogMap, 0, 0));
	sampleLogViewDesc.setGraphicsFramebufferLayout(_sampleLogImageLayout);
	_texSampleLogView = pipeline.createFramebuffer(sampleLogViewDesc);

	GraphicsFramebufferDesc sampleLog1ViewDesc;
	sampleLog1ViewDesc.setWidth(1);
	sampleLog1ViewDesc.setHeight(1);
	sampleLog1ViewDesc.addColorAttachment(GraphicsAttachmentBinding(_texSampleLumMap, 0, 0));
	sampleLog1ViewDesc.setGraphicsFramebufferLayout(_sampleLogImageLayout);
	_texSampleLumView = pipeline.createFramebuffer(sampleLog1ViewDesc);

	_texBloom1View.resize(3);
	_texBloom2View.resize(3);

	for (std::size_t i = 0; i < 3; i++)
	{
		GraphicsFramebufferDesc bloom1ViewDesc;
		bloom1ViewDesc.setWidth(width / ((2 << i)));
		bloom1ViewDesc.setHeight(height / ((2 << i)));
		bloom1ViewDesc.addColorAttachment(GraphicsAttachmentBinding(_texBloom1Map, i, 0));
		bloom1ViewDesc.setGraphicsFramebufferLayout(_sampleBloomImageLayout);
		_texBloom1View[i] = pipeline.createFramebuffer(bloom1ViewDesc);

		GraphicsFramebufferDesc bloom2ViewDesc;
		bloom2ViewDesc.setWidth(width / ((2 << i)));
		bloom2ViewDesc.setHeight(height / ((2 << i)));
		bloom2ViewDesc.addColorAttachment(GraphicsAttachmentBinding(_texBloom2Map, i, 0));
		bloom2ViewDesc.setGraphicsFramebufferLayout(_sampleBloomImageLayout);
		_texBloom2View[i] = pipeline.createFramebuffer(bloom2ViewDesc);
	}

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
	_texSourceLevel = _fimic->getParameter("texSourceLevel");

	_texLumAve->uniformTexture(_texSampleLumMap);
	_toneLumExposure->uniform1f(_setting.exposure);

	float offset[] = { -5.0f, -4.0f, -3.0f, -2.0f, -1.0f, 1.0f, 2.0f, 3.0f, 4.0f, 5.0f,	};
	float weight[] = { 0.2f, 0.02f, 0.044f, 0.0716f, 0.1046f, 0.1686f, 0.1686f, 0.1046f, 0.0716f, 0.044f, 0.02f };

	_bloomOffset->uniform1fv(sizeof(offset) / sizeof(offset[0]), (float*)offset);
	_bloomWeight->uniform1fv(sizeof(weight) / sizeof(weight[0]), weight);

	_timer = std::make_shared<Timer>();
	_timer->open();

	this->setSetting(_setting);
}

void
FimicToneMapping::onDeactivate(RenderPipeline& pipeline) noexcept
{
	_timer.reset();
	_fimic.reset();
	_sunLum.reset();
	_sunLumLog.reset();
	_avgLuminance.reset();
	_bloom.reset();
	_blurh.reset();
	_blurv.reset();
	_tone.reset();
	_bloomThreshold.reset();
	_bloomIntensity.reset();
	_bloomOffset.reset();
	_bloomWeight.reset();
	_toneBloom.reset();
	_toneLumAve.reset();
	_toneLumKey.reset();
	_toneLumExposure.reset();
	_toneBurnout.reset();
	_toneDefocus.reset();
	_delta.reset();
	_texLumAve.reset();
	_texSource.reset();
	_texSourceSizeInv.reset();

	_texBloom1View.clear();
	_texBloom2View.clear();
	_texSampleLogView.reset();
	_texSampleLumView.reset();

	_texBloom1Map.reset();
	_texBloom2Map.reset();
	_texSampleLogMap.reset();
	_texSampleLumMap.reset();

	_sampleBloomImageLayout.reset();
	_sampleLogImageLayout.reset();
}

bool
FimicToneMapping::onRender(RenderPipeline& pipeline, RenderQueue queue, GraphicsFramebufferPtr& source, GraphicsFramebufferPtr swap) noexcept
{
	if (queue != RenderQueue::RenderQueuePostprocess)
		return false;

	auto texture = source->getGraphicsFramebufferDesc().getColorAttachment(0).getBindingTexture();

	this->sunLum(pipeline, texture, _texBloom1View[0]);
	this->sunLumLog(pipeline, _texBloom1Map, _texSampleLogView);

	this->avgLuminance(pipeline, _texSampleLumMap, _texSampleLogMap, _texSampleLumView);

	this->generateBloom(pipeline, _texBloom1Map, _texBloom2View[0]);

	for (std::size_t i = 0; i < 3; i++)
	{
		this->blurh(pipeline, _texBloom2Map, _texBloom1View[i], i);
		this->blurv(pipeline, _texBloom1Map, _texBloom2View[i], i);
	}

	this->generateToneMapping(pipeline, _texBloom2Map, texture, swap);

	return true;
}

_NAME_END