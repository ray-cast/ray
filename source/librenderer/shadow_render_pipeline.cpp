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
	: _shadowDepthFormat(GraphicsFormat::GraphicsFormatD16UNorm)
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
	if (!initTextureFormat(*pipeline))
		return false;

	if (!setupShadowMaterial(*pipeline))
		return false;

	if (!setupShadowMaps(*pipeline))
		return false;

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
ShadowRenderPipeline::renderShadowMaps(const CameraPtr& mainCamera) noexcept
{
	_pipeline->setCamera(mainCamera);

	const auto& lights = _pipeline->getCamera()->getRenderDataManager()->getRenderData(RenderQueue::RenderQueueLighting);
	for (auto& it : lights)
	{
		auto light = it->downcast<Light>();
		if (light->getShadowType() == LightShadowType::LightShadowTypeNone)
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
	auto lightType = light.getLightType();
	auto lightShadowType = light.getShadowType();

	auto& cameras = light.getCameras();
	for (auto& camera : cameras)
	{
		auto& depthFrambuffer = camera->getFramebuffer();
		auto& depthLienarFrambuffer = camera->getDepthLinearFramebuffer();

		auto shadowFrambuffer = depthFrambuffer ? depthFrambuffer : _shadowShadowDepthViewTemps[lightShadowType];
		auto shadowTexture = shadowFrambuffer->getGraphicsFramebufferDesc().getDepthStencilAttachment().getBindingTexture();
		auto shadowTextureSizeInv = float2(1.0f / shadowTexture->getGraphicsTextureDesc().getWidth(), 1.0f / shadowTexture->getGraphicsTextureDesc().getHeight());

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

		if (light.getSoftShadow())
		{
			_shadowShadowSource->uniformTexture(shadowTexture);
			_shadowShadowSourceInv->uniform2f(shadowTextureSizeInv);
			_shadowClipConstant->uniform4f(float4(camera->getClipConstant().xy(), 1.0, 1.0));
			_pipeline->setFramebuffer(_shadowShadowDepthLinearViewTemps[lightShadowType]);
			_pipeline->discardFramebuffer(0);

			if (lightType == LightType::LightTypeSun ||
				lightType == LightType::LightTypeDirectional)
			{
				_pipeline->drawScreenQuad(*_shadowBlurOrthoShadowX);
			}
			else
			{
				_pipeline->drawScreenQuad(*_shadowBlurPerspectiveFovShadowX);
			}

			_shadowShadowSource->uniformTexture(_shadowShadowDepthLinearMapTemps[lightShadowType]);
			_shadowShadowSourceInv->uniform2f(shadowTextureSizeInv);
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

			if (lightType == LightType::LightTypeSun ||
				lightType == LightType::LightTypeDirectional)
			{
				_pipeline->drawScreenQuad(*_shadowConvOrthoLinearDepth);
			}
			else
			{
				_pipeline->drawScreenQuad(*_shadowConvPerspectiveFovLinearDepth);
			}
		}
	}
}

bool
ShadowRenderPipeline::initTextureFormat(RenderPipeline& pipeline) noexcept
{
	if (pipeline.isTextureSupport(GraphicsFormat::GraphicsFormatR32SFloat))
		_shadowDepthLinearFormat = GraphicsFormat::GraphicsFormatR32SFloat;
	else if (pipeline.isTextureSupport(GraphicsFormat::GraphicsFormatR16SFloat))
		_shadowDepthLinearFormat = GraphicsFormat::GraphicsFormatR16SFloat;
	else if (pipeline.isTextureSupport(GraphicsFormat::GraphicsFormatR8G8B8A8UNorm))
		_shadowDepthLinearFormat = GraphicsFormat::GraphicsFormatR8G8B8A8UNorm;
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
	_shadowRender = pipeline.createMaterial("sys:fx/shadowmap.fxml");
	_shadowConvOrthoLinearDepth = _shadowRender->getTech("ConvOrthoLinearDepth");
	_shadowConvPerspectiveFovLinearDepth = _shadowRender->getTech("ConvPerspectiveFovLinearDepth");
	_shadowBlurOrthoShadowX = _shadowRender->getTech("ConvOrthoLinearDepthBlurX");
	_shadowBlurPerspectiveFovShadowX = _shadowRender->getTech("ConvPerspectiveFovLinearDepthBlurX");
	_shadowBlurShadowY = _shadowRender->getTech("BlurY");
	_shadowLogBlurShadowX = _shadowRender->getTech("LogBlurX");
	_shadowLogBlurShadowY = _shadowRender->getTech("LogBlurY");
	_shadowShadowSource = _shadowRender->getParameter("texSource");
	_shadowShadowSourceInv = _shadowRender->getParameter("texSourceSizeInv");
	_shadowClipConstant = _shadowRender->getParameter("clipConstant");
	_shadowOffset = _shadowRender->getParameter("offset");
	_shadowWeight = _shadowRender->getParameter("weight");

	const float offsets[4] = { 1.3846153846f, 3.2307692308f, -1.3846153846f, -3.2307692308f };
	const float weights[3] = { 0.2270270270f,  0.3162162162f, 0.0702702703f };

	_shadowOffset->uniform1fv(4, offsets);
	_shadowWeight->uniform1fv(3, weights);

	return true;
}

bool
ShadowRenderPipeline::setupShadowMaps(RenderPipeline& pipeline) noexcept
{
	std::uint32_t shadowMapSize[LightShadowType::LightShadowTypeRangeSize];
	shadowMapSize[LightShadowType::LightShadowTypeNone] = 0;
	shadowMapSize[LightShadowType::LightShadowTypeLow] = LightShadowSize::LightShadowSizeLow;
	shadowMapSize[LightShadowType::LightShadowTypeMedium] = LightShadowSize::LightShadowSizeMedium;
	shadowMapSize[LightShadowType::LightShadowTypeHigh] = LightShadowSize::LightShadowSizeHigh;
	shadowMapSize[LightShadowType::LightShadowTypeVeryHigh] = LightShadowSize::LightShadowSizeVeryHigh;

	_shadowShadowDepthMapTemps.resize(LightShadowType::LightShadowTypeRangeSize);
	_shadowShadowDepthLinearMapTemps.resize(LightShadowType::LightShadowTypeRangeSize);

	_shadowShadowDepthViewTemps.resize(LightShadowType::LightShadowTypeRangeSize);
	_shadowShadowDepthLinearViewTemps.resize(LightShadowType::LightShadowTypeRangeSize);

	GraphicsFramebufferLayoutDesc shadowDephLayoutDesc;
	shadowDephLayoutDesc.addComponent(GraphicsAttachmentLayout(0, GraphicsImageLayout::GraphicsImageLayoutDepthStencilAttachmentOptimal, _shadowDepthFormat));
	_shadowShadowDepthImageLayout = pipeline.createFramebufferLayout(shadowDephLayoutDesc);
	if (!_shadowShadowDepthImageLayout)
		return false;

	GraphicsFramebufferLayoutDesc shaodwMapLayoutDesc;
	shaodwMapLayoutDesc.addComponent(GraphicsAttachmentLayout(0, GraphicsImageLayout::GraphicsImageLayoutColorAttachmentOptimal, _shadowDepthLinearFormat));
	_shadowShadowDepthLinearImageLayout = pipeline.createFramebufferLayout(shaodwMapLayoutDesc);
	if (!_shadowShadowDepthLinearImageLayout)
		return false;

	for (std::size_t i = LightShadowType::LightShadowTypeLow; i <= LightShadowType::LightShadowTypeVeryHigh; i++)
	{
		_shadowShadowDepthMapTemps[i] = pipeline.createTexture(shadowMapSize[i], shadowMapSize[i], GraphicsTextureDim::GraphicsTextureDim2D, _shadowDepthFormat);
		if (!_shadowShadowDepthMapTemps[i])
			return false;

		_shadowShadowDepthLinearMapTemps[i] = pipeline.createTexture(shadowMapSize[i], shadowMapSize[i], GraphicsTextureDim::GraphicsTextureDim2D, _shadowDepthLinearFormat);
		if (!_shadowShadowDepthLinearMapTemps[i])
			return false;

		GraphicsFramebufferDesc shadowDepthViewDesc;
		shadowDepthViewDesc.setWidth(shadowMapSize[i]);
		shadowDepthViewDesc.setHeight(shadowMapSize[i]);
		shadowDepthViewDesc.setDepthStencilAttachment(GraphicsAttachmentBinding(_shadowShadowDepthMapTemps[i], 0, 0));
		shadowDepthViewDesc.setGraphicsFramebufferLayout(_shadowShadowDepthImageLayout);
		_shadowShadowDepthViewTemps[i] = pipeline.createFramebuffer(shadowDepthViewDesc);
		if (!_shadowShadowDepthViewTemps[i])
			return false;

		GraphicsFramebufferDesc shadowDepthLinearViewDesc;
		shadowDepthLinearViewDesc.setWidth(shadowMapSize[i]);
		shadowDepthLinearViewDesc.setHeight(shadowMapSize[i]);
		shadowDepthLinearViewDesc.addColorAttachment(GraphicsAttachmentBinding(_shadowShadowDepthLinearMapTemps[i], 0, 0));
		shadowDepthLinearViewDesc.setGraphicsFramebufferLayout(_shadowShadowDepthLinearImageLayout);
		_shadowShadowDepthLinearViewTemps[i] = pipeline.createFramebuffer(shadowDepthLinearViewDesc);
		if (!_shadowShadowDepthLinearViewTemps[i])
			return false;
	}

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

	_shadowRender.reset();
}

void
ShadowRenderPipeline::destroyShadowMaps() noexcept
{
	_shadowShadowDepthMapTemps.clear();
	_shadowShadowDepthViewTemps.clear();
	_shadowShadowDepthImageLayout.reset();

	_shadowShadowDepthLinearMapTemps.clear();
	_shadowShadowDepthLinearViewTemps.clear();
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