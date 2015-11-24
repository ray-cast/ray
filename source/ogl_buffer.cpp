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

OGLVertexBuffer::OGLVertexBuffer() noexcept
	: _vbo(GL_NONE)
	, _bindlessVbo(GL_NONE)
	, _data(nullptr)
{
}

OGLVertexBuffer::~OGLVertexBuffer() noexcept
{
	this->close();
}

void
OGLVertexBuffer::setup(VertexBufferDataPtr vb) except
{
	assert(!_vbo);
	assert(vb && vb->data());

	glGenBuffers(1, &_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, _vbo);

	auto vertexUsage = vb->getVertexUsage();
	auto vertexDataSize = vb->getVertexDataSize();
	auto vertexByteSize = vb->getVertexSize();

	_vb = vb;
	_dataSize = vertexDataSize;

#if !defined(EGLAPI)
	if (vertexUsage & GPU_IMMUTABLE_STORAGE && OGLFeatures::ARB_buffer_storage)
	{
		GLbitfield flags = GL_MAP_READ_BIT;
		if (vertexUsage & GPU_MAP_READ_BIT)
			flags |= GL_MAP_READ_BIT;
		if (vertexUsage & GPU_MAP_WRITE_BIT)
			flags |= GL_MAP_WRITE_BIT;
		if (vertexUsage & GPU_DYNAMIC_STORAGE_BIT)
			flags |= GL_DYNAMIC_STORAGE_BIT;
		if (vertexUsage & GPU_MAP_PERSISTENT_BIT)
			flags |= GL_MAP_PERSISTENT_BIT;
		if (vertexUsage & GPU_CLIENT_STORAGE_BIT)
			flags |= GL_CLIENT_STORAGE_BIT;

		glBufferStorage(GL_ARRAY_BUFFER, vertexDataSize, vb->data(), flags);

		if (vertexUsage & GPU_MAP_PERSISTENT_BIT &&
			vertexUsage & GPU_MAP_WRITE_BIT)
		{
			_data = glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY);
		}
	}
	else
#endif
	{
		GLenum usage = GL_STATIC_DRAW;
		if (vertexUsage & GPU_MAP_READ_BIT)
			usage = GL_STATIC_DRAW;
		if (vertexUsage & GPU_MAP_WRITE_BIT)
			usage = GL_DYNAMIC_DRAW;

		glBufferData(GL_ARRAY_BUFFER, vertexDataSize, vb->data(), usage);
	}

	GLuint offset = 0;

	auto& components = vb->getVertexComponents();
	for (auto& it : components)
	{
		auto type = OGLTypes::asOGLVertexFormat(it.getVertexFormat());
		auto normalize = it.getNormalize() ? GL_TRUE : GL_FALSE;

#if !defined(EGLAPI)
		if (OGLFeatures::NV_vertex_buffer_unified_memory)
		{
			glBindVertexBuffer(it.getVertexAttrib(), _vbo, offset, vertexByteSize);
		}
		else if (OGLFeatures::ARB_vertex_attrib_binding)
		{
			glEnableVertexAttribArray((GLuint)it.getVertexAttrib());
			glVertexAttribFormat(it.getVertexAttrib(), it.getVertexCount(), type, normalize, 0);
			glVertexAttribBinding(it.getVertexAttrib(), it.getVertexAttrib());

			glBindVertexBuffer(it.getVertexAttrib(), _vbo, offset, vertexByteSize);
		}
		else
#endif
		{
			glEnableVertexAttribArray((GLuint)it.getVertexAttrib());
			glVertexAttribPointer(it.getVertexAttrib(), it.getVertexCount(), type, normalize, vertexByteSize, (const char*)nullptr + offset);
		}

		offset += it.getVertexSize();
	}

#if !defined(EGLAPI)
	if (OGLFeatures::NV_vertex_buffer_unified_memory)
	{
		glGetNamedBufferParameterui64vNV(_vbo, GL_BUFFER_GPU_ADDRESS_NV, &_bindlessVbo);
		glMakeNamedBufferResidentNV(_vbo, GL_READ_ONLY);
	}
#endif
}

void
OGLVertexBuffer::close() noexcept
{
	if (_vbo)
	{
		glDeleteBuffers(1, &_vbo);
		_vbo = GL_NONE;
	}
}

void 
OGLVertexBuffer::update() noexcept
{
	auto vertexUsage = _vb->getVertexUsage();
	auto vertexDataSize = _vb->getVertexDataSize();
	auto vertexByteSize = _vb->getVertexSize();

	if (_data)
	{
		assert(_dataSize == vertexDataSize);
		std::memcpy(_data, _vb->data(), _vb->getVertexDataSize());
	}
	else
	{
		GLenum usage = GL_STATIC_DRAW;
		if (vertexUsage & GPU_MAP_READ_BIT)
			usage = GL_STATIC_DRAW;
		if (vertexUsage & GPU_MAP_WRITE_BIT)
			usage = GL_DYNAMIC_DRAW;

#if !defined(EGLAPI)
		if (OGLFeatures::ARB_direct_state_access)
		{
			glNamedBufferData(_vbo, vertexDataSize, _vb->data(), usage);
		}
		else
#endif
		{
			glBindBuffer(GL_ARRAY_BUFFER, _vbo);
			glBufferData(GL_ARRAY_BUFFER, vertexDataSize, _vb->data(), usage);
		}

		_dataSize = vertexDataSize;
	}
}

GLuint
OGLVertexBuffer::getInstanceID() noexcept
{
	return _vbo;
}

GLuint64
OGLVertexBuffer::getInstanceAddr() noexcept
{
	return _bindlessVbo;
}

VertexBufferDataPtr
OGLVertexBuffer::getVertexBufferData() const noexcept
{
	return _vb;
}

OGLIndexBuffer::OGLIndexBuffer() noexcept
	: _ibo(GL_NONE)
	, _bindlessIbo(GL_NONE)
	, _data(nullptr)
{
}

OGLIndexBuffer::~OGLIndexBuffer() noexcept
{
	this->close();
}

void
OGLIndexBuffer::setup(IndexBufferDataPtr ib) noexcept
{
	assert(!_ibo);
	assert(ib && ib->data());

	glGenBuffers(1, &_ibo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _ibo);

	auto indexType = OGLTypes::asOGLIndexType(ib->getIndexType());
	auto indexUsage = ib->getIndexUsage();
	auto indexCount = ib->getIndexCount();
	auto indexDataSize = ib->getIndexDataSize();

	_ib = ib;

#if !defined(EGLAPI)
	if (indexUsage & GPU_IMMUTABLE_STORAGE && OGLFeatures::ARB_buffer_storage)
	{
		GLbitfield flags = GL_MAP_READ_BIT;
		if (indexUsage & GPU_MAP_READ_BIT)
			flags |= GL_MAP_READ_BIT;
		if (indexUsage & GPU_MAP_WRITE_BIT)
			flags |= GL_MAP_WRITE_BIT;
		if (indexUsage & GPU_DYNAMIC_STORAGE_BIT)
			flags |= GL_DYNAMIC_STORAGE_BIT;
		if (indexUsage & GPU_MAP_PERSISTENT_BIT)
			flags |= GL_MAP_PERSISTENT_BIT;
		if (indexUsage & GPU_CLIENT_STORAGE_BIT)
			flags |= GL_CLIENT_STORAGE_BIT;

		glBufferStorage(GL_ELEMENT_ARRAY_BUFFER, indexDataSize, ib->data(), flags);

		if (indexUsage & GPU_MAP_PERSISTENT_BIT &&
			indexUsage & GPU_MAP_WRITE_BIT)
		{
			_data = glMapBuffer(GL_ELEMENT_ARRAY_BUFFER, GL_WRITE_ONLY);
		}
	}
	else
#endif
	{
		GLenum usage = GL_STATIC_DRAW;
		if (indexUsage & GPU_MAP_READ_BIT)
			usage = GL_STATIC_DRAW;
		if (indexUsage & GPU_MAP_WRITE_BIT)
			usage = GL_DYNAMIC_DRAW;

		glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexDataSize, ib->data(), usage);
	}

#if !defined(EGLAPI)
	if (OGLFeatures::NV_vertex_buffer_unified_memory)
	{
		glGetNamedBufferParameterui64vNV(_ibo, GL_BUFFER_GPU_ADDRESS_NV, &_bindlessIbo);
		glMakeNamedBufferResidentNV(_ibo, GL_READ_ONLY);
	}
#endif
}

void
OGLIndexBuffer::close() noexcept
{
	if (_ibo)
	{
		glDeleteBuffers(1, &_ibo);
		_ibo = GL_NONE;
	}
}

void
OGLIndexBuffer::update() noexcept
{
	auto indexUsage = _ib->getIndexUsage();
	auto indexDataSize = _ib->getIndexDataSize();
	auto indexByteSize = _ib->getIndexSize();

	if (_data)
	{
		assert(_dataSize == indexDataSize);
		std::memcpy(_data, _ib->data(), indexDataSize);
	}
	else
	{
		GLenum usage = GL_STATIC_DRAW;
		if (indexUsage & GPU_MAP_READ_BIT)
			usage = GL_STATIC_DRAW;
		if (indexUsage & GPU_MAP_WRITE_BIT)
			usage = GL_DYNAMIC_DRAW;

#if !defined(EGLAPI)
		if (OGLFeatures::ARB_direct_state_access)
		{
			glNamedBufferData(_ibo, indexDataSize, _ib->data(), usage);
		}
		else
#endif
		{
			glBindBuffer(GL_ARRAY_BUFFER, _ibo);
			glBufferData(GL_ARRAY_BUFFER, indexDataSize, _ib->data(), usage);
		}

		_dataSize = indexDataSize;
	}
}

GLuint
OGLIndexBuffer::getInstanceID() noexcept
{
	return _ibo;
}

GLuint64
OGLIndexBuffer::getInstanceAddr() noexcept
{
	return _bindlessIbo;
}

IndexBufferDataPtr 
OGLIndexBuffer::getIndexBufferData() const noexcept
{
	return _ib;
}

OGLRenderBuffer::OGLRenderBuffer() noexcept
	:_vao(GL_NONE)
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
			glGenVertexArrays(1, &_vao);
			glBindVertexArray(_vao);
		}
	}
#else
	glGenVertexArrays(1, &_vao);
	glBindVertexArray(_vao);
#endif

	if (vb)
	{
		_vb = std::make_shared<OGLVertexBuffer>();
		_vb->setup(vb);
	}

	if (ib)
	{
		_ib = std::make_shared<OGLIndexBuffer>();
		_ib->setup(ib);
	}

	this->setVertexBuffer(vb);
	this->setIndexBuffer(ib);
}

void
OGLRenderBuffer::close() noexcept
{
	if (_vao)
	{
		glDeleteVertexArrays(1, &_vao);
		_vao = GL_NONE;
	}
}

GLuint
OGLRenderBuffer::getInstanceID() noexcept
{
	return _vao;
}

void 
OGLRenderBuffer::update() noexcept
{
	if (_vb)
	{
		_vb->update();
	}

	if (_ib)
	{
		_ib->update();
	}
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
				glBufferAddressRangeNV(GL_VERTEX_ATTRIB_ARRAY_ADDRESS_NV, it.getVertexAttrib(), bindlessVbo + offset, vertexSize - offset);

				offset += it.getVertexSize();
			}
		}

		if (ib)
		{
			auto bindlessIbo = ib->getInstanceAddr();
			auto indexDataSize = ib->getIndexDataSize();

			glBufferAddressRangeNV(GL_ELEMENT_ARRAY_ADDRESS_NV, 0, bindlessIbo, indexDataSize);
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
				glBindVertexBuffer(it.getVertexAttrib(), vbo, offset, vb->getVertexSize());

				offset += it.getVertexSize();
			}
		}

		if (ib)
		{
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ib->getInstanceID());
		}
	}
	else
#endif
	{
		glBindVertexArray(_vao);
	}
}

_NAME_END