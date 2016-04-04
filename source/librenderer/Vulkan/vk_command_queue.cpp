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
#include "vk_command_queue.h"
#include "vk_command_list.h"
#include "vk_swapchain.h"
#include "vk_device.h"
#include "vk_semaphore.h"

_NAME_BEGIN

__ImplementSubClass(VulkanCommandQueue, GraphicsCommandQueue, "VulkanCommandQueue")

VulkanCommandQueue::VulkanCommandQueue() noexcept
	: _vkQueue(VK_NULL_HANDLE)
{
}

VulkanCommandQueue::~VulkanCommandQueue() noexcept
{
}

bool
VulkanCommandQueue::setup(const GraphicsCommandQueueDesc& commandQueueDesc) noexcept
{
	auto device = this->getDevice()->downcast<VulkanDevice>();

	std::uint32_t queueCount = 0;
	vkGetPhysicalDeviceQueueFamilyProperties(device->getPhysicsDevice(), &queueCount, 0);
	if (queueCount == 0)
		return false;

	std::vector<VkQueueFamilyProperties> props(queueCount);
	vkGetPhysicalDeviceQueueFamilyProperties(device->getPhysicsDevice(), &queueCount, &props[0]);

	std::uint32_t graphicsQueueNodeIndex = UINT32_MAX;
	for (std::uint32_t i = 0; i < queueCount; i++)
	{
		if (commandQueueDesc.getCommandQueueType() == GraphicsCommandType::GraphicsCommandTypeGraphics)
		{
			if (props[i].queueFlags & VK_QUEUE_GRAPHICS_BIT)
			{
				graphicsQueueNodeIndex = i;
				break;
			}
		}
		else if (commandQueueDesc.getCommandQueueType() == GraphicsCommandType::GraphicsCommandTypeCompute)
		{
			if (props[i].queueFlags & VK_QUEUE_COMPUTE_BIT)
			{
				graphicsQueueNodeIndex = i;
				break;
			}
		}
	}

	if (graphicsQueueNodeIndex == UINT32_MAX)
	{
		VK_PLATFORM_LOG("Can't find compatible command type.");
		return false;
	}

	vkGetDeviceQueue(device->getDevice(), graphicsQueueNodeIndex, 0, &_vkQueue);

	_commandQueueDesc = commandQueueDesc;
	return true;
}

void 
VulkanCommandQueue::close() noexcept
{
}

void
VulkanCommandQueue::beginEvent() noexcept
{
}

void
VulkanCommandQueue::endEvent() noexcept
{
}

void
VulkanCommandQueue::wait() noexcept
{
	vkQueueWaitIdle(_vkQueue);
}

void 
VulkanCommandQueue::executeCommandLists(GraphicsCommandListPtr commandLists[], std::uint32_t count) noexcept
{
	VkSubmitInfo submit[VK_MAX_SUBMIT_BUFFER];
	VkCommandBuffer buffers[VK_MAX_SUBMIT_BUFFER];

	std::uint32_t submitCount = 0;

	for (std::uint32_t i = 0; i < count; i++, submitCount++)
	{
		submit[submitCount].sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
		submit[submitCount].pNext = 0;
		submit[submitCount].waitSemaphoreCount = 0;
		submit[submitCount].pWaitSemaphores = 0;
		submit[submitCount].pWaitDstStageMask = 0;
		submit[submitCount].commandBufferCount = 1;
		submit[submitCount].pCommandBuffers = &buffers[submitCount];
		submit[submitCount].signalSemaphoreCount = 0;
		submit[submitCount].pSignalSemaphores = 0;

		buffers[submitCount] = commandLists[i]->downcast<VulkanCommandList>()->getInstance();

		if (submitCount == VK_MAX_SUBMIT_BUFFER)
		{
			vkQueueSubmit(_vkQueue, submitCount, submit, VK_NULL_HANDLE);
			submitCount = 0;
		}
	}

	if (submitCount > 0)
	{
		vkQueueSubmit(_vkQueue, submitCount, submit, VK_NULL_HANDLE);
	}
}

void
VulkanCommandQueue::present(GraphicsSwapchainPtr canvas[], std::uint32_t count) noexcept
{
	VkSwapchainKHR swapchains[VK_MAX_PRESENT];
	VkSemaphore waitSemaphores[VK_MAX_PRESENT];

	std::uint32_t swapchainImage[VK_MAX_PRESENT];

	VkPresentInfoKHR present;
	present.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
	present.pNext = nullptr;
	present.waitSemaphoreCount = 0;
	present.pWaitSemaphores = waitSemaphores;
	present.swapchainCount = 0;
	present.pSwapchains = swapchains;
	present.pImageIndices = swapchainImage;
	present.pResults = nullptr;

	for (std::uint32_t i = 0; i < count; i++)
	{
		VkSwapchainKHR swapchain = canvas[i]->downcast<VulkanSwapchain>()->getSwapchain();
		VkSemaphore semaphore = canvas[i]->downcast<VulkanSwapchain>()->getSemaphore()->downcast<VulkanSemaphore>()->getSemaphore();

		if (swapchain != VK_NULL_HANDLE &&
			semaphore != VK_NULL_HANDLE)
		{
			swapchains[present.swapchainCount] = swapchain;
			swapchainImage[present.swapchainCount] = canvas[i]->downcast<VulkanSwapchain>()->getSwapchainImageIndex();

			waitSemaphores[present.waitSemaphoreCount] = semaphore;

			present.swapchainCount++;
			present.waitSemaphoreCount++;
		}

		if (present.swapchainCount == VK_MAX_PRESENT)
		{
			vkQueuePresentKHR(_vkQueue, &present);
			present.swapchainCount = 0;
		}
	}

	if (present.swapchainCount > 0)
	{
		vkQueuePresentKHR(_vkQueue, &present);
	}
}

void
VulkanCommandQueue::setDevice(GraphicsDevicePtr device) noexcept
{
	_device = device;
}

GraphicsDevicePtr
VulkanCommandQueue::getDevice() noexcept
{
	return _device.lock();
}

const GraphicsCommandQueueDesc& 
VulkanCommandQueue::getGraphicsCommandQueueDesc() const noexcept
{
	return _commandQueueDesc;
}

_NAME_END