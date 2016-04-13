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
#include "ogl_graphics_data.h"

_NAME_BEGIN

__ImplementSubInterface(OGLGraphicsData, GraphicsData, "OGLGraphicsData")

OGLGraphicsData::OGLGraphicsData() noexcept
	: _buffer(GL_NONE)
	, _isMapping(GL_FALSE)
	, _data(nullptr)
{
}

OGLGraphicsData::~OGLGraphicsData() noexcept
{
	this->close();
}

bool
OGLGraphicsData::setup(const GraphicsDataDesc& desc) noexcept
{
	assert(!_buffer);
	assert(desc.getStride() > 0);

	_data = nullptr;
	_dataOffset = 0;
	_dataSize = desc.getStreamSize();
	_usage = desc.getUsage();
	_desc = desc;
	_isMapping = false;

	auto type = desc.getType();
	if (type == GraphicsDataType::GraphicsDataTypeStorageVertexBuffer)
		_target = GL_ARRAY_BUFFER;
	else if (type == GraphicsDataType::GraphicsDataTypeStorageIndexBuffer)
		_target = GL_ELEMENT_ARRAY_BUFFER;
	else if (type == GraphicsDataType::GraphicsDataTypeUniformBuffer)
		_target = GL_UNIFORM_BUFFER;
	else if (type == GraphicsDataType::GraphicsDataTypeStorageTexelBuffer)
		_target = GL_TEXTURE_BUFFER;
	else if (type == GraphicsDataType::GraphicsDataTypeStorageBuffer)
		_target = GL_SHADER_STORAGE_BUFFER;
	else if (type == GraphicsDataType::GraphicsDataTypeStorageBuffer)
		_target = GL_DRAW_INDIRECT_BUFFER;
	else
		return false;

	glCreateBuffers(1, &_buffer);
	if (_buffer == GL_NONE)
	{
		GL_PLATFORM_LOG("glCreateBuffers() fail");
		return false;
	}

	auto usage = desc.getUsage();
	if (usage & GraphicsUsageFlags::GraphicsUsageFlagsImmutableStorage)
	{
		GLbitfield flags = GL_MAP_READ_BIT;
		if (usage & GraphicsUsageFlags::GraphicsUsageFlagsReadBit)
			flags |= GL_MAP_READ_BIT;
		if (usage & GraphicsUsageFlags::GraphicsUsageFlagsWriteBit)
			flags |= GL_MAP_WRITE_BIT;
		if (usage & GraphicsUsageFlags::GraphicsUsageFlagsPersistentBit)
			flags |= GL_MAP_PERSISTENT_BIT;
		if (usage & GraphicsUsageFlags::GraphicsUsageFlagsCoherentBit)
			flags |= GL_MAP_COHERENT_BIT;
		if (usage & GraphicsUsageFlags::GraphicsUsageFlagsFlushExplicitBit)
			flags |= GL_MAP_FLUSH_EXPLICIT_BIT;
		if (usage & GraphicsUsageFlags::GraphicsUsageFlagsDynamicStorageBit)
			flags |= GL_DYNAMIC_STORAGE_BIT;
		if (usage & GraphicsUsageFlags::GraphicsUsageFlagsClientStorageBit)
			flags |= GL_CLIENT_STORAGE_BIT;

		glNamedBufferStorage(_buffer, _dataSize, desc.getStream(), flags);
	}
	else
	{
		GLenum flags = GL_STATIC_DRAW;
		if (usage & GraphicsUsageFlags::GraphicsUsageFlagsReadBit)
			flags = GL_STATIC_DRAW;
		if (usage & GraphicsUsageFlags::GraphicsUsageFlagsWriteBit)
			flags = GL_DYNAMIC_READ;

		glNamedBufferData(_buffer, _dataSize, desc.getStream(), flags);
	}

	return true;
}

bool
OGLGraphicsData::is_open() const noexcept
{
	return _buffer == GL_NONE ? false : true;
}

void
OGLGraphicsData::close() noexcept
{
	assert(!_isMapping);

	if (_isMapping)
	{
		this->unmap();
	}

	if (_buffer)
	{
		glDeleteBuffers(1, &_buffer);
		_buffer = 0;
	}
}

GLsizeiptr
OGLGraphicsData::size() const noexcept
{
	return _dataSize;
}

void
OGLGraphicsData::resize(const char* data, GLsizeiptr datasize) noexcept
{
	if (_usage & GraphicsUsageFlags::GraphicsUsageFlagsImmutableStorage)
		return;

	GLenum flags = GL_STATIC_DRAW;
	if (_usage & GraphicsUsageFlags::GraphicsUsageFlagsReadBit)
		flags = GL_STATIC_DRAW;
	if (_usage & GraphicsUsageFlags::GraphicsUsageFlagsWriteBit)
		flags = GL_DYNAMIC_READ;

	glNamedBufferData(_buffer, datasize, data, flags);
	_dataSize = datasize;
}

int
OGLGraphicsData::flush() noexcept
{
	return this->flush(0, _dataSize);
}

int
OGLGraphicsData::flush(GLintptr offset, GLsizeiptr cnt) noexcept
{
	glFlushMappedNamedBufferRange(_buffer, offset, cnt);
	return cnt;
}

GLsizeiptr
OGLGraphicsData::read(char* str, GLsizeiptr cnt) noexcept
{
	if (_dataSize < _dataOffset + cnt)
	{
		cnt = _dataSize - _dataOffset;
		if (cnt == 0)
			return 0;
	}

	void* data = this->map(_dataOffset, cnt, GraphicsAccessFlagsBits::GraphicsAccessFlagsMapReadBit);
	if (data)
	{
		std::memcpy(str, data, cnt);
		_dataOffset += cnt;
	}

	this->unmap();
	return cnt;
}

GLsizeiptr
OGLGraphicsData::write(const char* str, GLsizeiptr cnt) noexcept
{
	if (_dataSize >= _dataOffset + cnt)
	{
		cnt = _dataSize - _dataOffset;
		if (cnt == 0)
			return 0;
	}

	void* data = this->map(_dataOffset, cnt, GraphicsAccessFlagsBits::GraphicsAccessFlagsMapWriteBit);
	if (data)
	{
		std::memcpy(data, str, cnt);
		_dataOffset += cnt;
		this->unmap();
		return cnt;
	}

	this->unmap();
	return 0;
}

void*
OGLGraphicsData::map(std::uint32_t access) noexcept
{
	return this->map(0, _dataSize, access);
}

void*
OGLGraphicsData::map(GLintptr offset, GLsizeiptr cnt, std::uint32_t access) noexcept
{
	if (!_data)
	{
		GLbitfield flags = GL_MAP_READ_BIT;
		if (access & GraphicsAccessFlagsBits::GraphicsAccessFlagsMapReadBit)
			flags |= GL_MAP_READ_BIT;
		if (access & GraphicsAccessFlagsBits::GraphicsAccessFlagsMapWriteBit)
			flags |= GL_MAP_WRITE_BIT;
		if (access & GraphicsAccessFlagsBits::GraphicsAccessFlagsUnsynchronizedBit)
			flags |= GL_MAP_UNSYNCHRONIZED_BIT;
		if (_usage & GraphicsUsageFlags::GraphicsUsageFlagsPersistentBit)
			flags |= GL_MAP_PERSISTENT_BIT;
		if (_usage & GraphicsUsageFlags::GraphicsUsageFlagsCoherentBit)
			flags |= GL_MAP_COHERENT_BIT;
		if (_usage & GraphicsUsageFlags::GraphicsUsageFlagsFlushExplicitBit)
			flags |= GL_MAP_FLUSH_EXPLICIT_BIT;

		_data = glMapNamedBufferRange(_buffer, offset, cnt, flags);
	}

	if (_data)
	{
		_isMapping = GL_TRUE;
		return _data;
	}

	return _data;
}

void
OGLGraphicsData::unmap() noexcept
{
	assert(_isMapping);

	if (_usage & GraphicsUsageFlags::GraphicsUsageFlagsPersistentBit)
	{
		_isMapping = GL_FALSE;
		return;
	}

	glUnmapNamedBuffer(_buffer);
	_data = nullptr;
	_isMapping = GL_FALSE;
}

bool
OGLGraphicsData::isMapping() const noexcept
{
	return _isMapping ? GL_TRUE : GL_FALSE;
}

GLuint
OGLGraphicsData::getInstanceID() const noexcept
{
	return _buffer;
}

void
OGLGraphicsData::bind() noexcept
{
	glBindBuffer(_target, _buffer);
}

const GraphicsDataDesc&
OGLGraphicsData::getGraphicsDataDesc() const noexcept
{
	return _desc;
}

void 
OGLGraphicsData::setDevice(GraphicsDevicePtr device) noexcept
{
	_device = device;
}

GraphicsDevicePtr
OGLGraphicsData::getDevice() noexcept
{
	return _device.lock();
}

_NAME_END