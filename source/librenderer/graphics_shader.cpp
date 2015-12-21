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
#include <ray/graphics_shader.h>

_NAME_BEGIN

__ImplementSubInterface(GraphicsShader, GraphicsChild, "GraphicsShader")
__ImplementSubInterface(GraphicsProgram, GraphicsChild, "GraphicsProgram")

ShaderVariant::ShaderVariant() noexcept
{
}

ShaderVariant::~ShaderVariant() noexcept
{
}

ShaderParameter::ShaderParameter() noexcept
	: _needUpdate(true)
{
}

ShaderParameter::~ShaderParameter() noexcept
{
}

void
ShaderParameter::setName(const std::string& name) noexcept
{
	_name = name;
}

const std::string&
ShaderParameter::getName() const noexcept
{
	return _name;
}

void
ShaderParameter::needUpdate(bool update) noexcept
{
	_needUpdate = update;
}

bool
ShaderParameter::needUpdate() const noexcept
{
	return _needUpdate;
}

ShaderUniform::ShaderUniform(ShaderVariant* value) noexcept
	: _value(value)
{
}

ShaderUniform::~ShaderUniform() noexcept
{
}

void
ShaderUniform::setType(ShaderVariantType type) noexcept
{
	_type = type;
}

ShaderVariantType
ShaderUniform::getType() const noexcept
{
	return _type;
}

void
ShaderUniform::assign(bool value) noexcept
{
	assert(_value);
	_value->assign(value);
}

void
ShaderUniform::assign(int value) noexcept
{
	assert(_value);
	_value->assign(value);
}

void
ShaderUniform::assign(const int2& value) noexcept
{
	assert(_value);
	_value->assign(value);
}

void
ShaderUniform::assign(float value) noexcept
{
	assert(_value);
	_value->assign(value);
}

void
ShaderUniform::assign(const float2& value) noexcept
{
	assert(_value);
	_value->assign(value);
}

void
ShaderUniform::assign(const float3& value) noexcept
{
	assert(_value);
	_value->assign(value);
}

void
ShaderUniform::assign(const float4& value) noexcept
{
	assert(_value);
	_value->assign(value);
}

void
ShaderUniform::assign(const float3x3& value) noexcept
{
	assert(_value);
	_value->assign(value);
}

void
ShaderUniform::assign(const float4x4& value) noexcept
{
	assert(_value);
	_value->assign(value);
}

void
ShaderUniform::assign(const std::vector<float>& value) noexcept
{
	assert(_value);
	_value->assign(value);
}

void
ShaderUniform::assign(const std::vector<float2>& value) noexcept
{
	assert(_value);
	_value->assign(value);
}

void
ShaderUniform::assign(const std::vector<float3>& value) noexcept
{
	assert(_value);
	_value->assign(value);
}

void
ShaderUniform::assign(const std::vector<float4>& value) noexcept
{
	assert(_value);
	_value->assign(value);
}

ShaderDesc::ShaderDesc() noexcept
{
}

ShaderDesc::ShaderDesc(ShaderType type, const std::string& code) noexcept
{
	this->setType(type);
	this->setSource(code);
}

ShaderDesc::~ShaderDesc() noexcept
{
}

void
ShaderDesc::setType(ShaderType type) noexcept
{
	_type = type;
}

void
ShaderDesc::setSource(const std::string& source) noexcept
{
	_source = source;
}

ShaderType
ShaderDesc::getType()const noexcept
{
	return _type;
}

const std::string&
ShaderDesc::getSource() const noexcept
{
	return _source;
}

ShaderObjectDesc::ShaderObjectDesc() noexcept
{
}

ShaderObjectDesc::~ShaderObjectDesc() noexcept
{
}

void
ShaderObjectDesc::addShader(const ShaderDesc& shader) noexcept
{
	if (shader.getSource().empty())
		return;

	auto it = std::find_if(_shaders.begin(), _shaders.end(), [&](const ShaderDesc& shaderDesc) { return shaderDesc.getType() == shader.getType();});
	if (it == _shaders.end())
		_shaders.push_back(shader);
}

void
ShaderObjectDesc::removeShader(ShaderType type) noexcept
{
	auto it = std::find_if(_shaders.begin(), _shaders.end(), [&](const ShaderDesc& shaderDesc) { return shaderDesc.getType() == type;});
	if (it != _shaders.end())
		_shaders.erase(it);
}

const ShadersDesc&
ShaderObjectDesc::getShaders() const noexcept
{
	return _shaders;
}

GraphicsShader::GraphicsShader() noexcept
{
}

GraphicsShader::~GraphicsShader() noexcept
{
}

GraphicsProgram::GraphicsProgram() noexcept
{
}

GraphicsProgram::~GraphicsProgram() noexcept
{
}

_NAME_END