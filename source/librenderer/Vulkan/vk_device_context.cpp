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
	auto device = this->getDevice()->downcast<VulkanDevice>();
	if (device)
	{
		_swapchain = desc.getSwapchain();
		if (!_swapchain)
			return false;

		GraphicsCommandQueueDesc queueDesc;
		queueDesc.setCommandQueueFlags(GraphicsCommandQueueFlags::GraphicsCommandQueueFlagsNone);
		queueDesc.setCommandQueueType(GraphicsCommandType::GraphicsCommandTypeGraphics);
		queueDesc.setCommandQueuePriority(GraphicsCommandQueuePriority::GraphicsCommandQueuePriorityNormal);
		queueDesc.setCommandQueueMask(0);

		GraphicsCommandPoolDesc poolDesc;
		poolDesc.setCommandListType(GraphicsCommandType::GraphicsCommandTypeGraphics);
		poolDesc.setCommandFlags(GraphicsCommandPoolFlags::GraphicsCommandPoolResetCommandBuffer);

		_commandQueue = device->createCommandQueue(queueDesc);
		_commandPool = device->createCommandPool(poolDesc);

		GraphicsCommandListDesc commandListDesc;
		commandListDesc.setGraphicsCommandPool(_commandPool);

		_commandList = device->createCommandList(commandListDesc);

		return true;
	}

	return false;
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
	auto swapchaic = _swapchain->downcast<VulkanSwapchain>();
	swapchaic->acquireNextImage();

	_commandList->renderBegin();
	_commandList->setFramebuffer(swapchaic->getSwapchainFramebuffers()[swapchaic->getSwapchainImageIndex()]);
}

void 
VulkanDeviceContext::renderEnd() noexcept
{
	_commandList->renderEnd();
}

void 
VulkanDeviceContext::setViewport(const Viewport& viewport) noexcept
{
	_commandList->setViewport(&viewport, 0, 1);
}

const Viewport& 
VulkanDeviceContext::getViewport() const noexcept
{
	return _viewports[0];
}

void 
VulkanDeviceContext::setScissor(const Scissor& scissor) noexcept
{
	_commandList->setScissor(&scissor, 0, 1);
}

const Scissor& 
VulkanDeviceContext::getScissor() const noexcept
{
	return _scissor[0];
}

void 
VulkanDeviceContext::setFramebuffer(GraphicsFramebufferPtr framebuffer) noexcept
{
	if (_framebuffer != framebuffer)
	{
		_commandList->setFramebuffer(framebuffer);
		_framebuffer = framebuffer;
	}
}

void 
VulkanDeviceContext::clearFramebuffer(GraphicsClearFlags flags, const float4& color, float depth, std::int32_t stencil) noexcept
{
}

void 
VulkanDeviceContext::clearFramebuffer(GraphicsClearFlags flags, const float4& color, float depth, std::int32_t stencil, std::size_t i) noexcept
{
}

void 
VulkanDeviceContext::discardFramebuffer() noexcept
{
}

void 
VulkanDeviceContext::blitFramebuffer(GraphicsFramebufferPtr src, const Viewport& v1, GraphicsFramebufferPtr dest, const Viewport& v2) noexcept
{
}

void 
VulkanDeviceContext::readFramebuffer(GraphicsFramebufferPtr source, GraphicsFormat pfd, std::size_t w, std::size_t h, void* data) noexcept
{
}

GraphicsFramebufferPtr 
VulkanDeviceContext::getFramebuffer() const noexcept
{
	return _framebuffer;
}

void 
VulkanDeviceContext::setRenderPipeline(GraphicsPipelinePtr pipeline) noexcept
{
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
	_commandList->setDescriptorSet(descriptorSet);
}

GraphicsDescriptorSetPtr
VulkanDeviceContext::getDescriptorSet() const noexcept
{
	return _descriptorSet;
}

void 
VulkanDeviceContext::setVertexBufferData(GraphicsDataPtr data) noexcept
{
}

GraphicsDataPtr
VulkanDeviceContext::getVertexBufferData() const noexcept
{
	return _vertexBuffer;
}

void 
VulkanDeviceContext::setIndexBufferData(GraphicsDataPtr data) noexcept
{
	_commandList->setIndexBuffer(data);
}

GraphicsDataPtr
VulkanDeviceContext::getIndexBufferData() const noexcept
{
	return _indexBuffer;
}

bool 
VulkanDeviceContext::updateBuffer(GraphicsDataPtr& data, void* buf, std::size_t cnt) noexcept
{
	return false;
}

void* 
VulkanDeviceContext::mapBuffer(GraphicsDataPtr& data, std::uint32_t access) noexcept
{
	return nullptr;
}

void 
VulkanDeviceContext::unmapBuffer(GraphicsDataPtr& data) noexcept
{
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
	_commandQueue->executeCommandLists(&_commandList, 1);
	_commandQueue->present(&_swapchain, 1);
	_commandQueue->wait();
}

void
VulkanDeviceContext::setDevice(GraphicsDevicePtr device) noexcept
{
	_device = device;
}

GraphicsDevicePtr
VulkanDeviceContext::getDevice() noexcept
{
	return _device.lock();
}

_NAME_END