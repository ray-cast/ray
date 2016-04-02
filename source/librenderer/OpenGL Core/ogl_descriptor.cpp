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
#include "ogl_descriptor.h"
#include "ogl_texture.h"
#include "ogl_shader.h"
#include "ogl_sampler.h"

_NAME_BEGIN

__ImplementSubClass(OGLDescriptorSet, GraphicsDescriptorSet, "OGLDescriptorSet")
__ImplementSubClass(OGLDescriptorSetLayout, GraphicsDescriptorSetLayout, "OGLDescriptorSetLayout")

OGLDescriptorSetLayout::OGLDescriptorSetLayout() noexcept
{
}

OGLDescriptorSetLayout::~OGLDescriptorSetLayout() noexcept
{
	this->close();
}

bool
OGLDescriptorSetLayout::setup(const GraphicsDescriptorSetLayoutDesc& descriptorSetDesc) noexcept
{
	_descriptorSetDesc = descriptorSetDesc;
	return true;
}

void
OGLDescriptorSetLayout::close() noexcept
{
}

const GraphicsDescriptorSetLayoutDesc&
OGLDescriptorSetLayout::getGraphicsDescriptorSetLayoutDesc() const noexcept
{
	return _descriptorSetDesc;
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
	_descriptorSetDesc = descriptorSetDesc;
	return true;
}

void
OGLDescriptorSet::close() noexcept
{
}

void
OGLDescriptorSet::bindProgram(GraphicsProgramPtr shaderObject) noexcept
{
	std::uint32_t textureUnit = 0;
	auto program = shaderObject->downcast<OGLShaderObject>()->getInstanceID();
	auto& descriptorSets = _descriptorSetDesc.getGraphicsDescriptorSetLayout()->getGraphicsDescriptorSetLayoutDesc().getUniformComponents();
	for (auto& it : descriptorSets)
	{
		auto type = it->getType();
		switch (type)
		{
		case GraphicsUniformType::GraphicsUniformTypeBool:
			glProgramUniform1i(program, it->getBindingPoint(), it->getBool());
			break;
		case GraphicsUniformType::GraphicsUniformTypeInt:
			glProgramUniform1i(program, it->getBindingPoint(), it->getInt());
			break;
		case GraphicsUniformType::GraphicsUniformTypeInt2:
			glProgramUniform2iv(program, it->getBindingPoint(), 1, (GLint*)it->getInt2().ptr());
			break;
		case GraphicsUniformType::GraphicsUniformTypeInt3:
			glProgramUniform3iv(program, it->getBindingPoint(), 1, (GLint*)it->getInt3().ptr());
			break;
		case GraphicsUniformType::GraphicsUniformTypeInt4:
			glProgramUniform4iv(program, it->getBindingPoint(), 1, (GLint*)it->getInt4().ptr());
			break;
		case GraphicsUniformType::GraphicsUniformTypeUInt:
			glProgramUniform1ui(program, it->getBindingPoint(), it->getUInt());
			break;
		case GraphicsUniformType::GraphicsUniformTypeUInt2:
			glProgramUniform2uiv(program, it->getBindingPoint(), 1, (GLuint*)it->getUInt2().ptr());
			break;
		case GraphicsUniformType::GraphicsUniformTypeUInt3:
			glProgramUniform3uiv(program, it->getBindingPoint(), 1, (GLuint*)it->getUInt3().ptr());
			break;
		case GraphicsUniformType::GraphicsUniformTypeUInt4:
			glProgramUniform4uiv(program, it->getBindingPoint(), 1, (GLuint*)it->getUInt4().ptr());
			break;
		case GraphicsUniformType::GraphicsUniformTypeFloat:
			glProgramUniform1f(program, it->getBindingPoint(), it->getFloat());
			break;
		case GraphicsUniformType::GraphicsUniformTypeFloat2:
			glProgramUniform2fv(program, it->getBindingPoint(), 1, it->getFloat2().ptr());
			break;
		case GraphicsUniformType::GraphicsUniformTypeFloat3:
			glProgramUniform3fv(program, it->getBindingPoint(), 1, it->getFloat3().ptr());
			break;
		case GraphicsUniformType::GraphicsUniformTypeFloat4:
			glProgramUniform4fv(program, it->getBindingPoint(), 1, it->getFloat4().ptr());
			break;
		case GraphicsUniformType::GraphicsUniformTypeFloat3x3:
			glProgramUniformMatrix3fv(program, it->getBindingPoint(), 1, GL_FALSE, it->getFloat3x3().ptr());
			break;
		case GraphicsUniformType::GraphicsUniformTypeFloat4x4:
			glProgramUniformMatrix4fv(program, it->getBindingPoint(), 1, GL_FALSE, it->getFloat4x4().ptr());
			break;
		case GraphicsUniformType::GraphicsUniformTypeIntArray:
			glProgramUniform1iv(program, it->getBindingPoint(), it->getIntArray().size(), it->getIntArray().data());
			break;
		case GraphicsUniformType::GraphicsUniformTypeInt2Array:
			glProgramUniform2iv(program, it->getBindingPoint(), it->getInt2Array().size(), (GLint*)it->getInt2Array().data());
			break;
		case GraphicsUniformType::GraphicsUniformTypeInt3Array:
			glProgramUniform3iv(program, it->getBindingPoint(), it->getInt3Array().size(), (GLint*)it->getInt3Array().data());
			break;
		case GraphicsUniformType::GraphicsUniformTypeInt4Array:
			glProgramUniform4iv(program, it->getBindingPoint(), it->getInt4Array().size(), (GLint*)it->getInt4Array().data());
			break;
		case GraphicsUniformType::GraphicsUniformTypeUIntArray:
			glProgramUniform1uiv(program, it->getBindingPoint(), it->getUIntArray().size(), it->getUIntArray().data());
			break;
		case GraphicsUniformType::GraphicsUniformTypeUInt2Array:
			glProgramUniform2uiv(program, it->getBindingPoint(), it->getUInt2Array().size(), (GLuint*)it->getUInt2Array().data());
			break;
		case GraphicsUniformType::GraphicsUniformTypeUInt3Array:
			glProgramUniform3uiv(program, it->getBindingPoint(), it->getUInt3Array().size(), (GLuint*)it->getUInt3Array().data());
			break;
		case GraphicsUniformType::GraphicsUniformTypeUInt4Array:
			glProgramUniform4uiv(program, it->getBindingPoint(), it->getUInt4Array().size(), (GLuint*)it->getUInt4Array().data());
			break;
		case GraphicsUniformType::GraphicsUniformTypeFloatArray:
			glProgramUniform1fv(program, it->getBindingPoint(), it->getFloatArray().size(), (GLfloat*)it->getFloatArray().data());
			break;
		case GraphicsUniformType::GraphicsUniformTypeFloat2Array:
			glProgramUniform2fv(program, it->getBindingPoint(), it->getFloat2Array().size(), (GLfloat*)it->getFloat2Array().data());
			break;
		case GraphicsUniformType::GraphicsUniformTypeFloat3Array:
			glProgramUniform3fv(program, it->getBindingPoint(), it->getFloat3Array().size(), (GLfloat*)it->getFloat3Array().data());
			break;
		case GraphicsUniformType::GraphicsUniformTypeFloat4Array:
			glProgramUniform4fv(program, it->getBindingPoint(), it->getFloat4Array().size(), (GLfloat*)it->getFloat4Array().data());
			break;
		case GraphicsUniformType::GraphicsUniformTypeFloat3x3Array:
			glProgramUniformMatrix3fv(program, it->getBindingPoint(), it->getFloat3x3Array().size(), GL_FALSE, (GLfloat*)it->getFloat3x3Array().data());
			break;
		case GraphicsUniformType::GraphicsUniformTypeFloat4x4Array:
			glProgramUniformMatrix4fv(program, it->getBindingPoint(), it->getFloat3x3Array().size(), GL_FALSE, (GLfloat*)it->getFloat4x4Array().data());
			break;
		case GraphicsUniformType::GraphicsUniformTypeSampler:
			glBindSampler(it->getBindingPoint(), it->getTextureSampler()->downcast<OGLSampler>()->getInstanceID());
			break;
		case GraphicsUniformType::GraphicsUniformTypeSamplerImage:
			if (it->getTexture())
			{
				glProgramUniform1i(program, it->getBindingPoint(), textureUnit);
				glBindTextureUnit(textureUnit, it->getTexture()->downcast<OGLTexture>()->getInstanceID());
				textureUnit++;
			}				
			break;
		case GraphicsUniformType::GraphicsUniformTypeCombinedImageSampler:
			if (it->getTexture())
			{
				glProgramUniform1i(program, it->getBindingPoint(), textureUnit);
				glBindTextureUnit(textureUnit, it->getTexture()->downcast<OGLTexture>()->getInstanceID());
				textureUnit++;
			}				
			break;
		case GraphicsUniformType::GraphicsUniformTypeStorageImage:
			if (it->getTexture())
			{
				glProgramUniform1i(program, it->getBindingPoint(), textureUnit);
				glBindTextureUnit(textureUnit, it->getTexture()->downcast<OGLTexture>()->getInstanceID());
				textureUnit++;
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