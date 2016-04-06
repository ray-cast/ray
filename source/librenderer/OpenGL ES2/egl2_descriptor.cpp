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
#include "egl2_descriptor.h"
#include "egl2_texture.h"
#include "egl2_shader.h"
#include "egl2_sampler.h"

_NAME_BEGIN

__ImplementSubClass(EGL2GraphicsUniformSet, GraphicsUniformSet, "EGL2GraphicsUniformSet")
__ImplementSubClass(EGL2DescriptorSet, GraphicsDescriptorSet, "EGL2DescriptorSet")
__ImplementSubClass(EGL2DescriptorSetLayout, GraphicsDescriptorSetLayout, "EGL2DescriptorSetLayout")
__ImplementSubClass(EGL2DescriptorPool, GraphicsDescriptorPool, "EGL2DescriptorPool")

EGL2GraphicsUniformSet::EGL2GraphicsUniformSet() noexcept
{
}

EGL2GraphicsUniformSet::~EGL2GraphicsUniformSet() noexcept
{
}

void
EGL2GraphicsUniformSet::setType(GraphicsUniformType type) noexcept
{
	_variant.setType(type);
}

GraphicsUniformType
EGL2GraphicsUniformSet::getType() const noexcept
{
	return _variant.getType();
}

void
EGL2GraphicsUniformSet::assign(bool value) noexcept
{
	_variant.assign(value);
}

void
EGL2GraphicsUniformSet::assign(int value) noexcept
{
	_variant.assign(value);
}

void
EGL2GraphicsUniformSet::assign(const int2& value) noexcept
{
	_variant.assign(value);
}

void
EGL2GraphicsUniformSet::assign(const int3& value) noexcept
{
	_variant.assign(value);
}

void
EGL2GraphicsUniformSet::assign(const int4& value) noexcept
{
	_variant.assign(value);
}

void
EGL2GraphicsUniformSet::assign(uint1 value) noexcept
{
	_variant.assign(value);
}

void
EGL2GraphicsUniformSet::assign(const uint2& value) noexcept
{
	_variant.assign(value);
}

void
EGL2GraphicsUniformSet::assign(const uint3& value) noexcept
{
	_variant.assign(value);
}

void
EGL2GraphicsUniformSet::assign(const uint4& value) noexcept
{
	_variant.assign(value);
}

void
EGL2GraphicsUniformSet::assign(float value) noexcept
{
	_variant.assign(value);
}

void
EGL2GraphicsUniformSet::assign(const float2& value) noexcept
{
	_variant.assign(value);
}

void
EGL2GraphicsUniformSet::assign(const float3& value) noexcept
{
	_variant.assign(value);
}

void
EGL2GraphicsUniformSet::assign(const float4& value) noexcept
{
	_variant.assign(value);
}

void
EGL2GraphicsUniformSet::assign(const float3x3& value) noexcept
{
	_variant.assign(value);
}

void
EGL2GraphicsUniformSet::assign(const float4x4& value) noexcept
{
	_variant.assign(value);
}

void
EGL2GraphicsUniformSet::assign(const std::vector<int1>& value) noexcept
{
	_variant.assign(value);
}

void
EGL2GraphicsUniformSet::assign(const std::vector<int2>& value) noexcept
{
	_variant.assign(value);
}

void
EGL2GraphicsUniformSet::assign(const std::vector<int3>& value) noexcept
{
	_variant.assign(value);
}

void
EGL2GraphicsUniformSet::assign(const std::vector<int4>& value) noexcept
{
	_variant.assign(value);
}

void
EGL2GraphicsUniformSet::assign(const std::vector<uint1>& value) noexcept
{
	_variant.assign(value);
}

void
EGL2GraphicsUniformSet::assign(const std::vector<uint2>& value) noexcept
{
	_variant.assign(value);
}

void
EGL2GraphicsUniformSet::assign(const std::vector<uint3>& value) noexcept
{
	_variant.assign(value);
}

void
EGL2GraphicsUniformSet::assign(const std::vector<uint4>& value) noexcept
{
	_variant.assign(value);
}

void
EGL2GraphicsUniformSet::assign(const std::vector<float1>& value) noexcept
{
	_variant.assign(value);
}

void
EGL2GraphicsUniformSet::assign(const std::vector<float2>& value) noexcept
{
	_variant.assign(value);
}

void
EGL2GraphicsUniformSet::assign(const std::vector<float3>& value) noexcept
{
	_variant.assign(value);
}

void
EGL2GraphicsUniformSet::assign(const std::vector<float4>& value) noexcept
{
	_variant.assign(value);
}

void
EGL2GraphicsUniformSet::assign(const std::vector<float3x3>& value) noexcept
{
	_variant.assign(value);
}

void
EGL2GraphicsUniformSet::assign(const std::vector<float4x4>& value) noexcept
{
	_variant.assign(value);
}

void
EGL2GraphicsUniformSet::assign(GraphicsTexturePtr texture, GraphicsSamplerPtr sampler) noexcept
{
	_variant.assign(texture, sampler);
}

bool
EGL2GraphicsUniformSet::getBool() const noexcept
{
	return _variant.getBool();
}

int
EGL2GraphicsUniformSet::getInt() const noexcept
{
	return _variant.getInt();
}

const int2&
EGL2GraphicsUniformSet::getInt2() const noexcept
{
	return _variant.getInt2();
}

const int3&
EGL2GraphicsUniformSet::getInt3() const noexcept
{
	return _variant.getInt3();
}

const int4&
EGL2GraphicsUniformSet::getInt4() const noexcept
{
	return _variant.getInt4();
}

uint
EGL2GraphicsUniformSet::getUInt() const noexcept
{
	return _variant.getUInt();
}

const uint2&
EGL2GraphicsUniformSet::getUInt2() const noexcept
{
	return _variant.getUInt2();
}

const uint3&
EGL2GraphicsUniformSet::getUInt3() const noexcept
{
	return _variant.getUInt3();
}

const uint4&
EGL2GraphicsUniformSet::getUInt4() const noexcept
{
	return _variant.getUInt4();
}

float
EGL2GraphicsUniformSet::getFloat() const noexcept
{
	return _variant.getFloat();
}

const float2&
EGL2GraphicsUniformSet::getFloat2() const noexcept
{
	return _variant.getFloat2();
}

const float3&
EGL2GraphicsUniformSet::getFloat3() const noexcept
{
	return _variant.getFloat3();
}

const float4&
EGL2GraphicsUniformSet::getFloat4() const noexcept
{
	return _variant.getFloat4();
}

const float3x3&
EGL2GraphicsUniformSet::getFloat3x3() const noexcept
{
	return _variant.getFloat3x3();
}

const float4x4&
EGL2GraphicsUniformSet::getFloat4x4() const noexcept
{
	return _variant.getFloat4x4();
}

const std::vector<int1>&
EGL2GraphicsUniformSet::getIntArray() const noexcept
{
	return _variant.getIntArray();
}

const std::vector<int2>&
EGL2GraphicsUniformSet::getInt2Array() const noexcept
{
	return _variant.getInt2Array();
}

const std::vector<int3>&
EGL2GraphicsUniformSet::getInt3Array() const noexcept
{
	return _variant.getInt3Array();
}

const std::vector<int4>&
EGL2GraphicsUniformSet::getInt4Array() const noexcept
{
	return _variant.getInt4Array();
}

const std::vector<uint1>&
EGL2GraphicsUniformSet::getUIntArray() const noexcept
{
	return _variant.getUIntArray();
}

const std::vector<uint2>&
EGL2GraphicsUniformSet::getUInt2Array() const noexcept
{
	return _variant.getUInt2Array();
}

const std::vector<uint3>&
EGL2GraphicsUniformSet::getUInt3Array() const noexcept
{
	return _variant.getUInt3Array();
}

const std::vector<uint4>&
EGL2GraphicsUniformSet::getUInt4Array() const noexcept
{
	return _variant.getUInt4Array();
}

const std::vector<float1>&
EGL2GraphicsUniformSet::getFloatArray() const noexcept
{
	return _variant.getFloatArray();
}

const std::vector<float2>&
EGL2GraphicsUniformSet::getFloat2Array() const noexcept
{
	return _variant.getFloat2Array();
}

const std::vector<float3>&
EGL2GraphicsUniformSet::getFloat3Array() const noexcept
{
	return _variant.getFloat3Array();
}

const std::vector<float4>&
EGL2GraphicsUniformSet::getFloat4Array() const noexcept
{
	return _variant.getFloat4Array();
}

const std::vector<float3x3>&
EGL2GraphicsUniformSet::getFloat3x3Array() const noexcept
{
	return _variant.getFloat3x3Array();
}

const std::vector<float4x4>&
EGL2GraphicsUniformSet::getFloat4x4Array() const noexcept
{
	return _variant.getFloat4x4Array();
}

GraphicsTexturePtr
EGL2GraphicsUniformSet::getTexture() const noexcept
{
	return _variant.getTexture();
}

GraphicsSamplerPtr
EGL2GraphicsUniformSet::getTextureSampler() const noexcept
{
	return _variant.getTextureSampler();
}

void
EGL2GraphicsUniformSet::setGraphicsUniform(GraphicsUniformPtr uniform) noexcept
{
	_uniform = uniform;
}

GraphicsUniformPtr
EGL2GraphicsUniformSet::getGraphicsUniform() const noexcept
{
	return _uniform;
}

EGL2DescriptorPool::EGL2DescriptorPool() noexcept
{
}

EGL2DescriptorPool::~EGL2DescriptorPool() noexcept
{
	this->close();
}

bool
EGL2DescriptorPool::setup(const GraphicsDescriptorPoolDesc& desc) noexcept
{
	return true;
}

void
EGL2DescriptorPool::close() noexcept
{
}

const GraphicsDescriptorPoolDesc&
EGL2DescriptorPool::getGraphicsDescriptorPoolDesc() const noexcept
{
	return _descriptorPoolDesc;
}

void
EGL2DescriptorPool::setDevice(GraphicsDevicePtr device) noexcept
{
	_device = device;
}

GraphicsDevicePtr
EGL2DescriptorPool::getDevice() noexcept
{
	return _device.lock();
}

EGL2DescriptorSetLayout::EGL2DescriptorSetLayout() noexcept
{
}

EGL2DescriptorSetLayout::~EGL2DescriptorSetLayout() noexcept
{
	this->close();
}

bool
EGL2DescriptorSetLayout::setup(const GraphicsDescriptorSetLayoutDesc& descriptorSetDesc) noexcept
{
	_descriptorSetDesc = descriptorSetDesc;
	return true;
}

void
EGL2DescriptorSetLayout::close() noexcept
{
}

const GraphicsDescriptorSetLayoutDesc&
EGL2DescriptorSetLayout::getGraphicsDescriptorSetLayoutDesc() const noexcept
{
	return _descriptorSetDesc;
}

void
EGL2DescriptorSetLayout::setDevice(GraphicsDevicePtr device) noexcept
{
	_device = device;
}

GraphicsDevicePtr
EGL2DescriptorSetLayout::getDevice() noexcept
{
	return _device.lock();
}

EGL2DescriptorSet::EGL2DescriptorSet() noexcept
{
}

EGL2DescriptorSet::~EGL2DescriptorSet() noexcept
{
	this->close();
}

bool
EGL2DescriptorSet::setup(const GraphicsDescriptorSetDesc& descriptorSetDesc) noexcept
{
	assert(descriptorSetDesc.getGraphicsDescriptorSetLayout());

	auto& descriptorSetLayoutDesc = descriptorSetDesc.getGraphicsDescriptorSetLayout()->getGraphicsDescriptorSetLayoutDesc();
	auto& components = descriptorSetLayoutDesc.getUniformComponents();
	for (auto& component : components)
	{
		auto uniformSet = std::make_shared<EGL2GraphicsUniformSet>();
		uniformSet->setGraphicsUniform(component);
		uniformSet->setType(component->getType());

		_activeUniformSets.push_back(uniformSet);
	}

	_descriptorSetDesc = descriptorSetDesc;
	return true;
}

void
EGL2DescriptorSet::close() noexcept
{
}

void
EGL2DescriptorSet::bindProgram(GraphicsProgramPtr shaderObject) noexcept
{
	std::uint32_t textureUnit = 0;
	for (auto& it : _activeUniformSets)
	{
		auto uniform = it->downcast<EGL2GraphicsUniformSet>();
		auto type = it->getGraphicsUniform()->getType();
		auto location = it->getGraphicsUniform()->downcast<EGL2GraphicsUniform>()->getBindingPoint();
		switch (type)
		{
		case GraphicsUniformType::GraphicsUniformTypeBool:
			GL_CHECK(glUniform1i(location, uniform->getBool()));
			break;
		case GraphicsUniformType::GraphicsUniformTypeInt:
			GL_CHECK(glUniform1i(location, uniform->getInt()));
			break;
		case GraphicsUniformType::GraphicsUniformTypeInt2:
			GL_CHECK(glUniform2iv(location, 1, (GLint*)uniform->getInt2().ptr()));
			break;
		case GraphicsUniformType::GraphicsUniformTypeInt3:
			GL_CHECK(glUniform3iv(location, 1, (GLint*)uniform->getInt3().ptr()));
			break;
		case GraphicsUniformType::GraphicsUniformTypeInt4:
			GL_CHECK(glUniform4iv(location, 1, (GLint*)uniform->getInt4().ptr()));
			break;
		case GraphicsUniformType::GraphicsUniformTypeFloat:
			GL_CHECK(glUniform1f(location, uniform->getFloat()));
			break;
		case GraphicsUniformType::GraphicsUniformTypeFloat2:
			GL_CHECK(glUniform2fv(location, 1, uniform->getFloat2().ptr()));
			break;
		case GraphicsUniformType::GraphicsUniformTypeFloat3:
			GL_CHECK(glUniform3fv(location, 1, uniform->getFloat3().ptr()));
			break;
		case GraphicsUniformType::GraphicsUniformTypeFloat4:
			GL_CHECK(glUniform4fv(location, 1, uniform->getFloat4().ptr()));
			break;
		case GraphicsUniformType::GraphicsUniformTypeFloat3x3:
			GL_CHECK(glUniformMatrix3fv(location, 1, GL_FALSE, uniform->getFloat3x3().ptr()));
			break;
		case GraphicsUniformType::GraphicsUniformTypeFloat4x4:
			GL_CHECK(glUniformMatrix4fv(location, 1, GL_FALSE, uniform->getFloat4x4().ptr()));
			break;
		case GraphicsUniformType::GraphicsUniformTypeIntArray:
			GL_CHECK(glUniform1iv(location, uniform->getIntArray().size(), uniform->getIntArray().data()));
			break;
		case GraphicsUniformType::GraphicsUniformTypeInt2Array:
			GL_CHECK(glUniform2iv(location, uniform->getInt2Array().size(), (GLint*)uniform->getInt2Array().data()));
			break;
		case GraphicsUniformType::GraphicsUniformTypeInt3Array:
			GL_CHECK(glUniform3iv(location, uniform->getInt3Array().size(), (GLint*)uniform->getInt3Array().data()));
			break;
		case GraphicsUniformType::GraphicsUniformTypeInt4Array:
			GL_CHECK(glUniform4iv(location, uniform->getInt4Array().size(), (GLint*)uniform->getInt4Array().data()));
			break;
		case GraphicsUniformType::GraphicsUniformTypeFloatArray:
			GL_CHECK(glUniform1fv(location, uniform->getFloatArray().size(), (GLfloat*)uniform->getFloatArray().data()));
			break;
		case GraphicsUniformType::GraphicsUniformTypeFloat2Array:
			GL_CHECK(glUniform2fv(location, uniform->getFloat2Array().size(), (GLfloat*)uniform->getFloat2Array().data()));
			break;
		case GraphicsUniformType::GraphicsUniformTypeFloat3Array:
			GL_CHECK(glUniform3fv(location, uniform->getFloat3Array().size(), (GLfloat*)uniform->getFloat3Array().data()));
			break;
		case GraphicsUniformType::GraphicsUniformTypeFloat4Array:
			GL_CHECK(glUniform4fv(location, uniform->getFloat4Array().size(), (GLfloat*)uniform->getFloat4Array().data()));
			break;
		case GraphicsUniformType::GraphicsUniformTypeFloat3x3Array:
			GL_CHECK(glUniformMatrix3fv(location, uniform->getFloat3x3Array().size(), GL_FALSE, (GLfloat*)uniform->getFloat3x3Array().data()));
			break;
		case GraphicsUniformType::GraphicsUniformTypeFloat4x4Array:
			GL_CHECK(glUniformMatrix4fv(location, uniform->getFloat3x3Array().size(), GL_FALSE, (GLfloat*)uniform->getFloat4x4Array().data()));
			break;
		case GraphicsUniformType::GraphicsUniformTypeSampler:
			assert(false);
			break;
		case GraphicsUniformType::GraphicsUniformTypeSamplerImage:
			if (uniform->getTexture())
			{
				GL_CHECK(glUniform1i(location, textureUnit));
				GL_CHECK(glActiveTexture(GL_TEXTURE0 + textureUnit);)
				GL_CHECK(glBindTexture(uniform->getTexture()->downcast<EGL2Texture>()->getTarget(), uniform->getTexture()->downcast<EGL2Texture>()->getInstanceID()));
				textureUnit++;
			}
			break;
		case GraphicsUniformType::GraphicsUniformTypeCombinedImageSampler:
			if (uniform->getTexture())
			{
				GL_CHECK(glUniform1i(location, textureUnit));
				GL_CHECK(glActiveTexture(GL_TEXTURE0 + textureUnit);)
				GL_CHECK(glBindTexture(uniform->getTexture()->downcast<EGL2Texture>()->getTarget(), uniform->getTexture()->downcast<EGL2Texture>()->getInstanceID()));
				textureUnit++;
			}
			break;
		case GraphicsUniformType::GraphicsUniformTypeStorageImage:
			if (uniform->getTexture())
			{
				GL_CHECK(glUniform1i(location, textureUnit));
				GL_CHECK(glActiveTexture(GL_TEXTURE0 + textureUnit);)
				GL_CHECK(glBindTexture(uniform->getTexture()->downcast<EGL2Texture>()->getTarget(), uniform->getTexture()->downcast<EGL2Texture>()->getInstanceID()));
				textureUnit++;
			}
			break;
		case GraphicsUniformType::GraphicsUniformTypeStorageTexelBuffer:
		case GraphicsUniformType::GraphicsUniformTypeUniformTexelBuffer:
			GL_PLATFORM_LOG("Can't support TBO.");
			break;
		case GraphicsUniformType::GraphicsUniformTypeStorageBuffer:
		case GraphicsUniformType::GraphicsUniformTypeStorageBufferDynamic:
		case GraphicsUniformType::GraphicsUniformTypeUniformBuffer:
		case GraphicsUniformType::GraphicsUniformTypeUniformBufferDynamic:
			GL_PLATFORM_LOG("Can't support UBO.");
			break;
		case GraphicsUniformType::GraphicsUniformTypeInputAttachment:
			break;
		case GraphicsUniformType::GraphicsUniformTypeUInt:
		case GraphicsUniformType::GraphicsUniformTypeUInt2:
		case GraphicsUniformType::GraphicsUniformTypeUInt3:
		case GraphicsUniformType::GraphicsUniformTypeUInt4:
		case GraphicsUniformType::GraphicsUniformTypeUIntArray:
		case GraphicsUniformType::GraphicsUniformTypeUInt2Array:
		case GraphicsUniformType::GraphicsUniformTypeUInt3Array:
		case GraphicsUniformType::GraphicsUniformTypeUInt4Array:
			GL_PLATFORM_LOG("Can't support unsigned type.");
			break;
		default:
			break;
		}
	}
}

const GraphicsUniformSets&
EGL2DescriptorSet::getGraphicsUniformSets() const noexcept
{
	return _activeUniformSets;
}

const GraphicsDescriptorSetDesc&
EGL2DescriptorSet::getGraphicsDescriptorSetDesc() const noexcept
{
	return _descriptorSetDesc;
}

void
EGL2DescriptorSet::setDevice(GraphicsDevicePtr device) noexcept
{
	_device = device;
}

GraphicsDevicePtr
EGL2DescriptorSet::getDevice() noexcept
{
	return _device.lock();
}

_NAME_END