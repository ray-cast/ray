// +----------------------------------------------------------------------
// | Project : ray.
// | All rights reserved.
// +----------------------------------------------------------------------
// | Copyright (c) 2013-2017.
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

MaterialParamListener::MaterialParamListener() noexcept
{
}

MaterialParamListener::~MaterialParamListener() noexcept
{
}

MaterialParam::MaterialParam() noexcept
	: _semanticType(GlobalSemanticType::GlobalSemanticTypeNone)
{
}

MaterialParam::MaterialParam(const std::string& name, GraphicsUniformType type) noexcept
	: _name(name)
	, _semanticType(GlobalSemanticType::GlobalSemanticTypeNone)
	, _variant(type)
{
}

MaterialParam::MaterialParam(std::string&& name, GraphicsUniformType type) noexcept
	: _name(std::move(name))
	, _semanticType(GlobalSemanticType::GlobalSemanticTypeNone)
	, _variant(type)
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

void
MaterialParam::setName(std::string&& name) noexcept
{
	_name = std::move(name);
}

const std::string&
MaterialParam::getName() const noexcept
{
	return _name;
}

void
MaterialParam::setType(GraphicsUniformType type) noexcept
{
	_variant.setType(type);
}

GraphicsUniformType
MaterialParam::getType() const noexcept
{
	return _variant.getType();
}

void
MaterialParam::setSemanticType(GlobalSemanticType type) noexcept
{
	_semanticType = type;
}

GlobalSemanticType
MaterialParam::getSemanticType() const noexcept
{
	return _semanticType;
}

void
MaterialParam::uniform1b(bool value) noexcept
{
	for (auto& it : _listeners)
		it->uniform1b(value);
	_variant.uniform1b(value);
}

void
MaterialParam::uniform1i(std::int32_t value) noexcept
{
	for (auto& it : _listeners)
		it->uniform1i(value);
	_variant.uniform1i(value);
}

void
MaterialParam::uniform2i(const int2& value) noexcept
{
	for (auto& it : _listeners)
		it->uniform2i(value);
	_variant.uniform2i(value);
}

void
MaterialParam::uniform2i(std::int32_t i1, std::int32_t i2) noexcept
{
	for (auto& it : _listeners)
		it->uniform2i(i1, i2);
	_variant.uniform2i(i1, i2);
}

void
MaterialParam::uniform3i(const int3& value) noexcept
{
	for (auto& it : _listeners)
		it->uniform3i(value);
	_variant.uniform3i(value);
}

void
MaterialParam::uniform3i(std::int32_t i1, std::int32_t i2, std::int32_t i3) noexcept
{
	for (auto& it : _listeners)
		it->uniform3i(i1, i2, i3);
	_variant.uniform3i(i1, i2, i3);
}

void
MaterialParam::uniform4i(const int4& value) noexcept
{
	for (auto& it : _listeners)
		it->uniform4i(value);
	_variant.uniform4i(value);
}

void
MaterialParam::uniform4i(std::int32_t i1, std::int32_t i2, std::int32_t i3, std::int32_t i4) noexcept
{
	for (auto& it : _listeners)
		it->uniform4i(i1, i2, i3, i4);
	_variant.uniform4i(i1, i2, i3, i4);
}

void
MaterialParam::uniform1ui(std::uint32_t value) noexcept
{
	for (auto& it : _listeners)
		it->uniform1ui(value);
	_variant.uniform1ui(value);
}

void
MaterialParam::uniform2ui(const uint2& value) noexcept
{
	for (auto& it : _listeners)
		it->uniform2ui(value);
	_variant.uniform2ui(value);
}

void
MaterialParam::uniform2ui(std::uint32_t ui1, std::uint32_t ui2) noexcept
{
	for (auto& it : _listeners)
		it->uniform2ui(ui1, ui2);
	_variant.uniform2ui(ui1, ui2);
}

void
MaterialParam::uniform3ui(const uint3& value) noexcept
{
	for (auto& it : _listeners)
		it->uniform3ui(value);
	_variant.uniform3ui(value);
}

void
MaterialParam::uniform3ui(std::uint32_t ui1, std::uint32_t ui2, std::uint32_t ui3) noexcept
{
	for (auto& it : _listeners)
		it->uniform3ui(ui1, ui2, ui3);
	_variant.uniform3ui(ui1, ui2, ui3);
}

void
MaterialParam::uniform4ui(const uint4& value) noexcept
{
	for (auto& it : _listeners)
		it->uniform4ui(value);
	_variant.uniform4ui(value);
}

void
MaterialParam::uniform4ui(std::uint32_t ui1, std::uint32_t ui2, std::uint32_t ui3, std::uint32_t ui4) noexcept
{
	for (auto& it : _listeners)
		it->uniform4ui(ui1, ui2, ui3, ui4);
	_variant.uniform4ui(ui1, ui2, ui3, ui4);
}

void
MaterialParam::uniform1f(float value) noexcept
{
	for (auto& it : _listeners)
		it->uniform1f(value);
	_variant.uniform1f(value);
}

void
MaterialParam::uniform2f(const float2& value) noexcept
{
	for (auto& it : _listeners)
		it->uniform2f(value);
	_variant.uniform2f(value);
}

void
MaterialParam::uniform2f(float f1, float f2) noexcept
{
	for (auto& it : _listeners)
		it->uniform2f(f1, f2);
	_variant.uniform2f(f1, f2);
}

void
MaterialParam::uniform3f(const float3& value) noexcept
{
	for (auto& it : _listeners)
		it->uniform3f(value);
	_variant.uniform3f(value);
}

void
MaterialParam::uniform3f(float f1, float f2, float f3) noexcept
{
	for (auto& it : _listeners)
		it->uniform3f(f1, f2, f3);
	_variant.uniform3f(f1, f2, f3);
}

void
MaterialParam::uniform4f(const float4& value) noexcept
{
	for (auto& it : _listeners)
		it->uniform4f(value);
	_variant.uniform4f(value);
}

void
MaterialParam::uniform4f(float f1, float f2, float f3, float f4) noexcept
{
	for (auto& it : _listeners)
		it->uniform4f(f1, f2, f3, f4);
	_variant.uniform4f(f1, f2, f3, f4);
}

void
MaterialParam::uniform2fmat(const float2x2& value) noexcept
{
	for (auto& it : _listeners)
		it->uniform2fmat(value);
	_variant.uniform2fmat(value);
}

void
MaterialParam::uniform2fmat(const float* mat2) noexcept
{
	for (auto& it : _listeners)
		it->uniform2fmat(mat2);
	_variant.uniform2fmat(mat2);
}

void
MaterialParam::uniform3fmat(const float3x3& value) noexcept
{
	for (auto& it : _listeners)
		it->uniform3fmat(value);
	_variant.uniform3fmat(value);
}

void
MaterialParam::uniform3fmat(const float* value) noexcept
{
	for (auto& it : _listeners)
		it->uniform3fmat(value);
	_variant.uniform3fmat(value);
}

void
MaterialParam::uniform4fmat(const float4x4& value) noexcept
{
	for (auto& it : _listeners)
		it->uniform4fmat(value);
	_variant.uniform4fmat(value);
}

void
MaterialParam::uniform4fmat(const float* value) noexcept
{
	for (auto& it : _listeners)
		it->uniform4fmat(value);
	_variant.uniform4fmat(value);
}

void
MaterialParam::uniform1iv(std::size_t num, const std::int32_t* i1v) noexcept
{
	for (auto& it : _listeners)
		it->uniform1iv(num, i1v);
	_variant.uniform1iv(num, i1v);
}

void
MaterialParam::uniform2iv(std::size_t num, const std::int32_t* value) noexcept
{
	for (auto& it : _listeners)
		it->uniform2iv(num, value);
	_variant.uniform2iv(num, value);
}

void
MaterialParam::uniform3iv(std::size_t num, const std::int32_t* value) noexcept
{
	for (auto& it : _listeners)
		it->uniform3iv(num, value);
	_variant.uniform3iv(num, value);
}

void
MaterialParam::uniform4iv(std::size_t num, const std::int32_t* value) noexcept
{
	for (auto& it : _listeners)
		it->uniform4iv(num, value);
	_variant.uniform4iv(num, value);
}

void
MaterialParam::uniform1uiv(std::size_t num, const std::uint32_t* value) noexcept
{
	for (auto& it : _listeners)
		it->uniform1uiv(num, value);
	_variant.uniform1uiv(num, value);
}

void
MaterialParam::uniform2uiv(std::size_t num, const std::uint32_t* value) noexcept
{
	for (auto& it : _listeners)
		it->uniform2uiv(num, value);
	_variant.uniform2uiv(num, value);
}

void
MaterialParam::uniform3uiv(std::size_t num, const std::uint32_t* value) noexcept
{
	for (auto& it : _listeners)
		it->uniform3uiv(num, value);
	_variant.uniform3uiv(num, value);
}

void
MaterialParam::uniform4uiv(std::size_t num, const std::uint32_t* value) noexcept
{
	for (auto& it : _listeners)
		it->uniform4uiv(num, value);
	_variant.uniform4uiv(num, value);
}

void
MaterialParam::uniform1fv(std::size_t num, const float* value) noexcept
{
	for (auto& it : _listeners)
		it->uniform1fv(num, value);
	_variant.uniform1fv(num, value);
}

void
MaterialParam::uniform2fv(std::size_t num, const float* value) noexcept
{
	for (auto& it : _listeners)
		it->uniform2fv(num, value);
	_variant.uniform2fv(num, value);
}

void
MaterialParam::uniform3fv(std::size_t num, const float* value) noexcept
{
	for (auto& it : _listeners)
		it->uniform3fv(num, value);
	_variant.uniform3fv(num, value);
}

void
MaterialParam::uniform4fv(std::size_t num, const float* value) noexcept
{
	for (auto& it : _listeners)
		it->uniform4fv(num, value);
	_variant.uniform4fv(num, value);
}

void
MaterialParam::uniform2fmatv(std::size_t num, const float* value) noexcept
{
	for (auto& it : _listeners)
		it->uniform2fmatv(num, value);
	_variant.uniform2fmatv(num, value);
}

void
MaterialParam::uniform3fmatv(std::size_t num, const float* value) noexcept
{
	for (auto& it : _listeners)
		it->uniform3fmatv(num, value);
	_variant.uniform3fmatv(num, value);
}

void
MaterialParam::uniform4fmatv(std::size_t num, const float* value) noexcept
{
	for (auto& it : _listeners)
		it->uniform4fmatv(num, value);
	_variant.uniform4fmatv(num, value);
}

void
MaterialParam::uniform1iv(const std::vector<int1>& value) noexcept
{
	for (auto& it : _listeners)
		it->uniform1iv(value);
	_variant.uniform1iv(value);
}

void
MaterialParam::uniform2iv(const std::vector<int2>& value) noexcept
{
	for (auto& it : _listeners)
		it->uniform2iv(value);
	_variant.uniform2iv(value);
}

void
MaterialParam::uniform3iv(const std::vector<int3>& value) noexcept
{
	for (auto& it : _listeners)
		it->uniform3iv(value);
	_variant.uniform3iv(value);
}

void
MaterialParam::uniform4iv(const std::vector<int4>& value) noexcept
{
	for (auto& it : _listeners)
		it->uniform4iv(value);
	_variant.uniform4iv(value);
}

void
MaterialParam::uniform1uiv(const std::vector<uint1>& value) noexcept
{
	for (auto& it : _listeners)
		it->uniform1uiv(value);
	_variant.uniform1uiv(value);
}

void
MaterialParam::uniform2uiv(const std::vector<uint2>& value) noexcept
{
	for (auto& it : _listeners)
		it->uniform2uiv(value);
	_variant.uniform2uiv(value);
}

void
MaterialParam::uniform3uiv(const std::vector<uint3>& value) noexcept
{
	for (auto& it : _listeners)
		it->uniform3uiv(value);
	_variant.uniform3uiv(value);
}

void
MaterialParam::uniform4uiv(const std::vector<uint4>& value) noexcept
{
	for (auto& it : _listeners)
		it->uniform4uiv(value);
	_variant.uniform4uiv(value);
}

void
MaterialParam::uniform1fv(const std::vector<float1>& value) noexcept
{
	for (auto& it : _listeners)
		it->uniform1fv(value);
	_variant.uniform1fv(value);
}

void
MaterialParam::uniform2fv(const std::vector<float2>& value) noexcept
{
	for (auto& it : _listeners)
		it->uniform2fv(value);
	_variant.uniform2fv(value);
}

void
MaterialParam::uniform3fv(const std::vector<float3>& value) noexcept
{
	for (auto& it : _listeners)
		it->uniform3fv(value);
	_variant.uniform3fv(value);
}

void
MaterialParam::uniform4fv(const std::vector<float4>& value) noexcept
{
	for (auto& it : _listeners)
		it->uniform4fv(value);
	_variant.uniform4fv(value);
}

void
MaterialParam::uniform2fmatv(const std::vector<float2x2>& value) noexcept
{
	for (auto& it : _listeners)
		it->uniform2fmatv(value);
	_variant.uniform2fmatv(value);
}

void
MaterialParam::uniform3fmatv(const std::vector<float3x3>& value) noexcept
{
	for (auto& it : _listeners)
		it->uniform3fmatv(value);
	_variant.uniform3fmatv(value);
}

void
MaterialParam::uniform4fmatv(const std::vector<float4x4>& value) noexcept
{
	for (auto& it : _listeners)
		it->uniform4fmatv(value);
	_variant.uniform4fmatv(value);
}

void
MaterialParam::uniformTexture(GraphicsTexturePtr texture, GraphicsSamplerPtr sampler) noexcept
{
	for (auto& it : _listeners)
		it->uniformTexture(texture, sampler);
	_variant.uniformTexture(texture, sampler);
}

void
MaterialParam::uniformBuffer(GraphicsDataPtr value) noexcept
{
	for (auto& it : _listeners)
		it->uniformBuffer(value);
	_variant.uniformBuffer(value);
}

void
MaterialParam::uniformParam(const MaterialParam& params) noexcept
{
	assert(this->getType() == params.getType());

	auto type = params.getType();
	switch (type)
	{
	case ray::GraphicsUniformType::GraphicsUniformTypeBool:
		this->uniform1b(params.value().getBool());
		break;
	case ray::GraphicsUniformType::GraphicsUniformTypeInt:
		this->uniform1i(params.value().getInt());
		break;
	case ray::GraphicsUniformType::GraphicsUniformTypeInt2:
		this->uniform2i(params.value().getInt2());
		break;
	case ray::GraphicsUniformType::GraphicsUniformTypeInt3:
		this->uniform3i(params.value().getInt3());
		break;
	case ray::GraphicsUniformType::GraphicsUniformTypeInt4:
		this->uniform4i(params.value().getInt4());
		break;
	case ray::GraphicsUniformType::GraphicsUniformTypeUInt:
		this->uniform1ui(params.value().getUInt());
		break;
	case ray::GraphicsUniformType::GraphicsUniformTypeUInt2:
		this->uniform2ui(params.value().getUInt2());
		break;
	case ray::GraphicsUniformType::GraphicsUniformTypeUInt3:
		this->uniform3ui(params.value().getUInt3());
		break;
	case ray::GraphicsUniformType::GraphicsUniformTypeUInt4:
		this->uniform4ui(params.value().getUInt4());
		break;
	case ray::GraphicsUniformType::GraphicsUniformTypeFloat:
		this->uniform1f(params.value().getFloat());
		break;
	case ray::GraphicsUniformType::GraphicsUniformTypeFloat2:
		this->uniform2f(params.value().getFloat2());
		break;
	case ray::GraphicsUniformType::GraphicsUniformTypeFloat3:
		this->uniform3f(params.value().getFloat3());
		break;
	case ray::GraphicsUniformType::GraphicsUniformTypeFloat4:
		this->uniform4f(params.value().getFloat4());
		break;
	case ray::GraphicsUniformType::GraphicsUniformTypeFloat2x2:
		this->uniform2fmat(params.value().getFloat2x2());
		break;
	case ray::GraphicsUniformType::GraphicsUniformTypeFloat3x3:
		this->uniform3fmat(params.value().getFloat3x3());
		break;
	case ray::GraphicsUniformType::GraphicsUniformTypeFloat4x4:
		this->uniform4fmat(params.value().getFloat4x4());
		break;
	case ray::GraphicsUniformType::GraphicsUniformTypeIntArray:
		this->uniform1iv(params.value().getIntArray());
		break;
	case ray::GraphicsUniformType::GraphicsUniformTypeInt2Array:
		this->uniform2iv(params.value().getInt2Array());
		break;
	case ray::GraphicsUniformType::GraphicsUniformTypeInt3Array:
		this->uniform3iv(params.value().getInt3Array());
		break;
	case ray::GraphicsUniformType::GraphicsUniformTypeInt4Array:
		this->uniform4iv(params.value().getInt4Array());
		break;
	case ray::GraphicsUniformType::GraphicsUniformTypeUIntArray:
		this->uniform1uiv(params.value().getUIntArray());
		break;
	case ray::GraphicsUniformType::GraphicsUniformTypeUInt2Array:
		this->uniform2uiv(params.value().getUInt2Array());
		break;
	case ray::GraphicsUniformType::GraphicsUniformTypeUInt3Array:
		this->uniform3uiv(params.value().getUInt3Array());
		break;
	case ray::GraphicsUniformType::GraphicsUniformTypeUInt4Array:
		this->uniform4uiv(params.value().getUInt4Array());
		break;
	case ray::GraphicsUniformType::GraphicsUniformTypeFloatArray:
		this->uniform1fv(params.value().getFloatArray());
		break;
	case ray::GraphicsUniformType::GraphicsUniformTypeFloat2Array:
		this->uniform2fv(params.value().getFloat2Array());
		break;
	case ray::GraphicsUniformType::GraphicsUniformTypeFloat3Array:
		this->uniform3fv(params.value().getFloat3Array());
		break;
	case ray::GraphicsUniformType::GraphicsUniformTypeFloat4Array:
		this->uniform4fv(params.value().getFloat4Array());
		break;
	case ray::GraphicsUniformType::GraphicsUniformTypeFloat2x2Array:
		this->uniform2fmatv(params.value().getFloat2x2Array());
		break;
	case ray::GraphicsUniformType::GraphicsUniformTypeFloat3x3Array:
		this->uniform3fmatv(params.value().getFloat3x3Array());
		break;
	case ray::GraphicsUniformType::GraphicsUniformTypeFloat4x4Array:
		this->uniform4fmatv(params.value().getFloat4x4Array());
		break;
	case ray::GraphicsUniformType::GraphicsUniformTypeSampler:
	case ray::GraphicsUniformType::GraphicsUniformTypeSamplerImage:
	case ray::GraphicsUniformType::GraphicsUniformTypeStorageImage:
		this->uniformTexture(params.value().getTexture(), params.value().getTextureSampler());
		break;
	default:
		assert(false);
		break;
	}
}

const MaterialVariant&
MaterialParam::value() const noexcept
{
	return _variant;
}

void
MaterialParam::addParamListener(MaterialParamListener* listener) noexcept
{
	auto it = std::find(_listeners.begin(), _listeners.end(), listener);
	if (it == _listeners.end())
		_listeners.push_back(listener);
}

void
MaterialParam::removeParamListener(MaterialParamListener* listener) noexcept
{
	auto it = std::find(_listeners.begin(), _listeners.end(), listener);
	if (it != _listeners.end())
		_listeners.erase(it);
}

MaterialParamPtr
MaterialParam::clone() const noexcept
{
	auto param = std::make_shared<MaterialParam>();
	param->setName(this->getName());
	param->setType(this->getType());
	param->setSemanticType(this->getSemanticType());
	return param;
}

MaterialMacro::MaterialMacro() noexcept
{
}

MaterialMacro::MaterialMacro(const std::string& name, GraphicsUniformType type) noexcept
	: _name(name)
{
	_variant.setType(type);
}

MaterialMacro::MaterialMacro(std::string&& name, GraphicsUniformType type) noexcept
	: _name(std::move(name))
{
	_variant.setType(type);
}

MaterialMacro::~MaterialMacro() noexcept
{
}

void
MaterialMacro::setName(const std::string& name) noexcept
{
	_name = name;
}

void
MaterialMacro::setName(std::string&& name) noexcept
{
	_name = std::move(name);
}

const std::string&
MaterialMacro::getName() const noexcept
{
	return _name;
}

void
MaterialMacro::setType(GraphicsUniformType type) noexcept
{
	_variant.setType(type);
}

GraphicsUniformType
MaterialMacro::getType() const noexcept
{
	return _variant.getType();
}

void
MaterialMacro::uniform1b(bool value) noexcept
{
	_variant.uniform1b(value);
	for (auto& it : _listeners)
		it->uniform1b(value);
}

void
MaterialMacro::uniform1i(std::int32_t i1) noexcept
{
	_variant.uniform1i(i1);
	for (auto& it : _listeners)
		it->uniform1i(i1);
}

void
MaterialMacro::uniform2i(const int2& value) noexcept
{
	_variant.uniform2i(value);
	for (auto& it : _listeners)
		it->uniform2i(value);
}

void
MaterialMacro::uniform2i(std::int32_t i1, std::int32_t i2) noexcept
{
	_variant.uniform2i(i1, i2);
	for (auto& it : _listeners)
		it->uniform2i(i1, i2);
}

void
MaterialMacro::uniform3i(const int3& value) noexcept
{
	_variant.uniform3i(value);
	for (auto& it : _listeners)
		it->uniform3i(value);
}

void
MaterialMacro::uniform3i(std::int32_t i1, std::int32_t i2, std::int32_t i3) noexcept
{
	_variant.uniform3i(i1, i2, i3);
	for (auto& it : _listeners)
		it->uniform3i(i1, i2, i3);
}

void
MaterialMacro::uniform4i(const int4& value) noexcept
{
	_variant.uniform4i(value);
	for (auto& it : _listeners)
		it->uniform4i(value);
}

void
MaterialMacro::uniform4i(std::int32_t i1, std::int32_t i2, std::int32_t i3, std::int32_t i4) noexcept
{
	_variant.uniform4i(i1, i2, i3, i4);
	for (auto& it : _listeners)
		it->uniform4i(i1, i2, i3, i4);
}

void
MaterialMacro::uniform1ui(std::uint32_t ui1) noexcept
{
	_variant.uniform1ui(ui1);
	for (auto& it : _listeners)
		it->uniform1ui(ui1);
}

void
MaterialMacro::uniform2ui(const uint2& value) noexcept
{
	_variant.uniform2ui(value);
	for (auto& it : _listeners)
		it->uniform2ui(value);
}

void
MaterialMacro::uniform2ui(std::uint32_t ui1, std::uint32_t ui2) noexcept
{
	_variant.uniform2ui(ui1, ui2);
	for (auto& it : _listeners)
		it->uniform2ui(ui1, ui2);
}

void
MaterialMacro::uniform3ui(const uint3& value) noexcept
{
	_variant.uniform3ui(value);
	for (auto& it : _listeners)
		it->uniform3ui(value);
}

void
MaterialMacro::uniform3ui(std::uint32_t ui1, std::uint32_t ui2, std::uint32_t ui3) noexcept
{
	_variant.uniform3ui(ui1, ui2, ui3);
	for (auto& it : _listeners)
		it->uniform3ui(ui1, ui2, ui3);
}

void
MaterialMacro::uniform4ui(const uint4& value) noexcept
{
	_variant.uniform4ui(value);
	for (auto& it : _listeners)
		it->uniform4ui(value);
}

void
MaterialMacro::uniform4ui(std::uint32_t ui1, std::uint32_t ui2, std::uint32_t ui3, std::uint32_t ui4) noexcept
{
	_variant.uniform4ui(ui1, ui2, ui3, ui4);
	for (auto& it : _listeners)
		it->uniform4ui(ui1, ui2, ui3, ui4);
}

void
MaterialMacro::uniform1f(float f1) noexcept
{
	_variant.uniform1f(f1);
	for (auto& it : _listeners)
		it->uniform1f(f1);
}

void
MaterialMacro::uniform2f(const float2& value) noexcept
{
	_variant.uniform2f(value);
	for (auto& it : _listeners)
		it->uniform2f(value);
}

void
MaterialMacro::uniform2f(float f1, float f2) noexcept
{
	_variant.uniform2f(f1, f2);
	for (auto& it : _listeners)
		it->uniform2f(f1, f2);
}

void
MaterialMacro::uniform3f(const float3& value) noexcept
{
	_variant.uniform3f(value);
	for (auto& it : _listeners)
		it->uniform3f(value);
}

void
MaterialMacro::uniform3f(float f1, float f2, float f3) noexcept
{
	_variant.uniform3f(f1, f2, f3);
	for (auto& it : _listeners)
		it->uniform3f(f1, f2, f3);
}

void
MaterialMacro::uniform4f(const float4& value) noexcept
{
	_variant.uniform4f(value);
	for (auto& it : _listeners)
		it->uniform4f(value);
}

void
MaterialMacro::uniform4f(float f1, float f2, float f3, float f4) noexcept
{
	_variant.uniform4f(f1, f2, f3, f4);
	for (auto& it : _listeners)
		it->uniform4f(f1, f2, f3, f4);
}

void
MaterialMacro::uniform2fmat(const float2x2& value) noexcept
{
	_variant.uniform2fmat(value);
	for (auto& it : _listeners)
		it->uniform2fmat(value);
}

void
MaterialMacro::uniform2fmat(const float* mat2) noexcept
{
	_variant.uniform2fmat(mat2);
	for (auto& it : _listeners)
		it->uniform2fmat(mat2);
}

void
MaterialMacro::uniform3fmat(const float3x3& value) noexcept
{
	_variant.uniform3fmat(value);
	for (auto& it : _listeners)
		it->uniform3fmat(value);
}

void
MaterialMacro::uniform3fmat(const float* mat3) noexcept
{
	_variant.uniform3fmat(mat3);
	for (auto& it : _listeners)
		it->uniform3fmat(mat3);
}

void
MaterialMacro::uniform4fmat(const float4x4& value) noexcept
{
	_variant.uniform4fmat(value);
	for (auto& it : _listeners)
		it->uniform4fmat(value);
}

void
MaterialMacro::uniform4fmat(const float* mat4) noexcept
{
	_variant.uniform4fmat(mat4);
	for (auto& it : _listeners)
		it->uniform4fmat(mat4);
}

void
MaterialMacro::uniform1iv(std::size_t num, const std::int32_t* i1v) noexcept
{
	_variant.uniform1iv(num, i1v);
	for (auto& it : _listeners)
		it->uniform1iv(num, i1v);
}

void
MaterialMacro::uniform2iv(std::size_t num, const std::int32_t* i2v) noexcept
{
	_variant.uniform2iv(num, i2v);
	for (auto& it : _listeners)
		it->uniform2iv(num, i2v);
}

void
MaterialMacro::uniform3iv(std::size_t num, const std::int32_t* i3v) noexcept
{
	_variant.uniform3iv(num, i3v);
	for (auto& it : _listeners)
		it->uniform3iv(num, i3v);
}

void
MaterialMacro::uniform4iv(std::size_t num, const std::int32_t* i4v) noexcept
{
	_variant.uniform4iv(num, i4v);
	for (auto& it : _listeners)
		it->uniform4iv(num, i4v);
}

void
MaterialMacro::uniform1uiv(std::size_t num, const std::uint32_t* value) noexcept
{
	_variant.uniform1uiv(num, value);
	for (auto& it : _listeners)
		it->uniform1uiv(num, value);
}

void
MaterialMacro::uniform2uiv(std::size_t num, const std::uint32_t* ui2v) noexcept
{
	_variant.uniform2uiv(num, ui2v);
	for (auto& it : _listeners)
		it->uniform2uiv(num, ui2v);
}

void
MaterialMacro::uniform3uiv(std::size_t num, const std::uint32_t* ui3v) noexcept
{
	_variant.uniform3uiv(num, ui3v);
	for (auto& it : _listeners)
		it->uniform3uiv(num, ui3v);
}

void
MaterialMacro::uniform4uiv(std::size_t num, const std::uint32_t* ui4v) noexcept
{
	_variant.uniform4uiv(num, ui4v);
	for (auto& it : _listeners)
		it->uniform4uiv(num, ui4v);
}

void
MaterialMacro::uniform1fv(std::size_t num, const float* f1v) noexcept
{
	_variant.uniform1fv(num, f1v);
	for (auto& it : _listeners)
		it->uniform1fv(num, f1v);
}

void
MaterialMacro::uniform2fv(std::size_t num, const float* f2v) noexcept
{
	_variant.uniform2fv(num, f2v);
	for (auto& it : _listeners)
		it->uniform2fv(num, f2v);
}

void
MaterialMacro::uniform3fv(std::size_t num, const float* f3v) noexcept
{
	_variant.uniform3fv(num, f3v);
	for (auto& it : _listeners)
		it->uniform3fv(num, f3v);
}

void
MaterialMacro::uniform4fv(std::size_t num, const float* f4v) noexcept
{
	_variant.uniform4fv(num, f4v);
	for (auto& it : _listeners)
		it->uniform4fv(num, f4v);
}

void
MaterialMacro::uniform2fmatv(std::size_t num, const float* mat2) noexcept
{
	_variant.uniform2fmatv(num, mat2);
	for (auto& it : _listeners)
		it->uniform2fmatv(num, mat2);
}

void
MaterialMacro::uniform3fmatv(std::size_t num, const float* mat3) noexcept
{
	_variant.uniform3fmatv(num, mat3);
	for (auto& it : _listeners)
		it->uniform3fmatv(num, mat3);
}

void
MaterialMacro::uniform4fmatv(std::size_t num, const float* mat4) noexcept
{
	_variant.uniform4fmatv(num, mat4);
	for (auto& it : _listeners)
		it->uniform4fmatv(num, mat4);
}

void
MaterialMacro::uniform1iv(const std::vector<int1>& value) noexcept
{
	_variant.uniform1iv(value);
	for (auto& it : _listeners)
		it->uniform1iv(value);
}

void
MaterialMacro::uniform2iv(const std::vector<int2>& value) noexcept
{
	_variant.uniform2iv(value);
	for (auto& it : _listeners)
		it->uniform2iv(value);
}

void
MaterialMacro::uniform3iv(const std::vector<int3>& value) noexcept
{
	_variant.uniform3iv(value);
	for (auto& it : _listeners)
		it->uniform3iv(value);
}

void
MaterialMacro::uniform4iv(const std::vector<int4>& value) noexcept
{
	_variant.uniform4iv(value);
	for (auto& it : _listeners)
		it->uniform4iv(value);
}

void
MaterialMacro::uniform1uiv(const std::vector<uint1>& value) noexcept
{
	_variant.uniform1uiv(value);
	for (auto& it : _listeners)
		it->uniform1uiv(value);
}

void
MaterialMacro::uniform2uiv(const std::vector<uint2>& value) noexcept
{
	_variant.uniform2uiv(value);
	for (auto& it : _listeners)
		it->uniform2uiv(value);
}

void
MaterialMacro::uniform3uiv(const std::vector<uint3>& value) noexcept
{
	_variant.uniform3uiv(value);
	for (auto& it : _listeners)
		it->uniform3uiv(value);
}

void
MaterialMacro::uniform4uiv(const std::vector<uint4>& value) noexcept
{
	_variant.uniform4uiv(value);
	for (auto& it : _listeners)
		it->uniform4uiv(value);
}

void
MaterialMacro::uniform1fv(const std::vector<float1>& value) noexcept
{
	_variant.uniform1fv(value);
	for (auto& it : _listeners)
		it->uniform1fv(value);
}

void
MaterialMacro::uniform2fv(const std::vector<float2>& value) noexcept
{
	_variant.uniform2fv(value);
	for (auto& it : _listeners)
		it->uniform2fv(value);
}

void
MaterialMacro::uniform3fv(const std::vector<float3>& value) noexcept
{
	_variant.uniform3fv(value);
	for (auto& it : _listeners)
		it->uniform3fv(value);
}

void
MaterialMacro::uniform4fv(const std::vector<float4>& value) noexcept
{
	_variant.uniform4fv(value);
	for (auto& it : _listeners)
		it->uniform4fv(value);
}

void
MaterialMacro::uniform2fmatv(const std::vector<float2x2>& value) noexcept
{
	_variant.uniform2fmatv(value);
	for (auto& it : _listeners)
		it->uniform2fmatv(value);
}

void
MaterialMacro::uniform3fmatv(const std::vector<float3x3>& value) noexcept
{
	_variant.uniform3fmatv(value);
	for (auto& it : _listeners)
		it->uniform3fmatv(value);
}

void
MaterialMacro::uniform4fmatv(const std::vector<float4x4>& value) noexcept
{
	_variant.uniform4fmatv(value);
	for (auto& it : _listeners)
		it->uniform4fmatv(value);
}

void
MaterialMacro::uniformTexture(GraphicsTexturePtr texture, GraphicsSamplerPtr sampler) noexcept
{
	_variant.uniformTexture(texture, sampler);
	for (auto& it : _listeners)
		it->uniformTexture(texture, sampler);
}

void
MaterialMacro::uniformBuffer(GraphicsDataPtr ubo) noexcept
{
	_variant.uniformBuffer(ubo);
	for (auto& it : _listeners)
		it->uniformBuffer(ubo);
}

bool
MaterialMacro::getBool() const noexcept
{
	return _variant.getBool();
}

int
MaterialMacro::getInt() const noexcept
{
	return _variant.getInt();
}

const int2&
MaterialMacro::getInt2() const noexcept
{
	return _variant.getInt2();
}

const int3&
MaterialMacro::getInt3() const noexcept
{
	return _variant.getInt3();
}

const int4&
MaterialMacro::getInt4() const noexcept
{
	return _variant.getInt4();
}

uint
MaterialMacro::getUInt() const noexcept
{
	return _variant.getUInt();
}

const uint2&
MaterialMacro::getUInt2() const noexcept
{
	return _variant.getUInt2();
}

const uint3&
MaterialMacro::getUInt3() const noexcept
{
	return _variant.getUInt3();
}

const uint4&
MaterialMacro::getUInt4() const noexcept
{
	return _variant.getUInt4();
}

float
MaterialMacro::getFloat() const noexcept
{
	return _variant.getFloat();
}

const float2&
MaterialMacro::getFloat2() const noexcept
{
	return _variant.getFloat2();
}

const float3&
MaterialMacro::getFloat3() const noexcept
{
	return _variant.getFloat3();
}

const float4&
MaterialMacro::getFloat4() const noexcept
{
	return _variant.getFloat4();
}

const float2x2&
MaterialMacro::getFloat2x2() const noexcept
{
	return _variant.getFloat2x2();
}

const float3x3&
MaterialMacro::getFloat3x3() const noexcept
{
	return _variant.getFloat3x3();
}

const float4x4&
MaterialMacro::getFloat4x4() const noexcept
{
	return _variant.getFloat4x4();
}

const std::vector<int1>&
MaterialMacro::getIntArray() const noexcept
{
	return _variant.getIntArray();
}

const std::vector<int2>&
MaterialMacro::getInt2Array() const noexcept
{
	return _variant.getInt2Array();
}

const std::vector<int3>&
MaterialMacro::getInt3Array() const noexcept
{
	return _variant.getInt3Array();
}

const std::vector<int4>&
MaterialMacro::getInt4Array() const noexcept
{
	return _variant.getInt4Array();
}

const std::vector<uint1>&
MaterialMacro::getUIntArray() const noexcept
{
	return _variant.getUIntArray();
}

const std::vector<uint2>&
MaterialMacro::getUInt2Array() const noexcept
{
	return _variant.getUInt2Array();
}

const std::vector<uint3>&
MaterialMacro::getUInt3Array() const noexcept
{
	return _variant.getUInt3Array();
}

const std::vector<uint4>&
MaterialMacro::getUInt4Array() const noexcept
{
	return _variant.getUInt4Array();
}

const std::vector<float1>&
MaterialMacro::getFloatArray() const noexcept
{
	return _variant.getFloatArray();
}

const std::vector<float2>&
MaterialMacro::getFloat2Array() const noexcept
{
	return _variant.getFloat2Array();
}

const std::vector<float3>&
MaterialMacro::getFloat3Array() const noexcept
{
	return _variant.getFloat3Array();
}

const std::vector<float4>&
MaterialMacro::getFloat4Array() const noexcept
{
	return _variant.getFloat4Array();
}

const std::vector<float2x2>&
MaterialMacro::getFloat2x2Array() const noexcept
{
	return _variant.getFloat2x2Array();
}

const std::vector<float3x3>&
MaterialMacro::getFloat3x3Array() const noexcept
{
	return _variant.getFloat3x3Array();
}

const std::vector<float4x4>&
MaterialMacro::getFloat4x4Array() const noexcept
{
	return _variant.getFloat4x4Array();
}

GraphicsTexturePtr
MaterialMacro::getTexture() const noexcept
{
	return _variant.getTexture();
}

GraphicsSamplerPtr
MaterialMacro::getTextureSampler() const noexcept
{
	return _variant.getTextureSampler();
}

GraphicsDataPtr
MaterialMacro::getBuffer() const noexcept
{
	return _variant.getBuffer();
}

void
MaterialMacro::addParamListener(MaterialParamListener* listener) noexcept
{
	auto it = std::find(_listeners.begin(), _listeners.end(), listener);
	if (it == _listeners.end())
		_listeners.push_back(listener);
}

void
MaterialMacro::removeParamListener(MaterialParamListener* listener) noexcept
{
	auto it = std::find(_listeners.begin(), _listeners.end(), listener);
	if (it != _listeners.end())
		_listeners.erase(it);
}

MaterialMacroPtr
MaterialMacro::clone() const noexcept
{
	auto macro = std::make_shared<MaterialMacro>();
	macro->_name = _name;
	macro->_variant.copy(_variant);
	return macro;
}

_NAME_END