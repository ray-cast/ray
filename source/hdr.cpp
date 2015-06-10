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

_NAME_BEGIN

HDR::HDR() except
    : _lumAdapt(0.5)
    , _lumKey(1.0)
    , _lumMax(1.0)
    , _exposure(2.0)
    , _exposureBias(2.2)
    , _vignetteEnable(true)
    , _vignetteInner(0.7)
    , _vignetteOuter(1.7)
{
    _texSample4 = RenderTexture::create();
    _texSample4->setup(512, 512, TextureDim::DIM_2D, PixelFormat::R16G16B16F);

    _texSample8 = RenderTexture::create();
    _texSample8->setup(128, 128, TextureDim::DIM_2D, PixelFormat::R16G16B16F);

    _texSampleLog = RenderTexture::create();
    _texSampleLog->setup(32, 32, TextureDim::DIM_2D, PixelFormat::R16F);

    _texBloom = RenderTexture::create();
    _texBloom->setup(512, 512, TextureDim::DIM_2D, PixelFormat::R16G16B16F);

    _hdr = MaterialMaker("sys:fx/hdr.glsl");

    _sample4 = _hdr->getTech(RenderQueue::PostProcess)->getPass("sample");
    _sample8 = _hdr->getTech(RenderQueue::PostProcess)->getPass("sample");
    _samplelog = _hdr->getTech(RenderQueue::PostProcess)->getPass("samplelog");
    _bloom = _hdr->getTech(RenderQueue::PostProcess)->getPass("bloom");
    _blurh = _hdr->getTech(RenderQueue::PostProcess)->getPass("blurh");
    _blurv = _hdr->getTech(RenderQueue::PostProcess)->getPass("blurv");
    _tone = _hdr->getTech(RenderQueue::PostProcess)->getPass("tone");

    _toneSource = _hdr->getParameter("texSource");
    _toneBloom = _hdr->getParameter("texBloom");
    _toneLumAve = _hdr->getParameter("lumAve");
    _toneLumKey = _hdr->getParameter("lumKey");
    _toneLumMax = _hdr->getParameter("lumMax");
    _toneExposure = _hdr->getParameter("exposure");
    _toneExposureBias = _hdr->getParameter("exposureBias");
    _toneVignetteEnable = _hdr->getParameter("vignetteEnable");
    _toneVignetteInner = _hdr->getParameter("vignetteInner");
    _toneVignetteOuter = _hdr->getParameter("vignetteOuter");
}

HDR::~HDR() noexcept
{
}

void
HDR::measureLuminance(RenderPipeline* pipeline, RenderTexturePtr source)
{
    float delta = Material::getMaterialSemantic()->getFloatParam(GlobalFloatSemantic::TimeDelta);

    std::size_t count = source->getWidth() * source->getHeight();

    _data.resize(count);

    pipeline->readRenderTexture(source, PixelFormat::R16F, source->getWidth(), source->getHeight(), _data.data());

    float lum = 0;

    for (std::size_t i = 0; i < count; ++i)
    {
        lum += _data[i];
    }

    lum /= count;
    lum = exp(lum);

    _lumAdapt = _lumAdapt + ((lum - _lumAdapt) * (1.0f - pow(0.98f, 30.0f * delta)));
    _lumAdapt += 0.001f;
}

void
HDR::generateBloom(RenderPipeline* pipeline, RenderTexturePtr source, RenderTexturePtr dest)
{
    _toneSource->setTexture(source->getResolveTexture());
    _toneLumAve->assign(_lumAdapt);

    pipeline->setRenderTexture(dest);
    pipeline->setTechnique(_bloom);
    pipeline->drawSceneQuad();
}

void
HDR::sample4(RenderPipeline* pipeline, RenderTexturePtr source, RenderTexturePtr dest) noexcept
{
    _toneSource->setTexture(source->getResolveTexture());

    pipeline->setRenderTexture(dest);
    pipeline->setTechnique(_sample4);
    pipeline->drawSceneQuad();
}

void
HDR::sample8(RenderPipeline* pipeline, RenderTexturePtr source, RenderTexturePtr dest) noexcept
{
    _toneSource->setTexture(source->getResolveTexture());

    pipeline->setRenderTexture(dest);
    pipeline->setTechnique(_sample8);
    pipeline->drawSceneQuad();
}

void
HDR::sampleLog(RenderPipeline* pipeline, RenderTexturePtr source, RenderTexturePtr dest) noexcept
{
    _toneSource->setTexture(source->getResolveTexture());

    pipeline->setRenderTexture(dest);
    pipeline->setTechnique(_samplelog);
    pipeline->drawSceneQuad();
}

void
HDR::blurh(RenderPipeline* pipeline, RenderTexturePtr source, RenderTexturePtr dest) noexcept
{
    _toneSource->setTexture(source->getResolveTexture());

    pipeline->setRenderTexture(dest);
    pipeline->setTechnique(_blurh);
    pipeline->drawSceneQuad();
}

void
HDR::blurv(RenderPipeline* pipeline, RenderTexturePtr source, RenderTexturePtr dest) noexcept
{
    _toneSource->setTexture(source->getResolveTexture());

    pipeline->setRenderTexture(dest);
    pipeline->setTechnique(_blurv);
    pipeline->drawSceneQuad();
}

void
HDR::generateToneMapping(RenderPipeline* pipeline, RenderTexturePtr source, RenderTexturePtr bloom)
{
    _toneSource->setTexture(source->getResolveTexture());
    _toneBloom->setTexture(bloom->getResolveTexture());
    _toneLumAve->assign(_lumAdapt);
    _toneLumKey->assign(_lumKey);
    _toneLumMax->assign(_lumMax);
    _toneExposure->assign(_exposure);
    _toneExposureBias->assign(_exposureBias);
    _toneVignetteEnable->assign(_vignetteEnable);
    _toneVignetteInner->assign(_vignetteInner);
    _toneVignetteOuter->assign(_vignetteOuter);

    pipeline->setRenderTexture(source);
    pipeline->setTechnique(_tone);
    pipeline->drawSceneQuad();
}

void
HDR::render(RenderPipeline* pipeline, RenderTexturePtr source) noexcept
{
    this->sample4(pipeline, source, _texSample4);
    this->sample8(pipeline, _texSample4, _texSample8);
    this->sampleLog(pipeline, _texSample8, _texSampleLog);

    this->measureLuminance(pipeline, _texSampleLog);

    this->generateBloom(pipeline, _texSample4, _texBloom);

    this->blurh(pipeline, _texBloom, _texSample4);
    this->blurv(pipeline, _texSample4, _texBloom);

    this->generateToneMapping(pipeline, source, _texBloom);
}

_NAME_END