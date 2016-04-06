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
#include "egl3_descriptor.h"
#include "egl3_texture.h"
#include "egl3_shader.h"
#include "egl3_sampler.h"

_NAME_BEGIN

__ImplementSubClass(EGL3GraphicsUniformSet, GraphicsUniformSet, "EGL3GraphicsUniformSet")
__ImplementSubClass(EGL3DescriptorSet, GraphicsDescriptorSet, "EGL3DescriptorSet")
__ImplementSubClass(EGL3DescriptorSetLayout, GraphicsDescriptorSetLayout, "EGL3DescriptorSetLayout")
__ImplementSubClass(EGL3DescriptorPool, GraphicsDescriptorPool, "EGL3DescriptorPool")

EGL3GraphicsUniformSet::EGL3GraphicsUniformSet() noexcept
{
}

EGL3GraphicsUniformSet::~EGL3GraphicsUniformSet() noexcept
{
}

void
EGL3GraphicsUniformSet::setType(GraphicsUniformType type) noexcept
{
	_variant.setType(type);
}

GraphicsUniformType
EGL3GraphicsUniformSet::getType() const noexcept
{
	return _variant.getType();
}

void
EGL3GraphicsUniformSet::assign(bool value) noexcept
{
	_variant.assign(value);
}

void
EGL3GraphicsUniformSet::assign(int value) noexcept
{
	_variant.assign(value);
}

void
EGL3GraphicsUniformSet::assign(const int2& value) noexcept
{
	_variant.assign(value);
}

void
EGL3GraphicsUniformSet::assign(const int3& value) noexcept
{
	_variant.assign(value);
}

void
EGL3GraphicsUniformSet::assign(const int4& value) noexcept
{
	_variant.assign(value);
}

void
EGL3GraphicsUniformSet::assign(uint1 value) noexcept
{
	_variant.assign(value);
}

void
EGL3GraphicsUniformSet::assign(const uint2& value) noexcept
{
	_variant.assign(value);
}

void
EGL3GraphicsUniformSet::assign(const uint3& value) noexcept
{
	_variant.assign(value);
}

void
EGL3GraphicsUniformSet::assign(const uint4& value) noexcept
{
	_variant.assign(value);
}

void
EGL3GraphicsUniformSet::assign(float value) noexcept
{
	_variant.assign(value);
}

void
EGL3GraphicsUniformSet::assign(const float2& value) noexcept
{
	_variant.assign(value);
}

void
EGL3GraphicsUniformSet::assign(const float3& value) noexcept
{
	_variant.assign(value);
}

void
EGL3GraphicsUniformSet::assign(const float4& value) noexcept
{
	_variant.assign(value);
}

void
EGL3GraphicsUniformSet::assign(const float3x3& value) noexcept
{
	_variant.assign(value);
}

void
EGL3GraphicsUniformSet::assign(const float4x4& value) noexcept
{
	_variant.assign(value);
}

void
EGL3GraphicsUniformSet::assign(const std::vector<int1>& value) noexcept
{
	_variant.assign(value);
}

void
EGL3GraphicsUniformSet::assign(const std::vector<int2>& value) noexcept
{
	_variant.assign(value);
}

void
EGL3GraphicsUniformSet::assign(const std::vector<int3>& value) noexcept
{
	_variant.assign(value);
}

void
EGL3GraphicsUniformSet::assign(const std::vector<int4>& value) noexcept
{
	_variant.assign(value);
}

void
EGL3GraphicsUniformSet::assign(const std::vector<uint1>& value) noexcept
{
	_variant.assign(value);
}

void
EGL3GraphicsUniformSet::assign(const std::vector<uint2>& value) noexcept
{
	_variant.assign(value);
}

void
EGL3GraphicsUniformSet::assign(const std::vector<uint3>& value) noexcept
{
	_variant.assign(value);
}

void
EGL3GraphicsUniformSet::assign(const std::vector<uint4>& value) noexcept
{
	_variant.assign(value);
}

void
EGL3GraphicsUniformSet::assign(const std::vector<float1>& value) noexcept
{
	_variant.assign(value);
}

void
EGL3GraphicsUniformSet::assign(const std::vector<float2>& value) noexcept
{
	_variant.assign(value);
}

void
EGL3GraphicsUniformSet::assign(const std::vector<float3>& value) noexcept
{
	_variant.assign(value);
}

void
EGL3GraphicsUniformSet::assign(const std::vector<float4>& value) noexcept
{
	_variant.assign(value);
}

void
EGL3GraphicsUniformSet::assign(const std::vector<float3x3>& value) noexcept
{
	_variant.assign(value);
}

void
EGL3GraphicsUniformSet::assign(const std::vector<float4x4>& value) noexcept
{
	_variant.assign(value);
}

void
EGL3GraphicsUniformSet::assign(GraphicsTexturePtr texture, GraphicsSamplerPtr sampler) noexcept
{
	_variant.assign(texture, sampler);
}

bool
EGL3GraphicsUniformSet::getBool() const noexcept
{
	return _variant.getBool();
}

int
EGL3GraphicsUniformSet::getInt() const noexcept
{
	return _variant.getInt();
}

const int2&
EGL3GraphicsUniformSet::getInt2() const noexcept
{
	return _variant.getInt2();
}

const int3&
EGL3GraphicsUniformSet::getInt3() const noexcept
{
	return _variant.getInt3();
}

const int4&
EGL3GraphicsUniformSet::getInt4() const noexcept
{
	return _variant.getInt4();
}

uint
EGL3GraphicsUniformSet::getUInt() const noexcept
{
	return _variant.getUInt();
}

const uint2&
EGL3GraphicsUniformSet::getUInt2() const noexcept
{
	return _variant.getUInt2();
}

const uint3&
EGL3GraphicsUniformSet::getUInt3() const noexcept
{
	return _variant.getUInt3();
}

const uint4&
EGL3GraphicsUniformSet::getUInt4() const noexcept
{
	return _variant.getUInt4();
}

float
EGL3GraphicsUniformSet::getFloat() const noexcept
{
	return _variant.getFloat();
}

const float2&
EGL3GraphicsUniformSet::getFloat2() const noexcept
{
	return _variant.getFloat2();
}

const float3&
EGL3GraphicsUniformSet::getFloat3() const noexcept
{
	return _variant.getFloat3();
}

const float4&
EGL3GraphicsUniformSet::getFloat4() const noexcept
{
	return _variant.getFloat4();
}

const float3x3&
EGL3GraphicsUniformSet::getFloat3x3() const noexcept
{
	return _variant.getFloat3x3();
}

const float4x4&
EGL3GraphicsUniformSet::getFloat4x4() const noexcept
{
	return _variant.getFloat4x4();
}

const std::vector<int1>&
EGL3GraphicsUniformSet::getIntArray() const noexcept
{
	return _variant.getIntArray();
}

const std::vector<int2>&
EGL3GraphicsUniformSet::getInt2Array() const noexcept
{
	return _variant.getInt2Array();
}

const std::vector<int3>&
EGL3GraphicsUniformSet::getInt3Array() const noexcept
{
	return _variant.getInt3Array();
}

const std::vector<int4>&
EGL3GraphicsUniformSet::getInt4Array() const noexcept
{
	return _variant.getInt4Array();
}

const std::vector<uint1>&
EGL3GraphicsUniformSet::getUIntArray() const noexcept
{
	return _variant.getUIntArray();
}

const std::vector<uint2>&
EGL3GraphicsUniformSet::getUInt2Array() const noexcept
{
	return _variant.getUInt2Array();
}

const std::vector<uint3>&
EGL3GraphicsUniformSet::getUInt3Array() const noexcept
{
	return _variant.getUInt3Array();
}

const std::vector<uint4>&
EGL3GraphicsUniformSet::getUInt4Array() const noexcept
{
	return _variant.getUInt4Array();
}

const std::vector<float1>&
EGL3GraphicsUniformSet::getFloatArray() const noexcept
{
	return _variant.getFloatArray();
}

const std::vector<float2>&
EGL3GraphicsUniformSet::getFloat2Array() const noexcept
{
	return _variant.getFloat2Array();
}

const std::vector<float3>&
EGL3GraphicsUniformSet::getFloat3Array() const noexcept
{
	return _variant.getFloat3Array();
}

const std::vector<float4>&
EGL3GraphicsUniformSet::getFloat4Array() const noexcept
{
	return _variant.getFloat4Array();
}

const std::vector<float3x3>&
EGL3GraphicsUniformSet::getFloat3x3Array() const noexcept
{
	return _variant.getFloat3x3Array();
}

const std::vector<float4x4>&
EGL3GraphicsUniformSet::getFloat4x4Array() const noexcept
{
	return _variant.getFloat4x4Array();
}

GraphicsTexturePtr
EGL3GraphicsUniformSet::getTexture() const noexcept
{
	return _variant.getTexture();
}

GraphicsSamplerPtr
EGL3GraphicsUniformSet::getTextureSampler() const noexcept
{
	return _variant.getTextureSampler();
}

void
EGL3GraphicsUniformSet::setGraphicsUniform(GraphicsUniformPtr uniform) noexcept
{
	_uniform = uniform;
}

GraphicsUniformPtr
EGL3GraphicsUniformSet::getGraphicsUniform() const noexcept
{
	return _uniform;
}

EGL3DescriptorPool::EGL3DescriptorPool() noexcept
{
}

EGL3DescriptorPool::~EGL3DescriptorPool() noexcept
{
	this->close();
}

bool
EGL3DescriptorPool::setup(const GraphicsDescriptorPoolDesc& desc) noexcept
{
	return true;
}

void
EGL3DescriptorPool::close() noexcept
{
}

const GraphicsDescriptorPoolDesc&
EGL3DescriptorPool::getGraphicsDescriptorPoolDesc() const noexcept
{
	return _descriptorPoolDesc;
}

void
EGL3DescriptorPool::setDevice(GraphicsDevicePtr device) noexcept
{
	_device = device;
}

GraphicsDevicePtr
EGL3DescriptorPool::getDevice() noexcept
{
	return _device.lock();
}

EGL3DescriptorSetLayout::EGL3DescriptorSetLayout() noexcept
{
}

EGL3DescriptorSetLayout::~EGL3DescriptorSetLayout() noexcept
{
	this->close();
}

bool
EGL3DescriptorSetLayout::setup(const GraphicsDescriptorSetLayoutDesc& descriptorSetDesc) noexcept
{
	_descriptorSetDesc = descriptorSetDesc;
	return true;
}

void
EGL3DescriptorSetLayout::close() noexcept
{
}

const GraphicsDescriptorSetLayoutDesc&
EGL3DescriptorSetLayout::getGraphicsDescriptorSetLayoutDesc() const noexcept
{
	return _descriptorSetDesc;
}

void
EGL3DescriptorSetLayout::setDevice(GraphicsDevicePtr device) noexcept
{
	_device = device;
}

GraphicsDevicePtr
EGL3DescriptorSetLayout::getDevice() noexcept
{
	return _device.lock();
}

EGL3DescriptorSet::EGL3DescriptorSet() noexcept
{
}

EGL3DescriptorSet::~EGL3DescriptorSet() noexcept
{
	this->close();
}

bool
EGL3DescriptorSet::setup(const GraphicsDescriptorSetDesc& descriptorSetDesc) noexcept
{
	assert(descriptorSetDesc.getGraphicsDescriptorSetLayout());

	auto& descriptorSetLayoutDesc = descriptorSetDesc.getGraphicsDescriptorSetLayout()->getGraphicsDescriptorSetLayoutDesc();
	auto& components = descriptorSetLayoutDesc.getUniformComponents();
	for (auto& component : components)
	{
		auto uniformSet = std::make_shared<EGL3GraphicsUniformSet>();
		uniformSet->setGraphicsUniform(component);
		uniformSet->setType(component->getType());

		_activeUniformSets.push_back(uniformSet);
	}

	_descriptorSetDesc = descriptorSetDesc;
	return true;
}

void
EGL3DescriptorSet::close() noexcept
{
}

void
EGL3DescriptorSet::bindProgram(GraphicsProgramPtr shaderObject) noexcept
{
	std::uint32_t textureUnit = 0;
	auto program = shaderObject->downcast<EGL3ShaderObject>()->getInstanceID();
	for (auto& it : _activeUniformSets)
	{
		auto uniform = it->downcast<EGL3GraphicsUniformSet>();
		auto type = it->getGraphicsUniform()->getType();
		auto location = it->getGraphicsUniform()->downcast<EGL3GraphicsUniform>()->getBindingPoint();
		switch (type)
		{
		case GraphicsUniformType::GraphicsUniformTypeBool:
			GL_CHECK(glProgramUniform1i(program, location, uniform->getBool()));
			break;
		case GraphicsUniformType::GraphicsUniformTypeInt:
			GL_CHECK(glProgramUniform1i(program, location, uniform->getInt()));
			break;
		case GraphicsUniformType::GraphicsUniformTypeInt2:
			GL_CHECK(glProgramUniform2iv(program, location, 1, (GLint*)uniform->getInt2().ptr()));
			break;
		case GraphicsUniformType::GraphicsUniformTypeInt3:
			GL_CHECK(glProgramUniform3iv(program, location, 1, (GLint*)uniform->getInt3().ptr()));
			break;
		case GraphicsUniformType::GraphicsUniformTypeInt4:
			GL_CHECK(glProgramUniform4iv(program, location, 1, (GLint*)uniform->getInt4().ptr()));
			break;
		case GraphicsUniformType::GraphicsUniformTypeUInt:
			GL_CHECK(glProgramUniform1ui(program, location, uniform->getUInt()));
			break;
		case GraphicsUniformType::GraphicsUniformTypeUInt2:
			GL_CHECK(glProgramUniform2uiv(program, location, 1, (GLuint*)uniform->getUInt2().ptr()));
			break;
		case GraphicsUniformType::GraphicsUniformTypeUInt3:
			GL_CHECK(glProgramUniform3uiv(program, location, 1, (GLuint*)uniform->getUInt3().ptr()));
			break;
		case GraphicsUniformType::GraphicsUniformTypeUInt4:
			GL_CHECK(glProgramUniform4uiv(program, location, 1, (GLuint*)uniform->getUInt4().ptr()));
			break;
		case GraphicsUniformType::GraphicsUniformTypeFloat:
			GL_CHECK(glProgramUniform1f(program, location, uniform->getFloat()));
			break;
		case GraphicsUniformType::GraphicsUniformTypeFloat2:
			GL_CHECK(glProgramUniform2fv(program, location, 1, uniform->getFloat2().ptr()));
			break;
		case GraphicsUniformType::GraphicsUniformTypeFloat3:
			GL_CHECK(glProgramUniform3fv(program, location, 1, uniform->getFloat3().ptr()));
			break;
		case GraphicsUniformType::GraphicsUniformTypeFloat4:
			GL_CHECK(glProgramUniform4fv(program, location, 1, uniform->getFloat4().ptr()));
			break;
		case GraphicsUniformType::GraphicsUniformTypeFloat3x3:
			GL_CHECK(glProgramUniformMatrix3fv(program, location, 1, GL_FALSE, uniform->getFloat3x3().ptr()));
			break;
		case GraphicsUniformType::GraphicsUniformTypeFloat4x4:
			GL_CHECK(glProgramUniformMatrix4fv(program, location, 1, GL_FALSE, uniform->getFloat4x4().ptr()));
			break;
		case GraphicsUniformType::GraphicsUniformTypeIntArray:
			GL_CHECK(glProgramUniform1iv(program, location, uniform->getIntArray().size(), uniform->getIntArray().data()));
			break;
		case GraphicsUniformType::GraphicsUniformTypeInt2Array:
			GL_CHECK(glProgramUniform2iv(program, location, uniform->getInt2Array().size(), (GLint*)uniform->getInt2Array().data()));
			break;
		case GraphicsUniformType::GraphicsUniformTypeInt3Array:
			GL_CHECK(glProgramUniform3iv(program, location, uniform->getInt3Array().size(), (GLint*)uniform->getInt3Array().data()));
			break;
		case GraphicsUniformType::GraphicsUniformTypeInt4Array:
			GL_CHECK(glProgramUniform4iv(program, location, uniform->getInt4Array().size(), (GLint*)uniform->getInt4Array().data()));
			break;
		case GraphicsUniformType::GraphicsUniformTypeUIntArray:
			GL_CHECK(glProgramUniform1uiv(program, location, uniform->getUIntArray().size(), uniform->getUIntArray().data()));
			break;
		case GraphicsUniformType::GraphicsUniformTypeUInt2Array:
			GL_CHECK(glProgramUniform2uiv(program, location, uniform->getUInt2Array().size(), (GLuint*)uniform->getUInt2Array().data()));
			break;
		case GraphicsUniformType::GraphicsUniformTypeUInt3Array:
			GL_CHECK(glProgramUniform3uiv(program, location, uniform->getUInt3Array().size(), (GLuint*)uniform->getUInt3Array().data()));
			break;
		case GraphicsUniformType::GraphicsUniformTypeUInt4Array:
			GL_CHECK(glProgramUniform4uiv(program, location, uniform->getUInt4Array().size(), (GLuint*)uniform->getUInt4Array().data()));
			break;
		case GraphicsUniformType::GraphicsUniformTypeFloatArray:
			GL_CHECK(glProgramUniform1fv(program, location, uniform->getFloatArray().size(), (GLfloat*)uniform->getFloatArray().data()));
			break;
		case GraphicsUniformType::GraphicsUniformTypeFloat2Array:
			GL_CHECK(glProgramUniform2fv(program, location, uniform->getFloat2Array().size(), (GLfloat*)uniform->getFloat2Array().data()));
			break;
		case GraphicsUniformType::GraphicsUniformTypeFloat3Array:
			GL_CHECK(glProgramUniform3fv(program, location, uniform->getFloat3Array().size(), (GLfloat*)uniform->getFloat3Array().data()));
			break;
		case GraphicsUniformType::GraphicsUniformTypeFloat4Array:
			GL_CHECK(glProgramUniform4fv(program, location, uniform->getFloat4Array().size(), (GLfloat*)uniform->getFloat4Array().data()));
			break;
		case GraphicsUniformType::GraphicsUniformTypeFloat3x3Array:
			GL_CHECK(glProgramUniformMatrix3fv(program, location, uniform->getFloat3x3Array().size(), GL_FALSE, (GLfloat*)uniform->getFloat3x3Array().data()));
			break;
		case GraphicsUniformType::GraphicsUniformTypeFloat4x4Array:
			GL_CHECK(glProgramUniformMatrix4fv(program, location, uniform->getFloat3x3Array().size(), GL_FALSE, (GLfloat*)uniform->getFloat4x4Array().data()));
			break;
		case GraphicsUniformType::GraphicsUniformTypeSampler:
			glBindSampler(location, uniform->getTextureSampler()->downcast<EGL3Sampler>()->getInstanceID());
			break;
		case GraphicsUniformType::GraphicsUniformTypeSamplerImage:
			{
				if (uniform->getTexture())
				{
					GL_CHECK(glProgramUniform1i(program, location, textureUnit));
					GL_CHECK(glActiveTexture(GL_TEXTURE0 + textureUnit);)
					GL_CHECK(glBindTexture(uniform->getTexture()->downcast<EGL3Texture>()->getTarget(), uniform->getTexture()->downcast<EGL3Texture>()->getInstanceID()));
					textureUnit++;
				}
			}
			break;
		case GraphicsUniformType::GraphicsUniformTypeCombinedImageSampler:
			{
				if (uniform->getTexture())
				{
					GL_CHECK(glProgramUniform1i(program, location, textureUnit));
					GL_CHECK(glActiveTexture(GL_TEXTURE0 + textureUnit);)
					GL_CHECK(glBindTexture(uniform->getTexture()->downcast<EGL3Texture>()->getTarget(), uniform->getTexture()->downcast<EGL3Texture>()->getInstanceID()));
					textureUnit++;
				}
			}
			break;
		case GraphicsUniformType::GraphicsUniformTypeStorageImage:
			{
				if (uniform->getTexture())
				{
					GL_CHECK(glProgramUniform1i(program, location, textureUnit));
					GL_CHECK(glActiveTexture(GL_TEXTURE0 + textureUnit);)
					GL_CHECK(glBindTexture(uniform->getTexture()->downcast<EGL3Texture>()->getTarget(), uniform->getTexture()->downcast<EGL3Texture>()->getInstanceID()));
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
EGL3DescriptorSet::getGraphicsUniformSets() const noexcept
{
	return _activeUniformSets;
}

const GraphicsDescriptorSetDesc&
EGL3DescriptorSet::getGraphicsDescriptorSetDesc() const noexcept
{
	return _descriptorSetDesc;
}

void
EGL3DescriptorSet::setDevice(GraphicsDevicePtr device) noexcept
{
	_device = device;
}

GraphicsDevicePtr
EGL3DescriptorSet::getDevice() noexcept
{
	return _device.lock();
}

_NAME_END