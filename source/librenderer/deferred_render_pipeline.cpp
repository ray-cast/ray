// +----------------------------------------------------------------------
// | Project : ray.
// | All rights reserved.
// +----------------------------------------------------------------------
// | Copyright (c) 2013-2016.
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

	_materialDepthMap = std::make_shared<MaterialSemantic>("DepthMap", ShaderVariantType::Texture);
	_materialColorMap = std::make_shared<MaterialSemantic>("ColorMap", ShaderVariantType::Texture);
	_materialNormalMap = std::make_shared<MaterialSemantic>("NormalMap", ShaderVariantType::Texture);
	_materialDeferredDepthMap = std::make_shared<MaterialSemantic>("DeferredDepthMap", ShaderVariantType::Texture);
	_materialDeferredDepthLinearMap = std::make_shared<MaterialSemantic>("DeferredDepthLinearMap", ShaderVariantType::Texture);
	_materialDeferredGraphicMap = std::make_shared<MaterialSemantic>("DeferredGraphicMap", ShaderVariantType::Texture);
	_materialDeferredNormalMap = std::make_shared<MaterialSemantic>("DeferredNormalMap", ShaderVariantType::Texture);
	_materialDeferredLightMap = std::make_shared<MaterialSemantic>("DeferredLightMap", ShaderVariantType::Texture);
	_materialDeferredShadowMap = std::make_shared<MaterialSemantic>("DeferredShadowMap", ShaderVariantType::Texture);
}

DeferredRenderPipeline::~DeferredRenderPipeline() noexcept
{
}

void
DeferredRenderPipeline::renderShadowMap(RenderPipeline& pipeline) noexcept
{
	pipeline.setRenderTexture(pipeline.getCamera()->getRenderTexture());
	pipeline.clearRenderTexture(ClearFlags::Default, Vector4::Zero, 1.0, 0);

	pipeline.drawRenderQueue(RenderQueue::RQ_OPAQUE, RenderPass::RP_OPAQUES, _deferredDepthOnly);
	pipeline.drawRenderQueue(RenderQueue::RQ_TRANSPARENT, RenderPass::RP_TRANSPARENT, _deferredDepthOnly);
}

void
DeferredRenderPipeline::render2DEnvMap(RenderPipeline& pipeline) noexcept
{
	pipeline.setRenderTexture(_deferredShadingMap);
	pipeline.clearRenderTexture(ClearFlags::Default, pipeline.getCamera()->getClearColor(), 1.0, 0);
	pipeline.drawRenderQueue(RenderQueue::RQ_OPAQUE, RenderPass::RP_OPAQUES);
}

void
DeferredRenderPipeline::render3DEnvMap(RenderPipeline& pipeline) noexcept
{
	_materialDeferredDepthMap->assign(_deferredDepthMap->getResolveTexture());
	_materialDeferredDepthLinearMap->assign(_deferredDepthLinearMap->getResolveTexture());
	_materialDeferredNormalMap->assign(_deferredNormalMap->getResolveTexture());
	_materialDeferredGraphicMap->assign(_deferredGraphicMap->getResolveTexture());
	_materialDeferredLightMap->assign(_deferredLightMap->getResolveTexture());

	_materialDepthMap->assign(_deferredDepthMap->getResolveTexture());
	_materialColorMap->assign(_deferredShadingMap->getResolveTexture());
	_materialNormalMap->assign(_deferredNormalMap->getResolveTexture());

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
DeferredRenderPipeline::renderOpaques(RenderPipeline& pipeline, GraphicsMultiRenderTexturePtr target) noexcept
{
	pipeline.setMultiRenderTexture(target);
	pipeline.clearRenderTexture(ClearFlags::Default, Vector4::Zero, 1.0, 0);
	pipeline.drawRenderQueue(RenderQueue::RQ_OPAQUE, RenderPass::RP_OPAQUES);
}

void
DeferredRenderPipeline::renderOpaquesDepthLinear(RenderPipeline& pipeline, GraphicsRenderTexturePtr target) noexcept
{
	_clipInfo->assign(pipeline.getCamera()->getClipConstant());
	_projInfo->assign(pipeline.getCamera()->getProjConstant());

	pipeline.setRenderTexture(target);
	pipeline.clearRenderTexture(ClearFlags::Default, Vector4::Zero, 1.0, 0);
	pipeline.drawScreenQuad(_deferredDepthLinear);
}

void
DeferredRenderPipeline::renderOpaquesShading(RenderPipeline& pipeline, GraphicsRenderTexturePtr target, int layer) noexcept
{
	pipeline.setRenderTexture(target);
	pipeline.clearRenderTexture(ClearFlags::Color, Vector4::Zero, 1.0, 0);
	pipeline.drawScreenQuad(_deferredShadingOpaques);
}

void
DeferredRenderPipeline::renderOpaquesSpecificShading(RenderPipeline& pipeline, GraphicsRenderTexturePtr target, int layer) noexcept
{
	pipeline.setRenderTexture(target);
	pipeline.drawRenderQueue(RenderQueue::RQ_OPAQUE, RenderPass::RP_SPECIFIC);
	pipeline.drawRenderQueue(RenderQueue::RQ_OPAQUE, RenderPass::RP_POSTPROCESS, nullptr, target);
}

void
DeferredRenderPipeline::renderTransparent(RenderPipeline& pipeline, GraphicsMultiRenderTexturePtr renderTexture) noexcept
{
	pipeline.setMultiRenderTexture(renderTexture);
	pipeline.clearRenderTexture(ClearFlags::ColorStencil, Vector4::Zero, 1.0, 0, 0);

	pipeline.drawRenderQueue(RenderQueue::RQ_TRANSPARENT, RenderPass::RP_TRANSPARENT);
}

void
DeferredRenderPipeline::renderTransparentDepthLinear(RenderPipeline& pipeline, GraphicsRenderTexturePtr target) noexcept
{
	pipeline.setRenderTexture(target);
	pipeline.clearRenderTexture(ClearFlags::Color, Vector4::Zero, 1.0, 0, 0);

	pipeline.drawRenderQueue(RenderQueue::RQ_TRANSPARENT, RenderPass::RP_DEPTH, _deferredDepthLinear);
}

void
DeferredRenderPipeline::renderTransparentShading(RenderPipeline& pipeline, GraphicsRenderTexturePtr target, int layer) noexcept
{
	pipeline.setRenderTexture(target);
	pipeline.setRenderTextureLayer(target, layer);
	pipeline.drawScreenQuad(_deferredShadingTransparents);
}

void
DeferredRenderPipeline::renderTransparentSpecificShading(RenderPipeline& pipeline, GraphicsRenderTexturePtr target, int layer) noexcept
{
	pipeline.setRenderTexture(target);
	pipeline.setRenderTextureLayer(target, layer);
	pipeline.drawRenderQueue(RenderQueue::RQ_TRANSPARENT, RenderPass::RP_SPECIFIC);
	pipeline.drawRenderQueue(RenderQueue::RQ_TRANSPARENT, RenderPass::RP_POSTPROCESS);
}

void
DeferredRenderPipeline::renderLights(RenderPipeline& pipeline, GraphicsRenderTexturePtr target) noexcept
{
	pipeline.setRenderTexture(target);
	pipeline.clearRenderTexture(ClearFlags::Color, Vector4::Zero, 1.0, 0);

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
	_lightDirection->assign(~(light.getTransform().getTranslate() - light.getLightLookat()) * float3x3(pipeline.getCamera()->getView()));
	_lightPosition->assign(light.getTransform().getTranslate());
	_lightAttenuation->assign(light.getLightAttenuation());
	_lightRange->assign(light.getRange());

	_eyePosition->assign(pipeline.getCamera()->getTranslate() * float3x3(pipeline.getCamera()->getView()));

	if (light.getShadow())
	{
		_shadowFactor->assign(ESM_FACTOR);
		_shadowMap->assign(light.getShadowCamera()->getRenderTexture()->getResolveTexture());
		_shadowMatrix->assign(light.getShadowCamera()->getViewProject());

		pipeline.drawScreenQuad(_deferredSunLightShadow);
	}
	else
	{
		pipeline.drawScreenQuad(_deferredSunLight);
	}
}

void
DeferredRenderPipeline::renderDirectionalLight(RenderPipeline& pipeline, const Light& light) noexcept
{
	_lightColor->assign(light.getLightColor() * light.getIntensity());
	_lightDirection->assign(~(light.getTransform().getTranslate() - light.getLightLookat()) * float3x3(pipeline.getCamera()->getView()));
	_lightPosition->assign(light.getTransform().getTranslate());
	_lightAttenuation->assign(light.getLightAttenuation());
	_lightRange->assign(light.getRange());

	_eyePosition->assign(pipeline.getCamera()->getTranslate() * float3x3(pipeline.getCamera()->getView()));

	if (light.getShadow())
	{
		_shadowMap->assign(light.getShadowCamera()->getRenderTexture()->getResolveTexture());
		_shadowFactor->assign(ESM_FACTOR);
		_shadowMatrix->assign(light.getShadowCamera()->getViewProject());

		pipeline.drawScreenQuad(_deferredDirectionalLightShadow);
	}
	else
	{
		pipeline.drawScreenQuad(_deferredDirectionalLight);
	}
}

void
DeferredRenderPipeline::renderPointLight(RenderPipeline& pipeline, const Light& light) noexcept
{
	_lightColor->assign(light.getLightColor() * light.getIntensity());
	_lightDirection->assign(~(light.getTransform().getTranslate() - light.getLightLookat()) * float3x3(pipeline.getCamera()->getView()));
	_lightPosition->assign(light.getTransform().getTranslate());
	_lightAttenuation->assign(light.getLightAttenuation());
	_lightRange->assign(light.getRange());
	_lightIntensity->assign(light.getIntensity());

	_eyePosition->assign(pipeline.getCamera()->getTranslate() * float3x3(pipeline.getCamera()->getView()));

	pipeline.setModelMatrix(light.getTransform());
	pipeline.drawSphere(_deferredPointLight);
}

void
DeferredRenderPipeline::renderSpotLight(RenderPipeline& pipeline, const Light& light) noexcept
{
	_lightColor->assign(light.getLightColor() * light.getIntensity());
	_lightDirection->assign(~(light.getTransform().getTranslate() - light.getLightLookat()) * float3x3(pipeline.getCamera()->getView()));
	_lightPosition->assign(light.getTransform().getTranslate());
	_lightAttenuation->assign(light.getLightAttenuation());
	_lightRange->assign(light.getRange());
	_lightIntensity->assign(light.getIntensity());
	_lightSpotInnerCone->assign(light.getSpotInnerCone());
	_lightSpotOuterCone->assign(light.getSpotOuterCone());

	_eyePosition->assign(pipeline.getCamera()->getTranslate() * float3x3(pipeline.getCamera()->getView()));

	pipeline.setModelMatrix(light.getTransform());
	pipeline.drawCone(_deferredSpotLight);
}

void
DeferredRenderPipeline::renderAmbientLight(RenderPipeline& pipeline, const Light& light) noexcept
{
	_lightColor->assign(light.getLightColor() * light.getIntensity());
	_lightDirection->assign(~(light.getTransform().getTranslate() - light.getLightLookat()) * float3x3(pipeline.getCamera()->getView()));
	_lightPosition->assign(light.getTransform().getTranslate());
	_lightAttenuation->assign(light.getLightAttenuation());
	_lightRange->assign(light.getRange());
	_lightIntensity->assign(light.getIntensity());

	_eyePosition->assign(pipeline.getCamera()->getTranslate());

	pipeline.drawScreenQuad(_deferredAmbientLight);
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
DeferredRenderPipeline::copyRenderTexture(RenderPipeline& pipeline, GraphicsRenderTexturePtr src, GraphicsRenderTexturePtr dst, const Viewport& view) noexcept
{
	_texSource->assign(src->getResolveTexture());
	pipeline.setRenderTexture(dst);
	pipeline.drawScreenQuad(_deferredCopyOnly);
}

void
DeferredRenderPipeline::setupSemantic(RenderPipeline& pipeline)
{
	auto materialManager = pipeline.getMaterialManager();
	materialManager->addSemantic(_materialDepthMap);
	materialManager->addSemantic(_materialColorMap);
	materialManager->addSemantic(_materialNormalMap);
	materialManager->addSemantic(_materialDeferredDepthMap);
	materialManager->addSemantic(_materialDeferredDepthLinearMap);
	materialManager->addSemantic(_materialDeferredGraphicMap);
	materialManager->addSemantic(_materialDeferredNormalMap);
	materialManager->addSemantic(_materialDeferredLightMap);
	materialManager->addSemantic(_materialDeferredShadowMap);
}

void
DeferredRenderPipeline::setupMaterials(RenderPipeline& pipeline)
{
	_deferredLighting = pipeline.createMaterial("sys:fx\\deferred_lighting.fxml.o");
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
}

void
DeferredRenderPipeline::setupRenderTextures(RenderPipeline& pipeline)
{
	std::uint32_t width, height;
	pipeline.getWindowResolution(width, height);

	_deferredDepthMap = pipeline.createRenderTexture(width, height, TextureDim::DIM_2D, TextureFormat::DEPTH32_STENCIL8);
	_deferredDepthLinearMap = pipeline.createRenderTexture(width, height, TextureDim::DIM_2D, TextureFormat::R32F);
	_deferredGraphicMap = pipeline.createRenderTexture(width, height, TextureDim::DIM_2D, TextureFormat::R8G8B8A8);
	_deferredNormalMap = pipeline.createRenderTexture(width, height, TextureDim::DIM_2D, TextureFormat::R8G8B8A8);

	GraphicsRenderTextureDesc deferredLightDesc;
	deferredLightDesc.setSharedStencilTexture(_deferredDepthMap);
	deferredLightDesc.setGraphicsTexture(pipeline.createTexture(width, height, TextureDim::DIM_2D, TextureFormat::R8G8B8A8));
	_deferredLightMap = pipeline.createRenderTexture(deferredLightDesc);

	GraphicsRenderTextureDesc deferredShadingDesc;
	deferredShadingDesc.setSharedDepthTexture(_deferredDepthMap);
	deferredShadingDesc.setSharedStencilTexture(_deferredDepthMap);
	deferredShadingDesc.setGraphicsTexture(pipeline.createTexture(width, height, TextureDim::DIM_2D, TextureFormat::R8G8B8A8));
	_deferredShadingMap = pipeline.createRenderTexture(deferredShadingDesc);

	GraphicsMultiRenderTextureDesc deferredGraphicDesc;
	deferredGraphicDesc.setSharedDepthTexture(_deferredDepthMap);
	deferredGraphicDesc.setSharedStencilTexture(_deferredDepthMap);
	deferredGraphicDesc.attach(_deferredGraphicMap);
	deferredGraphicDesc.attach(_deferredNormalMap);

	_deferredGraphicMaps = pipeline.createMultiRenderTexture(deferredGraphicDesc);
}

void
DeferredRenderPipeline::destroySemantic(RenderPipeline& pipeline)
{
	auto materialManager = pipeline.getMaterialManager();
	materialManager->removeSemantic(_materialDepthMap);
	materialManager->removeSemantic(_materialColorMap);
	materialManager->removeSemantic(_materialNormalMap);
	materialManager->removeSemantic(_materialDeferredDepthMap);
	materialManager->removeSemantic(_materialDeferredDepthLinearMap);
	materialManager->removeSemantic(_materialDeferredGraphicMap);
	materialManager->removeSemantic(_materialDeferredNormalMap);
	materialManager->removeSemantic(_materialDeferredLightMap);
	materialManager->removeSemantic(_materialDeferredShadowMap);
}

void
DeferredRenderPipeline::destroyMaterials(RenderPipeline& pipeline)
{
	_deferredLighting.reset();
	_deferredDepthOnly.reset();
	_deferredDepthLinear.reset();
	_deferredSunLight.reset();
	_deferredSunLightShadow.reset();
	_deferredDirectionalLight.reset();
	_deferredDirectionalLightShadow.reset();
	_deferredSpotLight.reset();
	_deferredPointLight.reset();
	_deferredAmbientLight.reset();
	_deferredShadingOpaques.reset();
	_deferredShadingTransparents.reset();
	_deferredDebugLayer.reset();
	_deferredCopyOnly.reset();

	_texMRT0.reset();
	_texMRT1.reset();
	_texDepth.reset();
	_texLight.reset();
	_texSource.reset();
	_texEnvironmentMap.reset();

	_eyePosition.reset();
	_clipInfo.reset();
	_projInfo.reset();

	_shadowDecal.reset();
	_shadowChannel.reset();
	_shadowMap.reset();
	_shadowArrayMap.reset();
	_shadowFactor.reset();
	_shadowMatrix.reset();
	_shadowOffset.reset();
	_shadowWeight.reset();

	_lightColor.reset();
	_lightPosition.reset();
	_lightDirection.reset();
	_lightRange.reset();
	_lightIntensity.reset();
	_lightAttenuation.reset();
	_lightSpotInnerCone.reset();
	_lightSpotOuterCone.reset();
}

void
DeferredRenderPipeline::destroyRenderTextures(RenderPipeline& pipeline)
{
	_deferredDepthMap.reset();
	_deferredDepthLinearMap.reset();
	_deferredGraphicMap.reset();
	_deferredNormalMap.reset();
	_deferredLightMap.reset();
	_deferredGraphicMaps.reset();
}

void
DeferredRenderPipeline::onActivate(RenderPipeline& pipeline) except
{
	this->setupSemantic(pipeline);
	this->setupMaterials(pipeline);
	this->setupRenderTextures(pipeline);
}

void
DeferredRenderPipeline::onDeactivate(RenderPipeline& pipeline) noexcept
{
	this->destroySemantic(pipeline);
	this->destroyMaterials(pipeline);
	this->destroyRenderTextures(pipeline);
}

void 
DeferredRenderPipeline::onResolutionChangeBefore(RenderPipeline& pipeline) noexcept
{
}

void 
DeferredRenderPipeline::onResolutionChangeAfter(RenderPipeline& pipeline) noexcept
{
	destroyRenderTextures(pipeline);
	setupRenderTextures(pipeline);
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

		auto& textureDesc = renderTexture->getResolveTexture()->getGraphicsTextureDesc();
		auto v1 = Viewport(0, 0, textureDesc.getWidth(), textureDesc.getHeight());
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
		pipeline.blitRenderTexture(_deferredGraphicMap, Viewport(0, 0, 1376, 768), 0, Viewport(0, 768 / 2, 1376 / 3, 768));
	if (_deferredNormalMap)
		pipeline.blitRenderTexture(_deferredNormalMap, Viewport(0, 0, 1376, 768), 0, Viewport(1376 / 3, 768 / 2, 1376 / 3 * 2, 768));
	if (_deferredLightMap)
		pipeline.blitRenderTexture(_deferredLightMap, Viewport(0, 0, 1376, 768), 0, Viewport(1376 / 3 * 2, 768 / 2, 1376, 768));
	if (_deferredShadingMap)
		pipeline.blitRenderTexture(_deferredShadingMap, Viewport(0, 0, 1376, 768), 0, Viewport(1376 / 3 * 2, 0, 1376, 768 / 2));*/
}

_NAME_END