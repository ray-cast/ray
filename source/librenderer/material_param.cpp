// +----------------------------------------------------------------------
// | Project : ray.
// | All rights reserved.
// +----------------------------------------------------------------------
// | Copyright (c) 2013-2015.
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
#include <ray/material_param.h>
#include <ray/graphics_shader.h>

_NAME_BEGIN

MaterialVariant::MaterialVariant() noexcept
	: _type(GraphicsVariantType::GraphicsVariantTypeNone)
{
}

MaterialVariant::MaterialVariant(const std::string& name, GraphicsVariantType type) noexcept
	: _name(name)
{
	this->setType(type);
}

MaterialVariant::~MaterialVariant() noexcept
{
}

void
MaterialVariant::setName(const std::string& name) noexcept
{
	_name = name;
}

const std::string&
MaterialVariant::getName() const noexcept
{
	return _name;
}

std::size_t
MaterialVariant::getSize() const noexcept
{
	switch (_type)
	{
	case GraphicsVariantType::GraphicsVariantTypeBool:
		return sizeof(int);
	case GraphicsVariantType::GraphicsVariantTypeShort:
		return sizeof(short1);
	case GraphicsVariantType::GraphicsVariantTypeShort2:
		return sizeof(short2);
	case GraphicsVariantType::GraphicsVariantTypeShort3:
		return sizeof(short3);
	case GraphicsVariantType::GraphicsVariantTypeShort4:
		return sizeof(short4);
	case GraphicsVariantType::GraphicsVariantTypeUShort:
		return sizeof(ushort1);
	case GraphicsVariantType::GraphicsVariantTypeUShort2:
		return sizeof(ushort2);
	case GraphicsVariantType::GraphicsVariantTypeUShort3:
		return sizeof(ushort3);
	case GraphicsVariantType::GraphicsVariantTypeUShort4:
		return sizeof(ushort4);
	case GraphicsVariantType::GraphicsVariantTypeInt:
		return sizeof(int);
	case GraphicsVariantType::GraphicsVariantTypeInt2:
		return sizeof(int2);
	case GraphicsVariantType::GraphicsVariantTypeInt3:
		return sizeof(int3);
	case GraphicsVariantType::GraphicsVariantTypeInt4:
		return sizeof(int4);
	case GraphicsVariantType::GraphicsVariantTypeUInt:
		return sizeof(uint);
	case GraphicsVariantType::GraphicsVariantTypeUInt2:
		return sizeof(uint2);
	case GraphicsVariantType::GraphicsVariantTypeUInt3:
		return sizeof(uint3);
	case GraphicsVariantType::GraphicsVariantTypeUInt4:
		return sizeof(uint4);
	case GraphicsVariantType::GraphicsVariantTypeFloat:
		return sizeof(float);
	case GraphicsVariantType::GraphicsVariantTypeFloat2:
		return sizeof(float2);
	case GraphicsVariantType::GraphicsVariantTypeFloat3:
		return sizeof(float3);
	case GraphicsVariantType::GraphicsVariantTypeFloat4:
		return sizeof(float4);
	case GraphicsVariantType::GraphicsVariantTypeFloat3x3:
		return sizeof(float3x3);
	case GraphicsVariantType::GraphicsVariantTypeFloat4x4:
		return sizeof(float4x4);
	default:
		assert(false);
		return 0;
	}
}

void
MaterialVariant::setType(GraphicsVariantType type) noexcept
{
	if (_type != type)
	{
		if (_type == GraphicsVariantType::GraphicsVariantTypeFloatArray)
		{
			delete _value.farray;
			_value.farray = nullptr;
		}
		else if (_type == GraphicsVariantType::GraphicsVariantTypeFloat2Array)
		{
			delete _value.farray2;
			_value.farray2 = nullptr;
		}
		else if (_type == GraphicsVariantType::GraphicsVariantTypeFloat3x3)
		{
			delete _value.m3;
			_value.m3 = nullptr;
		}
		else if (_type == GraphicsVariantType::GraphicsVariantTypeFloat4x4)
		{
			delete _value.m4;
			_value.m4 = nullptr;
		}

		if (type == GraphicsVariantType::GraphicsVariantTypeFloatArray)
		{
			_value.farray = new std::vector<float>();
		}
		else if (type == GraphicsVariantType::GraphicsVariantTypeFloat2Array)
		{
			_value.farray2 = new std::vector<float2>();
		}
		else if (type == GraphicsVariantType::GraphicsVariantTypeFloat3x3)
		{
			_value.m3 = new Matrix3x3;
		}
		else if (type == GraphicsVariantType::GraphicsVariantTypeFloat4x4)
		{
			_value.m4 = new Matrix4x4;
		}

		_type = type;
	}
}

GraphicsVariantType
MaterialVariant::getType() const noexcept
{
	return _type;
}

void
MaterialVariant::assign(bool value) noexcept
{
	this->setType(GraphicsVariantType::GraphicsVariantTypeBool);
	if (_value.b != value)
	{
		_value.b = value;
	}
}

void
MaterialVariant::assign(int value) noexcept
{
	this->setType(GraphicsVariantType::GraphicsVariantTypeInt);
	if (_value.i[0] != value)
	{
		_value.i[0] = value;
	}
}

void
MaterialVariant::assign(const int2& value) noexcept
{
	this->setType(GraphicsVariantType::GraphicsVariantTypeInt2);
	if (_value.i[0] != value.x ||
		_value.i[1] != value.y)
	{
		_value.i[0] = value.x;
		_value.i[1] = value.y;
	}
}

void
MaterialVariant::assign(const int3& value) noexcept
{
	this->setType(GraphicsVariantType::GraphicsVariantTypeInt3);
	if (_value.i[0] != value.x ||
		_value.i[1] != value.y ||
		_value.i[2] != value.z)
	{
		_value.i[0] = value.x;
		_value.i[1] = value.y;
		_value.i[2] = value.z;
	}
}

void
MaterialVariant::assign(const int4& value) noexcept
{
	this->setType(GraphicsVariantType::GraphicsVariantTypeInt3);
	if (_value.i[0] != value.x ||
		_value.i[1] != value.y ||
		_value.i[2] != value.z ||
		_value.i[3] != value.w)
	{
		_value.i[0] = value.x;
		_value.i[1] = value.y;
		_value.i[1] = value.z;
		_value.i[1] = value.w;
	}
}

void
MaterialVariant::assign(float value) noexcept
{
	this->setType(GraphicsVariantType::GraphicsVariantTypeFloat);
	if (_value.f[0] != value)
	{
		_value.f[0] = value;
	}
}

void
MaterialVariant::assign(const float2& value) noexcept
{
	this->setType(GraphicsVariantType::GraphicsVariantTypeFloat2);
	if (_value.f[0] != value.x ||
		_value.f[1] != value.y)
	{
		_value.f[0] = value.x;
		_value.f[1] = value.y;
	}
}

void
MaterialVariant::assign(const float3& value) noexcept
{
	this->setType(GraphicsVariantType::GraphicsVariantTypeFloat3);
	if (_value.f[0] != value.x ||
		_value.f[1] != value.y ||
		_value.f[2] != value.z)
	{
		_value.f[0] = value.x;
		_value.f[1] = value.y;
		_value.f[2] = value.z;
	}
}

void
MaterialVariant::assign(const float4& value) noexcept
{
	this->setType(GraphicsVariantType::GraphicsVariantTypeFloat4);
	if (_value.f[0] != value.x ||
		_value.f[1] != value.y ||
		_value.f[2] != value.z ||
		_value.f[3] != value.w)
	{
		_value.f[0] = value.x;
		_value.f[1] = value.y;
		_value.f[2] = value.z;
		_value.f[3] = value.w;
	}
}

void
MaterialVariant::assign(const float3x3& value) noexcept
{
	this->setType(GraphicsVariantType::GraphicsVariantTypeFloat3x3);
	*_value.m3 = value;
}

void
MaterialVariant::assign(const float4x4& value) noexcept
{
	this->setType(GraphicsVariantType::GraphicsVariantTypeFloat4x4);
	*_value.m4 = value;
}

void
MaterialVariant::assign(const std::vector<float>& value) noexcept
{
	this->setType(GraphicsVariantType::GraphicsVariantTypeFloatArray);
	*_value.farray = value;
}

void
MaterialVariant::assign(const std::vector<float2>& value) noexcept
{
	this->setType(GraphicsVariantType::GraphicsVariantTypeFloat2Array);
	*_value.farray2 = value;
}

void
MaterialVariant::assign(const std::vector<float3>& value) noexcept
{
	this->setType(GraphicsVariantType::GraphicsVariantTypeFloat3Array);
	*_value.farray3 = value;
}

void
MaterialVariant::assign(const std::vector<float4>& value) noexcept
{
	this->setType(GraphicsVariantType::GraphicsVariantTypeFloat4Array);
	*_value.farray4 = value;
}

void
MaterialVariant::assign(GraphicsTexturePtr texture, GraphicsSamplerPtr sampler) noexcept
{
	this->setType(GraphicsVariantType::GraphicsVariantTypeTexture);
	if (_texture != texture)
	{
		_texture = texture;
		_textureSampler = sampler;
	}
}

bool
MaterialVariant::getBool() const noexcept
{
	assert(_type == GraphicsVariantType::GraphicsVariantTypeBool);
	return _value.b;
}

int
MaterialVariant::getInt() const noexcept
{
	assert(_type == GraphicsVariantType::GraphicsVariantTypeInt);
	return _value.i[0];
}

const int2&
MaterialVariant::getInt2() const noexcept
{
	assert(_type == GraphicsVariantType::GraphicsVariantTypeInt2);
	return (int2&)_value.i[0];
}

const int3&
MaterialVariant::getInt3() const noexcept
{
	assert(_type == GraphicsVariantType::GraphicsVariantTypeInt3);
	return (int3&)_value.i[0];
}

const int4&
MaterialVariant::getInt4() const noexcept
{
	assert(_type == GraphicsVariantType::GraphicsVariantTypeInt4);
	return (int4&)_value.i[0];
}

float
MaterialVariant::getFloat() const noexcept
{
	assert(_type == GraphicsVariantType::GraphicsVariantTypeFloat);
	return _value.f[0];
}

const float2&
MaterialVariant::getFloat2() const noexcept
{
	assert(_type == GraphicsVariantType::GraphicsVariantTypeFloat2);
	return (float2&)_value.f[0];
}

const float3&
MaterialVariant::getFloat3() const noexcept
{
	assert(_type == GraphicsVariantType::GraphicsVariantTypeFloat3);
	return (float3&)_value.f[0];
}

const float4&
MaterialVariant::getFloat4() const noexcept
{
	assert(_type == GraphicsVariantType::GraphicsVariantTypeFloat4);
	return (float4&)_value.f[0];
}

const float3x3&
MaterialVariant::getFloat3x3() const noexcept
{
	assert(_type == GraphicsVariantType::GraphicsVariantTypeFloat3x3);
	return (float3x3&)*_value.m3;
}

const float4x4&
MaterialVariant::getFloat4x4() const noexcept
{
	assert(_type == GraphicsVariantType::GraphicsVariantTypeFloat4x4);
	return (float4x4&)*_value.m4;
}

const std::vector<float>&
MaterialVariant::getFloatArray() const noexcept
{
	assert(_type == GraphicsVariantType::GraphicsVariantTypeFloatArray);
	return *_value.farray;
}

const std::vector<float2>&
MaterialVariant::getFloat2Array() const noexcept
{
	assert(_type == GraphicsVariantType::GraphicsVariantTypeFloat2Array);
	return *_value.farray2;
}

const std::vector<float3>&
MaterialVariant::getFloat3Array() const noexcept
{
	assert(_type == GraphicsVariantType::GraphicsVariantTypeFloat3Array);
	return *_value.farray3;
}

const std::vector<float4>&
MaterialVariant::getFloat4Array() const noexcept
{
	assert(_type == GraphicsVariantType::GraphicsVariantTypeFloat4Array);
	return *_value.farray4;
}

GraphicsTexturePtr
MaterialVariant::getTexture() const noexcept
{
	assert(_type == GraphicsVariantType::GraphicsVariantTypeTexture);
	return _texture;
}

GraphicsSamplerPtr
MaterialVariant::getTextureSampler() const noexcept
{
	assert(_type == GraphicsVariantType::GraphicsVariantTypeTexture);
	return _textureSampler;
}

MaterialSemantic::MaterialSemantic() noexcept
{
}

MaterialSemantic::MaterialSemantic(const std::string& name, GraphicsVariantType type) noexcept
	: MaterialVariant(name, type)
{
}

MaterialSemantic::~MaterialSemantic() noexcept
{
}

MaterialParam::MaterialParam() noexcept
{
}

MaterialParam::MaterialParam(const std::string& name, GraphicsVariantType type) noexcept
	: _name(name)
	, _type(type)
{
}

MaterialParam::~MaterialParam() noexcept
{
}

void
MaterialParam::setName(const std::string& name) noexcept
{
	_name = name;
}

const std::string&
MaterialParam::getName() const noexcept
{
	return _name;
}

void
MaterialParam::setType(GraphicsVariantType type) noexcept
{
	_type = type;
}

GraphicsVariantType
MaterialParam::getType() const noexcept
{
	return _type;
}

void
MaterialParam::assign(bool value) noexcept
{
	for (auto& it : _uniforms)
		it->assign(value);
}

void
MaterialParam::assign(int value) noexcept
{
	for (auto& it : _uniforms)
		it->assign(value);
}

void
MaterialParam::assign(const int2& value) noexcept
{
	for (auto& it : _uniforms)
		it->assign(value);
}

void
MaterialParam::assign(float value) noexcept
{
	for (auto& it : _uniforms)
		it->assign(value);
}

void
MaterialParam::assign(const float2& value) noexcept
{
	for (auto& it : _uniforms)
		it->assign(value);
}

void
MaterialParam::assign(const float3& value) noexcept
{
	for (auto& it : _uniforms)
		it->assign(value);
}

void
MaterialParam::assign(const float4& value) noexcept
{
	for (auto& it : _uniforms)
		it->assign(value);
}

void
MaterialParam::assign(const float3x3& value) noexcept
{
	for (auto& it : _uniforms)
		it->assign(value);
}

void
MaterialParam::assign(const float4x4& value) noexcept
{
	for (auto& it : _uniforms)
		it->assign(value);
}

void
MaterialParam::assign(const std::vector<float>& value) noexcept
{
	for (auto& it : _uniforms)
		it->assign(value);
}

void
MaterialParam::assign(const std::vector<float2>& value) noexcept
{
	for (auto& it : _uniforms)
		it->assign(value);
}

void
MaterialParam::assign(const std::vector<float3>& value) noexcept
{
	for (auto& it : _uniforms)
		it->assign(value);
}

void
MaterialParam::assign(const std::vector<float4>& value) noexcept
{
	for (auto& it : _uniforms)
		it->assign(value);
}

void
MaterialParam::assign(GraphicsTexturePtr texture) noexcept
{
	_texture = texture;
}

void
MaterialParam::assign(GraphicsSamplerPtr sampler) noexcept
{
	_sampler = sampler;
}

GraphicsTexturePtr
MaterialParam::getTexture() const noexcept
{
	return _texture;
}

GraphicsSamplerPtr
MaterialParam::getSampler() const noexcept
{
	return _sampler;
}

void
MaterialParam::setSemantic(MaterialSemanticPtr semantic) noexcept
{
	_semantic = semantic;
}

MaterialSemanticPtr
MaterialParam::getSemantic() const noexcept
{
	return _semantic;
}

void
MaterialParam::addShaderUniform(ShaderUniformPtr& uniform) noexcept
{
	assert(uniform);
	assert(std::find(_uniforms.begin(), _uniforms.end(), uniform) == _uniforms.end());
	_uniforms.push_back(uniform);
}

void
MaterialParam::removeShaderUniform(ShaderUniformPtr& uniform) noexcept
{
	assert(uniform);
	auto it = std::find(_uniforms.begin(), _uniforms.end(), uniform);
	if (it != _uniforms.end())
		_uniforms.erase(it);
}

ShaderUniforms&
MaterialParam::getShaderUniform() noexcept
{
	return _uniforms;
}

_NAME_END