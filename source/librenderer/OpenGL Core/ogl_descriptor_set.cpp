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
OGLGraphicsUniformSet::setType(GraphicsUniformType type) noexcept
{
	_variant.setType(type);
}

GraphicsUniformType
OGLGraphicsUniformSet::getType() const noexcept
{
	return _variant.getType();
}

void
OGLGraphicsUniformSet::assign(bool value) noexcept
{
	_variant.assign(value);
}

void
OGLGraphicsUniformSet::assign(int value) noexcept
{
	_variant.assign(value);
}

void
OGLGraphicsUniformSet::assign(const int2& value) noexcept
{
	_variant.assign(value);
}

void
OGLGraphicsUniformSet::assign(const int3& value) noexcept
{
	_variant.assign(value);
}

void
OGLGraphicsUniformSet::assign(const int4& value) noexcept
{
	_variant.assign(value);
}

void
OGLGraphicsUniformSet::assign(uint1 value) noexcept
{
	_variant.assign(value);
}

void
OGLGraphicsUniformSet::assign(const uint2& value) noexcept
{
	_variant.assign(value);
}

void
OGLGraphicsUniformSet::assign(const uint3& value) noexcept
{
	_variant.assign(value);
}

void
OGLGraphicsUniformSet::assign(const uint4& value) noexcept
{
	_variant.assign(value);
}

void
OGLGraphicsUniformSet::assign(float value) noexcept
{
	_variant.assign(value);
}

void
OGLGraphicsUniformSet::assign(const float2& value) noexcept
{
	_variant.assign(value);
}

void
OGLGraphicsUniformSet::assign(const float3& value) noexcept
{
	_variant.assign(value);
}

void
OGLGraphicsUniformSet::assign(const float4& value) noexcept
{
	_variant.assign(value);
}

void
OGLGraphicsUniformSet::assign(const float3x3& value) noexcept
{
	_variant.assign(value);
}

void
OGLGraphicsUniformSet::assign(const float4x4& value) noexcept
{
	_variant.assign(value);
}

void
OGLGraphicsUniformSet::assign(const std::vector<int1>& value) noexcept
{
	_variant.assign(value);
}

void
OGLGraphicsUniformSet::assign(const std::vector<int2>& value) noexcept
{
	_variant.assign(value);
}

void
OGLGraphicsUniformSet::assign(const std::vector<int3>& value) noexcept
{
	_variant.assign(value);
}

void
OGLGraphicsUniformSet::assign(const std::vector<int4>& value) noexcept
{
	_variant.assign(value);
}

void
OGLGraphicsUniformSet::assign(const std::vector<uint1>& value) noexcept
{
	_variant.assign(value);
}

void
OGLGraphicsUniformSet::assign(const std::vector<uint2>& value) noexcept
{
	_variant.assign(value);
}

void
OGLGraphicsUniformSet::assign(const std::vector<uint3>& value) noexcept
{
	_variant.assign(value);
}

void
OGLGraphicsUniformSet::assign(const std::vector<uint4>& value) noexcept
{
	_variant.assign(value);
}

void
OGLGraphicsUniformSet::assign(const std::vector<float1>& value) noexcept
{
	_variant.assign(value);
}

void
OGLGraphicsUniformSet::assign(const std::vector<float2>& value) noexcept
{
	_variant.assign(value);
}

void
OGLGraphicsUniformSet::assign(const std::vector<float3>& value) noexcept
{
	_variant.assign(value);
}

void
OGLGraphicsUniformSet::assign(const std::vector<float4>& value) noexcept
{
	_variant.assign(value);
}

void
OGLGraphicsUniformSet::assign(const std::vector<float3x3>& value) noexcept
{
	_variant.assign(value);
}

void
OGLGraphicsUniformSet::assign(const std::vector<float4x4>& value) noexcept
{
	_variant.assign(value);
}

void
OGLGraphicsUniformSet::assign(GraphicsTexturePtr texture, GraphicsSamplerPtr sampler) noexcept
{
	_variant.assign(texture, sampler);
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

GraphicsTexturePtr
OGLGraphicsUniformSet::getTexture() const noexcept
{
	return _variant.getTexture();
}

GraphicsSamplerPtr
OGLGraphicsUniformSet::getTextureSampler() const noexcept
{
	return _variant.getTextureSampler();
}

void
OGLGraphicsUniformSet::setGraphicsUniform(GraphicsUniformPtr uniform) noexcept
{
	_uniform = uniform;
}

GraphicsUniformPtr
OGLGraphicsUniformSet::getGraphicsUniform() const noexcept
{
	return _uniform;
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
	auto& components = descriptorSetLayoutDesc.getUniformComponents();
	for (auto& component : components)
	{
		auto uniformSet = std::make_shared<OGLGraphicsUniformSet>();
		uniformSet->setGraphicsUniform(component);
		uniformSet->setType(component->getType());

		_activeUniformSets.push_back(uniformSet);
	}

	_descriptorSetDesc = descriptorSetDesc;
	return true;
}

void
OGLDescriptorSet::close() noexcept
{
}

void
OGLDescriptorSet::apply(GraphicsProgramPtr shaderObject) noexcept
{
	std::uint32_t textureUnit = 0;
	auto program = shaderObject->downcast<OGLProgram>()->getInstanceID();
	for (auto& it : _activeUniformSets)
	{
		auto uniform = it->downcast<OGLGraphicsUniformSet>();
		auto type = it->getGraphicsUniform()->getType();
		auto location = it->getGraphicsUniform()->downcast<OGLGraphicsUniform>()->getBindingPoint();
		switch (type)
		{
		case GraphicsUniformType::GraphicsUniformTypeBool:
			glProgramUniform1i(program, location, uniform->getBool());
			break;
		case GraphicsUniformType::GraphicsUniformTypeInt:
			glProgramUniform1i(program, location, uniform->getInt());
			break;
		case GraphicsUniformType::GraphicsUniformTypeInt2:
			glProgramUniform2iv(program, location, 1, (GLint*)uniform->getInt2().ptr());
			break;
		case GraphicsUniformType::GraphicsUniformTypeInt3:
			glProgramUniform3iv(program, location, 1, (GLint*)uniform->getInt3().ptr());
			break;
		case GraphicsUniformType::GraphicsUniformTypeInt4:
			glProgramUniform4iv(program, location, 1, (GLint*)uniform->getInt4().ptr());
			break;
		case GraphicsUniformType::GraphicsUniformTypeUInt:
			glProgramUniform1ui(program, location, uniform->getUInt());
			break;
		case GraphicsUniformType::GraphicsUniformTypeUInt2:
			glProgramUniform2uiv(program, location, 1, (GLuint*)uniform->getUInt2().ptr());
			break;
		case GraphicsUniformType::GraphicsUniformTypeUInt3:
			glProgramUniform3uiv(program, location, 1, (GLuint*)uniform->getUInt3().ptr());
			break;
		case GraphicsUniformType::GraphicsUniformTypeUInt4:
			glProgramUniform4uiv(program, location, 1, (GLuint*)uniform->getUInt4().ptr());
			break;
		case GraphicsUniformType::GraphicsUniformTypeFloat:
			glProgramUniform1f(program, location, uniform->getFloat());
			break;
		case GraphicsUniformType::GraphicsUniformTypeFloat2:
			glProgramUniform2fv(program, location, 1, uniform->getFloat2().ptr());
			break;
		case GraphicsUniformType::GraphicsUniformTypeFloat3:
			glProgramUniform3fv(program, location, 1, uniform->getFloat3().ptr());
			break;
		case GraphicsUniformType::GraphicsUniformTypeFloat4:
			glProgramUniform4fv(program, location, 1, uniform->getFloat4().ptr());
			break;
		case GraphicsUniformType::GraphicsUniformTypeFloat3x3:
			glProgramUniformMatrix3fv(program, location, 1, GL_FALSE, uniform->getFloat3x3().ptr());
			break;
		case GraphicsUniformType::GraphicsUniformTypeFloat4x4:
			glProgramUniformMatrix4fv(program, location, 1, GL_FALSE, uniform->getFloat4x4().ptr());
			break;
		case GraphicsUniformType::GraphicsUniformTypeIntArray:
			glProgramUniform1iv(program, location, uniform->getIntArray().size(), uniform->getIntArray().data());
			break;
		case GraphicsUniformType::GraphicsUniformTypeInt2Array:
			glProgramUniform2iv(program, location, uniform->getInt2Array().size(), (GLint*)uniform->getInt2Array().data());
			break;
		case GraphicsUniformType::GraphicsUniformTypeInt3Array:
			glProgramUniform3iv(program, location, uniform->getInt3Array().size(), (GLint*)uniform->getInt3Array().data());
			break;
		case GraphicsUniformType::GraphicsUniformTypeInt4Array:
			glProgramUniform4iv(program, location, uniform->getInt4Array().size(), (GLint*)uniform->getInt4Array().data());
			break;
		case GraphicsUniformType::GraphicsUniformTypeUIntArray:
			glProgramUniform1uiv(program, location, uniform->getUIntArray().size(), uniform->getUIntArray().data());
			break;
		case GraphicsUniformType::GraphicsUniformTypeUInt2Array:
			glProgramUniform2uiv(program, location, uniform->getUInt2Array().size(), (GLuint*)uniform->getUInt2Array().data());
			break;
		case GraphicsUniformType::GraphicsUniformTypeUInt3Array:
			glProgramUniform3uiv(program, location, uniform->getUInt3Array().size(), (GLuint*)uniform->getUInt3Array().data());
			break;
		case GraphicsUniformType::GraphicsUniformTypeUInt4Array:
			glProgramUniform4uiv(program, location, uniform->getUInt4Array().size(), (GLuint*)uniform->getUInt4Array().data());
			break;
		case GraphicsUniformType::GraphicsUniformTypeFloatArray:
			glProgramUniform1fv(program, location, uniform->getFloatArray().size(), (GLfloat*)uniform->getFloatArray().data());
			break;
		case GraphicsUniformType::GraphicsUniformTypeFloat2Array:
			glProgramUniform2fv(program, location, uniform->getFloat2Array().size(), (GLfloat*)uniform->getFloat2Array().data());
			break;
		case GraphicsUniformType::GraphicsUniformTypeFloat3Array:
			glProgramUniform3fv(program, location, uniform->getFloat3Array().size(), (GLfloat*)uniform->getFloat3Array().data());
			break;
		case GraphicsUniformType::GraphicsUniformTypeFloat4Array:
			glProgramUniform4fv(program, location, uniform->getFloat4Array().size(), (GLfloat*)uniform->getFloat4Array().data());
			break;
		case GraphicsUniformType::GraphicsUniformTypeFloat3x3Array:
			glProgramUniformMatrix3fv(program, location, uniform->getFloat3x3Array().size(), GL_FALSE, (GLfloat*)uniform->getFloat3x3Array().data());
			break;
		case GraphicsUniformType::GraphicsUniformTypeFloat4x4Array:
			glProgramUniformMatrix4fv(program, location, uniform->getFloat3x3Array().size(), GL_FALSE, (GLfloat*)uniform->getFloat4x4Array().data());
			break;
		case GraphicsUniformType::GraphicsUniformTypeSampler:
			glBindSampler(location, uniform->getTextureSampler()->downcast<OGLSampler>()->getInstanceID());
			break;
		case GraphicsUniformType::GraphicsUniformTypeSamplerImage:
		{
			auto texture = uniform->getTexture();
			if (texture)
			{
				auto gltexture = texture->downcast<OGLTexture>();
				glProgramUniform1i(program, location, textureUnit);
				glActiveTexture(GL_TEXTURE0 + textureUnit);
				glBindTexture(gltexture->getTarget(), gltexture->getInstanceID());
				textureUnit++;
			}
		}
		break;
		case GraphicsUniformType::GraphicsUniformTypeCombinedImageSampler:
		{
			auto texture = uniform->getTexture();
			if (texture)
			{
				auto gltexture = texture->downcast<OGLTexture>();
				glProgramUniform1i(program, location, textureUnit);
				glActiveTexture(GL_TEXTURE0 + textureUnit);
				glBindTexture(gltexture->getTarget(), gltexture->getInstanceID());
				textureUnit++;
			}
		}
		break;
		case GraphicsUniformType::GraphicsUniformTypeStorageImage:
		{
			auto texture = uniform->getTexture();
			if (texture)
			{
				auto gltexture = texture->downcast<OGLTexture>();
				glProgramUniform1i(program, location, textureUnit);
				glActiveTexture(GL_TEXTURE0 + textureUnit);
				glBindTexture(gltexture->getTarget(), gltexture->getInstanceID());
				textureUnit++;
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