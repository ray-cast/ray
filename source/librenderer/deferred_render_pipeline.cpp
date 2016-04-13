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
#include "deferred_render_pipeline.h"
#include <ray/render_pipeline.h>
#include <ray/camera.h>
#include <ray/light.h>
#include <ray/render_scene.h>
#include <ray/graphics_state.h>
#include <ray/graphics_texture.h>
#include <ray/graphics_framebuffer.h>
#include <ray/material_manager.h>

#include "ssss.h"

_NAME_BEGIN

DeferredRenderPipeline::DeferredRenderPipeline() noexcept
	: _shaodwFactor(300.0f)
	, _shaodwMapSize(512, 512)
	, _shadowFormat(GraphicsFormat::GraphicsFormatR32SFloat)
{
}

DeferredRenderPipeline::~DeferredRenderPipeline() noexcept
{
	this->close();
}

void
DeferredRenderPipeline::renderShadowMaps(RenderPipeline& pipeline) noexcept
{
	const auto lights = pipeline.getRenderData(RenderQueue::RenderQueueShadow, RenderPass::RenderPassLights);
	
	std::size_t numSoftLight = 0;
	for (auto& it : lights)
	{
		auto light = it->downcast<Light>();
		if (light->getSoftShadow())
			numSoftLight++;
	}

	if (_softShaodwViews.size() < numSoftLight)
	{
		auto num = numSoftLight - _softShaodwViews.size();
		for (std::size_t i = 0; i < num; i++)
		{
			auto blurShaodwMap = pipeline.createTexture(_shaodwMapSize.x, _shaodwMapSize.y, GraphicsTextureDim::GraphicsTextureDim2D, _shadowFormat);
			if (!blurShaodwMap)
				return;

			GraphicsFramebufferDesc shadowViewDesc;
			shadowViewDesc.setWidth(_shaodwMapSize.x);
			shadowViewDesc.setHeight(_shaodwMapSize.y);
			shadowViewDesc.attach(blurShaodwMap);
			shadowViewDesc.setGraphicsFramebufferLayout(_softShaodwViewLayout);
			auto blurShaodwView = pipeline.createFramebuffer(shadowViewDesc);
			if (!blurShaodwView)
				return;

			_softShaodwMaps.push_back(blurShaodwMap);
			_softShaodwViews.push_back(blurShaodwView);
		}
	}

	numSoftLight = 0;

	for (auto& it : lights)
	{
		auto light = it->downcast<Light>();
		pipeline.setCamera(light->getShadowCamera());
		pipeline.setFramebuffer(light->getShadowCamera()->getFramebuffer());
		pipeline.clearFramebuffer(GraphicsClearFlags::GraphicsClearFlagsDepth, float4::Zero, 1.0, 0);
		pipeline.drawRenderQueue(RenderQueue::RenderQueueOpaque, RenderPass::RenderPassOpaques, _deferredDepthOnly);

		if (light->getSoftShadow())
		{
			_softBlurShadowSource->assign(light->getShadowMap());
			_softBlurShadowSourceInv->assign(float2(1.0f, 1.0f) / light->getShadowSize());
			pipeline.setFramebuffer(_softShaodwViewTemp);
			pipeline.drawScreenQuad(_softBlurShadowX);

			_softBlurShadowSource->assign(_softShaodwMapTemp);
			_softBlurShadowSourceInv->assign(float2(1.0f, 1.0f) / _shaodwMapSize);
			pipeline.setFramebuffer(_softShaodwViews[numSoftLight]);
			pipeline.drawScreenQuad(_softBlurShadowY);

			numSoftLight++;
		}
	}
}

void
DeferredRenderPipeline::render2DEnvMap(RenderPipeline& pipeline) noexcept
{
	pipeline.setFramebuffer(_deferredFinalView);
	pipeline.clearFramebuffer(GraphicsClearFlags::GraphicsClearFlagsAll, pipeline.getCamera()->getClearColor(), 1.0, 0);
	pipeline.drawRenderQueue(RenderQueue::RenderQueueOpaque, RenderPass::RenderPassOpaques);
}

void
DeferredRenderPipeline::render3DEnvMap(RenderPipeline& pipeline) noexcept
{
	_materialDeferredDepthMap->assign(_deferredDepthMap);
	_materialDeferredDepthLinearMap->assign(_deferredDepthLinearMap);
	_materialDeferredNormalMap->assign(_deferredNormalMap);
	_materialDeferredGraphicMap->assign(_deferredGraphicsMap);
	_materialDeferredLightMap->assign(_deferredLightMap);

	_materialDepthMap->assign(_deferredDepthMap);
	_materialColorMap->assign(_deferredShadingMap);
	_materialNormalMap->assign(_deferredNormalMap);

	_clipInfo->assign(pipeline.getCamera()->getClipConstant());
	_projInfo->assign(pipeline.getCamera()->getProjConstant());

	this->renderOpaques(pipeline, _deferredGraphicsViews);
	this->renderOpaquesDepthLinear(pipeline, _deferredDepthLinearView);
	this->renderLights(pipeline, _deferredLightingView);
	this->renderOpaquesShading(pipeline, _deferredShadingView);
	this->renderOpaquesSpecificShading(pipeline, _deferredShadingView);

	/*this->renderTransparent(_deferredGraphicsViews);
	this->renderLights(_deferredLightingView);
	this->renderTransparentShading(_deferredShadingView);
	this->renderTransparentSpecificShading(_deferredShadingView);*/

	pipeline.drawPostProcess(RenderQueue::RenderQueuePostprocess, _deferredShadingView, _deferredSwapView, _deferredFinalView);

	if (_SSSS)
	{
		auto ssss = _SSSS->downcast<SSSS>();
		if (!ssss->getActive())
		{
			_SSSS->_setRenderPipeline(&pipeline);
			_SSSS->setActive(true);
		}

		std::size_t shadowIndex = 0;

		auto& lights = pipeline.getRenderData(RenderQueue::RenderQueueLighting, RenderPass::RenderPassLights);
		for (auto& it : lights)
		{
			auto light = std::dynamic_pointer_cast<Light>(it);
			if (light->getShadow() && light->getSubsurfaceScattering())
			{
				if (light->getSoftShadow())
					ssss->translucency(pipeline, light, _softShaodwMaps[shadowIndex], _deferredFinalView);
				else
					ssss->translucency(pipeline, light, light->getShadowMap(), _deferredFinalView);
			}

			if (light->getSoftShadow())
				shadowIndex++;
		}

		ssss->blurX(pipeline, _deferredFinalMap, _deferredSwapView);
		ssss->blurX(pipeline, _deferredSwapMap, _deferredFinalView);
	}

	auto renderTexture = pipeline.getCamera()->getFramebuffer();
	if (renderTexture)
		this->copyRenderTexture(pipeline, _deferredFinalMap, renderTexture, pipeline.getCamera()->getViewport());
}

void
DeferredRenderPipeline::renderOpaques(RenderPipeline& pipeline, GraphicsFramebufferPtr target) noexcept
{
	pipeline.setFramebuffer(target);
	pipeline.clearFramebuffer(GraphicsClearFlags::GraphicsClearFlagsAll, float4::Zero, 1.0, 0);
	pipeline.drawRenderQueue(RenderQueue::RenderQueueOpaque, RenderPass::RenderPassOpaques);
}

void
DeferredRenderPipeline::renderOpaquesDepthLinear(RenderPipeline& pipeline, GraphicsFramebufferPtr target) noexcept
{
	pipeline.setFramebuffer(target);
	pipeline.clearFramebuffer(GraphicsClearFlags::GraphicsClearFlagsAll, float4::Zero, 1.0, 0);
	pipeline.drawScreenQuad(_deferredDepthLinear);
}

void
DeferredRenderPipeline::renderOpaquesShading(RenderPipeline& pipeline, GraphicsFramebufferPtr target) noexcept
{
	pipeline.setFramebuffer(target);
	pipeline.clearFramebuffer(GraphicsClearFlags::GraphicsClearFlagsColor, pipeline.getCamera()->getClearColor(), 1.0, 0);
	pipeline.drawScreenQuad(_deferredShadingOpaques);
}

void
DeferredRenderPipeline::renderOpaquesSpecificShading(RenderPipeline& pipeline, GraphicsFramebufferPtr target) noexcept
{
	pipeline.setFramebuffer(target);
	pipeline.drawRenderQueue(RenderQueue::RenderQueueOpaque, RenderPass::RenderPassSpecific);
	pipeline.drawPostProcess(RenderQueue::RenderQueueOpaque, target, target, _deferredFinalView);
}

void
DeferredRenderPipeline::renderTransparent(RenderPipeline& pipeline, GraphicsFramebufferPtr renderTexture) noexcept
{
	pipeline.setFramebuffer(renderTexture);
	pipeline.clearFramebuffer(GraphicsClearFlags::GraphicsClearFlagsColorStencil, float4::Zero, 1.0, 0);
	pipeline.drawRenderQueue(RenderQueue::RenderQueueTransparent, RenderPass::RenderPassTransparent);
}

void
DeferredRenderPipeline::renderTransparentDepthLinear(RenderPipeline& pipeline, GraphicsFramebufferPtr target) noexcept
{
	pipeline.setFramebuffer(target);
	pipeline.clearFramebuffer(GraphicsClearFlags::GraphicsClearFlagsColor, float4::Zero, 1.0, 0);
	pipeline.drawRenderQueue(RenderQueue::RenderQueueTransparent, RenderPass::RenderPassDepth, _deferredDepthLinear);
}

void
DeferredRenderPipeline::renderTransparentShading(RenderPipeline& pipeline, GraphicsFramebufferPtr target) noexcept
{
	pipeline.setFramebuffer(target);
	pipeline.drawScreenQuad(_deferredShadingTransparents);
}

void
DeferredRenderPipeline::renderTransparentSpecificShading(RenderPipeline& pipeline, GraphicsFramebufferPtr target) noexcept
{
	pipeline.setFramebuffer(target);
	pipeline.drawRenderQueue(RenderQueue::RenderQueueTransparent, RenderPass::RenderPassSpecific);
	pipeline.drawRenderQueue(RenderQueue::RenderQueueTransparent, RenderPass::RenderPassPostprocess);
}

void
DeferredRenderPipeline::renderLights(RenderPipeline& pipeline, GraphicsFramebufferPtr target) noexcept
{
	pipeline.setFramebuffer(target);
	pipeline.clearFramebuffer(GraphicsClearFlags::GraphicsClearFlagsColor, float4::Zero, 1.0, 0);

	std::size_t shadowIndex = 0;

	auto& lights = pipeline.getRenderData(RenderQueue::RenderQueueLighting, RenderPass::RenderPassLights);
	for (auto& it : lights)
	{
		auto light = std::dynamic_pointer_cast<Light>(it);
		GraphicsTexturePtr lightShadow = nullptr;

		if (light->getSoftShadow())
		{
			lightShadow = _softShaodwMaps[shadowIndex];
			shadowIndex++;
		}
	
		switch (light->getLightType())
		{
		case LightType::LightTypeSun:
			this->renderSunLight(pipeline, *light, lightShadow);
			break;
		case LightType::LightTypeDirectional:
			this->renderDirectionalLight(pipeline, *light, lightShadow);
			break;
		case LightType::LightTypePoint:
			this->renderPointLight(pipeline, *light, lightShadow);
			break;
		case LightType::LightTypeSpot:
			this->renderSpotLight(pipeline, *light, lightShadow);
			break;
		case LightType::LightTypeAmbient:
			this->renderAmbientLight(pipeline, *light, lightShadow);
			break;
		default:
			break;
		}
	}

	pipeline.drawPostProcess(RenderQueue::RenderQueueLighting, target, target, _deferredFinalView);
}

void
DeferredRenderPipeline::renderSunLight(RenderPipeline& pipeline, const Light& light, GraphicsTexturePtr shadowMap) noexcept
{
	_lightColor->assign(light.getLightColor() * light.getIntensity());
	_lightEyeDirection->assign(light.getForward() * float3x3(pipeline.getCamera()->getView()));
	_lightAttenuation->assign(light.getLightAttenuation());
	
	if (light.getShadow())
	{	
		float3 clipConstant = light.getShadowCamera()->getClipConstant().xyz();
		float shadowFactor = _shaodwFactor / (light.getShadowCamera()->getFar() - light.getShadowCamera()->getNear());

		_shadowMap->assign(shadowMap ? shadowMap : light.getShadowMap());
		_shadowFactor->assign(float4(clipConstant, shadowFactor));
		_shadowView2LightView->assign((pipeline.getCamera()->getViewInverse() * light.getShadowCamera()->getView()).getAxisZ());
		_shadowView2LightViewProject->assign(pipeline.getCamera()->getViewInverse() * light.getShadowCamera()->getViewProject());

		pipeline.drawScreenQuad(_deferredSunLightShadow);
	}
	else
	{
		pipeline.drawScreenQuad(_deferredSunLight);
	}
}

void
DeferredRenderPipeline::renderDirectionalLight(RenderPipeline& pipeline, const Light& light, GraphicsTexturePtr shadowMap) noexcept
{
	_lightColor->assign(light.getLightColor() * light.getIntensity());
	_lightEyeDirection->assign(light.getForward() * float3x3(pipeline.getCamera()->getView()));
	_lightAttenuation->assign(light.getLightAttenuation());

	if (light.getShadow())
	{
		float3 clipConstant = light.getShadowCamera()->getClipConstant().xyz();
		float shadowFactor = _shaodwFactor / (light.getShadowCamera()->getFar() - light.getShadowCamera()->getNear());

		_shadowMap->assign(shadowMap ? shadowMap : light.getShadowMap());
		_shadowFactor->assign(float4(clipConstant, shadowFactor));
		_shadowView2LightView->assign((pipeline.getCamera()->getViewInverse() * light.getShadowCamera()->getView()).getAxisZ());
		_shadowView2LightViewProject->assign(pipeline.getCamera()->getViewInverse() * light.getShadowCamera()->getViewProject());

		pipeline.drawScreenQuad(_deferredDirectionalLightShadow);
	}
	else
	{
		pipeline.drawScreenQuad(_deferredDirectionalLight);
	}
}

void
DeferredRenderPipeline::renderPointLight(RenderPipeline& pipeline, const Light& light, GraphicsTexturePtr shadowMap) noexcept
{
	_lightColor->assign(light.getLightColor() * light.getIntensity());
	_lightEyePosition->assign(light.getTransform().getTranslate() * pipeline.getCamera()->getView());
	_lightAttenuation->assign(light.getLightAttenuation());

	auto transform = light.getTransform();
	transform.scale(light.getRange());

	pipeline.setTransform(transform);
	pipeline.drawSphere(_deferredPointLight);
}

void
DeferredRenderPipeline::renderSpotLight(RenderPipeline& pipeline, const Light& light, GraphicsTexturePtr shadowMap) noexcept
{
	_lightColor->assign(light.getLightColor() * light.getIntensity());
	_lightEyePosition->assign(light.getTransform().getTranslate() * pipeline.getCamera()->getView());
	_lightEyeDirection->assign(light.getForward() * float3x3(pipeline.getCamera()->getView()));
	_lightAttenuation->assign(light.getLightAttenuation());
	_lightOuterInner->assign(float2(light.getSpotOuterCone(), light.getSpotInnerCone()));
	
	auto transform = light.getTransform();
	transform.translate(light.getForward() * light.getRange());
	transform.scale(float3(0.5, 1.0, 0.5) * light.getRange());

	pipeline.setTransform(transform);

	if (light.getShadow())
	{
		_shadowMap->assign(shadowMap ? shadowMap : light.getShadowMap());
		_shadowFactor->assign(float4(light.getShadowCamera()->getClipConstant().xyz(), _shaodwFactor / (light.getShadowCamera()->getFar() - light.getShadowCamera()->getNear())));
		_shadowView2LightView->assign((pipeline.getCamera()->getViewInverse() * light.getShadowCamera()->getView()).getAxisZ());
		_shadowView2LightViewProject->assign(pipeline.getCamera()->getViewInverse() * light.getShadowCamera()->getViewProject());

		pipeline.drawCone(_deferredSpotLightShadow);
	}
	else
	{
		pipeline.drawCone(_deferredSpotLight);
	}
}

void
DeferredRenderPipeline::renderAmbientLight(RenderPipeline& pipeline, const Light& light, GraphicsTexturePtr shadowMap) noexcept
{
	_lightColor->assign(light.getLightColor() * light.getIntensity());
	_lightEyePosition->assign(light.getTransform().getTranslate());
	_lightEyeDirection->assign(light.getForward() * float3x3(pipeline.getCamera()->getView()));
	_lightAttenuation->assign(light.getLightAttenuation());
	
	pipeline.drawScreenQuad(_deferredAmbientLight);
}

void
DeferredRenderPipeline::copyRenderTexture(RenderPipeline& pipeline, GraphicsTexturePtr src, GraphicsFramebufferPtr dst, const Viewport& view) noexcept
{
	_texSource->assign(src);
	pipeline.setFramebuffer(dst);
	pipeline.drawScreenQuad(_deferredCopyOnly);
}

bool
DeferredRenderPipeline::enableSSSS(bool enable) noexcept
{
	if (!_SSSS)
	{
		_SSSS = std::make_shared<SSSS>();
	}

	return true;
}

bool
DeferredRenderPipeline::isEnableSSSS() const noexcept
{
	return _SSSS ? true : false;
}

bool
DeferredRenderPipeline::setupSemantic(RenderPipeline& pipeline) noexcept
{
	_materialDepthMap = pipeline.getSemantic("DepthMap");
	_materialColorMap = pipeline.getSemantic("ColorMap");
	_materialNormalMap = pipeline.getSemantic("NormalMap");
	_materialDeferredDepthMap = pipeline.getSemantic("DeferredDepthMap");
	_materialDeferredDepthLinearMap = pipeline.getSemantic("DeferredDepthLinearMap");
	_materialDeferredGraphicMap = pipeline.getSemantic("DeferredGraphicMap");
	_materialDeferredNormalMap = pipeline.getSemantic("DeferredNormalMap");
	_materialDeferredLightMap = pipeline.getSemantic("DeferredLightMap");
	_materialDeferredShadowMap = pipeline.getSemantic("DeferredShadowMap");

	return true;
}

bool
DeferredRenderPipeline::setupDeferredMaterials(RenderPipeline& pipeline) noexcept
{
	_deferredLighting = pipeline.createMaterial("sys:fx/deferred_lighting.fxml.o");
	_deferredDepthOnly = _deferredLighting->getTech(RenderQueue::RenderQueueCustom)->getPass("DeferredDepthOnly");
	_deferredDepthLinear = _deferredLighting->getTech(RenderQueue::RenderQueueCustom)->getPass("DeferredDepthLinear");
	_deferredPointLight = _deferredLighting->getTech(RenderQueue::RenderQueueCustom)->getPass("DeferredPointLight");
	_deferredAmbientLight = _deferredLighting->getTech(RenderQueue::RenderQueueCustom)->getPass("DeferredAmbientLight");
	_deferredSunLight = _deferredLighting->getTech(RenderQueue::RenderQueueCustom)->getPass("DeferredSunLight");
	_deferredSunLightShadow = _deferredLighting->getTech(RenderQueue::RenderQueueCustom)->getPass("DeferredSunLightShadow");
	_deferredDirectionalLight = _deferredLighting->getTech(RenderQueue::RenderQueueCustom)->getPass("DeferredDirectionalLight");
	_deferredDirectionalLightShadow = _deferredLighting->getTech(RenderQueue::RenderQueueCustom)->getPass("DeferredDirectionalLightShadow");
	_deferredSpotLight = _deferredLighting->getTech(RenderQueue::RenderQueueCustom)->getPass("DeferredSpotLight");
	_deferredSpotLightShadow = _deferredLighting->getTech(RenderQueue::RenderQueueCustom)->getPass("DeferredSpotLightShadow");
	_deferredShadingOpaques = _deferredLighting->getTech(RenderQueue::RenderQueueCustom)->getPass("DeferredShadingOpaques");
	_deferredShadingTransparents = _deferredLighting->getTech(RenderQueue::RenderQueueCustom)->getPass("DeferredShadingTransparents");
	_deferredCopyOnly = _deferredLighting->getTech(RenderQueue::RenderQueueCustom)->getPass("DeferredCopyOnly");

	_texMRT0 = _deferredLighting->getParameter("texMRT0");
	_texMRT1 = _deferredLighting->getParameter("texMRT1");
	_texDepth = _deferredLighting->getParameter("texDepth");
	_texLight = _deferredLighting->getParameter("texLight");
	_texSource = _deferredLighting->getParameter("texSource");
	_texEnvironmentMap = _deferredLighting->getParameter("texEnvironmentMap");

	_clipInfo = _deferredLighting->getParameter("clipInfo");
	_projInfo = _deferredLighting->getParameter("projInfo");

	_lightColor = _deferredLighting->getParameter("lightColor");
	_lightEyePosition = _deferredLighting->getParameter("lightEyePosition");
	_lightEyeDirection = _deferredLighting->getParameter("lightEyeDirection");
	_lightOuterInner = _deferredLighting->getParameter("lightOuterInner");
	_lightAttenuation = _deferredLighting->getParameter("lightAttenuation");

	_shadowMap = _deferredLighting->getParameter("shadowMap");
	_shadowFactor = _deferredLighting->getParameter("shadowFactor");
	_shadowView2LightView = _deferredLighting->getParameter("shadowView2LightView");
	_shadowView2LightViewProject = _deferredLighting->getParameter("shadowView2LightViewProject");

	return true;
}

bool
DeferredRenderPipeline::setupDeferredRenderTextureLayouts(RenderPipeline& pipeline) noexcept
{
	GraphicsFramebufferLayoutDesc deferredDepthLayoutDesc;
	deferredDepthLayoutDesc.addComponent(GraphicsAttachmentDesc(GraphicsViewLayout::GraphicsViewLayoutDepthStencilAttachmentOptimal, GraphicsFormat::GraphicsFormatD24UNorm_S8UInt, 0));
	_deferredDepthViewLayout = pipeline.createFramebufferLayout(deferredDepthLayoutDesc);
	if (!_deferredDepthViewLayout)
		return false;

	GraphicsFramebufferLayoutDesc deferredDepthLinearLayoutDesc;
	deferredDepthLinearLayoutDesc.addComponent(GraphicsAttachmentDesc(GraphicsViewLayout::GraphicsViewLayoutColorAttachmentOptimal, GraphicsFormat::GraphicsFormatR32SFloat, 0));
	_deferredDepthLinearViewLayout = pipeline.createFramebufferLayout(deferredDepthLinearLayoutDesc);
	if (!_deferredDepthLinearViewLayout)
		return false;

	GraphicsFramebufferLayoutDesc deferredGraphicsLayoutDesc;
	deferredGraphicsLayoutDesc.addComponent(GraphicsAttachmentDesc(GraphicsViewLayout::GraphicsViewLayoutColorAttachmentOptimal, GraphicsFormat::GraphicsFormatR8G8B8A8UNorm, 0));
	_deferredGraphicsViewLayout = pipeline.createFramebufferLayout(deferredGraphicsLayoutDesc);
	if (!_deferredGraphicsViewLayout)
		return false;

	GraphicsFramebufferLayoutDesc deferredNormalLayoutDesc;
	deferredNormalLayoutDesc.addComponent(GraphicsAttachmentDesc(GraphicsViewLayout::GraphicsViewLayoutColorAttachmentOptimal, GraphicsFormat::GraphicsFormatR8G8B8A8UNorm, 0));
	_deferredNormalViewLayout = pipeline.createFramebufferLayout(deferredNormalLayoutDesc);
	if (!_deferredNormalViewLayout)
		return false;

	GraphicsFramebufferLayoutDesc deferredGraphicsViewsLayoutDesc;
	deferredGraphicsViewsLayoutDesc.addComponent(GraphicsAttachmentDesc(GraphicsViewLayout::GraphicsViewLayoutColorAttachmentOptimal, GraphicsFormat::GraphicsFormatR8G8B8A8UNorm, 0));
	deferredGraphicsViewsLayoutDesc.addComponent(GraphicsAttachmentDesc(GraphicsViewLayout::GraphicsViewLayoutColorAttachmentOptimal, GraphicsFormat::GraphicsFormatR8G8B8A8UNorm, 1));
	deferredGraphicsViewsLayoutDesc.addComponent(GraphicsAttachmentDesc(GraphicsViewLayout::GraphicsViewLayoutDepthStencilAttachmentOptimal, GraphicsFormat::GraphicsFormatD24UNorm_S8UInt, 2));
	_deferredGraphicsViewsLayout = pipeline.createFramebufferLayout(deferredGraphicsViewsLayoutDesc);
	if (!_deferredGraphicsViewsLayout)
		return false;

	GraphicsFramebufferLayoutDesc deferredLightingLayoutDesc;
	deferredLightingLayoutDesc.addComponent(GraphicsAttachmentDesc(GraphicsViewLayout::GraphicsViewLayoutColorAttachmentOptimal, GraphicsFormat::GraphicsFormatR16G16B16A16SFloat, 0));
	deferredLightingLayoutDesc.addComponent(GraphicsAttachmentDesc(GraphicsViewLayout::GraphicsViewLayoutDepthStencilReadOnlyOptimal, GraphicsFormat::GraphicsFormatD24UNorm_S8UInt, 1));
	_deferredLightingViewLayout = pipeline.createFramebufferLayout(deferredLightingLayoutDesc);
	if (!_deferredLightingViewLayout)
		return false;

	GraphicsFramebufferLayoutDesc deferredShadingLayoutDesc;
	deferredShadingLayoutDesc.addComponent(GraphicsAttachmentDesc(GraphicsViewLayout::GraphicsViewLayoutColorAttachmentOptimal, GraphicsFormat::GraphicsFormatR8G8B8A8UNorm, 0));
	deferredShadingLayoutDesc.addComponent(GraphicsAttachmentDesc(GraphicsViewLayout::GraphicsViewLayoutDepthStencilReadOnlyOptimal, GraphicsFormat::GraphicsFormatD24UNorm_S8UInt, 1));
	_deferredShadingViewLayout = pipeline.createFramebufferLayout(deferredShadingLayoutDesc);
	if (!_deferredShadingViewLayout)
		return false;

	GraphicsFramebufferLayoutDesc deferredFinalLayoutDesc;
	deferredFinalLayoutDesc.addComponent(GraphicsAttachmentDesc(GraphicsViewLayout::GraphicsViewLayoutColorAttachmentOptimal, GraphicsFormat::GraphicsFormatR8G8B8A8UNorm, 0));
	deferredFinalLayoutDesc.addComponent(GraphicsAttachmentDesc(GraphicsViewLayout::GraphicsViewLayoutDepthStencilReadOnlyOptimal, GraphicsFormat::GraphicsFormatD24UNorm_S8UInt, 1));
	_deferredFinalViewLayout = pipeline.createFramebufferLayout(deferredFinalLayoutDesc);
	if (!_deferredFinalViewLayout)
		return false;

	GraphicsFramebufferLayoutDesc deferredSwapLayoutDesc;
	deferredSwapLayoutDesc.addComponent(GraphicsAttachmentDesc(GraphicsViewLayout::GraphicsViewLayoutColorAttachmentOptimal, GraphicsFormat::GraphicsFormatR8G8B8A8UNorm, 0));
	deferredSwapLayoutDesc.addComponent(GraphicsAttachmentDesc(GraphicsViewLayout::GraphicsViewLayoutDepthStencilReadOnlyOptimal, GraphicsFormat::GraphicsFormatD24UNorm_S8UInt, 1));
	_deferredSwapViewLayout = pipeline.createFramebufferLayout(deferredSwapLayoutDesc);
	if (!_deferredSwapViewLayout)
		return false;

	return true;
}

bool
DeferredRenderPipeline::setupDeferredRenderTextures(RenderPipeline& pipeline) noexcept
{
	std::uint32_t width, height;
	pipeline.getWindowResolution(width, height);

	_deferredDepthMap = pipeline.createTexture(width, height, GraphicsTextureDim::GraphicsTextureDim2D, GraphicsFormat::GraphicsFormatD24UNorm_S8UInt);
	if (!_deferredDepthMap)
		return false;

	_deferredDepthLinearMap = pipeline.createTexture(width, height, GraphicsTextureDim::GraphicsTextureDim2D, GraphicsFormat::GraphicsFormatR32SFloat);
	if (!_deferredDepthLinearMap)
		return false;

	_deferredGraphicsMap = pipeline.createTexture(width, height, GraphicsTextureDim::GraphicsTextureDim2D, GraphicsFormat::GraphicsFormatR8G8B8A8UNorm);
	if (!_deferredGraphicsMap)
		return false;

	_deferredNormalMap = pipeline.createTexture(width, height, GraphicsTextureDim::GraphicsTextureDim2D, GraphicsFormat::GraphicsFormatR8G8B8A8UNorm);
	if (!_deferredNormalMap)
		return false;

	_deferredLightMap = pipeline.createTexture(width, height, GraphicsTextureDim::GraphicsTextureDim2D, GraphicsFormat::GraphicsFormatR16G16B16A16SFloat);
	if (!_deferredLightMap)
		return false;

	_deferredShadingMap = pipeline.createTexture(width, height, GraphicsTextureDim::GraphicsTextureDim2D, GraphicsFormat::GraphicsFormatR8G8B8A8UNorm);
	if (!_deferredShadingMap)
		return false;

	_deferredSwapMap = pipeline.createTexture(width, height, GraphicsTextureDim::GraphicsTextureDim2D, GraphicsFormat::GraphicsFormatR8G8B8A8UNorm);
	if (!_deferredSwapMap)
		return false;

	_deferredFinalMap = pipeline.createTexture(width, height, GraphicsTextureDim::GraphicsTextureDim2D, GraphicsFormat::GraphicsFormatR8G8B8A8UNorm);
	if (!_deferredFinalMap)
		return false;

	GraphicsFramebufferDesc deferredDepthDesc;
	deferredDepthDesc.setWidth(width);
	deferredDepthDesc.setHeight(height);
	deferredDepthDesc.setSharedDepthTexture(_deferredDepthMap);
	deferredDepthDesc.setSharedStencilTexture(_deferredDepthMap);
	deferredDepthDesc.setGraphicsFramebufferLayout(_deferredDepthViewLayout);
	_deferredDepthView = pipeline.createFramebuffer(deferredDepthDesc);
	if (!_deferredDepthView)
		return false;

	GraphicsFramebufferDesc deferredDepthLinearDesc;
	deferredDepthLinearDesc.setWidth(width);
	deferredDepthLinearDesc.setHeight(height);
	deferredDepthLinearDesc.attach(_deferredDepthLinearMap);
	deferredDepthLinearDesc.setGraphicsFramebufferLayout(_deferredDepthLinearViewLayout);
	_deferredDepthLinearView = pipeline.createFramebuffer(deferredDepthLinearDesc);
	if (!_deferredDepthLinearView)
		return false;

	GraphicsFramebufferDesc deferredGraphicsDesc;
	deferredGraphicsDesc.setWidth(width);
	deferredGraphicsDesc.setHeight(height);
	deferredGraphicsDesc.attach(_deferredGraphicsMap);
	deferredGraphicsDesc.setGraphicsFramebufferLayout(_deferredGraphicsViewLayout);
	_deferredGraphicsView = pipeline.createFramebuffer(deferredGraphicsDesc);
	if (!_deferredGraphicsView)
		return false;

	GraphicsFramebufferDesc deferredNormalDesc;
	deferredNormalDesc.setWidth(width);
	deferredNormalDesc.setHeight(height);
	deferredNormalDesc.attach(_deferredNormalMap);
	deferredNormalDesc.setGraphicsFramebufferLayout(_deferredNormalViewLayout);
	_deferredNormalView = pipeline.createFramebuffer(deferredNormalDesc);
	if (!_deferredNormalView)
		return false;

	GraphicsFramebufferDesc deferredGraphicDesc;
	deferredGraphicDesc.setWidth(width);
	deferredGraphicDesc.setHeight(height);
	deferredGraphicDesc.setSharedDepthTexture(_deferredDepthMap);
	deferredGraphicDesc.setSharedStencilTexture(_deferredDepthMap);
	deferredGraphicDesc.attach(_deferredGraphicsMap);
	deferredGraphicDesc.attach(_deferredNormalMap);
	deferredGraphicDesc.setGraphicsFramebufferLayout(_deferredGraphicsViewsLayout);
	_deferredGraphicsViews = pipeline.createFramebuffer(deferredGraphicDesc);
	if (!_deferredGraphicsViews)
		return false;

	GraphicsFramebufferDesc deferredLightingDesc;
	deferredLightingDesc.setWidth(width);
	deferredLightingDesc.setHeight(height);
	deferredLightingDesc.setSharedDepthTexture(_deferredDepthMap);
	deferredLightingDesc.setSharedStencilTexture(_deferredDepthMap);
	deferredLightingDesc.attach(_deferredLightMap);
	deferredLightingDesc.setGraphicsFramebufferLayout(_deferredLightingViewLayout);
	_deferredLightingView = pipeline.createFramebuffer(deferredLightingDesc);
	if (!_deferredLightingView)
		return false;

	GraphicsFramebufferDesc deferredShadingDesc;
	deferredShadingDesc.setWidth(width);
	deferredShadingDesc.setHeight(height);
	deferredShadingDesc.setSharedDepthTexture(_deferredDepthMap);
	deferredShadingDesc.setSharedStencilTexture(_deferredDepthMap);
	deferredShadingDesc.attach(_deferredShadingMap);
	deferredShadingDesc.setGraphicsFramebufferLayout(_deferredShadingViewLayout);
	_deferredShadingView = pipeline.createFramebuffer(deferredShadingDesc);
	if (!_deferredShadingView)
		return false;

	GraphicsFramebufferDesc deferredFinalDesc;
	deferredFinalDesc.setWidth(width);
	deferredFinalDesc.setHeight(height);
	deferredFinalDesc.setSharedDepthTexture(_deferredDepthMap);
	deferredFinalDesc.setSharedStencilTexture(_deferredDepthMap);
	deferredFinalDesc.attach(_deferredFinalMap);
	deferredFinalDesc.setGraphicsFramebufferLayout(_deferredFinalViewLayout);
	_deferredFinalView = pipeline.createFramebuffer(deferredFinalDesc);
	if (!_deferredFinalView)
		return false;

	GraphicsFramebufferDesc deferredSwapDesc;
	deferredSwapDesc.setWidth(width);
	deferredSwapDesc.setHeight(height);
	deferredSwapDesc.setSharedDepthTexture(_deferredDepthMap);
	deferredSwapDesc.setSharedStencilTexture(_deferredDepthMap);
	deferredSwapDesc.attach(_deferredSwapMap);
	deferredSwapDesc.setGraphicsFramebufferLayout(_deferredSwapViewLayout);
	_deferredSwapView = pipeline.createFramebuffer(deferredSwapDesc);
	if (!_deferredSwapView)
		return false;

	return true;
}

bool
DeferredRenderPipeline::setupShadowMaterial(RenderPipeline& pipeline) noexcept
{
	_softBlur = pipeline.createMaterial("sys:fx/blur.fxml.o");
	_softBlurShadowX = _softBlur->getTech(RenderQueue::RenderQueueCustom)->getPass("blurX");
	_softBlurShadowY = _softBlur->getTech(RenderQueue::RenderQueueCustom)->getPass("blurY");
	_softBlurShadowSource = _softBlur->getParameter("texSource");
	_softBlurShadowSourceInv = _softBlur->getParameter("texSourceInv");
	return true;
}

bool 
DeferredRenderPipeline::setupShadowMap(RenderPipeline& pipeline) noexcept
{
	GraphicsFramebufferLayoutDesc shaodwMapLayoutDesc;
	shaodwMapLayoutDesc.addComponent(GraphicsAttachmentDesc(GraphicsViewLayout::GraphicsViewLayoutColorAttachmentOptimal, _shadowFormat, 0));
	_softShaodwViewLayout = pipeline.createFramebufferLayout(shaodwMapLayoutDesc);
	if (!_softShaodwViewLayout)
		return false;

	_softShaodwMapTemp = pipeline.createTexture(_shaodwMapSize.x, _shaodwMapSize.y, GraphicsTextureDim::GraphicsTextureDim2D, _shadowFormat);
	if (!_softShaodwMapTemp)
		return false;

	GraphicsFramebufferDesc shadowViewDesc;
	shadowViewDesc.setWidth(_shaodwMapSize.x);
	shadowViewDesc.setHeight(_shaodwMapSize.y);
	shadowViewDesc.attach(_softShaodwMapTemp);
	shadowViewDesc.setGraphicsFramebufferLayout(_softShaodwViewLayout);
	_softShaodwViewTemp = pipeline.createFramebuffer(shadowViewDesc);
	if (!_deferredSwapView)
		return false;

	return true;
}

void
DeferredRenderPipeline::destroySemantic() noexcept
{
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
DeferredRenderPipeline::destroyDeferredMaterials() noexcept
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
	_texDepth.reset();
	_texLight.reset();
	_texSource.reset();
	_texEnvironmentMap.reset();

	_clipInfo.reset();
	_projInfo.reset();

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
DeferredRenderPipeline::destroyDeferredRenderTextureLayouts() noexcept
{
	_deferredDepthViewLayout.reset();
	_deferredDepthLinearViewLayout.reset();
	_deferredGraphicsViewLayout.reset();
	_deferredNormalViewLayout.reset();
	_deferredLightingViewLayout.reset();
	_deferredShadingViewLayout.reset();
	_deferredSwapViewLayout.reset();
	_deferredFinalViewLayout.reset();
	_deferredGraphicsViewsLayout.reset();
}

void
DeferredRenderPipeline::destroyDeferredRenderTextures() noexcept
{
	_deferredDepthMap.reset();
	_deferredDepthLinearMap.reset();
	_deferredGraphicsMap.reset();
	_deferredNormalMap.reset();
	_deferredLightMap.reset();
	_deferredShadingMap.reset();
	_deferredSwapMap.reset();
	_deferredFinalMap.reset();

	_deferredDepthView.reset();
	_deferredDepthLinearView.reset();
	_deferredGraphicsView.reset();
	_deferredNormalView.reset();
	_deferredLightingView.reset();
	_deferredShadingView.reset();
	_deferredSwapView.reset();
	_deferredFinalView.reset();
	_deferredGraphicsViews.reset();
}

void
DeferredRenderPipeline::destroyShadowMaterial() noexcept
{
	_softBlur.reset();
	_softBlurShadowX.reset();
	_softBlurShadowY.reset();
	_softBlurShadowSource.reset();
	_softBlurShadowSourceInv.reset();
}

void 
DeferredRenderPipeline::destroyShadowMap() noexcept
{
	_softShaodwMapTemp.reset();
	_softShaodwViews.clear();
	_softShaodwViewTemp.reset();
	_softShaodwViewLayout.reset();
}

void
DeferredRenderPipeline::setup(RenderPipeline& pipeline) noexcept
{
	this->setupSemantic(pipeline);
	this->setupDeferredMaterials(pipeline);
	this->setupDeferredRenderTextureLayouts(pipeline);
	this->setupDeferredRenderTextures(pipeline);
	this->setupShadowMaterial(pipeline);
	this->setupShadowMap(pipeline);
}

void
DeferredRenderPipeline::close() noexcept
{
	this->destroySemantic();
	this->destroyDeferredMaterials();
	this->destroyDeferredRenderTextures();
	this->destroyDeferredRenderTextureLayouts();
	this->destroyShadowMap();
	this->destroyShadowMaterial();
}

void 
DeferredRenderPipeline::onResolutionChangeBefore(RenderPipeline& pipeline) noexcept
{
}

void 
DeferredRenderPipeline::onResolutionChangeAfter(RenderPipeline& pipeline) noexcept
{
	destroyDeferredRenderTextures();
	setupDeferredRenderTextures(pipeline);
}

void
DeferredRenderPipeline::onRenderPre(RenderPipeline& pipeline) noexcept
{
}

void
DeferredRenderPipeline::onRenderPipeline(RenderPipeline& pipeline, const CameraPtr& camera) noexcept
{
	if (!camera)
		return;

	if (camera->getCameraOrder() != CameraOrder::CameraOrderMain)
		return;

	pipeline.setCamera(camera);

	if (camera->getCameraType() != CameraType::CameraTypePerspective)
		this->render2DEnvMap(pipeline);
	else
	{
		this->renderShadowMaps(pipeline);

		pipeline.setCamera(camera);
		this->render3DEnvMap(pipeline);
	}
}

void
DeferredRenderPipeline::onRenderPost(RenderPipeline& pipeline) noexcept
{
	auto camera = pipeline.getCamera();
	if (!camera)
		return;

	if (camera->getCameraRender() == CameraRender::CameraRenderScreen)
	{
		auto renderTexture = camera->getFramebuffer();
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
	
	/*if (_deferredGraphicsView)
		pipeline.blitFramebuffer(_deferredGraphicsView, Viewport(0, 0, 1376, 768), 0, Viewport(0, 768 / 2, 1376 / 3, 768));
	if (_deferredNormalView)
		pipeline.blitFramebuffer(_deferredNormalView, Viewport(0, 0, 1376, 768), 0, Viewport(1376 / 3, 768 / 2, 1376 / 3 * 2, 768));
	if (_deferredLightingView)
		pipeline.blitFramebuffer(_deferredLightingView, Viewport(0, 0, 1376, 768), 0, Viewport(1376 / 3 * 2, 768 / 2, 1376, 768));
	if (_deferredShadingView)
		pipeline.blitFramebuffer(_deferredShadingView, Viewport(0, 0, 1376, 768), 0, Viewport(1376 / 3 * 2, 0, 1376, 768 / 2));*/
}

_NAME_END