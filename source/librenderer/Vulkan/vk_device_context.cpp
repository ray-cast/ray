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
	_commandList->renderBegin();

	if (_swapchain)
	{
		auto swapchaic = _swapchain->downcast_pointer<VulkanSwapchain>();
		swapchaic->acquireNextImage();

		_commandList->setFramebuffer(swapchaic->getSwapchainFramebuffers()[swapchaic->getSwapchainImageIndex()]);
	}
}

void
VulkanDeviceContext::renderEnd() noexcept
{
	_commandList->renderEnd();
	_commandQueue->executeCommandLists(&_commandList, 1);

	if (_swapchain)
		_commandQueue->present(&_swapchain, 1);

	_commandQueue->wait();
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
VulkanDeviceContext::setStencilCompareMask(GraphicsStencilFaceFlags face, std::uint32_t mask) noexcept
{
	_commandList->setStencilCompareMask(face, mask);
}

std::uint32_t
VulkanDeviceContext::getStencilCompareMask(GraphicsStencilFaceFlagBits face) noexcept
{
	return _commandList->getStencilCompareMask(face);
}

void
VulkanDeviceContext::setStencilReference(GraphicsStencilFaceFlags face, std::uint32_t reference) noexcept
{
	_commandList->setStencilReference(face, reference);
}

std::uint32_t
VulkanDeviceContext::getStencilReference(GraphicsStencilFaceFlagBits face) noexcept
{
	return _commandList->getStencilReference(face);
}

void
VulkanDeviceContext::setStencilWriteMask(GraphicsStencilFaceFlags face, std::uint32_t mask) noexcept
{
	_commandList->setStencilWriteMask(face, mask);
}

std::uint32_t
VulkanDeviceContext::getStencilWriteMask(GraphicsStencilFaceFlagBits face) noexcept
{
	return _commandList->getStencilWriteMask(face);
}

void 
VulkanDeviceContext::generateMipmap(const GraphicsTexturePtr& texture) noexcept
{
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
VulkanDeviceContext::setFramebufferClear(std::uint32_t i, GraphicsClearFlags flags, const float4& color, float depth, std::int32_t stencil) noexcept
{
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

	descriptorSet->downcast<VulkanDescriptorSet>()->update();
	_commandList->setDescriptorSet(descriptorSet);
}

GraphicsDescriptorSetPtr
VulkanDeviceContext::getDescriptorSet() const noexcept
{
	return _descriptorSet;
}

void
VulkanDeviceContext::setVertexBufferData(std::uint32_t i, GraphicsDataPtr data, std::intptr_t offset) noexcept
{
	assert(data);
	assert(data->isInstanceOf<VulkanGraphicsData>());
	assert(data->getGraphicsDataDesc().getType() == GraphicsDataType::GraphicsDataTypeStorageVertexBuffer);

	_vertexBuffer = data;
	_commandList->setVertexBuffers(&data, i, 1);
}

GraphicsDataPtr
VulkanDeviceContext::getVertexBufferData(std::uint32_t i) const noexcept
{
	return _vertexBuffer;
}

void
VulkanDeviceContext::setIndexBufferData(GraphicsDataPtr data, std::intptr_t offset, GraphicsIndexType indexType) noexcept
{
	assert(data);
	assert(data->isInstanceOf<VulkanGraphicsData>());
	assert(data->getGraphicsDataDesc().getType() == GraphicsDataType::GraphicsDataTypeStorageIndexBuffer);
	assert(indexType == GraphicsIndexType::GraphicsIndexTypeUInt16 || indexType == GraphicsIndexType::GraphicsIndexTypeUInt32);

	_indexBuffer = data;
	_commandList->setIndexBuffer(data, offset, indexType);
}

GraphicsDataPtr
VulkanDeviceContext::getIndexBufferData() const noexcept
{
	return _indexBuffer;
}

void 
VulkanDeviceContext::draw(std::uint32_t numVertices, std::uint32_t numInstances, std::uint32_t startVertice, std::uint32_t startInstances) noexcept
{
	_commandList->draw(numVertices, numInstances, startVertice, startInstances);
}

void 
VulkanDeviceContext::drawIndexed(std::uint32_t numIndices, std::uint32_t numInstances, std::uint32_t startIndice, std::uint32_t startVertice, std::uint32_t startInstances) noexcept
{
	_commandList->drawIndexed(numIndices, numInstances, startIndice, startVertice, startInstances);
}

void
VulkanDeviceContext::present() noexcept
{
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