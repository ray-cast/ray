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
#include <ray/graphics_descriptor.h>
#include <ray/graphics_variant.h>

_NAME_BEGIN

__ImplementSubInterface(GraphicsDescriptorPool, GraphicsChild, "GraphicsDescriptorPool")
__ImplementSubInterface(GraphicsDescriptorSet, GraphicsChild, "GraphicsDescriptorSet")
__ImplementSubInterface(GraphicsDescriptorSetLayout, GraphicsChild, "GraphicsDescriptorSetLayout")

GraphicsUniform::GraphicsUniform() noexcept
	: _shaderStage(GraphicsShaderStage::GraphicsShaderStageNone)
	, _binding(0)
{
}

GraphicsUniform::GraphicsUniform(GraphicsShaderStage stage, std::uint32_t binding) noexcept
	: _shaderStage(stage)
	, _binding(binding)
{
}

GraphicsUniform::GraphicsUniform(GraphicsShaderStage stage, std::uint32_t binding, GraphicsUniformType type) noexcept
	: _shaderStage(stage)
	, _binding(binding)
{
	_variant.setType(type);
}

GraphicsUniform::GraphicsUniform(GraphicsShaderStage stage, std::uint32_t binding, bool value) noexcept
	: _shaderStage(stage)
	, _binding(binding)
{
	_variant.assign(value);
}

GraphicsUniform::GraphicsUniform(GraphicsShaderStage stage, std::uint32_t binding, int value) noexcept
	: _shaderStage(stage)
	, _binding(binding)
{
	_variant.assign(value);
}

GraphicsUniform::GraphicsUniform(GraphicsShaderStage stage, std::uint32_t binding, const int2& value) noexcept
	: _shaderStage(stage)
	, _binding(binding)
{
	_variant.assign(value);
}

GraphicsUniform::GraphicsUniform(GraphicsShaderStage stage, std::uint32_t binding, const int3& value) noexcept
	: _shaderStage(stage)
	, _binding(binding)
{
	_variant.assign(value);
}

GraphicsUniform::GraphicsUniform(GraphicsShaderStage stage, std::uint32_t binding, const int4& value) noexcept
	: _shaderStage(stage)
	, _binding(binding)
{
	_variant.assign(value);
}

GraphicsUniform::GraphicsUniform(GraphicsShaderStage stage, std::uint32_t binding, uint1 value) noexcept
	: _shaderStage(stage)
	, _binding(binding)
{
	_variant.assign(value);
}

GraphicsUniform::GraphicsUniform(GraphicsShaderStage stage, std::uint32_t binding, const uint2& value) noexcept
	: _shaderStage(stage)
	, _binding(binding)
{
	_variant.assign(value);
}

GraphicsUniform::GraphicsUniform(GraphicsShaderStage stage, std::uint32_t binding, const uint3& value) noexcept
	: _shaderStage(stage)
	, _binding(binding)
{
	_variant.assign(value);
}

GraphicsUniform::GraphicsUniform(GraphicsShaderStage stage, std::uint32_t binding, const uint4& value) noexcept
	: _shaderStage(stage)
	, _binding(binding)
{
	_variant.assign(value);
}

GraphicsUniform::GraphicsUniform(GraphicsShaderStage stage, std::uint32_t binding, float value) noexcept
	: _shaderStage(stage)
	, _binding(binding)
{
	_variant.assign(value);
}

GraphicsUniform::GraphicsUniform(GraphicsShaderStage stage, std::uint32_t binding, const float2& value) noexcept
	: _shaderStage(stage)
	, _binding(binding)
{
	_variant.assign(value);
}

GraphicsUniform::GraphicsUniform(GraphicsShaderStage stage, std::uint32_t binding, const float3& value) noexcept
	: _shaderStage(stage)
	, _binding(binding)
{
	_variant.assign(value);
}

GraphicsUniform::GraphicsUniform(GraphicsShaderStage stage, std::uint32_t binding, const float4& value) noexcept
	: _shaderStage(stage)
	, _binding(binding)
{
	_variant.assign(value);
}

GraphicsUniform::GraphicsUniform(GraphicsShaderStage stage, std::uint32_t binding, const float3x3& value) noexcept
	: _shaderStage(stage)
	, _binding(binding)
{
	_variant.assign(value);
}

GraphicsUniform::GraphicsUniform(GraphicsShaderStage stage, std::uint32_t binding, const float4x4& value) noexcept
	: _shaderStage(stage)
	, _binding(binding)
{
	_variant.assign(value);
}

GraphicsUniform::GraphicsUniform(GraphicsShaderStage stage, std::uint32_t binding, const std::vector<int1>& value) noexcept
	: _shaderStage(stage)
	, _binding(binding)
{
	_variant.assign(value);
}

GraphicsUniform::GraphicsUniform(GraphicsShaderStage stage, std::uint32_t binding, const std::vector<int2>& value) noexcept
	: _shaderStage(stage)
	, _binding(binding)
{
	_variant.assign(value);
}

GraphicsUniform::GraphicsUniform(GraphicsShaderStage stage, std::uint32_t binding, const std::vector<int3>& value) noexcept
	: _shaderStage(stage)
	, _binding(binding)
{
	_variant.assign(value);
}

GraphicsUniform::GraphicsUniform(GraphicsShaderStage stage, std::uint32_t binding, const std::vector<int4>& value) noexcept
	: _shaderStage(stage)
	, _binding(binding)
{
	_variant.assign(value);
}

GraphicsUniform::GraphicsUniform(GraphicsShaderStage stage, std::uint32_t binding, const std::vector<uint1>& value) noexcept
	: _shaderStage(stage)
	, _binding(binding)
{
	_variant.assign(value);
}

GraphicsUniform::GraphicsUniform(GraphicsShaderStage stage, std::uint32_t binding, const std::vector<uint2>& value) noexcept
	: _shaderStage(stage)
	, _binding(binding)
{
	_variant.assign(value);
}

GraphicsUniform::GraphicsUniform(GraphicsShaderStage stage, std::uint32_t binding, const std::vector<uint3>& value) noexcept
	: _shaderStage(stage)
	, _binding(binding)
{
	_variant.assign(value);
}

GraphicsUniform::GraphicsUniform(GraphicsShaderStage stage, std::uint32_t binding, const std::vector<uint4>& value) noexcept
	: _shaderStage(stage)
	, _binding(binding)
{
	_variant.assign(value);
}

GraphicsUniform::GraphicsUniform(GraphicsShaderStage stage, std::uint32_t binding, const std::vector<float1>& value) noexcept
	: _shaderStage(stage)
	, _binding(binding)
{
	_variant.assign(value);
}

GraphicsUniform::GraphicsUniform(GraphicsShaderStage stage, std::uint32_t binding, const std::vector<float2>& value) noexcept
	: _shaderStage(stage)
	, _binding(binding)
{
	_variant.assign(value);
}

GraphicsUniform::GraphicsUniform(GraphicsShaderStage stage, std::uint32_t binding, const std::vector<float3>& value) noexcept
	: _shaderStage(stage)
	, _binding(binding)
{
	_variant.assign(value);
}

GraphicsUniform::GraphicsUniform(GraphicsShaderStage stage, std::uint32_t binding, const std::vector<float4>& value) noexcept
	: _shaderStage(stage)
	, _binding(binding)
{
	_variant.assign(value);
}

GraphicsUniform::GraphicsUniform(GraphicsShaderStage stage, std::uint32_t binding, GraphicsTexturePtr texture, GraphicsSamplerPtr sampler) noexcept
	: _shaderStage(stage)
	, _binding(binding)
{
	_variant.assign(texture, sampler);
}

GraphicsUniform::~GraphicsUniform() noexcept
{
}

void
GraphicsUniform::setShaderStage(GraphicsShaderStage stage) noexcept
{
	_shaderStage = stage;
}

GraphicsShaderStage
GraphicsUniform::getShaderStage() const noexcept
{
	return _shaderStage;
}

void
GraphicsUniform::setType(GraphicsUniformType type) noexcept
{
	_variant.setType(type);
}

GraphicsUniformType
GraphicsUniform::getType() const noexcept
{
	return _variant.getType();
}

void
GraphicsUniform::setBindingPoint(std::uint32_t binding) noexcept
{
	_binding = binding;
}

std::uint32_t
GraphicsUniform::getBindingPoint() const noexcept
{
	return _binding;
}

void
GraphicsUniform::assign(bool value) noexcept
{
	_variant.assign(value);
}

void
GraphicsUniform::assign(int value) noexcept
{
	_variant.assign(value);
}

void
GraphicsUniform::assign(const int2& value) noexcept
{
	_variant.assign(value);
}

void
GraphicsUniform::assign(const int3& value) noexcept
{
	_variant.assign(value);
}

void
GraphicsUniform::assign(const int4& value) noexcept
{
	_variant.assign(value);
}

void
GraphicsUniform::assign(uint1 value) noexcept
{
	_variant.assign(value);
}

void
GraphicsUniform::assign(const uint2& value) noexcept
{
	_variant.assign(value);
}

void
GraphicsUniform::assign(const uint3& value) noexcept
{
	_variant.assign(value);
}

void
GraphicsUniform::assign(const uint4& value) noexcept
{
	_variant.assign(value);
}

void
GraphicsUniform::assign(float value) noexcept
{
	_variant.assign(value);
}

void
GraphicsUniform::assign(const float2& value) noexcept
{
	_variant.assign(value);
}

void
GraphicsUniform::assign(const float3& value) noexcept
{
	_variant.assign(value);
}

void
GraphicsUniform::assign(const float4& value) noexcept
{
	_variant.assign(value);
}

void
GraphicsUniform::assign(const float3x3& value) noexcept
{
	_variant.assign(value);
}

void
GraphicsUniform::assign(const float4x4& value) noexcept
{
	_variant.assign(value);
}

void
GraphicsUniform::assign(const std::vector<int1>& value) noexcept
{
	_variant.assign(value);
}

void
GraphicsUniform::assign(const std::vector<int2>& value) noexcept
{
	_variant.assign(value);
}

void
GraphicsUniform::assign(const std::vector<int3>& value) noexcept
{
	_variant.assign(value);
}

void
GraphicsUniform::assign(const std::vector<int4>& value) noexcept
{
	_variant.assign(value);
}

void
GraphicsUniform::assign(const std::vector<uint1>& value) noexcept
{
	_variant.assign(value);
}

void
GraphicsUniform::assign(const std::vector<uint2>& value) noexcept
{
	_variant.assign(value);
}

void
GraphicsUniform::assign(const std::vector<uint3>& value) noexcept
{
	_variant.assign(value);
}

void
GraphicsUniform::assign(const std::vector<uint4>& value) noexcept
{
	_variant.assign(value);
}

void
GraphicsUniform::assign(const std::vector<float1>& value) noexcept
{
	_variant.assign(value);
}

void
GraphicsUniform::assign(const std::vector<float2>& value) noexcept
{
	_variant.assign(value);
}

void
GraphicsUniform::assign(const std::vector<float3>& value) noexcept
{
	_variant.assign(value);
}

void
GraphicsUniform::assign(const std::vector<float4>& value) noexcept
{
	_variant.assign(value);
}

void
GraphicsUniform::assign(const std::vector<float3x3>& value) noexcept
{
	_variant.assign(value);
}

void
GraphicsUniform::assign(const std::vector<float4x4>& value) noexcept
{
	_variant.assign(value);
}

void 
GraphicsUniform::assign(GraphicsTexturePtr texture, GraphicsSamplerPtr sampler) noexcept
{
	_variant.assign(texture, sampler);
}

bool
GraphicsUniform::getBool() const noexcept
{
	return _variant.getBool();
}

int
GraphicsUniform::getInt() const noexcept
{
	return _variant.getInt();
}

const int2&
GraphicsUniform::getInt2() const noexcept
{
	return _variant.getInt2();
}

const int3&
GraphicsUniform::getInt3() const noexcept
{
	return _variant.getInt3();
}

const int4&
GraphicsUniform::getInt4() const noexcept
{
	return _variant.getInt4();
}

uint
GraphicsUniform::getUint() const noexcept
{
	return _variant.getUint();
}

const uint2&
GraphicsUniform::getUint2() const noexcept
{
	return _variant.getUint2();
}

const uint3&
GraphicsUniform::getUint3() const noexcept
{
	return _variant.getUint3();
}

const uint4&
GraphicsUniform::getUint4() const noexcept
{
	return _variant.getUint4();
}

float
GraphicsUniform::getFloat() const noexcept
{
	return _variant.getFloat();
}

const float2&
GraphicsUniform::getFloat2() const noexcept
{
	return _variant.getFloat2();
}

const float3&
GraphicsUniform::getFloat3() const noexcept
{
	return _variant.getFloat3();
}

const float4&
GraphicsUniform::getFloat4() const noexcept
{
	return _variant.getFloat4();
}

const float3x3&
GraphicsUniform::getFloat3x3() const noexcept
{
	return _variant.getFloat3x3();
}

const float4x4&
GraphicsUniform::getFloat4x4() const noexcept
{
	return _variant.getFloat4x4();
}

const std::vector<int1>&
GraphicsUniform::getIntArray() const noexcept
{
	return _variant.getIntArray();
}

const std::vector<int2>&
GraphicsUniform::getInt2Array() const noexcept
{
	return _variant.getInt2Array();
}

const std::vector<int3>&
GraphicsUniform::getInt3Array() const noexcept
{
	return _variant.getInt3Array();
}

const std::vector<int4>&
GraphicsUniform::getInt4Array() const noexcept
{
	return _variant.getInt4Array();
}

const std::vector<uint1>&
GraphicsUniform::getUintArray() const noexcept
{
	return _variant.getUintArray();
}

const std::vector<uint2>&
GraphicsUniform::getUint2Array() const noexcept
{
	return _variant.getUint2Array();
}

const std::vector<uint3>&
GraphicsUniform::getUint3Array() const noexcept
{
	return _variant.getUint3Array();
}

const std::vector<uint4>&
GraphicsUniform::getUint4Array() const noexcept
{
	return _variant.getUint4Array();
}

const std::vector<float1>&
GraphicsUniform::getFloatArray() const noexcept
{
	return _variant.getFloatArray();
}

const std::vector<float2>&
GraphicsUniform::getFloat2Array() const noexcept
{
	return _variant.getFloat2Array();
}

const std::vector<float3>&
GraphicsUniform::getFloat3Array() const noexcept
{
	return _variant.getFloat3Array();
}

const std::vector<float4>&
GraphicsUniform::getFloat4Array() const noexcept
{
	return _variant.getFloat4Array();
}

const std::vector<float3x3>&
GraphicsUniform::getFloat3x3Array() const noexcept
{
	return _variant.getFloat3x3Array();
}

const std::vector<float4x4>&
GraphicsUniform::getFloat4x4Array() const noexcept
{
	return _variant.getFloat4x4Array();
}

GraphicsTexturePtr
GraphicsUniform::getTexture() const noexcept
{
	return _variant.getTexture();
}

GraphicsSamplerPtr
GraphicsUniform::getTextureSampler() const noexcept
{
	return _variant.getTextureSampler();
}

GraphicsDescriptorSetLayoutDesc::GraphicsDescriptorSetLayoutDesc() noexcept
{
}

GraphicsDescriptorSetLayoutDesc::~GraphicsDescriptorSetLayoutDesc() noexcept
{
}

void
GraphicsDescriptorSetLayoutDesc::setUniformComponents(const GraphicsUniforms& component) noexcept
{
	_uniforms = component;
}

const GraphicsUniforms&
GraphicsDescriptorSetLayoutDesc::getUniformComponents() const noexcept
{
	return _uniforms;
}

void
GraphicsDescriptorSetLayoutDesc::addUniformComponent(GraphicsUniformPtr component) noexcept
{
	assert(component);

	auto it = _uniforms.begin();
	auto end = _uniforms.end();
	for (; it != end; ++it)
	{
		if ((*it)->getShaderStage() == component->getShaderStage() &&
			(*it)->getBindingPoint() == component->getBindingPoint())
		{
			return;
		}
	}

	_uniforms.push_back(component);
}

void
GraphicsDescriptorSetLayoutDesc::removeUniformComponent(GraphicsUniformPtr component) noexcept
{
	assert(component);
	auto it = std::find(_uniforms.begin(), _uniforms.end(), component);
	if (it != _uniforms.end())
		_uniforms.erase(it);
}

GraphicsDescriptorPoolComponent::GraphicsDescriptorPoolComponent() noexcept
	: _type(GraphicsUniformType::GraphicsUniformTypeNone)
	, _descriptorNums(0)
{
}

GraphicsDescriptorPoolComponent::GraphicsDescriptorPoolComponent(GraphicsUniformType type, std::uint32_t nums) noexcept
	: _type(type)
	, _descriptorNums(nums)
{
}

GraphicsDescriptorPoolComponent::~GraphicsDescriptorPoolComponent() noexcept
{
}

void
GraphicsDescriptorPoolComponent::setDescriptorType(GraphicsUniformType type) noexcept
{
	_type = type;
}

GraphicsUniformType
GraphicsDescriptorPoolComponent::getDescriptorType() const noexcept
{
	return _type;
}

void
GraphicsDescriptorPoolComponent::setDescriptorNums(std::uint32_t nums) noexcept
{
	_descriptorNums = nums;
}

std::uint32_t
GraphicsDescriptorPoolComponent::getDescriptorNums() const noexcept
{
	return _descriptorNums;
}

GraphicsDescriptorPoolDesc::GraphicsDescriptorPoolDesc() noexcept
	: _maxSets(1)
{
}

GraphicsDescriptorPoolDesc::GraphicsDescriptorPoolDesc(std::uint32_t maxSets) noexcept
	: _maxSets(maxSets)
{
}

GraphicsDescriptorPoolDesc::~GraphicsDescriptorPoolDesc() noexcept
{
}

void
GraphicsDescriptorPoolDesc::setMaxSets(std::uint32_t maxsets) noexcept
{
	_maxSets = maxsets;
}

std::uint32_t
GraphicsDescriptorPoolDesc::getMaxSets() const noexcept
{
	return _maxSets;
}

const GraphicsDescriptorPoolComponents&
GraphicsDescriptorPoolDesc::getGraphicsDescriptorPoolComponents() const noexcept
{
	return _poolSizes;
}

void
GraphicsDescriptorPoolDesc::addGraphicsDescriptorPoolComponent(const GraphicsDescriptorPoolComponent& component) noexcept
{
	auto it = _poolSizes.begin();
	auto end = _poolSizes.begin();
	for (; it != end; ++it)
	{
		if (it->getDescriptorType() == component.getDescriptorType())
		{
			_poolSizes.erase(it);
			return;
		}
	}

	_poolSizes.push_back(component);
}

GraphicsDescriptorSetDesc::GraphicsDescriptorSetDesc() noexcept
{
}

GraphicsDescriptorSetDesc::~GraphicsDescriptorSetDesc() noexcept
{
}

void
GraphicsDescriptorSetDesc::setGraphicsDescriptorSetLayout(GraphicsDescriptorSetLayoutPtr layout) noexcept
{
	_layout = layout;
}

GraphicsDescriptorSetLayoutPtr
GraphicsDescriptorSetDesc::getGraphicsDescriptorSetLayout() const noexcept
{
	return _layout;
}

void
GraphicsDescriptorSetDesc::setGraphicsDescriptorPool(GraphicsDescriptorPoolPtr pool) noexcept
{
	_pool = pool;
}

GraphicsDescriptorPoolPtr
GraphicsDescriptorSetDesc::getGraphicsDescriptorPool() const noexcept
{
	return _pool;
}

GraphicsDescriptorSetLayout::GraphicsDescriptorSetLayout() noexcept
{
}

GraphicsDescriptorSetLayout::~GraphicsDescriptorSetLayout() noexcept
{
}

GraphicsDescriptorSet::GraphicsDescriptorSet() noexcept
{
}

GraphicsDescriptorSet::~GraphicsDescriptorSet() noexcept
{
}

GraphicsDescriptorPool::GraphicsDescriptorPool() noexcept
{
}

GraphicsDescriptorPool::~GraphicsDescriptorPool() noexcept
{
}

_NAME_END