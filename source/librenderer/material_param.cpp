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
#include <ray/material_param.h>
#include <ray/graphics_descriptor.h>

_NAME_BEGIN

MaterialParam::MaterialParam() noexcept
{
}

MaterialParam::MaterialParam(const std::string& name, GraphicsUniformType type) noexcept
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
MaterialParam::setType(GraphicsUniformType type) noexcept
{
	_type = type;
}

GraphicsUniformType
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
MaterialParam::assign(GraphicsTexturePtr texture, GraphicsSamplerPtr sampler) noexcept
{
	for (auto& it : _uniforms)
		it->assign(texture, sampler);
}

void
MaterialParam::setSemantic(MaterialVariantPtr semantic) noexcept
{
	_semantic = semantic;
}

MaterialVariantPtr
MaterialParam::getSemantic() const noexcept
{
	return _semantic;
}

void
MaterialParam::addShaderUniform(GraphicsUniformPtr uniform) noexcept
{
	assert(uniform);
	assert(std::find(_uniforms.begin(), _uniforms.end(), uniform) == _uniforms.end());
	_uniforms.push_back(uniform);
}

void
MaterialParam::removeShaderUniform(GraphicsUniformPtr uniform) noexcept
{
	assert(uniform);
	auto it = std::find(_uniforms.begin(), _uniforms.end(), uniform);
	if (it != _uniforms.end())
		_uniforms.erase(it);
}

GraphicsUniforms&
MaterialParam::getShaderUniform() noexcept
{
	return _uniforms;
}

_NAME_END