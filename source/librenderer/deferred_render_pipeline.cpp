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

_NAME_BEGIN

DeferredRenderPipeline::DeferredRenderPipeline() noexcept
	: _shadowEsmFactor(300.0f)
	, _shadowMapSize(512)
	, _shadowFormat(GraphicsFormat::GraphicsFormatR32SFloat)
	, _shadowDepthFormat(GraphicsFormat::GraphicsFormatD16UNorm)
{
}

DeferredRenderPipeline::~DeferredRenderPipeline() noexcept
{
	this->close();
}

bool
DeferredRenderPipeline::setup(RenderPipeline& pipeline) noexcept
{
	if (!this->initTextureFormat(pipeline))
		return false;

	if (!this->setupSemantic(pipeline))
		return false;

	if (!this->setupDeferredMaterials(pipeline))
		return false;

	if (!this->setupDeferredTextures(pipeline))
		return false;

	if (!this->setupDeferredRenderTextureLayouts(pipeline))
		return false;

	if (!this->setupDeferredRenderTextures(pipeline))
		return false;

	if (!this->setupShadowMaterial(pipeline))
		return false;

	if (!this->setupShadowMap(pipeline))
		return false;

	return true;
}

void
DeferredRenderPipeline::close() noexcept
{
	this->destroySemantic();
	this->destroyDeferredMaterials();
	this->destroyDeferredRenderTextures();
	this->destroyDeferredRenderTextureLayouts();
	this->destroyDeferredTextures();
	this->destroyShadowMap();
	this->destroyShadowMaterial();
}

void
DeferredRenderPipeline::renderShadowMaps(RenderPipeline& pipeline) noexcept
{
	const auto lights = pipeline.getRenderData(RenderQueue::RenderQueueShadow);

	std::size_t numLight = lights.size();
	if (_softShadowViews.size() < numLight)
	{
		auto num = numLight - _softShadowViews.size();
		for (std::size_t i = 0; i < num; i++)
		{
			GraphicsTextureDesc textureDesc;
			textureDesc.setWidth(_shadowMapSize);
			textureDesc.setHeight(_shadowMapSize);
			textureDesc.setTexDim(GraphicsTextureDim::GraphicsTextureDim2D);
			textureDesc.setTexFormat(_shadowFormat);
			auto blurShadowMap = pipeline.createTexture(textureDesc);
			if (!blurShadowMap)
				return;

			GraphicsFramebufferDesc shadowViewDesc;
			shadowViewDesc.setWidth(_shadowMapSize);
			shadowViewDesc.setHeight(_shadowMapSize);
			shadowViewDesc.attach(blurShadowMap);
			shadowViewDesc.setGraphicsFramebufferLayout(_softShadowViewLayout);
			auto blurShadowView = pipeline.createFramebuffer(shadowViewDesc);
			if (!blurShadowView)
				return;

			_softShadowMaps.push_back(blurShadowMap);
			_softShadowViews.push_back(blurShadowView);
		}
	}

	for (std::size_t i = 0; i < numLight; i++)
	{
		auto light = lights[i]->downcast<Light>();
		auto lightType = light->getLightType();

		pipeline.setCamera(light->getShadowCamera());
		pipeline.setFramebuffer(_softShadowDepthViewTemp);
		pipeline.clearFramebuffer(GraphicsClearFlags::GraphicsClearFlagsDepth, float4::Zero, 1.0, 0);
		pipeline.drawRenderQueue(RenderQueue::RenderQueueOpaque, _softGenShadowMap);

		if (light->getSoftShadow())
		{
			_softShadowSource->uniformTexture(_softShadowDepthMapTemp);
			_softShadowSourceInv->uniform1f(1.0f / _shadowMapSize);
			_softClipConstant->uniform4f(float4(light->getShadowCamera()->getClipConstant().xy(), 1.0, 1.0));
			pipeline.setFramebuffer(_softShadowViewTemp);

			if (lightType == LightType::LightTypeSun ||
				lightType == LightType::LightTypeDirectional)
			{
				pipeline.drawScreenQuad(_softBlurOrthoShadowX);
			}
			else
			{
				pipeline.drawScreenQuad(_softBlurPerspectiveFovShadowX);
			}

			_softShadowSource->uniformTexture(_softShadowMapTemp);
			_softShadowSourceInv->uniform1f(1.0f / _shadowMapSize);
			pipeline.setFramebuffer(_softShadowViews[i]);
			pipeline.drawScreenQuad(_softBlurShadowY);
		}
		else
		{
			if (lightType == LightType::LightTypeSun ||
				lightType == LightType::LightTypeDirectional)
			{
				_softShadowSource->uniformTexture(_softShadowDepthMapTemp);
				_softClipConstant->uniform4f(float4(light->getShadowCamera()->getClipConstant().xy(), 1.0, 1.0));
				pipeline.setFramebuffer(_softShadowViews[i]);
				pipeline.drawScreenQuad(_softConvOrthoLinearDepth);
			}
			else
			{
				_softShadowSource->uniformTexture(_softShadowDepthMapTemp);
				_softClipConstant->uniform4f(float4(light->getShadowCamera()->getClipConstant().xy(), 1.0, 1.0));
				pipeline.setFramebuffer(_softShadowViews[i]);
				pipeline.drawScreenQuad(_softConvPerspectiveFovLinearDepth);
			}
		}
	}
}

void
DeferredRenderPipeline::render2DEnvMap(RenderPipeline& pipeline) noexcept
{
	pipeline.setFramebuffer(_deferredShadingView);
	pipeline.clearFramebuffer(GraphicsClearFlags::GraphicsClearFlagsAll, pipeline.getCamera()->getClearColor(), 1.0, 0);
	pipeline.drawRenderQueue(RenderQueue::RenderQueueOpaque);
}

void
DeferredRenderPipeline::render3DEnvMap(RenderPipeline& pipeline) noexcept
{
	_materialDeferredDepthMap->uniformTexture(_deferredDepthMap);
	_materialDeferredDepthLinearMap->uniformTexture(_deferredDepthLinearMap);
	_materialDeferredNormalMap->uniformTexture(_deferredNormalMap);
	_materialDeferredGraphicMap->uniformTexture(_deferredGraphicsMap);
	_materialDeferredLightMap->uniformTexture(_deferredLightMap);

	_materialDepthMap->uniformTexture(_deferredDepthMap);
	_materialColorMap->uniformTexture(_deferredShadingMap);
	_materialNormalMap->uniformTexture(_deferredNormalMap);

	_clipInfo->uniform2f(pipeline.getCamera()->getClipConstant().xy());
	_projInfo->uniform4f(pipeline.getCamera()->getProjConstant());

	this->renderOpaques(pipeline, _deferredGraphicsViews);
	this->renderOpaquesDepthLinear(pipeline, _deferredDepthLinearView);
	this->renderLights(pipeline, _deferredLightingView);
	this->renderOpaquesShading(pipeline, _deferredShadingView);
	this->renderOpaquesSpecificShading(pipeline, _deferredShadingView);

	/*if (!pipeline.getRenderData(RenderQueue::RenderQueueTransparent).empty())
	{
		this->renderTransparent(pipeline, _deferredGraphicsViews);
		this->renderLights(pipeline, _deferredLightingView);
		this->renderTransparentShading(pipeline, _deferredShadingView);
		this->renderTransparentSpecificShading(pipeline, _deferredShadingView);
	}*/

	pipeline.drawPostProcess(RenderQueue::RenderQueuePostprocess, _deferredShadingView, _deferredSwapView);
}

void
DeferredRenderPipeline::renderOpaques(RenderPipeline& pipeline, GraphicsFramebufferPtr target) noexcept
{
	pipeline.setFramebuffer(target);
	pipeline.clearFramebuffer(GraphicsClearFlags::GraphicsClearFlagsAll, float4::Zero, 1.0, 0);
	pipeline.drawRenderQueue(RenderQueue::RenderQueueOpaque);
}

void
DeferredRenderPipeline::renderOpaquesDepthLinear(RenderPipeline& pipeline, GraphicsFramebufferPtr target) noexcept
{
	pipeline.setFramebuffer(target);
	pipeline.drawScreenQuad(_deferredDepthLinear);
}

void
DeferredRenderPipeline::renderOpaquesShading(RenderPipeline& pipeline, GraphicsFramebufferPtr target) noexcept
{
	pipeline.setFramebuffer(target);
	pipeline.clearFramebuffer(GraphicsClearFlags::GraphicsClearFlagsColor, float4::One, 1.0, 0);
	pipeline.drawScreenQuad(_deferredShadingOpaques);
}

void
DeferredRenderPipeline::renderOpaquesSpecificShading(RenderPipeline& pipeline, GraphicsFramebufferPtr target) noexcept
{
	pipeline.setFramebuffer(target);
	pipeline.drawRenderQueue(RenderQueue::RenderQueueOpaqueSpecific);
	pipeline.drawPostProcess(RenderQueue::RenderQueueOpaqueSpecific, target, target);
}

void
DeferredRenderPipeline::renderTransparent(RenderPipeline& pipeline, GraphicsFramebufferPtr renderTexture) noexcept
{
	pipeline.setFramebuffer(renderTexture);
	pipeline.clearFramebuffer(GraphicsClearFlags::GraphicsClearFlagsColorStencil, float4::Zero, 1.0, 0);
	pipeline.drawRenderQueue(RenderQueue::RenderQueueTransparent);
}

void
DeferredRenderPipeline::renderTransparentDepthLinear(RenderPipeline& pipeline, GraphicsFramebufferPtr target) noexcept
{
	pipeline.setFramebuffer(target);
	pipeline.clearFramebuffer(GraphicsClearFlags::GraphicsClearFlagsColor, float4::Zero, 1.0, 0);
	pipeline.drawScreenQuad(_deferredDepthLinear);
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
	pipeline.drawRenderQueue(RenderQueue::RenderQueueTransparentSpecific);
	pipeline.drawRenderQueue(RenderQueue::RenderQueueTransparentPostprocess);
}

void
DeferredRenderPipeline::renderLights(RenderPipeline& pipeline, GraphicsFramebufferPtr target) noexcept
{
	pipeline.setFramebuffer(target);
	pipeline.clearFramebuffer(GraphicsClearFlags::GraphicsClearFlagsColor, float4::Zero, 1.0, 0);

	std::size_t shadowIndex = 0;

	auto& lights = pipeline.getRenderData(RenderQueue::RenderQueueLighting);
	for (auto& it : lights)
	{
		auto light = std::dynamic_pointer_cast<Light>(it);
		GraphicsTexturePtr lightShadow = nullptr;

		if (light->getShadow() && shadowIndex < _softShadowMaps.size())
		{
			lightShadow = _softShadowMaps[shadowIndex++];
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
}

void
DeferredRenderPipeline::renderSunLight(RenderPipeline& pipeline, const Light& light, GraphicsTexturePtr shadowMap) noexcept
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
		_shadowFactor->uniform4f(float4(clipConstant, shadowFactor, shaodwBias));
		_shadowView2LightView->uniform4f((pipeline.getCamera()->getViewInverse() * light.getShadowCamera()->getView()).getAxisZ());
		_shadowView2LightViewProject->uniform4fmat(pipeline.getCamera()->getViewInverse() * light.getShadowCamera()->getViewProject());

		pipeline.drawScreenQuad(_deferredSunLightShadow, light.getLayer());
	}
	else
	{
		pipeline.drawScreenQuad(_deferredSunLight, light.getLayer());
	}
}

void
DeferredRenderPipeline::renderDirectionalLight(RenderPipeline& pipeline, const Light& light, GraphicsTexturePtr shadowMap) noexcept
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
		_shadowFactor->uniform4f(float4(clipConstant, shadowFactor, shaodwBias));
		_shadowView2LightView->uniform4f((pipeline.getCamera()->getViewInverse() * light.getShadowCamera()->getView()).getAxisZ());
		_shadowView2LightViewProject->uniform4fmat(pipeline.getCamera()->getViewInverse() * light.getShadowCamera()->getViewProject());

		pipeline.drawScreenQuad(_deferredDirectionalLightShadow, light.getLayer());
	}
	else
	{
		pipeline.drawScreenQuad(_deferredDirectionalLight, light.getLayer());
	}
}

void
DeferredRenderPipeline::renderPointLight(RenderPipeline& pipeline, const Light& light, GraphicsTexturePtr shadowMap) noexcept
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
DeferredRenderPipeline::renderSpotLight(RenderPipeline& pipeline, const Light& light, GraphicsTexturePtr shadowMap) noexcept
{
	_lightColor->uniform3f(light.getLightColor() * light.getIntensity());
	_lightEyePosition->uniform3f(light.getTransform().getTranslate() * pipeline.getCamera()->getView());
	_lightEyeDirection->uniform3f(light.getForward() * float3x3(pipeline.getCamera()->getView()));
	_lightAttenuation->uniform3f(light.getLightAttenuation());
	_lightOuterInner->uniform2f(float2(light.getSpotCosOuterCone(), light.getSpotCosInnerCone()));
	
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
		_shadowFactor->uniform4f(float4(clipConstant, shadowFactor, shaodwBias));
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
DeferredRenderPipeline::renderAmbientLight(RenderPipeline& pipeline, const Light& light, GraphicsTexturePtr shadowMap) noexcept
{
	_lightColor->uniform3f(light.getLightColor() * light.getIntensity());
	_lightEyePosition->uniform3f(light.getTransform().getTranslate());
	_lightEyeDirection->uniform3f(light.getForward() * float3x3(pipeline.getCamera()->getView()));
	_lightAttenuation->uniform3f(light.getLightAttenuation());
	
	pipeline.drawScreenQuad(_deferredAmbientLight, light.getLayer());
}

void
DeferredRenderPipeline::copyRenderTexture(RenderPipeline& pipeline, GraphicsTexturePtr src, GraphicsFramebufferPtr dst, const Viewport& viewport) noexcept
{
	_texSource->uniformTexture(src);
	pipeline.setFramebuffer(dst);
	pipeline.setViewport(viewport);
	pipeline.drawScreenQuad(_deferredCopyOnly);
}

bool
DeferredRenderPipeline::initTextureFormat(RenderPipeline& pipeline) noexcept
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
	{
		_deferredGraphicsFormat = GraphicsFormat::GraphicsFormatR8G8B8A8UNorm;
		_deferredNormalFormat = GraphicsFormat::GraphicsFormatR8G8B8A8UNorm;
		_deferredShadingFormat = GraphicsFormat::GraphicsFormatR8G8B8A8UNorm;
	}
	else if (pipeline.isTextureSupport(GraphicsFormat::GraphicsFormatR5G6B5UNormPack16))
	{
		_deferredGraphicsFormat = GraphicsFormat::GraphicsFormatR8G8B8A8UNorm;
		_deferredNormalFormat = GraphicsFormat::GraphicsFormatR8G8B8A8UNorm;
		_deferredShadingFormat = GraphicsFormat::GraphicsFormatR8G8B8A8UNorm;
	}
	else
	{
		return false;
	}

	if (pipeline.isTextureSupport(GraphicsFormat::GraphicsFormatR32SFloat))
		_shadowFormat = GraphicsFormat::GraphicsFormatR32SFloat;
	else if (pipeline.isTextureSupport(GraphicsFormat::GraphicsFormatR16SFloat))
		_shadowFormat = GraphicsFormat::GraphicsFormatR16SFloat;
	else if (pipeline.isTextureSupport(GraphicsFormat::GraphicsFormatR8G8B8A8UNorm))
		_shadowFormat = GraphicsFormat::GraphicsFormatR8G8B8A8UNorm;
	else
		return false;

	return true;
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
	_deferredLighting = pipeline.createMaterial("sys:fx/deferred_lighting.fxml.o"); if (!_deferredLighting) return false;
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
	_texEnvironmentMap = _deferredLighting->getParameter("texEnvironmentMap"); if (!_texEnvironmentMap) return false;

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
DeferredRenderPipeline::setupDeferredTextures(RenderPipeline& pipeline) noexcept
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
	_deferredLightMap = pipeline.createTexture(_deferredLightDesc);
	if (!_deferredLightMap)
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
DeferredRenderPipeline::setupDeferredRenderTextureLayouts(RenderPipeline& pipeline) noexcept
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
DeferredRenderPipeline::setupDeferredRenderTextures(RenderPipeline& pipeline) noexcept
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
	deferredLightingDesc.attach(_deferredLightMap);
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

bool
DeferredRenderPipeline::setupShadowMaterial(RenderPipeline& pipeline) noexcept
{
	_softBlur = pipeline.createMaterial("sys:fx/shadowmap.fxml.o");
	_softGenShadowMap = _softBlur->getTech("GenShadowMap");
	_softConvOrthoLinearDepth = _softBlur->getTech("ConvOrthoLinearDepth");
	_softConvPerspectiveFovLinearDepth = _softBlur->getTech("ConvPerspectiveFovLinearDepth");
	_softBlurOrthoShadowX = _softBlur->getTech("ConvOrthoLinearDepthBlurX");
	_softBlurPerspectiveFovShadowX = _softBlur->getTech("ConvPerspectiveFovLinearDepthBlurX");
	_softBlurShadowY = _softBlur->getTech("BlurY");
	_softLogBlurShadowX = _softBlur->getTech("LogBlurX");
	_softLogBlurShadowY = _softBlur->getTech("LogBlurY");
	_softShadowSource = _softBlur->getParameter("texSource");
	_softShadowSourceInv = _softBlur->getParameter("texSourceSizeInv");
	_softClipConstant = _softBlur->getParameter("clipConstant");
	_softOffset = _softBlur->getParameter("offset");
	_softWeight = _softBlur->getParameter("weight");

	const float offsets[4] = { 1.3846153846, 3.2307692308, -1.3846153846, -3.2307692308 };
	const float weight[3] = { 0.2270270270,  0.3162162162, 0.0702702703 };

	_softOffset->uniform1fv(4, offsets);
	_softWeight->uniform1fv(3, weight);

	return true;
}

bool 
DeferredRenderPipeline::setupShadowMap(RenderPipeline& pipeline) noexcept
{
	GraphicsFramebufferLayoutDesc shaodwMapLayoutDesc;
	shaodwMapLayoutDesc.addComponent(GraphicsAttachmentDesc(GraphicsViewLayout::GraphicsViewLayoutColorAttachmentOptimal, _shadowFormat, 0));
	_softShadowViewLayout = pipeline.createFramebufferLayout(shaodwMapLayoutDesc);
	if (!_softShadowViewLayout)
		return false;

	GraphicsFramebufferLayoutDesc shadowDephLayoutDesc;
	shadowDephLayoutDesc.addComponent(GraphicsAttachmentDesc(GraphicsViewLayout::GraphicsViewLayoutDepthStencilAttachmentOptimal, _shadowDepthFormat, 0));
	_softShadowDepthViewLayout = pipeline.createFramebufferLayout(shadowDephLayoutDesc);
	if (!_softShadowDepthViewLayout)
		return false;

	_softShadowMapTemp = pipeline.createTexture(_shadowMapSize, _shadowMapSize, GraphicsTextureDim::GraphicsTextureDim2D, _shadowFormat);
	if (!_softShadowMapTemp)
		return false;

	_softShadowDepthMapTemp = pipeline.createTexture(_shadowMapSize, _shadowMapSize, GraphicsTextureDim::GraphicsTextureDim2D, _shadowDepthFormat);
	if (!_softShadowDepthMapTemp)
		return false;

	GraphicsFramebufferDesc shadowViewDesc;
	shadowViewDesc.setWidth(_shadowMapSize);
	shadowViewDesc.setHeight(_shadowMapSize);
	shadowViewDesc.attach(_softShadowMapTemp);
	shadowViewDesc.setGraphicsFramebufferLayout(_softShadowViewLayout);
	_softShadowViewTemp = pipeline.createFramebuffer(shadowViewDesc);
	if (!_deferredSwapView)
		return false;

	GraphicsFramebufferDesc shadowDepthViewDesc;
	shadowDepthViewDesc.setWidth(_shadowMapSize);
	shadowDepthViewDesc.setHeight(_shadowMapSize);
	shadowDepthViewDesc.setSharedDepthStencilTexture(_softShadowDepthMapTemp);
	shadowDepthViewDesc.setGraphicsFramebufferLayout(_softShadowDepthViewLayout);
	_softShadowDepthViewTemp = pipeline.createFramebuffer(shadowDepthViewDesc);
	if (!_softShadowDepthViewTemp)
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
	_deferredGraphicsViewsLayout.reset();
}

void
DeferredRenderPipeline::destroyDeferredTextures() noexcept
{
	_deferredDepthMap.reset();
	_deferredDepthLinearMap.reset();
	_deferredGraphicsMap.reset();
	_deferredNormalMap.reset();
	_deferredLightMap.reset();
	_deferredShadingMap.reset();
	_deferredSwapMap.reset();
}

void
DeferredRenderPipeline::destroyDeferredRenderTextures() noexcept
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
DeferredRenderPipeline::destroyShadowMaterial() noexcept
{
	_softBlur.reset();
	_softConvOrthoLinearDepth.reset();
	_softConvPerspectiveFovLinearDepth.reset();
	_softShadowSource.reset();
	_softShadowSourceInv.reset();
}

void 
DeferredRenderPipeline::destroyShadowMap() noexcept
{
	_softShadowMapTemp.reset();
	_softShadowViews.clear();
	_softShadowViewTemp.reset();
	_softShadowViewLayout.reset();
}

void 
DeferredRenderPipeline::onResolutionChangeBefore(RenderPipeline& pipeline) noexcept
{
}

void 
DeferredRenderPipeline::onResolutionChangeAfter(RenderPipeline& pipeline) noexcept
{
	destroyDeferredTextures();
	destroyDeferredRenderTextures();
	setupDeferredTextures(pipeline);
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

	pipeline.setCamera(camera);

	CameraOrder order = camera->getCameraOrder();
	if (order == CameraOrder::CameraOrder2D)
	{
		this->render2DEnvMap(pipeline);
	}
	else if (order == CameraOrder::CameraOrder3D)
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
		auto viewport = camera->getViewport();
		if (viewport.width == 0 || viewport.height == 0)
		{
			std::uint32_t width, height;
			pipeline.getWindowResolution(width, height);

			viewport.left = 0;
			viewport.top = 0;
			viewport.width = width;
			viewport.height = height;
		}

		auto framebuffer = camera->getFramebuffer();
		if (framebuffer)
			this->copyRenderTexture(pipeline, _deferredShadingMap, framebuffer, viewport);

		this->copyRenderTexture(pipeline, _deferredShadingMap, nullptr, viewport);
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