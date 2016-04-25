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

_NAME_BEGIN

MaterialParamListener::MaterialParamListener() noexcept
{
}

MaterialParamListener::~MaterialParamListener() noexcept
{
}

MaterialParam::MaterialParam() noexcept
{
}

MaterialParam::MaterialParam(const std::string& name, GraphicsUniformType type) noexcept
	: _name(name)
{
	_variant.setType(type);
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
	_variant.setType(type);
}

GraphicsUniformType
MaterialParam::getType() const noexcept
{
	return _variant.getType();
}

void
MaterialParam::setSemantic(MaterialParamPtr semantic) noexcept
{
	_semantic = semantic;
}

MaterialParamPtr
MaterialParam::getSemantic() const noexcept
{
	return _semantic;
}

void
MaterialParam::uniform1b(bool value) noexcept
{
	_variant.uniform1b(value);
	this->_needUpdate();
}

void
MaterialParam::uniform1i(std::int32_t i1) noexcept
{
	_variant.uniform1i(i1);
	this->_needUpdate();
}

void
MaterialParam::uniform2i(const int2& value) noexcept
{
	_variant.uniform2i(value);
	this->_needUpdate();
}

void
MaterialParam::uniform2i(std::int32_t i1, std::int32_t i2) noexcept
{
	_variant.uniform2i(i1, i2);
	this->_needUpdate();
}

void
MaterialParam::uniform3i(const int3& value) noexcept
{
	_variant.uniform3i(value);
	this->_needUpdate();
}

void
MaterialParam::uniform3i(std::int32_t i1, std::int32_t i2, std::int32_t i3) noexcept
{
	_variant.uniform3i(i1, i2, i3);
	this->_needUpdate();
}

void
MaterialParam::uniform4i(const int4& value) noexcept
{
	_variant.uniform4i(value);
	this->_needUpdate();
}

void
MaterialParam::uniform4i(std::int32_t i1, std::int32_t i2, std::int32_t i3, std::int32_t i4) noexcept
{
	_variant.uniform4i(i1, i2, i3, i4);
	this->_needUpdate();
}

void
MaterialParam::uniform1ui(std::uint32_t ui1) noexcept
{
	_variant.uniform1ui(ui1);
	this->_needUpdate();
}

void
MaterialParam::uniform2ui(const uint2& value) noexcept
{
	_variant.uniform2ui(value);
	this->_needUpdate();
}

void
MaterialParam::uniform2ui(std::uint32_t ui1, std::uint32_t ui2) noexcept
{
	_variant.uniform2ui(ui1, ui2);
	this->_needUpdate();
}

void
MaterialParam::uniform3ui(const uint3& value) noexcept
{
	_variant.uniform3ui(value);
	this->_needUpdate();
}

void
MaterialParam::uniform3ui(std::uint32_t ui1, std::uint32_t ui2, std::uint32_t ui3) noexcept
{
	_variant.uniform3ui(ui1, ui2, ui3);
	this->_needUpdate();
}

void
MaterialParam::uniform4ui(const uint4& value) noexcept
{
	_variant.uniform4ui(value);
	this->_needUpdate();
}

void
MaterialParam::uniform4ui(std::uint32_t ui1, std::uint32_t ui2, std::uint32_t ui3, std::uint32_t ui4) noexcept
{
	_variant.uniform4ui(ui1, ui2, ui3, ui4);
	this->_needUpdate();
}

void
MaterialParam::uniform1f(float f1) noexcept
{
	_variant.uniform1f(f1);
	this->_needUpdate();
}

void
MaterialParam::uniform2f(const float2& value) noexcept
{
	_variant.uniform2f(value);
	this->_needUpdate();
}

void
MaterialParam::uniform2f(float f1, float f2) noexcept
{
	_variant.uniform2f(f1, f2);
	this->_needUpdate();
}

void
MaterialParam::uniform3f(const float3& value) noexcept
{
	_variant.uniform3f(value);
	this->_needUpdate();
}

void
MaterialParam::uniform3f(float f1, float f2, float f3) noexcept
{
	_variant.uniform3f(f1, f2, f3);
	this->_needUpdate();
}

void
MaterialParam::uniform4f(const float4& value) noexcept
{
	_variant.uniform4f(value);
	this->_needUpdate();
}

void
MaterialParam::uniform4f(float f1, float f2, float f3, float f4) noexcept
{
	_variant.uniform4f(f1, f2, f3, f4);
	this->_needUpdate();
}

void
MaterialParam::uniform2fmat(const float2x2& value) noexcept
{
	_variant.uniform2fmat(value);
	this->_needUpdate();
}

void
MaterialParam::uniform2fmat(const float* mat2) noexcept
{
	_variant.uniform2fmat(mat2);
	this->_needUpdate();
}

void
MaterialParam::uniform3fmat(const float3x3& value) noexcept
{
	_variant.uniform3fmat(value);
	this->_needUpdate();
}

void
MaterialParam::uniform3fmat(const float* mat3) noexcept
{
	_variant.uniform3fmat(mat3);
	this->_needUpdate();
}

void
MaterialParam::uniform4fmat(const float4x4& value) noexcept
{
	_variant.uniform4fmat(value);
	this->_needUpdate();
}

void
MaterialParam::uniform4fmat(const float* mat4) noexcept
{
	_variant.uniform4fmat(mat4);
	this->_needUpdate();
}

void
MaterialParam::uniform1iv(const std::vector<int1>& value) noexcept
{
	_variant.uniform1iv(value);
	this->_needUpdate();
}

void
MaterialParam::uniform1iv(std::size_t num, const std::int32_t* i1v) noexcept
{
	_variant.uniform1iv(num, i1v);
	this->_needUpdate();
}

void
MaterialParam::uniform2iv(const std::vector<int2>& value) noexcept
{
	_variant.uniform2iv(value);
	this->_needUpdate();
}

void
MaterialParam::uniform2iv(std::size_t num, const std::int32_t* i2v) noexcept
{
	_variant.uniform2iv(num, i2v);
	this->_needUpdate();
}

void
MaterialParam::uniform3iv(const std::vector<int3>& value) noexcept
{
	_variant.uniform3iv(value);
	this->_needUpdate();
}

void
MaterialParam::uniform3iv(std::size_t num, const std::int32_t* i3v) noexcept
{
	_variant.uniform3iv(num, i3v);
	this->_needUpdate();
}

void
MaterialParam::uniform4iv(const std::vector<int4>& value) noexcept
{
	_variant.uniform4iv(value);
	this->_needUpdate();
}

void
MaterialParam::uniform4iv(std::size_t num, const std::int32_t* i4v) noexcept
{
	_variant.uniform4iv(num, i4v);
	this->_needUpdate();
}

void
MaterialParam::uniform1uiv(const std::vector<uint1>& value) noexcept
{
	_variant.uniform1uiv(value);
	this->_needUpdate();
}

void
MaterialParam::uniform1uiv(std::size_t num, const std::uint32_t* ui1v) noexcept
{
	_variant.uniform1uiv(num, ui1v);
	this->_needUpdate();
}

void
MaterialParam::uniform2uiv(const std::vector<uint2>& value) noexcept
{
	_variant.uniform2uiv(value);
	this->_needUpdate();
}

void
MaterialParam::uniform2uiv(std::size_t num, const std::uint32_t* ui2v) noexcept
{
	_variant.uniform2uiv(num, ui2v);
	this->_needUpdate();
}

void
MaterialParam::uniform3uiv(const std::vector<uint3>& value) noexcept
{
	_variant.uniform3uiv(value);
	this->_needUpdate();
}

void
MaterialParam::uniform3uiv(std::size_t num, const std::uint32_t* ui3v) noexcept
{
	_variant.uniform3uiv(num, ui3v);
	this->_needUpdate();
}

void
MaterialParam::uniform4uiv(const std::vector<uint4>& value) noexcept
{
	_variant.uniform4uiv(value);
	this->_needUpdate();
}

void
MaterialParam::uniform4uiv(std::size_t num, const std::uint32_t* ui4v) noexcept
{
	_variant.uniform4uiv(num, ui4v);
	this->_needUpdate();
}

void
MaterialParam::uniform1fv(const std::vector<float1>& value) noexcept
{
	_variant.uniform1fv(value);
	this->_needUpdate();
}

void
MaterialParam::uniform1fv(std::size_t num, const float* f1v) noexcept
{
	_variant.uniform1fv(num, f1v);
	this->_needUpdate();
}

void
MaterialParam::uniform2fv(const std::vector<float2>& value) noexcept
{
	_variant.uniform2fv(value);
	this->_needUpdate();
}

void
MaterialParam::uniform2fv(std::size_t num, const float* f2v) noexcept
{
	_variant.uniform2fv(num, f2v);
	this->_needUpdate();
}

void
MaterialParam::uniform3fv(const std::vector<float3>& value) noexcept
{
	_variant.uniform3fv(value);
	this->_needUpdate();
}

void
MaterialParam::uniform3fv(std::size_t num, const float* f3v) noexcept
{
	_variant.uniform3fv(num, f3v);
	this->_needUpdate();
}

void
MaterialParam::uniform4fv(const std::vector<float4>& value) noexcept
{
	_variant.uniform4fv(value);
	this->_needUpdate();
}

void
MaterialParam::uniform4fv(std::size_t num, const float* f4v) noexcept
{
	_variant.uniform4fv(num, f4v);
	this->_needUpdate();
}

void
MaterialParam::uniform2fmatv(const std::vector<float2x2>& value) noexcept
{
	_variant.uniform2fmatv(value);
	this->_needUpdate();
}

void
MaterialParam::uniform2fmatv(std::size_t num, const float* mat2) noexcept
{
	_variant.uniform2fmatv(num, mat2);
	this->_needUpdate();
}

void
MaterialParam::uniform3fmatv(const std::vector<float3x3>& value) noexcept
{
	_variant.uniform3fmatv(value);
	this->_needUpdate();
}

void
MaterialParam::uniform3fmatv(std::size_t num, const float* mat3) noexcept
{
	_variant.uniform3fmatv(num, mat3);
	this->_needUpdate();
}

void
MaterialParam::uniform4fmatv(const std::vector<float4x4>& value) noexcept
{
	_variant.uniform4fmatv(value);
	this->_needUpdate();
}

void
MaterialParam::uniform4fmatv(std::size_t num, const float* mat4) noexcept
{
	_variant.uniform4fmatv(num, mat4);
	this->_needUpdate();
}

void
MaterialParam::uniformTexture(GraphicsTexturePtr texture, GraphicsSamplerPtr sampler) noexcept
{
	_variant.uniformTexture(texture, sampler);
	this->_needUpdate();
}

void
MaterialParam::uniformBuffer(GraphicsDataPtr ubo) noexcept
{
	_variant.uniformBuffer(ubo);
	this->_needUpdate();
}

bool
MaterialParam::getBool() const noexcept
{
	return _variant.getBool();
}

int
MaterialParam::getInt() const noexcept
{
	return _variant.getInt();
}

const int2&
MaterialParam::getInt2() const noexcept
{
	return _variant.getInt2();
}

const int3&
MaterialParam::getInt3() const noexcept
{
	return _variant.getInt3();
}

const int4&
MaterialParam::getInt4() const noexcept
{
	return _variant.getInt4();
}

uint
MaterialParam::getUInt() const noexcept
{
	return _variant.getUInt();
}

const uint2&
MaterialParam::getUInt2() const noexcept
{
	return _variant.getUInt2();
}

const uint3&
MaterialParam::getUInt3() const noexcept
{
	return _variant.getUInt3();
}

const uint4&
MaterialParam::getUInt4() const noexcept
{
	return _variant.getUInt4();
}

float
MaterialParam::getFloat() const noexcept
{
	return _variant.getFloat();
}

const float2&
MaterialParam::getFloat2() const noexcept
{
	return _variant.getFloat2();
}

const float3&
MaterialParam::getFloat3() const noexcept
{
	return _variant.getFloat3();
}

const float4&
MaterialParam::getFloat4() const noexcept
{
	return _variant.getFloat4();
}

const float2x2&
MaterialParam::getFloat2x2() const noexcept
{
	return _variant.getFloat2x2();
}

const float3x3&
MaterialParam::getFloat3x3() const noexcept
{
	return _variant.getFloat3x3();
}

const float4x4&
MaterialParam::getFloat4x4() const noexcept
{
	return _variant.getFloat4x4();
}

const std::vector<int1>&
MaterialParam::getIntArray() const noexcept
{
	return _variant.getIntArray();
}

const std::vector<int2>&
MaterialParam::getInt2Array() const noexcept
{
	return _variant.getInt2Array();
}

const std::vector<int3>&
MaterialParam::getInt3Array() const noexcept
{
	return _variant.getInt3Array();
}

const std::vector<int4>&
MaterialParam::getInt4Array() const noexcept
{
	return _variant.getInt4Array();
}

const std::vector<uint1>&
MaterialParam::getUIntArray() const noexcept
{
	return _variant.getUIntArray();
}

const std::vector<uint2>&
MaterialParam::getUInt2Array() const noexcept
{
	return _variant.getUInt2Array();
}

const std::vector<uint3>&
MaterialParam::getUInt3Array() const noexcept
{
	return _variant.getUInt3Array();
}

const std::vector<uint4>&
MaterialParam::getUInt4Array() const noexcept
{
	return _variant.getUInt4Array();
}

const std::vector<float1>&
MaterialParam::getFloatArray() const noexcept
{
	return _variant.getFloatArray();
}

const std::vector<float2>&
MaterialParam::getFloat2Array() const noexcept
{
	return _variant.getFloat2Array();
}

const std::vector<float3>&
MaterialParam::getFloat3Array() const noexcept
{
	return _variant.getFloat3Array();
}

const std::vector<float4>&
MaterialParam::getFloat4Array() const noexcept
{
	return _variant.getFloat4Array();
}

const std::vector<float2x2>&
MaterialParam::getFloat2x2Array() const noexcept
{
	return _variant.getFloat2x2Array();
}

const std::vector<float3x3>&
MaterialParam::getFloat3x3Array() const noexcept
{
	return _variant.getFloat3x3Array();
}

const std::vector<float4x4>&
MaterialParam::getFloat4x4Array() const noexcept
{
	return _variant.getFloat4x4Array();
}

GraphicsTexturePtr
MaterialParam::getTexture() const noexcept
{
	return _variant.getTexture();
}

GraphicsSamplerPtr
MaterialParam::getTextureSampler() const noexcept
{
	return _variant.getTextureSampler();
}

GraphicsDataPtr 
MaterialParam::getBuffer() const noexcept
{
	return _variant.getBuffer();
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
	param->setSemantic(this->getSemantic());
	param->_variant.copy(this->_variant);
	return param;
}

void
MaterialParam::_needUpdate() noexcept
{
	for (auto& it : _listeners)
		it->onNeedUpdate();
}

_NAME_END