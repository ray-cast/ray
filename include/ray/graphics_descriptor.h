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
#ifndef _H_GRAPHICS_DESCRIPTOR_H_
#define _H_GRAPHICS_DESCRIPTOR_H_

#include <ray/graphics_child.h>
#include <ray/graphics_variant.h>

_NAME_BEGIN

class EXPORT GraphicsUniform
{
public:
	GraphicsUniform() noexcept;
	GraphicsUniform(GraphicsShaderStage stage, std::uint32_t binding) noexcept;
	GraphicsUniform(GraphicsShaderStage stage, std::uint32_t binding, GraphicsUniformType type) noexcept;
	GraphicsUniform(GraphicsShaderStage stage, std::uint32_t binding, bool value) noexcept;
	GraphicsUniform(GraphicsShaderStage stage, std::uint32_t binding, int value) noexcept;
	GraphicsUniform(GraphicsShaderStage stage, std::uint32_t binding, const int2& value) noexcept;
	GraphicsUniform(GraphicsShaderStage stage, std::uint32_t binding, const int3& value) noexcept;
	GraphicsUniform(GraphicsShaderStage stage, std::uint32_t binding, const int4& value) noexcept;
	GraphicsUniform(GraphicsShaderStage stage, std::uint32_t binding, uint1 value) noexcept;
	GraphicsUniform(GraphicsShaderStage stage, std::uint32_t binding, const uint2& value) noexcept;
	GraphicsUniform(GraphicsShaderStage stage, std::uint32_t binding, const uint3& value) noexcept;
	GraphicsUniform(GraphicsShaderStage stage, std::uint32_t binding, const uint4& value) noexcept;
	GraphicsUniform(GraphicsShaderStage stage, std::uint32_t binding, float value) noexcept;
	GraphicsUniform(GraphicsShaderStage stage, std::uint32_t binding, const float2& value) noexcept;
	GraphicsUniform(GraphicsShaderStage stage, std::uint32_t binding, const float3& value) noexcept;
	GraphicsUniform(GraphicsShaderStage stage, std::uint32_t binding, const float4& value) noexcept;
	GraphicsUniform(GraphicsShaderStage stage, std::uint32_t binding, const float3x3& value) noexcept;
	GraphicsUniform(GraphicsShaderStage stage, std::uint32_t binding, const float4x4& value) noexcept;
	GraphicsUniform(GraphicsShaderStage stage, std::uint32_t binding, const std::vector<int1>& value) noexcept;
	GraphicsUniform(GraphicsShaderStage stage, std::uint32_t binding, const std::vector<int2>& value) noexcept;
	GraphicsUniform(GraphicsShaderStage stage, std::uint32_t binding, const std::vector<int3>& value) noexcept;
	GraphicsUniform(GraphicsShaderStage stage, std::uint32_t binding, const std::vector<int4>& value) noexcept;
	GraphicsUniform(GraphicsShaderStage stage, std::uint32_t binding, const std::vector<uint1>& value) noexcept;
	GraphicsUniform(GraphicsShaderStage stage, std::uint32_t binding, const std::vector<uint2>& value) noexcept;
	GraphicsUniform(GraphicsShaderStage stage, std::uint32_t binding, const std::vector<uint3>& value) noexcept;
	GraphicsUniform(GraphicsShaderStage stage, std::uint32_t binding, const std::vector<uint4>& value) noexcept;
	GraphicsUniform(GraphicsShaderStage stage, std::uint32_t binding, const std::vector<float1>& value) noexcept;
	GraphicsUniform(GraphicsShaderStage stage, std::uint32_t binding, const std::vector<float2>& value) noexcept;
	GraphicsUniform(GraphicsShaderStage stage, std::uint32_t binding, const std::vector<float3>& value) noexcept;
	GraphicsUniform(GraphicsShaderStage stage, std::uint32_t binding, const std::vector<float4>& value) noexcept;
	GraphicsUniform(GraphicsShaderStage stage, std::uint32_t binding, GraphicsTexturePtr texture, GraphicsSamplerPtr sampler = nullptr) noexcept;
	virtual ~GraphicsUniform() noexcept;

	void setShaderStage(GraphicsShaderStage stage) noexcept;
	GraphicsShaderStage getShaderStage() const noexcept;

	void setType(GraphicsUniformType type) noexcept;
	GraphicsUniformType getType() const noexcept;

	void setBindingPoint(std::uint32_t binding) noexcept;
	std::uint32_t getBindingPoint() const noexcept;

	void assign(bool value) noexcept;
	void assign(int value) noexcept;
	void assign(const int2& value) noexcept;
	void assign(const int3& value) noexcept;
	void assign(const int4& value) noexcept;
	void assign(uint1 value) noexcept;
	void assign(const uint2& value) noexcept;
	void assign(const uint3& value) noexcept;
	void assign(const uint4& value) noexcept;
	void assign(float value) noexcept;
	void assign(const float2& value) noexcept;
	void assign(const float3& value) noexcept;
	void assign(const float4& value) noexcept;
	void assign(const float3x3& value) noexcept;
	void assign(const float4x4& value) noexcept;
	void assign(const std::vector<int1>& value) noexcept;
	void assign(const std::vector<int2>& value) noexcept;
	void assign(const std::vector<int3>& value) noexcept;
	void assign(const std::vector<int4>& value) noexcept;
	void assign(const std::vector<uint1>& value) noexcept;
	void assign(const std::vector<uint2>& value) noexcept;
	void assign(const std::vector<uint3>& value) noexcept;
	void assign(const std::vector<uint4>& value) noexcept;
	void assign(const std::vector<float1>& value) noexcept;
	void assign(const std::vector<float2>& value) noexcept;
	void assign(const std::vector<float3>& value) noexcept;
	void assign(const std::vector<float4>& value) noexcept;
	void assign(const std::vector<float3x3>& value) noexcept;
	void assign(const std::vector<float4x4>& value) noexcept;
	void assign(GraphicsTexturePtr texture, GraphicsSamplerPtr sampler = nullptr) noexcept;

	bool getBool() const noexcept;
	int getInt() const noexcept;
	const int2& getInt2() const noexcept;
	const int3& getInt3() const noexcept;
	const int4& getInt4() const noexcept;
	uint getUInt() const noexcept;
	const uint2& getUInt2() const noexcept;
	const uint3& getUInt3() const noexcept;
	const uint4& getUInt4() const noexcept;
	float getFloat() const noexcept;
	const float2& getFloat2() const noexcept;
	const float3& getFloat3() const noexcept;
	const float4& getFloat4() const noexcept;
	const float3x3& getFloat3x3() const noexcept;
	const float4x4& getFloat4x4() const noexcept;
	const std::vector<int1>& getIntArray() const noexcept;
	const std::vector<int2>& getInt2Array() const noexcept;
	const std::vector<int3>& getInt3Array() const noexcept;
	const std::vector<int4>& getInt4Array() const noexcept;
	const std::vector<uint1>& getUIntArray() const noexcept;
	const std::vector<uint2>& getUInt2Array() const noexcept;
	const std::vector<uint3>& getUInt3Array() const noexcept;
	const std::vector<uint4>& getUInt4Array() const noexcept;
	const std::vector<float1>& getFloatArray() const noexcept;
	const std::vector<float2>& getFloat2Array() const noexcept;
	const std::vector<float3>& getFloat3Array() const noexcept;
	const std::vector<float4>& getFloat4Array() const noexcept;
	const std::vector<float3x3>& getFloat3x3Array() const noexcept;
	const std::vector<float4x4>& getFloat4x4Array() const noexcept;
	GraphicsTexturePtr getTexture() const noexcept;
	GraphicsSamplerPtr getTextureSampler() const noexcept;

private:
	GraphicsUniform(const GraphicsUniform&) noexcept = delete;
	GraphicsUniform& operator=(const GraphicsUniform&) noexcept = delete;

private:
	std::uint32_t _binding;
	GraphicsVariant _variant;
	GraphicsShaderStage _shaderStage;
};

class EXPORT GraphicsDescriptorPoolComponent final
{
public:
	GraphicsDescriptorPoolComponent() noexcept;
	GraphicsDescriptorPoolComponent(GraphicsUniformType type, std::uint32_t nums) noexcept;
	~GraphicsDescriptorPoolComponent() noexcept;

	void setDescriptorType(GraphicsUniformType type) noexcept;
	GraphicsUniformType getDescriptorType() const noexcept;

	void setDescriptorNums(std::uint32_t nums) noexcept;
	std::uint32_t getDescriptorNums() const noexcept;

private:
	std::uint32_t _descriptorNums;
	GraphicsUniformType _type;
};

class EXPORT GraphicsDescriptorSetLayoutDesc final
{
public:
	GraphicsDescriptorSetLayoutDesc() noexcept;
	~GraphicsDescriptorSetLayoutDesc() noexcept;

	void addUniformComponent(GraphicsUniformPtr component) noexcept;
	void removeUniformComponent(GraphicsUniformPtr component) noexcept;

	void setUniformComponents(const GraphicsUniforms& component) noexcept;
	const GraphicsUniforms& getUniformComponents() const noexcept;

private:
	GraphicsUniforms _uniforms;
};

class EXPORT GraphicsDescriptorPoolDesc final
{
public:
	GraphicsDescriptorPoolDesc() noexcept;
	GraphicsDescriptorPoolDesc(std::uint32_t maxSets) noexcept;
	~GraphicsDescriptorPoolDesc() noexcept;

	void setMaxSets(std::uint32_t maxsets) noexcept;
	std::uint32_t getMaxSets() const noexcept;

	void addGraphicsDescriptorPoolComponent(const GraphicsDescriptorPoolComponent& desc) noexcept;
	const GraphicsDescriptorPoolComponents& getGraphicsDescriptorPoolComponents() const noexcept;

private:
	std::uint32_t _maxSets;
	GraphicsDescriptorPoolComponents _poolSizes;
};

class EXPORT GraphicsDescriptorSetDesc final
{
public:
	GraphicsDescriptorSetDesc() noexcept;
	~GraphicsDescriptorSetDesc() noexcept;

	void setGraphicsDescriptorSetLayout(GraphicsDescriptorSetLayoutPtr layout) noexcept;
	GraphicsDescriptorSetLayoutPtr getGraphicsDescriptorSetLayout() const noexcept;

	void setGraphicsDescriptorPool(GraphicsDescriptorPoolPtr pool) noexcept;
	GraphicsDescriptorPoolPtr getGraphicsDescriptorPool() const noexcept;

private:
	GraphicsDescriptorPoolPtr _pool;
	GraphicsDescriptorSetLayoutPtr _layout;
};

class EXPORT GraphicsDescriptorPool : public GraphicsChild
{
	__DeclareSubInterface(GraphicsDescriptorPool, GraphicsChild)
public:
	GraphicsDescriptorPool() noexcept;
	virtual ~GraphicsDescriptorPool() noexcept;

	virtual const GraphicsDescriptorPoolDesc& getGraphicsDescriptorPoolDesc() const noexcept = 0;

private:
	GraphicsDescriptorPool(const GraphicsDescriptorPool&) = delete;
	GraphicsDescriptorPool& operator=(const GraphicsDescriptorPool&) = delete;
};

class EXPORT GraphicsDescriptorSetLayout : public GraphicsChild
{
	__DeclareSubInterface(GraphicsDescriptorSetLayout, GraphicsChild)
public:
	GraphicsDescriptorSetLayout() noexcept;
	virtual ~GraphicsDescriptorSetLayout() noexcept;

	virtual const GraphicsDescriptorSetLayoutDesc& getGraphicsDescriptorSetLayoutDesc() const noexcept = 0;

private:
	GraphicsDescriptorSetLayout(const GraphicsDescriptorSetLayout&) = delete;
	GraphicsDescriptorSetLayout& operator=(const GraphicsDescriptorSetLayout&) = delete;
};

class EXPORT GraphicsDescriptorSet : public GraphicsChild
{
	__DeclareSubInterface(GraphicsDescriptorSet, GraphicsChild)
public:
	GraphicsDescriptorSet() noexcept;
	virtual ~GraphicsDescriptorSet() noexcept;

	virtual const GraphicsDescriptorSetDesc& getGraphicsDescriptorSetDesc() const noexcept = 0;

private:
	GraphicsDescriptorSet(const GraphicsDescriptorSet&) = delete;
	GraphicsDescriptorSet& operator=(const GraphicsDescriptorSet&) = delete;
};

_NAME_END

#endif