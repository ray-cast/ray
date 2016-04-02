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

__ImplementSubClass(EGL2DescriptorSet, GraphicsDescriptorSet, "EGL2DescriptorSet")
__ImplementSubClass(EGL2DescriptorSetLayout, GraphicsDescriptorSetLayout, "EGL2DescriptorSetLayout")

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
	auto& descriptorSets = _descriptorSetDesc.getGraphicsDescriptorSetLayout()->getGraphicsDescriptorSetLayoutDesc().getUniformComponents();
	for (auto& it : descriptorSets)
	{
		auto type = it->getType();
		switch (type)
		{
		case GraphicsUniformType::GraphicsUniformTypeBool:
			GL_CHECK(glUniform1i(it->getBindingPoint(), it->getBool()));
			break;
		case GraphicsUniformType::GraphicsUniformTypeInt:
			GL_CHECK(glUniform1i(it->getBindingPoint(), it->getInt()));
			break;
		case GraphicsUniformType::GraphicsUniformTypeInt2:
			GL_CHECK(glUniform2iv(it->getBindingPoint(), 1, (GLint*)it->getInt2().ptr()));
			break;
		case GraphicsUniformType::GraphicsUniformTypeInt3:
			GL_CHECK(glUniform3iv(it->getBindingPoint(), 1, (GLint*)it->getInt3().ptr()));
			break;
		case GraphicsUniformType::GraphicsUniformTypeInt4:
			GL_CHECK(glUniform4iv(it->getBindingPoint(), 1, (GLint*)it->getInt4().ptr()));
			break;
		case GraphicsUniformType::GraphicsUniformTypeFloat:
			GL_CHECK(glUniform1f(it->getBindingPoint(), it->getFloat()));
			break;
		case GraphicsUniformType::GraphicsUniformTypeFloat2:
			GL_CHECK(glUniform2fv(it->getBindingPoint(), 1, it->getFloat2().ptr()));
			break;
		case GraphicsUniformType::GraphicsUniformTypeFloat3:
			GL_CHECK(glUniform3fv(it->getBindingPoint(), 1, it->getFloat3().ptr()));
			break;
		case GraphicsUniformType::GraphicsUniformTypeFloat4:
			GL_CHECK(glUniform4fv(it->getBindingPoint(), 1, it->getFloat4().ptr()));
			break;
		case GraphicsUniformType::GraphicsUniformTypeFloat3x3:
			GL_CHECK(glUniformMatrix3fv(it->getBindingPoint(), 1, GL_FALSE, it->getFloat3x3().ptr()));
			break;
		case GraphicsUniformType::GraphicsUniformTypeFloat4x4:
			GL_CHECK(glUniformMatrix4fv(it->getBindingPoint(), 1, GL_FALSE, it->getFloat4x4().ptr()));
			break;
		case GraphicsUniformType::GraphicsUniformTypeIntArray:
			GL_CHECK(glUniform1iv(it->getBindingPoint(), it->getIntArray().size(), it->getIntArray().data()));
			break;
		case GraphicsUniformType::GraphicsUniformTypeInt2Array:
			GL_CHECK(glUniform2iv(it->getBindingPoint(), it->getInt2Array().size(), (GLint*)it->getInt2Array().data()));
			break;
		case GraphicsUniformType::GraphicsUniformTypeInt3Array:
			GL_CHECK(glUniform3iv(it->getBindingPoint(), it->getInt3Array().size(), (GLint*)it->getInt3Array().data()));
			break;
		case GraphicsUniformType::GraphicsUniformTypeInt4Array:
			GL_CHECK(glUniform4iv(it->getBindingPoint(), it->getInt4Array().size(), (GLint*)it->getInt4Array().data()));
			break;
		case GraphicsUniformType::GraphicsUniformTypeFloatArray:
			GL_CHECK(glUniform1fv(it->getBindingPoint(), it->getFloatArray().size(), (GLfloat*)it->getFloatArray().data()));
			break;
		case GraphicsUniformType::GraphicsUniformTypeFloat2Array:
			GL_CHECK(glUniform2fv(it->getBindingPoint(), it->getFloat2Array().size(), (GLfloat*)it->getFloat2Array().data()));
			break;
		case GraphicsUniformType::GraphicsUniformTypeFloat3Array:
			GL_CHECK(glUniform3fv(it->getBindingPoint(), it->getFloat3Array().size(), (GLfloat*)it->getFloat3Array().data()));
			break;
		case GraphicsUniformType::GraphicsUniformTypeFloat4Array:
			GL_CHECK(glUniform4fv(it->getBindingPoint(), it->getFloat4Array().size(), (GLfloat*)it->getFloat4Array().data()));
			break;
		case GraphicsUniformType::GraphicsUniformTypeFloat3x3Array:
			GL_CHECK(glUniformMatrix3fv(it->getBindingPoint(), it->getFloat3x3Array().size(), GL_FALSE, (GLfloat*)it->getFloat3x3Array().data()));
			break;
		case GraphicsUniformType::GraphicsUniformTypeFloat4x4Array:
			GL_CHECK(glUniformMatrix4fv(it->getBindingPoint(), it->getFloat3x3Array().size(), GL_FALSE, (GLfloat*)it->getFloat4x4Array().data()));
			break;
		case GraphicsUniformType::GraphicsUniformTypeSampler:
			assert(false);
			break;
		case GraphicsUniformType::GraphicsUniformTypeSamplerImage:
			if (it->getTexture())
			{
				GL_CHECK(glUniform1i(it->getBindingPoint(), textureUnit));
				GL_CHECK(glActiveTexture(GL_TEXTURE0 + textureUnit);)
				GL_CHECK(glBindTexture(it->getTexture()->downcast<EGL2Texture>()->getTarget(), it->getTexture()->downcast<EGL2Texture>()->getInstanceID()));
				textureUnit++;
			}
			break;
		case GraphicsUniformType::GraphicsUniformTypeCombinedImageSampler:
			if (it->getTexture())
			{
				GL_CHECK(glUniform1i(it->getBindingPoint(), textureUnit));
				GL_CHECK(glActiveTexture(GL_TEXTURE0 + textureUnit);)
				GL_CHECK(glBindTexture(it->getTexture()->downcast<EGL2Texture>()->getTarget(), it->getTexture()->downcast<EGL2Texture>()->getInstanceID()));
				textureUnit++;
			}
			break;
		case GraphicsUniformType::GraphicsUniformTypeStorageImage:
			if (it->getTexture())
			{
				GL_CHECK(glUniform1i(it->getBindingPoint(), textureUnit));
				GL_CHECK(glActiveTexture(GL_TEXTURE0 + textureUnit);)
				GL_CHECK(glBindTexture(it->getTexture()->downcast<EGL2Texture>()->getTarget(), it->getTexture()->downcast<EGL2Texture>()->getInstanceID()));
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