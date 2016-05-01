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
#include <ray/material_manager.h>

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
	_clipInfo->uniform2f(camera->getClipConstant().xy());
	_projInfo->uniform4f(camera->getProjConstant());

	_pipeline->setCamera(camera);

	this->renderOpaques(*_pipeline, _deferredGraphicsViews);
	this->renderOpaquesDepthLinear(*_pipeline, _deferredDepthLinearView);
	this->renderLights(*_pipeline, _deferredLightingView);
	this->renderOpaquesShading(*_pipeline, _deferredShadingView);
	this->renderOpaquesSpecificShading(*_pipeline, _deferredShadingView);

	/*if (!pipeline.getRenderData(RenderQueue::RenderQueueTransparent).empty())
	{
		this->renderTransparent(pipeline, _deferredGraphicsViews);
		this->renderLights(pipeline, _deferredLightingView);
		this->renderTransparentShading(pipeline, _deferredShadingView);
		this->renderTransparentSpecificShading(pipeline, _deferredShadingView);
	}*/

	_pipeline->drawPostProcess(RenderQueue::RenderQueuePostprocess, _deferredShadingView, _deferredSwapView);
}

void
DeferredLightingPipeline::renderOpaques(RenderPipeline& pipeline, GraphicsFramebufferPtr target) noexcept
{
	pipeline.setFramebuffer(target);
	pipeline.clearFramebuffer(GraphicsClearFlagBits::GraphicsClearFlagsAll, float4::Zero, 1.0, 0);
	pipeline.drawRenderQueue(RenderQueue::RenderQueueOpaque);
}

void
DeferredLightingPipeline::renderOpaquesDepthLinear(RenderPipeline& pipeline, GraphicsFramebufferPtr target) noexcept
{
	pipeline.setFramebuffer(target);
	pipeline.drawScreenQuad(_deferredDepthLinear);
}

void
DeferredLightingPipeline::renderOpaquesShading(RenderPipeline& pipeline, GraphicsFramebufferPtr target) noexcept
{
	pipeline.setFramebuffer(target);
	pipeline.clearFramebuffer(GraphicsClearFlagBits::GraphicsClearFlagsColor, pipeline.getCamera()->getClearColor(), 1.0, 0);
	pipeline.drawScreenQuad(_deferredShadingOpaques);
	pipeline.drawRenderQueue(RenderQueue::RenderQueueOpaqueShading);
}

void
DeferredLightingPipeline::renderOpaquesSpecificShading(RenderPipeline& pipeline, GraphicsFramebufferPtr target) noexcept
{
	pipeline.setFramebuffer(target);
	pipeline.drawRenderQueue(RenderQueue::RenderQueueOpaqueSpecific);
	pipeline.drawPostProcess(RenderQueue::RenderQueueOpaqueSpecific, target, target);
}

void
DeferredLightingPipeline::renderTransparent(RenderPipeline& pipeline, GraphicsFramebufferPtr renderTexture) noexcept
{
	pipeline.setFramebuffer(renderTexture);
	pipeline.clearFramebuffer(GraphicsClearFlagBits::GraphicsClearFlagsColorStencil, float4::Zero, 1.0, 0);
	pipeline.drawRenderQueue(RenderQueue::RenderQueueTransparent);
}

void
DeferredLightingPipeline::renderTransparentDepthLinear(RenderPipeline& pipeline, GraphicsFramebufferPtr target) noexcept
{
	pipeline.setFramebuffer(target);
	pipeline.clearFramebuffer(GraphicsClearFlagBits::GraphicsClearFlagsColor, float4::Zero, 1.0, 0);
	pipeline.drawScreenQuad(_deferredDepthLinear);
}

void
DeferredLightingPipeline::renderTransparentShading(RenderPipeline& pipeline, GraphicsFramebufferPtr target) noexcept
{
	pipeline.setFramebuffer(target);
	pipeline.drawScreenQuad(_deferredShadingTransparents);
}

void
DeferredLightingPipeline::renderTransparentSpecificShading(RenderPipeline& pipeline, GraphicsFramebufferPtr target) noexcept
{
	pipeline.setFramebuffer(target);
	pipeline.drawRenderQueue(RenderQueue::RenderQueueTransparentSpecific);
	pipeline.drawPostProcess(RenderQueue::RenderQueueTransparentSpecific, target, target);
}

void
DeferredLightingPipeline::renderLights(RenderPipeline& pipeline, GraphicsFramebufferPtr target) noexcept
{
	pipeline.setFramebuffer(target);
	pipeline.clearFramebuffer(GraphicsClearFlagBits::GraphicsClearFlagsColor, float4::Zero, 1.0, 0);

	auto& lights = pipeline.getRenderData(RenderQueue::RenderQueueLighting);
	for (auto& it : lights)
	{
		auto light = std::dynamic_pointer_cast<Light>(it);
		auto lightShadow = light->getShadowMap();

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
}

void
DeferredLightingPipeline::renderSunLight(RenderPipeline& pipeline, const Light& light, GraphicsTexturePtr shadowMap) noexcept
{
	_lightColor->uniform3f(light.getLightColor() * light.getIntensity());
	_lightEyeDirection->uniform3f(light.getForward() * float3x3(pipeline.getCamera()->getView()));
	_lightAttenuation->uniform3f(light.getLightAttenuation());

	if (shadowMap)
	{
		float shadowFactor = _shadowEsmFactor / (light.getShadowCamera()->getFar() - light.getShadowCamera()->getNear());
		float shaodwBias = light.getShadowBias();

		_shadowMap->uniformTexture(shadowMap);
		_shadowFactor->uniform2f(shadowFactor, shaodwBias);
		_shadowView2LightView->uniform4f((pipeline.getCamera()->getViewInverse() * light.getShadowCamera()->getView()).getAxisZ());
		_shadowView2LightViewProject->uniform4fmat(pipeline.getCamera()->getViewInverse() * light.getShadowCamera()->getViewProject());

		pipeline.drawScreenQuadLayer(_deferredSunLightShadow, light.getLayer());
	}
	else
	{
		pipeline.drawScreenQuadLayer(_deferredSunLight, light.getLayer());
	}
}

void
DeferredLightingPipeline::renderDirectionalLight(RenderPipeline& pipeline, const Light& light, GraphicsTexturePtr shadowMap) noexcept
{
	_lightColor->uniform3f(light.getLightColor() * light.getIntensity());
	_lightEyeDirection->uniform3f(light.getForward() * float3x3(pipeline.getCamera()->getView()));
	_lightAttenuation->uniform3f(light.getLightAttenuation());

	if (shadowMap)
	{
		float2 clipConstant = light.getShadowCamera()->getClipConstant().xy();
		float shadowFactor = _shadowEsmFactor / (light.getShadowCamera()->getFar() - light.getShadowCamera()->getNear());
		float shaodwBias = light.getShadowBias();

		_shadowMap->uniformTexture(shadowMap);
		_shadowFactor->uniform2f(shadowFactor, shaodwBias);
		_shadowView2LightView->uniform4f((pipeline.getCamera()->getViewInverse() * light.getShadowCamera()->getView()).getAxisZ());
		_shadowView2LightViewProject->uniform4fmat(pipeline.getCamera()->getViewInverse() * light.getShadowCamera()->getViewProject());

		pipeline.drawScreenQuadLayer(_deferredDirectionalLightShadow, light.getLayer());
	}
	else
	{
		pipeline.drawScreenQuadLayer(_deferredDirectionalLight, light.getLayer());
	}
}

void
DeferredLightingPipeline::renderPointLight(RenderPipeline& pipeline, const Light& light, GraphicsTexturePtr shadowMap) noexcept
{
	_lightColor->uniform3f(light.getLightColor() * light.getIntensity());
	_lightEyePosition->uniform3f(light.getTransform().getTranslate() * pipeline.getCamera()->getView());
	_lightAttenuation->uniform3f(light.getLightAttenuation());

	auto transform = light.getTransform();
	transform.scale(light.getRange());

	pipeline.setTransform(transform);
	pipeline.drawSphere(_deferredPointLight, light.getLayer());
}

void
DeferredLightingPipeline::renderSpotLight(RenderPipeline& pipeline, const Light& light, GraphicsTexturePtr shadowMap) noexcept
{
	_lightColor->uniform3f(light.getLightColor() * light.getIntensity());
	_lightEyePosition->uniform3f(light.getTransform().getTranslate() * pipeline.getCamera()->getView());
	_lightEyeDirection->uniform3f(light.getForward() * float3x3(pipeline.getCamera()->getView()));
	_lightAttenuation->uniform3f(light.getLightAttenuation());
	_lightOuterInner->uniform2f(float2(light.getSpotOuterCone().y, light.getSpotInnerCone().y));

	auto transform = light.getTransform();
	transform.translate(light.getForward() * light.getRange());
	transform.scale(light.getRange());

	pipeline.setTransform(transform);

	if (shadowMap)
	{
		float2 clipConstant = light.getShadowCamera()->getClipConstant().xy();
		float shadowFactor = _shadowEsmFactor / (light.getShadowCamera()->getFar() - light.getShadowCamera()->getNear());
		float shaodwBias = light.getShadowBias();

		_shadowMap->uniformTexture(shadowMap);
		_shadowFactor->uniform2f(shadowFactor, shaodwBias);
		_shadowView2LightView->uniform4f((pipeline.getCamera()->getViewInverse() * light.getShadowCamera()->getView()).getAxisZ());
		_shadowView2LightViewProject->uniform4fmat(pipeline.getCamera()->getViewInverse() * light.getShadowCamera()->getViewProject());

		pipeline.drawCone(_deferredSpotLightShadow, light.getLayer());
	}
	else
	{
		pipeline.drawCone(_deferredSpotLight, light.getLayer());
	}
}

void
DeferredLightingPipeline::renderAmbientLight(RenderPipeline& pipeline, const Light& light, GraphicsTexturePtr shadowMap) noexcept
{
	_lightColor->uniform3f(light.getLightColor() * light.getIntensity());
	_lightEyePosition->uniform3f(light.getTransform().getTranslate());
	_lightEyeDirection->uniform3f(light.getForward() * float3x3(pipeline.getCamera()->getView()));
	_lightAttenuation->uniform3f(light.getLightAttenuation());

	pipeline.drawScreenQuadLayer(_deferredAmbientLight, light.getLayer());
}

void
DeferredLightingPipeline::copyRenderTexture(RenderPipeline& pipeline, GraphicsTexturePtr src, GraphicsFramebufferPtr dst, const Viewport& viewport) noexcept
{
	_texSource->uniformTexture(src);
	pipeline.setFramebuffer(dst);
	pipeline.setViewport(viewport);
	pipeline.drawScreenQuad(_deferredCopyOnly);
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
	else if (pipeline.isTextureSupport(GraphicsFormat::GraphicsFormatR8G8B8A8UNorm))
		_deferredLightFormat = GraphicsFormat::GraphicsFormatR8G8B8A8UNorm;
	else
		return false;

	if (pipeline.isTextureSupport(GraphicsFormat::GraphicsFormatR8G8B8A8UNorm))
		_deferredGraphicsFormat = _deferredNormalFormat = GraphicsFormat::GraphicsFormatR8G8B8A8UNorm;
	else
		return false;

	if (pipeline.isTextureSupport(GraphicsFormat::GraphicsFormatR16G16B16A16SFloat))
		_deferredShadingFormat = GraphicsFormat::GraphicsFormatR16G16B16A16SFloat;
	else if (pipeline.isTextureSupport(GraphicsFormat::GraphicsFormatR8G8B8A8UNorm))
		_deferredShadingFormat = GraphicsFormat::GraphicsFormatR8G8B8A8UNorm;
	else
		return false;

	return true;
}

bool
DeferredLightingPipeline::setupSemantic(RenderPipeline& pipeline) noexcept
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
	_texDepth = _deferredLighting->getParameter("texDepth"); if (!_texDepth) return false;
	_texLight = _deferredLighting->getParameter("texLight"); if (!_texLight) return false;
	_texSource = _deferredLighting->getParameter("texSource"); if (!_texSource) return false;

	_clipInfo = _deferredLighting->getParameter("clipInfo"); if (!_clipInfo) return false;
	_projInfo = _deferredLighting->getParameter("projInfo"); if (!_projInfo) return false;

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

	GraphicsTextureDesc _deferredGraphicsDesc;
	_deferredGraphicsDesc.setWidth(width);
	_deferredGraphicsDesc.setHeight(height);
	_deferredGraphicsDesc.setTexDim(GraphicsTextureDim2D);
	_deferredGraphicsDesc.setTexFormat(_deferredGraphicsFormat);
	_deferredGraphicsDesc.setSamplerFilter(GraphicsSamplerFilter::GraphicsSamplerFilterNearest);
	_deferredGraphicsMap = pipeline.createTexture(_deferredGraphicsDesc);
	if (!_deferredGraphicsMap)
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
	_deferredShadingMap = pipeline.createTexture(_deferredShadingDesc);
	if (!_deferredShadingMap)
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
	deferredDepthLayoutDesc.addComponent(GraphicsAttachmentDesc(GraphicsViewLayout::GraphicsViewLayoutDepthStencilAttachmentOptimal, _deferredDepthFormat, 0));
	_deferredDepthViewLayout = pipeline.createFramebufferLayout(deferredDepthLayoutDesc);
	if (!_deferredDepthViewLayout)
		return false;

	GraphicsFramebufferLayoutDesc deferredDepthLinearLayoutDesc;
	deferredDepthLinearLayoutDesc.addComponent(GraphicsAttachmentDesc(GraphicsViewLayout::GraphicsViewLayoutColorAttachmentOptimal, _deferredDepthLinearFormat, 0));
	_deferredDepthLinearViewLayout = pipeline.createFramebufferLayout(deferredDepthLinearLayoutDesc);
	if (!_deferredDepthLinearViewLayout)
		return false;

	GraphicsFramebufferLayoutDesc deferredGraphicsLayoutDesc;
	deferredGraphicsLayoutDesc.addComponent(GraphicsAttachmentDesc(GraphicsViewLayout::GraphicsViewLayoutColorAttachmentOptimal, _deferredGraphicsFormat, 0));
	_deferredGraphicsViewLayout = pipeline.createFramebufferLayout(deferredGraphicsLayoutDesc);
	if (!_deferredGraphicsViewLayout)
		return false;

	GraphicsFramebufferLayoutDesc deferredNormalLayoutDesc;
	deferredNormalLayoutDesc.addComponent(GraphicsAttachmentDesc(GraphicsViewLayout::GraphicsViewLayoutColorAttachmentOptimal, _deferredNormalFormat, 0));
	_deferredNormalViewLayout = pipeline.createFramebufferLayout(deferredNormalLayoutDesc);
	if (!_deferredNormalViewLayout)
		return false;

	GraphicsFramebufferLayoutDesc deferredGraphicsViewsLayoutDesc;
	deferredGraphicsViewsLayoutDesc.addComponent(GraphicsAttachmentDesc(GraphicsViewLayout::GraphicsViewLayoutColorAttachmentOptimal, _deferredGraphicsFormat, 0));
	deferredGraphicsViewsLayoutDesc.addComponent(GraphicsAttachmentDesc(GraphicsViewLayout::GraphicsViewLayoutColorAttachmentOptimal, _deferredNormalFormat, 1));
	deferredGraphicsViewsLayoutDesc.addComponent(GraphicsAttachmentDesc(GraphicsViewLayout::GraphicsViewLayoutDepthStencilAttachmentOptimal, _deferredDepthFormat, 2));
	_deferredGraphicsViewsLayout = pipeline.createFramebufferLayout(deferredGraphicsViewsLayoutDesc);
	if (!_deferredGraphicsViewsLayout)
		return false;

	GraphicsFramebufferLayoutDesc deferredLightingLayoutDesc;
	deferredLightingLayoutDesc.addComponent(GraphicsAttachmentDesc(GraphicsViewLayout::GraphicsViewLayoutColorAttachmentOptimal, _deferredLightFormat, 0));
	deferredLightingLayoutDesc.addComponent(GraphicsAttachmentDesc(GraphicsViewLayout::GraphicsViewLayoutDepthStencilReadOnlyOptimal, _deferredDepthFormat, 1));
	_deferredLightingViewLayout = pipeline.createFramebufferLayout(deferredLightingLayoutDesc);
	if (!_deferredLightingViewLayout)
		return false;

	GraphicsFramebufferLayoutDesc deferredShadingLayoutDesc;
	deferredShadingLayoutDesc.addComponent(GraphicsAttachmentDesc(GraphicsViewLayout::GraphicsViewLayoutColorAttachmentOptimal, _deferredShadingFormat, 0));
	deferredShadingLayoutDesc.addComponent(GraphicsAttachmentDesc(GraphicsViewLayout::GraphicsViewLayoutDepthStencilReadOnlyOptimal, _deferredDepthFormat, 1));
	_deferredShadingViewLayout = pipeline.createFramebufferLayout(deferredShadingLayoutDesc);
	if (!_deferredShadingViewLayout)
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
	deferredGraphicDesc.setSharedDepthStencilTexture(_deferredDepthMap);
	deferredGraphicDesc.attach(_deferredGraphicsMap);
	deferredGraphicDesc.attach(_deferredNormalMap);
	deferredGraphicDesc.setGraphicsFramebufferLayout(_deferredGraphicsViewsLayout);
	_deferredGraphicsViews = pipeline.createFramebuffer(deferredGraphicDesc);
	if (!_deferredGraphicsViews)
		return false;

	GraphicsFramebufferDesc deferredLightingDesc;
	deferredLightingDesc.setWidth(width);
	deferredLightingDesc.setHeight(height);
	deferredLightingDesc.setSharedDepthStencilTexture(_deferredDepthMap);
	deferredLightingDesc.attach(_deferredLightingMap);
	deferredLightingDesc.setGraphicsFramebufferLayout(_deferredLightingViewLayout);
	_deferredLightingView = pipeline.createFramebuffer(deferredLightingDesc);
	if (!_deferredLightingView)
		return false;

	GraphicsFramebufferDesc deferredShadingDesc;
	deferredShadingDesc.setWidth(width);
	deferredShadingDesc.setHeight(height);
	deferredShadingDesc.setSharedDepthStencilTexture(_deferredDepthMap);
	deferredShadingDesc.attach(_deferredShadingMap);
	deferredShadingDesc.setGraphicsFramebufferLayout(_deferredShadingViewLayout);
	_deferredShadingView = pipeline.createFramebuffer(deferredShadingDesc);
	if (!_deferredShadingView)
		return false;

	GraphicsFramebufferDesc deferredSwapDesc;
	deferredSwapDesc.setWidth(width);
	deferredSwapDesc.setHeight(height);
	deferredSwapDesc.setSharedDepthStencilTexture(_deferredDepthMap);
	deferredSwapDesc.attach(_deferredSwapMap);
	deferredSwapDesc.setGraphicsFramebufferLayout(_deferredShadingViewLayout);
	_deferredSwapView = pipeline.createFramebuffer(deferredSwapDesc);
	if (!_deferredSwapView)
		return false;

	return true;
}

void
DeferredLightingPipeline::destroySemantic() noexcept
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
	_texDepth.reset();
	_texLight.reset();
	_texSource.reset();

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
DeferredLightingPipeline::destroyDeferredRenderTextureLayouts() noexcept
{
	_deferredDepthViewLayout.reset();
	_deferredDepthLinearViewLayout.reset();
	_deferredGraphicsViewLayout.reset();
	_deferredNormalViewLayout.reset();
	_deferredLightingViewLayout.reset();
	_deferredShadingViewLayout.reset();
	_deferredGraphicsViewsLayout.reset();
}

void
DeferredLightingPipeline::destroyDeferredTextures() noexcept
{
	_deferredDepthMap.reset();
	_deferredDepthLinearMap.reset();
	_deferredGraphicsMap.reset();
	_deferredNormalMap.reset();
	_deferredLightingMap.reset();
	_deferredShadingMap.reset();
	_deferredSwapMap.reset();
}

void
DeferredLightingPipeline::destroyDeferredRenderTextures() noexcept
{
	_deferredDepthView.reset();
	_deferredDepthLinearView.reset();
	_deferredGraphicsView.reset();
	_deferredNormalView.reset();
	_deferredLightingView.reset();
	_deferredShadingView.reset();
	_deferredSwapView.reset();
	_deferredGraphicsViews.reset();
}

void
DeferredLightingPipeline::onResolutionChangeBefore() noexcept
{
}

void
DeferredLightingPipeline::onResolutionChangeAfter() noexcept
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
	_materialDeferredGraphicMap->uniformTexture(_deferredGraphicsMap);
	_materialDeferredLightMap->uniformTexture(_deferredLightingMap);

	_materialDepthMap->uniformTexture(_deferredDepthMap);
	_materialColorMap->uniformTexture(_deferredShadingMap);
	_materialNormalMap->uniformTexture(_deferredNormalMap);
}

void
DeferredLightingPipeline::onRenderPipeline(const CameraPtr& camera) noexcept
{
	assert(camera);
	assert(camera->getCameraOrder() == CameraOrder::CameraOrder3D);
	this->render3DEnvMap(camera);
}

void
DeferredLightingPipeline::onRenderPost() noexcept
{
	auto camera = _pipeline->getCamera();
	if (!camera)
		return;

	auto viewport = camera->getViewport();
	if (viewport.width == 0 || viewport.height == 0)
	{
		std::uint32_t width, height;
		_pipeline->getWindowResolution(width, height);

		viewport.left = 0;
		viewport.top = 0;
		viewport.width = width;
		viewport.height = height;
	}

	auto flags = camera->getCameraRenderFlags();
	if (flags & CameraRenderFlagBits::CameraRenderTextureBit)
	{
		auto framebuffer = camera->getFramebuffer();
		if (framebuffer)
		{
			if (flags & CameraRenderFlagBits::CameraRenderGbufferDiffuseBit)
				this->copyRenderTexture(*_pipeline, _deferredGraphicsMap, framebuffer, viewport);
			else if (flags & CameraRenderFlagBits::CameraRenderGbufferNormalBit)
				this->copyRenderTexture(*_pipeline, _deferredNormalMap, framebuffer, viewport);
			else if (flags & CameraRenderFlagBits::CameraRenderLightingBit)
				this->copyRenderTexture(*_pipeline, _deferredLightingMap, framebuffer, viewport);
			else if (flags & CameraRenderFlagBits::CameraRenderShadingBit)
				this->copyRenderTexture(*_pipeline, _deferredShadingMap, framebuffer, viewport);
		}
	}

	if (flags & CameraRenderFlagBits::CameraRenderScreenBit)
	{
		if (flags & CameraRenderFlagBits::CameraRenderGbufferDiffuseBit)
			this->copyRenderTexture(*_pipeline, _deferredGraphicsMap, nullptr, viewport);
		else if (flags & CameraRenderFlagBits::CameraRenderGbufferNormalBit)
			this->copyRenderTexture(*_pipeline, _deferredNormalMap, nullptr, viewport);
		else if (flags & CameraRenderFlagBits::CameraRenderLightingBit)
			this->copyRenderTexture(*_pipeline, _deferredLightingMap, nullptr, viewport);
		else if (flags & CameraRenderFlagBits::CameraRenderShadingBit)
			this->copyRenderTexture(*_pipeline, _deferredShadingMap, nullptr, viewport);
	}
}

_NAME_END