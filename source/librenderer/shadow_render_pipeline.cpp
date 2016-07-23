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
	: _shadowMode(ShadowMode::ShadowModeSoft)
	, _shadowQuality(ShadowQuality::ShadowQualityMedium)
	, _shadowDepthFormat(GraphicsFormat::GraphicsFormatX8_D24UNormPack32)
	, _shadowDepthLinearFormat(GraphicsFormat::GraphicsFormatR32SFloat)
{
}

ShadowRenderPipeline::~ShadowRenderPipeline() noexcept
{
	this->close();
}

bool
ShadowRenderPipeline::setup(RenderPipelinePtr pipeline) noexcept
{
	if (_shadowQuality != ShadowQuality::ShadowQualityNone)
	{
		if (!setupShadowMaterial(*pipeline))
			return false;

		if (!setupShadowMaps(*pipeline))
			return false;

		if (_shadowMode == ShadowMode::ShadowModeSoft)
		{
			if (!setupShadowSoftMaps(*pipeline))
				return false;
		}
	}

	_pipeline = pipeline;
	return true;
}

void
ShadowRenderPipeline::close() noexcept
{
	this->destroyShadowMaps();
	this->destroyShadowMaterial();
	_pipeline.reset();
}

void
ShadowRenderPipeline::setShadowMode(ShadowMode mode) noexcept
{
	_shadowMode = mode;
}

ShadowMode 
ShadowRenderPipeline::getShadowMode() const noexcept
{
	return _shadowMode;
}

void
ShadowRenderPipeline::setShadowQuality(ShadowQuality quality) noexcept
{
	_shadowQuality = quality;
}

ShadowQuality 
ShadowRenderPipeline::getShadowQuality() const noexcept
{
	return _shadowQuality;
}

void
ShadowRenderPipeline::renderShadowMaps(const CameraPtr& mainCamera) noexcept
{
	_pipeline->setCamera(mainCamera);

	const auto& lights = _pipeline->getCamera()->getRenderDataManager()->getRenderData(RenderQueue::RenderQueueLighting);
	for (auto& it : lights)
	{
		auto light = it->downcast<Light>();
		if (light->getShadowMode() == ShadowMode::ShadowModeNone)
			continue;

		if (!light->getGlobalIllumination())
			this->renderShadowMap(*light, RenderQueue::RenderQueueShadow);
		else
			this->renderShadowMap(*light, RenderQueue::RenderQueueReflectiveShadow);
	}
}

void
ShadowRenderPipeline::renderShadowMap(const Light& light, RenderQueue queue) noexcept
{
	auto& cameras = light.getCameras();
	for (auto& camera : cameras)
	{
		auto& depthFrambuffer = camera->getFramebuffer();
		auto& depthLienarFrambuffer = camera->getDepthLinearFramebuffer();

		auto shadowFrambuffer = depthFrambuffer ? depthFrambuffer : _shadowShadowDepthViewTemp;
		auto shadowTexture = shadowFrambuffer->getGraphicsFramebufferDesc().getDepthStencilAttachment().getBindingTexture();

		_pipeline->setCamera(camera);
		_pipeline->setFramebuffer(shadowFrambuffer);

		if (queue == RenderQueue::RenderQueueReflectiveShadow)
		{
			_pipeline->clearFramebuffer(0, GraphicsClearFlagBits::GraphicsClearFlagColorBit, float4::Zero, 1.0, 0);
			_pipeline->clearFramebuffer(1, GraphicsClearFlagBits::GraphicsClearFlagColorBit, float4::Zero, 1.0, 0);
			_pipeline->clearFramebuffer(2, GraphicsClearFlagBits::GraphicsClearFlagDepthBit, float4::Zero, 1.0, 0);
		}
		else
		{
			_pipeline->clearFramebuffer(0, GraphicsClearFlagBits::GraphicsClearFlagDepthBit, float4::Zero, 1.0, 0);
		}

		_pipeline->drawRenderQueue(queue, nullptr);

		if (_shadowMode == ShadowMode::ShadowModeSoft && light.getShadowMode() == ShadowMode::ShadowModeSoft)
		{
			_shadowShadowSource->uniformTexture(shadowTexture);
			_shadowClipConstant->uniform4f(float4(camera->getClipConstant().xy(), 1.0, 1.0));

			_pipeline->setFramebuffer(_shadowShadowDepthLinearViewTemp);
			_pipeline->discardFramebuffer(0);
			_pipeline->drawScreenQuad(*_shadowBlurShadowX[light.getLightType()]);

			_shadowShadowSource->uniformTexture(_shadowShadowDepthLinearMapTemp);

			_pipeline->setFramebuffer(depthLienarFrambuffer);
			_pipeline->discardFramebuffer(0);
			_pipeline->drawScreenQuad(*_shadowBlurShadowY);
		}
		else
		{
			_shadowShadowSource->uniformTexture(shadowTexture);
			_shadowClipConstant->uniform4f(float4(camera->getClipConstant().xy(), 1.0f, 1.0f));

			_pipeline->setFramebuffer(depthLienarFrambuffer);
			_pipeline->discardFramebuffer(0);
			_pipeline->drawScreenQuad(*_shadowBlurShadowX[light.getLightType()]);
		}
	}
}

bool
ShadowRenderPipeline::setupShadowMaterial(RenderPipeline& pipeline) noexcept
{
	_shadowRender = pipeline.createMaterial("sys:fx/shadowmap.fxml"); if (!_shadowRender) return false;
	_shadowConvOrthoLinearDepth = _shadowRender->getTech("ConvOrthoLinearDepth"); if (!_shadowConvOrthoLinearDepth) return false;
	_shadowConvPerspectiveFovLinearDepth = _shadowRender->getTech("ConvPerspectiveFovLinearDepth"); if (!_shadowConvPerspectiveFovLinearDepth) return false;
	_shadowBlurOrthoShadowX = _shadowRender->getTech("ConvOrthoLinearDepthBlurX"); if (!_shadowBlurOrthoShadowX) return false;
	_shadowBlurPerspectiveFovShadowX = _shadowRender->getTech("ConvPerspectiveFovLinearDepthBlurX"); if (!_shadowBlurPerspectiveFovShadowX) return false;
	_shadowBlurShadowY = _shadowRender->getTech("BlurY"); if (!_shadowBlurShadowY) return false;
	_shadowLogBlurShadowX = _shadowRender->getTech("LogBlurX"); if (!_shadowLogBlurShadowX) return false;
	_shadowLogBlurShadowY = _shadowRender->getTech("LogBlurY"); if (!_shadowLogBlurShadowY) return false;
	_shadowShadowSource = _shadowRender->getParameter("texSource"); if (!_shadowShadowSource) return false;
	_shadowShadowSourceInv = _shadowRender->getParameter("texSourceSizeInv"); if (!_shadowShadowSourceInv) return false;
	_shadowClipConstant = _shadowRender->getParameter("clipConstant"); if (!_shadowClipConstant) return false;
	_shadowOffset = _shadowRender->getParameter("offset"); if (!_shadowOffset) return false;
	_shadowWeight = _shadowRender->getParameter("weight"); if (!_shadowWeight) return false;

	_shadowBlurShadowX[LightType::LightTypeSun] = _shadowBlurOrthoShadowX;
	_shadowBlurShadowX[LightType::LightTypeDirectional] = _shadowBlurOrthoShadowX;
	_shadowBlurShadowX[LightType::LightTypeSpot] = _shadowBlurPerspectiveFovShadowX;
	_shadowBlurShadowX[LightType::LightTypePoint] = _shadowBlurPerspectiveFovShadowX;

	const float offsets[4] = { 1.3846153846f, 3.2307692308f, -1.3846153846f, -3.2307692308f };
	const float weights[3] = { 0.2270270270f,  0.3162162162f, 0.0702702703f };

	std::uint32_t shadowMapSize[ShadowQuality::ShadowQualityRangeSize];
	shadowMapSize[ShadowQuality::ShadowQualityNone] = 0;
	shadowMapSize[ShadowQuality::ShadowQualityLow] = LightShadowSize::LightShadowSizeLow;
	shadowMapSize[ShadowQuality::ShadowQualityMedium] = LightShadowSize::LightShadowSizeMedium;
	shadowMapSize[ShadowQuality::ShadowQualityHigh] = LightShadowSize::LightShadowSizeHigh;
	shadowMapSize[ShadowQuality::ShadowQualityVeryHigh] = LightShadowSize::LightShadowSizeVeryHigh;

	_shadowOffset->uniform1fv(4, offsets);
	_shadowWeight->uniform1fv(3, weights);
	_shadowShadowSourceInv->uniform1f(1.0f / shadowMapSize[_shadowQuality]);

	return true;
}

bool
ShadowRenderPipeline::setupShadowMaps(RenderPipeline& pipeline) noexcept
{
	std::uint32_t shadowMapSize[ShadowQuality::ShadowQualityRangeSize];
	shadowMapSize[ShadowQuality::ShadowQualityNone] = 0;
	shadowMapSize[ShadowQuality::ShadowQualityLow] = LightShadowSize::LightShadowSizeLow;
	shadowMapSize[ShadowQuality::ShadowQualityMedium] = LightShadowSize::LightShadowSizeMedium;
	shadowMapSize[ShadowQuality::ShadowQualityHigh] = LightShadowSize::LightShadowSizeHigh;
	shadowMapSize[ShadowQuality::ShadowQualityVeryHigh] = LightShadowSize::LightShadowSizeVeryHigh;

	if (!pipeline.isTextureSupport(_shadowDepthFormat))
		return false;

	GraphicsFramebufferLayoutDesc shadowDephLayoutDesc;
	shadowDephLayoutDesc.addComponent(GraphicsAttachmentLayout(0, GraphicsImageLayout::GraphicsImageLayoutDepthStencilAttachmentOptimal, _shadowDepthFormat));
	_shadowShadowDepthImageLayout = pipeline.createFramebufferLayout(shadowDephLayoutDesc);
	if (!_shadowShadowDepthImageLayout)
		return false;

	GraphicsTextureDesc shadowDepthMapDesc;
	shadowDepthMapDesc.setWidth(shadowMapSize[_shadowQuality]);
	shadowDepthMapDesc.setHeight(shadowMapSize[_shadowQuality]);
	shadowDepthMapDesc.setTexFormat(_shadowDepthFormat);
	shadowDepthMapDesc.setSamplerFilter(GraphicsSamplerFilter::GraphicsSamplerFilterLinear);
	_shadowShadowDepthMapTemp = pipeline.createTexture(shadowDepthMapDesc);
	if (!_shadowShadowDepthMapTemp)
		return false;

	GraphicsFramebufferDesc shadowDepthViewDesc;
	shadowDepthViewDesc.setWidth(shadowMapSize[_shadowQuality]);
	shadowDepthViewDesc.setHeight(shadowMapSize[_shadowQuality]);
	shadowDepthViewDesc.setDepthStencilAttachment(GraphicsAttachmentBinding(_shadowShadowDepthMapTemp, 0, 0));
	shadowDepthViewDesc.setGraphicsFramebufferLayout(_shadowShadowDepthImageLayout);
	_shadowShadowDepthViewTemp = pipeline.createFramebuffer(shadowDepthViewDesc);
	if (!_shadowShadowDepthViewTemp)
		return false;

	return true;
}

bool 
ShadowRenderPipeline::setupShadowSoftMaps(RenderPipeline& pipeline) noexcept
{
	if (!pipeline.isTextureSupport(_shadowDepthLinearFormat))
		return false;

	std::uint32_t shadowMapSize[ShadowQuality::ShadowQualityRangeSize];
	shadowMapSize[ShadowQuality::ShadowQualityNone] = 0;
	shadowMapSize[ShadowQuality::ShadowQualityLow] = LightShadowSize::LightShadowSizeLow;
	shadowMapSize[ShadowQuality::ShadowQualityMedium] = LightShadowSize::LightShadowSizeMedium;
	shadowMapSize[ShadowQuality::ShadowQualityHigh] = LightShadowSize::LightShadowSizeHigh;
	shadowMapSize[ShadowQuality::ShadowQualityVeryHigh] = LightShadowSize::LightShadowSizeVeryHigh;

	GraphicsFramebufferLayoutDesc shaodwMapLayoutDesc;
	shaodwMapLayoutDesc.addComponent(GraphicsAttachmentLayout(0, GraphicsImageLayout::GraphicsImageLayoutColorAttachmentOptimal, _shadowDepthLinearFormat));
	_shadowShadowDepthLinearImageLayout = pipeline.createFramebufferLayout(shaodwMapLayoutDesc);
	if (!_shadowShadowDepthLinearImageLayout)
		return false;

	GraphicsTextureDesc shadowDepthLinearMapDesc;
	shadowDepthLinearMapDesc.setWidth(shadowMapSize[_shadowQuality]);
	shadowDepthLinearMapDesc.setHeight(shadowMapSize[_shadowQuality]);
	shadowDepthLinearMapDesc.setTexFormat(_shadowDepthLinearFormat);
	shadowDepthLinearMapDesc.setSamplerFilter(GraphicsSamplerFilter::GraphicsSamplerFilterLinear);
	_shadowShadowDepthLinearMapTemp = pipeline.createTexture(shadowDepthLinearMapDesc);
	if (!_shadowShadowDepthLinearMapTemp)
		return false;

	GraphicsFramebufferDesc shadowDepthLinearViewDesc;
	shadowDepthLinearViewDesc.setWidth(shadowMapSize[_shadowQuality]);
	shadowDepthLinearViewDesc.setHeight(shadowMapSize[_shadowQuality]);
	shadowDepthLinearViewDesc.addColorAttachment(GraphicsAttachmentBinding(_shadowShadowDepthLinearMapTemp, 0, 0));
	shadowDepthLinearViewDesc.setGraphicsFramebufferLayout(_shadowShadowDepthLinearImageLayout);
	_shadowShadowDepthLinearViewTemp = pipeline.createFramebuffer(shadowDepthLinearViewDesc);
	if (!_shadowShadowDepthLinearViewTemp)
		return false;

	return true;
}

void
ShadowRenderPipeline::destroyShadowMaterial() noexcept
{
	_shadowShadowSource.reset();
	_shadowShadowSourceInv.reset();
	_shadowClipConstant.reset();
	_shadowOffset.reset();
	_shadowWeight.reset();

	_shadowBlurOrthoShadowX.reset();
	_shadowBlurPerspectiveFovShadowX.reset();
	_shadowBlurShadowY.reset();
	_shadowLogBlurShadowX.reset();
	_shadowLogBlurShadowY.reset();
	_shadowConvOrthoLinearDepth.reset();
	_shadowConvPerspectiveFovLinearDepth.reset();

	for (std::size_t i = 0; i < LightType::LightTypeRangeSize; i++)
		_shadowBlurShadowX[i].reset();

	_shadowRender.reset();
}

void
ShadowRenderPipeline::destroyShadowMaps() noexcept
{
	_shadowShadowDepthMapTemp.reset();
	_shadowShadowDepthViewTemp.reset();
	_shadowShadowDepthImageLayout.reset();

	_shadowShadowDepthLinearMapTemp.reset();
	_shadowShadowDepthLinearViewTemp.reset();
	_shadowShadowDepthLinearImageLayout.reset();
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

	if (_shadowQuality != ShadowQuality::ShadowQualityNone)
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