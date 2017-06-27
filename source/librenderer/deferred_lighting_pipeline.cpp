// +----------------------------------------------------------------------
// | Project : ray.
// | All rights reserved.
// +----------------------------------------------------------------------
// | Copyright (c) 2013-2017.
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
#include <ray/deferred_lighting_framebuffers.h>
#include <ray/render_pipeline.h>
#include <ray/render_pipeline_manager.h>
#include <ray/render_pipeline_framebuffer.h>
#include <ray/render_object_manager.h>
#include <ray/camera.h>
#include <ray/light.h>
#include <ray/render_scene.h>
#include <ray/graphics_state.h>
#include <ray/graphics_texture.h>
#include <ray/graphics_framebuffer.h>
#include <ray/material.h>

_NAME_BEGIN

DeferredLightingPipeline::DeferredLightingPipeline() noexcept
	: _mrsiiDerivMipBase(0)
	, _mrsiiDerivMipCount(4)
	, _enabledMRSSI(false)
{
}

DeferredLightingPipeline::~DeferredLightingPipeline() noexcept
{
	this->close();
}

bool
DeferredLightingPipeline::setup(RenderPipelineManagerPtr pipelineManager) noexcept
{
	assert(pipelineManager);

	_pipeline = pipelineManager->getRenderPipeline();
	_pipelineManager = pipelineManager;

	if (!this->initTextureFormat(*_pipeline))
		return false;

	if (!this->setupSemantic(*_pipeline))
		return false;

	if (!this->setupDeferredMaterials(*_pipeline))
		return false;

	return true;
}

void
DeferredLightingPipeline::close() noexcept
{
	this->destroySemantic();
	this->destroyDeferredMaterials();
	this->destroyMRSIIMaterials();
	this->destroyMRSIITextures();
	this->destroyMRSIIRenderTextures();
	this->destroyMRSIIRenderTextureLayouts();
}

void
DeferredLightingPipeline::render3DEnvMap(const CameraPtr& camera) noexcept
{
	auto framebuffers = camera->getRenderPipelineFramebuffer()->downcast<DeferredLightingFramebuffers>();
	auto& dataManager = _pipeline->getCamera()->getRenderDataManager();
	if (!dataManager->getRenderData(RenderQueue::RenderQueueTransparentBack).empty() ||
		!dataManager->getRenderData(RenderQueue::RenderQueueTransparentBatchBack).empty() ||
		!dataManager->getRenderData(RenderQueue::RenderQueueTransparentSpecificBack).empty() ||
		!dataManager->getRenderData(RenderQueue::RenderQueueTransparentFront).empty() ||
		!dataManager->getRenderData(RenderQueue::RenderQueueTransparentBatchFront).empty() ||
		!dataManager->getRenderData(RenderQueue::RenderQueueTransparentSpecificFront).empty())
	{
		this->renderOpaques(*_pipeline, framebuffers->getDeferredGbuffersView());
		this->renderOpaquesDepthLinear(*_pipeline, framebuffers->getDeferredDepthLinearView());
		this->renderLights(*_pipeline, framebuffers->getDeferredLightingView());
		this->renderOpaquesShading(*_pipeline, framebuffers->getDeferredOpaqueShadingView());
		this->renderOpaquesSpecificShading(*_pipeline, framebuffers->getDeferredOpaqueShadingView());

		_texOpaque->uniformTexture(framebuffers->getDeferredOpaqueShadingMap());

		this->renderTransparentBack(*_pipeline, framebuffers->getDeferredGbuffersView());
		this->renderTransparentDepthLinearBack(*_pipeline, framebuffers->getDeferredDepthLinearView());
		this->renderLights(*_pipeline, framebuffers->getDeferredLightingView());
		this->renderTransparentShadingBack(*_pipeline, framebuffers->getDeferredTransparentShadingViews());
		this->renderTransparentSpecificShadingBack(*_pipeline, framebuffers->getDeferredTransparentShadingViews());

		_texOpaque->uniformTexture(framebuffers->getDeferredTransparentShadingMap());

		this->renderTransparentFront(*_pipeline, framebuffers->getDeferredGbuffersView());
		this->renderTransparentDepthLinearFront(*_pipeline, framebuffers->getDeferredDepthLinearView());
		this->renderLights(*_pipeline, framebuffers->getDeferredLightingView());
		this->renderTransparentShadingFront(*_pipeline, framebuffers->getDeferredFinalShadingView());
		this->renderTransparentSpecificShadingFront(*_pipeline, framebuffers->getDeferredFinalShadingView());
	}
	else
	{
		this->renderOpaques(*_pipeline, framebuffers->getDeferredGbuffersView());
		this->renderOpaquesDepthLinear(*_pipeline, framebuffers->getDeferredDepthLinearView());
		this->renderLights(*_pipeline, framebuffers->getDeferredLightingView());
		this->renderOpaquesShading(*_pipeline, framebuffers->getDeferredFinalShadingView());
		this->renderOpaquesSpecificShading(*_pipeline, framebuffers->getDeferredFinalShadingView());
	}

	if (_pipeline->drawPostProcess(RenderQueue::RenderQueuePostprocess, framebuffers->getDeferredFinalShadingView(), framebuffers->getDeferredSwapView()))
		this->copyRenderTexture(*_pipeline, framebuffers->getDeferredSwapMap(), framebuffers->getDeferredFinalShadingView());
}

void
DeferredLightingPipeline::renderOpaques(RenderPipeline& pipeline, const GraphicsFramebufferPtr& target) noexcept
{
	pipeline.setFramebuffer(target);
	pipeline.clearFramebuffer(0, GraphicsClearFlagBits::GraphicsClearFlagColorBit, float4::Zero, 1.0, 0);
	pipeline.clearFramebuffer(1, GraphicsClearFlagBits::GraphicsClearFlagColorBit, float4::Zero, 1.0, 0);
	pipeline.clearFramebuffer(2, GraphicsClearFlagBits::GraphicsClearFlagColorBit, float4::Zero, 1.0, 0);
	pipeline.clearFramebuffer(3, GraphicsClearFlagBits::GraphicsClearFlagColorBit, float4::Zero, 1.0, 0);
	pipeline.clearFramebuffer(4, GraphicsClearFlagBits::GraphicsClearFlagDepthStencilBit, float4::Zero, 1.0, 0);

	pipeline.drawRenderQueue(RenderQueue::RenderQueueOpaque);
	pipeline.drawRenderQueue(RenderQueue::RenderQueueOpaqueBatch);
}

void
DeferredLightingPipeline::renderOpaquesDepthLinear(RenderPipeline& pipeline, const GraphicsFramebufferPtr& target) noexcept
{
	_clipInfo->uniform2f(_pipeline->getCamera()->getClipConstant().xy());

	pipeline.setFramebuffer(target);
	pipeline.drawScreenQuad(*_deferredDepthLinear);
}

void
DeferredLightingPipeline::renderOpaquesShading(RenderPipeline& pipeline, const GraphicsFramebufferPtr& target) noexcept
{
	auto camera = pipeline.getCamera();

	pipeline.setFramebuffer(target);

	if (camera->getClearFlags() & CameraClearFlagBits::CameraClearColorBit)
		pipeline.clearFramebuffer(0, GraphicsClearFlagBits::GraphicsClearFlagColorBit, camera->getClearColor(), 1.0, 0);
	else
		pipeline.clearFramebuffer(0, GraphicsClearFlagBits::GraphicsClearFlagColorBit, float4::Zero, 1.0, 0);

	pipeline.drawScreenQuad(*_deferredShadingOpaques);
	pipeline.drawRenderQueue(RenderQueue::RenderQueueOpaqueShading);
	pipeline.drawPostProcess(RenderQueue::RenderQueueOpaqueShading, target, target);
}

void
DeferredLightingPipeline::renderOpaquesSpecificShading(RenderPipeline& pipeline, const GraphicsFramebufferPtr& target) noexcept
{
	pipeline.setFramebuffer(target);
	pipeline.drawRenderQueue(RenderQueue::RenderQueueOpaqueSpecific);
	pipeline.drawPostProcess(RenderQueue::RenderQueueOpaqueSpecific, target, target);
}

void
DeferredLightingPipeline::renderTransparentBack(RenderPipeline& pipeline, const GraphicsFramebufferPtr& target) noexcept
{
	pipeline.setFramebuffer(target);
	pipeline.clearFramebuffer(0, GraphicsClearFlagBits::GraphicsClearFlagColorBit, float4::Zero, 1.0, 0);
	pipeline.clearFramebuffer(1, GraphicsClearFlagBits::GraphicsClearFlagColorBit, float4::Zero, 1.0, 0);
	pipeline.clearFramebuffer(2, GraphicsClearFlagBits::GraphicsClearFlagColorBit, float4::Zero, 1.0, 0);
	pipeline.clearFramebuffer(3, GraphicsClearFlagBits::GraphicsClearFlagColorBit, float4::Zero, 1.0, 0);
	pipeline.clearFramebuffer(4, GraphicsClearFlagBits::GraphicsClearFlagStencilBit, float4::Zero, 1.0, 0);

	pipeline.drawRenderQueue(RenderQueue::RenderQueueTransparentBack);
	pipeline.drawRenderQueue(RenderQueue::RenderQueueTransparentBatchBack);
}

void
DeferredLightingPipeline::renderTransparentDepthLinearBack(RenderPipeline& pipeline, const GraphicsFramebufferPtr& target) noexcept
{
	_clipInfo->uniform2f(_pipeline->getCamera()->getClipConstant().xy());

	pipeline.setFramebuffer(target);
	pipeline.drawScreenQuad(*_deferredDepthLinear);
}

void
DeferredLightingPipeline::renderTransparentShadingBack(RenderPipeline& pipeline, const GraphicsFramebufferPtr& target) noexcept
{
	pipeline.setFramebuffer(target);
	pipeline.clearFramebuffer(0, GraphicsClearFlagBits::GraphicsClearFlagColorBit, float4::Zero, 1.0, 0);
	pipeline.drawScreenQuad(*_deferredShadingTransparents);
	pipeline.drawRenderQueue(RenderQueue::RenderQueueTransparentShadingBack);
	pipeline.drawPostProcess(RenderQueue::RenderQueueTransparentShadingBack, target, target);
}

void
DeferredLightingPipeline::renderTransparentSpecificShadingBack(RenderPipeline& pipeline, const GraphicsFramebufferPtr& target) noexcept
{
	pipeline.setFramebuffer(target);
	pipeline.drawRenderQueue(RenderQueue::RenderQueueTransparentSpecificBack);
	pipeline.drawPostProcess(RenderQueue::RenderQueueTransparentSpecificBack, target, target);
}

void
DeferredLightingPipeline::renderTransparentFront(RenderPipeline& pipeline, const GraphicsFramebufferPtr& target) noexcept
{
	pipeline.setFramebuffer(target);
	pipeline.clearFramebuffer(0, GraphicsClearFlagBits::GraphicsClearFlagColorBit, float4::Zero, 1.0, 0);
	pipeline.clearFramebuffer(1, GraphicsClearFlagBits::GraphicsClearFlagColorBit, float4::Zero, 1.0, 0);
	pipeline.clearFramebuffer(2, GraphicsClearFlagBits::GraphicsClearFlagColorBit, float4::Zero, 1.0, 0);
	pipeline.clearFramebuffer(3, GraphicsClearFlagBits::GraphicsClearFlagColorBit, float4::Zero, 1.0, 0);
	pipeline.clearFramebuffer(4, GraphicsClearFlagBits::GraphicsClearFlagStencilBit, float4::Zero, 1.0, 0);

	pipeline.drawRenderQueue(RenderQueue::RenderQueueTransparentFront);
	pipeline.drawRenderQueue(RenderQueue::RenderQueueTransparentBatchFront);
}

void
DeferredLightingPipeline::renderTransparentDepthLinearFront(RenderPipeline& pipeline, const GraphicsFramebufferPtr& target) noexcept
{
	_clipInfo->uniform2f(_pipeline->getCamera()->getClipConstant().xy());

	pipeline.setFramebuffer(target);
	pipeline.drawScreenQuad(*_deferredDepthLinear);
}

void
DeferredLightingPipeline::renderTransparentShadingFront(RenderPipeline& pipeline, const GraphicsFramebufferPtr& target) noexcept
{
	pipeline.setFramebuffer(target);
	pipeline.drawScreenQuad(*_deferredShadingTransparents);
	pipeline.drawRenderQueue(RenderQueue::RenderQueueTransparentShadingFront);
	pipeline.drawPostProcess(RenderQueue::RenderQueueTransparentShadingFront, target, target);
}

void
DeferredLightingPipeline::renderTransparentSpecificShadingFront(RenderPipeline& pipeline, const GraphicsFramebufferPtr& target) noexcept
{
	pipeline.setFramebuffer(target);
	pipeline.drawRenderQueue(RenderQueue::RenderQueueTransparentSpecificFront);
	pipeline.drawPostProcess(RenderQueue::RenderQueueTransparentSpecificFront, target, target);
}

void
DeferredLightingPipeline::renderLights(RenderPipeline& pipeline, const GraphicsFramebufferPtr& target) noexcept
{
	pipeline.setFramebuffer(target);
	pipeline.clearFramebuffer(0, GraphicsClearFlagBits::GraphicsClearFlagColorBit, float4::Zero, 1.0, 0);

	this->renderAmbientLights(pipeline, target);
	this->renderDirectLights(pipeline, target);
	this->renderIndirectLights(pipeline, target);
}

void
DeferredLightingPipeline::renderAmbientLights(RenderPipeline& pipeline, const GraphicsFramebufferPtr& target) noexcept
{
	pipeline.setFramebuffer(target);

	auto& lights = pipeline.getCamera()->getRenderDataManager()->getRenderData(RenderQueue::RenderQueueLighting);
	for (auto& it : lights)
	{
		auto light = it->downcast<Light>();
		switch (light->getLightType())
		{
		case LightType::LightTypeAmbient:
			this->renderAmbientLight(pipeline, *light);
			break;
		case LightType::LightTypeEnvironment:
			this->renderEnvironmentLight(pipeline, *light);
			break;
		default:
			break;
		}
	}

	pipeline.drawPostProcess(RenderQueue::RenderQueueOpaqueLighting, target, target);
}

void
DeferredLightingPipeline::renderDirectLights(RenderPipeline& pipeline, const GraphicsFramebufferPtr& target) noexcept
{
	pipeline.setFramebuffer(target);

	auto& lights = pipeline.getCamera()->getRenderDataManager()->getRenderData(RenderQueue::RenderQueueLighting);
	for (auto& it : lights)
	{
		auto light = it->downcast<Light>();
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
}

void
DeferredLightingPipeline::renderSunLight(RenderPipeline& pipeline, const Light& light) noexcept
{
	_lightColor->uniform3f(light.getLightColor() * light.getLightIntensity());
	_lightEyeDirection->uniform3f(math::invRotateVector3(pipeline.getCamera()->getTransform(), light.getForward()));
	_lightAttenuation->uniform3f(light.getLightAttenuation());

	auto& shadowMap = light.getDepthLinearTexture();
	if (shadowMap)
	{
		float shadowFactor = light.getShadowFactor() / (light.getCamera(0)->getFar() - light.getCamera(0)->getNear());
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
	_lightColor->uniform3f(light.getLightColor() * light.getLightIntensity());
	_lightEyeDirection->uniform3f(math::invRotateVector3(pipeline.getCamera()->getTransform(), light.getForward()));
	_lightAttenuation->uniform3f(light.getLightAttenuation());

	auto& shadowMap = light.getDepthLinearTexture();
	if (shadowMap)
	{
		float shadowFactor = light.getShadowFactor() / (light.getCamera(0)->getFar() - light.getCamera(0)->getNear());
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
	_lightColor->uniform3f(light.getLightColor() * light.getLightIntensity());
	_lightEyePosition->uniform3f(math::invTranslateVector3(pipeline.getCamera()->getTransform(), light.getTranslate()));
	_lightAttenuation->uniform3f(light.getLightAttenuation());

	auto transform = light.getTransform();
	transform.scale(light.getLightRange());

	pipeline.setTransform(transform);
	pipeline.drawSphere(*_deferredPointLight, light.getLayer());
}

void
DeferredLightingPipeline::renderSpotLight(RenderPipeline& pipeline, const Light& light) noexcept
{
	_lightColor->uniform3f(light.getLightColor() * light.getLightIntensity());
	_lightEyePosition->uniform3f(math::invTranslateVector3(pipeline.getCamera()->getTransform(), light.getTranslate()));
	_lightEyeDirection->uniform3f(math::invRotateVector3(pipeline.getCamera()->getTransform(), light.getForward()));
	_lightAttenuation->uniform3f(light.getLightAttenuation());
	_lightOuterInner->uniform2f(light.getSpotOuterCone().y, light.getSpotInnerCone().y);

	auto transform = light.getTransform();
	transform.translate(light.getForward() * light.getLightRange());
	transform.scale(light.getLightRange());

	pipeline.setTransform(transform);

	auto& shadowMap = light.getDepthLinearTexture();
	if (shadowMap)
	{
		float shadowFactor = light.getShadowFactor() / (light.getCamera(0)->getFar() - light.getCamera(0)->getNear());
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
	_lightColor->uniform3f(light.getLightColor() * light.getLightIntensity());
	_lightEyePosition->uniform3f(math::invTranslateVector3(pipeline.getCamera()->getTransform(), light.getTranslate()));
	_lightEyeDirection->uniform3f(math::invRotateVector3(pipeline.getCamera()->getTransform(), light.getForward()));
	_lightAttenuation->uniform3f(light.getLightAttenuation());

	pipeline.drawScreenQuadLayer(*_deferredAmbientLight, light.getLayer());
}

void
DeferredLightingPipeline::renderEnvironmentLight(RenderPipeline& pipeline, const Light& light) noexcept
{
	auto camera = pipeline.getCamera();
	if (light.getSkyLightingDiffuse() && light.getSkyLightingSpecular())
	{
		float3 factor;
		factor.x = light.getSkyLightingSpecular()->getGraphicsTextureDesc().getMipNums();
		factor.y = 1.0f;
		factor.z = 1.0f;
		_envFactor->uniform3f(factor);

		_envBoxMin->uniform3f(float3(-camera->getFar()));
		_envBoxMax->uniform3f(float3(camera->getFar()));
		_envBoxCenter->uniform3f(camera->getTranslate());
		_envDiffuse->uniformTexture(light.getSkyLightingDiffuse());
		_envSpecular->uniformTexture(light.getSkyLightingSpecular());

		pipeline.drawScreenQuadLayer(*_deferredEnvironmentLighting, light.getLayer());
	}
}

void
DeferredLightingPipeline::renderIndirectSpotLight(RenderPipeline& pipeline, const Light& light) noexcept
{
	auto framebuffers = pipeline.getCamera()->getRenderPipelineFramebuffer()->downcast<DeferredLightingFramebuffers>();
	_mrsiiMRT0->uniformTexture(framebuffers->getDeferredGbuffer1Map());
	_mrsiiMRT1->uniformTexture(framebuffers->getDeferredGbuffer2Map());
	_mrsiiVPLsBuffer->uniformTexture(_mrsiiVPLsBufferMap);
	_mrsiiDepthLinearMap->uniformTexture(framebuffers->getDeferredDepthLinearMap());

	pipeline.drawScreenQuad(*_mrsiiGatherIndirect);
}

void
DeferredLightingPipeline::renderIndirectLights(RenderPipeline& pipeline, const GraphicsFramebufferPtr& target) noexcept
{
	if (!_pipelineManager->getRenderSetting().enableGlobalIllumination)
		return;

	bool hasIndirectLight = false;

	auto framebuffers = pipeline.getCamera()->getRenderPipelineFramebuffer()->downcast<DeferredLightingFramebuffers>();
	auto& lights = pipeline.getCamera()->getRenderDataManager()->getRenderData(RenderQueue::RenderQueueLighting);
	for (auto& it : lights)
	{
		auto light = it->downcast<Light>();
		if (light->getGlobalIllumination() &&
			light->getLightType() != LightType::LightTypeAmbient &&
			light->getLightType() != LightType::LightTypeEnvironment)
		{
			hasIndirectLight = true;
			break;
		}
	}

	if (!hasIndirectLight)
		return;

	if (!_enabledMRSSI)
	{
		_enabledMRSSI = this->setupMRSII(pipeline);
		if (!_enabledMRSSI)
			return;
	}

	for (auto& it : lights)
	{
		auto light = it->downcast<Light>();
		if (light->getLightType() == LightType::LightTypeAmbient ||
			light->getLightType() == LightType::LightTypeEnvironment)
			continue;

		if (!light->getGlobalIllumination())
			continue;

		hasIndirectLight = true;

		switch (light->getLightType())
		{
		case LightType::LightTypeSpot:
			this->computeSpotVPLBuffers(pipeline, *light);
			break;
		default:
			break;
		}
	}

	this->computeDepthDerivBuffer(pipeline, framebuffers->getDeferredDepthLinearMap(), _mrsiiDepthDerivViews);
	this->computeNormalDerivBuffer(pipeline, framebuffers->getDeferredGbuffer2Map(), _mrsiiNormalDerivViews);
	this->computeSubsplatStencil(pipeline, _mrsiiDepthDerivMap, _mrsiiNormalDerivMap, _mrsiiSubsplatStencilViews);

	for (std::uint32_t i = _mrsiiDerivMipBase; i < _mrsiiDerivMipBase + _mrsiiDerivMipCount; i++)
	{
		pipeline.setFramebuffer(_mrsiiGaterIndirectViews[i]);
		pipeline.clearFramebuffer(0, GraphicsClearFlagBits::GraphicsClearFlagColorBit, float4::Zero, 1.0f, 0);

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
				this->renderIndirectSpotLight(pipeline, *light);
				break;
			default:
				break;
			}
		}
	}

	this->computeUpsamplingMultiresBuffer(pipeline, _mrsiiGaterIndirectMap, _mrsiiGaterIndirectViews, target);
}

void
DeferredLightingPipeline::computeSpotVPLBuffers(RenderPipeline& pipeline, const Light& light) noexcept
{
	float gridCount = _mrsiiVPLsView->getGraphicsFramebufferDesc().getWidth();
	float gridSize = std::floor(std::sqrt(gridCount));
	float gridOffset = 1.0f / gridSize;
	float gridDelta = ((gridSize - 1) / gridSize) / gridSize;

	_mrsiiCountGridOffsetDelta->uniform4f(gridCount, gridSize, gridOffset, gridDelta);
	_mrsiiColorMap->uniformTexture(light.getColorTexture());
	_mrsiiNormalMap->uniformTexture(light.getNormalTexture());
	_mrsiiDepthLinearMap->uniformTexture(light.getDepthLinearTexture());
	_mrsiiLightAttenuation->uniform3f(light.getLightAttenuation());
	_mrsiiLightOuterInner->uniform2f(light.getSpotOuterCone().y, light.getSpotInnerCone().y);
	_mrsiiLightView2EyeView->uniform4fmat(pipeline.getCamera()->getView() * light.getCamera(0)->getViewInverse());
	_mrsiiLightColor->uniform3f(light.getLightColor() * light.getLightIntensity());

	auto camera = pipeline.getCamera();
	pipeline.setCamera(light.getCamera(0));
	pipeline.setFramebuffer(_mrsiiVPLsView);
	pipeline.discardFramebuffer(0);
	pipeline.drawScreenQuad(*_mrsiiRsm2VPLsSpot);
	pipeline.setCamera(camera);
}

void
DeferredLightingPipeline::computeDepthDerivBuffer(RenderPipeline& pipeline, const GraphicsTexturePtr& src, const GraphicsFramebuffers& dst)
{
	std::uint32_t width = src->getGraphicsTextureDesc().getWidth();
	std::uint32_t height = src->getGraphicsTextureDesc().getHeight();

	_mrsiiOffset->uniform2f(1.0f / width, 1.0f / height);
	_mrsiiDepthLinearMap->uniformTexture(src);

	pipeline.setFramebuffer(dst[0]);
	pipeline.discardFramebuffer(0);
	pipeline.drawScreenQuad(*_mrsiiDepthDerivate);

	for (std::uint32_t i = _mrsiiDerivMipBase; i < _mrsiiDerivMipBase + _mrsiiDerivMipCount - 1; i++)
	{
		std::uint32_t w = std::max<std::uint32_t>(width / (1 << i), 1);
		std::uint32_t h = std::max<std::uint32_t>(height / (1 << i), 1);

		_mrsiiOffset->uniform2f(1.0f / w, 1.0f / h);
		_mrsiiMipmapLevel->uniform4i(i, 0, 0, 0);
		_mrsiiDepthLinearMap->uniformTexture(dst[i]->getGraphicsFramebufferDesc().getColorAttachment(0).getBindingTexture());

		pipeline.setFramebuffer(dst[i + 1]);
		pipeline.discardFramebuffer(0);
		pipeline.drawScreenQuad(*_mrsiiDepthDerivateMipmap);
	}
}

void
DeferredLightingPipeline::computeNormalDerivBuffer(RenderPipeline& pipeline, const GraphicsTexturePtr& src, const GraphicsFramebuffers& dst)
{
	std::uint32_t width = src->getGraphicsTextureDesc().getWidth();
	std::uint32_t height = src->getGraphicsTextureDesc().getHeight();

	_mrsiiOffset->uniform2f(1.0f / width, 1.0f / height);
	_mrsiiNormalMap->uniformTexture(src);

	pipeline.setFramebuffer(dst[0]);
	pipeline.discardFramebuffer(0);
	pipeline.drawScreenQuad(*_mrsiiNormalDerivate);

	for (std::uint32_t i = _mrsiiDerivMipBase; i < _mrsiiDerivMipBase + _mrsiiDerivMipCount - 1; i++)
	{
		std::uint32_t w = std::max<std::uint32_t>(width / (1 << i), 1);
		std::uint32_t h = std::max<std::uint32_t>(height / (1 << i), 1);

		_mrsiiOffset->uniform2f(1.0f / w, 1.0f / h);
		_mrsiiMipmapLevel->uniform4i(i, 0, 0, 0);
		_mrsiiNormalMap->uniformTexture(dst[i]->getGraphicsFramebufferDesc().getColorAttachment(0).getBindingTexture());

		pipeline.setFramebuffer(dst[i + 1]);
		pipeline.discardFramebuffer(0);
		pipeline.drawScreenQuad(*_mrsiiNormalDerivateMipmap);
	}
}

void
DeferredLightingPipeline::computeSubsplatStencil(RenderPipeline& pipeline, const GraphicsTexturePtr& depth, const GraphicsTexturePtr& normal, const GraphicsFramebuffers& dst)
{
	_mrsiiNormalMap->uniformTexture(normal);
	_mrsiiDepthLinearMap->uniformTexture(depth);
	_mrsiiThreshold->uniform2f(0.6f, 0.2f);

	for (std::uint32_t i = _mrsiiDerivMipBase; i < _mrsiiDerivMipBase + _mrsiiDerivMipCount; i++)
	{
		int isNotFinestLevel = (i == _mrsiiDerivMipBase) ? 0 : 1;
		int isNotCoarsestLevel = (i == _mrsiiDerivMipBase + _mrsiiDerivMipCount - 1) ? 0 : 1;
		_mrsiiMipmapLevel->uniform4i(i, i + 1, isNotFinestLevel, isNotCoarsestLevel);

		pipeline.setFramebuffer(dst[i]);
		pipeline.clearFramebuffer(0, GraphicsClearFlagBits::GraphicsClearFlagStencilBit, float4::Zero, 1.0, 0);
		pipeline.drawScreenQuad(*_mrsiiComputeSubsplatStencil);
	}
}

void
DeferredLightingPipeline::computeUpsamplingMultiresBuffer(RenderPipeline& pipeline, GraphicsTexturePtr src, const GraphicsFramebuffers& srcviews, const GraphicsFramebufferPtr& dst)
{
	std::uint32_t width = src->getGraphicsTextureDesc().getWidth();
	std::uint32_t height = src->getGraphicsTextureDesc().getHeight();

	for (std::int32_t i = _mrsiiDerivMipBase + _mrsiiDerivMipCount - 2; i >= (std::int32_t)_mrsiiDerivMipBase; i--)
	{
		std::uint32_t w = std::max<std::uint32_t>(float(width) / (1 << i), 1);
		std::uint32_t h = std::max<std::uint32_t>(float(height) / (1 << i), 1);

		_mrsiiColorMap->uniformTexture(src);
		_mrsiiOffsetUpsampling->uniform4f(1.0f / w, 1.0f / h, 1.0f / w, 1.0f / h);
		_mrsiiMipmapLevel->uniform4i(i + 1, i, w, h);

		if (i != 0)
		{
			pipeline.setFramebuffer(srcviews[i]);
			pipeline.drawScreenQuad(*_mrsiiUpsampling);
		}
		else
		{
			pipeline.setFramebuffer(dst);
			pipeline.drawScreenQuad(*_mrsiiUpsamplingWithBlend);
		}
	}
}

void
DeferredLightingPipeline::copyRenderTexture(RenderPipeline& pipeline, const GraphicsTexturePtr& src, const GraphicsFramebufferPtr& dst) noexcept
{
	_texSource->uniformTexture(src);
	pipeline.setFramebuffer(dst);
	pipeline.discardFramebuffer(0);
	pipeline.drawScreenQuad(*_deferredCopyOnly);
}

void
DeferredLightingPipeline::copyRenderTexture(RenderPipeline& pipeline, const GraphicsTexturePtr& src, const GraphicsFramebufferPtr& dst, const float4& viewport) noexcept
{
	_texSource->uniformTexture(src);
	pipeline.setFramebuffer(dst);
	pipeline.clearFramebuffer(0, ray::GraphicsClearFlagBits::GraphicsClearFlagColorBit, ray::float4::Zero, 1.0, 0);
	pipeline.setViewport(0, Viewport(viewport.x, viewport.y, viewport.z, viewport.w));
	pipeline.setScissor(0, Scissor(viewport.x, viewport.y, viewport.z, viewport.w));
	pipeline.drawScreenQuad(*_deferredCopyOnly);
}

bool
DeferredLightingPipeline::setupSemantic(RenderPipeline& pipeline) noexcept
{
	_materialDeferredDepthMap = pipeline.getSemanticParam(GlobalSemanticType::GlobalSemanticTypeDepthMap);
	_materialDeferredDepthLinearMap = pipeline.getSemanticParam(GlobalSemanticType::GlobalSemanticTypeDepthLinearMap);
	_materialDeferredGbuffer1Map = pipeline.getSemanticParam(GlobalSemanticType::GlobalSemanticTypeDiffuseMap);
	_materialDeferredGbuffer2Map = pipeline.getSemanticParam(GlobalSemanticType::GlobalSemanticTypeNormalMap);
	_materialDeferredGbuffer3Map = pipeline.getSemanticParam(GlobalSemanticType::GlobalSemanticTypeGbuffer3Map);
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
	_deferredEnvironmentLighting = _deferredLighting->getTech("DeferredEnvironmentLighting"); if (!_deferredEnvironmentLighting) return false;
	_deferredShadingOpaques = _deferredLighting->getTech("DeferredShadingOpaques"); if (!_deferredShadingOpaques) return false;
	_deferredShadingTransparents = _deferredLighting->getTech("DeferredShadingTransparents"); if (!_deferredShadingTransparents) return false;
	_deferredCopyOnly = _deferredLighting->getTech("DeferredCopyOnly"); if (!_deferredCopyOnly) return false;

	_texMRT0 = _deferredLighting->getParameter("texMRT0"); if (!_texMRT0) return false;
	_texMRT1 = _deferredLighting->getParameter("texMRT1"); if (!_texMRT1) return false;
	_texMRT2 = _deferredLighting->getParameter("texMRT2"); if (!_texMRT2) return false;
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

	_envDiffuse = _deferredLighting->getParameter("envDiffuse");
	_envSpecular = _deferredLighting->getParameter("envSpecular");
	_envFactor = _deferredLighting->getParameter("envFactor");
	_envBoxMax = _deferredLighting->getParameter("envBoxMax");
	_envBoxMin = _deferredLighting->getParameter("envBoxMin");
	_envBoxCenter = _deferredLighting->getParameter("envBoxCenter");

	if (pipeline.getDeviceType() == GraphicsDeviceType::GraphicsDeviceTypeVulkan)
		_scaleY->uniform1f(-1.0f);
	else
		_scaleY->uniform1f(1.0f);

	return true;
}

bool
DeferredLightingPipeline::initTextureFormat(RenderPipeline& pipeline) noexcept
{
	if (pipeline.isTextureSupport(GraphicsFormat::GraphicsFormatR8G8B8A8UNorm))
		_mrsiiNormalFormat = GraphicsFormat::GraphicsFormatR8G8B8A8UNorm;
	else
		return false;

	if (pipeline.isTextureSupport(GraphicsFormat::GraphicsFormatR16SFloat))
		_mrsiiDepthLinearFormat = GraphicsFormat::GraphicsFormatR16SFloat;
	else
		return false;

	if (pipeline.isTextureSupport(GraphicsFormat::GraphicsFormatS8UInt))
		_mrsiiStencilFormat = GraphicsFormat::GraphicsFormatS8UInt;
	else
		return false;

	if (pipeline.isTextureSupport(GraphicsFormat::GraphicsFormatR16G16B16A16SFloat))
		_mrsiiLightFormat = GraphicsFormat::GraphicsFormatR16G16B16A16SFloat;
	else if (pipeline.isTextureSupport(GraphicsFormat::GraphicsFormatA2R10G10B10UNormPack32))
		_mrsiiLightFormat = GraphicsFormat::GraphicsFormatA2R10G10B10UNormPack32;
	else if (pipeline.isTextureSupport(GraphicsFormat::GraphicsFormatR8G8B8A8UNorm))
		_mrsiiLightFormat = GraphicsFormat::GraphicsFormatR8G8B8A8UNorm;
	else
		return false;

	return true;
}

bool
DeferredLightingPipeline::setupMRSII(RenderPipeline& pipeline) noexcept
{
	if (!this->setupMRSIIMaterials(*_pipeline))
		return false;

	if (!this->setupMRSIITextures(*_pipeline))
		return false;

	if (!this->setupMRSIIRenderTextureLayouts(*_pipeline))
		return false;

	if (!this->setupMRSIIRenderTextures(*_pipeline))
		return false;

	return true;
}

bool
DeferredLightingPipeline::setupMRSIIMaterials(RenderPipeline& pipeline) noexcept
{
	_mrsii = pipeline.createMaterial("sys:fx/mrsii.fxml"); if (!_mrsii) return false;
	_mrsiiRsm2VPLsSpot = _mrsii->getTech("RSM2VPLsSpot"); if (!_mrsiiRsm2VPLsSpot) return false;
	_mrsiiRsm2VPLsDirectional = _mrsii->getTech("RSM2VPLsDirectional"); if (!_mrsiiRsm2VPLsDirectional) return false;
	_mrsiiGatherIndirect = _mrsii->getTech("GatherIndirect"); if (!_mrsiiGatherIndirect) return false;
	_mrsiiGatherIndirectDebug = _mrsii->getTech("GatherIndirectDebug"); if (!_mrsiiGatherIndirectDebug) return false;
	_mrsiiDepthDerivate = _mrsii->getTech("DepthDerivate"); if (!_mrsiiDepthDerivate) return false;
	_mrsiiDepthDerivateMipmap = _mrsii->getTech("DepthDerivateMipmap"); if (!_mrsiiDepthDerivateMipmap) return false;
	_mrsiiNormalDerivate = _mrsii->getTech("NormalDerivate"); if (!_mrsiiNormalDerivate) return false;
	_mrsiiNormalDerivateMipmap = _mrsii->getTech("NormalDerivateMipmap"); if (!_mrsiiNormalDerivateMipmap) return false;
	_mrsiiComputeSubsplatStencil = _mrsii->getTech("ComputeSubsplatStencil"); if (!_mrsiiComputeSubsplatStencil) return false;
	_mrsiiUpsampling = _mrsii->getTech("Upsampling"); if (!_mrsiiUpsampling) return false;
	_mrsiiUpsamplingWithBlend = _mrsii->getTech("UpsamplingWithBlend"); if (!_mrsiiUpsamplingWithBlend) return false;

	_mrsiiColorMap = _mrsii->getParameter("texColor");
	_mrsiiNormalMap = _mrsii->getParameter("texNormal");
	_mrsiiDepthLinearMap = _mrsii->getParameter("texDepthLinear");
	_mrsiiLightAttenuation = _mrsii->getParameter("lightAttenuation");
	_mrsiiLightColor = _mrsii->getParameter("lightColor");
	_mrsiiLightOuterInner = _mrsii->getParameter("lightOuterInner");
	_mrsiiLightView2EyeView = _mrsii->getParameter("shadowView2EyeView");
	_mrsiiCountGridOffsetDelta = _mrsii->getParameter("VPLCountGridOffsetDelta");

	_mrsiiMRT0 = _mrsii->getParameter("texMRT0");
	_mrsiiMRT1 = _mrsii->getParameter("texMRT1");
	_mrsiiVPLsBuffer = _mrsii->getParameter("texVPLBuffer");
	_mrsiiOffset = _mrsii->getParameter("offset");
	_mrsiiOffsetUpsampling = _mrsii->getParameter("offsetUpsampling");
	_mrsiiMipmapLevel = _mrsii->getParameter("mipmapLevel");
	_mrsiiMipmapPass = _mrsii->getParameter("mipmapPass");
	_mrsiiThreshold = _mrsii->getParameter("threshold");

	return true;
}

bool
DeferredLightingPipeline::setupMRSIITextures(RenderPipeline& pipeline) noexcept
{
	std::uint32_t width, height;
	pipeline.getWindowResolution(width, height);

	GraphicsTextureDesc VPLsBufferDesc;
	VPLsBufferDesc.setWidth(256);
	VPLsBufferDesc.setHeight(3);
	VPLsBufferDesc.setTexFormat(GraphicsFormat::GraphicsFormatR16G16B16A16SFloat);
	VPLsBufferDesc.setSamplerFilter(GraphicsSamplerFilter::GraphicsSamplerFilterNearest, GraphicsSamplerFilter::GraphicsSamplerFilterNearest);
	VPLsBufferDesc.setSamplerWrap(GraphicsSamplerWrap::GraphicsSamplerWrapClampToEdge);
	_mrsiiVPLsBufferMap = pipeline.createTexture(VPLsBufferDesc);
	if (!_mrsiiVPLsBufferMap)
		return false;

	GraphicsTextureDesc depthDerivDesc;
	depthDerivDesc.setWidth(width);
	depthDerivDesc.setHeight(height);
	depthDerivDesc.setTexDim(GraphicsTextureDim::GraphicsTextureDim2D);
	depthDerivDesc.setTexFormat(_mrsiiDepthLinearFormat);
	depthDerivDesc.setSamplerWrap(GraphicsSamplerWrap::GraphicsSamplerWrapClampToEdge);
	depthDerivDesc.setSamplerFilter(GraphicsSamplerFilter::GraphicsSamplerFilterNearestMipmapNearest, GraphicsSamplerFilter::GraphicsSamplerFilterNearest);
	depthDerivDesc.setMipBase(_mrsiiDerivMipBase);
	depthDerivDesc.setMipNums(_mrsiiDerivMipCount);
	_mrsiiDepthDerivMap = pipeline.createTexture(depthDerivDesc);
	if (!_mrsiiDepthDerivMap)
		return false;

	GraphicsTextureDesc normalDeriveDesc;
	normalDeriveDesc.setWidth(width);
	normalDeriveDesc.setHeight(height);
	normalDeriveDesc.setTexDim(GraphicsTextureDim::GraphicsTextureDim2D);
	normalDeriveDesc.setTexFormat(_mrsiiNormalFormat);
	normalDeriveDesc.setSamplerWrap(GraphicsSamplerWrap::GraphicsSamplerWrapClampToEdge);
	normalDeriveDesc.setSamplerFilter(GraphicsSamplerFilter::GraphicsSamplerFilterNearestMipmapNearest, GraphicsSamplerFilter::GraphicsSamplerFilterNearest);
	normalDeriveDesc.setMipBase(_mrsiiDerivMipBase);
	normalDeriveDesc.setMipNums(_mrsiiDerivMipCount);
	_mrsiiNormalDerivMap = pipeline.createTexture(normalDeriveDesc);
	if (!_mrsiiNormalDerivMap)
		return false;

	GraphicsTextureDesc subsplatStencilDesc;
	subsplatStencilDesc.setWidth(width);
	subsplatStencilDesc.setHeight(height);
	subsplatStencilDesc.setTexDim(GraphicsTextureDim::GraphicsTextureDim2D);
	subsplatStencilDesc.setTexFormat(_mrsiiStencilFormat);
	subsplatStencilDesc.setSamplerWrap(GraphicsSamplerWrap::GraphicsSamplerWrapClampToEdge);
	subsplatStencilDesc.setSamplerFilter(GraphicsSamplerFilter::GraphicsSamplerFilterNearestMipmapNearest, GraphicsSamplerFilter::GraphicsSamplerFilterNearest);
	subsplatStencilDesc.setMipBase(_mrsiiDerivMipBase);
	subsplatStencilDesc.setMipNums(_mrsiiDerivMipCount);
	_mrsiiSubsplatStencilMap = pipeline.createTexture(subsplatStencilDesc);
	if (!_mrsiiSubsplatStencilMap)
		return false;

	GraphicsTextureDesc gaterIndirectDesc;
	gaterIndirectDesc.setWidth(width);
	gaterIndirectDesc.setHeight(height);
	gaterIndirectDesc.setTexDim(GraphicsTextureDim::GraphicsTextureDim2D);
	gaterIndirectDesc.setTexFormat(_mrsiiLightFormat);
	gaterIndirectDesc.setSamplerWrap(GraphicsSamplerWrap::GraphicsSamplerWrapClampToEdge);
	gaterIndirectDesc.setSamplerFilter(GraphicsSamplerFilter::GraphicsSamplerFilterNearestMipmapNearest, GraphicsSamplerFilter::GraphicsSamplerFilterNearest);
	gaterIndirectDesc.setMipBase(_mrsiiDerivMipBase);
	gaterIndirectDesc.setMipNums(_mrsiiDerivMipCount);
	_mrsiiGaterIndirectMap = pipeline.createTexture(gaterIndirectDesc);
	if (!_mrsiiGaterIndirectMap)
		return false;

	return true;
}

bool
DeferredLightingPipeline::setupMRSIIRenderTextureLayouts(RenderPipeline& pipeline) noexcept
{
	GraphicsFramebufferLayoutDesc VPLsBufferLayoutDesc;
	VPLsBufferLayoutDesc.addComponent(GraphicsAttachmentLayout(0, GraphicsImageLayout::GraphicsImageLayoutColorAttachmentOptimal, GraphicsFormat::GraphicsFormatR16G16B16A16SFloat));
	_mrsiiVPLsViewLayout = pipeline.createFramebufferLayout(VPLsBufferLayoutDesc);
	if (!_mrsiiVPLsViewLayout)
		return false;

	GraphicsFramebufferLayoutDesc depthDerivLayoutDesc;
	depthDerivLayoutDesc.addComponent(GraphicsAttachmentLayout(0, GraphicsImageLayout::GraphicsImageLayoutColorAttachmentOptimal, _mrsiiDepthLinearFormat));
	_mrsiiDepthDerivViewLayout = pipeline.createFramebufferLayout(depthDerivLayoutDesc);
	if (!_mrsiiDepthDerivViewLayout)
		return false;

	GraphicsFramebufferLayoutDesc depthNormalLayoutDesc;
	depthNormalLayoutDesc.addComponent(GraphicsAttachmentLayout(0, GraphicsImageLayout::GraphicsImageLayoutColorAttachmentOptimal, _mrsiiNormalFormat));
	_mrsiiNormalDerivViewLayout = pipeline.createFramebufferLayout(depthNormalLayoutDesc);
	if (!_mrsiiNormalDerivViewLayout)
		return false;

	GraphicsFramebufferLayoutDesc subsplatStencilViewLayoutDesc;
	subsplatStencilViewLayoutDesc.addComponent(GraphicsAttachmentLayout(2, GraphicsImageLayout::GraphicsImageLayoutDepthStencilAttachmentOptimal, _mrsiiStencilFormat));
	_mrsiiSubsplatStencilViewLayout = pipeline.createFramebufferLayout(subsplatStencilViewLayoutDesc);
	if (!_mrsiiSubsplatStencilViewLayout)
		return false;

	GraphicsFramebufferLayoutDesc gaterIndirectLayoutDesc;
	gaterIndirectLayoutDesc.addComponent(GraphicsAttachmentLayout(0, GraphicsImageLayout::GraphicsImageLayoutColorAttachmentOptimal, _mrsiiLightFormat));
	gaterIndirectLayoutDesc.addComponent(GraphicsAttachmentLayout(1, GraphicsImageLayout::GraphicsImageLayoutDepthStencilReadOnlyOptimal, GraphicsFormat::GraphicsFormatS8UInt));
	_mrsiiGaterIndirectViewLayout = pipeline.createFramebufferLayout(gaterIndirectLayoutDesc);
	if (!_mrsiiGaterIndirectViewLayout)
		return false;

	return true;
}

bool
DeferredLightingPipeline::setupMRSIIRenderTextures(RenderPipeline& pipeline) noexcept
{
	assert(_mrsiiVPLsViewLayout);

	std::uint32_t width, height;
	pipeline.getWindowResolution(width, height);

	GraphicsFramebufferDesc VPLsBufferDesc;
	VPLsBufferDesc.setWidth(256);
	VPLsBufferDesc.setHeight(3);
	VPLsBufferDesc.addColorAttachment(GraphicsAttachmentBinding(_mrsiiVPLsBufferMap, 0, 0));
	VPLsBufferDesc.setGraphicsFramebufferLayout(_mrsiiVPLsViewLayout);
	_mrsiiVPLsView = pipeline.createFramebuffer(VPLsBufferDesc);
	if (!_mrsiiVPLsView)
		return false;

	_mrsiiDepthDerivViews.resize(_mrsiiDerivMipCount);
	_mrsiiNormalDerivViews.resize(_mrsiiDerivMipCount);
	_mrsiiSubsplatStencilViews.resize(_mrsiiDerivMipCount);
	_mrsiiGaterIndirectViews.resize(_mrsiiDerivMipCount);

	for (std::uint32_t i = _mrsiiDerivMipBase; i < _mrsiiDerivMipBase + _mrsiiDerivMipCount; i++)
	{
		GraphicsFramebufferDesc depthDerivFrambufferDesc;
		depthDerivFrambufferDesc.setWidth(width / (1 << i));
		depthDerivFrambufferDesc.setHeight(height / (1 << i));
		depthDerivFrambufferDesc.addColorAttachment(GraphicsAttachmentBinding(_mrsiiDepthDerivMap, i, 0));
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
		normalDerivFrambufferDesc.addColorAttachment(GraphicsAttachmentBinding(_mrsiiNormalDerivMap, i, 0));
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
		subsplatStencilFramebufferDesc.setDepthStencilAttachment(GraphicsAttachmentBinding(_mrsiiSubsplatStencilMap, i, 0));
		subsplatStencilFramebufferDesc.setGraphicsFramebufferLayout(_mrsiiSubsplatStencilViewLayout);
		_mrsiiSubsplatStencilViews[i] = pipeline.createFramebuffer(subsplatStencilFramebufferDesc);
		if (!_mrsiiSubsplatStencilViews[i])
			return false;
	}

	for (std::uint32_t i = _mrsiiDerivMipBase; i < _mrsiiDerivMipBase + _mrsiiDerivMipCount; i++)
	{
		GraphicsFramebufferDesc gaterIndirectFrambufferDesc;
		gaterIndirectFrambufferDesc.setWidth(width / (1 << i));
		gaterIndirectFrambufferDesc.setHeight(height / (1 << i));
		gaterIndirectFrambufferDesc.addColorAttachment(GraphicsAttachmentBinding(_mrsiiGaterIndirectMap, i, 0));
		gaterIndirectFrambufferDesc.setDepthStencilAttachment(GraphicsAttachmentBinding(_mrsiiSubsplatStencilMap, i, 0));
		gaterIndirectFrambufferDesc.setGraphicsFramebufferLayout(_mrsiiGaterIndirectViewLayout);
		_mrsiiGaterIndirectViews[i] = pipeline.createFramebuffer(gaterIndirectFrambufferDesc);
		if (!_mrsiiGaterIndirectViews[i])
			return false;
	}

	return true;
}

void
DeferredLightingPipeline::destroySemantic() noexcept
{
	_materialDeferredDepthMap.reset();
	_materialDeferredDepthLinearMap.reset();
	_materialDeferredGbuffer1Map.reset();
	_materialDeferredGbuffer2Map.reset();
	_materialDeferredGbuffer3Map.reset();
	_materialDeferredLightMap.reset();
	_materialDeferredOpaqueShadingMap.reset();
}

void
DeferredLightingPipeline::destroyDeferredMaterials() noexcept
{
	_deferredLighting.reset();
	_deferredDepthOnly.reset();
	_deferredDepthLinear.reset();
	_deferredSunLight.reset();
	_deferredSunLightShadow.reset();
	_deferredDirectionalLight.reset();
	_deferredDirectionalLightShadow.reset();
	_deferredSpotLight.reset();
	_deferredSpotLightShadow.reset();
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
	_texOpaque.reset();

	_scaleY.reset();
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
DeferredLightingPipeline::destroyMRSIIMaterials() noexcept
{
	_mrsiiColorMap.reset();
	_mrsiiNormalMap.reset();
	_mrsiiDepthLinearMap.reset();
	_mrsiiLightOuterInner.reset();
	_mrsiiLightAttenuation.reset();
	_mrsiiCountGridOffsetDelta.reset();
	_mrsiiLightView2EyeView.reset();
	_mrsiiOffset.reset();
	_mrsiiOffsetUpsampling.reset();
	_mrsiiMRT0.reset();
	_mrsiiMRT1.reset();
	_mrsiiVPLsBuffer.reset();
	_mrsiiMipmapLevel.reset();
	_mrsiiMipmapPass.reset();
	_mrsiiThreshold.reset();
	_mrsiiRsm2VPLsSpot.reset();
	_mrsiiRsm2VPLsDirectional.reset();
	_mrsiiGatherIndirect.reset();
	_mrsiiGatherIndirectDebug.reset();
	_mrsiiDepthDerivate.reset();
	_mrsiiDepthDerivateMipmap.reset();
	_mrsiiNormalDerivate.reset();
	_mrsiiNormalDerivateMipmap.reset();
	_mrsiiComputeSubsplatStencil.reset();
	_mrsiiUpsampling.reset();
	_mrsiiUpsamplingWithBlend.reset();
	_mrsii.reset();
}

void
DeferredLightingPipeline::destroyMRSIITextures() noexcept
{
	_mrsiiVPLsBufferMap.reset();
	_mrsiiDepthDerivMap.reset();
	_mrsiiNormalDerivMap.reset();
	_mrsiiSubsplatStencilMap.reset();
	_mrsiiGaterIndirectMap.reset();
}

void
DeferredLightingPipeline::destroyMRSIIRenderTextures() noexcept
{
	_mrsiiDepthDerivViews.clear();
	_mrsiiNormalDerivViews.clear();
	_mrsiiSubsplatStencilViews.clear();
	_mrsiiGaterIndirectViews.clear();
	_mrsiiVPLsView.reset();
}

void
DeferredLightingPipeline::destroyMRSIIRenderTextureLayouts() noexcept
{
	_mrsiiVPLsViewLayout.reset();
	_mrsiiDepthDerivViewLayout.reset();
	_mrsiiNormalDerivViewLayout.reset();
	_mrsiiSubsplatStencilViewLayout.reset();
	_mrsiiGaterIndirectViewLayout.reset();
}

void
DeferredLightingPipeline::onResolutionChange() noexcept
{
	if (_enabledMRSSI && _pipelineManager->getRenderSetting().enableGlobalIllumination)
	{
		destroyMRSIIRenderTextures();
		destroyMRSIITextures();
		setupMRSIITextures(*_pipeline);
		setupMRSIIRenderTextures(*_pipeline);
	}
}

void
DeferredLightingPipeline::onRenderPre() noexcept
{
}

void
DeferredLightingPipeline::onRenderPipeline(const CameraPtr& camera) noexcept
{
	assert(camera);
	assert(camera->getCameraOrder() == CameraOrder::CameraOrder3D);

	auto framebuffers = camera->getRenderPipelineFramebuffer()->downcast<DeferredLightingFramebuffers>();
	_materialDeferredDepthMap->uniformTexture(framebuffers->getDeferredDepthMap());
	_materialDeferredDepthLinearMap->uniformTexture(framebuffers->getDeferredDepthLinearMap());
	_materialDeferredGbuffer1Map->uniformTexture(framebuffers->getDeferredGbuffer1Map());
	_materialDeferredGbuffer2Map->uniformTexture(framebuffers->getDeferredGbuffer2Map());
	_materialDeferredGbuffer3Map->uniformTexture(framebuffers->getDeferredGbuffer3Map());
	_materialDeferredLightMap->uniformTexture(framebuffers->getDeferredLightingMap());

	_texMRT0->uniformTexture(framebuffers->getDeferredGbuffer1Map());
	_texMRT1->uniformTexture(framebuffers->getDeferredGbuffer2Map());
	_texMRT2->uniformTexture(framebuffers->getDeferredGbuffer3Map());

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

	auto flags = camera->getCameraRenderFlags();
	if (flags & CameraRenderFlagBits::CameraRenderTextureBit)
	{
		auto framebuffers = camera->getRenderPipelineFramebuffer()->downcast<DeferredLightingFramebuffers>();
		if (framebuffers)
		{
			float4 viewport = camera->getPixelViewport();
			this->copyRenderTexture(*_pipeline, framebuffers->getDeferredFinalShadingMap(), framebuffers->getFramebuffer());
		}
	}

	if (flags & CameraRenderFlagBits::CameraRenderScreenBit)
	{
		auto framebuffers = camera->getRenderPipelineFramebuffer()->downcast<DeferredLightingFramebuffers>();
		if (framebuffers)
		{
			float4 viewport = camera->getPixelViewportDPI();
			this->copyRenderTexture(*_pipeline, framebuffers->getDeferredFinalShadingMap(), nullptr, viewport);
		}
	}
}

_NAME_END