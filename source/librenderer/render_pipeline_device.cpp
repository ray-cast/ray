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
#include <ray/render_pipeline_device.h>
#include <ray/render_pipeline.h>

#include <ray/material.h>
#include <ray/material_manager.h>

#include <ray/graphics_system.h>
#include <ray/graphics_device.h>
#include <ray/graphics_texture.h>

_NAME_BEGIN

__ImplementSubClass(RenderPipelineDevice, rtti::Interface, "RenderPipelineDevice")

RenderPipelineDevice::RenderPipelineDevice() noexcept
{
}

RenderPipelineDevice::~RenderPipelineDevice() noexcept
{
	this->close();
}

bool
RenderPipelineDevice::open(GraphicsDeviceType type) noexcept
{
	if (!GraphicsSystem::instance()->open(true))
		return false;

	GraphicsDeviceDesc deviceDesc;
	deviceDesc.setDeviceType(type);
	_graphicsDevice = GraphicsSystem::instance()->createDevice(deviceDesc);
	if (!_graphicsDevice)
		return false;

	_materialManager = std::make_shared<MaterialManager>();
	if (!_materialManager->setup(_graphicsDevice))
		return false;

	return true;
}

void
RenderPipelineDevice::close() noexcept
{
	_materialManager.reset();
	_graphicsDevice.reset();
}

GraphicsDeviceType
RenderPipelineDevice::getDeviceType() const noexcept
{
	return _graphicsDevice->getGraphicsDeviceDesc().getDeviceType();
}

RenderPipelinePtr
RenderPipelineDevice::createRenderPipeline(WindHandle window, std::uint32_t w, std::uint32_t h, std::uint32_t dpi_w, std::uint32_t dpi_h, GraphicsSwapInterval interval) noexcept
{
	auto pipelineContext = std::make_shared<RenderPipeline>();
	if (pipelineContext->setup(this->downcast_pointer<RenderPipelineDevice>(), window, w, h, dpi_w, dpi_h, interval))
		return pipelineContext;
	return nullptr;
}

MaterialPtr
RenderPipelineDevice::createMaterial(const std::string& name) noexcept
{
	assert(_materialManager);
	return _materialManager->createMaterial(name);
}

void
RenderPipelineDevice::destroyMaterial(MaterialPtr material) noexcept
{
	assert(_materialManager);
	return _materialManager->destroyMaterial(material);
}

GraphicsTexturePtr
RenderPipelineDevice::createTexture(const GraphicsTextureDesc& desc) noexcept
{
	assert(_graphicsDevice);
	return _graphicsDevice->createTexture(desc);
}

GraphicsTexturePtr
RenderPipelineDevice::createTexture(std::uint32_t w, std::uint32_t h, GraphicsTextureDim dim, GraphicsFormat format, GraphicsSamplerFilter filter, GraphicsSamplerWrap warp) noexcept
{
	assert(_graphicsDevice);
	GraphicsTextureDesc textureDesc;
	textureDesc.setWidth(w);
	textureDesc.setHeight(h);
	textureDesc.setTexDim(dim);
	textureDesc.setTexFormat(format);
	textureDesc.setSamplerFilter(filter);
	textureDesc.setSamplerWrap(warp);
	return _graphicsDevice->createTexture(textureDesc);
}

GraphicsSwapchainPtr
RenderPipelineDevice::createSwapchain(const GraphicsSwapchainDesc& desc) noexcept
{
	assert(_graphicsDevice);
	return _graphicsDevice->createSwapchain(desc);
}

GraphicsContextPtr
RenderPipelineDevice::createDeviceContext(const GraphicsContextDesc& desc) noexcept
{
	assert(_graphicsDevice);
	return _graphicsDevice->createDeviceContext(desc);
}

GraphicsFramebufferLayoutPtr
RenderPipelineDevice::createFramebufferLayout(const GraphicsFramebufferLayoutDesc& desc) noexcept
{
	assert(_graphicsDevice);
	return _graphicsDevice->createFramebufferLayout(desc);
}

GraphicsFramebufferPtr
RenderPipelineDevice::createFramebuffer(const GraphicsFramebufferDesc& desc) noexcept
{
	assert(_graphicsDevice);
	return _graphicsDevice->createFramebuffer(desc);
}

GraphicsInputLayoutPtr
RenderPipelineDevice::createInputLayout(const GraphicsInputLayoutDesc& desc) noexcept
{
	assert(_graphicsDevice);
	return _graphicsDevice->createInputLayout(desc);
}

GraphicsPipelinePtr
RenderPipelineDevice::createGraphicsPipeline(const GraphicsPipelineDesc& desc) noexcept
{
	assert(_graphicsDevice);
	return _graphicsDevice->createRenderPipeline(desc);
}

GraphicsDataPtr
RenderPipelineDevice::createGraphicsData(const GraphicsDataDesc& desc) noexcept
{
	assert(_graphicsDevice);
	return _graphicsDevice->createGraphicsData(desc);
}

_NAME_END