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
#include <ray/material_semantic.h>

_NAME_BEGIN

MaterialSemantic::MaterialSemantic() noexcept
{
}

MaterialSemantic::MaterialSemantic(const std::string& name, GraphicsUniformType type) noexcept
	: _name(name)
{
	_variant.setType(type);
}

MaterialSemantic::MaterialSemantic(std::string&& name, GraphicsUniformType type) noexcept
	: _name(std::move(name))
{
	_variant.setType(type);
}

MaterialSemantic::~MaterialSemantic() noexcept
{
}

void
MaterialSemantic::setName(const std::string& name) noexcept
{
	_name = name;
}

void
MaterialSemantic::setName(std::string&& name) noexcept
{
	_name = std::move(name);
}

const std::string&
MaterialSemantic::getName() const noexcept
{
	return _name;
}

void
MaterialSemantic::setType(GraphicsUniformType type) noexcept
{
	_variant.setType(type);
}

GraphicsUniformType
MaterialSemantic::getType() const noexcept
{
	return _variant.getType();
}

void
MaterialSemantic::uniform1b(bool value) noexcept
{
	_variant.uniform1b(value);
}

void
MaterialSemantic::uniform1i(std::int32_t i1) noexcept
{
	_variant.uniform1i(i1);
}

void
MaterialSemantic::uniform2i(const int2& value) noexcept
{
	_variant.uniform2i(value);
}

void
MaterialSemantic::uniform2i(std::int32_t i1, std::int32_t i2) noexcept
{
	_variant.uniform2i(i1, i2);
}

void
MaterialSemantic::uniform3i(const int3& value) noexcept
{
	_variant.uniform3i(value);
}

void
MaterialSemantic::uniform3i(std::int32_t i1, std::int32_t i2, std::int32_t i3) noexcept
{
	_variant.uniform3i(i1, i2, i3);
}

void
MaterialSemantic::uniform4i(const int4& value) noexcept
{
	_variant.uniform4i(value);
}

void
MaterialSemantic::uniform4i(std::int32_t i1, std::int32_t i2, std::int32_t i3, std::int32_t i4) noexcept
{
	_variant.uniform4i(i1, i2, i3, i4);
}

void
MaterialSemantic::uniform1ui(std::uint32_t ui1) noexcept
{
	_variant.uniform1ui(ui1);
}

void
MaterialSemantic::uniform2ui(const uint2& value) noexcept
{
	_variant.uniform2ui(value);
}

void
MaterialSemantic::uniform2ui(std::uint32_t ui1, std::uint32_t ui2) noexcept
{
	_variant.uniform2ui(ui1, ui2);
}

void
MaterialSemantic::uniform3ui(const uint3& value) noexcept
{
	_variant.uniform3ui(value);
}

void
MaterialSemantic::uniform3ui(std::uint32_t ui1, std::uint32_t ui2, std::uint32_t ui3) noexcept
{
	_variant.uniform3ui(ui1, ui2, ui3);
}

void
MaterialSemantic::uniform4ui(const uint4& value) noexcept
{
	_variant.uniform4ui(value);
}

void
MaterialSemantic::uniform4ui(std::uint32_t ui1, std::uint32_t ui2, std::uint32_t ui3, std::uint32_t ui4) noexcept
{
	_variant.uniform4ui(ui1, ui2, ui3, ui4);
}

void
MaterialSemantic::uniform1f(float f1) noexcept
{
	_variant.uniform1f(f1);
}

void
MaterialSemantic::uniform2f(const float2& value) noexcept
{
	_variant.uniform2f(value);
}

void
MaterialSemantic::uniform2f(float f1, float f2) noexcept
{
	_variant.uniform2f(f1, f2);
}

void
MaterialSemantic::uniform3f(const float3& value) noexcept
{
	_variant.uniform3f(value);
}

void
MaterialSemantic::uniform3f(float f1, float f2, float f3) noexcept
{
	_variant.uniform3f(f1, f2, f3);
}

void
MaterialSemantic::uniform4f(const float4& value) noexcept
{
	_variant.uniform4f(value);
}

void
MaterialSemantic::uniform4f(float f1, float f2, float f3, float f4) noexcept
{
	_variant.uniform4f(f1, f2, f3, f4);
}

void
MaterialSemantic::uniform2fmat(const float2x2& value) noexcept
{
	_variant.uniform2fmat(value);
}

void
MaterialSemantic::uniform2fmat(const float* mat2) noexcept
{
	_variant.uniform2fmat(mat2);
}

void
MaterialSemantic::uniform3fmat(const float3x3& value) noexcept
{
	_variant.uniform3fmat(value);
}

void
MaterialSemantic::uniform3fmat(const float* mat3) noexcept
{
	_variant.uniform3fmat(mat3);
}

void
MaterialSemantic::uniform4fmat(const float4x4& value) noexcept
{
	_variant.uniform4fmat(value);
}

void
MaterialSemantic::uniform4fmat(const float* mat4) noexcept
{
	_variant.uniform4fmat(mat4);
}

void
MaterialSemantic::uniform1iv(std::size_t num, const std::int32_t* i1v) noexcept
{
	_variant.uniform1iv(num, i1v);
}

void
MaterialSemantic::uniform2iv(std::size_t num, const std::int32_t* i2v) noexcept
{
	_variant.uniform2iv(num, i2v);
}

void
MaterialSemantic::uniform3iv(std::size_t num, const std::int32_t* i3v) noexcept
{
	_variant.uniform3iv(num, i3v);
}

void
MaterialSemantic::uniform4iv(std::size_t num, const std::int32_t* i4v) noexcept
{
	_variant.uniform4iv(num, i4v);
}

void
MaterialSemantic::uniform1uiv(std::size_t num, const std::uint32_t* ui1v) noexcept
{
	_variant.uniform1uiv(num, ui1v);
}

void
MaterialSemantic::uniform2uiv(std::size_t num, const std::uint32_t* ui2v) noexcept
{
	_variant.uniform2uiv(num, ui2v);
}

void
MaterialSemantic::uniform3uiv(std::size_t num, const std::uint32_t* ui3v) noexcept
{
	_variant.uniform3uiv(num, ui3v);
}

void
MaterialSemantic::uniform4uiv(std::size_t num, const std::uint32_t* ui4v) noexcept
{
	_variant.uniform4uiv(num, ui4v);
}

void
MaterialSemantic::uniform1fv(std::size_t num, const float* f1v) noexcept
{
	_variant.uniform1fv(num, f1v);
}

void
MaterialSemantic::uniform2fv(std::size_t num, const float* f2v) noexcept
{
	_variant.uniform2fv(num, f2v);
}

void
MaterialSemantic::uniform3fv(std::size_t num, const float* f3v) noexcept
{
	_variant.uniform3fv(num, f3v);
}

void
MaterialSemantic::uniform4fv(std::size_t num, const float* f4v) noexcept
{
	_variant.uniform4fv(num, f4v);
}

void
MaterialSemantic::uniform2fmatv(std::size_t num, const float* mat2) noexcept
{
	_variant.uniform2fmatv(num, mat2);
}

void
MaterialSemantic::uniform3fmatv(std::size_t num, const float* mat3) noexcept
{
	_variant.uniform3fmatv(num, mat3);
}

void
MaterialSemantic::uniform4fmatv(std::size_t num, const float* mat4) noexcept
{
	_variant.uniform4fmatv(num, mat4);
}

void
MaterialSemantic::uniform1iv(const std::vector<int1>& value) noexcept
{
	_variant.uniform1iv(value);
}

void
MaterialSemantic::uniform2iv(const std::vector<int2>& value) noexcept
{
	_variant.uniform2iv(value);
}

void
MaterialSemantic::uniform3iv(const std::vector<int3>& value) noexcept
{
	_variant.uniform3iv(value);
}

void
MaterialSemantic::uniform4iv(const std::vector<int4>& value) noexcept
{
	_variant.uniform4iv(value);
}

void
MaterialSemantic::uniform1uiv(const std::vector<uint1>& value) noexcept
{
	_variant.uniform1uiv(value);
}

void
MaterialSemantic::uniform2uiv(const std::vector<uint2>& value) noexcept
{
	_variant.uniform2uiv(value);
}

void
MaterialSemantic::uniform3uiv(const std::vector<uint3>& value) noexcept
{
	_variant.uniform3uiv(value);
}

void
MaterialSemantic::uniform4uiv(const std::vector<uint4>& value) noexcept
{
	_variant.uniform4uiv(value);
}

void
MaterialSemantic::uniform1fv(const std::vector<float1>& value) noexcept
{
	_variant.uniform1fv(value);
}

void
MaterialSemantic::uniform2fv(const std::vector<float2>& value) noexcept
{
	_variant.uniform2fv(value);
}

void
MaterialSemantic::uniform3fv(const std::vector<float3>& value) noexcept
{
	_variant.uniform3fv(value);
}

void
MaterialSemantic::uniform4fv(const std::vector<float4>& value) noexcept
{
	_variant.uniform4fv(value);
}

void
MaterialSemantic::uniform2fmatv(const std::vector<float2x2>& value) noexcept
{
	_variant.uniform2fmatv(value);
}

void
MaterialSemantic::uniform3fmatv(const std::vector<float3x3>& value) noexcept
{
	_variant.uniform3fmatv(value);
}

void
MaterialSemantic::uniform4fmatv(const std::vector<float4x4>& value) noexcept
{
	_variant.uniform4fmatv(value);
}

void
MaterialSemantic::uniformTexture(GraphicsTexturePtr texture, GraphicsSamplerPtr sampler) noexcept
{
	_variant.uniformTexture(texture, sampler);
}

void
MaterialSemantic::uniformBuffer(GraphicsDataPtr ubo) noexcept
{
	_variant.uniformBuffer(ubo);
}

bool
MaterialSemantic::getBool() const noexcept
{
	return _variant.getBool();
}

int
MaterialSemantic::getInt() const noexcept
{
	return _variant.getInt();
}

const int2&
MaterialSemantic::getInt2() const noexcept
{
	return _variant.getInt2();
}

const int3&
MaterialSemantic::getInt3() const noexcept
{
	return _variant.getInt3();
}

const int4&
MaterialSemantic::getInt4() const noexcept
{
	return _variant.getInt4();
}

uint
MaterialSemantic::getUInt() const noexcept
{
	return _variant.getUInt();
}

const uint2&
MaterialSemantic::getUInt2() const noexcept
{
	return _variant.getUInt2();
}

const uint3&
MaterialSemantic::getUInt3() const noexcept
{
	return _variant.getUInt3();
}

const uint4&
MaterialSemantic::getUInt4() const noexcept
{
	return _variant.getUInt4();
}

float
MaterialSemantic::getFloat() const noexcept
{
	return _variant.getFloat();
}

const float2&
MaterialSemantic::getFloat2() const noexcept
{
	return _variant.getFloat2();
}

const float3&
MaterialSemantic::getFloat3() const noexcept
{
	return _variant.getFloat3();
}

const float4&
MaterialSemantic::getFloat4() const noexcept
{
	return _variant.getFloat4();
}

const float2x2&
MaterialSemantic::getFloat2x2() const noexcept
{
	return _variant.getFloat2x2();
}

const float3x3&
MaterialSemantic::getFloat3x3() const noexcept
{
	return _variant.getFloat3x3();
}

const float4x4&
MaterialSemantic::getFloat4x4() const noexcept
{
	return _variant.getFloat4x4();
}

const std::vector<int1>&
MaterialSemantic::getIntArray() const noexcept
{
	return _variant.getIntArray();
}

const std::vector<int2>&
MaterialSemantic::getInt2Array() const noexcept
{
	return _variant.getInt2Array();
}

const std::vector<int3>&
MaterialSemantic::getInt3Array() const noexcept
{
	return _variant.getInt3Array();
}

const std::vector<int4>&
MaterialSemantic::getInt4Array() const noexcept
{
	return _variant.getInt4Array();
}

const std::vector<uint1>&
MaterialSemantic::getUIntArray() const noexcept
{
	return _variant.getUIntArray();
}

const std::vector<uint2>&
MaterialSemantic::getUInt2Array() const noexcept
{
	return _variant.getUInt2Array();
}

const std::vector<uint3>&
MaterialSemantic::getUInt3Array() const noexcept
{
	return _variant.getUInt3Array();
}

const std::vector<uint4>&
MaterialSemantic::getUInt4Array() const noexcept
{
	return _variant.getUInt4Array();
}

const std::vector<float1>&
MaterialSemantic::getFloatArray() const noexcept
{
	return _variant.getFloatArray();
}

const std::vector<float2>&
MaterialSemantic::getFloat2Array() const noexcept
{
	return _variant.getFloat2Array();
}

const std::vector<float3>&
MaterialSemantic::getFloat3Array() const noexcept
{
	return _variant.getFloat3Array();
}

const std::vector<float4>&
MaterialSemantic::getFloat4Array() const noexcept
{
	return _variant.getFloat4Array();
}

const std::vector<float2x2>&
MaterialSemantic::getFloat2x2Array() const noexcept
{
	return _variant.getFloat2x2Array();
}

const std::vector<float3x3>&
MaterialSemantic::getFloat3x3Array() const noexcept
{
	return _variant.getFloat3x3Array();
}

const std::vector<float4x4>&
MaterialSemantic::getFloat4x4Array() const noexcept
{
	return _variant.getFloat4x4Array();
}

GraphicsTexturePtr
MaterialSemantic::getTexture() const noexcept
{
	return _variant.getTexture();
}

GraphicsSamplerPtr
MaterialSemantic::getTextureSampler() const noexcept
{
	return _variant.getTextureSampler();
}

GraphicsDataPtr
MaterialSemantic::getBuffer() const noexcept
{
	return _variant.getBuffer();
}

MaterialSemanticManager::MaterialSemanticManager() noexcept
{
}

MaterialSemanticManager::~MaterialSemanticManager() noexcept
{
	this->close();
}

bool 
MaterialSemanticManager::setup() noexcept
{
	_parametes[GlobalSemanticType::GlobalSemanticTypeModel] = std::make_shared<MaterialSemantic>("matModel", GraphicsUniformType::GraphicsUniformTypeFloat4x4);
	_parametes[GlobalSemanticType::GlobalSemanticTypeModelInverse] = std::make_shared<MaterialSemantic>("matModelInverse", GraphicsUniformType::GraphicsUniformTypeFloat4x4);
	_parametes[GlobalSemanticType::GlobalSemanticTypeView] = std::make_shared<MaterialSemantic>("matView", GraphicsUniformType::GraphicsUniformTypeFloat4x4);
	_parametes[GlobalSemanticType::GlobalSemanticTypeViewInverse] = std::make_shared<MaterialSemantic>("matViewInverse", GraphicsUniformType::GraphicsUniformTypeFloat4x4);
	_parametes[GlobalSemanticType::GlobalSemanticTypeProject] = std::make_shared<MaterialSemantic>("matProject", GraphicsUniformType::GraphicsUniformTypeFloat4x4);
	_parametes[GlobalSemanticType::GlobalSemanticTypeProjectInverse] = std::make_shared<MaterialSemantic>("matProjectInverse", GraphicsUniformType::GraphicsUniformTypeFloat4x4);
	_parametes[GlobalSemanticType::GlobalSemanticTypeViewProject] = std::make_shared<MaterialSemantic>("matViewProject", GraphicsUniformType::GraphicsUniformTypeFloat4x4);
	_parametes[GlobalSemanticType::GlobalSemanticTypeViewProjectInverse] = std::make_shared<MaterialSemantic>("matViewProjectInverse", GraphicsUniformType::GraphicsUniformTypeFloat4x4);
	_parametes[GlobalSemanticType::GlobalSemanticTypeModelView] = std::make_shared<MaterialSemantic>("matModelView", GraphicsUniformType::GraphicsUniformTypeFloat4x4);
	_parametes[GlobalSemanticType::GlobalSemanticTypeModelViewProject] = std::make_shared<MaterialSemantic>("matModelViewProject", GraphicsUniformType::GraphicsUniformTypeFloat4x4);
	_parametes[GlobalSemanticType::GlobalSemanticTypeModelViewInverse] = std::make_shared<MaterialSemantic>("matModelViewInverse", GraphicsUniformType::GraphicsUniformTypeFloat4x4);
	
	_parametes[GlobalSemanticType::GlobalSemanticTypeCameraAperture] = std::make_shared<MaterialSemantic>("CameraAperture", GraphicsUniformType::GraphicsUniformTypeFloat);
	_parametes[GlobalSemanticType::GlobalSemanticTypeCameraNear] = std::make_shared<MaterialSemantic>("CameraNear", GraphicsUniformType::GraphicsUniformTypeFloat);
	_parametes[GlobalSemanticType::GlobalSemanticTypeCameraFar] = std::make_shared<MaterialSemantic>("CameraFar", GraphicsUniformType::GraphicsUniformTypeFloat);
	_parametes[GlobalSemanticType::GlobalSemanticTypeCameraPosition] = std::make_shared<MaterialSemantic>("CameraPosition", GraphicsUniformType::GraphicsUniformTypeFloat3);
	_parametes[GlobalSemanticType::GlobalSemanticTypeCameraDirection] = std::make_shared<MaterialSemantic>("CameraDirection", GraphicsUniformType::GraphicsUniformTypeFloat3);

	_parametes[GlobalSemanticType::GlobalSemanticTypeDepthMap] = std::make_shared<MaterialSemantic>("DepthMap", GraphicsUniformType::GraphicsUniformTypeSamplerImage);
	_parametes[GlobalSemanticType::GlobalSemanticTypeDepthLinearMap] = std::make_shared<MaterialSemantic>("DepthLinearMap", GraphicsUniformType::GraphicsUniformTypeSamplerImage);
	_parametes[GlobalSemanticType::GlobalSemanticTypeDiffuseMap] = std::make_shared<MaterialSemantic>("DiffuseMap", GraphicsUniformType::GraphicsUniformTypeSamplerImage);
	_parametes[GlobalSemanticType::GlobalSemanticTypeNormalMap] = std::make_shared<MaterialSemantic>("NormalMap", GraphicsUniformType::GraphicsUniformTypeSamplerImage);
	_parametes[GlobalSemanticType::GlobalSemanticTypeGbuffer3Map] = std::make_shared<MaterialSemantic>("Gbuffer3Map", GraphicsUniformType::GraphicsUniformTypeSamplerImage);	
	_parametes[GlobalSemanticType::GlobalSemanticTypeLightingMap] = std::make_shared<MaterialSemantic>("LightingMap", GraphicsUniformType::GraphicsUniformTypeSamplerImage);
	_parametes[GlobalSemanticType::GlobalSemanticTypeOpaqueShadingMap] = std::make_shared<MaterialSemantic>("OpaqueShadingMap", GraphicsUniformType::GraphicsUniformTypeSamplerImage);

	return true;
}

void 
MaterialSemanticManager::close() noexcept
{
	_parametes->reset();
}

const MaterialSemanticPtr&
MaterialSemanticManager::getSemantic(GlobalSemanticType type) const noexcept
{
	assert(type > GlobalSemanticType::GlobalSemanticTypeBeginRange && type <= GlobalSemanticType::GlobalSemanticTypeEndRange);
	return _parametes[type];
}

_NAME_END