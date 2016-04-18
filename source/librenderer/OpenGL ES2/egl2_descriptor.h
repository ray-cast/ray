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
#ifndef _H_EGL2_DESCRIPTOR_H_
#define _H_EGL2_DESCRIPTOR_H_

#include "egl2_types.h"

_NAME_BEGIN

class EGL2GraphicsUniformSet final : public GraphicsUniformSet
{
	__DeclareSubClass(EGL2GraphicsUniformSet, GraphicsUniformSet)
public:
	EGL2GraphicsUniformSet() noexcept;
	virtual ~EGL2GraphicsUniformSet() noexcept;

	void setType(GraphicsUniformType type) noexcept;
	GraphicsUniformType getType() const noexcept;

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

	void setGraphicsUniform(GraphicsUniformPtr uniform) noexcept;
	GraphicsUniformPtr getGraphicsUniform() const noexcept;

private:
	EGL2GraphicsUniformSet(const EGL2GraphicsUniformSet&) = delete;
	EGL2GraphicsUniformSet& operator=(const EGL2GraphicsUniformSet&) = delete;

private:
	GraphicsVariant _variant;
	GraphicsUniformPtr _uniform;
};

class EGL2DescriptorPool final : public GraphicsDescriptorPool
{
	__DeclareSubClass(EGL2DescriptorPool, GraphicsDescriptorPool)
public:
	EGL2DescriptorPool() noexcept;
	~EGL2DescriptorPool() noexcept;

	bool setup(const GraphicsDescriptorPoolDesc& desc) noexcept;
	void close() noexcept;

	const GraphicsDescriptorPoolDesc& getGraphicsDescriptorPoolDesc() const noexcept;

private:
	friend class EGL2Device;
	void setDevice(GraphicsDevicePtr device) noexcept;
	GraphicsDevicePtr getDevice() noexcept;

private:
	EGL2DescriptorPool(const EGL2DescriptorPool&) noexcept = delete;
	EGL2DescriptorPool& operator=(const EGL2DescriptorPool&) noexcept = delete;

private:
	GraphicsDeviceWeakPtr _device;
	GraphicsDescriptorPoolDesc _descriptorPoolDesc;
};

class EGL2DescriptorSetLayout final : public GraphicsDescriptorSetLayout
{
	__DeclareSubClass(EGL2DescriptorSetLayout, OGLCoreGraphicsData)
public:
	EGL2DescriptorSetLayout() noexcept;
	~EGL2DescriptorSetLayout() noexcept;

	bool setup(const GraphicsDescriptorSetLayoutDesc& desc) noexcept;
	void close() noexcept;

	const GraphicsDescriptorSetLayoutDesc& getGraphicsDescriptorSetLayoutDesc() const noexcept;

private:
	friend class EGL2Device;
	void setDevice(GraphicsDevicePtr device) noexcept;
	GraphicsDevicePtr getDevice() noexcept;

private:
	EGL2DescriptorSetLayout(const EGL2DescriptorSetLayout&) noexcept = delete;
	EGL2DescriptorSetLayout& operator=(const EGL2DescriptorSetLayout&) noexcept = delete;

private:
	GraphicsDeviceWeakPtr _device;
	GraphicsDescriptorSetLayoutDesc _descriptorSetDesc;
};

class EGL2DescriptorSet final : public GraphicsDescriptorSet
{
	__DeclareSubClass(EGL2DescriptorSet, OGLCoreGraphicsData)
public:
	EGL2DescriptorSet() noexcept;
	~EGL2DescriptorSet() noexcept;

	bool setup(const GraphicsDescriptorSetDesc& desc) noexcept;
	void close() noexcept;

	void apply(GraphicsProgramPtr program) noexcept;

	const GraphicsUniformSets& getGraphicsUniformSets() const noexcept;
	const GraphicsDescriptorSetDesc& getGraphicsDescriptorSetDesc() const noexcept;

private:
	friend class EGL2Device;
	void setDevice(GraphicsDevicePtr device) noexcept;
	GraphicsDevicePtr getDevice() noexcept;

private:
	EGL2DescriptorSet(const EGL2DescriptorSet&) noexcept = delete;
	EGL2DescriptorSet& operator=(const EGL2DescriptorSet&) noexcept = delete;

private:
	GraphicsUniformSets _activeUniformSets;
	GraphicsDeviceWeakPtr _device;
	GraphicsDescriptorSetDesc _descriptorSetDesc;
};

_NAME_END

#endif