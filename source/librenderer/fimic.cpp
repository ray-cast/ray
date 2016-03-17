// +----------------------------------------------------------------------
// | Project : ray.
// | All rights reserved.
// +----------------------------------------------------------------------
// | Copyright (c) 2013-2015.
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
FimicToneMapping::measureLuminance(RenderPipeline& pipeline, GraphicsRenderTexturePtr source) noexcept
{
	_timer->update();

	float lum = 0;
	float data[SAMPLE_LOG_COUNT];
	float delta = _timer->delta();

	auto& textureDesc = source->getResolveTexture()->getGraphicsTextureDesc();
	pipeline.readRenderTexture(source, GraphicsFormat::GraphicsFormatR16SFloat, textureDesc.getWidth(), textureDesc.getHeight(), data);

	for (std::size_t i = 0; i < SAMPLE_LOG_COUNT; ++i)
		lum += data[i];

	lum /= SAMPLE_LOG_COUNT;
	lum = std::exp(lum);

	_lumAdapt = _lumAdapt + ((lum - _lumAdapt) * (1.0f - pow(_setting.lumKey, _setting.lumDelta * delta)));
	
	_toneLumExposure->assign(ToneExposure(_lumAdapt));
}

void
FimicToneMapping::sunLum(RenderPipeline& pipeline, GraphicsRenderTexturePtr source, GraphicsRenderTexturePtr dest) noexcept
{
	_toneSource->assign(source->getResolveTexture());

	pipeline.setRenderTexture(dest);
	pipeline.discradRenderTexture();
	pipeline.drawScreenQuad(_sunLum);
}

void
FimicToneMapping::sunLumLog(RenderPipeline& pipeline, GraphicsRenderTexturePtr source, GraphicsRenderTexturePtr dest) noexcept
{
	_toneSource->assign(source->getResolveTexture());

	pipeline.setRenderTexture(dest);
	pipeline.discradRenderTexture();
	pipeline.drawScreenQuad(_sunLumLog);
}

void
FimicToneMapping::generateBloom(RenderPipeline& pipeline, GraphicsRenderTexturePtr source, GraphicsRenderTexturePtr dest) noexcept
{
	_toneSource->assign(source->getResolveTexture());

	pipeline.setRenderTexture(dest);
	pipeline.discradRenderTexture();
	pipeline.drawScreenQuad(_bloom);
}

void
FimicToneMapping::blurh(RenderPipeline& pipeline, GraphicsRenderTexturePtr source, GraphicsRenderTexturePtr dest) noexcept
{
	auto& textureDesc = source->getResolveTexture()->getGraphicsTextureDesc();
	_bloomTexSizeInv->assign(float2(1.0 / textureDesc.getWidth(), 1.0 / textureDesc.getHeight()));
	_toneSource->assign(source->getResolveTexture());

	pipeline.setRenderTexture(dest);
	pipeline.discradRenderTexture();
	pipeline.drawScreenQuad(_blurh);
}

void
FimicToneMapping::blurv(RenderPipeline& pipeline, GraphicsRenderTexturePtr source, GraphicsRenderTexturePtr dest) noexcept
{
	auto& textureDesc = source->getResolveTexture()->getGraphicsTextureDesc();
	_bloomTexSizeInv->assign(float2(1.0 / textureDesc.getWidth(), 1.0 / textureDesc.getHeight()));
	_toneSource->assign(source->getResolveTexture());

	pipeline.setRenderTexture(dest);
	pipeline.discradRenderTexture();
	pipeline.drawScreenQuad(_blurv);
}

void
FimicToneMapping::generateToneMapping(RenderPipeline& pipeline, GraphicsRenderTexturePtr bloom, GraphicsRenderTexturePtr source, GraphicsRenderTexturePtr dest) noexcept
{
	_toneSource->assign(source->getResolveTexture());
	_toneBloom->assign(bloom->getResolveTexture());

	pipeline.setRenderTexture(dest);
	pipeline.discradRenderTexture();
	pipeline.drawScreenQuad(_tone);
}

void
FimicToneMapping::onActivate(RenderPipeline& pipeline) except
{
	std::uint32_t width, height;
	pipeline.getWindowResolution(width, height);

	_texSample4 = pipeline.createRenderTexture(width / 4.0, height / 4.0, GraphicsTextureDim::GraphicsTextureDim2D, GraphicsFormat::GraphicsFormatR8G8B8A8UNorm);
	_texSample8 = pipeline.createRenderTexture(width / 8.0, height / 8.0, GraphicsTextureDim::GraphicsTextureDim2D, GraphicsFormat::GraphicsFormatR8G8B8A8UNorm);
	_texCombie = pipeline.createRenderTexture(width / 4.0, height / 4.0, GraphicsTextureDim::GraphicsTextureDim2D, GraphicsFormat::GraphicsFormatR8G8B8A8UNorm);

	_texSampleLog = pipeline.createRenderTexture(SAMPLE_LOG_SIZE, SAMPLE_LOG_SIZE, GraphicsTextureDim::GraphicsTextureDim2D, GraphicsFormat::GraphicsFormatR16SFloat);

	_fimic = pipeline.createMaterial("sys:fx/fimic.fxml.o");

	_sunLum = _fimic->getTech(RenderQueue::RQ_POSTPROCESS)->getPass("SumLum");
	_sunLumLog = _fimic->getTech(RenderQueue::RQ_POSTPROCESS)->getPass("SumLumLog");
	_bloom = _fimic->getTech(RenderQueue::RQ_POSTPROCESS)->getPass("GenerateBloom");
	_blurh = _fimic->getTech(RenderQueue::RQ_POSTPROCESS)->getPass("BlurBloomh");
	_blurv = _fimic->getTech(RenderQueue::RQ_POSTPROCESS)->getPass("BlurBloomv");
	_tone = _fimic->getTech(RenderQueue::RQ_POSTPROCESS)->getPass("FimicTongMapping");

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
	if (_texSample4)
	{
		_texSample4.reset();
		_texSample4 = nullptr;
	}

	if (_texSample8)
	{
		_texSample8.reset();
		_texSample8 = nullptr;
	}

	if (_texSampleLog)
	{
		_texSampleLog.reset();
		_texSampleLog = nullptr;
	}

	if (_texCombie)
	{
		_texCombie.reset();
		_texCombie = nullptr;
	}
}

void
FimicToneMapping::onRender(RenderPipeline& pipeline, GraphicsRenderTexturePtr source, GraphicsRenderTexturePtr dest) noexcept
{
	this->sunLum(pipeline, source, _texSample4);
	this->sunLum(pipeline, _texSample4, _texSample8);
	this->sunLumLog(pipeline, _texSample8, _texSampleLog);

	this->measureLuminance(pipeline, _texSampleLog);

	this->generateBloom(pipeline, _texSample4, _texCombie);

	this->blurh(pipeline, _texCombie, _texSample4);
	this->blurv(pipeline, _texSample4, _texCombie);

	this->generateToneMapping(pipeline, _texCombie, source, dest);
}

_NAME_END