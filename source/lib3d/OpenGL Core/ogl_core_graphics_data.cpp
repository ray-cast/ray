// +----------------------------------------------------------------------
// | Project : ray.
// | All rights reserved.
// +----------------------------------------------------------------------
// | Copyright (c) 2013-2017.
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
#include "ogl_core_graphics_data.h"

_NAME_BEGIN

__ImplementSubInterface(OGLCoreGraphicsData, GraphicsData, "OGLCoreGraphicsData")

OGLCoreGraphicsData::OGLCoreGraphicsData() noexcept
	: _buffer(GL_NONE)
	, _bufferAddr(GL_NONE)
{
}

OGLCoreGraphicsData::~OGLCoreGraphicsData() noexcept
{
	this->close();
}

bool
OGLCoreGraphicsData::setup(const GraphicsDataDesc& desc) noexcept
{
	assert(_buffer == GL_NONE);

	GLenum target;

	auto type = desc.getType();
	if (type == GraphicsDataType::GraphicsDataTypeStorageVertexBuffer)
		target = GL_ARRAY_BUFFER;
	else if (type == GraphicsDataType::GraphicsDataTypeStorageIndexBuffer)
		target = GL_ELEMENT_ARRAY_BUFFER;
	else if (type == GraphicsDataType::GraphicsDataTypeUniformBuffer)
		target = GL_UNIFORM_BUFFER;
	else if (type == GraphicsDataType::GraphicsDataTypeStorageTexelBuffer)
		target = GL_TEXTURE_BUFFER;
	else if (type == GraphicsDataType::GraphicsDataTypeStorageBuffer)
		target = GL_SHADER_STORAGE_BUFFER;
	else if (type == GraphicsDataType::GraphicsDataTypeStorageBuffer)
		target = GL_DRAW_INDIRECT_BUFFER;
	else
	{
		GL_PLATFORM_LOG("Unknown data type.");
		return false;
	}

	glCreateBuffers(1, &_buffer);
	if (_buffer == GL_NONE)
	{
		GL_PLATFORM_LOG("glCreateBuffers() fail.");
		return false;
	}

	auto usage = desc.getUsage();

	GLbitfield flags = GL_MAP_READ_BIT;
	if (usage & GraphicsUsageFlagBits::GraphicsUsageFlagReadBit)
		flags |= GL_MAP_READ_BIT;
	if (usage & GraphicsUsageFlagBits::GraphicsUsageFlagWriteBit)
		flags |= GL_MAP_WRITE_BIT;
	if (usage & GraphicsUsageFlagBits::GraphicsUsageFlagPersistentBit)
		flags |= GL_MAP_PERSISTENT_BIT;
	if (usage & GraphicsUsageFlagBits::GraphicsUsageFlagCoherentBit)
		flags |= GL_MAP_COHERENT_BIT;
	if (usage & GraphicsUsageFlagBits::GraphicsUsageFlagFlushExplicitBit)
		flags |= GL_MAP_FLUSH_EXPLICIT_BIT;
	if (usage & GraphicsUsageFlagBits::GraphicsUsageFlagDynamicStorageBit)
		flags |= GL_DYNAMIC_STORAGE_BIT;
	if (usage & GraphicsUsageFlagBits::GraphicsUsageFlagClientStorageBit)
		flags |= GL_CLIENT_STORAGE_BIT;

	glNamedBufferStorage(_buffer, desc.getStreamSize(), desc.getStream(), flags);

	if (GLEW_NV_vertex_buffer_unified_memory && type == GraphicsDataType::GraphicsDataTypeStorageVertexBuffer)
	{
		glGetNamedBufferParameterui64vNV(_buffer, GL_BUFFER_GPU_ADDRESS_NV, &_bufferAddr);
		glMakeNamedBufferResidentNV(_buffer, GL_READ_ONLY);
	}

	_desc = desc;
	return true;
}

void
OGLCoreGraphicsData::close() noexcept
{
	if (_buffer)
	{
		glDeleteBuffers(1, &_buffer);
		_buffer = 0;
	}
}

std::ptrdiff_t
OGLCoreGraphicsData::flush() noexcept
{
	return this->flush(0, _desc.getStreamSize());
}

std::ptrdiff_t
OGLCoreGraphicsData::flush(GLintptr offset, GLsizeiptr cnt) noexcept
{
	glFlushMappedNamedBufferRange(_buffer, offset, cnt);
	return cnt;
}

bool
OGLCoreGraphicsData::map(std::ptrdiff_t offset, std::ptrdiff_t count, void** data) noexcept
{
	assert(data);
	*data = glMapNamedBufferRange(_buffer, offset, count, GL_MAP_READ_BIT | GL_MAP_WRITE_BIT);
	return *data ? true : false;
}

void
OGLCoreGraphicsData::unmap() noexcept
{
	glUnmapNamedBuffer(_buffer);
}

GLuint
OGLCoreGraphicsData::getInstanceID() const noexcept
{
	return _buffer;
}

GLuint64
OGLCoreGraphicsData::getInstanceAddr() const noexcept
{
	return _bufferAddr;
}

const GraphicsDataDesc&
OGLCoreGraphicsData::getGraphicsDataDesc() const noexcept
{
	return _desc;
}

void
OGLCoreGraphicsData::setDevice(GraphicsDevicePtr device) noexcept
{
	_device = device;
}

GraphicsDevicePtr
OGLCoreGraphicsData::getDevice() noexcept
{
	return _device.lock();
}

_NAME_END