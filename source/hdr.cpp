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
#include <ray/hdr.h>
#include <ray/material_maker.h>
#include <ray/render_factory.h>

_NAME_BEGIN

float ToneKey(float avg)
{
    return 1.03 - 2 / (2 + std::log10(avg + 1.0));
}

float ToneExposure(float avg)
{
    float guess = 1.5 - (1.5 / (avg * 0.1 + 1.0));
    return 1 / (std::max(0.0f, guess) + 0.2f);
}

#define SAMPLE_LOG_SIZE 32
#define SAMPLE_LOG_COUNT SAMPLE_LOG_SIZE * SAMPLE_LOG_SIZE

HDR::Setting::Setting() noexcept
    : bloomFactor(0.5)
    , bloomIntensity(1.0)
    , lumAve(1.0)
    , lumKey(1.0)
    , lumExposure(2.0)
{
}

HDR::HDR() noexcept
    : _lumAdapt(0)
{
}

HDR::~HDR() noexcept
{
}

void
HDR::onActivate() except
{
    _texSample4 = RenderFactory::createRenderTarget();
    _texSample4->setup(1376 / 4.0, 768 / 4.0, TextureDim::DIM_2D, PixelFormat::R8G8B8A8);

    _texSample8 = RenderFactory::createRenderTarget();
    _texSample8->setup(1376 / 8.0, 768 / 8.0, TextureDim::DIM_2D, PixelFormat::R8G8B8A8);

    _texSampleLog = RenderFactory::createRenderTarget();
    _texSampleLog->setup(SAMPLE_LOG_SIZE, SAMPLE_LOG_SIZE, TextureDim::DIM_2D, PixelFormat::R16F);

    _texBloom = RenderFactory::createRenderTarget();
    _texBloom->setup(1376 / 4.0, 768 / 4.0, TextureDim::DIM_2D, PixelFormat::R8G8B8A8);

    _hdr = MaterialMaker("sys:fx/hdr.glsl");

    _sample4 = _hdr->getTech(RenderQueue::PostProcess)->getPass("sample");
    _sample8 = _hdr->getTech(RenderQueue::PostProcess)->getPass("sample");
    _samplelog = _hdr->getTech(RenderQueue::PostProcess)->getPass("samplelog");
    _bloom = _hdr->getTech(RenderQueue::PostProcess)->getPass("bloom");
    _blurh = _hdr->getTech(RenderQueue::PostProcess)->getPass("blurh");
    _blurv = _hdr->getTech(RenderQueue::PostProcess)->getPass("blurv");
    _tone = _hdr->getTech(RenderQueue::PostProcess)->getPass("tone");

    _texSizeInv = _hdr->getParameter("texSizeInv");

    _bloomFactor = _hdr->getParameter("bloomFactor");
    _bloomIntensity = _hdr->getParameter("bloomIntensity");

    _toneSource = _hdr->getParameter("texSource");
    _toneBloom = _hdr->getParameter("texBloom");
    _toneLumAve = _hdr->getParameter("lumAve");
    _toneLumKey = _hdr->getParameter("lumKey");
    _toneLumExposure = _hdr->getParameter("lumExposure");

    _bloomFactor->assign(_setting.bloomFactor);
    _bloomIntensity->assign(_setting.bloomIntensity);
    _toneLumAve->assign(_setting.lumAve);
    _toneLumKey->assign(_setting.lumKey);
    _toneLumExposure->assign(_setting.lumExposure);
}

void
HDR::onDeactivate() except
{
}

void
HDR::setSetting(const Setting& setting) noexcept
{
    _bloomFactor->assign(_setting.bloomFactor);
    _bloomIntensity->assign(_setting.bloomIntensity);

    _toneLumAve->assign(setting.lumAve);
    _toneLumKey->assign(setting.lumKey);
    _toneLumExposure->assign(setting.lumExposure);

    _setting = setting;
}

const HDR::Setting&
HDR::getSetting() const noexcept
{
    return _setting;
}

void
HDR::measureLuminance(RenderPipeline& pipeline, RenderTargetPtr source) noexcept
{
    float lum = 0;
    float data[SAMPLE_LOG_COUNT];
    float delta = Material::getMaterialSemantic()->getFloatParam(GlobalFloatSemantic::TimeDelta);

    pipeline.readRenderTarget(source, PixelFormat::R16F, source->getWidth(), source->getHeight(), data);

    for (std::size_t i = 0; i < SAMPLE_LOG_COUNT; ++i)
    {
        lum += data[i];
    }

    lum /= SAMPLE_LOG_COUNT;
    lum = exp(lum);

    _lumAdapt = _lumAdapt + ((lum - _lumAdapt) * (1.0f - pow(0.98f, 30.0f * delta)));
    _lumAdapt += 0.001f;

    _toneLumAve->assign(_lumAdapt);
    _toneLumKey->assign(ToneKey(_lumAdapt));
    _toneLumExposure->assign(ToneExposure(_lumAdapt));
}

void
HDR::sample4(RenderPipeline& pipeline, RenderTargetPtr source, RenderTargetPtr dest) noexcept
{
    _toneSource->assign(source->getResolveTexture());

    pipeline.setRenderTarget(dest);
    pipeline.setTechnique(_sample4);
    pipeline.drawSceneQuad();
}

void
HDR::sample8(RenderPipeline& pipeline, RenderTargetPtr source, RenderTargetPtr dest) noexcept
{
    _toneSource->assign(source->getResolveTexture());

    pipeline.setRenderTarget(dest);
    pipeline.setTechnique(_sample8);
    pipeline.drawSceneQuad();
}

void
HDR::sampleLog(RenderPipeline& pipeline, RenderTargetPtr source, RenderTargetPtr dest) noexcept
{
    _toneSource->assign(source->getResolveTexture());

    pipeline.setRenderTarget(dest);
    pipeline.setTechnique(_samplelog);
    pipeline.drawSceneQuad();
}

void
HDR::generateBloom(RenderPipeline& pipeline, RenderTargetPtr source, RenderTargetPtr dest) noexcept
{
    _texBloom->setClearFlags(ClearFlags::CLEAR_COLOR);
    _toneSource->assign(source->getResolveTexture());

    pipeline.setRenderTarget(dest);
    pipeline.setTechnique(_bloom);
    pipeline.drawSceneQuad();

    _texBloom->setClearFlags(ClearFlags::CLEAR_NONE);

    this->blurh(pipeline, _texBloom);
    this->blurv(pipeline, _texBloom);
}

void
HDR::blurh(RenderPipeline& pipeline, RenderTargetPtr source) noexcept
{
    _texSizeInv->assign(float2(1.0 / source->getWidth(), 1.0 / source->getHeight()));
    _toneSource->assign(source->getResolveTexture());

    pipeline.setRenderTarget(source);
    pipeline.setTechnique(_blurh);
    pipeline.drawSceneQuad();
}

void
HDR::blurv(RenderPipeline& pipeline, RenderTargetPtr source) noexcept
{
    _texSizeInv->assign(float2(1.0 / source->getWidth(), 1.0 / source->getHeight()));
    _toneSource->assign(source->getResolveTexture());

    pipeline.setRenderTarget(source);
    pipeline.setTechnique(_blurv);
    pipeline.drawSceneQuad();
}

void
HDR::generateToneMapping(RenderPipeline& pipeline, RenderTargetPtr source, RenderTargetPtr dest) noexcept
{
    _toneSource->assign(dest->getResolveTexture());
    _toneBloom->assign(source->getResolveTexture());

    pipeline.setRenderTarget(dest);
    pipeline.setTechnique(_tone);
    pipeline.drawSceneQuad();
}

void
HDR::render(RenderPipeline& pipeline, RenderTargetPtr source) noexcept
{
    this->sample4(pipeline, source, _texSample4);
    this->sample8(pipeline, _texSample4, _texSample8);
    this->sampleLog(pipeline, _texSample8, _texSampleLog);

    this->measureLuminance(pipeline, _texSampleLog);

    this->generateBloom(pipeline, _texSample4, _texBloom);

    this->generateToneMapping(pipeline, _texBloom, source);
}

_NAME_END