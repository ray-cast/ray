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
#include "light_probe_render_pipeline.h"
#include <ray/light_probe_render_framebuffer.h>

#include <ray/render_pipeline.h>
#include <ray/render_pipeline_framebuffer.h>
#include <ray/render_object_manager.h>

#include <ray/graphics_texture.h>
#include <ray/graphics_framebuffer.h>

#include <ray/camera.h>
#include <ray/light_probe.h>

_NAME_BEGIN

__ImplementSubClass(LightProbeRenderPipeline, RenderPipelineController, "LightProbeRenderPipeline")

const float4x4 viewMatrixs[] = {
	float4x4(0, 0,-1, 0, 0,-1, 0, 0, -1, 0, 0, 0, 0, 0, 0, 1),
	float4x4(0, 0, 1, 0, 0,-1, 0, 0,  1, 0, 0, 0, 0, 0, 0, 1),
	float4x4(1, 0, 0, 0, 0, 0, 1, 0, 0,-1, 0, 0, 0, 0, 0, 1),
	float4x4(1, 0, 0, 0, 0, 0,-1, 0, 0, 1, 0, 0, 0, 0, 0, 1),
	float4x4(1, 0, 0, 0, 0,-1, 0, 0, 0, 0,-1, 0, 0, 0, 0, 1),
	float4x4(-1, 0, 0, 0,  0,-1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1)
};

LightProbeRenderPipeline::LightProbeRenderPipeline() noexcept
{
}

LightProbeRenderPipeline::~LightProbeRenderPipeline() noexcept
{
	this->close();
}

bool
LightProbeRenderPipeline::setup(const RenderPipelinePtr& pipeline, std::uint32_t probeMapSize) noexcept
{
	assert(pipeline);
	assert(probeMapSize > 0);

	GraphicsFormat probeDepthFormat;
	if (pipeline->isTextureSupport(GraphicsFormat::GraphicsFormatD32_SFLOAT))
		probeDepthFormat = GraphicsFormat::GraphicsFormatD32_SFLOAT;
	else if (pipeline->isTextureSupport(GraphicsFormat::GraphicsFormatX8_D24UNormPack32))
		probeDepthFormat = GraphicsFormat::GraphicsFormatX8_D24UNormPack32;
	else if (pipeline->isTextureSupport(GraphicsFormat::GraphicsFormatD16UNorm))
		probeDepthFormat = GraphicsFormat::GraphicsFormatD16UNorm;
	else
		return false;

	GraphicsFormat probeColorFormat;
	GraphicsFormat probeNormalFormat;

	if (pipeline->isTextureSupport(GraphicsFormat::GraphicsFormatR8G8B8UNorm))
		probeColorFormat = probeNormalFormat = GraphicsFormat::GraphicsFormatR8G8B8UNorm;
	else
		return false;

	GraphicsTextureDesc probeDepthDesc;
	probeDepthDesc.setWidth(probeMapSize);
	probeDepthDesc.setHeight(probeMapSize);
	probeDepthDesc.setTexDim(GraphicsTextureDim::GraphicsTextureDim2DArray);
	probeDepthDesc.setTexFormat(probeDepthFormat);
	probeDepthDesc.setLayerNums(6);
	probeDepthDesc.setSamplerFilter(GraphicsSamplerFilter::GraphicsSamplerFilterNearest, GraphicsSamplerFilter::GraphicsSamplerFilterNearest);
	_probeDepthMap = pipeline->createTexture(probeDepthDesc);
	if (!_probeDepthMap)
		return false;

	GraphicsTextureDesc probeColorDesc;
	probeColorDesc.setWidth(probeMapSize);
	probeColorDesc.setHeight(probeMapSize);
	probeColorDesc.setTexDim(GraphicsTextureDim::GraphicsTextureDim2DArray);
	probeColorDesc.setTexFormat(probeColorFormat);
	probeColorDesc.setLayerNums(6);
	probeDepthDesc.setSamplerFilter(GraphicsSamplerFilter::GraphicsSamplerFilterNearest, GraphicsSamplerFilter::GraphicsSamplerFilterNearest);
	_probeColorMap = pipeline->createTexture(probeColorDesc);
	if (!_probeColorMap)
		return false;

	GraphicsTextureDesc probeNormalDesc;
	probeNormalDesc.setWidth(probeMapSize);
	probeNormalDesc.setHeight(probeMapSize);
	probeNormalDesc.setTexDim(GraphicsTextureDim::GraphicsTextureDim2DArray);
	probeNormalDesc.setTexFormat(probeNormalFormat);
	probeNormalDesc.setLayerNums(6);
	probeDepthDesc.setSamplerFilter(GraphicsSamplerFilter::GraphicsSamplerFilterNearest, GraphicsSamplerFilter::GraphicsSamplerFilterNearest);
	_probeNormalMap = pipeline->createTexture(probeNormalDesc);
	if (!_probeNormalMap)
		return false;

	GraphicsFramebufferLayoutDesc shaodwRSMMapLayoutDesc;
	shaodwRSMMapLayoutDesc.addComponent(GraphicsAttachmentLayout(0, GraphicsImageLayout::GraphicsImageLayoutColorAttachmentOptimal, probeColorFormat));
	shaodwRSMMapLayoutDesc.addComponent(GraphicsAttachmentLayout(1, GraphicsImageLayout::GraphicsImageLayoutColorAttachmentOptimal, probeNormalFormat));
	shaodwRSMMapLayoutDesc.addComponent(GraphicsAttachmentLayout(2, GraphicsImageLayout::GraphicsImageLayoutDepthStencilReadOnlyOptimal, probeDepthFormat));
	_probeRSMViewLayout = pipeline->createFramebufferLayout(shaodwRSMMapLayoutDesc);
	if (!_probeRSMViewLayout)
		return false;

	for (std::uint8_t i = 0; i < 6; i++)
	{
		GraphicsFramebufferDesc probeRSMViewDesc;
		probeRSMViewDesc.setWidth(probeMapSize);
		probeRSMViewDesc.setHeight(probeMapSize);
		probeRSMViewDesc.addColorAttachment(GraphicsAttachmentBinding(_probeColorMap, 0, i));
		probeRSMViewDesc.addColorAttachment(GraphicsAttachmentBinding(_probeNormalMap, 0, i));
		probeRSMViewDesc.setDepthStencilAttachment(GraphicsAttachmentBinding(_probeDepthMap, 0, i));
		probeRSMViewDesc.setGraphicsFramebufferLayout(_probeRSMViewLayout);
		_probeRSMViews[i] = pipeline->createFramebuffer(probeRSMViewDesc);
		if (!_probeRSMViews[i])
			return false;
	}

	_camera = std::make_shared<Camera>();
	_camera->setCameraType(ray::CameraType::CameraTypePerspective);
	_camera->setNear(0.125);
	_camera->setRatio(1.0);
	_camera->setFar(10.0f);
	_camera->setAperture(90.0f);
	_camera->setClearFlags(CameraClearFlagBits::CameraClearColorBit | CameraClearFlagBits::CameraClearDepthBit);

	_pipeline = pipeline;

	return true;
}

void
LightProbeRenderPipeline::close() noexcept
{
	_pipeline = nullptr;
}

void
LightProbeRenderPipeline::onRenderPipeline(const Camera* mainCamera) noexcept
{
	assert(mainCamera);

	_pipeline->setCamera(mainCamera);

	const auto& lightProbes = mainCamera->getRenderDataManager()->getRenderData(RenderQueue::RenderQueueLightProbes);
	for (auto& it : lightProbes)
	{
		auto lightProbe = it->downcast<LightProbe>();
		if (!lightProbe->needUpdateProbeMap())
			continue;

		auto& camera = lightProbe->getCamera();
		if (camera)
		{
			if (!camera->getRenderPipelineFramebuffer())
				continue;

			if (!camera->getRenderScene())
				continue;

			camera->onRenderBefore(*mainCamera);

			static float3 lookat[] = { float3::UnitX, -float3::UnitX, float3::UnitY, -float3::UnitY, float3::UnitZ, -float3::UnitZ };
			static float3 up[] = { -float3::UnitY, -float3::UnitY, float3::UnitZ, -float3::UnitZ, -float3::UnitY, -float3::UnitY };

			for (std::uint8_t i = 0; i < 6; i++)
			{
				ray::float4x4 transform;
				transform.makeLookAt_lh(it->getTranslate(), it->getTranslate() + lookat[i], up[i]);

				_camera->setTransform(math::transformInverse(transform), transform);
				_camera->setRenderDataManager(camera->getRenderDataManager());
				_camera->setRenderPipelineFramebuffer(camera->getRenderPipelineFramebuffer());

				_pipeline->setCamera(_camera.get(), true);
				_pipeline->setFramebuffer(_probeRSMViews[i]);

				if (_camera->getClearFlags() & CameraClearFlagBits::CameraClearColorBit)
				{
					_pipeline->clearFramebuffer(0, CameraClearFlagBits::CameraClearColorBit, _camera->getClearColor());
					_pipeline->clearFramebuffer(1, CameraClearFlagBits::CameraClearColorBit, _camera->getClearColor());
				}

				if (_camera->getClearFlags() & CameraClearFlagBits::CameraClearDepthBit ||
					_camera->getClearFlags() & CameraClearFlagBits::CameraClearStencilBit)
				{
					if (_camera->getClearFlags() & CameraClearFlagBits::CameraClearDepthStencilBit)
						_pipeline->clearFramebuffer(2, CameraClearFlagBits::CameraClearDepthStencilBit, _camera->getClearColor());
					else if (_camera->getClearFlags() & CameraClearFlagBits::CameraClearDepthBit)
						_pipeline->clearFramebuffer(2, CameraClearFlagBits::CameraClearDepthBit, _camera->getClearColor());
					else if (_camera->getClearFlags() & CameraClearFlagBits::CameraClearStencilBit)
						_pipeline->clearFramebuffer(2, CameraClearFlagBits::CameraClearStencilBit, _camera->getClearColor());
				}

				_pipeline->drawRenderQueue(RenderQueue::RenderQueueOpaque);

				auto framebuffers = camera->getRenderPipelineFramebuffer()->downcast<LightProbeRenderFramebuffer>();

				_pipeline->readFramebufferToCube(0, i, framebuffers->getColorMap(), 0, 0, 0, framebuffers->getColorMap()->getGraphicsTextureDesc().getWidth(), framebuffers->getColorMap()->getGraphicsTextureDesc().getHeight());
				_pipeline->readFramebufferToCube(1, i, framebuffers->getNormalMap(), 0, 0, 0, framebuffers->getNormalMap()->getGraphicsTextureDesc().getWidth(), framebuffers->getNormalMap()->getGraphicsTextureDesc().getHeight());

				_pipeline->discardFramebuffer(0);
				_pipeline->discardFramebuffer(1);
				_pipeline->discardFramebuffer(2);
			}

			camera->onRenderAfter(*mainCamera);
		}
	}
}

void
LightProbeRenderPipeline::onRenderBefore() noexcept
{
}

void
LightProbeRenderPipeline::onRenderAfter() noexcept
{
}

_NAME_END