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
#include "vk_shader.h"
#include "vk_graphics_data.h"
#include "vk_system.h"

_NAME_BEGIN

__ImplementSubClass(VulkanGraphicsUniformSet, GraphicsUniformSet, "VulkanGraphicsUniformSet")
__ImplementSubClass(VulkanDescriptorSet, GraphicsDescriptorSet, "VulkanDescriptorSet")

VulkanGraphicsUniformSet::VulkanGraphicsUniformSet() noexcept
{
}

VulkanGraphicsUniformSet::~VulkanGraphicsUniformSet() noexcept
{
}

void
VulkanGraphicsUniformSet::setType(GraphicsUniformType type) noexcept
{
	_variant.setType(type);
}

GraphicsUniformType
VulkanGraphicsUniformSet::getType() const noexcept
{
	return _variant.getType();
}

void
VulkanGraphicsUniformSet::assign(bool value) noexcept
{
	_variant.assign(value);
}

void
VulkanGraphicsUniformSet::assign(int value) noexcept
{
	_variant.assign(value);
}

void
VulkanGraphicsUniformSet::assign(const int2& value) noexcept
{
	_variant.assign(value);
}

void
VulkanGraphicsUniformSet::assign(const int3& value) noexcept
{
	_variant.assign(value);
}

void
VulkanGraphicsUniformSet::assign(const int4& value) noexcept
{
	_variant.assign(value);
}

void
VulkanGraphicsUniformSet::assign(uint1 value) noexcept
{
	_variant.assign(value);
}

void
VulkanGraphicsUniformSet::assign(const uint2& value) noexcept
{
	_variant.assign(value);
}

void
VulkanGraphicsUniformSet::assign(const uint3& value) noexcept
{
	_variant.assign(value);
}

void
VulkanGraphicsUniformSet::assign(const uint4& value) noexcept
{
	_variant.assign(value);
}

void
VulkanGraphicsUniformSet::assign(float value) noexcept
{
	_variant.assign(value);
}

void
VulkanGraphicsUniformSet::assign(const float2& value) noexcept
{
	_variant.assign(value);
}

void
VulkanGraphicsUniformSet::assign(const float3& value) noexcept
{
	_variant.assign(value);
}

void
VulkanGraphicsUniformSet::assign(const float4& value) noexcept
{
	_variant.assign(value);
}

void
VulkanGraphicsUniformSet::assign(const float3x3& value) noexcept
{
	_variant.assign(value);
}

void
VulkanGraphicsUniformSet::assign(const float4x4& value) noexcept
{
	_variant.assign(value);
}

void
VulkanGraphicsUniformSet::assign(const std::vector<int1>& value) noexcept
{
	_variant.assign(value);
}

void
VulkanGraphicsUniformSet::assign(const std::vector<int2>& value) noexcept
{
	_variant.assign(value);
}

void
VulkanGraphicsUniformSet::assign(const std::vector<int3>& value) noexcept
{
	_variant.assign(value);
}

void
VulkanGraphicsUniformSet::assign(const std::vector<int4>& value) noexcept
{
	_variant.assign(value);
}

void
VulkanGraphicsUniformSet::assign(const std::vector<uint1>& value) noexcept
{
	_variant.assign(value);
}

void
VulkanGraphicsUniformSet::assign(const std::vector<uint2>& value) noexcept
{
	_variant.assign(value);
}

void
VulkanGraphicsUniformSet::assign(const std::vector<uint3>& value) noexcept
{
	_variant.assign(value);
}

void
VulkanGraphicsUniformSet::assign(const std::vector<uint4>& value) noexcept
{
	_variant.assign(value);
}

void
VulkanGraphicsUniformSet::assign(const std::vector<float1>& value) noexcept
{
	_variant.assign(value);
}

void
VulkanGraphicsUniformSet::assign(const std::vector<float2>& value) noexcept
{
	_variant.assign(value);
}

void
VulkanGraphicsUniformSet::assign(const std::vector<float3>& value) noexcept
{
	_variant.assign(value);
}

void
VulkanGraphicsUniformSet::assign(const std::vector<float4>& value) noexcept
{
	_variant.assign(value);
}

void
VulkanGraphicsUniformSet::assign(const std::vector<float3x3>& value) noexcept
{
	_variant.assign(value);
}

void
VulkanGraphicsUniformSet::assign(const std::vector<float4x4>& value) noexcept
{
	_variant.assign(value);
}

void
VulkanGraphicsUniformSet::assign(GraphicsTexturePtr texture, GraphicsSamplerPtr sampler) noexcept
{
	_variant.assign(texture, sampler);
}

bool
VulkanGraphicsUniformSet::getBool() const noexcept
{
	return _variant.getBool();
}

int
VulkanGraphicsUniformSet::getInt() const noexcept
{
	return _variant.getInt();
}

const int2&
VulkanGraphicsUniformSet::getInt2() const noexcept
{
	return _variant.getInt2();
}

const int3&
VulkanGraphicsUniformSet::getInt3() const noexcept
{
	return _variant.getInt3();
}

const int4&
VulkanGraphicsUniformSet::getInt4() const noexcept
{
	return _variant.getInt4();
}

uint
VulkanGraphicsUniformSet::getUInt() const noexcept
{
	return _variant.getUInt();
}

const uint2&
VulkanGraphicsUniformSet::getUInt2() const noexcept
{
	return _variant.getUInt2();
}

const uint3&
VulkanGraphicsUniformSet::getUInt3() const noexcept
{
	return _variant.getUInt3();
}

const uint4&
VulkanGraphicsUniformSet::getUInt4() const noexcept
{
	return _variant.getUInt4();
}

float
VulkanGraphicsUniformSet::getFloat() const noexcept
{
	return _variant.getFloat();
}

const float2&
VulkanGraphicsUniformSet::getFloat2() const noexcept
{
	return _variant.getFloat2();
}

const float3&
VulkanGraphicsUniformSet::getFloat3() const noexcept
{
	return _variant.getFloat3();
}

const float4&
VulkanGraphicsUniformSet::getFloat4() const noexcept
{
	return _variant.getFloat4();
}

const float3x3&
VulkanGraphicsUniformSet::getFloat3x3() const noexcept
{
	return _variant.getFloat3x3();
}

const float4x4&
VulkanGraphicsUniformSet::getFloat4x4() const noexcept
{
	return _variant.getFloat4x4();
}

const std::vector<int1>&
VulkanGraphicsUniformSet::getIntArray() const noexcept
{
	return _variant.getIntArray();
}

const std::vector<int2>&
VulkanGraphicsUniformSet::getInt2Array() const noexcept
{
	return _variant.getInt2Array();
}

const std::vector<int3>&
VulkanGraphicsUniformSet::getInt3Array() const noexcept
{
	return _variant.getInt3Array();
}

const std::vector<int4>&
VulkanGraphicsUniformSet::getInt4Array() const noexcept
{
	return _variant.getInt4Array();
}

const std::vector<uint1>&
VulkanGraphicsUniformSet::getUIntArray() const noexcept
{
	return _variant.getUIntArray();
}

const std::vector<uint2>&
VulkanGraphicsUniformSet::getUInt2Array() const noexcept
{
	return _variant.getUInt2Array();
}

const std::vector<uint3>&
VulkanGraphicsUniformSet::getUInt3Array() const noexcept
{
	return _variant.getUInt3Array();
}

const std::vector<uint4>&
VulkanGraphicsUniformSet::getUInt4Array() const noexcept
{
	return _variant.getUInt4Array();
}

const std::vector<float1>&
VulkanGraphicsUniformSet::getFloatArray() const noexcept
{
	return _variant.getFloatArray();
}

const std::vector<float2>&
VulkanGraphicsUniformSet::getFloat2Array() const noexcept
{
	return _variant.getFloat2Array();
}

const std::vector<float3>&
VulkanGraphicsUniformSet::getFloat3Array() const noexcept
{
	return _variant.getFloat3Array();
}

const std::vector<float4>&
VulkanGraphicsUniformSet::getFloat4Array() const noexcept
{
	return _variant.getFloat4Array();
}

const std::vector<float3x3>&
VulkanGraphicsUniformSet::getFloat3x3Array() const noexcept
{
	return _variant.getFloat3x3Array();
}

const std::vector<float4x4>&
VulkanGraphicsUniformSet::getFloat4x4Array() const noexcept
{
	return _variant.getFloat4x4Array();
}

GraphicsTexturePtr
VulkanGraphicsUniformSet::getTexture() const noexcept
{
	return _variant.getTexture();
}

GraphicsSamplerPtr
VulkanGraphicsUniformSet::getTextureSampler() const noexcept
{
	return _variant.getTextureSampler();
}

void
VulkanGraphicsUniformSet::setGraphicsUniform(GraphicsUniformPtr uniform) noexcept
{
	_uniform = uniform;
}

GraphicsUniformPtr
VulkanGraphicsUniformSet::getGraphicsUniform() const noexcept
{
	return _uniform;
}

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
	assert(descriptorSetDesc.getGraphicsDescriptorPool());
	assert(descriptorSetDesc.getGraphicsDescriptorSetLayout());
	assert(_vkDescriptorSet == VK_NULL_HANDLE);

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
	{
		VK_PLATFORM_LOG("vkAllocateDescriptorSets() fail.");
		return false;
	}

	auto& descriptorSetLayoutDesc = descriptorSetDesc.getGraphicsDescriptorSetLayout()->getGraphicsDescriptorSetLayoutDesc();
	auto& uniformBlockLayouts = descriptorSetLayoutDesc.getUniformBlockComponents();
	for (auto& uniformBlockLayout : uniformBlockLayouts)
	{
		auto& name = uniformBlockLayout->getName();
		if (name != "Globals")
			continue;

		GraphicsDataDesc uniformBufferDesc;
		uniformBufferDesc.setType(GraphicsDataType::GraphicsDataTypeUniformBuffer);
		uniformBufferDesc.setStreamSize(uniformBlockLayout->getBlockSize());
		uniformBufferDesc.setUsage(GraphicsUsageFlags::GraphicsUsageFlagsReadBit | GraphicsUsageFlags::GraphicsUsageFlagsWriteBit | GraphicsUsageFlags::GraphicsUsageFlagsImmutableStorage);
		auto ubo = this->getDevice()->createGraphicsData(uniformBufferDesc);
		if (!ubo)
		{
			VK_PLATFORM_LOG("Can't create uniform buffer for %s", name);
			return false;
		}

		_globalBuffers.push_back(std::make_pair(uniformBlockLayout, ubo));

		auto& uniforms = uniformBlockLayout->getGraphicsUniforms();
		for (auto& uniform : uniforms)
		{
			auto uniformSet = std::make_shared<VulkanGraphicsUniformSet>();
			uniformSet->setType(uniformBlockLayout->getType());
			uniformSet->setGraphicsUniform(uniform);

			_activeUniformSets.push_back(uniformSet);
		}
	}

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

void 
VulkanDescriptorSet::update() noexcept
{
	for (const auto& it : _globalBuffers)
	{
		auto uniformBlock = it.first->downcast<VulkanGraphicsUniform>();
		auto data = it.second->downcast<VulkanGraphicsData>();

		VkDescriptorBufferInfo bufferInfo;
		bufferInfo.buffer = data->getBuffer();
		bufferInfo.offset = 0;
		bufferInfo.range = data->getGraphicsDataDesc().getStreamSize();

		VkWriteDescriptorSet write;
		write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		write.pNext = nullptr;
		write.descriptorType = VulkanTypes::asDescriptorType(uniformBlock->getType());
		write.descriptorCount = 1;
		write.dstSet = _vkDescriptorSet;
		write.dstArrayElement = 0;
		write.dstBinding = uniformBlock->getBindingPoint();
		write.pBufferInfo = &bufferInfo;
		write.pImageInfo = nullptr;
		write.pTexelBufferView = nullptr;

		_writes.push_back(write);
	}

	vkUpdateDescriptorSets(this->getDevice()->downcast<VulkanDevice>()->getDevice(), _writes.size(), _writes.data(), 0, nullptr);

	_writes.clear();
}

VkDescriptorSet
VulkanDescriptorSet::getDescriptorSet() const noexcept
{
	return _vkDescriptorSet;
}

const GraphicsUniformSets&
VulkanDescriptorSet::getGraphicsUniformSets() const noexcept
{
	return _activeUniformSets;
}

const GraphicsDescriptorSetDesc&
VulkanDescriptorSet::getGraphicsDescriptorSetDesc() const noexcept
{
	return _descriptorSetDesc;
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

_NAME_END