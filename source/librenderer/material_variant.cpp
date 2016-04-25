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
#include <ray/material_variant.h>

_NAME_BEGIN

MaterialVariant::MaterialVariant() noexcept
	: _type(GraphicsUniformType::GraphicsUniformTypeNone)
{
}

MaterialVariant::MaterialVariant(GraphicsUniformType type) noexcept
{
	this->setType(type);
}

MaterialVariant::~MaterialVariant() noexcept
{
	this->setType(GraphicsUniformType::GraphicsUniformTypeNone);
}

void
MaterialVariant::setType(GraphicsUniformType type) noexcept
{
	if (_type != type)
	{
		if (_type == GraphicsUniformType::GraphicsUniformTypeIntArray)
		{
			delete _value.iarray;
			_value.iarray = nullptr;
		}
		else if (_type == GraphicsUniformType::GraphicsUniformTypeInt2Array)
		{
			delete _value.iarray2;
			_value.iarray2 = nullptr;
		}
		else if (_type == GraphicsUniformType::GraphicsUniformTypeInt3Array)
		{
			delete _value.iarray3;
			_value.iarray3 = nullptr;
		}
		else if (_type == GraphicsUniformType::GraphicsUniformTypeInt4Array)
		{
			delete _value.iarray4;
			_value.iarray4 = nullptr;
		}
		else if (_type == GraphicsUniformType::GraphicsUniformTypeUIntArray)
		{
			delete _value.uiarray;
			_value.uiarray = nullptr;
		}
		else if (_type == GraphicsUniformType::GraphicsUniformTypeUInt2Array)
		{
			delete _value.uiarray2;
			_value.uiarray2 = nullptr;
		}
		else if (_type == GraphicsUniformType::GraphicsUniformTypeUInt3Array)
		{
			delete _value.uiarray3;
			_value.uiarray3 = nullptr;
		}
		else if (_type == GraphicsUniformType::GraphicsUniformTypeUInt4Array)
		{
			delete _value.uiarray4;
			_value.uiarray4 = nullptr;
		}
		else if (_type == GraphicsUniformType::GraphicsUniformTypeFloatArray)
		{
			delete _value.farray;
			_value.farray = nullptr;
		}
		else if (_type == GraphicsUniformType::GraphicsUniformTypeFloat2Array)
		{
			delete _value.farray2;
			_value.farray2 = nullptr;
		}
		else if (_type == GraphicsUniformType::GraphicsUniformTypeFloat3Array)
		{
			delete _value.farray3;
			_value.farray3 = nullptr;
		}
		else if (_type == GraphicsUniformType::GraphicsUniformTypeFloat4Array)
		{
			delete _value.farray4;
			_value.farray4 = nullptr;
		}
		else if (_type == GraphicsUniformType::GraphicsUniformTypeFloat2x2)
		{
			delete _value.m2;
			_value.m2 = nullptr;
		}
		else if (_type == GraphicsUniformType::GraphicsUniformTypeFloat3x3)
		{
			delete _value.m3;
			_value.m3 = nullptr;
		}
		else if (_type == GraphicsUniformType::GraphicsUniformTypeFloat4x4)
		{
			delete _value.m4;
			_value.m4 = nullptr;
		}
		else if (_type == GraphicsUniformType::GraphicsUniformTypeFloat2x2Array)
		{
			delete _value.m2array;
			_value.m2array = nullptr;
		}
		else if (_type == GraphicsUniformType::GraphicsUniformTypeFloat3x3Array)
		{
			delete _value.m3array;
			_value.m3array = nullptr;
		}
		else if (_type == GraphicsUniformType::GraphicsUniformTypeFloat4x4Array)
		{
			delete _value.m4array;
			_value.m4array = nullptr;
		}
		else if (_type == GraphicsUniformType::GraphicsUniformTypeUniformBuffer)
		{
			delete _value.buffer;
			_value.buffer = nullptr;
		}
		else if (_type == GraphicsUniformType::GraphicsUniformTypeSamplerImage ||
			_type == GraphicsUniformType::GraphicsUniformTypeStorageImage ||
			_type == GraphicsUniformType::GraphicsUniformTypeCombinedImageSampler)
		{
			if (type != GraphicsUniformType::GraphicsUniformTypeSamplerImage ||
				type != GraphicsUniformType::GraphicsUniformTypeStorageImage ||
				type != GraphicsUniformType::GraphicsUniformTypeCombinedImageSampler)
			{
				delete _value.texture;
				_value.texture = nullptr;
			}
		}

		if (type == GraphicsUniformType::GraphicsUniformTypeIntArray)
			_value.iarray = new std::vector<int1>();
		else if (type == GraphicsUniformType::GraphicsUniformTypeInt2Array)
			_value.iarray2 = new std::vector<int2>();
		else if (type == GraphicsUniformType::GraphicsUniformTypeInt3Array)
			_value.iarray3 = new std::vector<int3>();
		else if (type == GraphicsUniformType::GraphicsUniformTypeInt4Array)
			_value.iarray4 = new std::vector<int4>();
		else if (type == GraphicsUniformType::GraphicsUniformTypeUIntArray)
			_value.uiarray = new std::vector<uint1>();
		else if (type == GraphicsUniformType::GraphicsUniformTypeUInt2Array)
			_value.uiarray2 = new std::vector<uint2>();
		else if (type == GraphicsUniformType::GraphicsUniformTypeUInt3Array)
			_value.uiarray3 = new std::vector<uint3>();
		else if (type == GraphicsUniformType::GraphicsUniformTypeUInt4Array)
			_value.uiarray4 = new std::vector<uint4>();
		else if (type == GraphicsUniformType::GraphicsUniformTypeFloatArray)
			_value.farray = new std::vector<float1>();
		else if (type == GraphicsUniformType::GraphicsUniformTypeFloat2Array)
			_value.farray2 = new std::vector<float2>();
		else if (type == GraphicsUniformType::GraphicsUniformTypeFloat3Array)
			_value.farray3 = new std::vector<float3>();
		else if (type == GraphicsUniformType::GraphicsUniformTypeFloat4Array)
			_value.farray4 = new std::vector<float4>();
		else if (type == GraphicsUniformType::GraphicsUniformTypeFloat2x2)
			_value.m2 = new float2x2;
		else if (type == GraphicsUniformType::GraphicsUniformTypeFloat3x3)
			_value.m3 = new float3x3;
		else if (type == GraphicsUniformType::GraphicsUniformTypeFloat4x4)
			_value.m4 = new float4x4;
		else if (type == GraphicsUniformType::GraphicsUniformTypeFloat3x3Array)
			_value.m3array = new std::vector<float3x3>;
		else if (type == GraphicsUniformType::GraphicsUniformTypeFloat4x4Array)
			_value.m4array = new std::vector<float4x4>;
		else if (type == GraphicsUniformType::GraphicsUniformTypeUniformBuffer)
			_value.buffer = new GraphicsDataPtr;
		else if (type == GraphicsUniformType::GraphicsUniformTypeSamplerImage ||
			type == GraphicsUniformType::GraphicsUniformTypeStorageImage ||
			type == GraphicsUniformType::GraphicsUniformTypeCombinedImageSampler)
		{
			if (_type != GraphicsUniformType::GraphicsUniformTypeSamplerImage ||
				_type != GraphicsUniformType::GraphicsUniformTypeStorageImage ||
				_type != GraphicsUniformType::GraphicsUniformTypeCombinedImageSampler)
			{
				_value.texture = new TexturePack;
			}
			else
			{
				_value.texture->image = nullptr;
				_value.texture->sampler = nullptr;
			}
		}

		_type = type;
	}
}

GraphicsUniformType
MaterialVariant::getType() const noexcept
{
	return _type;
}

void
MaterialVariant::uniform1b(bool b1) noexcept
{
	assert(_type == GraphicsUniformType::GraphicsUniformTypeBool);
	_value.b = b1;
}

void
MaterialVariant::uniform1i(std::int32_t i1) noexcept
{
	assert(_type == GraphicsUniformType::GraphicsUniformTypeInt);
	_value.i[0] = i1;
}

void
MaterialVariant::uniform2i(const int2& value) noexcept
{
	assert(_type == GraphicsUniformType::GraphicsUniformTypeInt2);
	_value.i[0] = value.x;
	_value.i[1] = value.y;
}

void
MaterialVariant::uniform2i(std::int32_t i1, std::int32_t i2) noexcept
{
	assert(_type == GraphicsUniformType::GraphicsUniformTypeInt2);
	_value.i[0] = i1;
	_value.i[1] = i2;
}

void
MaterialVariant::uniform3i(const int3& value) noexcept
{
	assert(_type == GraphicsUniformType::GraphicsUniformTypeInt3);
	_value.i[0] = value.x;
	_value.i[1] = value.y;
	_value.i[2] = value.z;
}

void
MaterialVariant::uniform3i(std::int32_t i1, std::int32_t i2, std::int32_t i3) noexcept
{
	assert(_type == GraphicsUniformType::GraphicsUniformTypeInt3);
	_value.i[0] = i1;
	_value.i[1] = i2;
	_value.i[2] = i3;
}

void
MaterialVariant::uniform4i(const int4& value) noexcept
{
	assert(_type == GraphicsUniformType::GraphicsUniformTypeInt4);
	_value.i[0] = value.x;
	_value.i[1] = value.y;
	_value.i[2] = value.z;
	_value.i[3] = value.w;
}

void
MaterialVariant::uniform4i(std::int32_t i1, std::int32_t i2, std::int32_t i3, std::int32_t i4) noexcept
{
	assert(_type == GraphicsUniformType::GraphicsUniformTypeInt3);
	_value.i[0] = i1;
	_value.i[1] = i2;
	_value.i[2] = i3;
	_value.i[3] = i4;
}

void
MaterialVariant::uniform1ui(std::uint32_t ui1) noexcept
{
	assert(_type == GraphicsUniformType::GraphicsUniformTypeFloat);
	_value.ui[0] = ui1;
}

void
MaterialVariant::uniform2ui(const uint2& value) noexcept
{
	assert(_type == GraphicsUniformType::GraphicsUniformTypeUInt2);
	_value.ui[0] = value.x;
	_value.ui[1] = value.y;
}

void
MaterialVariant::uniform2ui(std::uint32_t ui1, std::uint32_t ui2) noexcept
{
	assert(_type == GraphicsUniformType::GraphicsUniformTypeFloat2);
	_value.ui[0] = ui1;
	_value.ui[1] = ui2;
}

void
MaterialVariant::uniform3ui(const uint3& value) noexcept
{
	assert(_type == GraphicsUniformType::GraphicsUniformTypeUInt3);
	_value.ui[0] = value.x;
	_value.ui[1] = value.y;
	_value.ui[2] = value.z;
}

void
MaterialVariant::uniform3ui(std::uint32_t ui1, std::uint32_t ui2, std::uint32_t ui3) noexcept
{
	assert(_type == GraphicsUniformType::GraphicsUniformTypeFloat3);
	_value.ui[0] = ui1;
	_value.ui[1] = ui2;
	_value.ui[2] = ui3;
}

void
MaterialVariant::uniform4ui(const uint4& value) noexcept
{
	assert(_type == GraphicsUniformType::GraphicsUniformTypeUInt4);
	_value.ui[0] = value.x;
	_value.ui[1] = value.y;
	_value.ui[2] = value.z;
	_value.ui[3] = value.w;
}

void
MaterialVariant::uniform4ui(std::uint32_t ui1, std::uint32_t ui2, std::uint32_t ui3, std::uint32_t ui4) noexcept
{
	assert(_type == GraphicsUniformType::GraphicsUniformTypeFloat3);
	_value.ui[0] = ui1;
	_value.ui[1] = ui2;
	_value.ui[2] = ui3;
	_value.ui[3] = ui4;
}

void
MaterialVariant::uniform1f(float f1) noexcept
{
	assert(_type == GraphicsUniformType::GraphicsUniformTypeFloat);
	_value.f[0] = f1;
}

void
MaterialVariant::uniform2f(const float2& value) noexcept
{
	assert(_type == GraphicsUniformType::GraphicsUniformTypeFloat2);
	_value.f[0] = value.x;
	_value.f[1] = value.y;
}

void
MaterialVariant::uniform2f(float f1, float f2) noexcept
{
	assert(_type == GraphicsUniformType::GraphicsUniformTypeFloat2);
	_value.f[0] = f1;
	_value.f[1] = f2;
}

void
MaterialVariant::uniform3f(const float3& value) noexcept
{
	assert(_type == GraphicsUniformType::GraphicsUniformTypeFloat3);
	_value.f[0] = value.x;
	_value.f[1] = value.y;
	_value.f[2] = value.z;
}

void
MaterialVariant::uniform3f(float f1, float f2, float f3) noexcept
{
	assert(_type == GraphicsUniformType::GraphicsUniformTypeFloat3);
	_value.f[0] = f1;
	_value.f[1] = f2;
	_value.f[2] = f3;
}

void
MaterialVariant::uniform4f(const float4& value) noexcept
{
	assert(_type == GraphicsUniformType::GraphicsUniformTypeFloat4);
	_value.f[0] = value.x;
	_value.f[1] = value.y;
	_value.f[2] = value.z;
	_value.f[3] = value.w;
}

void
MaterialVariant::uniform4f(float f1, float f2, float f3, float f4) noexcept
{
	assert(_type == GraphicsUniformType::GraphicsUniformTypeFloat3);
	_value.f[0] = f1;
	_value.f[1] = f2;
	_value.f[2] = f3;
	_value.f[3] = f4;
}

void
MaterialVariant::uniform2fmat(const float2x2& value) noexcept
{
	assert(_type == GraphicsUniformType::GraphicsUniformTypeFloat2x2);
	*_value.m2 = value;
}

void
MaterialVariant::uniform2fmat(const float* mat2) noexcept
{
	assert(_type == GraphicsUniformType::GraphicsUniformTypeFloat2x2);
	std::memcpy(_value.m2, mat2, sizeof(float2x2));
}

void
MaterialVariant::uniform3fmat(const float3x3& value) noexcept
{
	assert(_type == GraphicsUniformType::GraphicsUniformTypeFloat3x3);
	*_value.m3 = value;
}

void
MaterialVariant::uniform3fmat(const float* mat3) noexcept
{
	assert(_type == GraphicsUniformType::GraphicsUniformTypeFloat3x3);
	std::memcpy(_value.m3, mat3, sizeof(float3x3));
}

void
MaterialVariant::uniform4fmat(const float4x4& value) noexcept
{
	assert(_type == GraphicsUniformType::GraphicsUniformTypeFloat4x4);
	*_value.m4 = value;
}
void
MaterialVariant::uniform4fmat(const float* mat4) noexcept
{
	assert(_type == GraphicsUniformType::GraphicsUniformTypeFloat4x4);
	std::memcpy(_value.m4, mat4, sizeof(float4x4));
}

void
MaterialVariant::uniform1iv(const std::vector<int1>& value) noexcept
{
	assert(_type == GraphicsUniformType::GraphicsUniformTypeIntArray);
	*_value.iarray = value;
}

void
MaterialVariant::uniform1iv(std::size_t num, const std::int32_t* str) noexcept
{
	assert(_type == GraphicsUniformType::GraphicsUniformTypeIntArray);
	_value.iarray->resize(num);
	std::memcpy(_value.iarray->data(), str, sizeof(int1) * num);
}

void
MaterialVariant::uniform2iv(const std::vector<int2>& value) noexcept
{
	assert(_type == GraphicsUniformType::GraphicsUniformTypeInt2Array);
	*_value.iarray2 = value;
}

void
MaterialVariant::uniform2iv(std::size_t num, const std::int32_t* str) noexcept
{
	assert(_type == GraphicsUniformType::GraphicsUniformTypeInt2Array);
	_value.iarray2->resize(num);
	std::memcpy(_value.iarray2->data(), str, sizeof(int2) * num);
}

void
MaterialVariant::uniform3iv(const std::vector<int3>& value) noexcept
{
	assert(_type == GraphicsUniformType::GraphicsUniformTypeInt3Array);
	*_value.iarray3 = value;
}

void
MaterialVariant::uniform3iv(std::size_t num, const std::int32_t* str) noexcept
{
	assert(_type == GraphicsUniformType::GraphicsUniformTypeInt2Array);
	_value.iarray3->resize(num);
	std::memcpy(_value.iarray3->data(), str, sizeof(int3) * num);
}

void
MaterialVariant::uniform4iv(const std::vector<int4>& value) noexcept
{
	assert(_type == GraphicsUniformType::GraphicsUniformTypeInt4Array);
	*_value.iarray4 = value;
}

void
MaterialVariant::uniform4iv(std::size_t num, const std::int32_t* str) noexcept
{
	assert(_type == GraphicsUniformType::GraphicsUniformTypeInt2Array);
	_value.iarray4->resize(num);
	std::memcpy(_value.iarray4->data(), str, sizeof(int4) * num);
}

void
MaterialVariant::uniform1uiv(const std::vector<uint1>& value) noexcept
{
	assert(_type == GraphicsUniformType::GraphicsUniformTypeUIntArray);
	*_value.uiarray = value;
}

void
MaterialVariant::uniform1uiv(std::size_t num, const std::uint32_t* str) noexcept
{
	assert(_type == GraphicsUniformType::GraphicsUniformTypeUIntArray);
	_value.uiarray->resize(num);
	std::memcpy(_value.uiarray->data(), str, sizeof(uint1) * num);
}

void
MaterialVariant::uniform2uiv(const std::vector<uint2>& value) noexcept
{
	assert(_type == GraphicsUniformType::GraphicsUniformTypeUInt2Array);
	*_value.uiarray2 = value;
}

void
MaterialVariant::uniform2uiv(std::size_t num, const std::uint32_t* str) noexcept
{
	assert(_type == GraphicsUniformType::GraphicsUniformTypeUInt2Array);
	_value.uiarray2->resize(num);
	std::memcpy(_value.uiarray2->data(), str, sizeof(uint2) * num);
}

void
MaterialVariant::uniform3uiv(const std::vector<uint3>& value) noexcept
{
	assert(_type == GraphicsUniformType::GraphicsUniformTypeUInt3Array);
	*_value.uiarray3 = value;
}

void
MaterialVariant::uniform3uiv(std::size_t num, const std::uint32_t* str) noexcept
{
	assert(_type == GraphicsUniformType::GraphicsUniformTypeUInt3Array);
	_value.uiarray3->resize(num);
	std::memcpy(_value.uiarray3->data(), str, sizeof(uint3) * num);
}

void
MaterialVariant::uniform4uiv(const std::vector<uint4>& value) noexcept
{
	assert(_type == GraphicsUniformType::GraphicsUniformTypeUInt4Array);
	*_value.uiarray4 = value;
}

void
MaterialVariant::uniform4uiv(std::size_t num, const std::uint32_t* str) noexcept
{
	assert(_type == GraphicsUniformType::GraphicsUniformTypeUInt4Array);
	_value.uiarray4->resize(num);
	std::memcpy(_value.uiarray4->data(), str, sizeof(uint4) * num);
}

void
MaterialVariant::uniform1fv(const std::vector<float1>& value) noexcept
{
	assert(_type == GraphicsUniformType::GraphicsUniformTypeFloatArray);
	*_value.farray = value;
}

void
MaterialVariant::uniform1fv(std::size_t num, const float* str) noexcept
{
	assert(_type == GraphicsUniformType::GraphicsUniformTypeFloatArray);
	_value.farray->resize(num);
	std::memcpy(_value.farray->data(), str, sizeof(float1) * num);
}

void
MaterialVariant::uniform2fv(const std::vector<float2>& value) noexcept
{
	assert(_type == GraphicsUniformType::GraphicsUniformTypeFloat2Array);
	*_value.farray2 = value;
}

void
MaterialVariant::uniform2fv(std::size_t num, const float* str) noexcept
{
	assert(_type == GraphicsUniformType::GraphicsUniformTypeFloat2Array);
	_value.farray2->resize(num);
	std::memcpy(_value.farray2->data(), str, sizeof(float2) * num);
}

void
MaterialVariant::uniform3fv(const std::vector<float3>& value) noexcept
{
	assert(_type == GraphicsUniformType::GraphicsUniformTypeFloat3Array);
	*_value.farray3 = value;
}

void
MaterialVariant::uniform3fv(std::size_t num, const float* str) noexcept
{
	assert(_type == GraphicsUniformType::GraphicsUniformTypeFloat3Array);
	_value.farray3->resize(num);
	std::memcpy(_value.farray3->data(), str, sizeof(float3) * num);
}

void
MaterialVariant::uniform4fv(const std::vector<float4>& value) noexcept
{
	assert(_type == GraphicsUniformType::GraphicsUniformTypeFloat4Array);
	*_value.farray4 = value;
}

void
MaterialVariant::uniform4fv(std::size_t num, const float* str) noexcept
{
	assert(_type == GraphicsUniformType::GraphicsUniformTypeFloat4Array);
	_value.farray4->resize(num);
	std::memcpy(_value.farray4->data(), str, sizeof(float4) * num);
}

void
MaterialVariant::uniform2fmatv(const std::vector<float2x2>& value) noexcept
{
	assert(_type == GraphicsUniformType::GraphicsUniformTypeFloat2x2Array);
	*_value.m2array = value;
}

void
MaterialVariant::uniform2fmatv(std::size_t num, const float* mat2) noexcept
{
	assert(_type == GraphicsUniformType::GraphicsUniformTypeFloat4Array);
	_value.m2array->resize(num);
	std::memcpy(_value.m2array->data(), mat2, sizeof(float2x2) * num);
}

void
MaterialVariant::uniform3fmatv(const std::vector<float3x3>& value) noexcept
{
	assert(_type == GraphicsUniformType::GraphicsUniformTypeFloat3x3Array);
	*_value.m3array = value;
}

void
MaterialVariant::uniform3fmatv(std::size_t num, const float* mat3) noexcept
{
	assert(_type == GraphicsUniformType::GraphicsUniformTypeFloat4Array);
	_value.m3array->resize(num);
	std::memcpy(_value.m3array->data(), mat3, sizeof(float3x3) * num);
}

void
MaterialVariant::uniform4fmatv(const std::vector<float4x4>& value) noexcept
{
	assert(_type == GraphicsUniformType::GraphicsUniformTypeFloat4x4Array);
	*_value.m4array = value;
}

void
MaterialVariant::uniform4fmatv(std::size_t num, const float* mat4) noexcept
{
	assert(_type == GraphicsUniformType::GraphicsUniformTypeFloat4Array);
	_value.m4array->resize(num);
	std::memcpy(_value.m4array->data(), mat4, sizeof(float4x4) * num);
}

void
MaterialVariant::uniformTexture(GraphicsTexturePtr texture, GraphicsSamplerPtr sampler) noexcept
{
	assert(_type == GraphicsUniformType::GraphicsUniformTypeStorageImage);
	_value.texture->image = texture;
	_value.texture->sampler = sampler;
}

void
MaterialVariant::uniformBuffer(GraphicsDataPtr buffer) noexcept
{
	assert(_type == GraphicsUniformType::GraphicsUniformTypeUniformBuffer);
	*_value.buffer = buffer;
}

bool
MaterialVariant::getBool() const noexcept
{
	assert(_type == GraphicsUniformType::GraphicsUniformTypeBool);
	return _value.b;
}

int
MaterialVariant::getInt() const noexcept
{
	assert(_type == GraphicsUniformType::GraphicsUniformTypeInt);
	return _value.i[0];
}

const int2&
MaterialVariant::getInt2() const noexcept
{
	assert(_type == GraphicsUniformType::GraphicsUniformTypeInt2);
	return (int2&)_value.i;
}

const int3&
MaterialVariant::getInt3() const noexcept
{
	assert(_type == GraphicsUniformType::GraphicsUniformTypeInt3);
	return (int3&)_value.i;
}

const int4&
MaterialVariant::getInt4() const noexcept
{
	assert(_type == GraphicsUniformType::GraphicsUniformTypeInt4);
	return (int4&)_value.i;
}

uint1
MaterialVariant::getUInt() const noexcept
{
	assert(_type == GraphicsUniformType::GraphicsUniformTypeUInt);
	return _value.ui[0];
}

const uint2&
MaterialVariant::getUInt2() const noexcept
{
	assert(_type == GraphicsUniformType::GraphicsUniformTypeUInt2);
	return (uint2&)_value.ui;
}

const uint3&
MaterialVariant::getUInt3() const noexcept
{
	assert(_type == GraphicsUniformType::GraphicsUniformTypeUInt3);
	return (uint3&)_value.ui;
}

const uint4&
MaterialVariant::getUInt4() const noexcept
{
	assert(_type == GraphicsUniformType::GraphicsUniformTypeUInt4);
	return (uint4&)_value.ui;
}

float
MaterialVariant::getFloat() const noexcept
{
	assert(_type == GraphicsUniformType::GraphicsUniformTypeFloat);
	return _value.f[0];
}

const float2&
MaterialVariant::getFloat2() const noexcept
{
	assert(_type == GraphicsUniformType::GraphicsUniformTypeFloat2);
	return (float2&)_value.f;
}

const float3&
MaterialVariant::getFloat3() const noexcept
{
	assert(_type == GraphicsUniformType::GraphicsUniformTypeFloat3);
	return (float3&)_value.f;
}

const float4&
MaterialVariant::getFloat4() const noexcept
{
	assert(_type == GraphicsUniformType::GraphicsUniformTypeFloat4);
	return (float4&)_value.f;
}

const float2x2&
MaterialVariant::getFloat2x2() const noexcept
{
	assert(_type == GraphicsUniformType::GraphicsUniformTypeFloat2x2);
	return (float2x2&)_value.m2;
}

const float3x3&
MaterialVariant::getFloat3x3() const noexcept
{
	assert(_type == GraphicsUniformType::GraphicsUniformTypeFloat3x3);
	return (float3x3&)*_value.m3;
}

const float4x4&
MaterialVariant::getFloat4x4() const noexcept
{
	assert(_type == GraphicsUniformType::GraphicsUniformTypeFloat4x4);
	return (float4x4&)*_value.m4;
}

const std::vector<int1>&
MaterialVariant::getIntArray() const noexcept
{
	assert(_type == GraphicsUniformType::GraphicsUniformTypeIntArray);
	return *_value.iarray;
}

const std::vector<int2>&
MaterialVariant::getInt2Array() const noexcept
{
	assert(_type == GraphicsUniformType::GraphicsUniformTypeInt2Array);
	return *_value.iarray2;
}

const std::vector<int3>&
MaterialVariant::getInt3Array() const noexcept
{
	assert(_type == GraphicsUniformType::GraphicsUniformTypeInt3Array);
	return *_value.iarray3;
}

const std::vector<int4>&
MaterialVariant::getInt4Array() const noexcept
{
	assert(_type == GraphicsUniformType::GraphicsUniformTypeInt4Array);
	return *_value.iarray4;
}

const std::vector<uint1>&
MaterialVariant::getUIntArray() const noexcept
{
	assert(_type == GraphicsUniformType::GraphicsUniformTypeUIntArray);
	return *_value.uiarray;
}

const std::vector<uint2>&
MaterialVariant::getUInt2Array() const noexcept
{
	assert(_type == GraphicsUniformType::GraphicsUniformTypeUInt2Array);
	return *_value.uiarray2;
}

const std::vector<uint3>&
MaterialVariant::getUInt3Array() const noexcept
{
	assert(_type == GraphicsUniformType::GraphicsUniformTypeUInt3Array);
	return *_value.uiarray3;
}

const std::vector<uint4>&
MaterialVariant::getUInt4Array() const noexcept
{
	assert(_type == GraphicsUniformType::GraphicsUniformTypeUInt4Array);
	return *_value.uiarray4;
}

const std::vector<float>&
MaterialVariant::getFloatArray() const noexcept
{
	assert(_type == GraphicsUniformType::GraphicsUniformTypeFloatArray);
	return *_value.farray;
}

const std::vector<float2>&
MaterialVariant::getFloat2Array() const noexcept
{
	assert(_type == GraphicsUniformType::GraphicsUniformTypeFloat2Array);
	return *_value.farray2;
}

const std::vector<float3>&
MaterialVariant::getFloat3Array() const noexcept
{
	assert(_type == GraphicsUniformType::GraphicsUniformTypeFloat3Array);
	return *_value.farray3;
}

const std::vector<float4>&
MaterialVariant::getFloat4Array() const noexcept
{
	assert(_type == GraphicsUniformType::GraphicsUniformTypeFloat4Array);
	return *_value.farray4;
}

const std::vector<float2x2>&
MaterialVariant::getFloat2x2Array() const noexcept
{
	assert(_type == GraphicsUniformType::GraphicsUniformTypeFloat2x2Array);
	return *_value.m2array;
}

const std::vector<float3x3>&
MaterialVariant::getFloat3x3Array() const noexcept
{
	assert(_type == GraphicsUniformType::GraphicsUniformTypeFloat3x3Array);
	return *_value.m3array;
}

const std::vector<float4x4>&
MaterialVariant::getFloat4x4Array() const noexcept
{
	assert(_type == GraphicsUniformType::GraphicsUniformTypeFloat4x4Array);
	return *_value.m4array;
}


GraphicsTexturePtr
MaterialVariant::getTexture() const noexcept
{
	assert(_type == GraphicsUniformType::GraphicsUniformTypeSamplerImage || _type == GraphicsUniformType::GraphicsUniformTypeStorageImage);
	return _value.texture->image;
}

GraphicsSamplerPtr
MaterialVariant::getTextureSampler() const noexcept
{
	assert(_type == GraphicsUniformType::GraphicsUniformTypeSamplerImage || _type == GraphicsUniformType::GraphicsUniformTypeStorageImage);
	return _value.texture->sampler;
}

GraphicsDataPtr
MaterialVariant::getBuffer() const noexcept
{
	assert(_type == GraphicsUniformType::GraphicsUniformTypeUniformBuffer);
	return *_value.buffer;
}

void 
MaterialVariant::copy(const MaterialVariant& other) noexcept
{
	this->setType(other.getType());

	switch (_type)
	{
	case GraphicsUniformType::GraphicsUniformTypeBool:
		_value.b = other._value.b;
		break;
	case GraphicsUniformType::GraphicsUniformTypeInt:
		_value.i[0] = other._value.i[0];
		break;
	case GraphicsUniformType::GraphicsUniformTypeInt2:
		_value.i[0] = other._value.i[0];
		_value.i[1] = other._value.i[1];
		break;
	case GraphicsUniformType::GraphicsUniformTypeInt3:
		_value.i[0] = other._value.i[0];
		_value.i[1] = other._value.i[1];
		_value.i[2] = other._value.i[2];
		break;
	case GraphicsUniformType::GraphicsUniformTypeInt4:
		_value.i[0] = other._value.i[0];
		_value.i[1] = other._value.i[1];
		_value.i[2] = other._value.i[2];
		_value.i[3] = other._value.i[3];
		break;
	case GraphicsUniformType::GraphicsUniformTypeUInt:
		_value.ui[0] = other._value.ui[0];
		break;
	case GraphicsUniformType::GraphicsUniformTypeUInt2:
		_value.ui[0] = other._value.ui[0];
		_value.ui[1] = other._value.ui[1];
		break;
	case GraphicsUniformType::GraphicsUniformTypeUInt3:
		_value.ui[0] = other._value.ui[0];
		_value.ui[1] = other._value.ui[1];
		_value.ui[2] = other._value.ui[2];
		break;
	case GraphicsUniformType::GraphicsUniformTypeUInt4:
		_value.ui[0] = other._value.ui[0];
		_value.ui[1] = other._value.ui[1];
		_value.ui[2] = other._value.ui[2];
		_value.ui[3] = other._value.ui[3];
		break;
	case GraphicsUniformType::GraphicsUniformTypeFloat:
		_value.f[0] = other._value.f[0];
		break;
	case GraphicsUniformType::GraphicsUniformTypeFloat2:
		_value.f[0] = other._value.f[0];
		_value.f[1] = other._value.f[1];
		break;
	case GraphicsUniformType::GraphicsUniformTypeFloat3:
		_value.f[0] = other._value.f[0];
		_value.f[1] = other._value.f[1];
		_value.f[2] = other._value.f[2];
		break;
	case GraphicsUniformType::GraphicsUniformTypeFloat4:
		_value.f[0] = other._value.f[0];
		_value.f[1] = other._value.f[1];
		_value.f[2] = other._value.f[2];
		_value.f[3] = other._value.f[3];
		break;
	case GraphicsUniformType::GraphicsUniformTypeFloat2x2:
		std::memcpy(_value.m2, other._value.m2, sizeof(float2x2));
		break;
	case GraphicsUniformType::GraphicsUniformTypeFloat3x3:
		std::memcpy(_value.m3, other._value.m3, sizeof(float3x3));
		break;
	case GraphicsUniformType::GraphicsUniformTypeFloat4x4:
		std::memcpy(_value.m4, other._value.m4, sizeof(float4x4));
		break;
	case GraphicsUniformType::GraphicsUniformTypeIntArray:
		*_value.iarray = *other._value.iarray;
		break;
	case GraphicsUniformType::GraphicsUniformTypeInt2Array:
		*_value.iarray2 = *other._value.iarray2;
		break;
	case GraphicsUniformType::GraphicsUniformTypeInt3Array:
		*_value.iarray3 = *other._value.iarray3;
		break;
	case GraphicsUniformType::GraphicsUniformTypeInt4Array:
		*_value.iarray4 = *other._value.iarray4;
		break;
	case GraphicsUniformType::GraphicsUniformTypeUIntArray:
		*_value.uiarray = *other._value.uiarray;
		break;
	case GraphicsUniformType::GraphicsUniformTypeUInt2Array:
		*_value.uiarray2 = *other._value.uiarray2;
		break;
	case GraphicsUniformType::GraphicsUniformTypeUInt3Array:
		*_value.uiarray3 = *other._value.uiarray3;
		break;
	case GraphicsUniformType::GraphicsUniformTypeUInt4Array:
		*_value.uiarray4 = *other._value.uiarray4;
		break;
	case GraphicsUniformType::GraphicsUniformTypeFloatArray:
		*_value.farray = *other._value.farray;
		break;
	case GraphicsUniformType::GraphicsUniformTypeFloat2Array:
		*_value.farray2 = *other._value.farray2;
		break;
	case GraphicsUniformType::GraphicsUniformTypeFloat3Array:
		*_value.farray3 = *other._value.farray3;
		break;
	case GraphicsUniformType::GraphicsUniformTypeFloat4Array:
		*_value.farray3 = *other._value.farray3;
		break;
	case GraphicsUniformType::GraphicsUniformTypeFloat2x2Array:
		*_value.m2array = *other._value.m2array;
		break;
	case GraphicsUniformType::GraphicsUniformTypeFloat3x3Array:
		*_value.m3array = *other._value.m3array;
		break;
	case GraphicsUniformType::GraphicsUniformTypeFloat4x4Array:
		*_value.m4array = *other._value.m4array;
		break;
	case GraphicsUniformType::GraphicsUniformTypeSampler:
		*_value.texture = *other._value.texture;
		break;
	case GraphicsUniformType::GraphicsUniformTypeSamplerImage:
		*_value.texture = *other._value.texture;
		break;
	case GraphicsUniformType::GraphicsUniformTypeCombinedImageSampler:
		*_value.texture = *other._value.texture;
		break;
	case GraphicsUniformType::GraphicsUniformTypeStorageImage:
		*_value.texture = *other._value.texture;
		break;
	case GraphicsUniformType::GraphicsUniformTypeStorageTexelBuffer:
		*_value.buffer = *other._value.buffer;
		break;
	case GraphicsUniformType::GraphicsUniformTypeStorageBuffer:
		*_value.buffer = *other._value.buffer;
		break;
	case GraphicsUniformType::GraphicsUniformTypeStorageBufferDynamic:
		*_value.buffer = *other._value.buffer;
		break;
	case GraphicsUniformType::GraphicsUniformTypeUniformTexelBuffer:
		*_value.buffer = *other._value.buffer;
		break;
	case GraphicsUniformType::GraphicsUniformTypeUniformBuffer:
		*_value.buffer = *other._value.buffer;
		break;
	case GraphicsUniformType::GraphicsUniformTypeUniformBufferDynamic:
		*_value.buffer = *other._value.buffer;
		break;
	case GraphicsUniformType::GraphicsUniformTypeInputAttachment:
		break;
	default:
		break;
	}
}

_NAME_END