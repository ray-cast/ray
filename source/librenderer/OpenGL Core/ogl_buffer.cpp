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
#include "ogl_buffer.h"

_NAME_BEGIN

OGLBufferData::OGLBufferData() noexcept
	: _buffer(GL_NONE)
	, _isMapping(false)
	, _data(nullptr)
{
}

OGLBufferData::~OGLBufferData() noexcept
{
	this->close();
}

bool
OGLBufferData::open(StreamType type, std::uint32_t usage, const char* data, streamsize datasize) noexcept
{
	assert(!_buffer);
	assert(type == StreamType::VERTEX_STREAM || type == StreamType::INDEX_STREAM);

	_data = nullptr;
	_dataOffset = 0;
	_dataTell = 0;
	_dataSize = datasize;
	_usage = usage;
	_isMapping = false;

	if (type == StreamType::VERTEX_STREAM)
		_target = GL_ARRAY_BUFFER;
	else if (type == StreamType::INDEX_STREAM)
		_target = GL_ELEMENT_ARRAY_BUFFER;

	glGenBuffers(1, &_buffer);
	glBindBuffer(_target, _buffer);

	if (usage & VertexUsage::IMMUTABLE_STORAGE)
	{
		GLbitfield flags = GL_MAP_READ_BIT;
		if (usage & VertexUsage::MAP_READ_BIT)
			flags |= GL_MAP_READ_BIT;
		if (usage & VertexUsage::MAP_WRITE_BIT)
			flags |= GL_MAP_WRITE_BIT;
		if (usage & VertexUsage::MAP_PERSISTENT_BIT)
			flags |= GL_MAP_PERSISTENT_BIT;
		if (usage & VertexUsage::MAP_COHERENT_BIT)
			flags |= GL_MAP_COHERENT_BIT;
		if (usage & VertexUsage::DYNAMIC_STORAGE_BIT)
			flags |= GL_DYNAMIC_STORAGE_BIT;
		if (usage & VertexUsage::CLIENT_STORAGE_BIT)
			flags |= GL_CLIENT_STORAGE_BIT;

		glNamedBufferStorage(_buffer, _dataSize, data, flags);
	}
	else
	{
		GLenum flags = GL_STATIC_DRAW;
		if (usage & VertexUsage::MAP_READ_BIT)
			flags = GL_STATIC_DRAW;
		if (usage & VertexUsage::MAP_WRITE_BIT)
			flags = GL_DYNAMIC_READ;

		glNamedBufferData(_buffer, _dataSize, data, flags);
	}

	return true;
}

bool
OGLBufferData::is_open() const noexcept
{
	return _buffer == GL_NONE ? false : true;
}

void
OGLBufferData::close() noexcept
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


streamsize
OGLBufferData::size() const noexcept
{
	return _dataSize;
}

void
OGLBufferData::resize(const char* data, streamsize datasize) noexcept
{
	if (_usage & VertexUsage::IMMUTABLE_STORAGE)
		return;

	GLenum flags = GL_STATIC_DRAW;
	if (_usage & VertexUsage::MAP_READ_BIT)
		flags = GL_STATIC_DRAW;
	if (_usage & VertexUsage::MAP_WRITE_BIT)
		flags = GL_DYNAMIC_READ;

	glNamedBufferData(_buffer, datasize, data, flags);
	_dataSize = datasize;
}

int
OGLBufferData::flush() noexcept
{
	return this->flush(0, _dataSize);
}

int
OGLBufferData::flush(ios_base::off_type offset, streamsize cnt) noexcept
{
	glFlushMappedNamedBufferRange(_target, offset, cnt);
	return cnt;
}

streamsize
OGLBufferData::read(char* str, streamsize cnt) noexcept
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
OGLBufferData::write(const char* str, streamsize cnt) noexcept
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
OGLBufferData::seekg(ios_base::off_type pos, ios_base::seekdir dir) noexcept
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
OGLBufferData::tellg() noexcept
{
	return _dataTell;
}

const char*
OGLBufferData::map(std::uint32_t access) noexcept
{
	return this->map(0, _dataSize, access);
}

const char*
OGLBufferData::map(ios_base::off_type offset, streamsize cnt, std::uint32_t access) noexcept
{
	if (!_data)
	{
		if (_usage & VertexUsage::IMMUTABLE_STORAGE && OGLFeatures::ARB_buffer_storage)
		{
			GLbitfield flags = GL_MAP_READ_BIT;
			if (access & AccessMapping::MAP_READ_BIT)
				flags |= GL_MAP_READ_BIT;
			if (access & AccessMapping::MAP_WRITE_BIT)
				flags |= GL_MAP_WRITE_BIT;
			if (access & AccessMapping::MAP_UNSYNCHRONIZED_BIT)
				flags |= GL_MAP_UNSYNCHRONIZED_BIT;
			if (_usage & VertexUsage::MAP_PERSISTENT_BIT)
				flags |= GL_MAP_PERSISTENT_BIT;
			if (_usage & VertexUsage::MAP_COHERENT_BIT)
				flags |= GL_MAP_COHERENT_BIT;
			
			_data = (char*)glMapNamedBufferRange(_buffer, offset, cnt, flags);
		}
		else
		{
			GLbitfield flags = GL_MAP_READ_BIT;
			if (access & AccessMapping::MAP_READ_BIT)
				flags |= GL_MAP_READ_BIT;
			if (access & AccessMapping::MAP_WRITE_BIT)
				flags |= GL_MAP_WRITE_BIT;
			if (access & AccessMapping::MAP_UNSYNCHRONIZED_BIT)
				flags |= GL_MAP_UNSYNCHRONIZED_BIT;

			_data = (char*)glMapNamedBufferRange(_buffer, offset, cnt, flags);
		}
	}

	if (_data)
	{
		_isMapping = true;
		return (char*)_data;
	}

	return (char*)_data;
}

void
OGLBufferData::unmap() noexcept
{
	assert(_isMapping);

	if (OGLFeatures::ARB_buffer_storage)
	{
		if (_usage & VertexUsage::MAP_PERSISTENT_BIT)
		{
			_isMapping = false;
			return;
		}
	}

	glUnmapNamedBuffer(_buffer);
	_data = nullptr;
	_isMapping = false;
}

bool
OGLBufferData::isMapping() const noexcept
{
	return _isMapping;
}

GLuint
OGLBufferData::getInstanceID() noexcept
{
	return _buffer;
}

void
OGLBufferData::bind() noexcept
{
	glBindBuffer(_target, _buffer);
}

OGLVertexBuffer::OGLVertexBuffer() noexcept
	: VertexBufferData(&_vertexData)
{
}

OGLVertexBuffer::~OGLVertexBuffer() noexcept
{
	this->close();
}

void
OGLVertexBuffer::open(const VertexLayout& layout, std::uint32_t usage, const char* data, streamsize datasize) noexcept
{
	assert(datasize > 0);
	_vertexLayout = layout;
	_vertexData.open(StreamType::VERTEX_STREAM, usage, data, datasize);
}

void
OGLVertexBuffer::close() noexcept
{
	_vertexData.close();
}

void
OGLVertexBuffer::bind() noexcept
{
	GLuint offset = 0;
	GLsizei vertexByteSize = this->getVertexSize();

	auto& components = this->getVertexComponents();
	for (auto& it : components)
	{
		glBindVertexBuffer(it.getVertexAttrib(), _vertexData.getInstanceID(), offset, vertexByteSize);
		offset += it.getVertexSize();
	}
}

void
OGLVertexBuffer::bindLayout() noexcept
{
	GLuint offset = 0;

	auto& components = this->getVertexComponents();
	for (auto& it : components)
	{
		auto type = OGLTypes::asOGLVertexFormat(it.getVertexFormat());

		glEnableVertexAttribArray((GLuint)it.getVertexAttrib());
		glVertexAttribFormat(it.getVertexAttrib(), it.getVertexCount(), type, GL_FALSE, 0);
		glVertexAttribBinding(it.getVertexAttrib(), it.getVertexAttrib());

		offset += it.getVertexSize();
	}
}

const VertexComponents&
OGLVertexBuffer::getVertexComponents() const noexcept
{
	return _vertexLayout.getVertexComponents();
}

std::size_t
OGLVertexBuffer::getVertexCount() const noexcept
{
	return _vertexData.size() / _vertexLayout.getVertexSize();
}

std::uint32_t
OGLVertexBuffer::getVertexSize() const noexcept
{
	return _vertexLayout.getVertexSize();
}

std::size_t
OGLVertexBuffer::getVertexDataSize() const noexcept
{
	return _vertexData.size();
}

OGLIndexBuffer::OGLIndexBuffer() noexcept
	: IndexBufferData(&_indexData)
	, _indexType(IndexType::GPU_UINT16)
{
}

OGLIndexBuffer::~OGLIndexBuffer() noexcept
{
	this->close();
}

void
OGLIndexBuffer::open(IndexType type, std::uint32_t usage, const char* data, streamsize datasize) noexcept
{
	_indexType = type;
	_indexSize = type == IndexType::GPU_UINT16 ? sizeof(std::uint16_t) : sizeof(std::uint32_t);
	_indexData.open(StreamType::INDEX_STREAM, usage, data, datasize);
}

void
OGLIndexBuffer::close() noexcept
{
	_indexData.close();
}

std::size_t
OGLIndexBuffer::getIndexCount() const noexcept
{
	return this->getIndexDataSize() / _indexSize;
}

std::size_t
OGLIndexBuffer::getIndexDataSize() const noexcept
{
	return _indexData.size();
}

std::uint32_t
OGLIndexBuffer::getIndexSize() const noexcept
{
	return _indexSize;
}

IndexType
OGLIndexBuffer::getIndexType() const noexcept
{
	return _indexType;
}

GLuint
OGLIndexBuffer::getInstanceID() noexcept
{
	return _indexData.getInstanceID();
}

void
OGLIndexBuffer::bind() noexcept
{
	_indexData.bind();
}

OGLRenderBuffer::OGLRenderBuffer() noexcept
{
}

OGLRenderBuffer::~OGLRenderBuffer() noexcept
{
	this->close();
}

void
OGLRenderBuffer::setup(VertexBufferDataPtr vb, IndexBufferDataPtr ib) except
{
	_vb = std::dynamic_pointer_cast<OGLVertexBuffer>(vb);
	_ib = std::dynamic_pointer_cast<OGLIndexBuffer>(ib);
}

void
OGLRenderBuffer::close() noexcept
{
	if (_vb)
	{
		_vb.reset();
		_vb = nullptr;
	}
		
	if (_ib)
	{
		_ib.reset();
		_ib = nullptr;		
	}
}

std::size_t
OGLRenderBuffer::getNumVertices() const noexcept
{
	if (_vb)
		return _vb->getVertexCount();
	return 0;
}

std::size_t
OGLRenderBuffer::getNumIndices() const noexcept
{
	if (_ib)
		return _ib->getIndexCount();
	return 0;
}

VertexBufferDataPtr
OGLRenderBuffer::getVertexBuffer() const noexcept
{
	return _vb;
}

IndexBufferDataPtr
OGLRenderBuffer::getIndexBuffer() const noexcept
{
	return _ib;
}

void
OGLRenderBuffer::apply() noexcept
{
	if (_vb) _vb->bind();
	if (_ib) _ib->bind();
}

_NAME_END