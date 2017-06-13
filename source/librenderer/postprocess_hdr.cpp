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
#include "postprocess_hdr.h"
#include <ray/material.h>

#include <ray/graphics_framebuffer.h>
#include <ray/graphics_texture.h>
#include <ray/render_pipeline.h>

_NAME_BEGIN

PostProcessHDR::Setting::Setting() noexcept
	: bloomThreshold(1.07f)
	, bloomIntensity(1.0f)
	, exposure(2.0f)
	, enableBloom(true)
	, enableEyeAdaptation(true)
{
}

PostProcessHDR::PostProcessHDR() noexcept
{
}

PostProcessHDR::~PostProcessHDR() noexcept
{
}

void
PostProcessHDR::setSetting(const Setting& setting) noexcept
{
	_bloomThreshold->uniform1f(_setting.bloomThreshold);
	_bloomIntensity->uniform1f(_setting.bloomIntensity);

	_toneLumExposure->uniform1f(setting.exposure);

	_setting = setting;
}

const PostProcessHDR::Setting&
PostProcessHDR::getSetting() const noexcept
{
	return _setting;
}

void
PostProcessHDR::sumLum(RenderPipeline& pipeline, const GraphicsTexturePtr& source, const GraphicsFramebufferPtr& dest, std::uint8_t level) noexcept
{
	_texSource->uniformTexture(source);
	_texSourceLevel->uniform1i(level);
	_texSourceSizeInv->uniform2f(float(1 << level) / source->getGraphicsTextureDesc().getWidth(), float(1 << level) / source->getGraphicsTextureDesc().getHeight());

	pipeline.setFramebuffer(dest);
	pipeline.discardFramebuffer(0);
	pipeline.drawScreenQuad(*_sumLum);
}

void
PostProcessHDR::sumLumLog(RenderPipeline& pipeline, const GraphicsTexturePtr& source, const GraphicsFramebufferPtr& dest) noexcept
{
	_texSource->uniformTexture(source);
	_texSourceSizeInv->uniform2f(1.0f / source->getGraphicsTextureDesc().getWidth(), 1.0f / source->getGraphicsTextureDesc().getHeight());

	pipeline.setFramebuffer(dest);
	pipeline.discardFramebuffer(0);
	pipeline.drawScreenQuad(*_sumLumLog);
	pipeline.generateMipmap(dest->getGraphicsFramebufferDesc().getColorAttachment(0).getBindingTexture());
}

void
PostProcessHDR::avgLuminance(RenderPipeline& pipeline, const GraphicsTexturePtr& adaptedLum, const GraphicsTexturePtr& source, const GraphicsFramebufferPtr& dest) noexcept
{
	_timer->update();

	_texSource->uniformTexture(source);
	_texSourceSizeInv->uniform2f(1.0f / source->getGraphicsTextureDesc().getWidth(), 1.0f / source->getGraphicsTextureDesc().getHeight());

	_texLumAve->uniformTexture(adaptedLum);

	_delta->uniform1f(1.0f - std::pow(0.98f, 50.0f * _timer->delta()));

	pipeline.setFramebuffer(dest);
	pipeline.drawScreenQuad(*_avgLuminance);
}

void
PostProcessHDR::doBloom(RenderPipeline& pipeline, const GraphicsTexturePtr& source, const GraphicsFramebufferPtr& dest) noexcept
{
	_texSource->uniformTexture(source);
	_texSourceSizeInv->uniform2f(1.0f / source->getGraphicsTextureDesc().getWidth(), 1.0f / source->getGraphicsTextureDesc().getHeight());

	pipeline.setFramebuffer(dest);
	pipeline.discardFramebuffer(0);
	pipeline.drawScreenQuad(*_bloom);
	pipeline.generateMipmap(dest->getGraphicsFramebufferDesc().getColorAttachment(0).getBindingTexture());
}

void
PostProcessHDR::blurh(RenderPipeline& pipeline, const GraphicsTexturePtr& source, const GraphicsFramebufferPtr& dest, std::uint32_t level) noexcept
{
	_texSource->uniformTexture(source);
	_texSourceLevel->uniform1i(level);
	_texSourceSizeInv->uniform2f(float(1 << level) / source->getGraphicsTextureDesc().getWidth(), 0.0);

	pipeline.setFramebuffer(dest);
	pipeline.discardFramebuffer(0);
	pipeline.drawScreenQuad(*_blur);
}

void
PostProcessHDR::blurv(RenderPipeline& pipeline, const GraphicsTexturePtr& source, const GraphicsFramebufferPtr& dest, std::uint32_t level) noexcept
{
	_texSource->uniformTexture(source);
	_texSourceLevel->uniform1i(level);
	_texSourceSizeInv->uniform2f(0.0, float(1 << level) / source->getGraphicsTextureDesc().getHeight());

	pipeline.setFramebuffer(dest);
	pipeline.discardFramebuffer(0);
	pipeline.drawScreenQuad(*_blur);
}

void
PostProcessHDR::bloomCombine(RenderPipeline& pipeline, const GraphicsTexturePtr& source, const GraphicsFramebufferPtr& dest) noexcept
{
	_texSource->uniformTexture(source);

	pipeline.setFramebuffer(dest);
	pipeline.discardFramebuffer(0);
	pipeline.drawScreenQuad(*_bloomCombine);
}

void
PostProcessHDR::doTonemapping(RenderPipeline& pipeline, const GraphicsTexturePtr& bloom, const GraphicsTexturePtr& source, const GraphicsFramebufferPtr& dest) noexcept
{
	_texSource->uniformTexture(source);
	_toneBloom->uniformTexture(bloom);

	pipeline.setFramebuffer(dest);
	pipeline.discardFramebuffer(0);
	pipeline.drawScreenQuad(*_tone);
}

void
PostProcessHDR::onActivate(RenderPipeline& pipeline) except
{
	std::uint32_t width, height;
	pipeline.getFramebufferSize(width, height);

	GraphicsTextureDesc samplerBloomDesc;
	samplerBloomDesc.setWidth(width / 2);
	samplerBloomDesc.setHeight(height / 2);
	samplerBloomDesc.setTexDim(GraphicsTextureDim::GraphicsTextureDim2D);
	samplerBloomDesc.setTexFormat(GraphicsFormat::GraphicsFormatR16G16B16SFloat);
	samplerBloomDesc.setMipNums(5);
	samplerBloomDesc.setSamplerWrap(GraphicsSamplerWrap::GraphicsSamplerWrapClampToEdge);
	samplerBloomDesc.setSamplerFilter(GraphicsSamplerFilter::GraphicsSamplerFilterLinearMipmapNearest, GraphicsSamplerFilter::GraphicsSamplerFilterLinear);
	_texBloomMap = pipeline.createTexture(samplerBloomDesc);
	_texBloomTempMap = pipeline.createTexture(samplerBloomDesc);

	GraphicsTextureDesc samplerLogDesc;
	samplerLogDesc.setWidth(256);
	samplerLogDesc.setHeight(256);
	samplerLogDesc.setTexDim(GraphicsTextureDim::GraphicsTextureDim2D);
	samplerLogDesc.setTexFormat(GraphicsFormat::GraphicsFormatR16SFloat);
	samplerLogDesc.setMipNums(9);
	samplerLogDesc.setSamplerWrap(GraphicsSamplerWrap::GraphicsSamplerWrapClampToEdge);
	samplerLogDesc.setSamplerFilter(GraphicsSamplerFilter::GraphicsSamplerFilterNearestMipmapNearest, GraphicsSamplerFilter::GraphicsSamplerFilterNearest);
	_texSampleLogMap = pipeline.createTexture(samplerLogDesc);

	GraphicsTextureDesc samplerLumDesc;
	samplerLumDesc.setWidth(1);
	samplerLumDesc.setHeight(1);
	samplerLumDesc.setTexDim(GraphicsTextureDim::GraphicsTextureDim2D);
	samplerLumDesc.setTexFormat(GraphicsFormat::GraphicsFormatR16SFloat);
	samplerLumDesc.setSamplerFilter(GraphicsSamplerFilter::GraphicsSamplerFilterNearest, GraphicsSamplerFilter::GraphicsSamplerFilterNearest);
	samplerLumDesc.setSamplerWrap(GraphicsSamplerWrap::GraphicsSamplerWrapClampToEdge);
	_texSampleLumMap = pipeline.createTexture(samplerLumDesc);

	GraphicsFramebufferLayoutDesc framebufferBloomLayoutDesc;
	framebufferBloomLayoutDesc.addComponent(GraphicsAttachmentLayout(0, GraphicsImageLayout::GraphicsImageLayoutColorAttachmentOptimal, GraphicsFormat::GraphicsFormatR16G16B16SFloat));
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

	_texBloomView.resize(5);
	_texBloomTempView.resize(5);

	for (std::uint8_t i = 0; i < 5; i++)
	{
		GraphicsFramebufferDesc bloomViewDesc;
		bloomViewDesc.setWidth(width / ((2 << i)));
		bloomViewDesc.setHeight(height / ((2 << i)));
		bloomViewDesc.addColorAttachment(GraphicsAttachmentBinding(_texBloomMap, i, 0));
		bloomViewDesc.setGraphicsFramebufferLayout(_sampleBloomImageLayout);
		_texBloomView[i] = pipeline.createFramebuffer(bloomViewDesc);

		GraphicsFramebufferDesc bloomTempViewDesc;
		bloomTempViewDesc.setWidth(width / ((2 << i)));
		bloomTempViewDesc.setHeight(height / ((2 << i)));
		bloomTempViewDesc.addColorAttachment(GraphicsAttachmentBinding(_texBloomTempMap, i, 0));
		bloomTempViewDesc.setGraphicsFramebufferLayout(_sampleBloomImageLayout);
		_texBloomTempView[i] = pipeline.createFramebuffer(bloomTempViewDesc);
	}

	_fimic = pipeline.createMaterial("sys:fx/PostProcessHDR.fxml");
	assert(_fimic);

	_sumLum = _fimic->getTech("SumLum");
	_sumLumLog = _fimic->getTech("SumLumLog");
	_avgLuminance = _fimic->getTech("AvgLuminance");
	_bloom = _fimic->getTech("GenerateBloom");
	_blur = _fimic->getTech("BlurBloom");
	_bloomCombine = _fimic->getTech("BloomCombine");
	_tone = _fimic->getTech("Tonemapping");

	_bloomThreshold = _fimic->getParameter("bloomThreshold");
	_bloomIntensity = _fimic->getParameter("bloomIntensity");
	_bloomFactors = _fimic->getParameter("bloomFactors");

	_toneBloom = _fimic->getParameter("texBloom");
	_toneLumExposure = _fimic->getParameter("exposure");

	_delta = _fimic->getParameter("delta");

	_texLumAve = _fimic->getParameter("texLumAve");
	_texSource = _fimic->getParameter("texSource");
	_texSourceSizeInv = _fimic->getParameter("texSourceSizeInv");
	_texSourceLevel = _fimic->getParameter("texSourceLevel");

	_texLumAve->uniformTexture(_texSampleLumMap);
	_toneLumExposure->uniform1f(_setting.exposure);

	float factors[] = { 0.341586,0.315658,0.249096,0.16786,0.096594 };
	_bloomFactors->uniform1fv(sizeof(factors) / sizeof(factors[0]), factors);

	_timer = std::make_shared<Timer>();
	_timer->open();

	this->setSetting(_setting);
}

void
PostProcessHDR::onDeactivate(RenderPipeline& pipeline) noexcept
{
	_timer.reset();
	_fimic.reset();
	_sumLum.reset();
	_sumLumLog.reset();
	_avgLuminance.reset();
	_bloom.reset();
	_blur.reset();
	_tone.reset();
	_bloomThreshold.reset();
	_bloomIntensity.reset();
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

	_texBloomView.clear();
	_texBloomTempView.clear();
	_texSampleLogView.reset();
	_texSampleLumView.reset();

	_texBloomMap.reset();
	_texBloomTempMap.reset();
	_texSampleLogMap.reset();
	_texSampleLumMap.reset();

	_sampleBloomImageLayout.reset();
	_sampleLogImageLayout.reset();
}

bool
PostProcessHDR::onRender(RenderPipeline& pipeline, RenderQueue queue, const GraphicsFramebufferPtr& source, const GraphicsFramebufferPtr& swap) noexcept
{
	if (queue != RenderQueue::RenderQueuePostprocess)
		return false;

	auto texture = source->getGraphicsFramebufferDesc().getColorAttachment(0).getBindingTexture();

	this->sumLum(pipeline, texture, _texBloomTempView[0], 0);

	if (_setting.enableEyeAdaptation)
	{
		this->sumLumLog(pipeline, _texBloomTempMap, _texSampleLogView);
		this->avgLuminance(pipeline, _texSampleLumMap, _texSampleLogMap, _texSampleLumView);
	}

	if (_setting.enableBloom)
	{
		this->doBloom(pipeline, _texBloomTempMap, _texBloomView[0]);

		for (std::uint8_t i = 0; i < 5; i++)
		{
			this->blurh(pipeline, _texBloomMap, _texBloomTempView[i], i);
			this->blurv(pipeline, _texBloomTempMap, _texBloomView[i], i);
			if (i < 4) this->sumLum(pipeline, _texBloomMap, _texBloomView[i + 1], i);
		}

		this->bloomCombine(pipeline, _texBloomMap, _texBloomTempView[0]);
	}

	this->doTonemapping(pipeline, _texBloomTempMap, texture, swap);

	return true;
}

_NAME_END