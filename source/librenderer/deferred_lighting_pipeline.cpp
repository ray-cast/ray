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
	: _shadowEsmFactor(300.0f)
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

	_pipeline->drawPostProcess(RenderQueue::RenderQueuePostprocess, _deferredFinalShadingView, _deferredSwapView);
}

void
DeferredLightingPipeline::renderOpaques(RenderPipeline& pipeline, GraphicsFramebufferPtr& target) noexcept
{
	pipeline.setFramebuffer(target);
	pipeline.clearFramebuffer(GraphicsClearFlagBits::GraphicsClearFlagAllBit, float4::Zero, 1.0, 0);
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
	pipeline.clearFramebuffer(GraphicsClearFlagBits::GraphicsClearFlagColorBit, pipeline.getCamera()->getClearColor(), 1.0, 0);
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
	pipeline.clearFramebuffer(GraphicsClearFlagBits::GraphicsClearFlagStencilBit, float4::Zero, 1.0, 0);
	pipeline.clearFramebufferi(GraphicsClearFlagBits::GraphicsClearFlagColorBit, float4::Zero, 1.0, 0, 0);
	pipeline.clearFramebufferi(GraphicsClearFlagBits::GraphicsClearFlagColorBit, float4::Zero, 1.0, 0, 1);
	pipeline.clearFramebufferi(GraphicsClearFlagBits::GraphicsClearFlagColorBit, float4(1.0,1.0,1.0,0.0), 1.0, 0, 2);
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
	pipeline.setFramebuffer(target);
	pipeline.clearFramebuffer(GraphicsClearFlagBits::GraphicsClearFlagColorBit, float4::Zero, 1.0, 0);

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
	
	auto& shadowMap = light.getShadowMap();
	if (shadowMap)
	{
		float shadowFactor = _shadowEsmFactor / (light.getShadowCamera()->getFar() - light.getShadowCamera()->getNear());
		float shaodwBias = light.getShadowBias();

		_shadowMap->uniformTexture(shadowMap);
		_shadowFactor->uniform2f(shadowFactor, shaodwBias);
		_shadowView2LightView->uniform4f((light.getShadowCamera()->getView()).getAxisZ() * pipeline.getCamera()->getViewInverse());
		_shadowView2LightViewProject->uniform4fmat(light.getShadowCamera()->getViewProject() * pipeline.getCamera()->getViewInverse());

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

	auto& shadowMap = light.getShadowMap();
	if (shadowMap)
	{
		float shadowFactor = _shadowEsmFactor / (light.getShadowCamera()->getFar() - light.getShadowCamera()->getNear());
		float shaodwBias = light.getShadowBias();

		_shadowMap->uniformTexture(shadowMap);
		_shadowFactor->uniform2f(shadowFactor, shaodwBias);
		_shadowView2LightView->uniform4f((light.getShadowCamera()->getView()).getAxisZ() * pipeline.getCamera()->getViewInverse());
		_shadowView2LightViewProject->uniform4fmat(light.getShadowCamera()->getViewProject() * pipeline.getCamera()->getViewInverse());

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

	auto& shadowMap = light.getShadowMap();
	if (shadowMap)
	{
		float shadowFactor = _shadowEsmFactor / (light.getShadowCamera()->getFar() - light.getShadowCamera()->getNear());
		float shaodwBias = light.getShadowBias();

		_shadowMap->uniformTexture(shadowMap);
		_shadowFactor->uniform2f(shadowFactor, shaodwBias);
		_shadowView2LightView->uniform4f(light.getShadowCamera()->getView().getAxisZ() * pipeline.getCamera()->getViewInverse());
		_shadowView2LightViewProject->uniform4fmat(light.getShadowCamera()->getViewProject() * pipeline.getCamera()->getViewInverse());

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
	_lightEyePosition->uniform3f(light.getTranslate());
	_lightEyeDirection->uniform3f(math::invRotateVector3(pipeline.getCamera()->getTransform(), light.getForward()));
	_lightAttenuation->uniform3f(light.getLightAttenuation());

	pipeline.drawScreenQuadLayer(*_deferredAmbientLight, light.getLayer());
}

void
DeferredLightingPipeline::copyRenderTexture(RenderPipeline& pipeline, GraphicsTexturePtr& src, GraphicsFramebufferPtr dst, const Viewport& viewport) noexcept
{
	_texSource->uniformTexture(src);
	pipeline.setFramebuffer(dst);
	pipeline.setViewport(viewport);
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
	_deferredDepthDesc.setTexDim(GraphicsTextureDim2D);
	_deferredDepthDesc.setTexFormat(_deferredDepthFormat);
	_deferredDepthMap = pipeline.createTexture(_deferredDepthDesc);
	if (!_deferredDepthMap)
		return false;

	GraphicsTextureDesc _deferredDepthLinearDesc;
	_deferredDepthLinearDesc.setWidth(width);
	_deferredDepthLinearDesc.setHeight(height);
	_deferredDepthLinearDesc.setTexDim(GraphicsTextureDim2D);
	_deferredDepthLinearDesc.setTexFormat(_deferredDepthLinearFormat);
	_deferredDepthLinearMap = pipeline.createTexture(_deferredDepthLinearDesc);
	if (!_deferredDepthLinearMap)
		return false;

	GraphicsTextureDesc _deferredOpaqueDesc;
	_deferredOpaqueDesc.setWidth(width);
	_deferredOpaqueDesc.setHeight(height);
	_deferredOpaqueDesc.setTexDim(GraphicsTextureDim2D);
	_deferredOpaqueDesc.setTexFormat(_deferredOpaqueFormat);
	_deferredOpaqueDesc.setSamplerFilter(GraphicsSamplerFilter::GraphicsSamplerFilterNearest);
	_deferredOpaqueMap = pipeline.createTexture(_deferredOpaqueDesc);
	if (!_deferredOpaqueMap)
		return false;

	GraphicsTextureDesc _deferredNormalDesc;
	_deferredNormalDesc.setWidth(width);
	_deferredNormalDesc.setHeight(height);
	_deferredNormalDesc.setTexDim(GraphicsTextureDim2D);
	_deferredNormalDesc.setTexFormat(_deferredNormalFormat);
	_deferredNormalDesc.setSamplerFilter(GraphicsSamplerFilter::GraphicsSamplerFilterNearest);
	_deferredNormalMap = pipeline.createTexture(_deferredNormalDesc);
	if (!_deferredNormalMap)
		return false;

	GraphicsTextureDesc _deferredTransparentDesc;
	_deferredTransparentDesc.setWidth(width);
	_deferredTransparentDesc.setHeight(height);
	_deferredTransparentDesc.setTexDim(GraphicsTextureDim2D);
	_deferredTransparentDesc.setTexFormat(_deferredTransparentFormat);
	_deferredTransparentDesc.setSamplerFilter(GraphicsSamplerFilter::GraphicsSamplerFilterNearest);
	_deferredTransparentMap = pipeline.createTexture(_deferredTransparentDesc);
	if (!_deferredTransparentMap)
		return false;

	GraphicsTextureDesc _deferredAbufferDesc;
	_deferredAbufferDesc.setWidth(width);
	_deferredAbufferDesc.setHeight(height);
	_deferredAbufferDesc.setTexDim(GraphicsTextureDim2D);
	_deferredAbufferDesc.setTexFormat(_deferredAbufferFormat);
	_deferredAbufferDesc.setSamplerFilter(GraphicsSamplerFilter::GraphicsSamplerFilterNearest);
	_deferredAbufferMap = pipeline.createTexture(_deferredAbufferDesc);
	if (!_deferredAbufferMap)
		return false;

	GraphicsTextureDesc _deferredLightDesc;
	_deferredLightDesc.setWidth(width);
	_deferredLightDesc.setHeight(height);
	_deferredLightDesc.setTexDim(GraphicsTextureDim2D);
	_deferredLightDesc.setTexFormat(_deferredLightFormat);
	_deferredLightDesc.setSamplerFilter(GraphicsSamplerFilter::GraphicsSamplerFilterLinear);
	_deferredLightingMap = pipeline.createTexture(_deferredLightDesc);
	if (!_deferredLightingMap)
		return false;

	GraphicsTextureDesc _deferredShadingDesc;
	_deferredShadingDesc.setWidth(width);
	_deferredShadingDesc.setHeight(height);
	_deferredShadingDesc.setTexDim(GraphicsTextureDim2D);
	_deferredShadingDesc.setTexFormat(_deferredShadingFormat);
	_deferredShadingDesc.setSamplerFilter(GraphicsSamplerFilter::GraphicsSamplerFilterLinear);
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
	deferredDepthDesc.setSharedDepthStencilTexture(_deferredDepthMap);
	deferredDepthDesc.setGraphicsFramebufferLayout(_deferredDepthImageLayout);
	_deferredDepthView = pipeline.createFramebuffer(deferredDepthDesc);
	if (!_deferredDepthView)
		return false;

	GraphicsFramebufferDesc deferredDepthLinearDesc;
	deferredDepthLinearDesc.setWidth(width);
	deferredDepthLinearDesc.setHeight(height);
	deferredDepthLinearDesc.attach(_deferredDepthLinearMap);
	deferredDepthLinearDesc.setGraphicsFramebufferLayout(_deferredDepthLinearImageLayout);
	_deferredDepthLinearView = pipeline.createFramebuffer(deferredDepthLinearDesc);
	if (!_deferredDepthLinearView)
		return false;

	GraphicsFramebufferDesc deferredGraphicsDesc;
	deferredGraphicsDesc.setWidth(width);
	deferredGraphicsDesc.setHeight(height);
	deferredGraphicsDesc.attach(_deferredOpaqueMap);
	deferredGraphicsDesc.setGraphicsFramebufferLayout(_deferredGraphicsImageLayout);
	_deferredGraphicsView = pipeline.createFramebuffer(deferredGraphicsDesc);
	if (!_deferredGraphicsView)
		return false;

	GraphicsFramebufferDesc deferredNormalDesc;
	deferredNormalDesc.setWidth(width);
	deferredNormalDesc.setHeight(height);
	deferredNormalDesc.attach(_deferredNormalMap);
	deferredNormalDesc.setGraphicsFramebufferLayout(_deferredNormalImageLayout);
	_deferredNormalView = pipeline.createFramebuffer(deferredNormalDesc);
	if (!_deferredNormalView)
		return false;

	GraphicsFramebufferDesc deferredAbufferDesc;
	deferredAbufferDesc.setWidth(width);
	deferredAbufferDesc.setHeight(height);
	deferredAbufferDesc.attach(_deferredAbufferMap);
	deferredAbufferDesc.setGraphicsFramebufferLayout(_deferredAbufferImageLayout);
	_deferredAbufferView = pipeline.createFramebuffer(deferredAbufferDesc);
	if (!_deferredAbufferView)
		return false;

	GraphicsFramebufferDesc deferredLightingDesc;
	deferredLightingDesc.setWidth(width);
	deferredLightingDesc.setHeight(height);
	deferredLightingDesc.setSharedDepthStencilTexture(_deferredDepthMap);
	deferredLightingDesc.attach(_deferredLightingMap);
	deferredLightingDesc.setGraphicsFramebufferLayout(_deferredLightingImageLayout);
	_deferredLightingView = pipeline.createFramebuffer(deferredLightingDesc);
	if (!_deferredLightingView)
		return false;

	GraphicsFramebufferDesc deferredOpaqueDesc;
	deferredOpaqueDesc.setWidth(width);
	deferredOpaqueDesc.setHeight(height);
	deferredOpaqueDesc.setSharedDepthStencilTexture(_deferredDepthMap);
	deferredOpaqueDesc.attach(_deferredOpaqueMap);
	deferredOpaqueDesc.attach(_deferredNormalMap);
	deferredOpaqueDesc.setGraphicsFramebufferLayout(_deferredOpaqueImagesLayout);
	_deferredOpaqueViews = pipeline.createFramebuffer(deferredOpaqueDesc);
	if (!_deferredOpaqueViews)
		return false;

	GraphicsFramebufferDesc deferredTransparentDesc;
	deferredTransparentDesc.setWidth(width);
	deferredTransparentDesc.setHeight(height);
	deferredTransparentDesc.setSharedDepthStencilTexture(_deferredDepthMap);
	deferredTransparentDesc.attach(_deferredTransparentMap);
	deferredTransparentDesc.attach(_deferredNormalMap);
	deferredTransparentDesc.attach(_deferredAbufferMap);
	deferredTransparentDesc.setGraphicsFramebufferLayout(_deferredTransparentImagesLayout);
	_deferredTransparentViews = pipeline.createFramebuffer(deferredTransparentDesc);
	if (!_deferredTransparentViews)
		return false;

	GraphicsFramebufferDesc deferredOpaqueShadingDesc;
	deferredOpaqueShadingDesc.setWidth(width);
	deferredOpaqueShadingDesc.setHeight(height);
	deferredOpaqueShadingDesc.setSharedDepthStencilTexture(_deferredDepthMap);
	deferredOpaqueShadingDesc.attach(_deferredOpaqueShadingMap);
	deferredOpaqueShadingDesc.setGraphicsFramebufferLayout(_deferredShadingImageLayout);
	_deferredOpaqueShadingView = pipeline.createFramebuffer(deferredOpaqueShadingDesc);
	if (!_deferredOpaqueShadingView)
		return false;

	GraphicsFramebufferDesc deferredTransparentShadingDesc;
	deferredTransparentShadingDesc.setWidth(width);
	deferredTransparentShadingDesc.setHeight(height);
	deferredTransparentShadingDesc.setSharedDepthStencilTexture(_deferredDepthMap);
	deferredTransparentShadingDesc.attach(_deferredFinalShadingMap);
	deferredTransparentShadingDesc.setGraphicsFramebufferLayout(_deferredShadingImageLayout);
	_deferredFinalShadingView = pipeline.createFramebuffer(deferredTransparentShadingDesc);
	if (!_deferredFinalShadingView)
		return false;

	GraphicsFramebufferDesc deferredSwapDesc;
	deferredSwapDesc.setWidth(width);
	deferredSwapDesc.setHeight(height);
	deferredSwapDesc.setSharedDepthStencilTexture(_deferredDepthMap);
	deferredSwapDesc.attach(_deferredSwapMap);
	deferredSwapDesc.setGraphicsFramebufferLayout(_deferredShadingImageLayout);
	_deferredSwapView = pipeline.createFramebuffer(deferredSwapDesc);
	if (!_deferredSwapView)
		return false;

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

	_pipeline->setCamera(camera);
	this->render3DEnvMap(camera);
}

void
DeferredLightingPipeline::onRenderPost() noexcept
{
	auto camera = _pipeline->getCamera();
	if (!camera)
		return;

	float4 pixel = camera->getPixelViewport();

	Viewport viewport;
	viewport.left = pixel.x;
	viewport.top = pixel.y;
	viewport.width = pixel.z;
	viewport.height = pixel.w;

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
				this->copyRenderTexture(*_pipeline, _deferredOpaqueShadingMap, framebuffer, viewport);
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