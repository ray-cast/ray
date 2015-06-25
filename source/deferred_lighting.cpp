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
#include <ray/deferred_lighting.h>
#include <ray/camera.h>
#include <ray/light.h>
#include <ray/material_maker.h>

_NAME_BEGIN

const float ESM_FACTOR = 0.5f;

DeferredLighting::DeferredLighting() noexcept
    : _deferredGraphicMap(nullptr)
    , _deferredNormalMap(nullptr)
    , _deferredLightMap(nullptr)
    , _deferredGraphicMaps(nullptr)
{
}

DeferredLighting::~DeferredLighting() noexcept
{
}

void
DeferredLighting::onActivate() except
{
    _deferredLighting = MaterialMaker("sys:fx\\deferred_lighting.glsl");
    _deferredDepthOhly = _deferredLighting->getTech(RenderQueue::DeferredLight)->getPass("DeferredDepthOhly");
    _deferredPointLight = _deferredLighting->getTech(RenderQueue::DeferredLight)->getPass("DeferredPointLight");
    _deferredSunLight = _deferredLighting->getTech(RenderQueue::DeferredLight)->getPass("DeferredSunLight");
    _deferredSunLightShadow = _deferredLighting->getTech(RenderQueue::DeferredLight)->getPass("DeferredSunLightShadow");
    _deferredSpotLight = _deferredLighting->getTech(RenderQueue::DeferredLight)->getPass("DeferredSpotLight");
    _deferredShadingOpaques = _deferredLighting->getTech(RenderQueue::DeferredLight)->getPass("DeferredShadingOpaques");
    _deferredShadingTransparents = _deferredLighting->getTech(RenderQueue::DeferredLight)->getPass("DeferredShadingTransparents");

    _texDepth = _deferredLighting->getParameter("texDepth");
    _texDiffuse = _deferredLighting->getParameter("texDiffuse");
    _texNormal = _deferredLighting->getParameter("texNormal");
    _texLight = _deferredLighting->getParameter("texLight");

    _shadowChannel = _deferredLighting->getParameter("shadowChannel");
    _shadowMap = _deferredLighting->getParameter("shadowMap");
    _shadowLitFactor = _deferredLighting->getParameter("shadowFactor");
    _shadowMatrix = _deferredLighting->getParameter("shadowMatrix");

    _shadow = MaterialMaker("sys:fx\\shadow.glsl");
    _shadowGenerate = _shadow->getTech(RenderQueue::Shadow)->getPass("shadow");
    _shadowBlurX = _shadow->getTech(RenderQueue::Shadow)->getPass("blurX");
    _shadowBlurY = _shadow->getTech(RenderQueue::Shadow)->getPass("blurY");
    _shadowDecal = _shadow->getParameter("decal");
    _shadowFactor = _shadow->getParameter("factor");

    _shadowFactor->assign(ESM_FACTOR);
    _shadowLitFactor->assign(ESM_FACTOR);
}

void
DeferredLighting::onDectivate() noexcept
{
}

void
DeferredLighting::onRenderPre() noexcept
{
    _deferredDepthMap = this->getCamera()->getDeferredDepthMap();
    _deferredGraphicMap = this->getCamera()->getDeferredGraphicMap();
    _deferredNormalMap = this->getCamera()->getDeferredNormalMap();
    _deferredLightMap = this->getCamera()->getDeferredLightMap();
    _deferredShadingMap = this->getCamera()->getRenderTarget();
    _deferredGraphicMaps = this->getCamera()->getDeferredGraphicsMaps();

    if (_deferredLightMap)
        _deferredLightMap->setClearFlags(ClearFlags::CLEAR_COLOR);

    auto semantic = Material::getMaterialSemantic();
    if (_deferredDepthMap)
        semantic->setTexParam(GlobalTexSemantic::DeferredDepthMap, _deferredDepthMap->getResolveTexture());
    if (_deferredNormalMap)
        semantic->setTexParam(GlobalTexSemantic::DeferredNormalMap, _deferredNormalMap->getResolveTexture());
    if (_deferredGraphicMap)
        semantic->setTexParam(GlobalTexSemantic::DeferredGraphicMap, _deferredGraphicMap->getResolveTexture());
    if (_deferredLightMap)
        semantic->setTexParam(GlobalTexSemantic::DeferredLightMap, _deferredLightMap->getResolveTexture());
    if (_deferredDepthMap)
        semantic->setTexParam(GlobalTexSemantic::DepthMap, _deferredDepthMap->getResolveTexture());
    if (_deferredShadingMap)
        semantic->setTexParam(GlobalTexSemantic::ColorMap, _deferredShadingMap->getResolveTexture());
    if (_deferredNormalMap)
        semantic->setTexParam(GlobalTexSemantic::NormalMap, _deferredNormalMap->getResolveTexture());
}

void
DeferredLighting::onRenderPipeline() noexcept
{
    CameraOrder mode = this->getCamera()->getCameraOrder();
    switch (mode)
    {
    case CO_SHADOW:
    {
        this->renderShadow();
    }
    break;
    case CO_DEPTH_OPAQUES:
    {
        this->renderOpaquesDepthOhly();
    }
    break;
    case CO_DEPTH_TRANSPARENT:
    {
        this->renderTransparentDepthOhly();
    }
    break;
    case CO_LIGHT:
    {
        this->renderLights();
    }
    break;
    case CO_MAIN:
    {
        auto renderMode = this->getCamera()->getCameraRender();
        if (renderMode == CameraRender::CR_RENDER_TO_TEXTURE)
        {
            this->renderOpaquesDepthOhly();
            this->renderOpaques();
            this->renderLights();
            this->renderOpaquesShading();

            auto renderable = this->getRenderData(RenderQueue::Transparent);
            if (!renderable.empty())
            {
                this->renderTransparentDepthOhly();
                this->renderTransparent();
                this->renderLights();
                this->renderTransparentShading();
            }
        }
        else if (renderMode == CameraRender::CR_RENDER_TO_CUBEMAP)
        {
        }
    }
    break;
    }
}

void
DeferredLighting::onRenderPost() noexcept
{
    /*if (_deferredGraphicMap)
        this->copyRenderTarget(_deferredGraphicMap, this->getCamera()->getViewport(), 0, Viewport(0, 768 / 2, 1376 / 2, 768));
    if (_deferredNormalMap)
        this->copyRenderTarget(_deferredNormalMap, this->getCamera()->getViewport(), 0, Viewport(1376 / 2, 768 / 2, 1376, 768));
    if (_deferredLightMap)
        this->copyRenderTarget(_deferredLightMap, this->getCamera()->getViewport(), 0, Viewport(0, 0, 1376 / 2, 768 / 2));
    if (_deferredShadingMap)
        this->copyRenderTarget(_deferredShadingMap, this->getCamera()->getViewport(), 0, Viewport(1376 / 2, 0, 1376, 768 / 2));*/
}

void
DeferredLighting::renderOpaquesDepthOhly() noexcept
{
    _deferredDepthMap->setClearFlags(ClearFlags::CLEAR_DEPTH_STENCIL);

    this->setRenderTarget(_deferredDepthMap);
    this->drawRenderable(RenderQueue::Background, RenderPass::RP_GBUFFER, _deferredDepthOhly);
    this->drawRenderable(RenderQueue::Opaque, RenderPass::RP_DEPTH, _deferredDepthOhly);

    _deferredDepthMap->setClearFlags(ClearFlags::CLEAR_NONE);
}

void
DeferredLighting::renderOpaques() noexcept
{
    _deferredGraphicMap->setClearFlags(ClearFlags::CLEAR_COLOR);
    _deferredNormalMap->setClearFlags(ClearFlags::CLEAR_COLOR);

    this->setRenderTarget(_deferredGraphicMaps);
    this->drawRenderable(RenderQueue::Opaque, RenderPass::RP_GBUFFER);

    _deferredNormalMap->setClearFlags(ClearFlags::CLEAR_NONE);
}

void
DeferredLighting::renderOpaquesShading() noexcept
{
    _deferredShadingMap->setClearFlags(ClearFlags::CLEAR_COLOR);

    this->setRenderTarget(_deferredShadingMap);
    this->setTechnique(_deferredShadingOpaques);
    this->drawSceneQuad();

    this->drawRenderable(RenderQueue::Background, RenderPass::RP_GBUFFER);

    _deferredShadingMap->setClearFlags(ClearFlags::CLEAR_NONE);
}

void
DeferredLighting::renderTransparentDepthOhly() noexcept
{
    _deferredDepthMap->setClearFlags(ClearFlags::CLEAR_STENCIL);

    this->setRenderTarget(_deferredDepthMap);
    this->drawRenderable(RenderQueue::Transparent, RenderPass::RP_DEPTH, _deferredDepthOhly);

    _deferredDepthMap->setClearFlags(ClearFlags::CLEAR_NONE);
}

void
DeferredLighting::renderTransparent() noexcept
{
    this->setRenderTarget(_deferredGraphicMaps);
    this->drawRenderable(RenderQueue::Transparent, RenderPass::RP_GBUFFER);
}

void
DeferredLighting::renderTransparentShading() noexcept
{
    this->setRenderTarget(_deferredShadingMap);
    this->setTechnique(_deferredShadingTransparents);
    this->drawSceneQuad();
}

void
DeferredLighting::renderLights() noexcept
{
    this->setRenderTarget(_deferredLightMap);

    auto& lights = this->getRenderData(RenderQueue::DeferredLight);
    for (auto& it : lights)
    {
        Light* light = dynamic_cast<Light*>(it);

        LightType type = light->getLightType();
        switch (type)
        {
        case ray::LT_SUN:
            this->renderSunLight(*light);
            break;
        case ray::LT_POINT:
            this->renderPointLight(*light);
            break;
        case ray::LT_AREA:
            this->renderAreaLight(*light);
            break;
        case ray::LT_SPOT:
            this->renderSpotLight(*light);
            break;
        case ray::LT_HEMI_SPHERE:
            this->renderHemiSphereLight(*light);
            break;
        default:
            break;
        }
    }
}

void
DeferredLighting::renderSunLight(const Light& light) noexcept
{
    float3 color = light.getLightColor() * light.getIntensity();

    auto semantic = Material::getMaterialSemantic();
    semantic->setFloat3Param(LightDiffuse, color);
    semantic->setFloat3Param(LightDirection, light.getLightDirection());
    semantic->setFloat3Param(LightPosition, light.getTransform().getTranslate());
    semantic->setFloatParam(LightRange, light.getRange());
    semantic->setFloatParam(LightIntensity, light.getIntensity());

    if (light.getShadow())
    {
        RenderStencilState stencil = _deferredSunLightShadow->getRenderState()->getStencilState();
        stencil.stencilRef = 1 << light.getLayer();

        _deferredSunLightShadow->getRenderState()->setStencilState(stencil);

        _shadowMatrix->assign(light.getShadowCamera()->getViewProject());
        _shadowMap->assign(light.getShadowCamera()->getRenderTarget()->getResolveTexture());

        this->setTechnique(_deferredSunLightShadow);
        this->drawSceneQuad();
    }
    else
    {
        RenderStencilState stencil = _deferredSunLight->getRenderState()->getStencilState();
        stencil.stencilRef = 1 << light.getLayer();

        _deferredSunLight->getRenderState()->setStencilState(stencil);

        this->setTechnique(_deferredSunLight);
        this->drawSceneQuad();
    }
}

void
DeferredLighting::renderPointLight(const Light& light) noexcept
{
    float3 color = light.getLightColor() * light.getIntensity();

    auto semantic = Material::getMaterialSemantic();
    semantic->setMatrixParam(matModel, light.getTransform());
    semantic->setFloat3Param(LightDiffuse, color);
    semantic->setFloat3Param(LightPosition, light.getTransform().getTranslate());
    semantic->setFloatParam(LightRange, light.getRange());
    semantic->setFloatParam(LightIntensity, light.getIntensity());

    RenderStencilState stencil = _deferredPointLight->getRenderState()->getStencilState();
    stencil.stencilRef = 1 << light.getLayer();

    _deferredPointLight->getRenderState()->setStencilState(stencil);

    this->setTechnique(_deferredPointLight);
    this->drawSphere();
}

void
DeferredLighting::renderSpotLight(const Light& light) noexcept
{
    float3 color = light.getLightColor() * light.getIntensity();

    auto semantic = Material::getMaterialSemantic();
    semantic->setMatrixParam(matModel, light.getTransform());
    semantic->setFloat3Param(LightDiffuse, color);
    semantic->setFloat3Param(LightDirection, light.getLightDirection());
    semantic->setFloat3Param(LightPosition, light.getTransform().getTranslate());
    semantic->setFloatParam(LightRange, light.getRange());
    semantic->setFloatParam(LightIntensity, light.getIntensity());
    semantic->setFloatParam(LightSpotAngle, light.getSpotAngle());
    semantic->setFloatParam(LightSpotInnerCone, light.getSpotInnerCone());
    semantic->setFloatParam(LightSpotOuterCone, light.getSpotOuterCone());

    RenderStencilState stencil = _deferredSpotLight->getRenderState()->getStencilState();
    stencil.stencilRef = 1 << light.getLayer();

    _deferredSpotLight->getRenderState()->setStencilState(stencil);

    this->setTechnique(_deferredSpotLight);
    this->drawCone();
}

void
DeferredLighting::renderHemiSphereLight(const Light& light) noexcept
{
}

void
DeferredLighting::renderAreaLight(const Light& light) noexcept
{
}

void
DeferredLighting::renderShadow() noexcept
{
    auto renderTexture = this->getCamera()->getRenderTarget();
    auto swapTexture = this->getCamera()->getSwapTexture();
    if (renderTexture)
    {
        _shadowDecal->assign(renderTexture->getResolveTexture());

        this->setRenderTarget(renderTexture);
        this->drawRenderable(RenderQueue::Shadow, RenderPass::RP_SHADOW, _shadowGenerate);

        this->setRenderTarget(this->getCamera()->getSwapTexture());
        this->setTechnique(_shadowBlurX);
        this->drawSceneQuad();

        _shadowDecal->assign(swapTexture->getResolveTexture());

        this->setRenderTarget(renderTexture);
        this->setTechnique(_shadowBlurY);
        this->drawSceneQuad();
    }
}

/*void
DeferredLighting::renderTransparencyDepthPeeling(const Camera& camera, Renderable& object, Renderable& lights) noexcept
{
    _passMaterial->setClearDepth(0.0);

    _passMaterial->setCamera(&camera);
    _passLighting->setCamera(&camera);
    _passShadowing->setCamera(&camera);
    _passShading->setCamera(&camera);

    _passMaterial->setRenderable(&object);
    _passLighting->setRenderable(&lights);
    _passShadowing->setRenderable(&lights);

    _renderer.setDepthEnable(true);
    _renderer.setDepthWrite(true);
    _renderer.setDepthFunc(CompareFunc::greater);

    _renderer.setStencilEnable(true);
    _renderer.setStencilOp(StencilOp::keep, StencilOp::keep, StencilOp::replace);

    for (int layer = 0; layer < _requiredLayers; layer++)
    {
        _passShading->isClearColor(layer == 0);

        _lightMap->assign(_texDepthGreater);
        _depthPeeling->assign(_texDepthLess);

        _targetMaterial->set(Framebuffer::depth, _texDepthGreater);
        _targetMaterial->set(Framebuffer::stencil, _texDepthGreater);
        _targetLighting->set(Framebuffer::stencil, _texDepthGreater);
        _targetTransparent->set(Framebuffer::stencil, _texDepthGreater);

        _renderer.setCullMode(StateAttribute::none);

        _renderer.setStencilFunc(CompareFunc::always, 1, 0xFFFFFFFF);

        _renderer.renderTechnique(*_compMaterial);

        _renderer.setStencilFunc(CompareFunc::equal, 1, 0xFFFFFFFF);

        _renderer.renderTechnique(*_compLighting);

        _renderer.renderTechnique(*_compShadowing);

        _renderer.setBlendEnable(true);
        _renderer.renderTechnique(*_compShadingTransparent);
        _renderer.setBlendEnable(StateAttribute::off);

        _texDepthGreater.swap(_texDepthLess);
    }

    _passShading->isClearColor(true);

    _renderer.setStencilEnable(StateAttribute::off);

    _lightMap->assign(_texDepthLess);

    _targetMaterial->set(Framebuffer::depth, _texDepthLess);
    _targetMaterial->set(Framebuffer::stencil, _texDepthLess);
    _targetLighting->set(Framebuffer::stencil, _texDepthLess);
    _targetOpacity->set(Framebuffer::stencil, _texDepthLess);
}
*/

_NAME_END