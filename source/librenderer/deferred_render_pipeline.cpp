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
#include <ray/deferred_render_pipeline.h>
#include <ray/render_pipeline.h>
#include <ray/camera.h>
#include <ray/light.h>
#include <ray/render_scene.h>
#include <ray/render_texture.h>
#include <ray/graphics_state.h>
#include <ray/material_manager.h>

_NAME_BEGIN

const float ESM_FACTOR = 1000.0f;

DeferredRenderPipeline::DeferredRenderPipeline() noexcept
{
	_leftCamera = std::make_shared<Camera>();
	_rightCamera = std::make_shared<Camera>();
	_frontCamera = std::make_shared<Camera>();
	_backCamera = std::make_shared<Camera>();
	_topCamera = std::make_shared<Camera>();
	_bottomCamera = std::make_shared<Camera>();
}

DeferredRenderPipeline::~DeferredRenderPipeline() noexcept
{
}

void
DeferredRenderPipeline::renderShadowMap(RenderPipeline& pipeline) noexcept
{
	pipeline.setRenderTexture(pipeline.getCamera()->getRenderTexture());
	pipeline.clearRenderTexture(ClearFlags::CLEAR_ALL, Vector4::Zero, 1.0, 0.0);

	pipeline.drawRenderQueue(RenderQueue::RQ_OPAQUE, RenderPass::RP_OPAQUES, _deferredDepthOnly);
	pipeline.drawRenderQueue(RenderQueue::RQ_TRANSPARENT, RenderPass::RP_TRANSPARENT, _deferredDepthOnly);
}

void
DeferredRenderPipeline::render2DEnvMap(RenderPipeline& pipeline) noexcept
{
	pipeline.setRenderTexture(_deferredShadingMap);
	pipeline.clearRenderTexture(ClearFlags::CLEAR_ALL, pipeline.getCamera()->getClearColor(), 1.0, 0);
	pipeline.drawRenderQueue(RenderQueue::RQ_OPAQUE, RenderPass::RP_OPAQUES);
}

void
DeferredRenderPipeline::render3DEnvMap(RenderPipeline& pipeline) noexcept
{
	auto semantic = pipeline.getMaterialManager();
	semantic->setTexParam(MaterialSemantic::DeferredDepthMap, _deferredDepthMap->getResolveTexture());
	semantic->setTexParam(MaterialSemantic::DeferredDepthLinearMap, _deferredDepthLinearMap->getResolveTexture());
	semantic->setTexParam(MaterialSemantic::DeferredNormalMap, _deferredNormalMap->getResolveTexture());
	semantic->setTexParam(MaterialSemantic::DeferredGraphicMap, _deferredGraphicMap->getResolveTexture());
	semantic->setTexParam(MaterialSemantic::DeferredLightMap, _deferredLightMap->getResolveTexture());

	this->renderOpaques(pipeline, _deferredGraphicMaps);
	this->renderOpaquesDepthLinear(pipeline, _deferredDepthLinearMap);
	this->renderLights(pipeline, _deferredLightMap);
	this->renderOpaquesShading(pipeline, _deferredShadingMap);
	this->renderOpaquesSpecificShading(pipeline, _deferredShadingMap);

	/*this->renderTransparent(_deferredGraphicMaps);
	this->renderLights(_deferredLightMap);
	this->renderTransparentShading(_deferredShadingMap);
	this->renderTransparentSpecificShading(_deferredShadingMap);*/

	pipeline.renderPostProcess(_deferredShadingMap);

	auto renderTexture = pipeline.getCamera()->getRenderTexture();
	if (renderTexture)
		this->copyRenderTexture(pipeline, _deferredShadingMap, renderTexture, pipeline.getCamera()->getViewport());
}

void
DeferredRenderPipeline::renderOpaques(RenderPipeline& pipeline, MultiRenderTexturePtr target) noexcept
{
	pipeline.setMultiRenderTexture(target);
	pipeline.clearRenderTexture(ClearFlags::CLEAR_ALL, Vector4::Zero, 1.0, 0.0);
	pipeline.drawRenderQueue(RenderQueue::RQ_OPAQUE, RenderPass::RP_OPAQUES);
}

void
DeferredRenderPipeline::renderOpaquesDepthLinear(RenderPipeline& pipeline, RenderTexturePtr target) noexcept
{
	_clipInfo->assign(pipeline.getCamera()->getClipConstant());
	_projInfo->assign(pipeline.getCamera()->getProjConstant());

	pipeline.setRenderTexture(target);
	pipeline.clearRenderTexture(ClearFlags::CLEAR_ALL, Vector4::Zero, 1.0, 0.0);
	pipeline.drawSceneQuad(_deferredDepthLinear);
}

void
DeferredRenderPipeline::renderOpaquesShading(RenderPipeline& pipeline, RenderTexturePtr target, int layer) noexcept
{
	pipeline.setRenderTexture(target);
	pipeline.setRenderTextureLayer(target, layer);
	pipeline.clearRenderTexture(ClearFlags::CLEAR_COLOR, Vector4::Zero, 1.0, 0.0);
	pipeline.drawSceneQuad(_deferredShadingOpaques);
}

void
DeferredRenderPipeline::renderOpaquesSpecificShading(RenderPipeline& pipeline, RenderTexturePtr target, int layer) noexcept
{
	pipeline.setRenderTexture(target);
	pipeline.setRenderTextureLayer(target, layer);
	pipeline.drawRenderQueue(RenderQueue::RQ_OPAQUE, RenderPass::RP_SPECIFIC);
	pipeline.drawRenderQueue(RenderQueue::RQ_OPAQUE, RenderPass::RP_POSTPROCESS, nullptr, target);
}

void
DeferredRenderPipeline::renderTransparent(RenderPipeline& pipeline, MultiRenderTexturePtr renderTexture) noexcept
{
	pipeline.setMultiRenderTexture(renderTexture);
	pipeline.clearRenderTexture(ClearFlags::CLEAR_COLOR_STENCIL, Vector4::Zero, 1.0, 0.0, 0);

	pipeline.drawRenderQueue(RenderQueue::RQ_TRANSPARENT, RenderPass::RP_TRANSPARENT);
}

void
DeferredRenderPipeline::renderTransparentDepthLinear(RenderPipeline& pipeline, RenderTexturePtr target) noexcept
{
	pipeline.setRenderTexture(target);
	pipeline.clearRenderTexture(ClearFlags::CLEAR_COLOR, Vector4::Zero, 1.0, 0.0, 0);

	pipeline.drawRenderQueue(RenderQueue::RQ_TRANSPARENT, RenderPass::RP_DEPTH, _deferredDepthLinear);
}

void
DeferredRenderPipeline::renderTransparentShading(RenderPipeline& pipeline, RenderTexturePtr target, int layer) noexcept
{
	pipeline.setRenderTexture(target);
	pipeline.setRenderTextureLayer(target, layer);
	pipeline.drawSceneQuad(_deferredShadingTransparents);
}

void
DeferredRenderPipeline::renderTransparentSpecificShading(RenderPipeline& pipeline, RenderTexturePtr target, int layer) noexcept
{
	pipeline.setRenderTexture(target);
	pipeline.setRenderTextureLayer(target, layer);
	pipeline.drawRenderQueue(RenderQueue::RQ_TRANSPARENT, RenderPass::RP_SPECIFIC);
	pipeline.drawRenderQueue(RenderQueue::RQ_TRANSPARENT, RenderPass::RP_POSTPROCESS);
}

void
DeferredRenderPipeline::renderLights(RenderPipeline& pipeline, RenderTexturePtr target) noexcept
{
	pipeline.setRenderTexture(target);
	pipeline.clearRenderTexture(ClearFlags::CLEAR_COLOR, Vector4::Zero, 1.0, 0);

	auto& lights = pipeline.getRenderData(RenderQueue::RQ_LIGHTING, RenderPass::RP_LIGHTS);
	for (auto& it : lights)
	{
		auto light = std::dynamic_pointer_cast<Light>(it);

		switch (light->getLightType())
		{
		case ray::LT_SUN:
			this->renderSunLight(pipeline, *light);
			break;
		case ray::LT_DIRECTIONAL:
			this->renderDirectionalLight(pipeline, *light);
			break;
		case ray::LT_POINT:
			this->renderPointLight(pipeline, *light);
			break;
		case ray::LT_AREA:
			this->renderAreaLight(pipeline, *light);
			break;
		case ray::LT_SPOT:
			this->renderSpotLight(pipeline, *light);
			break;
		case ray::LT_HEMI_SPHERE:
			this->renderHemiSphereLight(pipeline, *light);
			break;
		case ray::LT_AMBIENT:
			this->renderAmbientLight(pipeline, *light);
			break;
		default:
			break;
		}
	}
}

void
DeferredRenderPipeline::renderSunLight(RenderPipeline& pipeline, const Light& light) noexcept
{
	_lightColor->assign(light.getLightColor() * light.getIntensity());
	_lightDirection->assign(float3x3(pipeline.getCamera()->getView()) * ~(light.getTransform().getTranslate() - light.getLightLookat()));
	_lightPosition->assign(light.getTransform().getTranslate());
	_lightAttenuation->assign(light.getLightAttenuation());
	_lightRange->assign(light.getRange());

	_eyePosition->assign(float3x3(pipeline.getCamera()->getView()) * pipeline.getCamera()->getTranslate());

	if (light.getShadow())
	{
		auto& stencil = _deferredSunLightShadow->getGraphicsState()->getStencilState();
		stencil.stencilRef = 1 << light.getLayer();

		_shadowFactor->assign(ESM_FACTOR);
		_shadowMap->assign(light.getShadowCamera()->getRenderTexture()->getResolveTexture());
		_shadowMatrix->assign(light.getShadowCamera()->getViewProject());

		pipeline.drawSceneQuad(_deferredSunLightShadow);
	}
	else
	{
		auto& stencil = _deferredSunLight->getGraphicsState()->getStencilState();
		stencil.stencilRef = 1 << light.getLayer();

		pipeline.drawSceneQuad(_deferredSunLight);
	}
}

void
DeferredRenderPipeline::renderDirectionalLight(RenderPipeline& pipeline, const Light& light) noexcept
{
	_lightColor->assign(light.getLightColor() * light.getIntensity());
	_lightDirection->assign(float3x3(pipeline.getCamera()->getView()) * ~(light.getTransform().getTranslate() - light.getLightLookat()));
	_lightPosition->assign(light.getTransform().getTranslate());
	_lightAttenuation->assign(light.getLightAttenuation());
	_lightRange->assign(light.getRange());

	_eyePosition->assign(float3x3(pipeline.getCamera()->getView()) * pipeline.getCamera()->getTranslate());

	if (light.getShadow())
	{
		RenderStencilState stencil = _deferredDirectionalLightShadow->getGraphicsState()->getStencilState();
		stencil.stencilRef = 1 << light.getLayer();

		_deferredDirectionalLightShadow->getGraphicsState()->setStencilState(stencil);

		_shadowMap->assign(light.getShadowCamera()->getRenderTexture()->getResolveTexture());
		_shadowFactor->assign(ESM_FACTOR);
		_shadowMatrix->assign(light.getShadowCamera()->getViewProject());

		pipeline.drawSceneQuad(_deferredDirectionalLightShadow);
	}
	else
	{
		RenderStencilState stencil = _deferredDirectionalLight->getGraphicsState()->getStencilState();
		stencil.stencilRef = 1 << light.getLayer();

		_deferredDirectionalLight->getGraphicsState()->setStencilState(stencil);

		pipeline.drawSceneQuad(_deferredDirectionalLight);
	}
}

void
DeferredRenderPipeline::renderPointLight(RenderPipeline& pipeline, const Light& light) noexcept
{
	_lightColor->assign(light.getLightColor() * light.getIntensity());
	_lightDirection->assign(float3x3(pipeline.getCamera()->getView()) * ~(light.getTransform().getTranslate() - light.getLightLookat()));
	_lightPosition->assign(light.getTransform().getTranslate());
	_lightAttenuation->assign(light.getLightAttenuation());
	_lightRange->assign(light.getRange());
	_lightIntensity->assign(light.getIntensity());

	_eyePosition->assign(float3x3(pipeline.getCamera()->getView()) * pipeline.getCamera()->getTranslate());

	auto semantic = pipeline.getMaterialManager();
	semantic->setMatrixParam(matModel, light.getTransform());

	RenderStencilState stencil = _deferredPointLight->getGraphicsState()->getStencilState();
	stencil.stencilRef = 1 << light.getLayer();

	_deferredPointLight->getGraphicsState()->setStencilState(stencil);

	pipeline.drawSphere(_deferredPointLight);
}

void
DeferredRenderPipeline::renderSpotLight(RenderPipeline& pipeline, const Light& light) noexcept
{
	auto semantic = pipeline.getMaterialManager();
	semantic->setMatrixParam(matModel, light.getTransform());

	_lightColor->assign(light.getLightColor() * light.getIntensity());
	_lightDirection->assign(float3x3(pipeline.getCamera()->getView()) * ~(light.getTransform().getTranslate() - light.getLightLookat()));
	_lightPosition->assign(light.getTransform().getTranslate());
	_lightAttenuation->assign(light.getLightAttenuation());
	_lightRange->assign(light.getRange());
	_lightIntensity->assign(light.getIntensity());
	_lightSpotInnerCone->assign(light.getSpotInnerCone());
	_lightSpotOuterCone->assign(light.getSpotOuterCone());

	_eyePosition->assign(float3x3(pipeline.getCamera()->getView()) * pipeline.getCamera()->getTranslate());

	RenderStencilState stencil = _deferredSpotLight->getGraphicsState()->getStencilState();
	stencil.stencilRef = 1 << light.getLayer();

	_deferredSpotLight->getGraphicsState()->setStencilState(stencil);

	pipeline.drawCone(_deferredSpotLight);
}

void
DeferredRenderPipeline::renderAmbientLight(RenderPipeline& pipeline, const Light& light) noexcept
{
	_lightColor->assign(light.getLightColor() * light.getIntensity());
	_lightDirection->assign(float3x3(pipeline.getCamera()->getView()) * ~(light.getTransform().getTranslate() - light.getLightLookat()));
	_lightPosition->assign(light.getTransform().getTranslate());
	_lightAttenuation->assign(light.getLightAttenuation());
	_lightRange->assign(light.getRange());
	_lightIntensity->assign(light.getIntensity());

	_eyePosition->assign(pipeline.getCamera()->getTranslate());

	RenderStencilState stencil = _deferredAmbientLight->getGraphicsState()->getStencilState();
	stencil.stencilRef = 1 << light.getLayer();

	_deferredAmbientLight->getGraphicsState()->setStencilState(stencil);

	pipeline.drawSceneQuad(_deferredAmbientLight);
}

void
DeferredRenderPipeline::renderHemiSphereLight(RenderPipeline& pipeline, const Light& light) noexcept
{
}

void
DeferredRenderPipeline::renderAreaLight(RenderPipeline& pipeline, const Light& light) noexcept
{
}

void
DeferredRenderPipeline::copyRenderTexture(RenderPipeline& pipeline, RenderTexturePtr src, RenderTexturePtr dst, const Viewport& view) noexcept
{
	_texSource->assign(src->getResolveTexture());
	pipeline.setRenderTexture(dst);
	pipeline.drawSceneQuad(_deferredCopyOnly);
}

void
DeferredRenderPipeline::onActivate(RenderPipeline& pipeline) except
{
	std::uint32_t width, height;
	pipeline.getWindowResolution(width, height);

	_deferredLighting = pipeline.createMaterial("sys:fx\\deferred_lighting.glsl");
	_deferredDepthOnly = _deferredLighting->getTech(RenderQueue::RQ_CUSTOM)->getPass("DeferredDepthOnly");
	_deferredDepthLinear = _deferredLighting->getTech(RenderQueue::RQ_CUSTOM)->getPass("DeferredDepthLinear");
	_deferredPointLight = _deferredLighting->getTech(RenderQueue::RQ_CUSTOM)->getPass("DeferredPointLight");
	_deferredAmbientLight = _deferredLighting->getTech(RenderQueue::RQ_CUSTOM)->getPass("DeferredAmbientLight");
	_deferredSunLight = _deferredLighting->getTech(RenderQueue::RQ_CUSTOM)->getPass("DeferredSunLight");
	_deferredSunLightShadow = _deferredLighting->getTech(RenderQueue::RQ_CUSTOM)->getPass("DeferredSunLightShadow");
	_deferredDirectionalLight = _deferredLighting->getTech(RenderQueue::RQ_CUSTOM)->getPass("DirectionalLight");
	_deferredDirectionalLightShadow = _deferredLighting->getTech(RenderQueue::RQ_CUSTOM)->getPass("DirectionalLightShadow");
	_deferredSpotLight = _deferredLighting->getTech(RenderQueue::RQ_CUSTOM)->getPass("DeferredSpotLight");
	_deferredShadingOpaques = _deferredLighting->getTech(RenderQueue::RQ_CUSTOM)->getPass("DeferredShadingOpaques");
	_deferredShadingTransparents = _deferredLighting->getTech(RenderQueue::RQ_CUSTOM)->getPass("DeferredShadingTransparents");
	_deferredDebugLayer = _deferredLighting->getTech(RenderQueue::RQ_CUSTOM)->getPass("DeferredDebugLayer");
	_deferredCopyOnly = _deferredLighting->getTech(RenderQueue::RQ_CUSTOM)->getPass("DeferredCopyOnly");

	_texMRT0 = _deferredLighting->getParameter("texMRT0");
	_texMRT1 = _deferredLighting->getParameter("texMRT1");
	_texDepth = _deferredLighting->getParameter("texDepth");
	_texLight = _deferredLighting->getParameter("texLight");
	_texSource = _deferredLighting->getParameter("texSource");
	_texEnvironmentMap = _deferredLighting->getParameter("texEnvironmentMap");

	_eyePosition = _deferredLighting->getParameter("eyePosition");
	_clipInfo = _deferredLighting->getParameter("clipInfo");
	_projInfo = _deferredLighting->getParameter("projInfo");

	_lightColor = _deferredLighting->getParameter("lightColor");
	_lightPosition = _deferredLighting->getParameter("lightPosition");
	_lightDirection = _deferredLighting->getParameter("lightDirection");
	_lightRange = _deferredLighting->getParameter("lightRange");
	_lightIntensity = _deferredLighting->getParameter("lightIntensity");
	_lightAttenuation = _deferredLighting->getParameter("lightAttenuation");
	_lightSpotInnerCone = _deferredLighting->getParameter("lightSpotInnerCone");
	_lightSpotOuterCone = _deferredLighting->getParameter("lightSpotOuterCone");

	_shadowChannel = _deferredLighting->getParameter("shadowChannel");
	_shadowMap = _deferredLighting->getParameter("shadowMap");
	_shadowArrayMap = _deferredLighting->getParameter("shadowArrayMap");
	_shadowFactor = _deferredLighting->getParameter("shadowFactor");
	_shadowMatrix = _deferredLighting->getParameter("shadowMatrix");

	_deferredDepthMap = pipeline.createRenderTexture();
	_deferredDepthMap->setup(width, height, TextureDim::DIM_2D, TextureFormat::DEPTH32_STENCIL8);

	_deferredDepthLinearMap = pipeline.createRenderTexture();
	_deferredDepthLinearMap->setup(width, height, TextureDim::DIM_2D, TextureFormat::R32F);

	_deferredGraphicMap = pipeline.createRenderTexture();
	_deferredGraphicMap->setup(width, height, TextureDim::DIM_2D, TextureFormat::R8G8B8A8);

	_deferredNormalMap = pipeline.createRenderTexture();
	_deferredNormalMap->setup(width, height, TextureDim::DIM_2D, TextureFormat::R8G8B8A8);

	_deferredLightMap = pipeline.createRenderTexture();
	_deferredLightMap->setSharedStencilTexture(_deferredDepthMap);
	_deferredLightMap->setup(width, height, TextureDim::DIM_2D, TextureFormat::R16G16B16A16F);

	_deferredShadingMap = pipeline.createRenderTexture();
	_deferredShadingMap->setSharedDepthTexture(_deferredDepthMap);
	_deferredShadingMap->setSharedStencilTexture(_deferredDepthMap);
	_deferredShadingMap->setup(width, height, TextureDim::DIM_2D, TextureFormat::R8G8B8A8);

	_deferredGraphicMaps = pipeline.createMultiRenderTexture();
	_deferredGraphicMaps->setSharedDepthTexture(_deferredDepthMap);
	_deferredGraphicMaps->setSharedStencilTexture(_deferredDepthMap);
	_deferredGraphicMaps->attach(_deferredGraphicMap);
	_deferredGraphicMaps->attach(_deferredNormalMap);
	_deferredGraphicMaps->setup();

	auto semantic = pipeline.getMaterialManager();
	semantic->setTexParam(MaterialSemantic::DeferredDepthMap, _deferredDepthMap->getResolveTexture());
	semantic->setTexParam(MaterialSemantic::DeferredNormalMap, _deferredNormalMap->getResolveTexture());
	semantic->setTexParam(MaterialSemantic::DeferredGraphicMap, _deferredGraphicMap->getResolveTexture());
	semantic->setTexParam(MaterialSemantic::DeferredLightMap, _deferredLightMap->getResolveTexture());
	semantic->setTexParam(MaterialSemantic::DepthMap, _deferredDepthMap->getResolveTexture());
	semantic->setTexParam(MaterialSemantic::ColorMap, _deferredShadingMap->getResolveTexture());
	semantic->setTexParam(MaterialSemantic::NormalMap, _deferredNormalMap->getResolveTexture());
}

void
DeferredRenderPipeline::onDeactivate(RenderPipeline& pipeline) noexcept
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
DeferredRenderPipeline::onRenderPre(RenderPipeline& pipeline) noexcept
{
}

void
DeferredRenderPipeline::onRenderPipeline(RenderPipeline& pipeline) noexcept
{
	auto camera = pipeline.getCamera();
	if (!camera)
		return;

	auto cameraOrder = camera->getCameraOrder();
	switch (cameraOrder)
	{
	case CO_SHADOW:
		this->renderShadowMap(pipeline);
		break;
	case CO_LIGHT:
		this->renderLights(pipeline, camera->getRenderTexture());
		break;
	case CO_MAIN:
	{
		switch (camera->getCameraType())
		{
		case CameraType::CT_PERSPECTIVE:
			this->render3DEnvMap(pipeline);
			break;
		case CameraType::CT_ORTHO:
			this->render2DEnvMap(pipeline);
			break;
		default:
			break;
		}
	}
	break;
	}
}

void
DeferredRenderPipeline::onRenderPost(RenderPipeline& pipeline) noexcept
{
	auto camera = pipeline.getCamera();
	if (camera->getCameraRender() == CR_RENDER_TO_SCREEN)
	{
		auto renderTexture = camera->getRenderTexture();
		if (!renderTexture)
			renderTexture = _deferredShadingMap;

		auto v1 = Viewport(0, 0, renderTexture->getWidth(), renderTexture->getHeight());
		auto v2 = camera->getViewport();
		if (v2.width == 0 || v2.height == 0)
		{
			std::uint32_t width, height;
			pipeline.getWindowResolution(width, height);

			v2.left = 0;
			v2.top = 0;
			v2.width = width;
			v2.height = height;
		}

		this->copyRenderTexture(pipeline, renderTexture, nullptr, v2);
	}

	/*if (_deferredGraphicMap)
	this->blitRenderTexture(_deferredGraphicMap, Viewport(0, 0, 1376, 768), 0, Viewport(0, 768 / 2, 1376 / 3, 768));
	if (_deferredNormalMap)
	this->blitRenderTexture(_deferredNormalMap, Viewport(0, 0, 1376, 768), 0, Viewport(1376 / 3, 768 / 2, 1376 / 3 * 2, 768));
	if (_deferredLightMap)
	this->blitRenderTexture(_deferredLightMap, Viewport(0, 0, 1376, 768), 0, Viewport(1376 / 3 * 2, 768 / 2, 1376, 768));
	if (_deferredShadingMap)
	this->blitRenderTexture(_deferredShadingMap, Viewport(0, 0, 1376, 768), 0, Viewport(1376 / 3 * 2, 0, 1376, 768 / 2));*/
}

_NAME_END