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
#include "egl2_buffer.h"

_NAME_BEGIN

EGL2BufferData::EGL2BufferData() noexcept
	: _buffer(GL_NONE)
	, _isMapping(false)
{
}

EGL2BufferData::~EGL2BufferData() noexcept
{
	this->close();
}

bool
EGL2BufferData::open(StreamType type, std::uint32_t usage, const char* data, streamsize datasize) noexcept
{
	assert(!_buffer);
	assert(type == StreamType::VERTEX_STREAM || type == StreamType::INDEX_STREAM);

	_dataOffset = 0;
	_dataTell = 0;
	_dataSize = datasize;
	_usage = usage;
	_isMapping = false;

	if (type == StreamType::VERTEX_STREAM)
		_target = GL_ARRAY_BUFFER;
	else if (type == StreamType::INDEX_STREAM)
		_target = GL_ELEMENT_ARRAY_BUFFER;

	GLenum flags = GL_STATIC_DRAW;
	if (usage & VertexUsage::MAP_READ_BIT)
		flags = GL_STATIC_DRAW;
	if (usage & VertexUsage::MAP_WRITE_BIT)
		flags = GL_DYNAMIC_DRAW;

	GL_CHECK(glGenBuffers(1, &_buffer));
	GL_CHECK(glBindBuffer(_target, _buffer));
	GL_CHECK(glBufferData(_target, _dataSize, data, flags));

	return true;
}

bool
EGL2BufferData::is_open() const noexcept
{
	return _buffer == GL_NONE ? false : true;
}

void
EGL2BufferData::close() noexcept
{
	assert(!_isMapping);

	if (_isMapping)
	{
		this->unmap();
	}

	if (_buffer)
	{
		GL_CHECK(glDeleteBuffers(1, &_buffer));
		_buffer = 0;
	}
}


streamsize
EGL2BufferData::size() const noexcept
{
	return _dataSize;
}

void
EGL2BufferData::resize(const char* data, streamsize datasize) noexcept
{
	if (_usage & VertexUsage::IMMUTABLE_STORAGE)
		return;

	GLenum flags = GL_STATIC_DRAW;
	if (_usage & VertexUsage::MAP_READ_BIT)
		flags = GL_STATIC_DRAW;
	if (_usage & VertexUsage::MAP_WRITE_BIT)
		flags = GL_DYNAMIC_DRAW;

	GL_CHECK(glBindBuffer(_target, _buffer));
	GL_CHECK(glBufferData(_target, datasize, data, flags));
	_dataSize = datasize;
}

int
EGL2BufferData::flush() noexcept
{
	return this->flush(0, _dataSize);
}

int 
EGL2BufferData::flush(ios_base::off_type offset, streamsize cnt) noexcept
{
	GL_CHECK(glBindBuffer(_target, _buffer));
	GL_CHECK(glFlushMappedBufferRangeEXT(_target, offset, cnt));
	return cnt;
}

streamsize
EGL2BufferData::read(char* str, streamsize cnt) noexcept
{
	if (_dataSize < _dataOffset + cnt)
	{
		cnt = _dataSize - _dataOffset;
		if (cnt == 0)
			return 0;
	}

	const char* data = this->map(_dataOffset, cnt, AccessMapping::MAP_READ_BIT);
	if (data)
	{
		std::memcpy(str, data, cnt);
		_dataOffset += cnt;
	}

	this->unmap();
	return cnt;
}

streamsize
EGL2BufferData::write(const char* str, streamsize cnt) noexcept
{
	if (_dataSize >= _dataOffset + cnt)
	{
		cnt = _dataSize - _dataOffset;
		if (cnt == 0)
			return 0;
	}

	const char* data = this->map(_dataOffset, cnt, AccessMapping::MAP_WRITE_BIT);
	if (data)
	{
		std::memcpy((char*)data, data, cnt);
		_dataOffset += cnt;
		this->unmap();
		return cnt;
	}

	this->unmap();
	return 0;
}

streamoff
EGL2BufferData::seekg(ios_base::off_type pos, ios_base::seekdir dir) noexcept
{
	assert(dir == ios_base::beg || dir == ios_base::cur || dir == ios_base::end);

	if (dir == ios_base::beg)
	{
		_dataTell = pos;
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

		_dataTell = pos;
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
		_dataTell = pos;
		return pos;
	}

	return 0;
}

streamoff
EGL2BufferData::tellg() noexcept
{
	return _dataTell;
}

const char*
EGL2BufferData::map(std::uint32_t access) noexcept
{
	return this->map(0, _dataSize, access);
}

const char*
EGL2BufferData::map(ios_base::off_type offset, streamsize cnt, std::uint32_t access) noexcept
{
	assert(!_isMapping);

	GLbitfield flags = GL_MAP_READ_BIT_EXT;
	if (access & AccessMapping::MAP_READ_BIT)
		flags |= GL_MAP_READ_BIT_EXT;
	if (access & AccessMapping::MAP_WRITE_BIT)
		flags |= GL_MAP_WRITE_BIT_EXT;

	_isMapping = true;
	
	GL_CHECK(glBindBuffer(_target, _buffer));
	auto result = (char*)glMapBufferRangeEXT(_target, offset, cnt, flags);
	GL_CHECK(result);
	assert(false);
	return result;
}

void
EGL2BufferData::unmap() noexcept
{
	assert(_isMapping);

	GL_CHECK(glBindBuffer(_target, _buffer));
	//GL_CHECK(glUnmapBufferEXT(_target));
	assert(false);
	_isMapping = false;
}

bool
EGL2BufferData::isMapping() const noexcept
{
	return _isMapping;
}

GLuint
EGL2BufferData::getInstanceID() noexcept
{
	return _buffer;
}

void
EGL2BufferData::bind() noexcept
{
	GL_CHECK(glBindBuffer(_target, _buffer));
}

EGL2VertexBuffer::EGL2VertexBuffer() noexcept
	: VertexBufferData(&_vertexData)
{
}

EGL2VertexBuffer::~EGL2VertexBuffer() noexcept
{
	this->close();
}

void
EGL2VertexBuffer::open(const VertexLayout& layout, std::uint32_t usage, const char* data, streamsize datasize) noexcept
{
	assert(datasize > 0);
	_vertexLayout = layout;
	_vertexData.open(StreamType::VERTEX_STREAM, usage, data, datasize);
}

void
EGL2VertexBuffer::close() noexcept
{
	_vertexData.close();
}

void
EGL2VertexBuffer::bind() noexcept
{
	_vertexData.bind();
}

void
EGL2VertexBuffer::bindLayout() noexcept
{
	GLuint offset = 0;
	GLuint stride = this->getVertexSize();

	auto& components = this->getVertexComponents();
	for (auto& it : components)
	{
		auto type = EGL2Types::asEGL2VertexFormat(it.getVertexFormat());

		GL_CHECK(glEnableVertexAttribArray((GLuint)it.getVertexAttrib()));
		GL_CHECK(glVertexAttribPointer(it.getVertexAttrib(), it.getVertexCount(), type, GL_FALSE, stride, (char*)offset));

		offset += it.getVertexSize();
	}
}

const VertexComponents&
EGL2VertexBuffer::getVertexComponents() const noexcept
{
	return _vertexLayout.getVertexComponents();
}

std::size_t
EGL2VertexBuffer::getVertexCount() const noexcept
{
	return _vertexData.size() / _vertexLayout.getVertexSize();
}

std::uint32_t
EGL2VertexBuffer::getVertexSize() const noexcept
{
	return _vertexLayout.getVertexSize();
}

std::size_t
EGL2VertexBuffer::getVertexDataSize() const noexcept
{
	return _vertexData.size();
}

EGL2IndexBuffer::EGL2IndexBuffer() noexcept
	: IndexBufferData(&_indexData)
	, _indexType(IndexType::GPU_UINT16)
{
}

EGL2IndexBuffer::~EGL2IndexBuffer() noexcept
{
	this->close();
}

void
EGL2IndexBuffer::open(IndexType type, std::uint32_t usage, const char* data, streamsize datasize) noexcept
{
	_indexType = type;
	_indexSize = type == IndexType::GPU_UINT16 ? sizeof(std::uint16_t) : sizeof(std::uint32_t);
	_indexData.open(StreamType::INDEX_STREAM, usage, data, datasize);
}

void
EGL2IndexBuffer::close() noexcept
{
	_indexData.close();
}

std::size_t
EGL2IndexBuffer::getIndexCount() const noexcept
{
	return this->getIndexDataSize() / _indexSize;
}

std::size_t
EGL2IndexBuffer::getIndexDataSize() const noexcept
{
	return _indexData.size();
}

std::uint32_t
EGL2IndexBuffer::getIndexSize() const noexcept
{
	return _indexSize;
}

IndexType
EGL2IndexBuffer::getIndexType() const noexcept
{
	return _indexType;
}

GLuint
EGL2IndexBuffer::getInstanceID() noexcept
{
	return _indexData.getInstanceID();
}

void
EGL2IndexBuffer::bind() noexcept
{
	_indexData.bind();
}

EGL2RenderBuffer::EGL2RenderBuffer() noexcept
{
}

EGL2RenderBuffer::~EGL2RenderBuffer() noexcept
{
	this->close();
}

void
EGL2RenderBuffer::setup(VertexBufferDataPtr vb, IndexBufferDataPtr ib) except
{
	_vb = std::dynamic_pointer_cast<EGL2VertexBuffer>(vb);
	_ib = std::dynamic_pointer_cast<EGL2IndexBuffer>(ib);
}

void
EGL2RenderBuffer::close() noexcept
{
	if (_vb)
	{
		_vb->close();
		_vb = nullptr;
	}
	
	if (_ib)
	{
		_ib->close();
		_ib = nullptr;
	}
}

std::size_t 
EGL2RenderBuffer::getNumVertices() const noexcept
{
	if (_vb)
		return _vb->getVertexCount();
	return 0;
}

std::size_t 
EGL2RenderBuffer::getNumIndices() const noexcept
{
	if (_ib)
		return _ib->getIndexCount();
	return 0;
}

VertexBufferDataPtr 
EGL2RenderBuffer::getVertexBuffer() const noexcept
{
	return _vb;
}

IndexBufferDataPtr 
EGL2RenderBuffer::getIndexBuffer() const noexcept
{
	return _ib;
}

void
EGL2RenderBuffer::apply() noexcept
{
	if (_vb)
	{
		_vb->bind();
		_vb->bindLayout();
	}

	if (_ib) _ib->bind();
}

_NAME_END