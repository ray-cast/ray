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
#include <ray/render_factory.h>

_NAME_BEGIN

const float ESM_FACTOR = 0.5f;

DeferredLighting::DeferredLighting() noexcept
{
}

DeferredLighting::~DeferredLighting() noexcept
{
}

void
DeferredLighting::renderOpaquesDepthOhly() noexcept
{
	_deferredDepthMap->setClearFlags(ClearFlags::CLEAR_DEPTH_STENCIL);

	this->setRenderTarget(_deferredDepthMap);
	this->drawRenderable(RenderQueue::Opaque, RenderPass::RP_DEPTH, _deferredDepthOnly);
	this->drawRenderable(RenderQueue::Background, RenderPass::RP_DEPTH, _deferredDepthOnly);

	_deferredDepthMap->setClearFlags(ClearFlags::CLEAR_NONE);
}

void
DeferredLighting::renderOpaques() noexcept
{
	_deferredGraphicMap->setClearFlags(ClearFlags::CLEAR_ALL);
	_deferredNormalMap->setClearFlags(ClearFlags::CLEAR_COLOR);

	this->setRenderTarget(_deferredGraphicMaps);
	this->drawRenderable(RenderQueue::Opaque, RenderPass::RP_OPAQUES);

	_deferredGraphicMap->setClearFlags(ClearFlags::CLEAR_NONE);
	_deferredNormalMap->setClearFlags(ClearFlags::CLEAR_NONE);
}

void
DeferredLighting::renderOpaquesShading() noexcept
{
	_deferredShadingMap->setClearFlags(ClearFlags::CLEAR_COLOR);

	this->setRenderTarget(_deferredShadingMap);
	this->setTechnique(_deferredShadingOpaques);
	this->drawSceneQuad();

	this->drawRenderable(RenderQueue::Background, RenderPass::RP_OPAQUES);

	_deferredShadingMap->setClearFlags(ClearFlags::CLEAR_NONE);
}

void
DeferredLighting::renderTransparentDepthOhly() noexcept
{
	_deferredDepthMap->setClearFlags(ClearFlags::CLEAR_STENCIL);

	this->setRenderTarget(_deferredDepthMap);
	this->drawRenderable(RenderQueue::Transparent, RenderPass::RP_DEPTH, _deferredDepthOnly);

	_deferredDepthMap->setClearFlags(ClearFlags::CLEAR_NONE);
}

void
DeferredLighting::renderTransparent() noexcept
{
	_deferredGraphicMap->setClearFlags(ClearFlags::CLEAR_COLOR_STENCIL);

	this->setRenderTarget(_deferredGraphicMaps);
	this->drawRenderable(RenderQueue::Transparent, RenderPass::RP_TRANSPARENT);

	_deferredGraphicMap->setClearFlags(ClearFlags::CLEAR_NONE);
}

void
DeferredLighting::renderTransparentShading() noexcept
{
	this->setRenderTarget(_deferredShadingMap);
	this->setTechnique(_deferredShadingTransparents);
	this->drawSceneQuad();

	this->drawRenderable(RenderQueue::Background, RenderPass::RP_TRANSPARENT);
}

void
DeferredLighting::renderLights() noexcept
{
	this->setRenderTarget(_deferredLightMap);

	auto& lights = this->getRenderData(RenderQueue::Lighting);
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
	if (renderTexture)
	{
		this->setRenderTarget(renderTexture);
		this->drawRenderable(RenderQueue::Opaque, RenderPass::RP_SHADOW, _shadowGenerate);
		this->drawRenderable(RenderQueue::Transparent, RenderPass::RP_SHADOW, _shadowGenerate);

		/*_shadowDecal->assign(renderTexture->getResolveTexture());

		this->setRenderTarget(_deferredShadowMap);
		this->setTechnique(_shadowBlurX);
		this->drawSceneQuad();

		_shadowDecal->assign(_deferredShadowMap->getResolveTexture());

		this->setRenderTarget(renderTexture);
		this->setTechnique(_shadowBlurY);
		this->drawSceneQuad();*/
	}
}

void
DeferredLighting::onActivate() except
{
	_deferredLighting = MaterialMaker("sys:fx\\deferred_lighting.glsl");
	_deferredDepthOnly = _deferredLighting->getTech(RenderQueue::Custom)->getPass("DeferredDepthOnly");
	_deferredPointLight = _deferredLighting->getTech(RenderQueue::Custom)->getPass("DeferredPointLight");
	_deferredSunLight = _deferredLighting->getTech(RenderQueue::Custom)->getPass("DeferredSunLight");
	_deferredSunLightShadow = _deferredLighting->getTech(RenderQueue::Custom)->getPass("DeferredSunLightShadow");
	_deferredSpotLight = _deferredLighting->getTech(RenderQueue::Custom)->getPass("DeferredSpotLight");
	_deferredShadingOpaques = _deferredLighting->getTech(RenderQueue::Custom)->getPass("DeferredShadingOpaques");
	_deferredShadingTransparents = _deferredLighting->getTech(RenderQueue::Custom)->getPass("DeferredShadingTransparents");

	_texDepth = _deferredLighting->getParameter("texDepth");
	_texDiffuse = _deferredLighting->getParameter("texDiffuse");
	_texNormal = _deferredLighting->getParameter("texNormal");
	_texLight = _deferredLighting->getParameter("texLight");

	_shadowChannel = _deferredLighting->getParameter("shadowChannel");
	_shadowMap = _deferredLighting->getParameter("shadowMap");
	_shadowLitFactor = _deferredLighting->getParameter("shadowFactor");
	_shadowMatrix = _deferredLighting->getParameter("shadowMatrix");

	_shadow = MaterialMaker("sys:fx\\shadow.glsl");
	_shadowGenerate = _shadow->getTech(RenderQueue::Custom)->getPass("shadow");
	_shadowBlurX = _shadow->getTech(RenderQueue::Custom)->getPass("blurX");
	_shadowBlurY = _shadow->getTech(RenderQueue::Custom)->getPass("blurY");
	_shadowDecal = _shadow->getParameter("decal");
	_shadowFactor = _shadow->getParameter("factor");

	_shadowFactor->assign(ESM_FACTOR);
	_shadowLitFactor->assign(ESM_FACTOR);

	std::size_t width = this->getWindowWidth();
	std::size_t height = this->getWindowHeight();

	_deferredDepthMap = RenderFactory::createRenderTarget();
	_deferredDepthMap->setup(width, height, TextureDim::DIM_2D, PixelFormat::DEPTH24_STENCIL8);

	_deferredGraphicMap = RenderFactory::createRenderTarget();
	_deferredGraphicMap->setup(width, height, TextureDim::DIM_2D, PixelFormat::SR8G8B8A8);

	_deferredNormalMap = RenderFactory::createRenderTarget();
	_deferredNormalMap->setup(width, height, TextureDim::DIM_2D, PixelFormat::R16G16B16A16F);

	_deferredLightMap = RenderFactory::createRenderTarget();
	_deferredLightMap->setSharedStencilTexture(_deferredDepthMap);
	_deferredLightMap->setClearFlags(ClearFlags::CLEAR_COLOR);
	_deferredLightMap->setup(width, height, TextureDim::DIM_2D, PixelFormat::R32G32B32F);

	_deferredShadowMap = RenderFactory::createRenderTarget();
	_deferredShadowMap->setup(width, height, TextureDim::DIM_2D, PixelFormat::R16F);

	_deferredGraphicMaps = RenderFactory::createMultiRenderTarget();
	_deferredGraphicMaps->setSharedDepthTexture(_deferredDepthMap);
	_deferredGraphicMaps->setSharedStencilTexture(_deferredDepthMap);
	_deferredGraphicMaps->attach(_deferredGraphicMap);
	_deferredGraphicMaps->attach(_deferredNormalMap);
	_deferredGraphicMaps->setup();

	_deferredShadingMap = RenderFactory::createRenderTarget();
	_deferredShadingMap->setSharedDepthTexture(_deferredDepthMap);
	_deferredShadingMap->setSharedStencilTexture(_deferredDepthMap);
	_deferredShadingMap->setup(width, height, TextureDim::DIM_2D, PixelFormat::R16G16B16A16F);

	auto semantic = Material::getMaterialSemantic();
	semantic->setTexParam(GlobalTexSemantic::DeferredDepthMap, _deferredDepthMap->getResolveTexture());
	semantic->setTexParam(GlobalTexSemantic::DeferredNormalMap, _deferredNormalMap->getResolveTexture());
	semantic->setTexParam(GlobalTexSemantic::DeferredGraphicMap, _deferredGraphicMap->getResolveTexture());
	semantic->setTexParam(GlobalTexSemantic::DeferredLightMap, _deferredLightMap->getResolveTexture());
	semantic->setTexParam(GlobalTexSemantic::DepthMap, _deferredDepthMap->getResolveTexture());
	semantic->setTexParam(GlobalTexSemantic::ColorMap, _deferredShadingMap->getResolveTexture());
	semantic->setTexParam(GlobalTexSemantic::NormalMap, _deferredNormalMap->getResolveTexture());
}

void
DeferredLighting::onDectivate() noexcept
{
	if (_deferredDepthMap)
	{
		_deferredDepthMap.reset();
		_deferredDepthMap = nullptr;
	}

	if (_deferredGraphicMap)
	{
		_deferredGraphicMap.reset();
		_deferredGraphicMap = nullptr;
	}

	if (_deferredNormalMap)
	{
		_deferredNormalMap.reset();
		_deferredNormalMap = nullptr;
	}

	if (_deferredLightMap)
	{
		_deferredLightMap.reset();
		_deferredLightMap = nullptr;
	}

	if (_deferredGraphicMaps)
	{
		_deferredGraphicMaps.reset();
		_deferredGraphicMaps = nullptr;
	}
}

void
DeferredLighting::onRenderPre() noexcept
{
	this->assignLight(this->getCamera());
	this->assignVisiable(this->getCamera());
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
		if (renderMode == CameraRender::CR_RENDER_TO_TEXTURE ||
			renderMode == CameraRender::CR_RENDER_TO_SCREEN)
		{
			this->renderOpaques();
			this->renderLights();
			this->renderOpaquesShading();

			auto renderable = this->getRenderData(RenderQueue::Transparent);
			if (!renderable.empty())
			{
				this->renderTransparent();
				this->renderLights();
				this->renderTransparentShading();
			}

			this->applyPostProcess(_deferredShadingMap);
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
	CameraOrder mode = this->getCamera()->getCameraOrder();
	if (mode != CO_SHADOW)
	{
		if (this->getCamera()->getCameraRender() == CR_RENDER_TO_SCREEN)
		{
			auto v1 = Viewport(0, 0, _deferredShadingMap->getWidth(), _deferredShadingMap->getHeight());
			auto v2 = this->getCamera()->getViewport();
			if (v2.width == 0 ||
				v2.height == 0)
			{
				v2.left = 0;
				v2.top = 0;
				v2.width = this->getWindowWidth();
				v2.height = this->getWindowHeight();
			}

			this->copyRenderTarget(_deferredShadingMap, v1, 0, v2);
		}
		else if (this->getCamera()->getCameraRender() == CR_RENDER_TO_TEXTURE)
		{
			auto target = this->getCamera()->getRenderTarget();

			auto v1 = Viewport(0, 0, _deferredShadingMap->getWidth(), _deferredShadingMap->getHeight());
			auto v2 = this->getCamera()->getViewport();
			if (v2.width == 0 ||
				v2.height == 0)
			{
				v2.left = 0;
				v2.top = 0;
				v2.width = target->getWidth();
				v2.height = target->getHeight();
			}

			this->copyRenderTarget(_deferredShadingMap, v1, target, v2);
		}

		/*if (_deferredGraphicMap)
			this->copyRenderTarget(_deferredGraphicMap, Viewport(0, 0, 1376, 768), 0, Viewport(0, 768 / 2, 1376 / 2, 768));
		if (_deferredNormalMap)
			this->copyRenderTarget(_deferredNormalMap, Viewport(0, 0, 1376, 768), 0, Viewport(1376 / 2, 768 / 2, 1376, 768));
		if (_deferredLightMap)
			this->copyRenderTarget(_deferredLightMap, Viewport(0, 0, 1376, 768), 0, Viewport(0, 0, 1376 / 2, 768 / 2));
		if (_deferredShadingMap)
			this->copyRenderTarget(_deferredShadingMap, Viewport(0, 0, 1376, 768), 0, Viewport(1376 / 2, 0, 1376, 768 / 2));*/
	}
}

void
DeferredLighting::onRenderObjectPre(RenderObject& object, RenderQueue queue, RenderPass type, MaterialPassPtr pass) except
{
}

_NAME_END