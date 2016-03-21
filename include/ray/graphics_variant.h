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
#ifndef _H_GRAPHICS_VARIANT_H_
#define _H_GRAPHICS_VARIANT_H_

#include <ray/graphics_types.h>

_NAME_BEGIN

class EXPORT GraphicsVariant final
{
public:
	GraphicsVariant() noexcept;
	GraphicsVariant(bool value) noexcept;
	GraphicsVariant(int value) noexcept;
	GraphicsVariant(const int2& value) noexcept;
	GraphicsVariant(const int3& value) noexcept;
	GraphicsVariant(const int4& value) noexcept;
	GraphicsVariant(uint1 value) noexcept;
	GraphicsVariant(const uint2& value) noexcept;
	GraphicsVariant(const uint3& value) noexcept;
	GraphicsVariant(const uint4& value) noexcept;
	GraphicsVariant(float value) noexcept;
	GraphicsVariant(const float2& value) noexcept;
	GraphicsVariant(const float3& value) noexcept;
	GraphicsVariant(const float4& value) noexcept;
	GraphicsVariant(const float3x3& value) noexcept;
	GraphicsVariant(const float4x4& value) noexcept;
	GraphicsVariant(const std::vector<int1>& value) noexcept;
	GraphicsVariant(const std::vector<int2>& value) noexcept;
	GraphicsVariant(const std::vector<int3>& value) noexcept;
	GraphicsVariant(const std::vector<int4>& value) noexcept;
	GraphicsVariant(const std::vector<uint1>& value) noexcept;
	GraphicsVariant(const std::vector<uint2>& value) noexcept;
	GraphicsVariant(const std::vector<uint3>& value) noexcept;
	GraphicsVariant(const std::vector<uint4>& value) noexcept;
	GraphicsVariant(const std::vector<float1>& value) noexcept;
	GraphicsVariant(const std::vector<float2>& value) noexcept;
	GraphicsVariant(const std::vector<float3>& value) noexcept;
	GraphicsVariant(const std::vector<float4>& value) noexcept;
	GraphicsVariant(GraphicsTexturePtr texture, GraphicsSamplerPtr sampler = nullptr) noexcept;
	virtual ~GraphicsVariant() noexcept;

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
	uint getUint() const noexcept;
	const uint2& getUint2() const noexcept;
	const uint3& getUint3() const noexcept;
	const uint4& getUint4() const noexcept;
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
	const std::vector<uint1>& getUintArray() const noexcept;
	const std::vector<uint2>& getUint2Array() const noexcept;
	const std::vector<uint3>& getUint3Array() const noexcept;
	const std::vector<uint4>& getUint4Array() const noexcept;
	const std::vector<float1>& getFloatArray() const noexcept;
	const std::vector<float2>& getFloat2Array() const noexcept;
	const std::vector<float3>& getFloat3Array() const noexcept;
	const std::vector<float4>& getFloat4Array() const noexcept;
	const std::vector<float3x3>& getFloat3x3Array() const noexcept;
	const std::vector<float4x4>& getFloat4x4Array() const noexcept;
	GraphicsTexturePtr getTexture() const noexcept;
	GraphicsSamplerPtr getTextureSampler() const noexcept;

private:
	GraphicsVariant(const GraphicsVariant&) noexcept = delete;
	GraphicsVariant& operator=(const GraphicsVariant&) noexcept = delete;

private:
	struct TexturePack
	{
		GraphicsTexturePtr image;
		GraphicsSamplerPtr sampler;
	};

	union
	{
		bool b;
		int i[4];
		uint1 ui[4];
		float f[4];
		float3x3* m3;
		float4x4* m4;
		std::vector<int1>* iarray;
		std::vector<int2>* iarray2;
		std::vector<int3>* iarray3;
		std::vector<int4>* iarray4;
		std::vector<uint1>* uiarray;
		std::vector<uint2>* uiarray2;
		std::vector<uint3>* uiarray3;
		std::vector<uint4>* uiarray4;
		std::vector<float1>* farray;
		std::vector<float2>* farray2;
		std::vector<float3>* farray3;
		std::vector<float4>* farray4;
		std::vector<float3x3>* m3array;
		std::vector<float4x4>* m4array;
		TexturePack* texture;
	} _value;

	GraphicsUniformType _type;
};

_NAME_END

#endif