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
#include <ray/fimic.h>

#include <ray/graphics_framebuffer.h>
#include <ray/graphics_texture.h>

_NAME_BEGIN

float ToneExposure(float avgLum)
{
	return 3 / (std::max(0.1f, 1 + 10 * avgLum)) * 1.6;
}

FimicToneMapping::Setting::Setting() noexcept
	: bloomThreshold(0.6)
	, bloomIntensity(1.0)
	, lumKey(0.98)
	, lumDelta(50.0)
	, lumExposure(2.0)
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
	_bloomThreshold->assign(_setting.bloomThreshold);
	_bloomIntensity->assign(_setting.bloomIntensity);

	_toneLumExposure->assign(setting.lumExposure);

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

	pipeline.readRenderTexture(source, GraphicsFormat::GraphicsFormatR16SFloat, SAMPLE_LOG_SIZE, SAMPLE_LOG_SIZE, data);

	for (std::size_t i = 0; i < SAMPLE_LOG_COUNT; ++i)
		lum += data[i];

	lum /= SAMPLE_LOG_COUNT;
	lum = std::exp(lum);

	_lumAdapt = _lumAdapt + ((lum - _lumAdapt) * (1.0f - pow(_setting.lumKey, _setting.lumDelta * delta)));
	
	_toneLumExposure->assign(ToneExposure(_lumAdapt));
}

void
FimicToneMapping::sunLum(RenderPipeline& pipeline, GraphicsTexturePtr source, GraphicsFramebufferPtr dest) noexcept
{
	_toneSource->assign(source);

	pipeline.setRenderTexture(dest);
	pipeline.discradRenderTexture();
	pipeline.drawScreenQuad(_sunLum);
}

void
FimicToneMapping::sunLumLog(RenderPipeline& pipeline, GraphicsTexturePtr source, GraphicsFramebufferPtr dest) noexcept
{
	_toneSource->assign(source);

	pipeline.setRenderTexture(dest);
	pipeline.discradRenderTexture();
	pipeline.drawScreenQuad(_sunLumLog);
}

void
FimicToneMapping::generateBloom(RenderPipeline& pipeline, GraphicsTexturePtr source, GraphicsFramebufferPtr dest) noexcept
{
	_toneSource->assign(source);

	pipeline.setRenderTexture(dest);
	pipeline.discradRenderTexture();
	pipeline.drawScreenQuad(_bloom);
}

void
FimicToneMapping::blurh(RenderPipeline& pipeline, GraphicsTexturePtr source, GraphicsFramebufferPtr dest) noexcept
{
	auto& textureDesc = source->getGraphicsTextureDesc();
	_bloomTexSizeInv->assign(float2(1.0 / textureDesc.getWidth(), 1.0 / textureDesc.getHeight()));
	_toneSource->assign(source);

	pipeline.setRenderTexture(dest);
	pipeline.discradRenderTexture();
	pipeline.drawScreenQuad(_blurh);
}

void
FimicToneMapping::blurv(RenderPipeline& pipeline, GraphicsTexturePtr source, GraphicsFramebufferPtr dest) noexcept
{
	auto& textureDesc = source->getGraphicsTextureDesc();
	_bloomTexSizeInv->assign(float2(1.0 / textureDesc.getWidth(), 1.0 / textureDesc.getHeight()));
	_toneSource->assign(source);

	pipeline.setRenderTexture(dest);
	pipeline.discradRenderTexture();
	pipeline.drawScreenQuad(_blurv);
}

void
FimicToneMapping::generateToneMapping(RenderPipeline& pipeline, GraphicsTexturePtr bloom, GraphicsTexturePtr source, GraphicsFramebufferPtr dest) noexcept
{
	_toneSource->assign(source);
	_toneBloom->assign(bloom);

	pipeline.setRenderTexture(dest);
	pipeline.discradRenderTexture();
	pipeline.drawScreenQuad(_tone);
}

void
FimicToneMapping::onActivate(RenderPipeline& pipeline) except
{
	std::uint32_t width, height;
	pipeline.getWindowResolution(width, height);

	_texSample4Map = pipeline.createTexture(width / 4.0, height / 4.0, GraphicsTextureDim::GraphicsTextureDim2D, GraphicsFormat::GraphicsFormatR8G8B8A8UNorm);
	_texSample8Map = pipeline.createTexture(width / 8.0, height / 8.0, GraphicsTextureDim::GraphicsTextureDim2D, GraphicsFormat::GraphicsFormatR8G8B8A8UNorm);
	_texSampleLogMap = pipeline.createTexture(SAMPLE_LOG_SIZE, SAMPLE_LOG_SIZE, GraphicsTextureDim::GraphicsTextureDim2D, GraphicsFormat::GraphicsFormatR16SFloat);

	_texCombieMap = pipeline.createTexture(width / 4.0, height / 4.0, GraphicsTextureDim::GraphicsTextureDim2D, GraphicsFormat::GraphicsFormatR8G8B8A8UNorm);

	GraphicsFramebufferDesc sample4ViewDesc;
	sample4ViewDesc.attach(_texSample4Map);
	_texSample4View = pipeline.createRenderTexture(sample4ViewDesc);

	GraphicsFramebufferDesc sample8ViewDesc;
	sample8ViewDesc.attach(_texSample8Map);
	_texSample8View = pipeline.createRenderTexture(sample8ViewDesc);

	GraphicsFramebufferDesc sampleLogViewDesc;
	sampleLogViewDesc.attach(_texSampleLogMap);
	_texSampleLogView = pipeline.createRenderTexture(sampleLogViewDesc);

	GraphicsFramebufferDesc sampleCombieViewDesc;
	sampleCombieViewDesc.attach(_texCombieMap);
	_texCombieView = pipeline.createRenderTexture(sampleCombieViewDesc);

	_fimic = pipeline.createMaterial("sys:fx/fimic.fxml.o");

	_sunLum = _fimic->getTech(RenderQueue::RenderQueuePostprocess)->getPass("SumLum");
	_sunLumLog = _fimic->getTech(RenderQueue::RenderQueuePostprocess)->getPass("SumLumLog");
	_bloom = _fimic->getTech(RenderQueue::RenderQueuePostprocess)->getPass("GenerateBloom");
	_blurh = _fimic->getTech(RenderQueue::RenderQueuePostprocess)->getPass("BlurBloomh");
	_blurv = _fimic->getTech(RenderQueue::RenderQueuePostprocess)->getPass("BlurBloomv");
	_tone = _fimic->getTech(RenderQueue::RenderQueuePostprocess)->getPass("FimicTongMapping");

	_bloomThreshold = _fimic->getParameter("bloomThreshold");
	_bloomIntensity = _fimic->getParameter("bloomIntensity");
	_bloomTexSizeInv = _fimic->getParameter("bloomSizeInv");

	_toneSource = _fimic->getParameter("texSource");
	_toneBloom = _fimic->getParameter("texBloom");
	_toneLumExposure = _fimic->getParameter("exposure");

	_timer = std::make_shared<Timer>();

	this->setSetting(_setting);
}

void
FimicToneMapping::onDeactivate(RenderPipeline& pipeline) except
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

void
FimicToneMapping::onRender(RenderPipeline& pipeline, GraphicsTexturePtr source, GraphicsFramebufferPtr dest) noexcept
{
	this->sunLum(pipeline, source, _texSample4View);
	this->sunLum(pipeline, _texSample4Map, _texSample8View);
	this->sunLumLog(pipeline, _texSample8Map, _texSampleLogView);

	this->measureLuminance(pipeline, _texSampleLogView);

	this->generateBloom(pipeline, _texSample4Map, _texCombieView);

	this->blurh(pipeline, _texCombieMap, _texSample4View);
	this->blurv(pipeline, _texSample4Map, _texCombieView);

	this->generateToneMapping(pipeline, _texCombieMap, source, dest);
}

_NAME_END