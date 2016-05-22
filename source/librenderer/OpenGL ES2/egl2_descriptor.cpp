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
EGL2GraphicsUniformSet::uniform1b(bool value) noexcept
{
	_variant.uniform1b(value);
}

void
EGL2GraphicsUniformSet::uniform1i(std::int32_t i1) noexcept
{
	_variant.uniform1i(i1);
}

void
EGL2GraphicsUniformSet::uniform2i(const int2& value) noexcept
{
	_variant.uniform2i(value);
}

void
EGL2GraphicsUniformSet::uniform2i(std::int32_t i1, std::int32_t i2) noexcept
{
	_variant.uniform2i(i1, i2);
}

void
EGL2GraphicsUniformSet::uniform3i(const int3& value) noexcept
{
	_variant.uniform3i(value);
}

void
EGL2GraphicsUniformSet::uniform3i(std::int32_t i1, std::int32_t i2, std::int32_t i3) noexcept
{
	_variant.uniform3i(i1, i2, i3);
}

void
EGL2GraphicsUniformSet::uniform4i(const int4& value) noexcept
{
	_variant.uniform4i(value);
}

void
EGL2GraphicsUniformSet::uniform4i(std::int32_t i1, std::int32_t i2, std::int32_t i3, std::int32_t i4) noexcept
{
	_variant.uniform4i(i1, i2, i3, i4);
}

void
EGL2GraphicsUniformSet::uniform1ui(std::uint32_t ui1) noexcept
{
	_variant.uniform1ui(ui1);
}

void
EGL2GraphicsUniformSet::uniform2ui(const uint2& value) noexcept
{
	_variant.uniform2ui(value);
}

void
EGL2GraphicsUniformSet::uniform2ui(std::uint32_t ui1, std::uint32_t ui2) noexcept
{
	_variant.uniform2ui(ui1, ui2);
}

void
EGL2GraphicsUniformSet::uniform3ui(const uint3& value) noexcept
{
	_variant.uniform3ui(value);
}

void
EGL2GraphicsUniformSet::uniform3ui(std::uint32_t ui1, std::uint32_t ui2, std::uint32_t ui3) noexcept
{
	_variant.uniform3ui(ui1, ui2, ui3);
}

void
EGL2GraphicsUniformSet::uniform4ui(const uint4& value) noexcept
{
	_variant.uniform4ui(value);
}

void
EGL2GraphicsUniformSet::uniform4ui(std::uint32_t ui1, std::uint32_t ui2, std::uint32_t ui3, std::uint32_t ui4) noexcept
{
	_variant.uniform4ui(ui1, ui2, ui3, ui4);
}

void
EGL2GraphicsUniformSet::uniform1f(float f1) noexcept
{
	_variant.uniform1f(f1);
}

void
EGL2GraphicsUniformSet::uniform2f(const float2& value) noexcept
{
	_variant.uniform2f(value);
}

void
EGL2GraphicsUniformSet::uniform2f(float f1, float f2) noexcept
{
	_variant.uniform2f(f1, f2);
}

void
EGL2GraphicsUniformSet::uniform3f(const float3& value) noexcept
{
	_variant.uniform3f(value);
}

void
EGL2GraphicsUniformSet::uniform3f(float f1, float f2, float f3) noexcept
{
	_variant.uniform3f(f1, f2, f3);
}

void
EGL2GraphicsUniformSet::uniform4f(const float4& value) noexcept
{
	_variant.uniform4f(value);
}

void
EGL2GraphicsUniformSet::uniform4f(float f1, float f2, float f3, float f4) noexcept
{
	_variant.uniform4f(f1, f2, f3, f4);
}

void
EGL2GraphicsUniformSet::uniform2fmat(const float2x2& value) noexcept
{
	_variant.uniform2fmat(value);
}

void
EGL2GraphicsUniformSet::uniform2fmat(const float* mat2) noexcept
{
	_variant.uniform2fmat(mat2);
}

void
EGL2GraphicsUniformSet::uniform3fmat(const float3x3& value) noexcept
{
	_variant.uniform3fmat(value);
}

void
EGL2GraphicsUniformSet::uniform3fmat(const float* mat3) noexcept
{
	_variant.uniform3fmat(mat3);
}

void
EGL2GraphicsUniformSet::uniform4fmat(const float4x4& value) noexcept
{
	_variant.uniform4fmat(value);
}

void
EGL2GraphicsUniformSet::uniform4fmat(const float* mat4) noexcept
{
	_variant.uniform4fmat(mat4);
}

void
EGL2GraphicsUniformSet::uniform1iv(const std::vector<int1>& value) noexcept
{
	_variant.uniform1iv(value);
}

void
EGL2GraphicsUniformSet::uniform1iv(std::size_t num, const std::int32_t* i1v) noexcept
{
	_variant.uniform1iv(num, i1v);
}

void
EGL2GraphicsUniformSet::uniform2iv(const std::vector<int2>& value) noexcept
{
	_variant.uniform2iv(value);
}

void
EGL2GraphicsUniformSet::uniform2iv(std::size_t num, const std::int32_t* i2v) noexcept
{
	_variant.uniform2iv(num, i2v);
}

void
EGL2GraphicsUniformSet::uniform3iv(const std::vector<int3>& value) noexcept
{
	_variant.uniform3iv(value);
}

void
EGL2GraphicsUniformSet::uniform3iv(std::size_t num, const std::int32_t* i3v) noexcept
{
	_variant.uniform3iv(num, i3v);
}

void
EGL2GraphicsUniformSet::uniform4iv(const std::vector<int4>& value) noexcept
{
	_variant.uniform4iv(value);
}

void
EGL2GraphicsUniformSet::uniform4iv(std::size_t num, const std::int32_t* i4v) noexcept
{
	_variant.uniform4iv(num, i4v);
}

void
EGL2GraphicsUniformSet::uniform1uiv(const std::vector<uint1>& value) noexcept
{
	_variant.uniform1uiv(value);
}

void
EGL2GraphicsUniformSet::uniform1uiv(std::size_t num, const std::uint32_t* ui1v) noexcept
{
	_variant.uniform1uiv(num, ui1v);
}

void
EGL2GraphicsUniformSet::uniform2uiv(const std::vector<uint2>& value) noexcept
{
	_variant.uniform2uiv(value);
}

void
EGL2GraphicsUniformSet::uniform2uiv(std::size_t num, const std::uint32_t* ui2v) noexcept
{
	_variant.uniform2uiv(num, ui2v);
}

void
EGL2GraphicsUniformSet::uniform3uiv(const std::vector<uint3>& value) noexcept
{
	_variant.uniform3uiv(value);
}

void
EGL2GraphicsUniformSet::uniform3uiv(std::size_t num, const std::uint32_t* ui3v) noexcept
{
	_variant.uniform3uiv(num, ui3v);
}

void
EGL2GraphicsUniformSet::uniform4uiv(const std::vector<uint4>& value) noexcept
{
	_variant.uniform4uiv(value);
}

void
EGL2GraphicsUniformSet::uniform4uiv(std::size_t num, const std::uint32_t* ui4v) noexcept
{
	_variant.uniform4uiv(num, ui4v);
}

void
EGL2GraphicsUniformSet::uniform1fv(const std::vector<float1>& value) noexcept
{
	_variant.uniform1fv(value);
}

void
EGL2GraphicsUniformSet::uniform1fv(std::size_t num, const float* f1v) noexcept
{
	_variant.uniform1fv(num, f1v);
}

void
EGL2GraphicsUniformSet::uniform2fv(const std::vector<float2>& value) noexcept
{
	_variant.uniform2fv(value);
}

void
EGL2GraphicsUniformSet::uniform2fv(std::size_t num, const float* f2v) noexcept
{
	_variant.uniform2fv(num, f2v);
}

void
EGL2GraphicsUniformSet::uniform3fv(const std::vector<float3>& value) noexcept
{
	_variant.uniform3fv(value);
}

void
EGL2GraphicsUniformSet::uniform3fv(std::size_t num, const float* f3v) noexcept
{
	_variant.uniform3fv(num, f3v);
}

void
EGL2GraphicsUniformSet::uniform4fv(const std::vector<float4>& value) noexcept
{
	_variant.uniform4fv(value);
}

void
EGL2GraphicsUniformSet::uniform4fv(std::size_t num, const float* f4v) noexcept
{
	_variant.uniform4fv(num, f4v);
}

void
EGL2GraphicsUniformSet::uniform2fmatv(const std::vector<float2x2>& value) noexcept
{
	_variant.uniform2fmatv(value);
}

void
EGL2GraphicsUniformSet::uniform2fmatv(std::size_t num, const float* mat2) noexcept
{
	_variant.uniform2fmatv(num, mat2);
}

void
EGL2GraphicsUniformSet::uniform3fmatv(const std::vector<float3x3>& value) noexcept
{
	_variant.uniform3fmatv(value);
}

void
EGL2GraphicsUniformSet::uniform3fmatv(std::size_t num, const float* mat3) noexcept
{
	_variant.uniform3fmatv(num, mat3);
}

void
EGL2GraphicsUniformSet::uniform4fmatv(const std::vector<float4x4>& value) noexcept
{
	_variant.uniform4fmatv(value);
}

void
EGL2GraphicsUniformSet::uniform4fmatv(std::size_t num, const float* mat4) noexcept
{
	_variant.uniform4fmatv(num, mat4);
}

void
EGL2GraphicsUniformSet::uniformTexture(GraphicsTexturePtr texture, GraphicsSamplerPtr sampler) noexcept
{
	_variant.uniformTexture(texture, sampler);
}

void
EGL2GraphicsUniformSet::uniformBuffer(GraphicsDataPtr ubo) noexcept
{
	_variant.uniformBuffer(ubo);
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

const float2x2&
EGL2GraphicsUniformSet::getFloat2x2() const noexcept
{
	return _variant.getFloat2x2();
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

const std::vector<float2x2>&
EGL2GraphicsUniformSet::getFloat2x2Array() const noexcept
{
	return _variant.getFloat2x2Array();
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

const GraphicsTexturePtr&
EGL2GraphicsUniformSet::getTexture() const noexcept
{
	return _variant.getTexture();
}

const GraphicsSamplerPtr&
EGL2GraphicsUniformSet::getTextureSampler() const noexcept
{
	return _variant.getTextureSampler();
}

const GraphicsDataPtr&
EGL2GraphicsUniformSet::getBuffer() const noexcept
{
	return _variant.getBuffer();
}

void
EGL2GraphicsUniformSet::setGraphicsParam(GraphicsParamPtr param) noexcept
{
	assert(param);
	_param = param;
	_variant.setType(param->getType());
}

const GraphicsParamPtr&
EGL2GraphicsUniformSet::getGraphicsParam() const noexcept
{
	return _param;
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
	auto& uniforms = descriptorSetLayoutDesc.getUniformComponents();
	for (auto& uniform : uniforms)
	{
		auto uniformSet = std::make_shared<EGL2GraphicsUniformSet>();
		uniformSet->setGraphicsParam(uniform);
		_activeUniformSets.push_back(uniformSet);
	}

	_descriptorSetDesc = descriptorSetDesc;
	return true;
}

void
EGL2DescriptorSet::close() noexcept
{
	_activeUniformSets.clear();
}

void
EGL2DescriptorSet::apply(GraphicsProgramPtr shaderObject) noexcept
{
	std::uint32_t textureUnit = 0;
	for (auto& it : _activeUniformSets)
	{
		auto type = it->getGraphicsParam()->getType();
		auto location = it->getGraphicsParam()->getBindingPoint();
		switch (type)
		{
		case GraphicsUniformType::GraphicsUniformTypeBool:
			GL_CHECK(glUniform1i(location, it->getBool()));
			break;
		case GraphicsUniformType::GraphicsUniformTypeInt:
			GL_CHECK(glUniform1i(location, it->getInt()));
			break;
		case GraphicsUniformType::GraphicsUniformTypeInt2:
			GL_CHECK(glUniform2iv(location, 1, (GLint*)it->getInt2().ptr()));
			break;
		case GraphicsUniformType::GraphicsUniformTypeInt3:
			GL_CHECK(glUniform3iv(location, 1, (GLint*)it->getInt3().ptr()));
			break;
		case GraphicsUniformType::GraphicsUniformTypeInt4:
			GL_CHECK(glUniform4iv(location, 1, (GLint*)it->getInt4().ptr()));
			break;
		case GraphicsUniformType::GraphicsUniformTypeFloat:
			GL_CHECK(glUniform1f(location, it->getFloat()));
			break;
		case GraphicsUniformType::GraphicsUniformTypeFloat2:
			GL_CHECK(glUniform2fv(location, 1, it->getFloat2().ptr()));
			break;
		case GraphicsUniformType::GraphicsUniformTypeFloat3:
			GL_CHECK(glUniform3fv(location, 1, it->getFloat3().ptr()));
			break;
		case GraphicsUniformType::GraphicsUniformTypeFloat4:
			GL_CHECK(glUniform4fv(location, 1, it->getFloat4().ptr()));
			break;
		case GraphicsUniformType::GraphicsUniformTypeFloat3x3:
			GL_CHECK(glUniformMatrix3fv(location, 1, GL_FALSE, it->getFloat3x3().ptr()));
			break;
		case GraphicsUniformType::GraphicsUniformTypeFloat4x4:
			GL_CHECK(glUniformMatrix4fv(location, 1, GL_FALSE, it->getFloat4x4().ptr()));
			break;
		case GraphicsUniformType::GraphicsUniformTypeIntArray:
			GL_CHECK(glUniform1iv(location, it->getIntArray().size(), it->getIntArray().data()));
			break;
		case GraphicsUniformType::GraphicsUniformTypeInt2Array:
			GL_CHECK(glUniform2iv(location, it->getInt2Array().size(), (GLint*)it->getInt2Array().data()));
			break;
		case GraphicsUniformType::GraphicsUniformTypeInt3Array:
			GL_CHECK(glUniform3iv(location, it->getInt3Array().size(), (GLint*)it->getInt3Array().data()));
			break;
		case GraphicsUniformType::GraphicsUniformTypeInt4Array:
			GL_CHECK(glUniform4iv(location, it->getInt4Array().size(), (GLint*)it->getInt4Array().data()));
			break;
		case GraphicsUniformType::GraphicsUniformTypeFloatArray:
			GL_CHECK(glUniform1fv(location, it->getFloatArray().size(), (GLfloat*)it->getFloatArray().data()));
			break;
		case GraphicsUniformType::GraphicsUniformTypeFloat2Array:
			GL_CHECK(glUniform2fv(location, it->getFloat2Array().size(), (GLfloat*)it->getFloat2Array().data()));
			break;
		case GraphicsUniformType::GraphicsUniformTypeFloat3Array:
			GL_CHECK(glUniform3fv(location, it->getFloat3Array().size(), (GLfloat*)it->getFloat3Array().data()));
			break;
		case GraphicsUniformType::GraphicsUniformTypeFloat4Array:
			GL_CHECK(glUniform4fv(location, it->getFloat4Array().size(), (GLfloat*)it->getFloat4Array().data()));
			break;
		case GraphicsUniformType::GraphicsUniformTypeFloat3x3Array:
			GL_CHECK(glUniformMatrix3fv(location, it->getFloat3x3Array().size(), GL_FALSE, (GLfloat*)it->getFloat3x3Array().data()));
			break;
		case GraphicsUniformType::GraphicsUniformTypeFloat4x4Array:
			GL_CHECK(glUniformMatrix4fv(location, it->getFloat4x4Array().size(), GL_FALSE, (GLfloat*)it->getFloat4x4Array().data()));
			break;
		case GraphicsUniformType::GraphicsUniformTypeSampler:
			assert(false);
			break;
		case GraphicsUniformType::GraphicsUniformTypeSamplerImage:
			{
				auto texture = it->getTexture();
				if (texture)
				{
					GL_CHECK(glUniform1i(location, textureUnit));
					GL_CHECK(glActiveTexture(GL_TEXTURE0 + textureUnit));
					GL_CHECK(glBindTexture(texture->downcast<EGL2Texture>()->getTarget(), texture->downcast<EGL2Texture>()->getInstanceID()));
					textureUnit++;
				}
			}
			break;
		case GraphicsUniformType::GraphicsUniformTypeCombinedImageSampler:
			{
				auto texture = it->getTexture();
				if (texture)
				{
					GL_CHECK(glUniform1i(location, textureUnit));
					GL_CHECK(glActiveTexture(GL_TEXTURE0 + textureUnit));
					GL_CHECK(glBindTexture(texture->downcast<EGL2Texture>()->getTarget(), texture->downcast<EGL2Texture>()->getInstanceID()));
					textureUnit++;
				}
			}
			break;
		case GraphicsUniformType::GraphicsUniformTypeStorageImage:
			{
				auto texture = it->getTexture();
				if (texture)
				{
					GL_CHECK(glUniform1i(location, textureUnit));
					GL_CHECK(glActiveTexture(GL_TEXTURE0 + textureUnit));
					GL_CHECK(glBindTexture(texture->downcast<EGL2Texture>()->getTarget(), texture->downcast<EGL2Texture>()->getInstanceID()));
					textureUnit++;
				}
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