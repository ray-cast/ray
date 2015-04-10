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
// +----------------------------------------------------------------------
#include <ray/ssao.h>
#include <ray/material_maker.h>
#include <ray/renderer.h>
#include <ray/camera.h>
#include <ray/render_pipeline.h>

_NAME_BEGIN

SSAO::SSAO() noexcept
{
    _texAmbient = std::make_shared<RenderTexture>();
    _texAmbient->setup(1376, 768, TextureDim::DIM_2D, PixelFormat::R16F);

    _texBlur = std::make_shared<RenderTexture>();
    _texBlur->setup(1376, 768, TextureDim::DIM_2D, PixelFormat::R16F);

    _ambientOcclusion = MaterialMaker("sys:fx\\ssao.glsl");
    _ambientOcclusionPass = _ambientOcclusion->getTech(RenderQueue::PostProcess)->getPass("ao");
    _ambientOcclusionBlurPass = _ambientOcclusion->getTech(RenderQueue::PostProcess)->getPass("blur");
    _ambientOcclusionCopyPass = _ambientOcclusion->getTech(RenderQueue::PostProcess)->getPass("copy");

    _radius = _ambientOcclusion->getParameter("radius");
    _projScale = _ambientOcclusion->getParameter("projScale");
    _projInfo = _ambientOcclusion->getParameter("projInfo");
    _clipInfo = _ambientOcclusion->getParameter("clipInfo");
    _bias = _ambientOcclusion->getParameter("bias");
    _epsilon = _ambientOcclusion->getParameter("epsilon");
    _intensityDivR6 = _ambientOcclusion->getParameter("intensityDivR6");
    _numSample = _ambientOcclusion->getParameter("numSample");

    _blurTexSource = _ambientOcclusion->getParameter("texSource");
    _blurFactor = _ambientOcclusion->getParameter("blurFactor");
    _blurSharpness = _ambientOcclusion->getParameter("blurSharpness");
    _blurRadius = _ambientOcclusion->getParameter("blurRadius");
    _blurDirection = _ambientOcclusion->getParameter("blurDirection");
    _blurGaussian = _ambientOcclusion->getParameter("blurGaussian");

    _copyAmbient = _ambientOcclusion->getParameter("texAO");
    _copyColor = _ambientOcclusion->getParameter("texColor");

    Setting setting;
    setting.radius = 1;
    setting.bias = 0.05;
    setting.epsilon = 0.01;
    setting.intensity = 4.0;
    setting.numSample = 10;

    setting.blur = true;
    setting.blurRadius = 8;
    setting.blurSharpness = 4;

    this->setSetting(setting);
}

SSAO::~SSAO() noexcept
{
}

void
SSAO::setSetting(const Setting& set) noexcept
{
    _setting.radius = set.radius;
    _setting.bias = set.bias;
    _setting.intensity = set.intensity;
    _setting.epsilon = set.epsilon;
    _setting.numSample = set.numSample;
    _setting.blur = set.blur;
    _setting.blurRadius = set.blurRadius;
    _setting.blurSharpness = set.blurSharpness;

    _radius->assign(_setting.radius);
    _bias->assign(_setting.bias);
    _epsilon->assign(_setting.epsilon);
    _intensityDivR6->assign(_setting.intensity / pow(_setting.radius, 6.0f));
    _numSample->assign(_setting.numSample);
    _blurRadius->assign(_setting.blurRadius);
    _blurFactor->assign((float)1.0 / (2 * ((_setting.blurRadius + 1) / 2) * ((_setting.blurRadius + 1) / 2)));
    _blurSharpness->assign(_setting.blurSharpness);
}

const SSAO::Setting&
SSAO::getSetting() const noexcept
{
    return _setting;
}

void
SSAO::computeRawAO(RenderPipeline* pipeline, RenderTexturePtr dest) noexcept
{
    _projInfo->assign(pipeline->camera->getProjConstant());
    _projScale->assign(pipeline->camera->getProjLength().y);
    _clipInfo->assign(pipeline->camera->getClipConstant());

    pipeline->setRenderTexture(dest);
    pipeline->setTechnique(_ambientOcclusionPass);
    pipeline->drawSceneQuad();
}

void
SSAO::blurHorizontal(RenderPipeline* pipeline, RenderTexturePtr source, RenderTexturePtr dest) noexcept
{
    int2 direction(2, 0);
    this->blurDirection(pipeline, source, dest, direction);
}

void
SSAO::blurVertical(RenderPipeline* pipeline, RenderTexturePtr source, RenderTexturePtr dest) noexcept
{
    int2 direction(0, 2);
    this->blurDirection(pipeline, source, dest, direction);
}

void
SSAO::blurDirection(RenderPipeline* pipeline, RenderTexturePtr source, RenderTexturePtr dest, const int2& direction) noexcept
{
    _blurDirection->assign(direction);
    _blurTexSource->setTexture(source->getResolveTexture());

    pipeline->setRenderTexture(dest);
    pipeline->setTechnique(_ambientOcclusionBlurPass);
    pipeline->drawSceneQuad();
}

void
SSAO::shading(RenderPipeline* pipeline, RenderTexturePtr color, RenderTexturePtr ao, RenderTexturePtr dest) noexcept
{
    _copyColor->setTexture(color->getResolveTexture());
    _copyAmbient->setTexture(ao->getResolveTexture());

    pipeline->setRenderTexture(dest);
    pipeline->setTechnique(_ambientOcclusionCopyPass);
    pipeline->drawSceneQuad();
}

void
SSAO::render(RenderPipeline* pipeline, RenderTexturePtr source, RenderTexturePtr dest) noexcept
{
    this->computeRawAO(pipeline, _texAmbient);

    if (_setting.blur)
    {
        this->blurHorizontal(pipeline, _texAmbient, _texBlur);
        this->blurVertical(pipeline, _texBlur, _texAmbient);
    }

    this->shading(pipeline, source, _texAmbient, dest);

    //pipeline->copyRenderTexture(_texAmbient, Viewport(0, 0, 1376, 768), 0, Viewport(1376 / 2, 0, 1376, 768 / 2));
    //pipeline->copyRenderTexture(dest, Viewport(0, 0, 1376, 768), 0, Viewport(0, 0, 1376, 768));
}

_NAME_END