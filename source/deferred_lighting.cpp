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
#include <ray/renderer.h>
#include <ray/camera.h>
#include <ray/light.h>
#include <ray/material_maker.h>

_NAME_BEGIN

const float ESM_FACTOR = 0.7f;

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
DeferredLighting::setup(std::size_t width, std::size_t height) except
{
    _deferredLighting = MaterialMaker("sys:fx\\deferred_lighting.glsl");
    _deferredDepthOhly = _deferredLighting->getTech(RenderQueue::DeferredLight)->getPass("DeferredDepthOhly");
    _deferredPointLight = _deferredLighting->getTech(RenderQueue::DeferredLight)->getPass("DeferredPointLight");
    _deferredSunLight = _deferredLighting->getTech(RenderQueue::DeferredLight)->getPass("DeferredSunLight");
    _deferredSpotLight = _deferredLighting->getTech(RenderQueue::DeferredLight)->getPass("DeferredSpotLight");
    _deferredShading = _deferredLighting->getTech(RenderQueue::DeferredLight)->getPass("DeferredShading");

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

    RenderPipeline::setup(width, height);
}

void
DeferredLighting::close() noexcept
{
    RenderPipeline::close();
}

void
DeferredLighting::render() noexcept
{
    CameraOrder mode = camera->getCameraOrder();
    if (mode == CO_SHADOW)
    {
        this->renderShadow();
    }
    else
    {
        _deferredDepthMap = camera->getDeferredDepthMap();
        _deferredGraphicMap = camera->getDeferredGraphicMap();
        _deferredNormalMap = camera->getDeferredNormalMap();
        _deferredLightMap = camera->getDeferredLightMap();
        _deferredShadingMap = camera->getRenderTexture();
        _deferredGraphicMaps = camera->getDeferredGraphicsMaps();

        auto semantic = Material::getMaterialSemantic();
        semantic->setTexParam(GlobalTexSemantic::DeferredDepthMap, _deferredDepthMap->getResolveTexture());
        semantic->setTexParam(GlobalTexSemantic::DeferredNormalMap, _deferredNormalMap->getResolveTexture());
        semantic->setTexParam(GlobalTexSemantic::DeferredGraphicMap, _deferredGraphicMap->getResolveTexture());
        semantic->setTexParam(GlobalTexSemantic::DeferredLightMap, _deferredLightMap->getResolveTexture());
        semantic->setTexParam(GlobalTexSemantic::DepthMap, _deferredDepthMap->getResolveTexture());
        semantic->setTexParam(GlobalTexSemantic::ColorMap, _deferredShadingMap->getResolveTexture());
        semantic->setTexParam(GlobalTexSemantic::NormalMap, _deferredNormalMap->getResolveTexture());

        this->renderOpaquesDepthOhly();
        this->renderOpaques();
        this->renderLights();
        this->renderOpaquesShading();

        this->SSAOPostProcess(_deferredShadingMap);

        this->renderTransparentDepthOhly();
        this->renderTransparent();
        this->renderLights();
        this->renderTransparentShading();

        this->onPostProcess(_deferredShadingMap);

        /*this->copyRenderTexture(_deferredGraphicMap, camera->getViewport(), 0, Viewport(0, 768 / 2, 1376 / 2, 768));
        this->copyRenderTexture(_deferredNormalMap, camera->getViewport(), 0, Viewport(1376 / 2, 768 / 2, 1376, 768));
        this->copyRenderTexture(_deferredLightMap, camera->getViewport(), 0, Viewport(0, 0, 1376 / 2, 768 / 2));
        this->copyRenderTexture(_deferredShadingMap, camera->getViewport(), 0, Viewport(1376 / 2, 0, 1376, 768 / 2));*/
        this->copyRenderTexture(_deferredShadingMap, camera->getViewport(), 0, camera->getViewport());
    }
}

void
DeferredLighting::renderOpaquesDepthOhly() noexcept
{
    _deferredDepthMap->getFramebuffer()->setClearFlags(ClearFlags::CLEAR_ALL);

    this->setRenderTexture(_deferredDepthMap);
    this->drawRenderable(RenderQueue::Opaque, RenderPass::RP_DEPTH, _deferredDepthOhly);
}

void
DeferredLighting::renderOpaques() noexcept
{
    _deferredGraphicMaps->getFramebuffer()->setClearFlags(ClearFlags::CLEAR_COLOR);

    this->setRenderTexture(_deferredGraphicMaps);
    this->drawRenderable(RenderQueue::Opaque, RenderPass::RP_GBUFFER);
}

void
DeferredLighting::renderOpaquesShading() noexcept
{
    _deferredShadingMap->getFramebuffer()->setClearFlags(ClearFlags::CLEAR_COLOR_DEPTH);

    this->setRenderTexture(_deferredShadingMap);
    this->setTechnique(_deferredShading);
    this->drawSceneQuad();
}

void
DeferredLighting::renderTransparentDepthOhly() noexcept
{
    _deferredDepthMap->getFramebuffer()->setClearFlags(ClearFlags::CLEAR_STENCIL);

    this->setRenderTexture(_deferredDepthMap);
    this->drawRenderable(RenderQueue::Transparent, RenderPass::RP_DEPTH, _deferredDepthOhly);
}

void
DeferredLighting::renderTransparent() noexcept
{
    this->setRenderTexture(_deferredGraphicMaps);
    this->drawRenderable(RenderQueue::Transparent, RenderPass::RP_GBUFFER);
}

void
DeferredLighting::renderTransparentShading() noexcept
{
    _deferredShadingMap->getFramebuffer()->setClearFlags(ClearFlags::CLEAR_NONE);

    this->setRenderTexture(_deferredShadingMap);
    this->setTechnique(_deferredShading);
    this->drawSceneQuad();
}

void
DeferredLighting::renderLights() noexcept
{
    _deferredLightMap->getFramebuffer()->setClearFlags(ClearFlags::CLEAR_COLOR_DEPTH);

    this->setRenderTexture(_deferredLightMap);

    for (auto& it : lights)
    {
        LightType type = it->getLightType();
        switch (type)
        {
        case ray::LT_SUN:
            this->renderSunLight(*it);
            break;
        case ray::LT_POINT:
            this->renderPointLight(*it);
            break;
        case ray::LT_AREA:
            this->renderAreaLight(*it);
            break;
        case ray::LT_SPOT:
            this->renderSpotLight(*it);
            break;
        case ray::LT_HEMI_SPHERE:
            this->renderHemiSphereLight(*it);
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

    RenderStencilState stencil = _deferredSunLight->getRenderState()->getStencilState();
    stencil.stencilRef = 1 << light.getLayer();

    _deferredSunLight->getRenderState()->setStencilState(stencil);

    if (light.getShadow())
    {
        _shadowChannel->assign(light.getShadow());
        _shadowMatrix->assign(light.getShadowCamera()->getViewProject());
        _shadowMap->setTexture(light.getShadowCamera()->getRenderTexture()->getResolveTexture());
    }

    this->setTechnique(_deferredSunLight);
    this->drawSceneQuad();
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
    auto pass = _shadow->getTech(RenderQueue::Shadow)->getPass(RenderPass::RP_SHADOW);

    this->setRenderTexture(camera->getRenderTexture());
    this->drawRenderable(RenderQueue::Shadow, RenderPass::RP_SHADOW, pass);

    _shadowDecal->setTexture(camera->getRenderTexture()->getResolveTexture());

    this->setRenderTexture(camera->getSwapTexture());
    this->setTechnique(_shadowBlurX);
    this->drawSceneQuad();

    _shadowDecal->setTexture(camera->getSwapTexture()->getResolveTexture());

    this->setRenderTexture(camera->getRenderTexture());
    this->setTechnique(_shadowBlurY);
    this->drawSceneQuad();
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