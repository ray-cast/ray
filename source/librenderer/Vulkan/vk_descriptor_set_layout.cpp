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
#include "vk_descriptor_set_layout.h"
#include "vk_device.h"
#include "vk_system.h"

_NAME_BEGIN

__ImplementSubClass(VulkanDescriptorSetLayout, GraphicsDescriptorSetLayout, "VulkanDescriptorSetLayout")

VulkanDescriptorSetLayout::VulkanDescriptorSetLayout() noexcept
	: _vkDescriptorSetLayout(VK_NULL_HANDLE)
{
}

VulkanDescriptorSetLayout::~VulkanDescriptorSetLayout() noexcept
{
	this->close();
}

bool
VulkanDescriptorSetLayout::setup(const GraphicsDescriptorSetLayoutDesc& descriptorSetLayoutDesc) noexcept
{
	std::vector<VkDescriptorSetLayoutBinding> layouts;

	const auto& uniforms = descriptorSetLayoutDesc.getUniformComponents();
	for (const auto& it : uniforms)
	{
		auto type = it->getType();
		if (type == GraphicsUniformType::GraphicsUniformTypeStorageImage ||
			type == GraphicsUniformType::GraphicsUniformTypeSamplerImage ||
			type == GraphicsUniformType::GraphicsUniformTypeCombinedImageSampler ||
			type == GraphicsUniformType::GraphicsUniformTypeUniformBuffer ||
			type == GraphicsUniformType::GraphicsUniformTypeUniformBufferDynamic)
		{
			VkDescriptorSetLayoutBinding layout;
			layout.descriptorType = VulkanTypes::asDescriptorType(it->getType());
			layout.descriptorCount = 1;
			layout.pImmutableSamplers = nullptr;
			layout.binding = it->getBindingPoint();
			layout.stageFlags = VkShaderStageFlagBits::VK_SHADER_STAGE_ALL;

			layouts.push_back(layout);
		}
	}

	VkDescriptorSetLayoutCreateInfo info;
	info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
	info.pNext = nullptr;
	info.flags = 0;
	info.bindingCount = layouts.size();
	info.pBindings = layouts.data();

	if (vkCreateDescriptorSetLayout(_device.lock()->getDevice(), &info, nullptr, &_vkDescriptorSetLayout) != VK_SUCCESS)
	{
		VK_PLATFORM_LOG("vkCreateDescriptorSetLayout() fail.");
		return false;
	}

	_descriptorSetLayoutDesc = descriptorSetLayoutDesc;
	return true;
}

void
VulkanDescriptorSetLayout::close() noexcept
{
	if (_vkDescriptorSetLayout != VK_NULL_HANDLE)
	{
		vkDestroyDescriptorSetLayout(_device.lock()->getDevice(), _vkDescriptorSetLayout, nullptr);
		_vkDescriptorSetLayout = VK_NULL_HANDLE;
	}
}

VkDescriptorSetLayout
VulkanDescriptorSetLayout::getDescriptorSetLayout() const noexcept
{
	return _vkDescriptorSetLayout;
}

const GraphicsDescriptorSetLayoutDesc&
VulkanDescriptorSetLayout::getGraphicsDescriptorSetLayoutDesc() const noexcept
{
	return _descriptorSetLayoutDesc;
}

void
VulkanDescriptorSetLayout::setDevice(GraphicsDevicePtr device) noexcept
{
	_device = device->downcast_pointer<VulkanDevice>();
}

GraphicsDevicePtr
VulkanDescriptorSetLayout::getDevice() noexcept
{
	return _device.lock();
}

_NAME_END