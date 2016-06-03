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
#include "vk_device_context.h"
#include "vk_device.h"
#include "vk_swapchain.h"
#include "vk_command_list.h"
#include "vk_command_queue.h"
#include "vk_pipeline.h"
#include "vk_descriptor_set.h"
#include "vk_graphics_data.h"
#include "vk_system.h"

_NAME_BEGIN

__ImplementSubClass(VulkanDeviceContext, GraphicsContext, "VulkanDeviceContext")

VulkanDeviceContext::VulkanDeviceContext() noexcept
	: _viewports(8)
	, _scissor(8)
	, _clearValues(8)
{
}

VulkanDeviceContext::~VulkanDeviceContext() noexcept
{
	this->close();
}

bool
VulkanDeviceContext::setup(const GraphicsContextDesc& desc) noexcept
{
	assert(desc.getSwapchain());
	assert(desc.getSwapchain()->isInstanceOf<VulkanSwapchain>());

	_swapchain = desc.getSwapchain()->downcast_pointer<VulkanSwapchain>();

	if (!this->initTextureSupports())
		return false;

	if (!this->initTextureDimSupports())
		return false;

	if (!this->initVertexSupports())
		return false;

	if (!this->initShaderSupports())
		return false;

	if (!this->initCommandList())
		return false;

	return true;
}

void
VulkanDeviceContext::close() noexcept
{
	_commandQueue.reset();
	_commandList.reset();
	_commandPool.reset();
}

void
VulkanDeviceContext::renderBegin() noexcept
{
	auto swapchaic = _swapchain->downcast_pointer<VulkanSwapchain>();
	swapchaic->acquireNextImage();

	_commandList->renderBegin();
	_commandList->setFramebuffer(swapchaic->getSwapchainFramebuffers()[swapchaic->getSwapchainImageIndex()]);
}

void
VulkanDeviceContext::renderEnd() noexcept
{
	_commandList->renderEnd();

	_commandQueue->executeCommandLists(&_commandList, 1);
	_commandQueue->present(&_swapchain, 1);
}

void
VulkanDeviceContext::setViewport(std::uint32_t i, const Viewport& viewport) noexcept
{
	_commandList->setViewport(&viewport, i, 1);
}

const Viewport&
VulkanDeviceContext::getViewport(std::uint32_t i) const noexcept
{
	return _viewports[0];
}

void
VulkanDeviceContext::setScissor(std::uint32_t i, const Scissor& scissor) noexcept
{
	_commandList->setScissor(&scissor, i, 1);
}

const Scissor&
VulkanDeviceContext::getScissor(std::uint32_t i) const noexcept
{
	return _scissor[0];
}

void
VulkanDeviceContext::setStencilCompareMask(GraphicsStencilFace face, std::uint32_t mask) noexcept
{
	_commandList->setStencilCompareMask(face, mask);
}

std::uint32_t
VulkanDeviceContext::getStencilCompareMask(GraphicsStencilFace face) noexcept
{
	return _commandList->getStencilCompareMask(face);
}

void
VulkanDeviceContext::setStencilReference(GraphicsStencilFace face, std::uint32_t reference) noexcept
{
	_commandList->setStencilReference(face, reference);
}

std::uint32_t
VulkanDeviceContext::getStencilReference(GraphicsStencilFace face) noexcept
{
	return _commandList->getStencilReference(face);
}

void
VulkanDeviceContext::setStencilFrontWriteMask(GraphicsStencilFace face, std::uint32_t mask) noexcept
{
	_commandList->setStencilFrontWriteMask(face, mask);
}

std::uint32_t
VulkanDeviceContext::getStencilFrontWriteMask(GraphicsStencilFace face) noexcept
{
	return _commandList->getStencilFrontWriteMask(face);
}

void
VulkanDeviceContext::setFramebuffer(GraphicsFramebufferPtr framebuffer) noexcept
{
	if (_framebuffer != framebuffer)
	{
		if (framebuffer)
			_commandList->setFramebuffer(framebuffer);
		else
		{
			assert(_swapchain);
			auto swapchain = _swapchain->downcast_pointer<VulkanSwapchain>();
			_commandList->setFramebuffer(swapchain->getSwapchainFramebuffers()[swapchain->getSwapchainImageIndex()]);
		}

		_framebuffer = framebuffer;
	}
}

void
VulkanDeviceContext::clearFramebuffer(std::uint32_t i, GraphicsClearFlags flags, const float4& color, float depth, std::int32_t stencil) noexcept
{
	_commandList->clearFramebuffer(i, flags, color, depth, stencil);
}

GraphicsFramebufferPtr
VulkanDeviceContext::getFramebuffer() const noexcept
{
	return _framebuffer;
}

void
VulkanDeviceContext::setRenderPipeline(GraphicsPipelinePtr pipeline) noexcept
{
	assert(pipeline);
	assert(pipeline->isInstanceOf<VulkanPipeline>());

	_commandList->setPipeline(pipeline);
}

GraphicsPipelinePtr
VulkanDeviceContext::getRenderPipeline() const noexcept
{
	return _pipeline;
}

void
VulkanDeviceContext::setDescriptorSet(GraphicsDescriptorSetPtr descriptorSet) noexcept
{
	assert(descriptorSet);
	assert(descriptorSet->isInstanceOf<VulkanDescriptorSet>());

	_commandList->setDescriptorSet(descriptorSet);
	descriptorSet->downcast<VulkanDescriptorSet>()->update();
}

GraphicsDescriptorSetPtr
VulkanDeviceContext::getDescriptorSet() const noexcept
{
	return _descriptorSet;
}

void
VulkanDeviceContext::setVertexBufferData(GraphicsDataPtr data) noexcept
{
	assert(data);
	assert(data->isInstanceOf<VulkanGraphicsData>());
	assert(data->getGraphicsDataDesc().getType() == GraphicsDataType::GraphicsDataTypeStorageVertexBuffer);

	_vertexBuffer = data;
	_commandList->setVertexBuffers(&data, 0, 1);
}

GraphicsDataPtr
VulkanDeviceContext::getVertexBufferData() const noexcept
{
	return _vertexBuffer;
}

void
VulkanDeviceContext::setIndexBufferData(GraphicsDataPtr data) noexcept
{
	assert(data);
	assert(data->isInstanceOf<VulkanGraphicsData>());
	assert(data->getGraphicsDataDesc().getType() == GraphicsDataType::GraphicsDataTypeStorageIndexBuffer);

	_indexBuffer = data;
	_commandList->setIndexBuffer(data);
}

GraphicsDataPtr
VulkanDeviceContext::getIndexBufferData() const noexcept
{
	return _indexBuffer;
}

void
VulkanDeviceContext::drawRenderMesh(const GraphicsIndirect& renderable) noexcept
{
	_commandList->drawRenderMesh(renderable);
}

void
VulkanDeviceContext::drawRenderMesh(const GraphicsIndirect renderable[], std::size_t first, std::size_t count) noexcept
{
	_commandList->drawRenderMesh(renderable + first, count);
}

void
VulkanDeviceContext::present() noexcept
{
}

bool
VulkanDeviceContext::isTextureSupport(GraphicsFormat format) noexcept
{
	return std::find(_supportTextures.begin(), _supportTextures.end(), format) != _supportTextures.end();
}

bool
VulkanDeviceContext::isTextureDimSupport(GraphicsTextureDim dimension) noexcept
{
	return std::find(_supportTextureDims.begin(), _supportTextureDims.end(), dimension) != _supportTextureDims.end();
}

bool
VulkanDeviceContext::isVertexSupport(GraphicsFormat format) noexcept
{
	return std::find(_supportAttribute.begin(), _supportAttribute.end(), format) != _supportAttribute.end();
}

bool
VulkanDeviceContext::isShaderSupport(GraphicsShaderStageFlagBits stage) noexcept
{
	return std::find(_supportShaders.begin(), _supportShaders.end(), stage) != _supportShaders.end();
}

bool
VulkanDeviceContext::initCommandList() noexcept
{
	auto device = this->getDevice()->downcast<VulkanDevice>();

	GraphicsCommandQueueDesc queueDesc;
	queueDesc.setCommandQueueFlags(GraphicsCommandQueueFlagBits::GraphicsCommandQueueFlagNoneBit);
	queueDesc.setCommandQueueType(GraphicsCommandType::GraphicsCommandTypeGraphics);
	queueDesc.setCommandQueuePriority(GraphicsCommandQueuePriority::GraphicsCommandQueuePriorityNormal);
	queueDesc.setCommandQueueMask(0);
	_commandQueue = device->createCommandQueue(queueDesc);
	if (!_commandQueue)
	{
		VK_PLATFORM_LOG("Can't create command queue.");
		return false;
	}

	GraphicsCommandPoolDesc poolDesc;
	poolDesc.setCommandListType(GraphicsCommandType::GraphicsCommandTypeGraphics);
	poolDesc.setCommandFlags(GraphicsCommandPoolFlagBits::GraphicsCommandPoolFlagResetCommandBufferBit);
	_commandPool = device->createCommandPool(poolDesc);
	if (!_commandPool)
	{
		VK_PLATFORM_LOG("Can't create command pool.");
		return false;
	}

	GraphicsCommandListDesc commandListDesc;
	commandListDesc.setGraphicsCommandPool(_commandPool);
	_commandList = device->createCommandList(commandListDesc);
	if (!_commandList)
	{
		VK_PLATFORM_LOG("Can't create command list.");
		return false;
	}

	return true;
}

bool
VulkanDeviceContext::initTextureSupports() noexcept
{
	_supportTextures.push_back(GraphicsFormat::GraphicsFormatR4G4UNormPack8);
	_supportTextures.push_back(GraphicsFormat::GraphicsFormatR4G4B4A4UNormPack16);
	_supportTextures.push_back(GraphicsFormat::GraphicsFormatR8G8B8UNorm);
	_supportTextures.push_back(GraphicsFormat::GraphicsFormatR8G8B8A8UNorm);
	_supportTextures.push_back(GraphicsFormat::GraphicsFormatR16G16B16A16UNorm);
	_supportTextures.push_back(GraphicsFormat::GraphicsFormatB8G8R8UNorm);
	_supportTextures.push_back(GraphicsFormat::GraphicsFormatB8G8R8A8UNorm);
	_supportTextures.push_back(GraphicsFormat::GraphicsFormatB4G4R4A4UNormPack16);
	_supportTextures.push_back(GraphicsFormat::GraphicsFormatB5G5R5A1UNormPack16);
	_supportTextures.push_back(GraphicsFormat::GraphicsFormatA1R5G5B5UNormPack16);
	_supportTextures.push_back(GraphicsFormat::GraphicsFormatA2R10G10B10UNormPack32);
	_supportTextures.push_back(GraphicsFormat::GraphicsFormatA2B10G10R10UNormPack32);
	_supportTextures.push_back(GraphicsFormat::GraphicsFormatA8B8G8R8UNormPack32);
	_supportTextures.push_back(GraphicsFormat::GraphicsFormatR16G16B16UInt);
	_supportTextures.push_back(GraphicsFormat::GraphicsFormatR16G16B16SInt);
	_supportTextures.push_back(GraphicsFormat::GraphicsFormatR16G16B16A16UInt);
	_supportTextures.push_back(GraphicsFormat::GraphicsFormatR16G16B16A16SInt);
	_supportTextures.push_back(GraphicsFormat::GraphicsFormatR32G32B32UInt);
	_supportTextures.push_back(GraphicsFormat::GraphicsFormatR32G32B32SInt);
	_supportTextures.push_back(GraphicsFormat::GraphicsFormatR32G32B32A32UInt);
	_supportTextures.push_back(GraphicsFormat::GraphicsFormatR32G32B32A32SInt);
	_supportTextures.push_back(GraphicsFormat::GraphicsFormatR8G8B8SInt);
	_supportTextures.push_back(GraphicsFormat::GraphicsFormatR8G8B8A8SInt);
	_supportTextures.push_back(GraphicsFormat::GraphicsFormatB8G8R8UInt);
	_supportTextures.push_back(GraphicsFormat::GraphicsFormatB8G8R8A8UInt);
	_supportTextures.push_back(GraphicsFormat::GraphicsFormatB8G8R8SInt);
	_supportTextures.push_back(GraphicsFormat::GraphicsFormatB8G8R8A8SInt);
	_supportTextures.push_back(GraphicsFormat::GraphicsFormatA8B8G8R8UIntPack32);
	_supportTextures.push_back(GraphicsFormat::GraphicsFormatA8B8G8R8SIntPack32);
	_supportTextures.push_back(GraphicsFormat::GraphicsFormatR16G16B16SFloat);
	_supportTextures.push_back(GraphicsFormat::GraphicsFormatR16G16B16A16SFloat);
	_supportTextures.push_back(GraphicsFormat::GraphicsFormatR32G32B32SFloat);
	_supportTextures.push_back(GraphicsFormat::GraphicsFormatR32G32B32A32SFloat);
	_supportTextures.push_back(GraphicsFormat::GraphicsFormatB10G11R11UFloatPack32);
	_supportTextures.push_back(GraphicsFormat::GraphicsFormatE5B9G9R9UFloatPack32);
	_supportTextures.push_back(GraphicsFormat::GraphicsFormatS8UInt);
	_supportTextures.push_back(GraphicsFormat::GraphicsFormatD16UNorm);
	_supportTextures.push_back(GraphicsFormat::GraphicsFormatX8_D24UNormPack32);
	_supportTextures.push_back(GraphicsFormat::GraphicsFormatD32_SFLOAT);
	_supportTextures.push_back(GraphicsFormat::GraphicsFormatD32_SFLOAT_S8UInt);
	_supportTextures.push_back(GraphicsFormat::GraphicsFormatD24UNorm_S8UInt);
	_supportTextures.push_back(GraphicsFormat::GraphicsFormatA2B10G10R10UIntPack32);
	_supportTextures.push_back(GraphicsFormat::GraphicsFormatA2R10G10B10UIntPack32);
	_supportTextures.push_back(GraphicsFormat::GraphicsFormatR8G8B8SRGB);
	_supportTextures.push_back(GraphicsFormat::GraphicsFormatR8G8B8A8SRGB);
	_supportTextures.push_back(GraphicsFormat::GraphicsFormatBC1RGBSRGBBlock);
	_supportTextures.push_back(GraphicsFormat::GraphicsFormatBC1RGBASRGBBlock);
	_supportTextures.push_back(GraphicsFormat::GraphicsFormatBC3SRGBBlock);
	_supportTextures.push_back(GraphicsFormat::GraphicsFormatB8G8R8SRGB);
	_supportTextures.push_back(GraphicsFormat::GraphicsFormatB8G8R8A8SRGB);
	_supportTextures.push_back(GraphicsFormat::GraphicsFormatA8B8G8R8SRGBPack32);
	_supportTextures.push_back(GraphicsFormat::GraphicsFormatA8B8G8R8SRGBPack32);
	_supportTextures.push_back(GraphicsFormat::GraphicsFormatA8B8G8R8SRGBPack32);
	_supportTextures.push_back(GraphicsFormat::GraphicsFormatA8B8G8R8SRGBPack32);
	_supportTextures.push_back(GraphicsFormat::GraphicsFormatR8UNorm);
	_supportTextures.push_back(GraphicsFormat::GraphicsFormatR16UNorm);
	_supportTextures.push_back(GraphicsFormat::GraphicsFormatR16UInt);
	_supportTextures.push_back(GraphicsFormat::GraphicsFormatR16SInt);
	_supportTextures.push_back(GraphicsFormat::GraphicsFormatR16SFloat);
	_supportTextures.push_back(GraphicsFormat::GraphicsFormatR16G16UNorm);
	_supportTextures.push_back(GraphicsFormat::GraphicsFormatR16G16UInt);
	_supportTextures.push_back(GraphicsFormat::GraphicsFormatR16G16SInt);
	_supportTextures.push_back(GraphicsFormat::GraphicsFormatR16G16SFloat);
	_supportTextures.push_back(GraphicsFormat::GraphicsFormatR16G16B16UNorm);
	_supportTextures.push_back(GraphicsFormat::GraphicsFormatR32G32SInt);
	_supportTextures.push_back(GraphicsFormat::GraphicsFormatR32UInt);
	_supportTextures.push_back(GraphicsFormat::GraphicsFormatR32SInt);
	_supportTextures.push_back(GraphicsFormat::GraphicsFormatR32SFloat);
	_supportTextures.push_back(GraphicsFormat::GraphicsFormatR32G32UInt);
	_supportTextures.push_back(GraphicsFormat::GraphicsFormatR32G32SFloat);
	_supportTextures.push_back(GraphicsFormat::GraphicsFormatR8SNorm);
	_supportTextures.push_back(GraphicsFormat::GraphicsFormatR8G8SNorm);
	_supportTextures.push_back(GraphicsFormat::GraphicsFormatR8G8B8SNorm);
	_supportTextures.push_back(GraphicsFormat::GraphicsFormatR8G8B8A8SNorm);
	_supportTextures.push_back(GraphicsFormat::GraphicsFormatR16SNorm);
	_supportTextures.push_back(GraphicsFormat::GraphicsFormatR16G16SNorm);
	_supportTextures.push_back(GraphicsFormat::GraphicsFormatR16G16B16SNorm);
	_supportTextures.push_back(GraphicsFormat::GraphicsFormatR16G16B16A16SNorm);
	_supportTextures.push_back(GraphicsFormat::GraphicsFormatB8G8R8SNorm);
	_supportTextures.push_back(GraphicsFormat::GraphicsFormatB8G8R8A8SNorm);
	_supportTextures.push_back(GraphicsFormat::GraphicsFormatA8B8G8R8SNormPack32);
	_supportTextures.push_back(GraphicsFormat::GraphicsFormatBC1RGBUNormBlock);
	_supportTextures.push_back(GraphicsFormat::GraphicsFormatBC1RGBAUNormBlock);
	_supportTextures.push_back(GraphicsFormat::GraphicsFormatBC3UNormBlock);
	_supportTextures.push_back(GraphicsFormat::GraphicsFormatBC5UNormBlock);
	_supportTextures.push_back(GraphicsFormat::GraphicsFormatR5G6B5UNormPack16);
	_supportTextures.push_back(GraphicsFormat::GraphicsFormatB5G6R5UNormPack16);
	_supportTextures.push_back(GraphicsFormat::GraphicsFormatETC2R8G8B8UNormBlock);
	_supportTextures.push_back(GraphicsFormat::GraphicsFormatETC2R8G8B8A1UNormBlock);
	_supportTextures.push_back(GraphicsFormat::GraphicsFormatETC2R8G8B8A8UNormBlock);
	_supportTextures.push_back(GraphicsFormat::GraphicsFormatETC2R8G8B8SRGBBlock);
	_supportTextures.push_back(GraphicsFormat::GraphicsFormatETC2R8G8B8A1SRGBBlock);
	_supportTextures.push_back(GraphicsFormat::GraphicsFormatETC2R8G8B8A8SRGBBlock);
	_supportTextures.push_back(GraphicsFormat::GraphicsFormatEACR11UNormBlock);
	_supportTextures.push_back(GraphicsFormat::GraphicsFormatEACR11SNormBlock);
	_supportTextures.push_back(GraphicsFormat::GraphicsFormatEACR11G11UNormBlock);
	_supportTextures.push_back(GraphicsFormat::GraphicsFormatEACR11G11SNormBlock);
	_supportTextures.push_back(GraphicsFormat::GraphicsFormatASTC4x4UNormBlock);
	_supportTextures.push_back(GraphicsFormat::GraphicsFormatASTC4x4SRGBBlock);
	_supportTextures.push_back(GraphicsFormat::GraphicsFormatASTC5x4UNormBlock);
	_supportTextures.push_back(GraphicsFormat::GraphicsFormatASTC5x4SRGBBlock);
	_supportTextures.push_back(GraphicsFormat::GraphicsFormatASTC5x5UNormBlock);
	_supportTextures.push_back(GraphicsFormat::GraphicsFormatASTC5x5SRGBBlock);
	_supportTextures.push_back(GraphicsFormat::GraphicsFormatASTC6x5UNormBlock);
	_supportTextures.push_back(GraphicsFormat::GraphicsFormatASTC6x5SRGBBlock);
	_supportTextures.push_back(GraphicsFormat::GraphicsFormatASTC6x6UNormBlock);
	_supportTextures.push_back(GraphicsFormat::GraphicsFormatASTC6x6SRGBBlock);
	_supportTextures.push_back(GraphicsFormat::GraphicsFormatASTC8x5UNormBlock);
	_supportTextures.push_back(GraphicsFormat::GraphicsFormatASTC8x5SRGBBlock);
	_supportTextures.push_back(GraphicsFormat::GraphicsFormatASTC8x6UNormBlock);
	_supportTextures.push_back(GraphicsFormat::GraphicsFormatASTC8x6SRGBBlock);
	_supportTextures.push_back(GraphicsFormat::GraphicsFormatASTC8x8UNormBlock);
	_supportTextures.push_back(GraphicsFormat::GraphicsFormatASTC8x8SRGBBlock);
	_supportTextures.push_back(GraphicsFormat::GraphicsFormatASTC10x5UNormBlock);
	_supportTextures.push_back(GraphicsFormat::GraphicsFormatASTC10x5SRGBBlock);
	_supportTextures.push_back(GraphicsFormat::GraphicsFormatASTC10x6UNormBlock);
	_supportTextures.push_back(GraphicsFormat::GraphicsFormatASTC10x6SRGBBlock);
	_supportTextures.push_back(GraphicsFormat::GraphicsFormatASTC10x8UNormBlock);
	_supportTextures.push_back(GraphicsFormat::GraphicsFormatASTC10x8SRGBBlock);
	_supportTextures.push_back(GraphicsFormat::GraphicsFormatASTC10x10UNormBlock);
	_supportTextures.push_back(GraphicsFormat::GraphicsFormatASTC10x10SRGBBlock);
	_supportTextures.push_back(GraphicsFormat::GraphicsFormatASTC12x10UNormBlock);
	_supportTextures.push_back(GraphicsFormat::GraphicsFormatASTC12x10SRGBBlock);
	_supportTextures.push_back(GraphicsFormat::GraphicsFormatASTC12x12UNormBlock);
	_supportTextures.push_back(GraphicsFormat::GraphicsFormatASTC12x12SRGBBlock);

	return true;
}

bool
VulkanDeviceContext::initTextureDimSupports() noexcept
{
	_supportTextureDims.push_back(GraphicsTextureDim::GraphicsTextureDim2D);
	_supportTextureDims.push_back(GraphicsTextureDim::GraphicsTextureDim2DArray);
	_supportTextureDims.push_back(GraphicsTextureDim::GraphicsTextureDim3D);
	_supportTextureDims.push_back(GraphicsTextureDim::GraphicsTextureDimCube);
	_supportTextureDims.push_back(GraphicsTextureDim::GraphicsTextureDimCubeArray);

	return true;
}

bool
VulkanDeviceContext::initVertexSupports() noexcept
{
	_supportAttribute.push_back(GraphicsFormat::GraphicsFormatR8SInt);
	_supportAttribute.push_back(GraphicsFormat::GraphicsFormatR8UInt);
	_supportAttribute.push_back(GraphicsFormat::GraphicsFormatR8UNorm);
	_supportAttribute.push_back(GraphicsFormat::GraphicsFormatR8SNorm);
	_supportAttribute.push_back(GraphicsFormat::GraphicsFormatR8G8SInt);
	_supportAttribute.push_back(GraphicsFormat::GraphicsFormatR8G8UInt);
	_supportAttribute.push_back(GraphicsFormat::GraphicsFormatR8G8SNorm);
	_supportAttribute.push_back(GraphicsFormat::GraphicsFormatR8G8UNorm);
	_supportAttribute.push_back(GraphicsFormat::GraphicsFormatR8G8B8SInt);
	_supportAttribute.push_back(GraphicsFormat::GraphicsFormatR8G8B8UInt);
	_supportAttribute.push_back(GraphicsFormat::GraphicsFormatR8G8B8SNorm);
	_supportAttribute.push_back(GraphicsFormat::GraphicsFormatR8G8B8UNorm);
	_supportAttribute.push_back(GraphicsFormat::GraphicsFormatR8G8B8A8SInt);
	_supportAttribute.push_back(GraphicsFormat::GraphicsFormatR8G8B8A8UInt);
	_supportAttribute.push_back(GraphicsFormat::GraphicsFormatR8G8B8A8SNorm);
	_supportAttribute.push_back(GraphicsFormat::GraphicsFormatR8G8B8A8UNorm);
	_supportAttribute.push_back(GraphicsFormat::GraphicsFormatR16SInt);
	_supportAttribute.push_back(GraphicsFormat::GraphicsFormatR16UInt);
	_supportAttribute.push_back(GraphicsFormat::GraphicsFormatR16UNorm);
	_supportAttribute.push_back(GraphicsFormat::GraphicsFormatR16SNorm);
	_supportAttribute.push_back(GraphicsFormat::GraphicsFormatR16G16SInt);
	_supportAttribute.push_back(GraphicsFormat::GraphicsFormatR16G16UInt);
	_supportAttribute.push_back(GraphicsFormat::GraphicsFormatR16G16SNorm);
	_supportAttribute.push_back(GraphicsFormat::GraphicsFormatR16G16UNorm);
	_supportAttribute.push_back(GraphicsFormat::GraphicsFormatR16G16B16SInt);
	_supportAttribute.push_back(GraphicsFormat::GraphicsFormatR16G16B16UInt);
	_supportAttribute.push_back(GraphicsFormat::GraphicsFormatR16G16B16SNorm);
	_supportAttribute.push_back(GraphicsFormat::GraphicsFormatR16G16B16UNorm);
	_supportAttribute.push_back(GraphicsFormat::GraphicsFormatR16G16B16A16SInt);
	_supportAttribute.push_back(GraphicsFormat::GraphicsFormatR16G16B16A16UInt);
	_supportAttribute.push_back(GraphicsFormat::GraphicsFormatR16G16B16A16SNorm);
	_supportAttribute.push_back(GraphicsFormat::GraphicsFormatR16G16B16A16UNorm);
	_supportAttribute.push_back(GraphicsFormat::GraphicsFormatR32SInt);
	_supportAttribute.push_back(GraphicsFormat::GraphicsFormatR32UInt);
	_supportAttribute.push_back(GraphicsFormat::GraphicsFormatR32G32SInt);
	_supportAttribute.push_back(GraphicsFormat::GraphicsFormatR32G32UInt);
	_supportAttribute.push_back(GraphicsFormat::GraphicsFormatR32G32B32SInt);
	_supportAttribute.push_back(GraphicsFormat::GraphicsFormatR32G32B32UInt);
	_supportAttribute.push_back(GraphicsFormat::GraphicsFormatR32G32B32A32SInt);
	_supportAttribute.push_back(GraphicsFormat::GraphicsFormatR32G32B32A32UInt);
	_supportAttribute.push_back(GraphicsFormat::GraphicsFormatR32SFloat);
	_supportAttribute.push_back(GraphicsFormat::GraphicsFormatR32G32SFloat);
	_supportAttribute.push_back(GraphicsFormat::GraphicsFormatR32G32B32SFloat);
	_supportAttribute.push_back(GraphicsFormat::GraphicsFormatR32G32B32A32SFloat);
	_supportAttribute.push_back(GraphicsFormat::GraphicsFormatR64SInt);
	_supportAttribute.push_back(GraphicsFormat::GraphicsFormatR64UInt);
	_supportAttribute.push_back(GraphicsFormat::GraphicsFormatR64G64SInt);
	_supportAttribute.push_back(GraphicsFormat::GraphicsFormatR64G64UInt);
	_supportAttribute.push_back(GraphicsFormat::GraphicsFormatR64G64B64SInt);
	_supportAttribute.push_back(GraphicsFormat::GraphicsFormatR64G64B64UInt);
	_supportAttribute.push_back(GraphicsFormat::GraphicsFormatR64G64B64A64SInt);
	_supportAttribute.push_back(GraphicsFormat::GraphicsFormatR64G64B64A64UInt);
	_supportAttribute.push_back(GraphicsFormat::GraphicsFormatB10G11R11UFloatPack32);
	_supportAttribute.push_back(GraphicsFormat::GraphicsFormatA2R10G10B10SIntPack32);
	_supportAttribute.push_back(GraphicsFormat::GraphicsFormatA2R10G10B10UIntPack32);
	_supportAttribute.push_back(GraphicsFormat::GraphicsFormatA2R10G10B10UNormPack32);
	_supportAttribute.push_back(GraphicsFormat::GraphicsFormatA2B10G10R10SIntPack32);
	_supportAttribute.push_back(GraphicsFormat::GraphicsFormatA2B10G10R10UIntPack32);
	_supportAttribute.push_back(GraphicsFormat::GraphicsFormatA2B10G10R10UNormPack32);
	_supportAttribute.push_back(GraphicsFormat::GraphicsFormatB8G8R8SInt);
	_supportAttribute.push_back(GraphicsFormat::GraphicsFormatB8G8R8UInt);
	_supportAttribute.push_back(GraphicsFormat::GraphicsFormatB8G8R8SNorm);
	_supportAttribute.push_back(GraphicsFormat::GraphicsFormatB8G8R8UNorm);
	_supportAttribute.push_back(GraphicsFormat::GraphicsFormatB8G8R8A8SInt);
	_supportAttribute.push_back(GraphicsFormat::GraphicsFormatB8G8R8A8UInt);
	_supportAttribute.push_back(GraphicsFormat::GraphicsFormatB8G8R8A8SNorm);
	_supportAttribute.push_back(GraphicsFormat::GraphicsFormatB8G8R8A8UNorm);

	return true;
}

bool
VulkanDeviceContext::initShaderSupports() noexcept
{
	_supportShaders.push_back(GraphicsShaderStageFlagBits::GraphicsShaderStageVertexBit);
	_supportShaders.push_back(GraphicsShaderStageFlagBits::GraphicsShaderStageFragmentBit);
	_supportShaders.push_back(GraphicsShaderStageFlagBits::GraphicsShaderStageGeometryBit);
	_supportShaders.push_back(GraphicsShaderStageFlagBits::GraphicsShaderStageComputeBit);
	_supportShaders.push_back(GraphicsShaderStageFlagBits::GraphicsShaderStageTessControlBit);
	_supportShaders.push_back(GraphicsShaderStageFlagBits::GraphicsShaderStageTessEvaluationBit);
	return true;
}

void
VulkanDeviceContext::setDevice(GraphicsDevicePtr device) noexcept
{
	_device = device->downcast_pointer<VulkanDevice>();
}

GraphicsDevicePtr
VulkanDeviceContext::getDevice() noexcept
{
	return _device.lock();
}

_NAME_END