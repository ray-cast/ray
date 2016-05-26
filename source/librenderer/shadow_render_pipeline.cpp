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
#include "shadow_render_pipeline.h"

#include <ray/render_pipeline.h>
#include <ray/render_object_manager.h>

#include <ray/camera.h>
#include <ray/light.h>
#include <ray/material.h>

#include <ray/graphics_texture.h>
#include <ray/graphics_framebuffer.h>

_NAME_BEGIN

__ImplementSubClass(ShadowRenderPipeline, RenderPipelineController, "ShadowRenderPipeline")

ShadowRenderPipeline::ShadowRenderPipeline() noexcept
	: _shadowFormat(GraphicsFormat::GraphicsFormatR32SFloat)
	, _shadowDepthFormat(GraphicsFormat::GraphicsFormatD16UNorm)
{
	_shadowMapSize[LightShadowType::LightShadowTypeNone] = 0;
	_shadowMapSize[LightShadowType::LightShadowTypeLow] = LightShadowSize::LightShadowSizeLow;
	_shadowMapSize[LightShadowType::LightShadowTypeMedium] = LightShadowSize::LightShadowSizeMedium;
	_shadowMapSize[LightShadowType::LightShadowTypeHigh] = LightShadowSize::LightShadowSizeHigh;
	_shadowMapSize[LightShadowType::LightShadowTypeVeryHigh] = LightShadowSize::LightShadowSizeVeryHigh;
	_shadowMapSizeInv[LightShadowType::LightShadowTypeNone] = 0;
	_shadowMapSizeInv[LightShadowType::LightShadowTypeLow] = 1.0f / LightShadowSize::LightShadowSizeLow;
	_shadowMapSizeInv[LightShadowType::LightShadowTypeMedium] = 1.0f / LightShadowSize::LightShadowSizeMedium;
	_shadowMapSizeInv[LightShadowType::LightShadowTypeHigh] = 1.0f / LightShadowSize::LightShadowSizeHigh;
	_shadowMapSizeInv[LightShadowType::LightShadowTypeVeryHigh] = 1.0f / LightShadowSize::LightShadowSizeVeryHigh;
}

ShadowRenderPipeline::~ShadowRenderPipeline() noexcept
{
	this->close();
}

bool
ShadowRenderPipeline::setup(RenderPipelinePtr pipeline) noexcept
{
	if (!initTextureFormat(*pipeline))
		return false;

	if (!setupShadowMaterial(*pipeline))
		return false;

	if (!setupShadowMap(*pipeline))
		return false;

	_pipeline = pipeline;
	return true;
}

void
ShadowRenderPipeline::close() noexcept
{
	_pipeline.reset();

	this->destroyShadowMap();
	this->destroyShadowMaterial();
}

void
ShadowRenderPipeline::renderShadowMaps(const CameraPtr& camera) noexcept
{
	_pipeline->setCamera(camera);

	const auto& lights = _pipeline->getCamera()->getRenderDataManager()->getRenderData(RenderQueue::RenderQueueLighting);
	for (auto& it : lights)
	{
		auto light = it->downcast<Light>();
		if (light->getShadowType() == LightShadowType::LightShadowTypeNone)
			continue;

		auto lightType = light->getLightType();
		auto lightShadowType = light->getShadowType();
		auto& lightShadowView = light->getShadowCamera()->getFramebuffer();

		_pipeline->setCamera(light->getShadowCamera());
		_pipeline->setFramebuffer(_softShadowDepthViewTemp[lightShadowType]);
		_pipeline->clearFramebuffer(GraphicsClearFlagBits::GraphicsClearFlagDepthBit, float4::Zero, 1.0, 0);
		_pipeline->drawRenderQueue(RenderQueue::RenderQueueShadow, _softGenShadowMap);

		if (light->getSoftShadow())
		{
			_softShadowSource->uniformTexture(_softShadowDepthMapTemp[lightShadowType]);
			_softShadowSourceInv->uniform1f(_shadowMapSizeInv[lightShadowType]);
			_softClipConstant->uniform4f(float4(light->getShadowCamera()->getClipConstant().xy(), 1.0, 1.0));
			_pipeline->setFramebuffer(_softShadowViewTemp[lightShadowType]);

			if (lightType == LightType::LightTypeSun ||
				lightType == LightType::LightTypeDirectional)
			{
				_pipeline->drawScreenQuad(*_softBlurOrthoShadowX);
			}
			else
			{
				_pipeline->drawScreenQuad(*_softBlurPerspectiveFovShadowX);
			}

			_softShadowSource->uniformTexture(_softShadowMapTemp[lightShadowType]);
			_softShadowSourceInv->uniform1f(_shadowMapSizeInv[lightShadowType]);
			_pipeline->setFramebuffer(lightShadowView);
			_pipeline->drawScreenQuad(*_softBlurShadowY);
		}
		else
		{
			if (lightType == LightType::LightTypeSun ||
				lightType == LightType::LightTypeDirectional)
			{
				_softShadowSource->uniformTexture(_softShadowDepthMapTemp[lightShadowType]);
				_softClipConstant->uniform4f(float4(light->getShadowCamera()->getClipConstant().xy(), 1.0f, 1.0f));
				_pipeline->setFramebuffer(lightShadowView);
				_pipeline->drawScreenQuad(*_softConvOrthoLinearDepth);
			}
			else
			{
				_softShadowSource->uniformTexture(_softShadowDepthMapTemp[lightShadowType]);
				_softClipConstant->uniform4f(float4(light->getShadowCamera()->getClipConstant().xy(), 1.0f, 1.0f));
				_pipeline->setFramebuffer(lightShadowView);
				_pipeline->drawScreenQuad(*_softConvPerspectiveFovLinearDepth);
			}
		}
	}
}

bool
ShadowRenderPipeline::initTextureFormat(RenderPipeline& pipeline) noexcept
{
	if (pipeline.isTextureSupport(GraphicsFormat::GraphicsFormatR32SFloat))
		_shadowFormat = GraphicsFormat::GraphicsFormatR32SFloat;
	else if (pipeline.isTextureSupport(GraphicsFormat::GraphicsFormatR16SFloat))
		_shadowFormat = GraphicsFormat::GraphicsFormatR16SFloat;
	else if (pipeline.isTextureSupport(GraphicsFormat::GraphicsFormatR8G8B8A8UNorm))
		_shadowFormat = GraphicsFormat::GraphicsFormatR8G8B8A8UNorm;
	else
		return false;

	if (pipeline.isTextureSupport(GraphicsFormat::GraphicsFormatX8_D24UNormPack32))
		_shadowDepthFormat = GraphicsFormat::GraphicsFormatX8_D24UNormPack32;
	else if (pipeline.isTextureSupport(GraphicsFormat::GraphicsFormatD16UNorm))
		_shadowDepthFormat = GraphicsFormat::GraphicsFormatD16UNorm;
	else
		return false;

	return true;
}

bool
ShadowRenderPipeline::setupShadowMaterial(RenderPipeline& pipeline) noexcept
{
	_softBlur = pipeline.createMaterial("sys:fx/shadowmap.fxml");
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

	const float offsets[4] = { 1.3846153846f, 3.2307692308f, -1.3846153846f, -3.2307692308f };
	const float weights[3] = { 0.2270270270f,  0.3162162162f, 0.0702702703f };

	_softOffset->uniform1fv(4, offsets);
	_softWeight->uniform1fv(3, weights);

	return true;
}

bool
ShadowRenderPipeline::setupShadowMap(RenderPipeline& pipeline) noexcept
{
	GraphicsFramebufferLayoutDesc shaodwMapLayoutDesc;
	shaodwMapLayoutDesc.addComponent(GraphicsAttachment(0, GraphicsImageLayout::GraphicsImageLayoutColorAttachmentOptimal, _shadowFormat));
	_softShadowImageLayout = pipeline.createFramebufferLayout(shaodwMapLayoutDesc);
	if (!_softShadowImageLayout)
		return false;

	GraphicsFramebufferLayoutDesc shadowDephLayoutDesc;
	shadowDephLayoutDesc.addComponent(GraphicsAttachment(0, GraphicsImageLayout::GraphicsImageLayoutDepthStencilAttachmentOptimal, _shadowDepthFormat));
	_softShadowDepthImageLayout = pipeline.createFramebufferLayout(shadowDephLayoutDesc);
	if (!_softShadowDepthImageLayout)
		return false;

	for (std::size_t i = LightShadowType::LightShadowTypeLow; i <= LightShadowType::LightShadowTypeVeryHigh; i++)
	{
		_softShadowMapTemp[i] = pipeline.createTexture(_shadowMapSize[i], _shadowMapSize[i], GraphicsTextureDim::GraphicsTextureDim2D, _shadowFormat);
		if (!_softShadowMapTemp[i])
			return false;

		_softShadowDepthMapTemp[i] = pipeline.createTexture(_shadowMapSize[i], _shadowMapSize[i], GraphicsTextureDim::GraphicsTextureDim2D, _shadowDepthFormat);
		if (!_softShadowDepthMapTemp[i])
			return false;

		GraphicsFramebufferDesc shadowViewDesc;
		shadowViewDesc.setWidth(_shadowMapSize[i]);
		shadowViewDesc.setHeight(_shadowMapSize[i]);
		shadowViewDesc.attach(_softShadowMapTemp[i]);
		shadowViewDesc.setGraphicsFramebufferLayout(_softShadowImageLayout);
		_softShadowViewTemp[i] = pipeline.createFramebuffer(shadowViewDesc);
		if (!_softShadowViewTemp[i])
			return false;

		GraphicsFramebufferDesc shadowDepthViewDesc;
		shadowDepthViewDesc.setWidth(_shadowMapSize[i]);
		shadowDepthViewDesc.setHeight(_shadowMapSize[i]);
		shadowDepthViewDesc.setSharedDepthStencilTexture(_softShadowDepthMapTemp[i]);
		shadowDepthViewDesc.setGraphicsFramebufferLayout(_softShadowDepthImageLayout);
		_softShadowDepthViewTemp[i] = pipeline.createFramebuffer(shadowDepthViewDesc);
		if (!_softShadowDepthViewTemp[i])
			return false;
	}

	return true;
}

void
ShadowRenderPipeline::destroyShadowMaterial() noexcept
{
	_softBlur.reset();
	_softConvOrthoLinearDepth.reset();
	_softConvPerspectiveFovLinearDepth.reset();
	_softShadowSource.reset();
	_softShadowSourceInv.reset();
}

void
ShadowRenderPipeline::destroyShadowMap() noexcept
{
	for (std::size_t i = 0; i < LightShadowType::LightShadowTypeRangeSize; i++)
		_softShadowMapTemp[i].reset();

	for (std::size_t i = 0; i < LightShadowType::LightShadowTypeRangeSize; i++)
		_softShadowViewTemp[i].reset();

	_softShadowImageLayout.reset();
}

void
ShadowRenderPipeline::onResolutionChangeBefore() noexcept
{
}

void
ShadowRenderPipeline::onResolutionChangeAfter() noexcept
{
}

void
ShadowRenderPipeline::onRenderPipeline(const CameraPtr& camera) noexcept
{
	assert(camera);
	assert(camera->getCameraOrder() == CameraOrder::CameraOrder3D);
	this->renderShadowMaps(camera);
}

void
ShadowRenderPipeline::onRenderPre() noexcept
{
}

void
ShadowRenderPipeline::onRenderPost() noexcept
{
}

_NAME_END