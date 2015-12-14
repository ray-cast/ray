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

_NAME_BEGIN

MaterialSemanticParam::MaterialSemanticParam(const std::string& name, ShaderVariantType type, MaterialSemantic semantic) noexcept
	: _name(name)
	, _semantic(semantic)
{
	this->setType(type);
}

MaterialSemanticParam::~MaterialSemanticParam() noexcept
{
}

void
MaterialSemanticParam::setName(const std::string& name) noexcept
{
	_name = name;
}

const std::string&
MaterialSemanticParam::getName() const noexcept
{
	return _name;
}

void
MaterialSemanticParam::setSemantic(MaterialSemantic semantic) noexcept
{
	_semantic = semantic;
}

MaterialSemantic
MaterialSemanticParam::getSemantic() const noexcept
{
	return _semantic;
}

std::size_t
MaterialSemanticParam::getSize() const noexcept
{
	switch (_type)
	{
	case SPT_BOOL:
		return sizeof(int);
	case SPT_INT:
		return sizeof(int);
	case SPT_INT2:
		return sizeof(int2);
	case SPT_FLOAT:
		return sizeof(float);
	case SPT_FLOAT2:
		return sizeof(float2);
	case SPT_FLOAT3:
		return sizeof(float3);
	case SPT_FLOAT4:
		return sizeof(float4);
	case SPT_FLOAT3X3:
		return sizeof(float3x3);
	case SPT_FLOAT4X4:
		return sizeof(float4x4);
	default:
		assert(false);
		return 0;
	}
}

void
MaterialSemanticParam::setType(ShaderVariantType type) noexcept
{
	if (_type != type)
	{
		if (_type == ShaderVariantType::SPT_FLOAT_ARRAY)
		{
			delete _value.farray;
			_value.farray = nullptr;
		}
		else if (_type == ShaderVariantType::SPT_FLOAT2_ARRAY)
		{
			delete _value.farray2;
			_value.farray2 = nullptr;
		}
		else if (_type == ShaderVariantType::SPT_FLOAT3X3)
		{
			delete _value.m3;
			_value.m3 = nullptr;
		}
		else if (_type == ShaderVariantType::SPT_FLOAT4X4)
		{
			delete _value.m4;
			_value.m4 = nullptr;
		}

		if (type == ShaderVariantType::SPT_FLOAT_ARRAY)
		{
			_value.farray = new std::vector<float>();
		}
		else if (type == ShaderVariantType::SPT_FLOAT2_ARRAY)
		{
			_value.farray2 = new std::vector<float2>();
		}
		else if (type == ShaderVariantType::SPT_FLOAT3X3)
		{
			_value.m3 = new Matrix3x3;
		}
		else if (type == ShaderVariantType::SPT_FLOAT4X4)
		{
			_value.m4 = new Matrix4x4;
		}

		_type = type;
	}
}

ShaderVariantType
MaterialSemanticParam::getType() const noexcept
{
	return _type;
}

void
MaterialSemanticParam::assign(bool value) noexcept
{
	this->setType(ShaderVariantType::SPT_BOOL);
	if (_value.b != value)
	{
		_value.b = value;
	}
}

void
MaterialSemanticParam::assign(int value) noexcept
{
	this->setType(ShaderVariantType::SPT_INT);
	if (_value.i[0] != value)
	{
		_value.i[0] = value;
	}
}

void
MaterialSemanticParam::assign(const int2& value) noexcept
{
	this->setType(ShaderVariantType::SPT_INT2);
	if (_value.i[0] != value.x ||
		_value.i[1] != value.y)
	{
		_value.i[0] = value.x;
		_value.i[1] = value.y;
	}
}

void
MaterialSemanticParam::assign(float value) noexcept
{
	this->setType(ShaderVariantType::SPT_FLOAT);
	if (_value.f[0] != value)
	{
		_value.f[0] = value;
	}
}

void
MaterialSemanticParam::assign(const float2& value) noexcept
{
	this->setType(ShaderVariantType::SPT_FLOAT2);
	if (_value.f[0] != value.x ||
		_value.f[1] != value.y)
	{
		_value.f[0] = value.x;
		_value.f[1] = value.y;
	}
}

void
MaterialSemanticParam::assign(const float3& value) noexcept
{
	this->setType(ShaderVariantType::SPT_FLOAT3);
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
MaterialSemanticParam::assign(const float4& value) noexcept
{
	this->setType(ShaderVariantType::SPT_FLOAT4);
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
MaterialSemanticParam::assign(const float3x3& value) noexcept
{
	this->setType(ShaderVariantType::SPT_FLOAT3X3);
	*_value.m3 = value;
}

void
MaterialSemanticParam::assign(const float4x4& value) noexcept
{
	this->setType(ShaderVariantType::SPT_FLOAT4X4);
	*_value.m4 = value;
}

void
MaterialSemanticParam::assign(const std::vector<float>& value) noexcept
{
	this->setType(ShaderVariantType::SPT_FLOAT_ARRAY);
	*_value.farray = value;
}

void
MaterialSemanticParam::assign(const std::vector<float2>& value) noexcept
{
	this->setType(ShaderVariantType::SPT_FLOAT2_ARRAY);
	*_value.farray2 = value;
}

void
MaterialSemanticParam::assign(const std::vector<float3>& value) noexcept
{
	this->setType(ShaderVariantType::SPT_FLOAT3_ARRAY);
	*_value.farray3 = value;
}

void
MaterialSemanticParam::assign(const std::vector<float4>& value) noexcept
{
	this->setType(ShaderVariantType::SPT_FLOAT4_ARRAY);
	*_value.farray4 = value;
}

void
MaterialSemanticParam::assign(TexturePtr texture, TextureSamplerPtr sampler) noexcept
{
	this->setType(ShaderVariantType::SPT_TEXTURE);
	if (_texture != texture)
	{
		_texture = texture;
		_textureSampler = sampler;
	}
}

bool
MaterialSemanticParam::getBool() const noexcept
{
	assert(_type == ShaderVariantType::SPT_BOOL);
	return _value.b;
}

int
MaterialSemanticParam::getInt() const noexcept
{
	assert(_type == ShaderVariantType::SPT_INT);
	return _value.i[0];
}

const int2&
MaterialSemanticParam::getInt2() const noexcept
{
	assert(_type == ShaderVariantType::SPT_INT2);
	return (int2&)_value.i[0];
}

float
MaterialSemanticParam::getFloat() const noexcept
{
	assert(_type == ShaderVariantType::SPT_FLOAT);
	return _value.f[0];
}

const float2&
MaterialSemanticParam::getFloat2() const noexcept
{
	assert(_type == ShaderVariantType::SPT_FLOAT2);
	return (float2&)_value.f[0];
}

const float3&
MaterialSemanticParam::getFloat3() const noexcept
{
	assert(_type == ShaderVariantType::SPT_FLOAT3);
	return (float3&)_value.f[0];
}

const float4&
MaterialSemanticParam::getFloat4() const noexcept
{
	assert(_type == ShaderVariantType::SPT_FLOAT4);
	return (float4&)_value.f[0];
}

const float3x3&
MaterialSemanticParam::getFloat3x3() const noexcept
{
	assert(_type == ShaderVariantType::SPT_FLOAT3X3);
	return (float3x3&)*_value.m3;
}

const float4x4&
MaterialSemanticParam::getFloat4x4() const noexcept
{
	assert(_type == ShaderVariantType::SPT_FLOAT4X4);
	return (float4x4&)*_value.m4;
}

const std::vector<float>&
MaterialSemanticParam::getFloatArray() const noexcept
{
	assert(_type == ShaderVariantType::SPT_FLOAT_ARRAY);
	return *_value.farray;
}

const std::vector<float2>&
MaterialSemanticParam::getFloat2Array() const noexcept
{
	assert(_type == ShaderVariantType::SPT_FLOAT2_ARRAY);
	return *_value.farray2;
}

const std::vector<float3>&
MaterialSemanticParam::getFloat3Array() const noexcept
{
	assert(_type == ShaderVariantType::SPT_FLOAT3_ARRAY);
	return *_value.farray3;
}

const std::vector<float4>&
MaterialSemanticParam::getFloat4Array() const noexcept
{
	assert(_type == ShaderVariantType::SPT_FLOAT4_ARRAY);
	return *_value.farray4;
}

TexturePtr
MaterialSemanticParam::getTexture() const noexcept
{
	assert(_type == ShaderVariantType::SPT_TEXTURE);
	return _texture;
}

TextureSamplerPtr
MaterialSemanticParam::getTextureSampler() const noexcept
{
	assert(_type == ShaderVariantType::SPT_TEXTURE);
	return _textureSampler;
}

MaterialParam::MaterialParam() noexcept
	: _semantic(MaterialSemantic::NotSemantic)
{
}

MaterialParam::MaterialParam(const std::string& name, ShaderVariantType type, MaterialSemantic semantic) noexcept
	: _name(name)
	, _type(type)
	, _semantic(semantic)
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
MaterialParam::setType(ShaderVariantType type) noexcept
{
	_type = type;
}

ShaderVariantType
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
MaterialParam::assign(TexturePtr texture, TextureSamplerPtr sampler) noexcept
{
	_texture = texture;
}

TexturePtr 
MaterialParam::getTexture() const noexcept
{
	return _texture;
}

void
MaterialParam::setSemantic(MaterialSemantic semantic) noexcept
{
	if (_semantic != semantic)
	{
		if (semantic == MaterialSemantic::matModel ||
			semantic == MaterialSemantic::matModelInverse ||
			semantic == MaterialSemantic::matModelInverseTranspose ||
			semantic == MaterialSemantic::matProject ||
			semantic == MaterialSemantic::matProjectInverse ||
			semantic == MaterialSemantic::matView ||
			semantic == MaterialSemantic::matViewInverse ||
			semantic == MaterialSemantic::matViewInverseTranspose ||
			semantic == MaterialSemantic::matViewProject ||
			semantic == MaterialSemantic::matViewProjectInverse)
		{
			this->setType(ShaderVariantType::SPT_FLOAT4X4);
		}
		else if (semantic == MaterialSemantic::CameraAperture ||
			semantic == MaterialSemantic::CameraNear ||
			semantic == MaterialSemantic::CameraFar)
		{
			this->setType(ShaderVariantType::SPT_FLOAT);
		}
		else if (semantic == MaterialSemantic::CameraView ||
			semantic == MaterialSemantic::CameraPosition ||
			semantic == MaterialSemantic::CameraDirection)
		{
			this->setType(ShaderVariantType::SPT_FLOAT3);
		}
		else if (semantic == MaterialSemantic::DeferredDepthLinearMap ||
			semantic == MaterialSemantic::DeferredDepthMap ||
			semantic == MaterialSemantic::DeferredGraphicMap ||
			semantic == MaterialSemantic::DeferredNormalMap ||
			semantic == MaterialSemantic::DeferredLightMap ||
			semantic == MaterialSemantic::DeferredShadowMap ||
			semantic == MaterialSemantic::DepthMap ||
			semantic == MaterialSemantic::ColorMap ||
			semantic == MaterialSemantic::NormalMap)
		{
			this->setType(ShaderVariantType::SPT_TEXTURE);
		}
		else
		{
			assert(false);
		}

		_semantic = semantic;
	}
}

MaterialSemantic
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