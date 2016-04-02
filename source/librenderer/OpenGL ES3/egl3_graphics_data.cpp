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
#include "egl3_graphics_data.h"

_NAME_BEGIN

__ImplementSubClass(EGL3GraphicsData, GraphicsData, "EGL3GraphicsData")

EGL3GraphicsData::EGL3GraphicsData() noexcept
	: _buffer(GL_NONE)
	, _isMapping(GL_FALSE)
	, _data(nullptr)
{
}

EGL3GraphicsData::~EGL3GraphicsData() noexcept
{
	this->close();
}

bool
EGL3GraphicsData::setup(const GraphicsDataDesc& desc) noexcept
{
	assert(!_buffer);

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
		_target = GL_TEXTURE_BUFFER_EXT;
	else if (type == GraphicsDataType::GraphicsDataTypeStorageBuffer)
		_target = GL_SHADER_STORAGE_BUFFER;
	else if (type == GraphicsDataType::GraphicsDataTypeStorageBuffer)
		_target = GL_DRAW_INDIRECT_BUFFER;
	else
		return false;

	GLenum flags = GL_STATIC_DRAW;

	auto usage = desc.getUsage();
	if (usage & GraphicsUsageFlags::GraphicsUsageFlagsReadBit)
		flags = GL_STATIC_DRAW;
	if (usage & GraphicsUsageFlags::GraphicsUsageFlagsWriteBit)
		flags = GL_DYNAMIC_READ;

	GL_CHECK(glGenBuffers(1, &_buffer));
	GL_CHECK(glBindBuffer(_target, _buffer));
	GL_CHECK(glBufferData(_target, _dataSize, desc.getStream(), flags));

	return true;
}

bool
EGL3GraphicsData::is_open() const noexcept
{
	return _buffer == GL_NONE ? false : true;
}

void
EGL3GraphicsData::close() noexcept
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
EGL3GraphicsData::size() const noexcept
{
	return _dataSize;
}

void
EGL3GraphicsData::resize(const char* data, GLsizeiptr datasize) noexcept
{
	if (_usage & GraphicsUsageFlags::GraphicsUsageFlagsImmutableStorage)
		return;

	GLenum flags = GL_STATIC_DRAW;
	if (_usage & GraphicsUsageFlags::GraphicsUsageFlagsReadBit)
		flags = GL_STATIC_DRAW;
	if (_usage & GraphicsUsageFlags::GraphicsUsageFlagsWriteBit)
		flags = GL_DYNAMIC_READ;

	GL_CHECK(glBindBuffer(_target, _buffer));
	GL_CHECK(glBufferData(_target, datasize, data, flags));
	_dataSize = datasize;
}

int
EGL3GraphicsData::flush() noexcept
{
	return this->flush(0, _dataSize);
}

int
EGL3GraphicsData::flush(GLintptr offset, GLsizeiptr cnt) noexcept
{
	GL_CHECK(glBindBuffer(_target, _buffer));
	GL_CHECK(glFlushMappedBufferRange(_buffer, offset, cnt));
	return cnt;
}

GLsizeiptr
EGL3GraphicsData::read(char* str, GLsizeiptr cnt) noexcept
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
EGL3GraphicsData::write(const char* str, GLsizeiptr cnt) noexcept
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

streamoff
EGL3GraphicsData::seekg(GLintptr pos, ios_base::seekdir dir) noexcept
{
	assert(dir == ios_base::beg || dir == ios_base::cur || dir == ios_base::end);

	if (dir == ios_base::beg)
	{
		_dataOffset = pos;
		return pos;
	}
	else if (dir == ios_base::cur)
	{
		_dataOffset = _dataOffset + pos;
		if (_dataOffset > _dataSize)
		{
			pos = _dataSize - _dataOffset;
			_dataOffset = _dataSize;
		}
		return pos;
	}
	else if (dir == ios_base::end)
	{
		GLsizei size = _dataSize;
		pos = size + pos;
		if (pos > size)
			_dataOffset = size;
		else
			_dataOffset = pos;
		return pos;
	}

	return 0;
}

streamoff
EGL3GraphicsData::tellg() noexcept
{
	return _dataOffset;
}

void*
EGL3GraphicsData::map(std::uint32_t access) noexcept
{
	return this->map(0, _dataSize, access);
}

void*
EGL3GraphicsData::map(GLintptr offset, GLsizeiptr cnt, std::uint32_t access) noexcept
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
		if (_usage & GraphicsUsageFlags::GraphicsUsageFlagsFlushExplicitBit)
			flags |= GL_MAP_FLUSH_EXPLICIT_BIT;

		GL_CHECK(glBindBuffer(_target, _buffer));
		_data = glMapBufferRange(_target, offset, cnt, flags);
		GL_CHECK(_data);
	}

	if (_data)
	{
		_isMapping = GL_TRUE;
		return _data;
	}

	return _data;
}

void
EGL3GraphicsData::unmap() noexcept
{
	assert(_isMapping);

	if (_usage & GraphicsUsageFlags::GraphicsUsageFlagsPersistentBit)
	{
		_isMapping = GL_FALSE;
		return;
	}

	GL_CHECK(glBindBuffer(_target, _buffer));
	GL_CHECK(glUnmapBuffer(_target));
	_data = nullptr;
	_isMapping = GL_FALSE;
}

bool
EGL3GraphicsData::isMapping() const noexcept
{
	return _isMapping ? GL_TRUE : GL_FALSE;
}

GLuint
EGL3GraphicsData::getInstanceID() noexcept
{
	return _buffer;
}

const GraphicsDataDesc&
EGL3GraphicsData::getGraphicsDataDesc() const noexcept
{
	return _desc;
}

void
EGL3GraphicsData::bind() noexcept
{
	GL_CHECK(glBindBuffer(_target, _buffer));
}

void
EGL3GraphicsData::setDevice(GraphicsDevicePtr device) noexcept
{
	_device = device;
}

GraphicsDevicePtr
EGL3GraphicsData::getDevice() noexcept
{
	return _device.lock();
}

_NAME_END