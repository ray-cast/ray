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
#include <ray/camera.h>
#include <ray/render_factory.h>
#include <ray/texture.h>

_NAME_BEGIN

SSAO::SSAO() noexcept
{
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
    _setting.blur = set.blur;
    _setting.blurRadius = set.blurRadius;
    _setting.blurScale = set.blurScale;
    _setting.blurSharpness = set.blurSharpness;

    _radius->assign(_setting.radius);
    _radius2->assign(_setting.radius * _setting.radius);

    _bias->assign(_setting.bias);
    _intensity->assign(_setting.intensity);

    const float blurSigma = _setting.blurRadius * 0.5;
    const float blurFalloff = 1.0 / (2.0 * blurSigma * blurSigma);

    _blurRadius->assign(_setting.blurRadius);
    _blurFactor->assign(blurFalloff);
    _blurSharpness->assign(_setting.blurSharpness);
}

const SSAO::Setting&
SSAO::getSetting() const noexcept
{
    return _setting;
}

void
SSAO::computeRawAO(RenderPipeline& pipeline, RenderTargetPtr dest) noexcept
{
    _projInfo->assign(pipeline.getCamera()->getProjConstant());
    _projScale->assign(pipeline.getCamera()->getProjLength().y * _setting.radius);
    _clipInfo->assign(pipeline.getCamera()->getClipConstant());
    _texSize->assign(float2(dest->getWidth(), dest->getHeight()));

    pipeline.setRenderTarget(dest);
    pipeline.setTechnique(_ambientOcclusionPass);
    pipeline.drawSceneQuad();
}

void
SSAO::blurHorizontal(RenderPipeline& pipeline, RenderTargetPtr source, RenderTargetPtr dest) noexcept
{
    float2 direction(2.0, 0.0f);
    direction.x /= source->getWidth();

    this->blurDirection(pipeline, source, dest, direction);
}

void
SSAO::blurVertical(RenderPipeline& pipeline, RenderTargetPtr source, RenderTargetPtr dest) noexcept
{
    float2 direction(0.0f, 2.0);
    direction.y /= source->getHeight();

    this->blurDirection(pipeline, source, dest, direction);
}

void
SSAO::blurDirection(RenderPipeline& pipeline, RenderTargetPtr source, RenderTargetPtr dest, const float2& direction) noexcept
{
    _blurDirection->assign(direction);
    _blurSource->assign(source->getResolveTexture());

    pipeline.setRenderTarget(dest);
    pipeline.setTechnique(_ambientOcclusionBlurPass);
    pipeline.drawSceneQuad();
}

void
SSAO::shading(RenderPipeline& pipeline, RenderTargetPtr source, RenderTargetPtr ao) noexcept
{
    _copyAmbient->assign(ao->getResolveTexture());

    pipeline.setRenderTarget(source);
    pipeline.setTechnique(_ambientOcclusionCopyPass);
    pipeline.drawSceneQuad();
}

void
SSAO::onActivate(RenderPipeline& pipeline) except
{
    std::size_t width = pipeline.getWindowWidth();
    std::size_t height = pipeline.getWindowHeight();

    _texAmbient = RenderFactory::createRenderTarget();
    _texAmbient->setup(width, height, TextureDim::DIM_2D, PixelFormat::R16G16B16F);

    _texBlur = RenderFactory::createRenderTarget();
    _texBlur->setup(width, height, TextureDim::DIM_2D, PixelFormat::R16F);

    _ambientOcclusion = MaterialMaker("sys:fx\\ssao.glsl");
    _ambientOcclusionPass = _ambientOcclusion->getTech(RenderQueue::PostProcess)->getPass("ao");
    _ambientOcclusionBlurPass = _ambientOcclusion->getTech(RenderQueue::PostProcess)->getPass("blur");
    _ambientOcclusionCopyPass = _ambientOcclusion->getTech(RenderQueue::PostProcess)->getPass("copy");

    _radius = _ambientOcclusion->getParameter("radius");
    _radius2 = _ambientOcclusion->getParameter("radius2");
    _projScale = _ambientOcclusion->getParameter("projScale");
    _projInfo = _ambientOcclusion->getParameter("projInfo");
    _clipInfo = _ambientOcclusion->getParameter("clipInfo");
    _bias = _ambientOcclusion->getParameter("bias");
    _intensity = _ambientOcclusion->getParameter("intensity");
    _texSize = _ambientOcclusion->getParameter("texSize");

    _blurSource = _ambientOcclusion->getParameter("texSource");
    _blurFactor = _ambientOcclusion->getParameter("blurFactor");
    _blurSharpness = _ambientOcclusion->getParameter("blurSharpness");
    _blurRadius = _ambientOcclusion->getParameter("blurRadius");
    _blurDirection = _ambientOcclusion->getParameter("blurDirection");
    _blurGaussian = _ambientOcclusion->getParameter("blurGaussian");

    _copyAmbient = _ambientOcclusion->getParameter("texAO");

    Setting setting;
    setting.radius = 1.0;
    setting.bias = 0.002;
    setting.intensity = 2;

    setting.blur = true;
    setting.blurRadius = 6;
    setting.blurScale = 2.5;
    setting.blurSharpness = 2;

    this->setSetting(setting);
}

void
SSAO::onDeactivate(RenderPipeline& pipeline) except
{
}

void
SSAO::onRender(RenderPipeline& pipeline, RenderTargetPtr source) except
{
    this->computeRawAO(pipeline, _texAmbient);

    if (_setting.blur)
    {
        this->blurHorizontal(pipeline, _texAmbient, _texBlur);
        this->blurVertical(pipeline, _texBlur, _texAmbient);
    }

    this->shading(pipeline, source, _texAmbient);

    //pipeline.copyRenderTarget(_texAmbient, Viewport(0, 0, 1376, 768), source, Viewport(0, 0, 1376, 768));
    //pipeline.copyRenderTarget(dest, Viewport(0, 0, 1376, 768), 0, Viewport(0, 0, 1376, 768));
}

_NAME_END