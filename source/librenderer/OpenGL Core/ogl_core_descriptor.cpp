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
#include "ogl_core_descriptor.h"
#include "ogl_core_texture.h"
#include "ogl_core_graphics_data.h"
#include "ogl_descriptor_set.h"
#include "ogl_shader.h"
#include "ogl_sampler.h"

_NAME_BEGIN

__ImplementSubClass(OGLCoreDescriptorSet, GraphicsDescriptorSet, "OGLCoreDescriptorSet")

OGLCoreDescriptorSet::OGLCoreDescriptorSet() noexcept
{
}

OGLCoreDescriptorSet::~OGLCoreDescriptorSet() noexcept
{
	this->close();
}

bool
OGLCoreDescriptorSet::setup(const GraphicsDescriptorSetDesc& descriptorSetDesc) noexcept
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
OGLCoreDescriptorSet::close() noexcept
{
	_activeUniformSets.clear();
}

void
OGLCoreDescriptorSet::apply(const OGLProgram& shaderObject) noexcept
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
			auto texture = it->getTexture();
			if (texture)
			{
				glBindTextureUnit(location, texture->downcast<OGLCoreTexture>()->getInstanceID());
			}
		}
		break;
		case GraphicsUniformType::GraphicsUniformTypeCombinedImageSampler:
		{
			auto& texture = it->getTexture();
			if (texture)
			{
				glBindTextureUnit(location, texture->downcast<OGLCoreTexture>()->getInstanceID());
			}
		}
		break;
		case GraphicsUniformType::GraphicsUniformTypeStorageImage:
		{
			auto& texture = it->getTexture();
			if (texture)
			{
				glBindTextureUnit(location, texture->downcast<OGLCoreTexture>()->getInstanceID());
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
				auto ubo = buffer->downcast<OGLCoreGraphicsData>();
				glBindBufferBase(GL_UNIFORM_BUFFER, location, ubo->getInstanceID());
			}
		}
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
OGLCoreDescriptorSet::getGraphicsUniformSets() const noexcept
{
	return _activeUniformSets;
}

const GraphicsDescriptorSetDesc&
OGLCoreDescriptorSet::getGraphicsDescriptorSetDesc() const noexcept
{
	return _descriptorSetDesc;
}

void
OGLCoreDescriptorSet::setDevice(GraphicsDevicePtr device) noexcept
{
	_device = device;
}

GraphicsDevicePtr
OGLCoreDescriptorSet::getDevice() noexcept
{
	return _device.lock();
}

_NAME_END