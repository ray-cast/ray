// +----------------------------------------------------------------------
// | Project : ray.
// | All rights reserved.
// +----------------------------------------------------------------------
// | Copyright (c) 2013-2015.
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
#include "ogl_graphics_buf.h"

_NAME_BEGIN

OGLGraphicsBuf::OGLGraphicsBuf() noexcept
	: _buffer(GL_NONE)
	, _isMapping(GL_FALSE)
	, _data(nullptr)
{
}

OGLGraphicsBuf::~OGLGraphicsBuf() noexcept
{
	this->close();
}

bool
OGLGraphicsBuf::open(const GraphicsDataDesc& desc) noexcept
{
	assert(!_buffer);

	_data = nullptr;
	_dataOffset = 0;
	_dataSize = desc.getStreamSize();
	_usage = desc.getUsage();
	_desc = desc;
	_isMapping = false;

	auto type = desc.getType();
	if (type == GraphicsStream::VBO)
		_target = GL_ARRAY_BUFFER;
	else if (type == GraphicsStream::IBO)
		_target = GL_ELEMENT_ARRAY_BUFFER;
	else if (type == GraphicsStream::UBO)
		_target = GL_UNIFORM_BUFFER;
	else if (type == GraphicsStream::TBO)
		_target = GL_TEXTURE_BUFFER;
	else if (type == GraphicsStream::SSBO)
		_target = GL_SHADER_STORAGE_BUFFER;
	else if (type == GraphicsStream::DIBO)
		_target = GL_DRAW_INDIRECT_BUFFER;
	else
		return false;

	glGenBuffers(1, &_buffer);
	glBindBuffer(_target, _buffer);

	auto usage = desc.getUsage();
	if (usage & UsageFlags::IMMUTABLE_STORAGE)
	{
		GLbitfield flags = GL_MAP_READ_BIT;
		if (usage & UsageFlags::MAP_READ_BIT)
			flags |= GL_MAP_READ_BIT;
		if (usage & UsageFlags::MAP_WRITE_BIT)
			flags |= GL_MAP_WRITE_BIT;
		if (usage & UsageFlags::MAP_PERSISTENT_BIT)
			flags |= GL_MAP_PERSISTENT_BIT;
		if (usage & UsageFlags::MAP_COHERENT_BIT)
			flags |= GL_MAP_COHERENT_BIT;
		if (usage & UsageFlags::MAP_FLUSH_EXPLICIT_BIT)
			flags |= GL_MAP_FLUSH_EXPLICIT_BIT;
		if (usage & UsageFlags::DYNAMIC_STORAGE_BIT)
			flags |= GL_DYNAMIC_STORAGE_BIT;
		if (usage & UsageFlags::CLIENT_STORAGE_BIT)
			flags |= GL_CLIENT_STORAGE_BIT;

		glNamedBufferStorage(_buffer, _dataSize, desc.getStream(), flags);
	}
	else
	{
		GLenum flags = GL_STATIC_DRAW;
		if (usage & UsageFlags::MAP_READ_BIT)
			flags = GL_STATIC_DRAW;
		if (usage & UsageFlags::MAP_WRITE_BIT)
			flags = GL_DYNAMIC_READ;

		glNamedBufferData(_buffer, _dataSize, desc.getStream(), flags);
	}

	return true;
}

bool
OGLGraphicsBuf::is_open() const noexcept
{
	return _buffer == GL_NONE ? false : true;
}

void
OGLGraphicsBuf::close() noexcept
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
OGLGraphicsBuf::size() const noexcept
{
	return _dataSize;
}

void
OGLGraphicsBuf::resize(const char* data, GLsizeiptr datasize) noexcept
{
	if (_usage & UsageFlags::IMMUTABLE_STORAGE)
		return;

	GLenum flags = GL_STATIC_DRAW;
	if (_usage & UsageFlags::MAP_READ_BIT)
		flags = GL_STATIC_DRAW;
	if (_usage & UsageFlags::MAP_WRITE_BIT)
		flags = GL_DYNAMIC_READ;

	glNamedBufferData(_buffer, datasize, data, flags);
	_dataSize = datasize;
}

int
OGLGraphicsBuf::flush() noexcept
{
	return this->flush(0, _dataSize);
}

int
OGLGraphicsBuf::flush(GLintptr offset, GLsizeiptr cnt) noexcept
{
	glFlushMappedNamedBufferRange(_buffer, offset, cnt);
	return cnt;
}

GLsizeiptr
OGLGraphicsBuf::read(char* str, GLsizeiptr cnt) noexcept
{
	if (_dataSize < _dataOffset + cnt)
	{
		cnt = _dataSize - _dataOffset;
		if (cnt == 0)
			return 0;
	}

	void* data = this->map(_dataOffset, cnt, AccessFlags::MAP_READ_BIT);
	if (data)
	{
		std::memcpy(str, data, cnt);
		_dataOffset += cnt;
	}

	this->unmap();
	return cnt;
}

GLsizeiptr
OGLGraphicsBuf::write(const char* str, GLsizeiptr cnt) noexcept
{
	if (_dataSize >= _dataOffset + cnt)
	{
		cnt = _dataSize - _dataOffset;
		if (cnt == 0)
			return 0;
	}

	void* data = this->map(_dataOffset, cnt, AccessFlags::MAP_WRITE_BIT);
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
OGLGraphicsBuf::seekg(GLintptr pos, ios_base::seekdir dir) noexcept
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
OGLGraphicsBuf::tellg() noexcept
{
	return _dataOffset;
}

void*
OGLGraphicsBuf::map(std::uint32_t access) noexcept
{
	return this->map(0, _dataSize, access);
}

void*
OGLGraphicsBuf::map(GLintptr offset, GLsizeiptr cnt, std::uint32_t access) noexcept
{
	if (!_data)
	{
		GLbitfield flags = GL_MAP_READ_BIT;
		if (access & AccessFlags::MAP_READ_BIT)
			flags |= GL_MAP_READ_BIT;
		if (access & AccessFlags::MAP_WRITE_BIT)
			flags |= GL_MAP_WRITE_BIT;
		if (access & AccessFlags::MAP_UNSYNCHRONIZED_BIT)
			flags |= GL_MAP_UNSYNCHRONIZED_BIT;
		if (_usage & UsageFlags::MAP_PERSISTENT_BIT)
			flags |= GL_MAP_PERSISTENT_BIT;
		if (_usage & UsageFlags::MAP_COHERENT_BIT)
			flags |= GL_MAP_COHERENT_BIT;
		if (_usage & UsageFlags::MAP_FLUSH_EXPLICIT_BIT)
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
OGLGraphicsBuf::unmap() noexcept
{
	assert(_isMapping);

	if (_usage & UsageFlags::MAP_PERSISTENT_BIT)
	{
		_isMapping = GL_FALSE;
		return;
	}

	glUnmapNamedBuffer(_buffer);
	_data = nullptr;
	_isMapping = GL_FALSE;
}

bool
OGLGraphicsBuf::isMapping() const noexcept
{
	return _isMapping ? GL_TRUE : GL_FALSE;
}

GLuint
OGLGraphicsBuf::getInstanceID() noexcept
{
	return _buffer;
}

const GraphicsDataDesc&
OGLGraphicsBuf::getGraphicsDataDesc() const noexcept
{
	return _desc;
}

void
OGLGraphicsBuf::bind() noexcept
{
	glBindBuffer(_target, _buffer);
}

_NAME_END