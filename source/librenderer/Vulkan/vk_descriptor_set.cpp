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
#include "vk_descriptor_set.h"
#include "vk_descriptor_set_layout.h"
#include "vk_descriptor_pool.h"
#include "vk_device.h"

_NAME_BEGIN

__ImplementSubClass(VulkanDescriptorSet, GraphicsDescriptorSet, "VulkanDescriptorSet")

VulkanDescriptorSet::VulkanDescriptorSet() noexcept
	: _vkDescriptorSet(VK_NULL_HANDLE)
{
}

VulkanDescriptorSet::~VulkanDescriptorSet() noexcept
{
	this->close();
}

bool
VulkanDescriptorSet::setup(const GraphicsDescriptorSetDesc& descriptorSetDesc) noexcept
{
	assert(_vkDescriptorSet == VK_NULL_HANDLE);
	assert(descriptorSetDesc.getGraphicsDescriptorPool());
	assert(descriptorSetDesc.getGraphicsDescriptorSetLayout());

	auto descriptorPool = descriptorSetDesc.getGraphicsDescriptorPool();
	auto descriptorSetLayout = descriptorSetDesc.getGraphicsDescriptorSetLayout();

	VkDescriptorPool poolHandle = descriptorPool->downcast<VulkanDescriptorPool>()->getDescriptorPool();
	VkDescriptorSetLayout layoutHandle = descriptorSetLayout->downcast<VulkanDescriptorSetLayout>()->getDescriptorSetLayout();

	VkDescriptorSetAllocateInfo info;
	info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
	info.pNext = nullptr;
	info.descriptorSetCount = 1;
	info.pSetLayouts = &layoutHandle;
	info.descriptorPool = poolHandle;

	if (vkAllocateDescriptorSets(this->getDevice()->downcast<VulkanDevice>()->getDevice(), &info, &_vkDescriptorSet) != VK_SUCCESS)
		return false;

	const auto& uniforms = descriptorSetLayout->getGraphicsDescriptorSetLayoutDesc().getUniformComponents();
	for (const auto& it : uniforms)
	{
		VkWriteDescriptorSet write;
		write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		write.pNext = nullptr;
		write.descriptorType = VulkanTypes::asDescriptorType(it->getType());
		write.descriptorCount = 1;
		write.dstSet = _vkDescriptorSet;
		write.dstArrayElement = 0;
		write.dstBinding = it->getBindingPoint();
		write.pBufferInfo = nullptr;
		write.pImageInfo = nullptr;
		write.pTexelBufferView = nullptr;

		_writes.push_back(write);
	}

	//vkUpdateDescriptorSets(this->getDevice()->downcast<VulkanDevice>()->getDevice(), _writes.size(), _writes.data(), 0, nullptr);

	_descriptorSetDesc = descriptorSetDesc;
	return true;
}

void
VulkanDescriptorSet::close() noexcept
{
	if (_vkDescriptorSet != VK_NULL_HANDLE)
	{
		vkFreeDescriptorSets(
			this->getDevice()->downcast<VulkanDevice>()->getDevice(), 
			_descriptorSetDesc.getGraphicsDescriptorPool()->downcast<VulkanDescriptorPool>()->getDescriptorPool(),
			1,
			&_vkDescriptorSet);

		_vkDescriptorSet = VK_NULL_HANDLE;
	}
}

VkDescriptorSet
VulkanDescriptorSet::getDescriptorSet() const noexcept
{
	return _vkDescriptorSet;
}

void 
VulkanDescriptorSet::setDevice(GraphicsDevicePtr device) noexcept
{
	_device = device;
}

GraphicsDevicePtr
VulkanDescriptorSet::getDevice() noexcept
{
	return _device.lock();
}

const GraphicsDescriptorSetDesc& 
VulkanDescriptorSet::getGraphicsDescriptorSetDesc() const noexcept
{
	return _descriptorSetDesc;
}

_NAME_END