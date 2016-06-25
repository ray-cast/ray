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
#include "deferred_lighting_pipeline.h"
#include <ray/render_pipeline.h>
#include <ray/camera.h>
#include <ray/light.h>
#include <ray/render_scene.h>
#include <ray/graphics_state.h>
#include <ray/graphics_texture.h>
#include <ray/graphics_framebuffer.h>
#include <ray/material.h>
#include <ray/render_object_manager.h>

_NAME_BEGIN

DeferredLightingPipeline::DeferredLightingPipeline() noexcept
	: _mrsiiDerivMipBase(0)
	, _mrsiiDerivMipCount(3)
	, _shadowEsmFactor(300.0f)
{
}

DeferredLightingPipeline::~DeferredLightingPipeline() noexcept
{
	this->close();
}

bool
DeferredLightingPipeline::setup(RenderPipelinePtr pipeline) noexcept
{
	assert(pipeline);

	if (!this->initTextureFormat(*pipeline))
		return false;

	if (!this->setupSemantic(*pipeline))
		return false;

	if (!this->setupDeferredMaterials(*pipeline))
		return false;

	if (!this->setupDeferredTextures(*pipeline))
		return false;

	if (!this->setupDeferredRenderTextureLayouts(*pipeline))
		return false;

	if (!this->setupDeferredRenderTextures(*pipeline))
		return false;

	if (!this->setupMRSIIMaterials(*pipeline))
		return false;

	if (!this->setupMRSIITexture(*pipeline))
		return false;

	if (!this->setupMRSIIRenderTextureLayouts(*pipeline))
		return false;

	if (!this->setupMRSIIRenderTextures(*pipeline))
		return false;

	_pipeline = pipeline;
	return true;
}

void
DeferredLightingPipeline::close() noexcept
{
	this->destroySemantic();
	this->destroyDeferredMaterials();
	this->destroyDeferredRenderTextures();
	this->destroyDeferredRenderTextureLayouts();
	this->destroyDeferredTextures();
}

void
DeferredLightingPipeline::render3DEnvMap(const CameraPtr& camera) noexcept
{
	auto& dataManager = _pipeline->getCamera()->getRenderDataManager();
	if (!dataManager->getRenderData(RenderQueue::RenderQueueTransparent).empty() ||
		!dataManager->getRenderData(RenderQueue::RenderQueueTransparentBatch).empty() ||
		!dataManager->getRenderData(RenderQueue::RenderQueueTransparentSpecific).empty())
	{
		_texMRT0->uniformTexture(_deferredOpaqueMap);
		_texMRT1->uniformTexture(_deferredNormalMap);

		_materialDeferredGraphicMap->uniformTexture(_deferredOpaqueMap);

		this->renderOpaques(*_pipeline, _deferredOpaqueViews);
		this->renderOpaquesDepthLinear(*_pipeline, _deferredDepthLinearView);
		this->renderLights(*_pipeline, _deferredLightingView);
		this->renderOpaquesShading(*_pipeline, _deferredOpaqueShadingView);
		this->renderOpaquesSpecificShading(*_pipeline, _deferredOpaqueShadingView);

		_texMRT0->uniformTexture(_deferredTransparentMap);
		_texMRT1->uniformTexture(_deferredNormalMap);
		_texMRT2->uniformTexture(_deferredAbufferMap);

		_materialDeferredGraphicMap->uniformTexture(_deferredTransparentMap);

		_texOpaque->uniformTexture(_deferredOpaqueShadingMap);

		this->renderTransparent(*_pipeline, _deferredTransparentViews);
		this->renderTransparentDepthLinear(*_pipeline, _deferredDepthLinearView);
		this->renderLights(*_pipeline, _deferredLightingView);
		this->renderTransparentShading(*_pipeline, _deferredFinalShadingView);
		this->renderTransparentSpecificShading(*_pipeline, _deferredFinalShadingView);
	}
	else
	{
		_texMRT0->uniformTexture(_deferredOpaqueMap);
		_texMRT1->uniformTexture(_deferredNormalMap);

		_materialDeferredGraphicMap->uniformTexture(_deferredOpaqueMap);

		this->renderOpaques(*_pipeline, _deferredOpaqueViews);
		this->renderOpaquesDepthLinear(*_pipeline, _deferredDepthLinearView);
		this->renderLights(*_pipeline, _deferredLightingView);
		this->renderOpaquesShading(*_pipeline, _deferredFinalShadingView);
		this->renderOpaquesSpecificShading(*_pipeline, _deferredFinalShadingView);
	}

	if (_pipeline->drawPostProcess(RenderQueue::RenderQueuePostprocess, _deferredFinalShadingView, _deferredSwapView))
		this->copyRenderTexture(*_pipeline, _deferredSwapMap, _deferredFinalShadingView);
}

void
DeferredLightingPipeline::renderOpaques(RenderPipeline& pipeline, GraphicsFramebufferPtr& target) noexcept
{
	pipeline.setFramebuffer(target);
	pipeline.clearFramebuffer(0, GraphicsClearFlagBits::GraphicsClearFlagAllBit, float4::Zero, 1.0, 0);
	pipeline.clearFramebuffer(1, GraphicsClearFlagBits::GraphicsClearFlagColorBit, float4::Zero, 1.0, 0);

	pipeline.drawRenderQueue(RenderQueue::RenderQueueOpaque);
	pipeline.drawRenderQueue(RenderQueue::RenderQueueOpaqueBatch);
}

void
DeferredLightingPipeline::renderOpaquesDepthLinear(RenderPipeline& pipeline, GraphicsFramebufferPtr& target) noexcept
{
	_clipInfo->uniform2f(_pipeline->getCamera()->getClipConstant().xy());

	pipeline.setFramebuffer(target);
	pipeline.drawScreenQuad(*_deferredDepthLinear);
}

void
DeferredLightingPipeline::renderOpaquesShading(RenderPipeline& pipeline, GraphicsFramebufferPtr& target) noexcept
{
	pipeline.setFramebuffer(target);
	pipeline.clearFramebuffer(0, GraphicsClearFlagBits::GraphicsClearFlagColorBit, pipeline.getCamera()->getClearColor(), 1.0, 0);
	pipeline.drawScreenQuad(*_deferredShadingOpaques);
	pipeline.drawRenderQueue(RenderQueue::RenderQueueOpaqueShading);
	pipeline.drawPostProcess(RenderQueue::RenderQueueOpaqueShading, target, target);
}

void
DeferredLightingPipeline::renderOpaquesSpecificShading(RenderPipeline& pipeline, GraphicsFramebufferPtr& target) noexcept
{
	pipeline.setFramebuffer(target);
	pipeline.drawRenderQueue(RenderQueue::RenderQueueOpaqueSpecific);
	pipeline.drawPostProcess(RenderQueue::RenderQueueOpaqueSpecific, target, target);
}

void
DeferredLightingPipeline::renderTransparent(RenderPipeline& pipeline, GraphicsFramebufferPtr& target) noexcept
{
	pipeline.setFramebuffer(target);
	pipeline.clearFramebuffer(0, GraphicsClearFlagBits::GraphicsClearFlagColorStencilBit, float4::Zero, 1.0, 0);
	pipeline.clearFramebuffer(1, GraphicsClearFlagBits::GraphicsClearFlagColorBit, float4::Zero, 1.0, 0);
	pipeline.clearFramebuffer(2, GraphicsClearFlagBits::GraphicsClearFlagColorBit, float4(1.0,1.0,1.0,0.0), 1.0, 0);

	pipeline.drawRenderQueue(RenderQueue::RenderQueueTransparent);
	pipeline.drawRenderQueue(RenderQueue::RenderQueueTransparentBatch);
}

void
DeferredLightingPipeline::renderTransparentDepthLinear(RenderPipeline& pipeline, GraphicsFramebufferPtr& target) noexcept
{
	_clipInfo->uniform2f(_pipeline->getCamera()->getClipConstant().xy());

	pipeline.setFramebuffer(target);
	pipeline.drawScreenQuad(*_deferredDepthLinear);
}

void
DeferredLightingPipeline::renderTransparentShading(RenderPipeline& pipeline, GraphicsFramebufferPtr& target) noexcept
{
	pipeline.setFramebuffer(target);
	pipeline.drawScreenQuad(*_deferredShadingTransparents);
	pipeline.drawRenderQueue(RenderQueue::RenderQueueTransparentShading);
	pipeline.drawPostProcess(RenderQueue::RenderQueueTransparentShading, target, target);
}

void
DeferredLightingPipeline::renderTransparentSpecificShading(RenderPipeline& pipeline, GraphicsFramebufferPtr& target) noexcept
{
	pipeline.setFramebuffer(target);
	pipeline.drawRenderQueue(RenderQueue::RenderQueueTransparentSpecific);
	pipeline.drawPostProcess(RenderQueue::RenderQueueTransparentSpecific, target, target);
}

void
DeferredLightingPipeline::renderLights(RenderPipeline& pipeline, GraphicsFramebufferPtr& target) noexcept
{
	this->renderDirectLights(pipeline, target);
	this->renderIndirectLights(pipeline, target);
}

void 
DeferredLightingPipeline::renderDirectLights(RenderPipeline& pipeline, GraphicsFramebufferPtr& target) noexcept
{
	pipeline.setFramebuffer(target);
	pipeline.clearFramebuffer(0, GraphicsClearFlagBits::GraphicsClearFlagColorBit, float4::Zero, 1.0, 0);

	auto& lights = pipeline.getCamera()->getRenderDataManager()->getRenderData(RenderQueue::RenderQueueLighting);
	for (auto& it : lights)
	{
		auto light = it->downcast<Light>();
		if (light->getLightType() == LightType::LightTypeAmbient)
			this->renderAmbientLight(pipeline, *light);
	}

	pipeline.drawPostProcess(RenderQueue::RenderQueueAmbientLighting, target, nullptr);

	for (auto& it : lights)
	{
		auto light = it->downcast<Light>();
		if (light->getLightType() == LightType::LightTypeAmbient)
			continue;

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
		case LightType::LightTypeSpot:
			this->renderSpotLight(pipeline, *light);
			break;
		default:
			break;
		}
	}

	pipeline.drawPostProcess(RenderQueue::RenderQueueLighting, target, target);
}

void
DeferredLightingPipeline::renderSunLight(RenderPipeline& pipeline, const Light& light) noexcept
{
	_lightColor->uniform3f(light.getLightColor() * light.getIntensity());
	_lightEyeDirection->uniform3f(math::invRotateVector3(pipeline.getCamera()->getTransform(), light.getForward()));
	_lightAttenuation->uniform3f(light.getLightAttenuation());

	auto& shadowMap = light.getDepthLinearTexture();
	if (shadowMap)
	{
		float shadowFactor = _shadowEsmFactor / (light.getCamera(0)->getFar() - light.getCamera(0)->getNear());
		float shaodwBias = light.getShadowBias();

		_shadowMap->uniformTexture(shadowMap);
		_shadowFactor->uniform2f(shadowFactor, shaodwBias);
		_shadowView2LightView->uniform4f(light.getCamera(0)->getView().getAxisZ() * pipeline.getCamera()->getViewInverse());
		_shadowView2LightViewProject->uniform4fmat(light.getCamera(0)->getViewProject() * pipeline.getCamera()->getViewInverse());

		pipeline.drawScreenQuadLayer(*_deferredSunLightShadow, light.getLayer());
	}
	else
	{
		pipeline.drawScreenQuadLayer(*_deferredSunLight, light.getLayer());
	}
}

void
DeferredLightingPipeline::renderDirectionalLight(RenderPipeline& pipeline, const Light& light) noexcept
{
	_lightColor->uniform3f(light.getLightColor() * light.getIntensity());
	_lightEyeDirection->uniform3f(math::invRotateVector3(pipeline.getCamera()->getTransform(), light.getForward()));
	_lightAttenuation->uniform3f(light.getLightAttenuation());

	auto& shadowMap = light.getDepthLinearTexture();
	if (shadowMap)
	{
		float shadowFactor = _shadowEsmFactor / (light.getCamera(0)->getFar() - light.getCamera(0)->getNear());
		float shaodwBias = light.getShadowBias();

		_shadowMap->uniformTexture(shadowMap);
		_shadowFactor->uniform2f(shadowFactor, shaodwBias);
		_shadowView2LightView->uniform4f(light.getCamera(0)->getView().getAxisZ() * pipeline.getCamera()->getViewInverse());
		_shadowView2LightViewProject->uniform4fmat(light.getCamera(0)->getViewProject() * pipeline.getCamera()->getViewInverse());

		pipeline.drawScreenQuadLayer(*_deferredDirectionalLightShadow, light.getLayer());
	}
	else
	{
		pipeline.drawScreenQuadLayer(*_deferredDirectionalLight, light.getLayer());
	}
}

void
DeferredLightingPipeline::renderPointLight(RenderPipeline& pipeline, const Light& light) noexcept
{
	_lightColor->uniform3f(light.getLightColor() * light.getIntensity());
	_lightEyePosition->uniform3f(math::invTranslateVector3(pipeline.getCamera()->getTransform(), light.getTranslate()));
	_lightAttenuation->uniform3f(light.getLightAttenuation());

	auto transform = light.getTransform();
	transform.scale(light.getRange());

	pipeline.setTransform(transform);
	pipeline.drawSphere(*_deferredPointLight, light.getLayer());
}

void
DeferredLightingPipeline::renderSpotLight(RenderPipeline& pipeline, const Light& light) noexcept
{
	_lightColor->uniform3f(light.getLightColor() * light.getIntensity());
	_lightEyePosition->uniform3f(math::invTranslateVector3(pipeline.getCamera()->getTransform(), light.getTranslate()));
	_lightEyeDirection->uniform3f(math::invRotateVector3(pipeline.getCamera()->getTransform(), light.getForward()));
	_lightAttenuation->uniform3f(light.getLightAttenuation());
	_lightOuterInner->uniform2f(light.getSpotOuterCone().y, light.getSpotInnerCone().y);

	auto transform = light.getTransform();
	transform.translate(light.getForward() * light.getRange());
	transform.scale(light.getRange());

	pipeline.setTransform(transform);

	auto& shadowMap = light.getDepthLinearTexture();
	if (shadowMap)
	{
		float shadowFactor = _shadowEsmFactor / (light.getCamera(0)->getFar() - light.getCamera(0)->getNear());
		float shaodwBias = light.getShadowBias();

		_shadowMap->uniformTexture(shadowMap);
		_shadowFactor->uniform2f(shadowFactor, shaodwBias);
		_shadowView2LightView->uniform4f(light.getCamera(0)->getView().getAxisZ() * pipeline.getCamera()->getViewInverse());
		_shadowView2LightViewProject->uniform4fmat(light.getCamera(0)->getViewProject() * pipeline.getCamera()->getViewInverse());

		pipeline.drawCone(*_deferredSpotLightShadow, light.getLayer());
	}
	else
	{
		pipeline.drawCone(*_deferredSpotLight, light.getLayer());
	}
}

void
DeferredLightingPipeline::renderAmbientLight(RenderPipeline& pipeline, const Light& light) noexcept
{
	_lightColor->uniform3f(light.getLightColor() * light.getIntensity());
	_lightEyePosition->uniform3f(math::invTranslateVector3(pipeline.getCamera()->getTransform(), light.getTranslate()));
	_lightEyeDirection->uniform3f(math::invRotateVector3(pipeline.getCamera()->getTransform(), -light.getForward()));
	_lightAttenuation->uniform3f(light.getLightAttenuation());

	pipeline.drawScreenQuadLayer(*_deferredAmbientLight, light.getLayer());
}

void 
DeferredLightingPipeline::renderIndirectSpotLight(RenderPipeline& pipeline, const Light& light) noexcept
{
	_siiMRT0->uniformTexture(_deferredOpaqueMap);
	_siiMRT1->uniformTexture(_deferredNormalMap);
	_siiVPLsBuffer->uniformTexture(_mrsiiVPLsBufferMap);
	_vplsDepthLinearMap->uniformTexture(_deferredDepthLinearMap);

	pipeline.setFramebuffer(_deferredLightingView);
	pipeline.drawScreenQuad(*_mrsiiGatherIndirect, 255);
}

void
DeferredLightingPipeline::renderIndirectLights(RenderPipeline& pipeline, GraphicsFramebufferPtr& target) noexcept
{
	pipeline.setFramebuffer(target);

	auto& lights = pipeline.getCamera()->getRenderDataManager()->getRenderData(RenderQueue::RenderQueueLighting);
	for (auto& it : lights)
	{
		auto light = it->downcast<Light>();
		if (light->getLightType() == LightType::LightTypeAmbient)
			continue;

		if (!light->getGlobalIllumination())
			continue;

		switch (light->getLightType())
		{
		case LightType::LightTypeSpot:
			this->computeSpotVPLBuffers(pipeline, *light);
			this->renderIndirectSpotLight(pipeline, *light);
			break;
		default:
			break;
		}
	}
}

void 
DeferredLightingPipeline::computeSpotVPLBuffers(RenderPipeline& pipeline, const Light& light) noexcept
{
	float gridCount = 255;
	float gridSize = 16;
	float gridOffset = 1.0f / 16.0f;
	float gridDelta =  (15.0f / 16.0f) / 16.0f;

	_vplsCountGridOffsetDelta->uniform4f(gridCount, gridSize, gridOffset, gridDelta);
	_vplsColorMap->uniformTexture(light.getColorTexture());
	_vplsNormalMap->uniformTexture(light.getNormalTexture());
	_vplsDepthLinearMap->uniformTexture(light.getDepthLinearTexture());
	_vplsLightAttenuation->uniform3f(light.getLightAttenuation());
	_vplsSpotOuterInner->uniform2f(light.getSpotOuterCone().y, light.getSpotInnerCone().y);
	_vplsLightView2EyeView->uniform4fmat(pipeline.getCamera()->getView() * light.getCamera(0)->getViewInverse());

	auto camera = pipeline.getCamera();
	pipeline.setCamera(light.getCamera(0));
	pipeline.setFramebuffer(_mrsiiVPLsView);
	pipeline.clearFramebuffer(0, GraphicsClearFlagBits::GraphicsClearFlagAllBit, float4::Zero, 1.0f, 0);
	pipeline.drawScreenQuad(*_mrsiiRsm2VPLsSpot);
	pipeline.setCamera(camera);
}

void 
DeferredLightingPipeline::computeDepthDerivBuffer(RenderPipeline& pipeline, const GraphicsTexturePtr& src, GraphicsFramebufferPtr dst)
{
	pipeline.setFramebuffer(dst);
	pipeline.drawScreenQuad(*_mrsiiDepthDerivate);

	for (std::uint32_t i = _mrsiiDerivMipBase + 1; i < _mrsiiDerivMipBase + _mrsiiDerivMipCount; i++)
	{
		pipeline.setFramebuffer(_mrsiiDepthDerivViews[i]);
		pipeline.drawScreenQuad(*_mrsiiDepthDerivateMipmap);
	}
}

void 
DeferredLightingPipeline::computeNormalDerivBuffer(RenderPipeline& pipeline, const GraphicsTexturePtr& src, GraphicsFramebufferPtr dst)
{
	pipeline.setFramebuffer(dst);
	pipeline.drawScreenQuad(*_mrsiiNormalDerivate);

	for (std::uint32_t i = _mrsiiDerivMipBase + 1; i < _mrsiiDerivMipBase + _mrsiiDerivMipCount; i++)
	{
		pipeline.setFramebuffer(_mrsiiNormalDerivViews[i]);
		pipeline.drawScreenQuad(*_mrsiiNormalDerivateMipmap);
	}
}

void 
DeferredLightingPipeline::computeSubsplatStencil(RenderPipeline& pipeline, const GraphicsTexturePtr& depth, const GraphicsTexturePtr& normal, GraphicsFramebufferPtr dst)
{
	pipeline.setFramebuffer(dst);
	pipeline.drawScreenQuad(*_mrsiiComputeSubsplatStencil);
}

void
DeferredLightingPipeline::computeUpsamplingMultiresBuffer(RenderPipeline& pipeline, const GraphicsTexturePtr& src, GraphicsFramebufferPtr dst)
{
	pipeline.setFramebuffer(dst);
	pipeline.drawScreenQuad(*_mrsiiUpsampling);
}

void
DeferredLightingPipeline::copyRenderTexture(RenderPipeline& pipeline, const GraphicsTexturePtr& src, GraphicsFramebufferPtr dst) noexcept
{
	_texSource->uniformTexture(src);
	pipeline.setFramebuffer(dst);
	pipeline.drawScreenQuad(*_deferredCopyOnly);
}

void
DeferredLightingPipeline::copyRenderTexture(RenderPipeline& pipeline, const GraphicsTexturePtr& src, GraphicsFramebufferPtr dst, const Viewport& viewport) noexcept
{
	_texSource->uniformTexture(src);
	pipeline.setFramebuffer(dst);
	pipeline.setViewport(0, viewport);
	pipeline.setScissor(0, Scissor(viewport.left, viewport.top, viewport.width, viewport.height));
	pipeline.drawScreenQuad(*_deferredCopyOnly);
}

bool
DeferredLightingPipeline::setupSemantic(RenderPipeline& pipeline) noexcept
{
	_materialDeferredDepthMap = pipeline.getSemanticParam(GlobalSemanticType::GlobalSemanticTypeDepthMap);
	_materialDeferredDepthLinearMap = pipeline.getSemanticParam(GlobalSemanticType::GlobalSemanticTypeDepthLinearMap);
	_materialDeferredGraphicMap = pipeline.getSemanticParam(GlobalSemanticType::GlobalSemanticTypeDiffuseMap);
	_materialDeferredNormalMap = pipeline.getSemanticParam(GlobalSemanticType::GlobalSemanticTypeNormalMap);
	_materialDeferredLightMap = pipeline.getSemanticParam(GlobalSemanticType::GlobalSemanticTypeLightingMap);
	_materialDeferredOpaqueShadingMap = pipeline.getSemanticParam(GlobalSemanticType::GlobalSemanticTypeOpaqueShadingMap);
	return true;
}

bool
DeferredLightingPipeline::setupDeferredMaterials(RenderPipeline& pipeline) noexcept
{
	_deferredLighting = pipeline.createMaterial("sys:fx/deferred_lighting.fxml"); if (!_deferredLighting) return false;
	_deferredDepthOnly = _deferredLighting->getTech("DeferredDepthOnly"); if (!_deferredDepthOnly) return false;
	_deferredDepthLinear = _deferredLighting->getTech("DeferredDepthLinear"); if (!_deferredDepthLinear) return false;
	_deferredPointLight = _deferredLighting->getTech("DeferredPointLight"); if (!_deferredPointLight) return false;
	_deferredAmbientLight = _deferredLighting->getTech("DeferredAmbientLight"); if (!_deferredAmbientLight) return false;
	_deferredSunLight = _deferredLighting->getTech("DeferredSunLight"); if (!_deferredSunLight) return false;
	_deferredSunLightShadow = _deferredLighting->getTech("DeferredSunLightShadow"); if (!_deferredSunLightShadow) return false;
	_deferredDirectionalLight = _deferredLighting->getTech("DeferredDirectionalLight"); if (!_deferredDirectionalLight) return false;
	_deferredDirectionalLightShadow = _deferredLighting->getTech("DeferredDirectionalLightShadow"); if (!_deferredDirectionalLightShadow) return false;
	_deferredSpotLight = _deferredLighting->getTech("DeferredSpotLight"); if (!_deferredSpotLight) return false;
	_deferredSpotLightShadow = _deferredLighting->getTech("DeferredSpotLightShadow"); if (!_deferredSpotLightShadow) return false;
	_deferredShadingOpaques = _deferredLighting->getTech("DeferredShadingOpaques"); if (!_deferredShadingOpaques) return false;
	_deferredShadingTransparents = _deferredLighting->getTech("DeferredShadingTransparents"); if (!_deferredShadingTransparents) return false;
	_deferredCopyOnly = _deferredLighting->getTech("DeferredCopyOnly"); if (!_deferredCopyOnly) return false;

	_texMRT0 = _deferredLighting->getParameter("texMRT0"); if (!_texMRT0) return false;
	_texMRT1 = _deferredLighting->getParameter("texMRT1"); if (!_texMRT1) return false;
	_texMRT2 = _deferredLighting->getParameter("texMRT2"); if (!_texMRT1) return false;
	_texDepth = _deferredLighting->getParameter("texDepth"); if (!_texDepth) return false;
	_texLight = _deferredLighting->getParameter("texLight"); if (!_texLight) return false;
	_texSource = _deferredLighting->getParameter("texSource"); if (!_texSource) return false;
	_texOpaque = _deferredLighting->getParameter("texOpaque"); if (!_texOpaque) return false;

	_scaleY = _deferredLighting->getParameter("scaleY"); if (!_scaleY) return false;
	_clipInfo = _deferredLighting->getParameter("clipInfo"); if (!_clipInfo) return false;

	_lightColor = _deferredLighting->getParameter("lightColor"); if (!_lightColor) return false;
	_lightEyePosition = _deferredLighting->getParameter("lightEyePosition"); if (!_lightEyePosition) return false;
	_lightEyeDirection = _deferredLighting->getParameter("lightEyeDirection"); if (!_lightEyeDirection) return false;
	_lightOuterInner = _deferredLighting->getParameter("lightOuterInner"); if (!_lightOuterInner) return false;
	_lightAttenuation = _deferredLighting->getParameter("lightAttenuation"); if (!_lightAttenuation) return false;

	_shadowMap = _deferredLighting->getParameter("shadowMap"); if (!_shadowMap) return false;
	_shadowFactor = _deferredLighting->getParameter("shadowFactor"); if (!_shadowFactor) return false;
	_shadowView2LightView = _deferredLighting->getParameter("shadowView2LightView"); if (!_shadowView2LightView) return false;
	_shadowView2LightViewProject = _deferredLighting->getParameter("shadowView2LightViewProject"); if (!_shadowView2LightViewProject) return false;

	if (pipeline.getDeviceType() == GraphicsDeviceType::GraphicsDeviceTypeVulkan)
		_scaleY->uniform1f(-1.0f);
	else
		_scaleY->uniform1f(1.0f);

	return true;
}

bool
DeferredLightingPipeline::initTextureFormat(RenderPipeline& pipeline) noexcept
{
	if (pipeline.isTextureSupport(GraphicsFormat::GraphicsFormatD24UNorm_S8UInt))
		_deferredDepthFormat = GraphicsFormat::GraphicsFormatD24UNorm_S8UInt;
	else if (pipeline.isTextureSupport(GraphicsFormat::GraphicsFormatD16UNorm_S8UInt))
		_deferredDepthFormat = GraphicsFormat::GraphicsFormatD16UNorm_S8UInt;
	else if (pipeline.isTextureSupport(GraphicsFormat::GraphicsFormatX8_D24UNormPack32))
		_deferredDepthFormat = GraphicsFormat::GraphicsFormatX8_D24UNormPack32;
	else if (pipeline.isTextureSupport(GraphicsFormat::GraphicsFormatD16UNorm))
		_deferredDepthFormat = GraphicsFormat::GraphicsFormatD16UNorm;
	else
		return false;

	if (pipeline.isTextureSupport(GraphicsFormat::GraphicsFormatR32SFloat))
		_deferredDepthLinearFormat = GraphicsFormat::GraphicsFormatR32SFloat;
	else if (pipeline.isTextureSupport(GraphicsFormat::GraphicsFormatR16SFloat))
		_deferredDepthLinearFormat = GraphicsFormat::GraphicsFormatR16SFloat;
	else if (pipeline.isTextureSupport(GraphicsFormat::GraphicsFormatR8G8B8A8UNorm))
		_deferredDepthLinearFormat = GraphicsFormat::GraphicsFormatR8G8B8A8UNorm;
	else
		return false;

	if (pipeline.isTextureSupport(GraphicsFormat::GraphicsFormatR16G16B16A16SFloat))
		_deferredLightFormat = GraphicsFormat::GraphicsFormatR16G16B16A16SFloat;
	else if (pipeline.isTextureSupport(GraphicsFormat::GraphicsFormatR16G16B16A16UNorm))
		_deferredLightFormat = GraphicsFormat::GraphicsFormatR16G16B16A16UNorm;
	else if (pipeline.isTextureSupport(GraphicsFormat::GraphicsFormatR8G8B8A8UNorm))
		_deferredLightFormat = GraphicsFormat::GraphicsFormatR8G8B8A8UNorm;
	else
		return false;

	if (pipeline.isTextureSupport(GraphicsFormat::GraphicsFormatR8G8B8A8UNorm))
		_deferredOpaqueFormat = _deferredNormalFormat = GraphicsFormat::GraphicsFormatR8G8B8A8UNorm;
	else
		return false;

	if (pipeline.isTextureSupport(GraphicsFormat::GraphicsFormatR16G16B16A16SFloat))
		_deferredTransparentFormat = _deferredAbufferFormat = GraphicsFormat::GraphicsFormatR16G16B16A16SFloat;
	else if (pipeline.isTextureSupport(GraphicsFormat::GraphicsFormatR16G16B16A16UNorm))
		_deferredTransparentFormat = _deferredAbufferFormat = GraphicsFormat::GraphicsFormatR16G16B16A16UNorm;
	else if (pipeline.isTextureSupport(GraphicsFormat::GraphicsFormatR8G8B8A8UNorm))
		_deferredTransparentFormat = _deferredAbufferFormat = GraphicsFormat::GraphicsFormatR8G8B8A8UNorm;
	else
		return false;

	if (pipeline.isTextureSupport(GraphicsFormat::GraphicsFormatR16G16B16A16UNorm))
		_deferredShadingFormat = GraphicsFormat::GraphicsFormatR16G16B16A16UNorm;
	else if (pipeline.isTextureSupport(GraphicsFormat::GraphicsFormatR8G8B8A8UNorm))
		_deferredShadingFormat = GraphicsFormat::GraphicsFormatR8G8B8A8UNorm;
	else
		return false;

	return true;
}

bool
DeferredLightingPipeline::setupDeferredTextures(RenderPipeline& pipeline) noexcept
{
	std::uint32_t width, height;
	pipeline.getWindowResolution(width, height);

	GraphicsTextureDesc _deferredDepthDesc;
	_deferredDepthDesc.setWidth(width);
	_deferredDepthDesc.setHeight(height);
	_deferredDepthDesc.setTexDim(GraphicsTextureDim::GraphicsTextureDim2D);
	_deferredDepthDesc.setTexFormat(_deferredDepthFormat);
	_deferredDepthDesc.setSamplerFilter(GraphicsSamplerFilter::GraphicsSamplerFilterNearestMipmapNearest);
	_deferredDepthDesc.setSamplerWrap(GraphicsSamplerWrap::GraphicsSamplerWrapClampToEdge);
	_deferredDepthMap = pipeline.createTexture(_deferredDepthDesc);
	if (!_deferredDepthMap)
		return false;

	GraphicsTextureDesc _deferredDepthLinearDesc;
	_deferredDepthLinearDesc.setWidth(width);
	_deferredDepthLinearDesc.setHeight(height);
	_deferredDepthLinearDesc.setTexDim(GraphicsTextureDim::GraphicsTextureDim2D);
	_deferredDepthLinearDesc.setTexFormat(_deferredDepthLinearFormat);
	_deferredDepthLinearDesc.setSamplerFilter(GraphicsSamplerFilter::GraphicsSamplerFilterNearestMipmapNearest);
	_deferredDepthLinearDesc.setSamplerWrap(GraphicsSamplerWrap::GraphicsSamplerWrapClampToEdge);
	_deferredDepthLinearMap = pipeline.createTexture(_deferredDepthLinearDesc);
	if (!_deferredDepthLinearMap)
		return false;

	GraphicsTextureDesc _deferredOpaqueDesc;
	_deferredOpaqueDesc.setWidth(width);
	_deferredOpaqueDesc.setHeight(height);
	_deferredOpaqueDesc.setTexDim(GraphicsTextureDim::GraphicsTextureDim2D);
	_deferredOpaqueDesc.setTexFormat(_deferredOpaqueFormat);
	_deferredOpaqueDesc.setSamplerFilter(GraphicsSamplerFilter::GraphicsSamplerFilterNearestMipmapNearest);
	_deferredOpaqueMap = pipeline.createTexture(_deferredOpaqueDesc);
	if (!_deferredOpaqueMap)
		return false;

	GraphicsTextureDesc _deferredNormalDesc;
	_deferredNormalDesc.setWidth(width);
	_deferredNormalDesc.setHeight(height);
	_deferredNormalDesc.setTexDim(GraphicsTextureDim::GraphicsTextureDim2D);
	_deferredNormalDesc.setTexFormat(_deferredNormalFormat);
	_deferredNormalDesc.setSamplerFilter(GraphicsSamplerFilter::GraphicsSamplerFilterNearestMipmapNearest);
	_deferredNormalMap = pipeline.createTexture(_deferredNormalDesc);
	if (!_deferredNormalMap)
		return false;

	GraphicsTextureDesc _deferredTransparentDesc;
	_deferredTransparentDesc.setWidth(width);
	_deferredTransparentDesc.setHeight(height);
	_deferredTransparentDesc.setTexDim(GraphicsTextureDim::GraphicsTextureDim2D);
	_deferredTransparentDesc.setTexFormat(_deferredTransparentFormat);
	_deferredTransparentDesc.setSamplerFilter(GraphicsSamplerFilter::GraphicsSamplerFilterNearestMipmapNearest);
	_deferredTransparentMap = pipeline.createTexture(_deferredTransparentDesc);
	if (!_deferredTransparentMap)
		return false;

	GraphicsTextureDesc _deferredAbufferDesc;
	_deferredAbufferDesc.setWidth(width);
	_deferredAbufferDesc.setHeight(height);
	_deferredAbufferDesc.setTexDim(GraphicsTextureDim::GraphicsTextureDim2D);
	_deferredAbufferDesc.setTexFormat(_deferredAbufferFormat);
	_deferredAbufferDesc.setSamplerFilter(GraphicsSamplerFilter::GraphicsSamplerFilterNearestMipmapNearest);
	_deferredAbufferMap = pipeline.createTexture(_deferredAbufferDesc);
	if (!_deferredAbufferMap)
		return false;

	GraphicsTextureDesc _deferredLightDesc;
	_deferredLightDesc.setWidth(width);
	_deferredLightDesc.setHeight(height);
	_deferredLightDesc.setTexDim(GraphicsTextureDim::GraphicsTextureDim2D);
	_deferredLightDesc.setTexFormat(_deferredLightFormat);
	_deferredLightDesc.setSamplerFilter(GraphicsSamplerFilter::GraphicsSamplerFilterNearestMipmapNearest);
	_deferredLightingMap = pipeline.createTexture(_deferredLightDesc);
	if (!_deferredLightingMap)
		return false;

	GraphicsTextureDesc _deferredShadingDesc;
	_deferredShadingDesc.setWidth(width);
	_deferredShadingDesc.setHeight(height);
	_deferredShadingDesc.setTexFormat(_deferredShadingFormat);
	_deferredShadingDesc.setTexDim(GraphicsTextureDim::GraphicsTextureDim2D);
	_deferredShadingDesc.setSamplerFilter(GraphicsSamplerFilter::GraphicsSamplerFilterNearestMipmapNearest);
	_deferredShadingDesc.setSamplerWrap(GraphicsSamplerWrap::GraphicsSamplerWrapClampToEdge);
	_deferredOpaqueShadingMap = pipeline.createTexture(_deferredShadingDesc);
	if (!_deferredOpaqueShadingMap)
		return false;

	_deferredFinalShadingMap = pipeline.createTexture(_deferredShadingDesc);
	if (!_deferredFinalShadingMap)
		return false;

	_deferredSwapMap = pipeline.createTexture(_deferredShadingDesc);
	if (!_deferredSwapMap)
		return false;

	return true;
}

bool
DeferredLightingPipeline::setupDeferredRenderTextureLayouts(RenderPipeline& pipeline) noexcept
{
	GraphicsFramebufferLayoutDesc deferredDepthLayoutDesc;
	deferredDepthLayoutDesc.addComponent(GraphicsAttachment(0, GraphicsImageLayout::GraphicsImageLayoutDepthStencilAttachmentOptimal, _deferredDepthFormat));
	_deferredDepthImageLayout = pipeline.createFramebufferLayout(deferredDepthLayoutDesc);
	if (!_deferredDepthImageLayout)
		return false;

	GraphicsFramebufferLayoutDesc deferredDepthLinearLayoutDesc;
	deferredDepthLinearLayoutDesc.addComponent(GraphicsAttachment(0, GraphicsImageLayout::GraphicsImageLayoutColorAttachmentOptimal, _deferredDepthLinearFormat));
	_deferredDepthLinearImageLayout = pipeline.createFramebufferLayout(deferredDepthLinearLayoutDesc);
	if (!_deferredDepthLinearImageLayout)
		return false;

	GraphicsFramebufferLayoutDesc deferredGraphicsLayoutDesc;
	deferredGraphicsLayoutDesc.addComponent(GraphicsAttachment(0, GraphicsImageLayout::GraphicsImageLayoutColorAttachmentOptimal, _deferredOpaqueFormat));
	_deferredGraphicsImageLayout = pipeline.createFramebufferLayout(deferredGraphicsLayoutDesc);
	if (!_deferredGraphicsImageLayout)
		return false;

	GraphicsFramebufferLayoutDesc deferredNormalLayoutDesc;
	deferredNormalLayoutDesc.addComponent(GraphicsAttachment(0, GraphicsImageLayout::GraphicsImageLayoutColorAttachmentOptimal, _deferredNormalFormat));
	_deferredNormalImageLayout = pipeline.createFramebufferLayout(deferredNormalLayoutDesc);
	if (!_deferredNormalImageLayout)
		return false;

	GraphicsFramebufferLayoutDesc deferredAbufferLayoutDesc;
	deferredAbufferLayoutDesc.addComponent(GraphicsAttachment(0, GraphicsImageLayout::GraphicsImageLayoutColorAttachmentOptimal, _deferredAbufferFormat));
	_deferredAbufferImageLayout = pipeline.createFramebufferLayout(deferredAbufferLayoutDesc);
	if (!_deferredAbufferImageLayout)
		return false;

	GraphicsFramebufferLayoutDesc deferredOqaqueImagesLayoutDesc;
	deferredOqaqueImagesLayoutDesc.addComponent(GraphicsAttachment(0, GraphicsImageLayout::GraphicsImageLayoutColorAttachmentOptimal, _deferredOpaqueFormat));
	deferredOqaqueImagesLayoutDesc.addComponent(GraphicsAttachment(1, GraphicsImageLayout::GraphicsImageLayoutColorAttachmentOptimal, _deferredNormalFormat));
	deferredOqaqueImagesLayoutDesc.addComponent(GraphicsAttachment(2, GraphicsImageLayout::GraphicsImageLayoutDepthStencilAttachmentOptimal, _deferredDepthFormat));
	_deferredOpaqueImagesLayout = pipeline.createFramebufferLayout(deferredOqaqueImagesLayoutDesc);
	if (!_deferredOpaqueImagesLayout)
		return false;

	GraphicsFramebufferLayoutDesc deferredTransparenImagesLayoutDesc;
	deferredTransparenImagesLayoutDesc.addComponent(GraphicsAttachment(0, GraphicsImageLayout::GraphicsImageLayoutColorAttachmentOptimal, _deferredTransparentFormat));
	deferredTransparenImagesLayoutDesc.addComponent(GraphicsAttachment(1, GraphicsImageLayout::GraphicsImageLayoutColorAttachmentOptimal, _deferredNormalFormat));
	deferredTransparenImagesLayoutDesc.addComponent(GraphicsAttachment(2, GraphicsImageLayout::GraphicsImageLayoutColorAttachmentOptimal, _deferredAbufferFormat));
	deferredTransparenImagesLayoutDesc.addComponent(GraphicsAttachment(3, GraphicsImageLayout::GraphicsImageLayoutDepthStencilAttachmentOptimal, _deferredDepthFormat));
	_deferredTransparentImagesLayout = pipeline.createFramebufferLayout(deferredTransparenImagesLayoutDesc);
	if (!_deferredTransparentImagesLayout)
		return false;

	GraphicsFramebufferLayoutDesc deferredLightingLayoutDesc;
	deferredLightingLayoutDesc.addComponent(GraphicsAttachment(0, GraphicsImageLayout::GraphicsImageLayoutColorAttachmentOptimal, _deferredLightFormat));
	deferredLightingLayoutDesc.addComponent(GraphicsAttachment(1, GraphicsImageLayout::GraphicsImageLayoutDepthStencilReadOnlyOptimal, _deferredDepthFormat));
	_deferredLightingImageLayout = pipeline.createFramebufferLayout(deferredLightingLayoutDesc);
	if (!_deferredLightingImageLayout)
		return false;

	GraphicsFramebufferLayoutDesc deferredShadingLayoutDesc;
	deferredShadingLayoutDesc.addComponent(GraphicsAttachment(0, GraphicsImageLayout::GraphicsImageLayoutColorAttachmentOptimal, _deferredShadingFormat));
	deferredShadingLayoutDesc.addComponent(GraphicsAttachment(1, GraphicsImageLayout::GraphicsImageLayoutDepthStencilReadOnlyOptimal, _deferredDepthFormat));
	_deferredShadingImageLayout = pipeline.createFramebufferLayout(deferredShadingLayoutDesc);
	if (!_deferredShadingImageLayout)
		return false;

	return true;
}

bool
DeferredLightingPipeline::setupDeferredRenderTextures(RenderPipeline& pipeline) noexcept
{
	std::uint32_t width, height;
	pipeline.getWindowResolution(width, height);

	GraphicsFramebufferDesc deferredDepthDesc;
	deferredDepthDesc.setWidth(width);
	deferredDepthDesc.setHeight(height);
	deferredDepthDesc.setDepthStencilAttachment(GraphicsTextureBinding(_deferredDepthMap, 0, 0));
	deferredDepthDesc.setGraphicsFramebufferLayout(_deferredDepthImageLayout);
	_deferredDepthView = pipeline.createFramebuffer(deferredDepthDesc);
	if (!_deferredDepthView)
		return false;

	GraphicsFramebufferDesc deferredDepthLinearDesc;
	deferredDepthLinearDesc.setWidth(width);
	deferredDepthLinearDesc.setHeight(height);
	deferredDepthLinearDesc.addColorAttachment(GraphicsTextureBinding(_deferredDepthLinearMap, 0, 0));
	deferredDepthLinearDesc.setGraphicsFramebufferLayout(_deferredDepthLinearImageLayout);
	_deferredDepthLinearView = pipeline.createFramebuffer(deferredDepthLinearDesc);
	if (!_deferredDepthLinearView)
		return false;

	GraphicsFramebufferDesc deferredGraphicsDesc;
	deferredGraphicsDesc.setWidth(width);
	deferredGraphicsDesc.setHeight(height);
	deferredGraphicsDesc.addColorAttachment(GraphicsTextureBinding(_deferredOpaqueMap, 0, 0));
	deferredGraphicsDesc.setGraphicsFramebufferLayout(_deferredGraphicsImageLayout);
	_deferredGraphicsView = pipeline.createFramebuffer(deferredGraphicsDesc);
	if (!_deferredGraphicsView)
		return false;

	GraphicsFramebufferDesc deferredNormalDesc;
	deferredNormalDesc.setWidth(width);
	deferredNormalDesc.setHeight(height);
	deferredNormalDesc.addColorAttachment(GraphicsTextureBinding(_deferredNormalMap, 0, 0));
	deferredNormalDesc.setGraphicsFramebufferLayout(_deferredNormalImageLayout);
	_deferredNormalView = pipeline.createFramebuffer(deferredNormalDesc);
	if (!_deferredNormalView)
		return false;

	GraphicsFramebufferDesc deferredAbufferDesc;
	deferredAbufferDesc.setWidth(width);
	deferredAbufferDesc.setHeight(height);
	deferredAbufferDesc.addColorAttachment(GraphicsTextureBinding(_deferredAbufferMap, 0, 0));
	deferredAbufferDesc.setGraphicsFramebufferLayout(_deferredAbufferImageLayout);
	_deferredAbufferView = pipeline.createFramebuffer(deferredAbufferDesc);
	if (!_deferredAbufferView)
		return false;

	GraphicsFramebufferDesc deferredLightingDesc;
	deferredLightingDesc.setWidth(width);
	deferredLightingDesc.setHeight(height);
	deferredLightingDesc.setDepthStencilAttachment(GraphicsTextureBinding(_deferredDepthMap, 0, 0));
	deferredLightingDesc.addColorAttachment(GraphicsTextureBinding(_deferredLightingMap, 0, 0));
	deferredLightingDesc.setGraphicsFramebufferLayout(_deferredLightingImageLayout);
	_deferredLightingView = pipeline.createFramebuffer(deferredLightingDesc);
	if (!_deferredLightingView)
		return false;

	GraphicsFramebufferDesc deferredOpaqueDesc;
	deferredOpaqueDesc.setWidth(width);
	deferredOpaqueDesc.setHeight(height);
	deferredOpaqueDesc.setDepthStencilAttachment(GraphicsTextureBinding(_deferredDepthMap, 0, 0));
	deferredOpaqueDesc.addColorAttachment(GraphicsTextureBinding(_deferredOpaqueMap, 0, 0));
	deferredOpaqueDesc.addColorAttachment(GraphicsTextureBinding(_deferredNormalMap, 0, 0));
	deferredOpaqueDesc.setGraphicsFramebufferLayout(_deferredOpaqueImagesLayout);
	_deferredOpaqueViews = pipeline.createFramebuffer(deferredOpaqueDesc);
	if (!_deferredOpaqueViews)
		return false;

	GraphicsFramebufferDesc deferredTransparentDesc;
	deferredTransparentDesc.setWidth(width);
	deferredTransparentDesc.setHeight(height);
	deferredTransparentDesc.setDepthStencilAttachment(GraphicsTextureBinding(_deferredDepthMap, 0, 0));
	deferredTransparentDesc.addColorAttachment(GraphicsTextureBinding(_deferredTransparentMap, 0, 0));
	deferredTransparentDesc.addColorAttachment(GraphicsTextureBinding(_deferredNormalMap, 0, 0));
	deferredTransparentDesc.addColorAttachment(GraphicsTextureBinding(_deferredAbufferMap, 0, 0));
	deferredTransparentDesc.setGraphicsFramebufferLayout(_deferredTransparentImagesLayout);
	_deferredTransparentViews = pipeline.createFramebuffer(deferredTransparentDesc);
	if (!_deferredTransparentViews)
		return false;

	GraphicsFramebufferDesc deferredOpaqueShadingDesc;
	deferredOpaqueShadingDesc.setWidth(width);
	deferredOpaqueShadingDesc.setHeight(height);
	deferredOpaqueShadingDesc.setDepthStencilAttachment(GraphicsTextureBinding(_deferredDepthMap, 0, 0));
	deferredOpaqueShadingDesc.addColorAttachment(GraphicsTextureBinding(_deferredOpaqueShadingMap, 0, 0));
	deferredOpaqueShadingDesc.setGraphicsFramebufferLayout(_deferredShadingImageLayout);
	_deferredOpaqueShadingView = pipeline.createFramebuffer(deferredOpaqueShadingDesc);
	if (!_deferredOpaqueShadingView)
		return false;

	GraphicsFramebufferDesc deferredTransparentShadingDesc;
	deferredTransparentShadingDesc.setWidth(width);
	deferredTransparentShadingDesc.setHeight(height);
	deferredTransparentShadingDesc.setDepthStencilAttachment(GraphicsTextureBinding(_deferredDepthMap, 0, 0));
	deferredTransparentShadingDesc.addColorAttachment(GraphicsTextureBinding(_deferredFinalShadingMap, 0, 0));
	deferredTransparentShadingDesc.setGraphicsFramebufferLayout(_deferredShadingImageLayout);
	_deferredFinalShadingView = pipeline.createFramebuffer(deferredTransparentShadingDesc);
	if (!_deferredFinalShadingView)
		return false;

	GraphicsFramebufferDesc deferredSwapDesc;
	deferredSwapDesc.setWidth(width);
	deferredSwapDesc.setHeight(height);
	deferredSwapDesc.setDepthStencilAttachment(GraphicsTextureBinding(_deferredDepthMap, 0, 0));
	deferredSwapDesc.addColorAttachment(GraphicsTextureBinding(_deferredSwapMap, 0, 0));
	deferredSwapDesc.setGraphicsFramebufferLayout(_deferredShadingImageLayout);
	_deferredSwapView = pipeline.createFramebuffer(deferredSwapDesc);
	if (!_deferredSwapView)
		return false;

	return true;
}

bool 
DeferredLightingPipeline::setupMRSIIMaterials(RenderPipeline& pipeline) noexcept
{
	_mrsii = pipeline.createMaterial("sys:fx/MRSII.fxml"); if (!_mrsii) return false;
	_mrsiiRsm2VPLsSpot = _mrsii->getTech("RSM2VPLsSpot"); if (!_mrsiiRsm2VPLsSpot) return false;
	_mrsiiGatherIndirect = _mrsii->getTech("GatherIndirect"); if (!_mrsiiGatherIndirect) return false;
	_mrsiiGatherIndirectDebug = _mrsii->getTech("GatherIndirectDebug"); if (!_mrsiiGatherIndirectDebug) return false;
	_mrsiiDepthDerivate = _mrsii->getTech("DepthDerivate"); if (!_mrsiiDepthDerivate) return false;
	_mrsiiDepthDerivateMipmap = _mrsii->getTech("DepthDerivateMipmap"); if (!_mrsiiDepthDerivateMipmap) return false;
	_mrsiiNormalDerivate = _mrsii->getTech("NormalDerivate"); if (!_mrsiiNormalDerivate) return false;
	_mrsiiNormalDerivateMipmap = _mrsii->getTech("NormalDerivateMipmap"); if (!_mrsiiNormalDerivateMipmap) return false;
	_mrsiiComputeSubsplatStencil = _mrsii->getTech("ComputeSubsplatStencil"); if (!_mrsiiComputeSubsplatStencil) return false;
	_mrsiiUpsampling = _mrsii->getTech("Upsampling"); if (!_mrsiiGatherIndirectDebug) return false;

	_vplsColorMap = _mrsii->getParameter("texColor");
	_vplsNormalMap = _mrsii->getParameter("texNormal");
	_vplsDepthLinearMap = _mrsii->getParameter("texDepthLinear");
	_vplsLightAttenuation = _mrsii->getParameter("lightAttenuation");
	_vplsSpotOuterInner = _mrsii->getParameter("lightOuterInner");
	_vplsLightView2EyeView = _mrsii->getParameter("shadowView2EyeView");
	_vplsCountGridOffsetDelta = _mrsii->getParameter("VPLCountGridOffsetDelta");

	_siiMRT0 = _mrsii->getParameter("texMRT0");
	_siiMRT1 = _mrsii->getParameter("texMRT1");
	_siiVPLsBuffer = _mrsii->getParameter("texVPLBuffer");

	return true;
}

bool 
DeferredLightingPipeline::setupMRSIITexture(RenderPipeline& pipeline) noexcept
{
	std::uint32_t width, height;
	pipeline.getWindowResolution(width, height);

	GraphicsTextureDesc VPLsBufferDesc;
	VPLsBufferDesc.setWidth(256);
	VPLsBufferDesc.setHeight(3);
	VPLsBufferDesc.setTexFormat(GraphicsFormat::GraphicsFormatR16G16B16A16SFloat);
	VPLsBufferDesc.setSamplerFilter(GraphicsSamplerFilter::GraphicsSamplerFilterNearest);
	VPLsBufferDesc.setSamplerWrap(GraphicsSamplerWrap::GraphicsSamplerWrapClampToEdge);
	_mrsiiVPLsBufferMap = pipeline.createTexture(VPLsBufferDesc);

	GraphicsTextureDesc depthDerivDesc;
	depthDerivDesc.setWidth(width);
	depthDerivDesc.setHeight(height);
	depthDerivDesc.setTexDim(GraphicsTextureDim::GraphicsTextureDim2D);
	depthDerivDesc.setTexFormat(_deferredNormalFormat);
	depthDerivDesc.setSamplerWrap(GraphicsSamplerWrap::GraphicsSamplerWrapClampToEdge);
	depthDerivDesc.setMipBase(_mrsiiDerivMipBase);
	depthDerivDesc.setMipLevel(_mrsiiDerivMipCount);
	_mrsiiDepthDerivMap = pipeline.createTexture(depthDerivDesc);
	if (!_mrsiiDepthDerivMap)
		return false;

	GraphicsTextureDesc normalDeriveDesc;
	normalDeriveDesc.setWidth(width);
	normalDeriveDesc.setHeight(height);
	normalDeriveDesc.setTexDim(GraphicsTextureDim::GraphicsTextureDim2D);
	normalDeriveDesc.setTexFormat(_deferredNormalFormat);
	normalDeriveDesc.setSamplerWrap(GraphicsSamplerWrap::GraphicsSamplerWrapClampToEdge);
	normalDeriveDesc.setMipBase(_mrsiiDerivMipBase);
	normalDeriveDesc.setMipLevel(_mrsiiDerivMipCount);
	_mrsiiNormalDerivMap = pipeline.createTexture(normalDeriveDesc);
	if (!_mrsiiNormalDerivMap)
		return false;

	GraphicsTextureDesc subsplatStencilDesc;
	subsplatStencilDesc.setWidth(width);
	subsplatStencilDesc.setHeight(height);
	subsplatStencilDesc.setTexDim(GraphicsTextureDim::GraphicsTextureDim2D);
	subsplatStencilDesc.setTexFormat(GraphicsFormat::GraphicsFormatS8UInt);
	subsplatStencilDesc.setSamplerWrap(GraphicsSamplerWrap::GraphicsSamplerWrapClampToEdge);
	subsplatStencilDesc.setMipBase(_mrsiiDerivMipBase);
	subsplatStencilDesc.setMipLevel(_mrsiiDerivMipCount);
	_mrsiiSubsplatStencilMap = pipeline.createTexture(subsplatStencilDesc);
	if (!_mrsiiSubsplatStencilMap)
		return false;	

	return true;
}

bool 
DeferredLightingPipeline::setupMRSIIRenderTextureLayouts(RenderPipeline& pipeline) noexcept
{
	GraphicsFramebufferLayoutDesc VPLsBufferLayoutDesc;
	VPLsBufferLayoutDesc.addComponent(GraphicsAttachment(0, GraphicsImageLayout::GraphicsImageLayoutColorAttachmentOptimal, GraphicsFormat::GraphicsFormatR16G16B16A16SFloat));
	_mrsiiVPLsViewLayout = pipeline.createFramebufferLayout(VPLsBufferLayoutDesc);
	if (!_mrsiiVPLsViewLayout)
		return false;

	GraphicsFramebufferLayoutDesc depthDerivLayoutDesc;
	depthDerivLayoutDesc.addComponent(GraphicsAttachment(0, GraphicsImageLayout::GraphicsImageLayoutColorAttachmentOptimal, _deferredDepthLinearFormat));
	_mrsiiDepthDerivViewLayout = pipeline.createFramebufferLayout(depthDerivLayoutDesc);
	if (!_mrsiiDepthDerivViewLayout)
		return false;

	GraphicsFramebufferLayoutDesc depthNormalLayoutDesc;
	depthNormalLayoutDesc.addComponent(GraphicsAttachment(0, GraphicsImageLayout::GraphicsImageLayoutColorAttachmentOptimal, _deferredNormalFormat));
	_mrsiiNormalDerivViewLayout = pipeline.createFramebufferLayout(depthNormalLayoutDesc);
	if (!_mrsiiNormalDerivViewLayout)
		return false;

	GraphicsFramebufferLayoutDesc subsplatStencilViewLayout;
	subsplatStencilViewLayout.addComponent(GraphicsAttachment(0, GraphicsImageLayout::GraphicsImageLayoutDepthStencilAttachmentOptimal, GraphicsFormat::GraphicsFormatS8UInt));
	_mrsiiSubsplatStencilViewLayout = pipeline.createFramebufferLayout(subsplatStencilViewLayout);
	if (!_mrsiiSubsplatStencilViewLayout)
		return false;

	return true;
}

bool
DeferredLightingPipeline::setupMRSIIRenderTextures(RenderPipeline& pipeline) noexcept
{
	std::uint32_t width, height;
	pipeline.getWindowResolution(width, height);

	GraphicsFramebufferDesc VPLsBufferDesc;
	VPLsBufferDesc.setWidth(256);
	VPLsBufferDesc.setHeight(3);
	VPLsBufferDesc.addColorAttachment(GraphicsTextureBinding(_mrsiiVPLsBufferMap, 0, 0));
	VPLsBufferDesc.setGraphicsFramebufferLayout(_mrsiiVPLsViewLayout);
	_mrsiiVPLsView = pipeline.createFramebuffer(VPLsBufferDesc);
	if (!_mrsiiVPLsView)
		return false;

	_mrsiiDepthDerivViews.resize(_mrsiiDerivMipCount);
	_mrsiiNormalDerivViews.resize(_mrsiiDerivMipCount);
	_mrsiiSubsplatStencilViews.resize(_mrsiiDerivMipCount);

	for (std::uint32_t i = _mrsiiDerivMipBase; i < _mrsiiDerivMipBase + _mrsiiDerivMipCount; i++)
	{
		GraphicsFramebufferDesc depthDerivFrambufferDesc;
		depthDerivFrambufferDesc.setWidth(width / (1 << i));
		depthDerivFrambufferDesc.setHeight(height / (1 << i));
		depthDerivFrambufferDesc.addColorAttachment(GraphicsTextureBinding(_mrsiiDepthDerivMap, i, 0));
		depthDerivFrambufferDesc.setGraphicsFramebufferLayout(_mrsiiDepthDerivViewLayout);
		_mrsiiDepthDerivViews[i] = pipeline.createFramebuffer(depthDerivFrambufferDesc);
		if (!_mrsiiDepthDerivViews[i])
			return false;
	}

	for (std::uint32_t i = _mrsiiDerivMipBase; i < _mrsiiDerivMipBase + _mrsiiDerivMipCount; i++)
	{
		GraphicsFramebufferDesc normalDerivFrambufferDesc;
		normalDerivFrambufferDesc.setWidth(width / (1 << i));
		normalDerivFrambufferDesc.setHeight(height / (1 << i));
		normalDerivFrambufferDesc.addColorAttachment(GraphicsTextureBinding(_mrsiiNormalDerivMap, i, 0));
		normalDerivFrambufferDesc.setGraphicsFramebufferLayout(_mrsiiNormalDerivViewLayout);
		_mrsiiNormalDerivViews[i] = pipeline.createFramebuffer(normalDerivFrambufferDesc);
		if (!_mrsiiNormalDerivViews[i])
			return false;
	}

	for (std::uint32_t i = _mrsiiDerivMipBase; i < _mrsiiDerivMipBase + _mrsiiDerivMipCount; i++)
	{
		GraphicsFramebufferDesc subsplatStencilFramebufferDesc;
		subsplatStencilFramebufferDesc.setWidth(width / (1 << i));
		subsplatStencilFramebufferDesc.setHeight(height / (1 << i));
		subsplatStencilFramebufferDesc.setDepthStencilAttachment(GraphicsTextureBinding(_mrsiiSubsplatStencilMap, i, 0));
		subsplatStencilFramebufferDesc.setGraphicsFramebufferLayout(_mrsiiSubsplatStencilViewLayout);
		_mrsiiSubsplatStencilViews[i] = pipeline.createFramebuffer(subsplatStencilFramebufferDesc);
		if (!_mrsiiSubsplatStencilViews[i])
			return false;
	}

	return true;
}

void
DeferredLightingPipeline::destroySemantic() noexcept
{
	_materialDeferredDepthMap.reset();
	_materialDeferredDepthLinearMap.reset();
	_materialDeferredGraphicMap.reset();
	_materialDeferredNormalMap.reset();
	_materialDeferredLightMap.reset();
}

void
DeferredLightingPipeline::destroyDeferredMaterials() noexcept
{
	_deferredLighting.reset();
	_deferredDepthOnly.reset();
	_deferredDepthLinear.reset();
	_deferredSunLight.reset();
	_deferredSunLightShadow.reset();
	_deferredSpotLight.reset();
	_deferredPointLight.reset();
	_deferredAmbientLight.reset();
	_deferredShadingOpaques.reset();
	_deferredShadingTransparents.reset();
	_deferredDebugLayer.reset();
	_deferredCopyOnly.reset();

	_texMRT0.reset();
	_texMRT1.reset();
	_texMRT2.reset();
	_texDepth.reset();
	_texLight.reset();
	_texSource.reset();

	_clipInfo.reset();

	_shadowDecal.reset();
	_shadowMap.reset();
	_shadowFactor.reset();
	_shadowView2LightView.reset();
	_shadowView2LightViewProject.reset();

	_lightColor.reset();
	_lightEyePosition.reset();
	_lightEyeDirection.reset();
	_lightAttenuation.reset();
	_lightOuterInner.reset();
}

void
DeferredLightingPipeline::destroyDeferredRenderTextureLayouts() noexcept
{
	_deferredDepthImageLayout.reset();
	_deferredDepthLinearImageLayout.reset();
	_deferredGraphicsImageLayout.reset();
	_deferredNormalImageLayout.reset();
	_deferredLightingImageLayout.reset();
	_deferredShadingImageLayout.reset();
	_deferredOpaqueImagesLayout.reset();
}

void
DeferredLightingPipeline::destroyDeferredTextures() noexcept
{
	_deferredDepthMap.reset();
	_deferredDepthLinearMap.reset();
	_deferredOpaqueMap.reset();
	_deferredTransparentMap.reset();
	_deferredNormalMap.reset();
	_deferredAbufferMap.reset();
	_deferredLightingMap.reset();
	_deferredOpaqueShadingMap.reset();
	_deferredFinalShadingMap.reset();
	_deferredSwapMap.reset();
}

void
DeferredLightingPipeline::destroyDeferredRenderTextures() noexcept
{
	_deferredDepthView.reset();
	_deferredDepthLinearView.reset();
	_deferredGraphicsView.reset();
	_deferredNormalView.reset();
	_deferredAbufferView.reset();
	_deferredLightingView.reset();
	_deferredOpaqueViews.reset();
	_deferredTransparentViews.reset();
	_deferredOpaqueShadingView.reset();
	_deferredFinalShadingView.reset();
	_deferredSwapView.reset();
}

void
DeferredLightingPipeline::onResolutionChange() noexcept
{
	destroyDeferredTextures();
	destroyDeferredRenderTextures();
	setupDeferredTextures(*_pipeline);
	setupDeferredRenderTextures(*_pipeline);
}

void
DeferredLightingPipeline::onRenderPre() noexcept
{
	_materialDeferredDepthMap->uniformTexture(_deferredDepthMap);
	_materialDeferredDepthLinearMap->uniformTexture(_deferredDepthLinearMap);
	_materialDeferredNormalMap->uniformTexture(_deferredNormalMap);
	_materialDeferredGraphicMap->uniformTexture(_deferredOpaqueMap);
	_materialDeferredLightMap->uniformTexture(_deferredLightingMap);
}

void
DeferredLightingPipeline::onRenderPipeline(const CameraPtr& camera) noexcept
{
	assert(camera);
	assert(camera->getCameraOrder() == CameraOrder::CameraOrder3D);

	std::uint32_t width, height;
	_pipeline->getWindowResolution(width, height);

	_pipeline->setCamera(camera);
	_pipeline->setViewport(0, Viewport(0, 0, width, height));
	_pipeline->setViewport(1, Viewport(0, 0, width, height));
	_pipeline->setScissor(0, Scissor(0, 0, width, height));
	_pipeline->setScissor(1, Scissor(0, 0, width, height));

	this->render3DEnvMap(camera);
}

void
DeferredLightingPipeline::onRenderPost() noexcept
{
	auto camera = _pipeline->getCamera();
	if (!camera)
		return;

	float4 pixel = camera->getPixelViewport();

	Viewport viewport(pixel.x, pixel.y, pixel.z, pixel.w);

	auto flags = camera->getCameraRenderFlags();
	if (flags & CameraRenderFlagBits::CameraRenderTextureBit)
	{
		auto framebuffer = camera->getFramebuffer();
		if (framebuffer)
		{
			if (flags & CameraRenderFlagBits::CameraRenderGbufferDiffuseBit)
				this->copyRenderTexture(*_pipeline, _deferredOpaqueMap, framebuffer, viewport);
			else if (flags & CameraRenderFlagBits::CameraRenderGbufferNormalBit)
				this->copyRenderTexture(*_pipeline, _deferredNormalMap, framebuffer, viewport);
			else if (flags & CameraRenderFlagBits::CameraRenderLightingBit)
				this->copyRenderTexture(*_pipeline, _deferredLightingMap, framebuffer, viewport);
			else if (flags & CameraRenderFlagBits::CameraRenderShadingBit)
				this->copyRenderTexture(*_pipeline, _deferredFinalShadingMap, framebuffer, viewport);
		}
	}

	if (flags & CameraRenderFlagBits::CameraRenderScreenBit)
	{
		if (flags & CameraRenderFlagBits::CameraRenderGbufferDiffuseBit)
			this->copyRenderTexture(*_pipeline, _deferredOpaqueMap, nullptr, viewport);
		else if (flags & CameraRenderFlagBits::CameraRenderGbufferNormalBit)
			this->copyRenderTexture(*_pipeline, _deferredNormalMap, nullptr, viewport);
		else if (flags & CameraRenderFlagBits::CameraRenderLightingBit)
			this->copyRenderTexture(*_pipeline, _deferredLightingMap, nullptr, viewport);
		else if (flags & CameraRenderFlagBits::CameraRenderShadingBit)
			this->copyRenderTexture(*_pipeline, _deferredFinalShadingMap, nullptr, viewport);
	}
}

_NAME_END