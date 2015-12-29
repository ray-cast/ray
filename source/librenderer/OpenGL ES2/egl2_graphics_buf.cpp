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
#include "egl2_graphics_buf.h"

_NAME_BEGIN

EGL2GraphicsBuf::EGL2GraphicsBuf() noexcept
	: _buffer(GL_NONE)
	, _isMapping(GL_FALSE)
	, _data(nullptr)
{
}

EGL2GraphicsBuf::~EGL2GraphicsBuf() noexcept
{
	this->close();
}

bool
EGL2GraphicsBuf::open(const GraphicsDataDesc& desc) noexcept
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
	else
		return false;

	GLenum flags = GL_STATIC_DRAW;

	auto usage = desc.getUsage();
	if (usage & UsageFlags::MAP_READ_BIT)
		flags = GL_STATIC_DRAW;
	if (usage & UsageFlags::MAP_WRITE_BIT)
		flags = GL_DYNAMIC_DRAW;

	GL_CHECK(glGenBuffers(1, &_buffer));
	GL_CHECK(glBindBuffer(_target, _buffer));
	GL_CHECK(glBufferData(_target, _dataSize, desc.getStream(), flags));

	return true;
}

bool
EGL2GraphicsBuf::is_open() const noexcept
{
	return _buffer == GL_NONE ? false : true;
}

void
EGL2GraphicsBuf::close() noexcept
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
EGL2GraphicsBuf::size() const noexcept
{
	return _dataSize;
}

void
EGL2GraphicsBuf::resize(const char* data, GLsizeiptr datasize) noexcept
{
	if (_usage & UsageFlags::IMMUTABLE_STORAGE)
		return;

	GLenum flags = GL_STATIC_DRAW;
	if (_usage & UsageFlags::MAP_READ_BIT)
		flags = GL_STATIC_DRAW;
	if (_usage & UsageFlags::MAP_WRITE_BIT)
		flags = GL_DYNAMIC_DRAW;

	GL_CHECK(glBindBuffer(_target, _buffer));
	GL_CHECK(glBufferData(_target, datasize, data, flags));
	_dataSize = datasize;
}

int
EGL2GraphicsBuf::flush() noexcept
{
	return this->flush(0, _dataSize);
}

int
EGL2GraphicsBuf::flush(GLintptr offset, GLsizeiptr cnt) noexcept
{
	GL_CHECK(glBindBuffer(_target, _buffer));
	GL_CHECK(glFlushMappedBufferRangeEXT(_target, offset, cnt));
	return cnt;
}

GLsizeiptr
EGL2GraphicsBuf::read(char* str, GLsizeiptr cnt) noexcept
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
EGL2GraphicsBuf::write(const char* str, GLsizeiptr cnt) noexcept
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
EGL2GraphicsBuf::seekg(GLintptr pos, ios_base::seekdir dir) noexcept
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
EGL2GraphicsBuf::tellg() noexcept
{
	return _dataOffset;
}

void*
EGL2GraphicsBuf::map(std::uint32_t access) noexcept
{
	return this->map(0, _dataSize, access);
}

void*
EGL2GraphicsBuf::map(GLintptr offset, GLsizeiptr cnt, std::uint32_t access) noexcept
{
	if (!_data)
	{
		GLbitfield flags = GL_MAP_READ_BIT_EXT;
		if (access & AccessFlags::MAP_READ_BIT)
			flags |= GL_MAP_READ_BIT_EXT;
		if (access & AccessFlags::MAP_WRITE_BIT)
			flags |= GL_MAP_WRITE_BIT_EXT;
		if (access & AccessFlags::MAP_UNSYNCHRONIZED_BIT)
			flags |= GL_MAP_UNSYNCHRONIZED_BIT_EXT;
		if (_usage & UsageFlags::MAP_FLUSH_EXPLICIT_BIT)
			flags |= GL_MAP_FLUSH_EXPLICIT_BIT_EXT;
		
		GL_CHECK(glBindBuffer(_target, _buffer));
		_data = glMapBufferRangeEXT(_target, offset, cnt, flags);
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
EGL2GraphicsBuf::unmap() noexcept
{
	assert(_isMapping);

	if (_usage & UsageFlags::MAP_PERSISTENT_BIT)
	{
		_isMapping = GL_FALSE;
		return;
	}
	
	GL_CHECK(glBindBuffer(_target, _buffer));
	GL_CHECK(glUnmapBufferOES(_target));
	_data = nullptr;
	_isMapping = GL_FALSE;
}

bool
EGL2GraphicsBuf::isMapping() const noexcept
{
	return _isMapping ? GL_TRUE : GL_FALSE;
}

GLuint
EGL2GraphicsBuf::getInstanceID() noexcept
{
	return _buffer;
}

const GraphicsDataDesc&
EGL2GraphicsBuf::getGraphicsDataDesc() const noexcept
{
	return _desc;
}

void
EGL2GraphicsBuf::bind() noexcept
{
	GL_CHECK(glBindBuffer(_target, _buffer));
}

_NAME_END