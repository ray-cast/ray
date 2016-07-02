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
#include "vk_device.h"
#include "vk_shader.h"
#include "vk_texture.h"
#include "vk_graphics_data.h"
#include "vk_system.h"

_NAME_BEGIN

__ImplementSubClass(VulkanGraphicsUniformSet, GraphicsUniformSet, "VulkanGraphicsUniformSet")
__ImplementSubClass(VulkanDescriptorPool, GraphicsDescriptorPool, "VulkanDescriptorPool")
__ImplementSubClass(VulkanDescriptorSet, GraphicsDescriptorSet, "VulkanDescriptorSet")
__ImplementSubClass(VulkanDescriptorSetLayout, GraphicsDescriptorSetLayout, "VulkanDescriptorSetLayout")

VulkanGraphicsUniformSet::VulkanGraphicsUniformSet() noexcept
	: _needUpdate(true)
{
}

VulkanGraphicsUniformSet::~VulkanGraphicsUniformSet() noexcept
{
}

void
VulkanGraphicsUniformSet::needUpdate(bool needUpdate) noexcept
{
	_needUpdate = needUpdate;
}

bool
VulkanGraphicsUniformSet::needUpdate() const noexcept
{
	return _needUpdate;
}

void
VulkanGraphicsUniformSet::uniform1b(bool value) noexcept
{
	this->needUpdate(true);
	_variant.uniform1b(value);
}

void
VulkanGraphicsUniformSet::uniform1i(std::int32_t i1) noexcept
{
	this->needUpdate(true);
	_variant.uniform1i(i1);
}

void
VulkanGraphicsUniformSet::uniform2i(const int2& value) noexcept
{
	this->needUpdate(true);
	_variant.uniform2i(value);
}

void
VulkanGraphicsUniformSet::uniform2i(std::int32_t i1, std::int32_t i2) noexcept
{
	this->needUpdate(true);
	_variant.uniform2i(i1, i2);
}

void
VulkanGraphicsUniformSet::uniform3i(const int3& value) noexcept
{
	this->needUpdate(true);
	_variant.uniform3i(value);
}

void
VulkanGraphicsUniformSet::uniform3i(std::int32_t i1, std::int32_t i2, std::int32_t i3) noexcept
{
	this->needUpdate(true);
	_variant.uniform3i(i1, i2, i3);
}

void
VulkanGraphicsUniformSet::uniform4i(const int4& value) noexcept
{
	this->needUpdate(true);
	_variant.uniform4i(value);
}

void
VulkanGraphicsUniformSet::uniform4i(std::int32_t i1, std::int32_t i2, std::int32_t i3, std::int32_t i4) noexcept
{
	this->needUpdate(true);
	_variant.uniform4i(i1, i2, i3, i4);
}

void
VulkanGraphicsUniformSet::uniform1ui(std::uint32_t ui1) noexcept
{
	this->needUpdate(true);
	_variant.uniform1ui(ui1);
}

void
VulkanGraphicsUniformSet::uniform2ui(const uint2& value) noexcept
{
	this->needUpdate(true);
	_variant.uniform2ui(value);
}

void
VulkanGraphicsUniformSet::uniform2ui(std::uint32_t ui1, std::uint32_t ui2) noexcept
{
	this->needUpdate(true);
	_variant.uniform2ui(ui1, ui2);
}

void
VulkanGraphicsUniformSet::uniform3ui(const uint3& value) noexcept
{
	this->needUpdate(true);
	_variant.uniform3ui(value);
}

void
VulkanGraphicsUniformSet::uniform3ui(std::uint32_t ui1, std::uint32_t ui2, std::uint32_t ui3) noexcept
{
	this->needUpdate(true);
	_variant.uniform3ui(ui1, ui2, ui3);
}

void
VulkanGraphicsUniformSet::uniform4ui(const uint4& value) noexcept
{
	this->needUpdate(true);
	_variant.uniform4ui(value);
}

void
VulkanGraphicsUniformSet::uniform4ui(std::uint32_t ui1, std::uint32_t ui2, std::uint32_t ui3, std::uint32_t ui4) noexcept
{
	this->needUpdate(true);
	_variant.uniform4ui(ui1, ui2, ui3, ui4);
}

void
VulkanGraphicsUniformSet::uniform1f(float f1) noexcept
{
	this->needUpdate(true);
	_variant.uniform1f(f1);
}

void
VulkanGraphicsUniformSet::uniform2f(const float2& value) noexcept
{
	this->needUpdate(true);
	_variant.uniform2f(value);
}

void
VulkanGraphicsUniformSet::uniform2f(float f1, float f2) noexcept
{
	this->needUpdate(true);
	_variant.uniform2f(f1, f2);
}

void
VulkanGraphicsUniformSet::uniform3f(const float3& value) noexcept
{
	this->needUpdate(true);
	_variant.uniform3f(value);
}

void
VulkanGraphicsUniformSet::uniform3f(float f1, float f2, float f3) noexcept
{
	this->needUpdate(true);
	_variant.uniform3f(f1, f2, f3);
}

void
VulkanGraphicsUniformSet::uniform4f(const float4& value) noexcept
{
	this->needUpdate(true);
	_variant.uniform4f(value);
}

void
VulkanGraphicsUniformSet::uniform4f(float f1, float f2, float f3, float f4) noexcept
{
	this->needUpdate(true);
	_variant.uniform4f(f1, f2, f3, f4);
}

void
VulkanGraphicsUniformSet::uniform2fmat(const float2x2& value) noexcept
{
	this->needUpdate(true);
	_variant.uniform2fmat(value);
}

void
VulkanGraphicsUniformSet::uniform2fmat(const float* mat2) noexcept
{
	this->needUpdate(true);
	_variant.uniform2fmat(mat2);
}

void
VulkanGraphicsUniformSet::uniform3fmat(const float3x3& value) noexcept
{
	this->needUpdate(true);
	_variant.uniform3fmat(value);
}

void
VulkanGraphicsUniformSet::uniform3fmat(const float* mat3) noexcept
{
	this->needUpdate(true);
	_variant.uniform3fmat(mat3);
}

void
VulkanGraphicsUniformSet::uniform4fmat(const float4x4& value) noexcept
{
	this->needUpdate(true);
	_variant.uniform4fmat(value);
}

void
VulkanGraphicsUniformSet::uniform4fmat(const float* mat4) noexcept
{
	this->needUpdate(true);
	_variant.uniform4fmat(mat4);
}

void
VulkanGraphicsUniformSet::uniform1iv(const std::vector<int1>& value) noexcept
{
	this->needUpdate(true);
	_variant.uniform1iv(value);
}

void
VulkanGraphicsUniformSet::uniform1iv(std::size_t num, const std::int32_t* i1v) noexcept
{
	this->needUpdate(true);
	_variant.uniform1iv(num, i1v);
}

void
VulkanGraphicsUniformSet::uniform2iv(const std::vector<int2>& value) noexcept
{
	this->needUpdate(true);
	_variant.uniform2iv(value);
}

void
VulkanGraphicsUniformSet::uniform2iv(std::size_t num, const std::int32_t* i2v) noexcept
{
	this->needUpdate(true);
	_variant.uniform2iv(num, i2v);
}

void
VulkanGraphicsUniformSet::uniform3iv(const std::vector<int3>& value) noexcept
{
	this->needUpdate(true);
	_variant.uniform3iv(value);
}

void
VulkanGraphicsUniformSet::uniform3iv(std::size_t num, const std::int32_t* i3v) noexcept
{
	this->needUpdate(true);
	_variant.uniform3iv(num, i3v);
}

void
VulkanGraphicsUniformSet::uniform4iv(const std::vector<int4>& value) noexcept
{
	this->needUpdate(true);
	_variant.uniform4iv(value);
}

void
VulkanGraphicsUniformSet::uniform4iv(std::size_t num, const std::int32_t* i4v) noexcept
{
	this->needUpdate(true);
	_variant.uniform4iv(num, i4v);
}

void
VulkanGraphicsUniformSet::uniform1uiv(const std::vector<uint1>& value) noexcept
{
	this->needUpdate(true);
	_variant.uniform1uiv(value);
}

void
VulkanGraphicsUniformSet::uniform1uiv(std::size_t num, const std::uint32_t* ui1v) noexcept
{
	this->needUpdate(true);
	_variant.uniform1uiv(num, ui1v);
}

void
VulkanGraphicsUniformSet::uniform2uiv(const std::vector<uint2>& value) noexcept
{
	this->needUpdate(true);
	_variant.uniform2uiv(value);
}

void
VulkanGraphicsUniformSet::uniform2uiv(std::size_t num, const std::uint32_t* ui2v) noexcept
{
	this->needUpdate(true);
	_variant.uniform2uiv(num, ui2v);
}

void
VulkanGraphicsUniformSet::uniform3uiv(const std::vector<uint3>& value) noexcept
{
	this->needUpdate(true);
	_variant.uniform3uiv(value);
}

void
VulkanGraphicsUniformSet::uniform3uiv(std::size_t num, const std::uint32_t* ui3v) noexcept
{
	this->needUpdate(true);
	_variant.uniform3uiv(num, ui3v);
}

void
VulkanGraphicsUniformSet::uniform4uiv(const std::vector<uint4>& value) noexcept
{
	this->needUpdate(true);
	_variant.uniform4uiv(value);
}

void
VulkanGraphicsUniformSet::uniform4uiv(std::size_t num, const std::uint32_t* ui4v) noexcept
{
	this->needUpdate(true);
	_variant.uniform4uiv(num, ui4v);
}

void
VulkanGraphicsUniformSet::uniform1fv(const std::vector<float1>& value) noexcept
{
	this->needUpdate(true);
	_variant.uniform1fv(value);
}

void
VulkanGraphicsUniformSet::uniform1fv(std::size_t num, const float* f1v) noexcept
{
	this->needUpdate(true);
	_variant.uniform1fv(num, f1v);
}

void
VulkanGraphicsUniformSet::uniform2fv(const std::vector<float2>& value) noexcept
{
	this->needUpdate(true);
	_variant.uniform2fv(value);
}

void
VulkanGraphicsUniformSet::uniform2fv(std::size_t num, const float* f2v) noexcept
{
	this->needUpdate(true);
	_variant.uniform2fv(num, f2v);
}

void
VulkanGraphicsUniformSet::uniform3fv(const std::vector<float3>& value) noexcept
{
	this->needUpdate(true);
	_variant.uniform3fv(value);
}

void
VulkanGraphicsUniformSet::uniform3fv(std::size_t num, const float* f3v) noexcept
{
	this->needUpdate(true);
	_variant.uniform3fv(num, f3v);
}

void
VulkanGraphicsUniformSet::uniform4fv(const std::vector<float4>& value) noexcept
{
	this->needUpdate(true);
	_variant.uniform4fv(value);
}

void
VulkanGraphicsUniformSet::uniform4fv(std::size_t num, const float* f4v) noexcept
{
	this->needUpdate(true);
	_variant.uniform4fv(num, f4v);
}

void
VulkanGraphicsUniformSet::uniform2fmatv(const std::vector<float2x2>& value) noexcept
{
	this->needUpdate(true);
	_variant.uniform2fmatv(value);
}

void
VulkanGraphicsUniformSet::uniform2fmatv(std::size_t num, const float* mat2) noexcept
{
	this->needUpdate(true);
	_variant.uniform2fmatv(num, mat2);
}

void
VulkanGraphicsUniformSet::uniform3fmatv(const std::vector<float3x3>& value) noexcept
{
	this->needUpdate(true);
	_variant.uniform3fmatv(value);
}

void
VulkanGraphicsUniformSet::uniform3fmatv(std::size_t num, const float* mat3) noexcept
{
	this->needUpdate(true);
	_variant.uniform3fmatv(num, mat3);
}

void
VulkanGraphicsUniformSet::uniform4fmatv(const std::vector<float4x4>& value) noexcept
{
	this->needUpdate(true);
	_variant.uniform4fmatv(value);
}

void
VulkanGraphicsUniformSet::uniform4fmatv(std::size_t num, const float* mat4) noexcept
{
	this->needUpdate(true);
	_variant.uniform4fmatv(num, mat4);
}

void
VulkanGraphicsUniformSet::uniformTexture(GraphicsTexturePtr texture, GraphicsSamplerPtr sampler) noexcept
{
	this->needUpdate(true);
	_variant.uniformTexture(texture, sampler);
}

void
VulkanGraphicsUniformSet::uniformBuffer(GraphicsDataPtr ubo) noexcept
{
	this->needUpdate(true);
	_variant.uniformBuffer(ubo);
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

const float2x2&
VulkanGraphicsUniformSet::getFloat2x2() const noexcept
{
	return _variant.getFloat2x2();
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

const std::vector<float2x2>&
VulkanGraphicsUniformSet::getFloat2x2Array() const noexcept
{
	return _variant.getFloat2x2Array();
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

const GraphicsTexturePtr&
VulkanGraphicsUniformSet::getTexture() const noexcept
{
	return _variant.getTexture();
}

const GraphicsSamplerPtr&
VulkanGraphicsUniformSet::getTextureSampler() const noexcept
{
	return _variant.getTextureSampler();
}

const GraphicsDataPtr&
VulkanGraphicsUniformSet::getBuffer() const noexcept
{
	return _variant.getBuffer();
}

void
VulkanGraphicsUniformSet::setGraphicsParam(GraphicsParamPtr param) noexcept
{
	assert(param);
	_param = param;
	_variant.setType(param->getType());
}

const GraphicsParamPtr&
VulkanGraphicsUniformSet::getGraphicsParam() const noexcept
{
	return _param;
}

VulkanDescriptorPool::VulkanDescriptorPool() noexcept
	: _descriptorPool(VK_NULL_HANDLE)
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
	info.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
	info.maxSets = descriptorPoolDesc.getMaxSets();
	info.poolSizeCount = pool.size();
	info.pPoolSizes = pool.data();

	if (vkCreateDescriptorPool(_device.lock()->getDevice(), &info, nullptr, &_descriptorPool) != VK_SUCCESS)
	{
		VK_PLATFORM_LOG("vkCreateDescriptorPool() fail.");
		return false;
	}

	_descriptorPoolDesc = descriptorPoolDesc;
	return true;
}

void
VulkanDescriptorPool::close() noexcept
{
	if (_descriptorPool != VK_NULL_HANDLE)
	{
		vkDestroyDescriptorPool(_device.lock()->getDevice(), _descriptorPool, nullptr);
		_descriptorPool = VK_NULL_HANDLE;
	}
}

VkDescriptorPool
VulkanDescriptorPool::getDescriptorPool() const noexcept
{
	return _descriptorPool;
}

void
VulkanDescriptorPool::setDevice(GraphicsDevicePtr device) noexcept
{
	_device = device->downcast_pointer<VulkanDevice>();
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
			layout.stageFlags = VulkanTypes::asShaderStageFlags(it->getShaderStageFlags());

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

	if (vkAllocateDescriptorSets(_device.lock()->getDevice(), &info, &_vkDescriptorSet) != VK_SUCCESS)
	{
		VK_PLATFORM_LOG("vkAllocateDescriptorSets() fail.");
		return false;
	}

	auto& descriptorSetLayoutDesc = descriptorSetDesc.getGraphicsDescriptorSetLayout()->getGraphicsDescriptorSetLayoutDesc();
	auto& params = descriptorSetLayoutDesc.getUniformComponents();
	for (auto& param : params)
	{
		if (param->getType() == GraphicsUniformType::GraphicsUniformTypeUniformBuffer)
		{
			auto uniformBlock = param->downcast<VulkanGraphicsUniformBlock>();
			auto& name = uniformBlock->getName();
			if (name == "Globals")
			{
				GraphicsDataDesc uniformBufferDesc;
				uniformBufferDesc.setType(GraphicsDataType::GraphicsDataTypeUniformBuffer);
				uniformBufferDesc.setStreamSize(uniformBlock->getBlockSize());
				uniformBufferDesc.setUsage(GraphicsUsageFlagBits::GraphicsUsageFlagReadBit | GraphicsUsageFlagBits::GraphicsUsageFlagWriteBit);
				auto ubo = this->getDevice()->createGraphicsData(uniformBufferDesc);
				if (!ubo)
				{
					VK_PLATFORM_LOG("Can't create uniform buffer");
					return false;
				}

				auto& uniforms = uniformBlock->getGraphicsUniforms();
				for (auto& uniform : uniforms)
				{
					auto uniformSet = std::make_shared<VulkanGraphicsUniformSet>();
					uniformSet->setGraphicsParam(uniform);
					_activeUniformSets.push_back(uniformSet);
					_activeGlobalUniformSets.push_back(uniformSet);
				}

				_globalUniformBlock = uniformBlock->downcast_pointer<VulkanGraphicsUniformBlock>();
				_globalData = ubo->downcast_pointer<VulkanGraphicsData>();
			}
		}
		else
		{
			auto uniformSet = std::make_shared<VulkanGraphicsUniformSet>();
			uniformSet->setGraphicsParam(param);
			_activeUniformSets.push_back(uniformSet);
		}
	}

	std::vector<VkWriteDescriptorSet> descriptorWrites;

	if (_globalUniformBlock)
	{
		auto uniformBlock = _globalUniformBlock;
		auto data = _globalData->downcast<VulkanGraphicsData>();

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

		descriptorWrites.push_back(write);
	}

	if (!descriptorWrites.empty())
	{
		vkUpdateDescriptorSets(_device.lock()->getDevice(), descriptorWrites.size(), descriptorWrites.data(), 0, nullptr);
	}

	_descriptorSetDesc = descriptorSetDesc;
	return true;
}

void
VulkanDescriptorSet::close() noexcept
{
	if (_vkDescriptorSet != VK_NULL_HANDLE)
	{
		vkFreeDescriptorSets(_device.lock()->getDevice(), _descriptorSetDesc.getGraphicsDescriptorPool()->downcast<VulkanDescriptorPool>()->getDescriptorPool(), 1, &_vkDescriptorSet);
		_vkDescriptorSet = VK_NULL_HANDLE;
	}
}

void
VulkanDescriptorSet::update() noexcept
{
	std::uint32_t descriptorWriteCount = 0;
	VkWriteDescriptorSet descriptorWrites[10];
	VkDescriptorImageInfo descriptorImageInfos[10];

	for (auto& it : _activeUniformSets)
	{
		auto param = it->getGraphicsParam();
		auto type = it->getGraphicsParam()->getType();
		auto bindingPoint = it->getGraphicsParam()->getBindingPoint();

		switch (type)
		{
		case ray::GraphicsUniformTypeSampler:
			break;
		case ray::GraphicsUniformTypeSamplerImage:
		{
			auto texture = it->getTexture();
			if (texture)
			{
				auto vkTexture = texture->downcast<VulkanTexture>();

				auto& info = descriptorImageInfos[descriptorWriteCount];
				info.imageLayout = VkImageLayout::VK_IMAGE_LAYOUT_GENERAL;
				info.imageView = vkTexture->getImageView();
				info.sampler = nullptr;

				auto& write = descriptorWrites[descriptorWriteCount++];
				write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
				write.pNext = nullptr;
				write.descriptorType = VkDescriptorType::VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE;
				write.descriptorCount = 1;
				write.dstSet = _vkDescriptorSet;
				write.dstArrayElement = 0;
				write.dstBinding = bindingPoint;
				write.pBufferInfo = nullptr;
				write.pImageInfo = &info;
				write.pTexelBufferView = nullptr;
			}
		}
		break;
		case ray::GraphicsUniformTypeCombinedImageSampler:
			break;
		case ray::GraphicsUniformTypeStorageImage:
		{
			auto texture = it->getTexture();
			if (texture)
			{
				auto vkTexture = texture->downcast<VulkanTexture>();

				VkDescriptorImageInfo info;
				info.imageLayout = VkImageLayout::VK_IMAGE_LAYOUT_GENERAL;
				info.imageView = vkTexture->getImageView();
				info.sampler = nullptr;

				auto& write = descriptorWrites[descriptorWriteCount++];
				write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
				write.pNext = nullptr;
				write.descriptorType = VkDescriptorType::VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
				write.descriptorCount = 1;
				write.dstSet = _vkDescriptorSet;
				write.dstArrayElement = 0;
				write.dstBinding = bindingPoint;
				write.pBufferInfo = nullptr;
				write.pImageInfo = &info;
				write.pTexelBufferView = nullptr;
			}
		}
		break;
		case ray::GraphicsUniformTypeStorageTexelBuffer:
			break;
		case ray::GraphicsUniformTypeStorageBuffer:
			break;
		case ray::GraphicsUniformTypeStorageBufferDynamic:
			break;
		case ray::GraphicsUniformTypeUniformTexelBuffer:
			break;
		case ray::GraphicsUniformTypeUniformBuffer:
			break;
		case ray::GraphicsUniformTypeUniformBufferDynamic:
			break;
		case ray::GraphicsUniformTypeInputAttachment:
			break;
		default:
			break;
		}
	}

	if (descriptorWriteCount > 0)
	{
		vkUpdateDescriptorSets(_device.lock()->getDevice(), descriptorWriteCount, descriptorWrites, 0, nullptr);
	}

	void* buffer = nullptr;

	for (auto& activeUniformSet : _activeGlobalUniformSets)
	{
		auto uniformSet = activeUniformSet->downcast<VulkanGraphicsUniformSet>();
		if (!uniformSet->needUpdate())
			continue;

		if (!buffer)
		{
			if (!_globalData->map(0, _globalData->getGraphicsDataDesc().getStreamSize(), &buffer))
				break;
		}			

		auto uniform = uniformSet->getGraphicsParam()->downcast<VulkanGraphicsUniform>();
		auto uniformType = uniform->getType();
		switch (uniformType)
		{
		case ray::GraphicsUniformTypeBool:
			(*(int*)((char*)buffer + uniform->getOffset())) = uniformSet->getBool();
			break;
		case ray::GraphicsUniformTypeInt:
			(*(int1*)((char*)buffer + uniform->getOffset())) = uniformSet->getInt();
			break;
		case ray::GraphicsUniformTypeInt2:
			(*(int2*)((char*)buffer + uniform->getOffset())) = uniformSet->getInt2();
			break;
		case ray::GraphicsUniformTypeInt3:
			(*(int3*)((char*)buffer + uniform->getOffset())) = uniformSet->getInt3();
			break;
		case ray::GraphicsUniformTypeInt4:
			(*(int4*)((char*)buffer + uniform->getOffset())) = uniformSet->getInt4();
			break;
		case ray::GraphicsUniformTypeUInt:
			(*(uint1*)((char*)buffer + uniform->getOffset())) = uniformSet->getUInt();
			break;
		case ray::GraphicsUniformTypeUInt2:
			(*(uint2*)((char*)buffer + uniform->getOffset())) = uniformSet->getUInt2();
			break;
		case ray::GraphicsUniformTypeUInt3:
			(*(uint3*)((char*)buffer + uniform->getOffset())) = uniformSet->getUInt3();
			break;
		case ray::GraphicsUniformTypeUInt4:
			(*(uint4*)((char*)buffer + uniform->getOffset())) = uniformSet->getUInt4();
			break;
		case ray::GraphicsUniformTypeFloat:
			(*(float1*)((char*)buffer + uniform->getOffset())) = uniformSet->getFloat();
			break;
		case ray::GraphicsUniformTypeFloat2:
			(*(float2*)((char*)buffer + uniform->getOffset())) = uniformSet->getFloat2();
			break;
		case ray::GraphicsUniformTypeFloat3:
			(*(float3*)((char*)buffer + uniform->getOffset())) = uniformSet->getFloat3();
			break;
		case ray::GraphicsUniformTypeFloat4:
			(*(float4*)((char*)buffer + uniform->getOffset())) = uniformSet->getFloat4();
			break;
		case ray::GraphicsUniformTypeFloat2x2:
			(*(float2x2*)((char*)buffer + uniform->getOffset())) = uniformSet->getFloat2x2();
			break;
		case ray::GraphicsUniformTypeFloat3x3:
			(*(float3x3*)((char*)buffer + uniform->getOffset())) = uniformSet->getFloat3x3();
			break;
		case ray::GraphicsUniformTypeFloat4x4:
			(*(float4x4*)((char*)buffer + uniform->getOffset())) = uniformSet->getFloat4x4();
			break;
		case ray::GraphicsUniformTypeIntArray:
			std::memcpy((char*)buffer + uniform->getOffset(), uniformSet->getIntArray().data(), uniformSet->getIntArray().size()* sizeof(int1));
			break;
		case ray::GraphicsUniformTypeInt2Array:
			std::memcpy((char*)buffer + uniform->getOffset(), uniformSet->getInt2Array().data(), uniformSet->getInt2Array().size() * sizeof(int2));
			break;
		case ray::GraphicsUniformTypeInt3Array:
			std::memcpy((char*)buffer + uniform->getOffset(), uniformSet->getInt3Array().data(), uniformSet->getInt3Array().size() * sizeof(int3));
			break;
		case ray::GraphicsUniformTypeInt4Array:
			std::memcpy((char*)buffer + uniform->getOffset(), uniformSet->getInt4Array().data(), uniformSet->getInt4Array().size() * sizeof(int4));
			break;
		case ray::GraphicsUniformTypeUIntArray:
			std::memcpy((char*)buffer + uniform->getOffset(), uniformSet->getUIntArray().data(), uniformSet->getUIntArray().size()* sizeof(uint1));
			break;
		case ray::GraphicsUniformTypeUInt2Array:
			std::memcpy((char*)buffer + uniform->getOffset(), uniformSet->getUInt2Array().data(), uniformSet->getUInt2Array().size() * sizeof(uint2));
			break;
		case ray::GraphicsUniformTypeUInt3Array:
			std::memcpy((char*)buffer + uniform->getOffset(), uniformSet->getUInt3Array().data(), uniformSet->getUInt3Array().size() * sizeof(uint3));
			break;
		case ray::GraphicsUniformTypeUInt4Array:
			std::memcpy((char*)buffer + uniform->getOffset(), uniformSet->getUInt4Array().data(), uniformSet->getUInt4Array().size() * sizeof(uint4));
			break;
		case ray::GraphicsUniformTypeFloatArray:
			std::memcpy((char*)buffer + uniform->getOffset(), uniformSet->getFloatArray().data(), uniformSet->getFloatArray().size() * sizeof(float1));
			break;
		case ray::GraphicsUniformTypeFloat2Array:
			std::memcpy((char*)buffer + uniform->getOffset(), uniformSet->getFloat2Array().data(), uniformSet->getFloat2Array().size() * sizeof(float2));
			break;
		case ray::GraphicsUniformTypeFloat3Array:
			std::memcpy((char*)buffer + uniform->getOffset(), uniformSet->getFloat3Array().data(), uniformSet->getFloat3Array().size() * sizeof(float3));
			break;
		case ray::GraphicsUniformTypeFloat4Array:
			std::memcpy((char*)buffer + uniform->getOffset(), uniformSet->getFloat4Array().data(), uniformSet->getFloat4Array().size() * sizeof(float4));
			break;
		case ray::GraphicsUniformTypeFloat2x2Array:
			std::memcpy((char*)buffer + uniform->getOffset(), uniformSet->getFloat4Array().data(), uniformSet->getFloat4Array().size() * sizeof(float2x2));
			break;
		case ray::GraphicsUniformTypeFloat3x3Array:
			std::memcpy((char*)buffer + uniform->getOffset(), uniformSet->getFloat4Array().data(), uniformSet->getFloat4Array().size() * sizeof(float3x3));
			break;
		case ray::GraphicsUniformTypeFloat4x4Array:
			std::memcpy((char*)buffer + uniform->getOffset(), uniformSet->getFloat4Array().data(), uniformSet->getFloat4Array().size() * sizeof(float4x4));
			break;
		default:
			break;
		}

		uniformSet->needUpdate(false);
	}

	if (buffer)
	{
		_globalData->unmap();
	}
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
	_device = device->downcast_pointer<VulkanDevice>();
}

GraphicsDevicePtr
VulkanDescriptorSet::getDevice() noexcept
{
	return _device.lock();
}

_NAME_END