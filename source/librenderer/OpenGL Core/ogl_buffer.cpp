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
#include <ray/ogl_buffer.h>

_NAME_BEGIN

OGLBufferData::OGLBufferData() noexcept
	: _buffer(GL_NONE)
	, _bindlessBuffer(GL_NONE)
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

	GL_CHECK(glGenBuffers(1, &_buffer));
	GL_CHECK(glBindBuffer(_target, _buffer));

#if !defined(EGLAPI)
	if (usage & VertexUsage::IMMUTABLE_STORAGE && OGLFeatures::ARB_buffer_storage)
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

		GL_CHECK(glBufferStorage(_target, _dataSize, data, flags));
	}
	else
#endif
	{
		GLenum flags = GL_STATIC_DRAW;
		if (usage & VertexUsage::MAP_READ_BIT)
			flags = GL_STATIC_DRAW;
		if (usage & VertexUsage::MAP_WRITE_BIT)
			flags = GL_DYNAMIC_READ;

		GL_CHECK(glBufferData(_target, _dataSize, data, flags));
	}

#if !defined(EGLAPI)
	if (OGLFeatures::NV_vertex_buffer_unified_memory)
	{
		GL_CHECK(glGetNamedBufferParameterui64vNV(_buffer, GL_BUFFER_GPU_ADDRESS_NV, &_bindlessBuffer));
		GL_CHECK(glMakeNamedBufferResidentNV(_buffer, GL_READ_ONLY));
	}
#endif

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
		GL_CHECK(glDeleteBuffers(1, &_buffer));
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

	GL_CHECK(glBindBuffer(_target, _buffer));
	GL_CHECK(glBufferData(_target, datasize, data, flags));
	_dataSize = datasize;
}

int
OGLBufferData::flush() noexcept
{
	if (OGLFeatures::ARB_buffer_storage)
	{
		GL_CHECK(glBindBuffer(_target, _buffer));
		GL_CHECK(glFlushMappedBufferRange(_target, 0, _dataSize));
		return _dataSize;
	}

	return 0;
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

	const char* data = this->map(AccessMapping::MAP_READ_BIT);
	if (data)
	{
		std::memcpy(str, data + _dataOffset, cnt);
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

	auto data = this->map(AccessMapping::MAP_READ_BIT | AccessMapping::MAP_WRITE_BIT);
	if (data)
	{
		std::memcpy((char*)_data + _dataOffset, data, cnt);
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
	if (!_data)
	{
		if (_usage & VertexUsage::IMMUTABLE_STORAGE && OGLFeatures::ARB_buffer_storage)
		{
			GLbitfield flags = GL_MAP_READ_BIT;
			if (access & AccessMapping::MAP_READ_BIT)
				flags |= GL_MAP_READ_BIT;
			if (access & AccessMapping::MAP_WRITE_BIT)
				flags |= GL_MAP_WRITE_BIT;
#if !defined(EGLAPI)
			if (_usage & VertexUsage::MAP_PERSISTENT_BIT)
				flags |= GL_MAP_PERSISTENT_BIT;
			if (_usage & VertexUsage::MAP_COHERENT_BIT)
				flags |= GL_MAP_COHERENT_BIT;
#endif

			GL_CHECK(glBindBuffer(_target, _buffer));
			_data = (char*)glMapBufferRange(_target, 0, _dataSize, flags);
		}
		else
		{
			GLbitfield flags = GL_MAP_READ_BIT;
			if (access & AccessMapping::MAP_READ_BIT)
				flags |= GL_MAP_READ_BIT;
			if (access & AccessMapping::MAP_WRITE_BIT)
				flags |= GL_MAP_WRITE_BIT;

			GL_CHECK(glBindBuffer(_target, _buffer));
			_data = (char*)glMapBufferRange(_target, 0, _dataSize, flags);
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

#if defined(EGLAPI)
	if (OGLFeatures::ARB_buffer_storage)
	{
		if (_usage & VertexUsage::MAP_PERSISTENT_BIT)
		{
			_isMapping = false;
			return;
		}
	}
#endif

	GL_CHECK(glBindBuffer(_target, _buffer));
	GL_CHECK(glUnmapBuffer(_target));
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

GLuint64
OGLBufferData::getInstanceAddr() noexcept
{
	return _bindlessBuffer;
}

void
OGLBufferData::bind() noexcept
{
	GL_CHECK(glBindBuffer(_target, _buffer));
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
	_vertexData.bind();
}

void
OGLVertexBuffer::bindLayout() noexcept
{
	GLuint offset = 0;
	GLsizei vertexByteSize = this->getVertexSize();

	auto& components = this->getVertexComponents();
	for (auto& it : components)
	{
		auto type = OGLTypes::asOGLVertexFormat(it.getVertexFormat());
		auto normalize = it.getNormalize() ? GL_TRUE : GL_FALSE;

#if !defined(EGLAPI)
		if (OGLFeatures::NV_vertex_buffer_unified_memory)
		{
			GL_CHECK(glBindVertexBuffer(it.getVertexAttrib(), _vertexData.getInstanceID(), offset, vertexByteSize));
		}
		else if (OGLFeatures::ARB_vertex_attrib_binding)
		{
			GL_CHECK(glEnableVertexAttribArray((GLuint)it.getVertexAttrib()));
			GL_CHECK(glVertexAttribFormat(it.getVertexAttrib(), it.getVertexCount(), type, normalize, 0));
			GL_CHECK(glVertexAttribBinding(it.getVertexAttrib(), it.getVertexAttrib()));

			GL_CHECK(glBindVertexBuffer(it.getVertexAttrib(), _vertexData.getInstanceID(), offset, vertexByteSize));
		}
		else
#endif
		{
			GL_CHECK(glEnableVertexAttribArray((GLuint)it.getVertexAttrib()));
			GL_CHECK(glVertexAttribPointer(it.getVertexAttrib(), it.getVertexCount(), type, normalize, vertexByteSize, (const char*)nullptr + offset));
		}

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

GLuint64
OGLIndexBuffer::getInstanceAddr() noexcept
{
	return _indexData.getInstanceAddr();
}

void
OGLIndexBuffer::bind() noexcept
{
	_indexData.bind();
}

OGLRenderBuffer::OGLRenderBuffer() noexcept
	: _vao(GL_NONE)
{
}

OGLRenderBuffer::~OGLRenderBuffer() noexcept
{
	this->close();
}

void
OGLRenderBuffer::setup(VertexBufferDataPtr vb, IndexBufferDataPtr ib) except
{
	assert(GL_NONE == _vao);

#if !defined(EGLAPI)
	if (!OGLFeatures::NV_vertex_buffer_unified_memory)
	{
		if (OGLFeatures::ARB_vertex_array_object)
		{
			GL_CHECK(glGenVertexArrays(1, &_vao));
			GL_CHECK(glBindVertexArray(_vao));
		}
	}
#else
	GL_CHECK(glGenVertexArrays(1, &_vao));
	GL_CHECK(glBindVertexArray(_vao));
#endif

	auto vbo = std::dynamic_pointer_cast<OGLVertexBuffer>(vb);
	if (vbo)
	{
		vbo->bind();
		vbo->bindLayout();
	}

	auto ibo = std::dynamic_pointer_cast<OGLIndexBuffer>(ib);
	if (ibo)
	{
		ibo->bind();
	}

	GL_CHECK(glBindVertexArray(0));

	this->setVertexBuffer(vb);
	this->setIndexBuffer(ib);
}

void
OGLRenderBuffer::close() noexcept
{
	if (_vao)
	{
		GL_CHECK(glDeleteVertexArrays(1, &_vao));
		_vao = GL_NONE;
	}
}

GLuint
OGLRenderBuffer::getInstanceID() noexcept
{
	return _vao;
}

void
OGLRenderBuffer::apply() noexcept
{
#if 0
	if (OGLFeatures::NV_vertex_buffer_unified_memory)
	{
		if (vb)
		{
			auto bindlessVbo = vb->getInstanceAddr();
			auto vertexSize = vb->getVertexDataSize();

			GLuint64 offset = 0;

			for (auto& it : vb->getVertexComponents())
			{
				GL_CHECK(glBufferAddressRangeNV(GL_VERTEX_ATTRIB_ARRAY_ADDRESS_NV, it.getVertexAttrib(), bindlessVbo + offset, vertexSize - offset));

				offset += it.getVertexSize();
			}
		}

		if (ib)
		{
			auto bindlessIbo = ib->getInstanceAddr();
			auto indexDataSize = ib->getIndexDataSize();

			GL_CHECK(glBufferAddressRangeNV(GL_ELEMENT_ARRAY_ADDRESS_NV, 0, bindlessIbo, indexDataSize));
		}
	}
	else if (OGLFeatures::ARB_vertex_attrib_binding)
	{
		if (vb)
		{
			auto vbo = vb->getInstanceID();

			GLuint offset = 0;

			for (auto& it : vb->getVertexComponents())
			{
				GL_CHECK(glBindVertexBuffer(it.getVertexAttrib(), vbo, offset, vb->getVertexSize()));

				offset += it.getVertexSize();
			}
		}

		if (ib)
		{
			GL_CHECK(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ib->getInstanceID()));
		}
	}
	else
#endif
	{
		GL_CHECK(glBindVertexArray(_vao));
	}
}

_NAME_END