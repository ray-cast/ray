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
#include "ogl_descriptor_set.h"
#include "ogl_core_descriptor.h"
#include "ogl_texture.h"
#include "ogl_shader.h"
#include "ogl_sampler.h"
#include "ogl_graphics_data.h"

_NAME_BEGIN

__ImplementSubClass(OGLDescriptorSet, GraphicsDescriptorSet, "OGLDescriptorSet")
__ImplementSubClass(OGLGraphicsUniformSet, GraphicsUniformSet, "OGLGraphicsUniformSet")
__ImplementSubClass(OGLDescriptorSetLayout, GraphicsDescriptorSetLayout, "OGLDescriptorSetLayout")
__ImplementSubClass(OGLDescriptorPool, GraphicsDescriptorPool, "OGLDescriptorPool")

OGLGraphicsUniformSet::OGLGraphicsUniformSet() noexcept
{
}

OGLGraphicsUniformSet::~OGLGraphicsUniformSet() noexcept
{
}

void
OGLGraphicsUniformSet::uniform1b(bool value) noexcept
{
	_variant.uniform1b(value);
}

void
OGLGraphicsUniformSet::uniform1i(std::int32_t i1) noexcept
{
	_variant.uniform1i(i1);
}

void
OGLGraphicsUniformSet::uniform2i(const int2& value) noexcept
{
	_variant.uniform2i(value);
}

void
OGLGraphicsUniformSet::uniform2i(std::int32_t i1, std::int32_t i2) noexcept
{
	_variant.uniform2i(i1, i2);
}

void
OGLGraphicsUniformSet::uniform3i(const int3& value) noexcept
{
	_variant.uniform3i(value);
}

void
OGLGraphicsUniformSet::uniform3i(std::int32_t i1, std::int32_t i2, std::int32_t i3) noexcept
{
	_variant.uniform3i(i1, i2, i3);
}

void
OGLGraphicsUniformSet::uniform4i(const int4& value) noexcept
{
	_variant.uniform4i(value);
}

void
OGLGraphicsUniformSet::uniform4i(std::int32_t i1, std::int32_t i2, std::int32_t i3, std::int32_t i4) noexcept
{
	_variant.uniform4i(i1, i2, i3, i4);
}

void
OGLGraphicsUniformSet::uniform1ui(std::uint32_t ui1) noexcept
{
	_variant.uniform1ui(ui1);
}

void
OGLGraphicsUniformSet::uniform2ui(const uint2& value) noexcept
{
	_variant.uniform2ui(value);
}

void
OGLGraphicsUniformSet::uniform2ui(std::uint32_t ui1, std::uint32_t ui2) noexcept
{
	_variant.uniform2ui(ui1, ui2);
}

void
OGLGraphicsUniformSet::uniform3ui(const uint3& value) noexcept
{
	_variant.uniform3ui(value);
}

void
OGLGraphicsUniformSet::uniform3ui(std::uint32_t ui1, std::uint32_t ui2, std::uint32_t ui3) noexcept
{
	_variant.uniform3ui(ui1, ui2, ui3);
}

void
OGLGraphicsUniformSet::uniform4ui(const uint4& value) noexcept
{
	_variant.uniform4ui(value);
}

void
OGLGraphicsUniformSet::uniform4ui(std::uint32_t ui1, std::uint32_t ui2, std::uint32_t ui3, std::uint32_t ui4) noexcept
{
	_variant.uniform4ui(ui1, ui2, ui3, ui4);
}

void
OGLGraphicsUniformSet::uniform1f(float f1) noexcept
{
	_variant.uniform1f(f1);
}

void
OGLGraphicsUniformSet::uniform2f(const float2& value) noexcept
{
	_variant.uniform2f(value);
}

void
OGLGraphicsUniformSet::uniform2f(float f1, float f2) noexcept
{
	_variant.uniform2f(f1, f2);
}

void
OGLGraphicsUniformSet::uniform3f(const float3& value) noexcept
{
	_variant.uniform3f(value);
}

void
OGLGraphicsUniformSet::uniform3f(float f1, float f2, float f3) noexcept
{
	_variant.uniform3f(f1, f2, f3);
}

void
OGLGraphicsUniformSet::uniform4f(const float4& value) noexcept
{
	_variant.uniform4f(value);
}

void
OGLGraphicsUniformSet::uniform4f(float f1, float f2, float f3, float f4) noexcept
{
	_variant.uniform4f(f1, f2, f3, f4);
}

void
OGLGraphicsUniformSet::uniform2fmat(const float2x2& value) noexcept
{
	_variant.uniform2fmat(value);
}

void
OGLGraphicsUniformSet::uniform2fmat(const float* mat2) noexcept
{
	_variant.uniform2fmat(mat2);
}

void
OGLGraphicsUniformSet::uniform3fmat(const float3x3& value) noexcept
{
	_variant.uniform3fmat(value);
}

void
OGLGraphicsUniformSet::uniform3fmat(const float* mat3) noexcept
{
	_variant.uniform3fmat(mat3);
}

void
OGLGraphicsUniformSet::uniform4fmat(const float4x4& value) noexcept
{
	_variant.uniform4fmat(value);
}

void
OGLGraphicsUniformSet::uniform4fmat(const float* mat4) noexcept
{
	_variant.uniform4fmat(mat4);
}

void
OGLGraphicsUniformSet::uniform1iv(const std::vector<int1>& value) noexcept
{
	_variant.uniform1iv(value);
}

void
OGLGraphicsUniformSet::uniform1iv(std::size_t num, const std::int32_t* i1v) noexcept
{
	_variant.uniform1iv(num, i1v);
}

void
OGLGraphicsUniformSet::uniform2iv(const std::vector<int2>& value) noexcept
{
	_variant.uniform2iv(value);
}

void
OGLGraphicsUniformSet::uniform2iv(std::size_t num, const std::int32_t* i2v) noexcept
{
	_variant.uniform2iv(num, i2v);
}

void
OGLGraphicsUniformSet::uniform3iv(const std::vector<int3>& value) noexcept
{
	_variant.uniform3iv(value);
}

void
OGLGraphicsUniformSet::uniform3iv(std::size_t num, const std::int32_t* i3v) noexcept
{
	_variant.uniform3iv(num, i3v);
}

void
OGLGraphicsUniformSet::uniform4iv(const std::vector<int4>& value) noexcept
{
	_variant.uniform4iv(value);
}

void
OGLGraphicsUniformSet::uniform4iv(std::size_t num, const std::int32_t* i4v) noexcept
{
	_variant.uniform4iv(num, i4v);
}

void
OGLGraphicsUniformSet::uniform1uiv(const std::vector<uint1>& value) noexcept
{
	_variant.uniform1uiv(value);
}

void
OGLGraphicsUniformSet::uniform1uiv(std::size_t num, const std::uint32_t* ui1v) noexcept
{
	_variant.uniform1uiv(num, ui1v);
}

void
OGLGraphicsUniformSet::uniform2uiv(const std::vector<uint2>& value) noexcept
{
	_variant.uniform2uiv(value);
}

void
OGLGraphicsUniformSet::uniform2uiv(std::size_t num, const std::uint32_t* ui2v) noexcept
{
	_variant.uniform2uiv(num, ui2v);
}

void
OGLGraphicsUniformSet::uniform3uiv(const std::vector<uint3>& value) noexcept
{
	_variant.uniform3uiv(value);
}

void
OGLGraphicsUniformSet::uniform3uiv(std::size_t num, const std::uint32_t* ui3v) noexcept
{
	_variant.uniform3uiv(num, ui3v);
}

void
OGLGraphicsUniformSet::uniform4uiv(const std::vector<uint4>& value) noexcept
{
	_variant.uniform4uiv(value);
}

void
OGLGraphicsUniformSet::uniform4uiv(std::size_t num, const std::uint32_t* ui4v) noexcept
{
	_variant.uniform4uiv(num, ui4v);
}

void
OGLGraphicsUniformSet::uniform1fv(const std::vector<float1>& value) noexcept
{
	_variant.uniform1fv(value);
}

void
OGLGraphicsUniformSet::uniform1fv(std::size_t num, const float* f1v) noexcept
{
	_variant.uniform1fv(num, f1v);
}

void
OGLGraphicsUniformSet::uniform2fv(const std::vector<float2>& value) noexcept
{
	_variant.uniform2fv(value);
}

void
OGLGraphicsUniformSet::uniform2fv(std::size_t num, const float* f2v) noexcept
{
	_variant.uniform2fv(num, f2v);
}

void
OGLGraphicsUniformSet::uniform3fv(const std::vector<float3>& value) noexcept
{
	_variant.uniform3fv(value);
}

void
OGLGraphicsUniformSet::uniform3fv(std::size_t num, const float* f3v) noexcept
{
	_variant.uniform3fv(num, f3v);
}

void
OGLGraphicsUniformSet::uniform4fv(const std::vector<float4>& value) noexcept
{
	_variant.uniform4fv(value);
}

void
OGLGraphicsUniformSet::uniform4fv(std::size_t num, const float* f4v) noexcept
{
	_variant.uniform4fv(num, f4v);
}

void
OGLGraphicsUniformSet::uniform2fmatv(const std::vector<float2x2>& value) noexcept
{
	_variant.uniform2fmatv(value);
}

void
OGLGraphicsUniformSet::uniform2fmatv(std::size_t num, const float* mat2) noexcept
{
	_variant.uniform2fmatv(num, mat2);
}

void
OGLGraphicsUniformSet::uniform3fmatv(const std::vector<float3x3>& value) noexcept
{
	_variant.uniform3fmatv(value);
}

void
OGLGraphicsUniformSet::uniform3fmatv(std::size_t num, const float* mat3) noexcept
{
	_variant.uniform3fmatv(num, mat3);
}

void
OGLGraphicsUniformSet::uniform4fmatv(const std::vector<float4x4>& value) noexcept
{
	_variant.uniform4fmatv(value);
}

void
OGLGraphicsUniformSet::uniform4fmatv(std::size_t num, const float* mat4) noexcept
{
	_variant.uniform4fmatv(num, mat4);
}

void
OGLGraphicsUniformSet::uniformTexture(GraphicsTexturePtr texture, GraphicsSamplerPtr sampler) noexcept
{
	_variant.uniformTexture(texture, sampler);
}

void
OGLGraphicsUniformSet::uniformBuffer(GraphicsDataPtr ubo) noexcept
{
	_variant.uniformBuffer(ubo);
}

bool
OGLGraphicsUniformSet::getBool() const noexcept
{
	return _variant.getBool();
}

int
OGLGraphicsUniformSet::getInt() const noexcept
{
	return _variant.getInt();
}

const int2&
OGLGraphicsUniformSet::getInt2() const noexcept
{
	return _variant.getInt2();
}

const int3&
OGLGraphicsUniformSet::getInt3() const noexcept
{
	return _variant.getInt3();
}

const int4&
OGLGraphicsUniformSet::getInt4() const noexcept
{
	return _variant.getInt4();
}

uint
OGLGraphicsUniformSet::getUInt() const noexcept
{
	return _variant.getUInt();
}

const uint2&
OGLGraphicsUniformSet::getUInt2() const noexcept
{
	return _variant.getUInt2();
}

const uint3&
OGLGraphicsUniformSet::getUInt3() const noexcept
{
	return _variant.getUInt3();
}

const uint4&
OGLGraphicsUniformSet::getUInt4() const noexcept
{
	return _variant.getUInt4();
}

float
OGLGraphicsUniformSet::getFloat() const noexcept
{
	return _variant.getFloat();
}

const float2&
OGLGraphicsUniformSet::getFloat2() const noexcept
{
	return _variant.getFloat2();
}

const float3&
OGLGraphicsUniformSet::getFloat3() const noexcept
{
	return _variant.getFloat3();
}

const float4&
OGLGraphicsUniformSet::getFloat4() const noexcept
{
	return _variant.getFloat4();
}

const float2x2&
OGLGraphicsUniformSet::getFloat2x2() const noexcept
{
	return _variant.getFloat2x2();
}

const float3x3&
OGLGraphicsUniformSet::getFloat3x3() const noexcept
{
	return _variant.getFloat3x3();
}

const float4x4&
OGLGraphicsUniformSet::getFloat4x4() const noexcept
{
	return _variant.getFloat4x4();
}

const std::vector<int1>&
OGLGraphicsUniformSet::getIntArray() const noexcept
{
	return _variant.getIntArray();
}

const std::vector<int2>&
OGLGraphicsUniformSet::getInt2Array() const noexcept
{
	return _variant.getInt2Array();
}

const std::vector<int3>&
OGLGraphicsUniformSet::getInt3Array() const noexcept
{
	return _variant.getInt3Array();
}

const std::vector<int4>&
OGLGraphicsUniformSet::getInt4Array() const noexcept
{
	return _variant.getInt4Array();
}

const std::vector<uint1>&
OGLGraphicsUniformSet::getUIntArray() const noexcept
{
	return _variant.getUIntArray();
}

const std::vector<uint2>&
OGLGraphicsUniformSet::getUInt2Array() const noexcept
{
	return _variant.getUInt2Array();
}

const std::vector<uint3>&
OGLGraphicsUniformSet::getUInt3Array() const noexcept
{
	return _variant.getUInt3Array();
}

const std::vector<uint4>&
OGLGraphicsUniformSet::getUInt4Array() const noexcept
{
	return _variant.getUInt4Array();
}

const std::vector<float1>&
OGLGraphicsUniformSet::getFloatArray() const noexcept
{
	return _variant.getFloatArray();
}

const std::vector<float2>&
OGLGraphicsUniformSet::getFloat2Array() const noexcept
{
	return _variant.getFloat2Array();
}

const std::vector<float3>&
OGLGraphicsUniformSet::getFloat3Array() const noexcept
{
	return _variant.getFloat3Array();
}

const std::vector<float4>&
OGLGraphicsUniformSet::getFloat4Array() const noexcept
{
	return _variant.getFloat4Array();
}

const std::vector<float2x2>&
OGLGraphicsUniformSet::getFloat2x2Array() const noexcept
{
	return _variant.getFloat2x2Array();
}

const std::vector<float3x3>&
OGLGraphicsUniformSet::getFloat3x3Array() const noexcept
{
	return _variant.getFloat3x3Array();
}

const std::vector<float4x4>&
OGLGraphicsUniformSet::getFloat4x4Array() const noexcept
{
	return _variant.getFloat4x4Array();
}

const GraphicsTexturePtr&
OGLGraphicsUniformSet::getTexture() const noexcept
{
	return _variant.getTexture();
}

const GraphicsSamplerPtr&
OGLGraphicsUniformSet::getTextureSampler() const noexcept
{
	return _variant.getTextureSampler();
}

const GraphicsDataPtr& 
OGLGraphicsUniformSet::getBuffer() const noexcept
{
	return _variant.getBuffer();
}

void
OGLGraphicsUniformSet::setGraphicsParam(GraphicsParamPtr param) noexcept
{
	assert(param);
	_param = param;
	_variant.setType(param->getType());
}

const GraphicsParamPtr&
OGLGraphicsUniformSet::getGraphicsParam() const noexcept
{
	return _param;
}

OGLDescriptorPool::OGLDescriptorPool() noexcept
{
}

OGLDescriptorPool::~OGLDescriptorPool() noexcept
{
	this->close();
}

bool
OGLDescriptorPool::setup(const GraphicsDescriptorPoolDesc& desc) noexcept
{
	return true;
}

void
OGLDescriptorPool::close() noexcept
{
}

const GraphicsDescriptorPoolDesc&
OGLDescriptorPool::getGraphicsDescriptorPoolDesc() const noexcept
{
	return _descriptorPoolDesc;
}

void
OGLDescriptorPool::setDevice(GraphicsDevicePtr device) noexcept
{
	_device = device;
}

GraphicsDevicePtr
OGLDescriptorPool::getDevice() noexcept
{
	return _device.lock();
}

OGLDescriptorSetLayout::OGLDescriptorSetLayout() noexcept
{
}

OGLDescriptorSetLayout::~OGLDescriptorSetLayout() noexcept
{
	this->close();
}

bool
OGLDescriptorSetLayout::setup(const GraphicsDescriptorSetLayoutDesc& descriptorSetLayoutDesc) noexcept
{
	_descripotrSetLayoutDesc = descriptorSetLayoutDesc;
	return true;
}

void
OGLDescriptorSetLayout::close() noexcept
{
}

const GraphicsDescriptorSetLayoutDesc&
OGLDescriptorSetLayout::getGraphicsDescriptorSetLayoutDesc() const noexcept
{
	return _descripotrSetLayoutDesc;
}

void
OGLDescriptorSetLayout::setDevice(GraphicsDevicePtr device) noexcept
{
	_device = device;
}

GraphicsDevicePtr
OGLDescriptorSetLayout::getDevice() noexcept
{
	return _device.lock();
}

OGLDescriptorSet::OGLDescriptorSet() noexcept
{
}

OGLDescriptorSet::~OGLDescriptorSet() noexcept
{
	this->close();
}

bool
OGLDescriptorSet::setup(const GraphicsDescriptorSetDesc& descriptorSetDesc) noexcept
{
	assert(descriptorSetDesc.getGraphicsDescriptorSetLayout());

	auto& descriptorSetLayoutDesc = descriptorSetDesc.getGraphicsDescriptorSetLayout()->getGraphicsDescriptorSetLayoutDesc();

	auto& uniforms = descriptorSetLayoutDesc.getUniformComponents();
	for (auto& uniform : uniforms)
	{
		auto uniformSet = std::make_shared<OGLGraphicsUniformSet>();
		uniformSet->setGraphicsParam(uniform);
		_activeUniformSets.push_back(uniformSet);
	}

	auto& uniformBlocks = descriptorSetLayoutDesc.getUniformBlockComponents();
	for (auto& uniformBlock : uniformBlocks)
	{
		auto uniformSet = std::make_shared<OGLGraphicsUniformSet>();
		uniformSet->setGraphicsParam(uniformBlock);
		_activeUniformSets.push_back(uniformSet);
	}

	_descriptorSetDesc = descriptorSetDesc;
	return true;
}

void
OGLDescriptorSet::close() noexcept
{
	_activeUniformSets.clear();
}

void
OGLDescriptorSet::apply(const OGLProgram& shaderObject) noexcept
{
	auto program = shaderObject.getInstanceID();
	for (auto& it : _activeUniformSets)
	{
		auto type = it->getGraphicsParam()->getType();
		auto location = it->getGraphicsParam()->getBindingPoint();
		switch (type)
		{
		case GraphicsUniformType::GraphicsUniformTypeBool:
			glProgramUniform1i(program, location, it->getBool());
			break;
		case GraphicsUniformType::GraphicsUniformTypeInt:
			glProgramUniform1i(program, location, it->getInt());
			break;
		case GraphicsUniformType::GraphicsUniformTypeInt2:
			glProgramUniform2iv(program, location, 1, (GLint*)it->getInt2().ptr());
			break;
		case GraphicsUniformType::GraphicsUniformTypeInt3:
			glProgramUniform3iv(program, location, 1, (GLint*)it->getInt3().ptr());
			break;
		case GraphicsUniformType::GraphicsUniformTypeInt4:
			glProgramUniform4iv(program, location, 1, (GLint*)it->getInt4().ptr());
			break;
		case GraphicsUniformType::GraphicsUniformTypeUInt:
			glProgramUniform1ui(program, location, it->getUInt());
			break;
		case GraphicsUniformType::GraphicsUniformTypeUInt2:
			glProgramUniform2uiv(program, location, 1, (GLuint*)it->getUInt2().ptr());
			break;
		case GraphicsUniformType::GraphicsUniformTypeUInt3:
			glProgramUniform3uiv(program, location, 1, (GLuint*)it->getUInt3().ptr());
			break;
		case GraphicsUniformType::GraphicsUniformTypeUInt4:
			glProgramUniform4uiv(program, location, 1, (GLuint*)it->getUInt4().ptr());
			break;
		case GraphicsUniformType::GraphicsUniformTypeFloat:
			glProgramUniform1f(program, location, it->getFloat());
			break;
		case GraphicsUniformType::GraphicsUniformTypeFloat2:
			glProgramUniform2fv(program, location, 1, it->getFloat2().ptr());
			break;
		case GraphicsUniformType::GraphicsUniformTypeFloat3:
			glProgramUniform3fv(program, location, 1, it->getFloat3().ptr());
			break;
		case GraphicsUniformType::GraphicsUniformTypeFloat4:
			glProgramUniform4fv(program, location, 1, it->getFloat4().ptr());
			break;
		case GraphicsUniformType::GraphicsUniformTypeFloat3x3:
			glProgramUniformMatrix3fv(program, location, 1, GL_FALSE, it->getFloat3x3().ptr());
			break;
		case GraphicsUniformType::GraphicsUniformTypeFloat4x4:
			glProgramUniformMatrix4fv(program, location, 1, GL_FALSE, it->getFloat4x4().ptr());
			break;
		case GraphicsUniformType::GraphicsUniformTypeIntArray:
			glProgramUniform1iv(program, location, it->getIntArray().size(), it->getIntArray().data());
			break;
		case GraphicsUniformType::GraphicsUniformTypeInt2Array:
			glProgramUniform2iv(program, location, it->getInt2Array().size(), (GLint*)it->getInt2Array().data());
			break;
		case GraphicsUniformType::GraphicsUniformTypeInt3Array:
			glProgramUniform3iv(program, location, it->getInt3Array().size(), (GLint*)it->getInt3Array().data());
			break;
		case GraphicsUniformType::GraphicsUniformTypeInt4Array:
			glProgramUniform4iv(program, location, it->getInt4Array().size(), (GLint*)it->getInt4Array().data());
			break;
		case GraphicsUniformType::GraphicsUniformTypeUIntArray:
			glProgramUniform1uiv(program, location, it->getUIntArray().size(), it->getUIntArray().data());
			break;
		case GraphicsUniformType::GraphicsUniformTypeUInt2Array:
			glProgramUniform2uiv(program, location, it->getUInt2Array().size(), (GLuint*)it->getUInt2Array().data());
			break;
		case GraphicsUniformType::GraphicsUniformTypeUInt3Array:
			glProgramUniform3uiv(program, location, it->getUInt3Array().size(), (GLuint*)it->getUInt3Array().data());
			break;
		case GraphicsUniformType::GraphicsUniformTypeUInt4Array:
			glProgramUniform4uiv(program, location, it->getUInt4Array().size(), (GLuint*)it->getUInt4Array().data());
			break;
		case GraphicsUniformType::GraphicsUniformTypeFloatArray:
			glProgramUniform1fv(program, location, it->getFloatArray().size(), (GLfloat*)it->getFloatArray().data());
			break;
		case GraphicsUniformType::GraphicsUniformTypeFloat2Array:
			glProgramUniform2fv(program, location, it->getFloat2Array().size(), (GLfloat*)it->getFloat2Array().data());
			break;
		case GraphicsUniformType::GraphicsUniformTypeFloat3Array:
			glProgramUniform3fv(program, location, it->getFloat3Array().size(), (GLfloat*)it->getFloat3Array().data());
			break;
		case GraphicsUniformType::GraphicsUniformTypeFloat4Array:
			glProgramUniform4fv(program, location, it->getFloat4Array().size(), (GLfloat*)it->getFloat4Array().data());
			break;
		case GraphicsUniformType::GraphicsUniformTypeFloat3x3Array:
			glProgramUniformMatrix3fv(program, location, it->getFloat3x3Array().size(), GL_FALSE, (GLfloat*)it->getFloat3x3Array().data());
			break;
		case GraphicsUniformType::GraphicsUniformTypeFloat4x4Array:
			glProgramUniformMatrix4fv(program, location, it->getFloat4x4Array().size(), GL_FALSE, (GLfloat*)it->getFloat4x4Array().data());
			break;
		case GraphicsUniformType::GraphicsUniformTypeSampler:
			glBindSampler(location, it->getTextureSampler()->downcast<OGLSampler>()->getInstanceID());
			break;
		case GraphicsUniformType::GraphicsUniformTypeSamplerImage:
		{
			auto& texture = it->getTexture();
			if (texture)
			{
				auto gltexture = texture->downcast<OGLTexture>();
				glActiveTexture(GL_TEXTURE0 + location);
				glBindTexture(gltexture->getTarget(), gltexture->getInstanceID());
			}
		}
		break;
		case GraphicsUniformType::GraphicsUniformTypeCombinedImageSampler:
		{
			auto& texture = it->getTexture();
			if (texture)
			{
				auto gltexture = texture->downcast<OGLTexture>();
				glActiveTexture(GL_TEXTURE0 + location);
				glBindTexture(gltexture->getTarget(), gltexture->getInstanceID());
			}
		}
		break;
		case GraphicsUniformType::GraphicsUniformTypeStorageImage:
		{
			auto& texture = it->getTexture();
			if (texture)
			{
				auto gltexture = texture->downcast<OGLTexture>();
				glActiveTexture(GL_TEXTURE0 + location);
				glBindTexture(gltexture->getTarget(), gltexture->getInstanceID());
			}
		}
		break;
		case GraphicsUniformType::GraphicsUniformTypeStorageTexelBuffer:
			break;
		case GraphicsUniformType::GraphicsUniformTypeStorageBuffer:
			break;
		case GraphicsUniformType::GraphicsUniformTypeStorageBufferDynamic:
			break;
		case GraphicsUniformType::GraphicsUniformTypeUniformTexelBuffer:
			break;
		case GraphicsUniformType::GraphicsUniformTypeUniformBuffer:
		{
			auto& buffer = it->getBuffer();
			if (buffer)
			{
				auto ubo = buffer->downcast<OGLGraphicsData>();
				glBindBufferBase(GL_UNIFORM_BUFFER, location, ubo->getInstanceID());
			}
		}
			break;
		case GraphicsUniformType::GraphicsUniformTypeUniformBufferDynamic:
			break;
		case GraphicsUniformType::GraphicsUniformTypeInputAttachment:
			break;
		default:
			break;
		}
	}
}

const GraphicsUniformSets&
OGLDescriptorSet::getGraphicsUniformSets() const noexcept
{
	return _activeUniformSets;
}

const GraphicsDescriptorSetDesc&
OGLDescriptorSet::getGraphicsDescriptorSetDesc() const noexcept
{
	return _descriptorSetDesc;
}

void
OGLDescriptorSet::setDevice(GraphicsDevicePtr device) noexcept
{
	_device = device;
}

GraphicsDevicePtr
OGLDescriptorSet::getDevice() noexcept
{
	return _device.lock();
}

_NAME_END