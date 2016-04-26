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
#include "vk_command_pool.h"
#include "vk_command_list.h"
#include "vk_device.h"
#include "vk_system.h"

_NAME_BEGIN

__ImplementSubClass(VulkanCommandPool, GraphicsCommandPool, "VulkanCommandPool")

VulkanCommandPool::VulkanCommandPool() noexcept
	: _vkCommandPool(VK_NULL_HANDLE)
{
}

VulkanCommandPool::~VulkanCommandPool() noexcept
{
	this->close();
}

bool
VulkanCommandPool::setup(const GraphicsCommandPoolDesc& commandPooldesc) noexcept
{
	std::uint32_t queueCount = 0;
	vkGetPhysicalDeviceQueueFamilyProperties(this->getDevice()->downcast<VulkanDevice>()->getPhysicsDevice(), &queueCount, 0);
	if (queueCount == 0)
		return false;

	std::vector<VkQueueFamilyProperties> props(queueCount);
	vkGetPhysicalDeviceQueueFamilyProperties(this->getDevice()->downcast<VulkanDevice>()->getPhysicsDevice(), &queueCount, &props[0]);

	std::uint32_t graphicsQueueNodeIndex = UINT32_MAX;
	for (std::uint32_t i = 0; i < queueCount; i++)
	{
		if (commandPooldesc.getCommandListType() == GraphicsCommandType::GraphicsCommandTypeGraphics)
		{
			if (props[i].queueFlags & VK_QUEUE_GRAPHICS_BIT)
			{
				graphicsQueueNodeIndex = i;
				break;
			}
		}
		else if (commandPooldesc.getCommandListType() == GraphicsCommandType::GraphicsCommandTypeCompute)
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

	VkCommandPoolCreateInfo info;
	info.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
	info.pNext = NULL;
	info.queueFamilyIndex = graphicsQueueNodeIndex;
	info.flags = 0;

	auto poolFlags = commandPooldesc.getCommandFlags();
	if (poolFlags & GraphicsCommandPoolFlags::GraphicsCommandPoolTransientBit)
		info.flags |= VK_COMMAND_POOL_CREATE_TRANSIENT_BIT;

	if (poolFlags & GraphicsCommandPoolFlags::GraphicsCommandPoolResetCommandBuffer)
		info.flags |= VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;

	if (vkCreateCommandPool(this->getDevice()->downcast<VulkanDevice>()->getDevice(), &info, nullptr, &_vkCommandPool) > 0)
	{
		VK_PLATFORM_LOG("vkCreateCommandPool() fail.");
		return false;
	}

	_commandPoolDesc = commandPooldesc;
	return true;
}

void
VulkanCommandPool::close() noexcept
{
	if (_vkCommandPool != VK_NULL_HANDLE)
	{
		vkDestroyCommandPool(this->getDevice()->downcast<VulkanDevice>()->getDevice(), _vkCommandPool, nullptr);
		_vkCommandPool = VK_NULL_HANDLE;
	}
}

VkCommandPool
VulkanCommandPool::getInstance() const noexcept
{
	return _vkCommandPool;
}

void
VulkanCommandPool::setDevice(GraphicsDevicePtr device) noexcept
{
	_device = device;
}

GraphicsDevicePtr
VulkanCommandPool::getDevice() noexcept
{
	return _device.lock();
}

const GraphicsCommandPoolDesc&
VulkanCommandPool::getGraphicsCommandPoolDesc() const noexcept
{
	return _commandPoolDesc;
}

_NAME_END