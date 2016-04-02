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
#include "vk_descriptor_pool.h"
#include "vk_device.h"

_NAME_BEGIN

__ImplementSubClass(VulkanDescriptorPool, GraphicsDescriptorPool, "VulkanDescriptorPool")

VulkanDescriptorPool::VulkanDescriptorPool() noexcept
	: _vkDescriptorPool(VK_NULL_HANDLE)
{
}

VulkanDescriptorPool::~VulkanDescriptorPool() noexcept
{
	this->close();
}

bool
VulkanDescriptorPool::setup(const GraphicsDescriptorPoolDesc& descriptorPoolDesc) noexcept
{
	std::vector<VkDescriptorPoolSize> pool;
	for (const auto& it : descriptorPoolDesc.getGraphicsDescriptorPoolComponents())
	{
		VkDescriptorPoolSize poolSize;
		poolSize.type = VulkanTypes::asDescriptorType(it.getDescriptorType());
		poolSize.descriptorCount = it.getDescriptorNums();

		pool.push_back(poolSize);
	}

	VkDescriptorPoolCreateInfo info;
	info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
	info.pNext = nullptr;
	info.flags = 0;
	info.maxSets = descriptorPoolDesc.getMaxSets();
	info.poolSizeCount = pool.size();
	info.pPoolSizes = pool.data();

	if (vkCreateDescriptorPool(this->getDevice()->downcast<VulkanDevice>()->getDevice(), &info, nullptr, &_vkDescriptorPool) != VK_SUCCESS)
		return false;

	_descriptorPoolDesc = descriptorPoolDesc;
	return true;
}

void
VulkanDescriptorPool::close() noexcept
{
	if (_vkDescriptorPool != VK_NULL_HANDLE)
	{
		vkDestroyDescriptorPool(this->getDevice()->downcast<VulkanDevice>()->getDevice(), _vkDescriptorPool, nullptr);
		_vkDescriptorPool = VK_NULL_HANDLE;
	}
}

VkDescriptorPool
VulkanDescriptorPool::getDescriptorPool() const noexcept
{
	return _vkDescriptorPool;
}

void
VulkanDescriptorPool::setDevice(GraphicsDevicePtr device) noexcept
{
	_device = device;
}

GraphicsDevicePtr
VulkanDescriptorPool::getDevice() noexcept
{
	return _device.lock();
}

const GraphicsDescriptorPoolDesc&
VulkanDescriptorPool::getGraphicsDescriptorPoolDesc() const noexcept
{
	return _descriptorPoolDesc;
}

_NAME_END