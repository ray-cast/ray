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

#define SAMPLE_LOG_SIZE 32
#define SAMPLE_LOG_COUNT SAMPLE_LOG_SIZE * SAMPLE_LOG_SIZE

float ToneKey(float avgLum)
{
	return 1.03 - 2 / (2 + std::log10(avgLum + 1.0));
}

float ToneExposure(float avgLum)
{
	float guess = 1.5 - (1.5 / (avgLum * 0.1 + 1.0));
	return 1.0 / (std::max(0.0f, guess) + 0.2f);
}

FimicToneMapping::Setting::Setting() noexcept
	: bloomThreshold(0.63)
	, bloomIntensity(1.0)
	, lumAve(1.0)
	, lumKey(0.98)
	, lumDelta(30.0)
	, lumExposure(2.0)
	, burnout(std::numeric_limits<float>::infinity())
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

	_toneLumAve->assign(setting.lumAve);
	_toneLumKey->assign(setting.lumKey);
	_toneLumExposure->assign(setting.lumExposure);

	_toneBurnout->assign(setting.burnout);

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
	pipeline.readRenderTexture(source, TextureFormat::R16F, textureDesc.getWidth(), textureDesc.getHeight(), data);

	for (std::size_t i = 0; i < SAMPLE_LOG_COUNT; ++i)
	{
		lum += data[i];
	}

	lum /= SAMPLE_LOG_COUNT;
	lum = exp(lum);

	_lumAdapt = _lumAdapt + ((lum - _lumAdapt) * (1.0f - pow(_setting.lumKey, _setting.lumDelta * delta)));
	_lumAdapt += 0.001f;

	_toneLumAve->assign(_lumAdapt);
	_toneLumKey->assign(ToneKey(_lumAdapt));
	_toneLumExposure->assign(ToneExposure(_lumAdapt));
}

void
FimicToneMapping::sample4(RenderPipeline& pipeline, GraphicsRenderTexturePtr source, GraphicsRenderTexturePtr dest) noexcept
{
	_toneSource->assign(source->getResolveTexture());

	pipeline.setRenderTexture(dest);
	pipeline.clearRenderTexture(ClearFlags::Color, Vector4::Zero, 1.0, 0);

	pipeline.drawScreenQuad(_sample4);
}

void
FimicToneMapping::sample8(RenderPipeline& pipeline, GraphicsRenderTexturePtr source, GraphicsRenderTexturePtr dest) noexcept
{
	_toneSource->assign(source->getResolveTexture());

	pipeline.setRenderTexture(dest);
	pipeline.clearRenderTexture(ClearFlags::Color, Vector4::Zero, 1.0, 0);

	pipeline.drawScreenQuad(_sample8);
}

void
FimicToneMapping::sampleLog(RenderPipeline& pipeline, GraphicsRenderTexturePtr source, GraphicsRenderTexturePtr dest) noexcept
{
	_toneSource->assign(source->getResolveTexture());

	pipeline.setRenderTexture(dest);
	pipeline.clearRenderTexture(ClearFlags::Color, Vector4::Zero, 1.0, 0);

	pipeline.drawScreenQuad(_samplelog);
}

void
FimicToneMapping::generateBloom(RenderPipeline& pipeline, GraphicsRenderTexturePtr source, GraphicsRenderTexturePtr dest) noexcept
{
	_toneSource->assign(source->getResolveTexture());

	pipeline.setRenderTexture(dest);
	pipeline.clearRenderTexture(ClearFlags::Color, Vector4::Zero, 1.0, 0);

	pipeline.drawScreenQuad(_bloom);
}

void
FimicToneMapping::blurh(RenderPipeline& pipeline, GraphicsRenderTexturePtr source, GraphicsRenderTexturePtr dest) noexcept
{
	auto& textureDesc = source->getResolveTexture()->getGraphicsTextureDesc();
	_bloomTexSizeInv->assign(float2(1.0 / textureDesc.getWidth(), 1.0 / textureDesc.getHeight()));
	_toneSource->assign(source->getResolveTexture());

	pipeline.setRenderTexture(dest);
	pipeline.drawScreenQuad(_blurh);
}

void
FimicToneMapping::blurv(RenderPipeline& pipeline, GraphicsRenderTexturePtr source, GraphicsRenderTexturePtr dest) noexcept
{
	auto& textureDesc = source->getResolveTexture()->getGraphicsTextureDesc();
	_bloomTexSizeInv->assign(float2(1.0 / textureDesc.getWidth(), 1.0 / textureDesc.getHeight()));
	_toneSource->assign(source->getResolveTexture());

	pipeline.setRenderTexture(dest);
	pipeline.drawScreenQuad(_blurv);
}

void
FimicToneMapping::generateToneMapping(RenderPipeline& pipeline, GraphicsRenderTexturePtr source, GraphicsRenderTexturePtr dest) noexcept
{
	_toneSource->assign(dest->getResolveTexture());
	_toneBloom->assign(source->getResolveTexture());

	pipeline.setRenderTexture(dest);
	pipeline.drawScreenQuad(_tone);
}

void
FimicToneMapping::onActivate(RenderPipeline& pipeline) except
{
	std::uint32_t width, height;
	pipeline.getWindowResolution(width, height);

	_texSample4 = pipeline.createRenderTexture(width / 4.0, height / 4.0, TextureDim::DIM_2D, TextureFormat::R8G8B8);
	_texSample8 = pipeline.createRenderTexture(width / 8.0, height / 8.0, TextureDim::DIM_2D, TextureFormat::R8G8B8);
	_texSampleLog = pipeline.createRenderTexture(SAMPLE_LOG_SIZE, SAMPLE_LOG_SIZE, TextureDim::DIM_2D, TextureFormat::R16F);
	_texBloom = pipeline.createRenderTexture(width / 4.0, height / 4.0, TextureDim::DIM_2D, TextureFormat::R8G8B8);

	_fimic = pipeline.createMaterial("sys:fx/fimic.glsl");

	_sample4 = _fimic->getTech(RenderQueue::RQ_POSTPROCESS)->getPass("Sample");
	_sample8 = _fimic->getTech(RenderQueue::RQ_POSTPROCESS)->getPass("Sample");
	_samplelog = _fimic->getTech(RenderQueue::RQ_POSTPROCESS)->getPass("SampleLog");
	_bloom = _fimic->getTech(RenderQueue::RQ_POSTPROCESS)->getPass("GenerateBloom");
	_blurh = _fimic->getTech(RenderQueue::RQ_POSTPROCESS)->getPass("BlurBloomh");
	_blurv = _fimic->getTech(RenderQueue::RQ_POSTPROCESS)->getPass("BlurBloomv");
	_tone = _fimic->getTech(RenderQueue::RQ_POSTPROCESS)->getPass("FimicTongMapping");

	_bloomThreshold = _fimic->getParameter("bloomThreshold");
	_bloomIntensity = _fimic->getParameter("bloomIntensity");
	_bloomTexSizeInv = _fimic->getParameter("bloomSizeInv");

	_toneSource = _fimic->getParameter("texSource");
	_toneBloom = _fimic->getParameter("texBloom");
	_toneLumAve = _fimic->getParameter("lumAve");
	_toneLumKey = _fimic->getParameter("lumKey");
	_toneLumExposure = _fimic->getParameter("exposure");
	_toneBurnout = _fimic->getParameter("burnout");

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

	if (_texBloom)
	{
		_texBloom.reset();
		_texBloom = nullptr;
	}
}

void
FimicToneMapping::onRender(RenderPipeline& pipeline, GraphicsRenderTexturePtr source) noexcept
{
	this->sample4(pipeline, source, _texSample4);
	this->sample8(pipeline, _texSample4, _texSample8);
	this->sampleLog(pipeline, source, _texSampleLog);

	this->measureLuminance(pipeline, _texSampleLog);

	this->generateBloom(pipeline, _texSample4, _texBloom);

	this->blurh(pipeline, _texBloom, _texSample4);
	this->blurv(pipeline, _texSample4, _texBloom);

	this->generateToneMapping(pipeline, _texBloom, source);
}

_NAME_END