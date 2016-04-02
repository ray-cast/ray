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
#include <ray/graphics_texture.h>
#include <ray/graphics_framebuffer.h>
#include <ray/material_manager.h>

_NAME_BEGIN

const float ESM_FACTOR = 300.0f;

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
	pipeline.clearRenderTexture(GraphicsClearFlags::GraphicsClearFlagsDepth, Vector4::Zero, 1.0, 0);
	pipeline.drawRenderQueue(RenderQueue::RenderQueueOpaque, RenderPass::RenderPassOpaques, _deferredDepthOnly);
}

void
DeferredRenderPipeline::render2DEnvMap(RenderPipeline& pipeline) noexcept
{
	pipeline.setRenderTexture(_deferredFinalView);
	pipeline.setViewport(Viewport(0, 0, 1376, 768), 0);
	pipeline.clearRenderTexture(GraphicsClearFlags::GraphicsClearFlagsAll, pipeline.getCamera()->getClearColor(), 1.0, 0);
	pipeline.drawRenderQueue(RenderQueue::RenderQueueOpaque, RenderPass::RenderPassOpaques);
}

void
DeferredRenderPipeline::render3DEnvMap(RenderPipeline& pipeline) noexcept
{
	_materialDeferredDepthMap->assign(_deferredDepthMap);
	_materialDeferredDepthLinearMap->assign(_deferredDepthLinearMap);
	_materialDeferredNormalMap->assign(_deferredNormalMap);
	_materialDeferredGraphicMap->assign(_deferredGraphicMap);
	_materialDeferredLightMap->assign(_deferredLightMap);

	_materialDepthMap->assign(_deferredDepthMap);
	_materialColorMap->assign(_deferredShadingMap);
	_materialNormalMap->assign(_deferredNormalMap);

	this->renderOpaques(pipeline, _deferredGraphicViews);
	this->renderOpaquesDepthLinear(pipeline, _deferredDepthLinearView);
	this->renderLights(pipeline, _deferredLightView);
	this->renderOpaquesShading(pipeline, _deferredShadingView);
	this->renderOpaquesSpecificShading(pipeline, _deferredShadingView);

	/*this->renderTransparent(_deferredGraphicViews);
	this->renderLights(_deferredLightView);
	this->renderTransparentShading(_deferredShadingView);
	this->renderTransparentSpecificShading(_deferredShadingView);*/

	pipeline.drawPostProcess(RenderQueue::RenderQueuePostprocess, _deferredShadingMap, _deferredSwapView, _deferredFinalView);

	auto renderTexture = pipeline.getCamera()->getRenderTexture();
	if (renderTexture)
		this->copyRenderTexture(pipeline, _deferredFinalMap, renderTexture, pipeline.getCamera()->getViewport());
}

void
DeferredRenderPipeline::renderOpaques(RenderPipeline& pipeline, GraphicsFramebufferPtr target) noexcept
{
	pipeline.setRenderTexture(target);
	pipeline.clearRenderTexture(GraphicsClearFlags::GraphicsClearFlagsAll, Vector4::Zero, 1.0, 0);
	pipeline.drawRenderQueue(RenderQueue::RenderQueueOpaque, RenderPass::RenderPassOpaques);
}

void
DeferredRenderPipeline::renderOpaquesDepthLinear(RenderPipeline& pipeline, GraphicsFramebufferPtr target) noexcept
{
	_clipInfo->assign(pipeline.getCamera()->getClipConstant());
	_projInfo->assign(pipeline.getCamera()->getProjConstant());

	pipeline.setRenderTexture(target);
	pipeline.clearRenderTexture(GraphicsClearFlags::GraphicsClearFlagsAll, Vector4::Zero, 1.0, 0);
	pipeline.drawScreenQuad(_deferredDepthLinear);
}

void
DeferredRenderPipeline::renderOpaquesShading(RenderPipeline& pipeline, GraphicsFramebufferPtr target) noexcept
{
	pipeline.setRenderTexture(target);
	pipeline.clearRenderTexture(GraphicsClearFlags::GraphicsClearFlagsColor, pipeline.getCamera()->getClearColor(), 1.0, 0);
	pipeline.drawScreenQuad(_deferredShadingOpaques);
}

void
DeferredRenderPipeline::renderOpaquesSpecificShading(RenderPipeline& pipeline, GraphicsFramebufferPtr target) noexcept
{
	pipeline.setRenderTexture(target);
	pipeline.drawRenderQueue(RenderQueue::RenderQueueOpaque, RenderPass::RenderPassSpecific);
	pipeline.drawPostProcess(RenderQueue::RenderQueueOpaque, target->getGraphicsFramebufferDesc().getTextures()[0], target, _deferredFinalView);
}

void
DeferredRenderPipeline::renderTransparent(RenderPipeline& pipeline, GraphicsFramebufferPtr renderTexture) noexcept
{
	pipeline.setRenderTexture(renderTexture);
	pipeline.clearRenderTexture(GraphicsClearFlags::GraphicsClearFlagsColorStencil, Vector4::Zero, 1.0, 0, 0);
	pipeline.drawRenderQueue(RenderQueue::RenderQueueTransparent, RenderPass::RenderPassTransparent);
}

void
DeferredRenderPipeline::renderTransparentDepthLinear(RenderPipeline& pipeline, GraphicsFramebufferPtr target) noexcept
{
	pipeline.setRenderTexture(target);
	pipeline.clearRenderTexture(GraphicsClearFlags::GraphicsClearFlagsColor, Vector4::Zero, 1.0, 0, 0);
	pipeline.drawRenderQueue(RenderQueue::RenderQueueTransparent, RenderPass::RenderPassDepth, _deferredDepthLinear);
}

void
DeferredRenderPipeline::renderTransparentShading(RenderPipeline& pipeline, GraphicsFramebufferPtr target) noexcept
{
	pipeline.setRenderTexture(target);
	pipeline.drawScreenQuad(_deferredShadingTransparents);
}

void
DeferredRenderPipeline::renderTransparentSpecificShading(RenderPipeline& pipeline, GraphicsFramebufferPtr target) noexcept
{
	pipeline.setRenderTexture(target);
	pipeline.drawRenderQueue(RenderQueue::RenderQueueTransparent, RenderPass::RenderPassSpecific);
	pipeline.drawRenderQueue(RenderQueue::RenderQueueTransparent, RenderPass::RenderPassPostprocess);
}

void
DeferredRenderPipeline::renderLights(RenderPipeline& pipeline, GraphicsFramebufferPtr target) noexcept
{
	pipeline.setRenderTexture(target);
	pipeline.clearRenderTexture(GraphicsClearFlags::GraphicsClearFlagsColor, Vector4::Zero, 1.0, 0);

	auto& lights = pipeline.getRenderData(RenderQueue::RenderQueueLighting, RenderPass::RenderPassLights);
	for (auto& it : lights)
	{
		auto light = std::dynamic_pointer_cast<Light>(it);

		switch (light->getLightType())
		{
		case LightType::LightTypeSun:
			this->renderSunLight(pipeline, *light);
			break;
		case LightType::LightTypeDirectional:
			this->renderDirectionalLight(pipeline, *light);
			break;
		case LightType::LightTypePoint:
			this->renderPointLight(pipeline, *light);
			break;
		case LightType::LightTypeArea:
			this->renderAreaLight(pipeline, *light);
			break;
		case LightType::LightTypeSpot:
			this->renderSpotLight(pipeline, *light);
			break;
		case LightType::LightTypeHemiSphere:
			this->renderHemiSphereLight(pipeline, *light);
			break;
		case LightType::LightTypeAmbient:
			this->renderAmbientLight(pipeline, *light);
			break;
		default:
			break;
		}
	}

	pipeline.drawPostProcess(RenderQueue::RenderQueueLighting, target->getGraphicsFramebufferDesc().getTextures()[0], target, _deferredFinalView);
}

void
DeferredRenderPipeline::renderSunLight(RenderPipeline& pipeline, const Light& light) noexcept
{
	_lightColor->assign(light.getLightColor() * light.getIntensity());
	_lightDirection->assign(-light.getForward() * float3x3(pipeline.getCamera()->getView()));
	_lightPosition->assign(light.getTransform().getTranslate());
	_lightAttenuation->assign(light.getLightAttenuation());
	_lightRange->assign(light.getRange());

	_eyePosition->assign(pipeline.getCamera()->getTranslate() * float3x3(pipeline.getCamera()->getView()));

	if (light.getShadow())
	{
		_shadowFactor->assign(float2(ESM_FACTOR, 0.001));
		_shadowMap->assign(light.getShadowMap());
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
	_lightDirection->assign(-light.getForward() * float3x3(pipeline.getCamera()->getView()));
	_lightPosition->assign(light.getTransform().getTranslate());
	_lightAttenuation->assign(light.getLightAttenuation());
	_lightRange->assign(light.getRange());

	_eyePosition->assign(pipeline.getCamera()->getTranslate() * float3x3(pipeline.getCamera()->getView()));

	if (light.getShadow())
	{
		_shadowFactor->assign(float2(ESM_FACTOR, 0.001));
		_shadowMap->assign(light.getShadowMap());
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
	_lightDirection->assign(-light.getForward() * float3x3(pipeline.getCamera()->getView()));
	_lightPosition->assign(light.getTransform().getTranslate());
	_lightAttenuation->assign(light.getLightAttenuation());
	_lightRange->assign(light.getRange());
	_lightIntensity->assign(light.getIntensity());

	_eyePosition->assign(pipeline.getCamera()->getView().getTranslate() * float3x3(pipeline.getCamera()->getView()));

	pipeline.drawSphere(_deferredPointLight, light.getTransform());
}

void
DeferredRenderPipeline::renderSpotLight(RenderPipeline& pipeline, const Light& light) noexcept
{
	_lightColor->assign(light.getLightColor() * light.getIntensity());
	_lightDirection->assign(-light.getForward() * float3x3(pipeline.getCamera()->getView()));
	_lightPosition->assign(light.getTransform().getTranslate());
	_lightAttenuation->assign(light.getLightAttenuation());
	_lightRange->assign(light.getRange());
	_lightIntensity->assign(light.getIntensity());
	_lightSpotInnerCone->assign(light.getSpotInnerCone());
	_lightSpotOuterCone->assign(light.getSpotOuterCone());

	_eyePosition->assign(pipeline.getCamera()->getView().getTranslate() * float3x3(pipeline.getCamera()->getView()));

	pipeline.drawCone(_deferredSpotLight, light.getTransform());
}

void
DeferredRenderPipeline::renderAmbientLight(RenderPipeline& pipeline, const Light& light) noexcept
{
	_lightColor->assign(light.getLightColor() * light.getIntensity());
	_lightDirection->assign(-light.getForward() * float3x3(pipeline.getCamera()->getView()));
	_lightPosition->assign(light.getTransform().getTranslate());
	_lightAttenuation->assign(light.getLightAttenuation());
	_lightRange->assign(light.getRange());
	_lightIntensity->assign(light.getIntensity());

	_eyePosition->assign(pipeline.getCamera()->getView().getTranslate());

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
DeferredRenderPipeline::copyRenderTexture(RenderPipeline& pipeline, GraphicsTexturePtr src, GraphicsFramebufferPtr dst, const Viewport& view) noexcept
{
	_texSource->assign(src);
	pipeline.setRenderTexture(dst);
	pipeline.drawScreenQuad(_deferredCopyOnly);
}

void
DeferredRenderPipeline::setupSemantic(RenderPipeline& pipeline)
{
	auto materialManager = pipeline.getMaterialManager();

	_materialDepthMap = std::make_shared<MaterialVariant>("DepthMap", GraphicsUniformType::GraphicsUniformTypeStorageImage);
	_materialColorMap = std::make_shared<MaterialVariant>("ColorMap", GraphicsUniformType::GraphicsUniformTypeStorageImage);
	_materialNormalMap = std::make_shared<MaterialVariant>("NormalMap", GraphicsUniformType::GraphicsUniformTypeStorageImage);
	_materialDeferredDepthMap = std::make_shared<MaterialVariant>("DeferredDepthMap", GraphicsUniformType::GraphicsUniformTypeStorageImage);
	_materialDeferredDepthLinearMap = std::make_shared<MaterialVariant>("DeferredDepthLinearMap", GraphicsUniformType::GraphicsUniformTypeStorageImage);
	_materialDeferredGraphicMap = std::make_shared<MaterialVariant>("DeferredGraphicMap", GraphicsUniformType::GraphicsUniformTypeStorageImage);
	_materialDeferredNormalMap = std::make_shared<MaterialVariant>("DeferredNormalMap", GraphicsUniformType::GraphicsUniformTypeStorageImage);
	_materialDeferredLightMap = std::make_shared<MaterialVariant>("DeferredLightMap", GraphicsUniformType::GraphicsUniformTypeStorageImage);
	_materialDeferredShadowMap = std::make_shared<MaterialVariant>("DeferredShadowMap", GraphicsUniformType::GraphicsUniformTypeStorageImage);

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
	_deferredDepthOnly = _deferredLighting->getTech(RenderQueue::RenderQueueCustom)->getPass("DeferredDepthOnly");
	_deferredDepthLinear = _deferredLighting->getTech(RenderQueue::RenderQueueCustom)->getPass("DeferredDepthLinear");
	_deferredPointLight = _deferredLighting->getTech(RenderQueue::RenderQueueCustom)->getPass("DeferredPointLight");
	_deferredAmbientLight = _deferredLighting->getTech(RenderQueue::RenderQueueCustom)->getPass("DeferredAmbientLight");
	_deferredSunLight = _deferredLighting->getTech(RenderQueue::RenderQueueCustom)->getPass("DeferredSunLight");
	_deferredSunLightShadow = _deferredLighting->getTech(RenderQueue::RenderQueueCustom)->getPass("DeferredSunLightShadow");
	_deferredDirectionalLight = _deferredLighting->getTech(RenderQueue::RenderQueueCustom)->getPass("DirectionalLight");
	_deferredDirectionalLightShadow = _deferredLighting->getTech(RenderQueue::RenderQueueCustom)->getPass("DirectionalLightShadow");
	_deferredSpotLight = _deferredLighting->getTech(RenderQueue::RenderQueueCustom)->getPass("DeferredSpotLight");
	_deferredShadingOpaques = _deferredLighting->getTech(RenderQueue::RenderQueueCustom)->getPass("DeferredShadingOpaques");
	_deferredShadingTransparents = _deferredLighting->getTech(RenderQueue::RenderQueueCustom)->getPass("DeferredShadingTransparents");
	_deferredCopyOnly = _deferredLighting->getTech(RenderQueue::RenderQueueCustom)->getPass("DeferredCopyOnly");

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

	_deferredDepthMap = pipeline.createTexture(width, height, GraphicsTextureDim::GraphicsTextureDim2D, GraphicsFormat::GraphicsFormatD24UNorm_S8UInt);
	_deferredDepthLinearMap = pipeline.createTexture(width, height, GraphicsTextureDim::GraphicsTextureDim2D, GraphicsFormat::GraphicsFormatR32SFloat);
	_deferredGraphicMap = pipeline.createTexture(width, height, GraphicsTextureDim::GraphicsTextureDim2D, GraphicsFormat::GraphicsFormatR8G8B8A8UNorm);
	_deferredNormalMap = pipeline.createTexture(width, height, GraphicsTextureDim::GraphicsTextureDim2D, GraphicsFormat::GraphicsFormatR8G8B8A8UNorm);
	_deferredLightMap = pipeline.createTexture(width, height, GraphicsTextureDim::GraphicsTextureDim2D, GraphicsFormat::GraphicsFormatR16G16B16A16SFloat);
	_deferredShadingMap = pipeline.createTexture(width, height, GraphicsTextureDim::GraphicsTextureDim2D, GraphicsFormat::GraphicsFormatR8G8B8A8UNorm);
	_deferredSwapMap = pipeline.createTexture(width, height, GraphicsTextureDim::GraphicsTextureDim2D, GraphicsFormat::GraphicsFormatR8G8B8A8UNorm);
	_deferredFinalMap = pipeline.createTexture(width, height, GraphicsTextureDim::GraphicsTextureDim2D, GraphicsFormat::GraphicsFormatR8G8B8A8UNorm);

	GraphicsFramebufferDesc deferredDepthDesc;
	deferredDepthDesc.setSharedDepthTexture(_deferredDepthMap);
	deferredDepthDesc.setSharedStencilTexture(_deferredDepthMap);
	_deferredDepthView = pipeline.createRenderTexture(deferredDepthDesc);

	GraphicsFramebufferDesc deferredDepthLinearDesc;
	deferredDepthLinearDesc.attach(_deferredDepthLinearMap);
	_deferredDepthLinearView = pipeline.createRenderTexture(deferredDepthLinearDesc);

	GraphicsFramebufferDesc deferredGraphicsDesc;
	deferredGraphicsDesc.attach(_deferredGraphicMap);
	_deferredGraphicView = pipeline.createRenderTexture(deferredGraphicsDesc);

	GraphicsFramebufferDesc deferredNormalDesc;
	deferredNormalDesc.attach(_deferredNormalMap);
	_deferredNormalView = pipeline.createRenderTexture(deferredNormalDesc);

	GraphicsFramebufferDesc deferredGraphicDesc;
	deferredGraphicDesc.setSharedDepthTexture(_deferredDepthMap);
	deferredGraphicDesc.setSharedStencilTexture(_deferredDepthMap);
	deferredGraphicDesc.attach(_deferredGraphicMap);
	deferredGraphicDesc.attach(_deferredNormalMap);
	_deferredGraphicViews = pipeline.createRenderTexture(deferredGraphicDesc);

	GraphicsFramebufferDesc deferredLightDesc;
	deferredLightDesc.setSharedStencilTexture(_deferredDepthMap);
	deferredLightDesc.attach(_deferredLightMap);
	_deferredLightView = pipeline.createRenderTexture(deferredLightDesc);

	GraphicsFramebufferDesc deferredShadingDesc;
	deferredShadingDesc.setSharedDepthTexture(_deferredDepthMap);
	deferredShadingDesc.setSharedStencilTexture(_deferredDepthMap);
	deferredShadingDesc.attach(_deferredShadingMap);
	_deferredShadingView = pipeline.createRenderTexture(deferredShadingDesc);

	GraphicsFramebufferDesc deferredFinalDesc;
	deferredFinalDesc.setSharedDepthTexture(_deferredDepthMap);
	deferredFinalDesc.setSharedStencilTexture(_deferredDepthMap);
	deferredFinalDesc.attach(_deferredFinalMap);
	_deferredFinalView = pipeline.createRenderTexture(deferredFinalDesc);

	GraphicsFramebufferDesc deferredSwapDesc;
	deferredSwapDesc.setSharedDepthTexture(_deferredDepthMap);
	deferredSwapDesc.setSharedStencilTexture(_deferredDepthMap);
	deferredSwapDesc.attach(_deferredSwapMap);
	_deferredSwapView = pipeline.createRenderTexture(deferredSwapDesc);
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

	_materialDepthMap.reset();
	_materialColorMap.reset();
	_materialNormalMap.reset();
	_materialDeferredDepthMap.reset();
	_materialDeferredDepthLinearMap.reset();
	_materialDeferredGraphicMap.reset();
	_materialDeferredNormalMap.reset();
	_materialDeferredLightMap.reset();
	_materialDeferredShadowMap.reset();
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
	_deferredShadingMap.reset();
	_deferredSwapMap.reset();
	_deferredFinalMap.reset();

	_deferredDepthView.reset();
	_deferredDepthLinearView.reset();
	_deferredGraphicView.reset();
	_deferredNormalView.reset();
	_deferredLightView.reset();
	_deferredShadingView.reset();
	_deferredSwapView.reset();
	_deferredFinalView.reset();
	_deferredGraphicViews.reset();
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
	case CameraOrder::CameraOrderShadow:
		this->renderShadowMap(pipeline);
		break;
	case CameraOrder::CameraOrderLight:
		this->renderLights(pipeline, camera->getRenderTexture());
		break;
	case CameraOrder::CameraOrderMain:
	{
		switch (camera->getCameraType())
		{
		case CameraType::CameraTypePerspective:
			this->render3DEnvMap(pipeline);
			break;
		case CameraType::CameraTypeOrtho:
			this->render2DEnvMap(pipeline);
			break;
		default:
			assert(false);
			break;
		}
		break;
	}
	default:
		assert(false);
		break;
	}
}

void
DeferredRenderPipeline::onRenderPost(RenderPipeline& pipeline) noexcept
{
	auto camera = pipeline.getCamera();
	if (camera->getCameraRender() == CameraRender::CameraRenderScreen)
	{
		auto renderTexture = camera->getRenderTexture();
		if (!renderTexture)
			renderTexture = _deferredFinalView ? _deferredFinalView : _deferredShadingView;

		auto v1 = camera->getViewport();
		if (v1.width == 0 || v1.height == 0)
		{
			std::uint32_t width, height;
			pipeline.getWindowResolution(width, height);

			v1.left = 0;
			v1.top = 0;
			v1.width = width;
			v1.height = height;
		}

		this->copyRenderTexture(pipeline, renderTexture->getGraphicsFramebufferDesc().getTextures()[0], nullptr, v1);
	}
	
	/*if (_deferredGraphicView)
		pipeline.blitRenderTexture(_deferredGraphicView, Viewport(0, 0, 1376, 768), 0, Viewport(0, 768 / 2, 1376 / 3, 768));
	if (_deferredNormalView)
		pipeline.blitRenderTexture(_deferredNormalView, Viewport(0, 0, 1376, 768), 0, Viewport(1376 / 3, 768 / 2, 1376 / 3 * 2, 768));
	if (_deferredLightView)
		pipeline.blitRenderTexture(_deferredLightView, Viewport(0, 0, 1376, 768), 0, Viewport(1376 / 3 * 2, 768 / 2, 1376, 768));
	if (_deferredShadingView)
		pipeline.blitRenderTexture(_deferredShadingView, Viewport(0, 0, 1376, 768), 0, Viewport(1376 / 3 * 2, 0, 1376, 768 / 2));*/
}

_NAME_END