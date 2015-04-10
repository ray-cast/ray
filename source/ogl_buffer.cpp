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
    , _vertexUsage(GL_STATIC_DRAW)
    , _vertexSize(0)
    , _vertexCount(0)
{
}

OGLVertexBuffer::~OGLVertexBuffer() noexcept
{
    this->close();
}

void
OGLVertexBuffer::setup(VertexBufferDataPtr vb) noexcept
{
    assert(!_vbo);
    assert(!vb->getVertexComponents().empty());

    _vertexUsage = OGLTypes::asOGLVertexUsage(vb->getVertexUsage());
    _vertexCount = vb->getVertexCount();
    _vertexSize = vb->getVertexSize() * _vertexCount;

    getVertexLayout().setVertexComponents(vb->getVertexComponents());

    glGenBuffers(1, &_vbo);
    glBindBuffer(GL_ARRAY_BUFFER, _vbo);
    glBufferData(GL_ARRAY_BUFFER, _vertexSize, vb->data(), _vertexUsage);
}

void
OGLVertexBuffer::close() noexcept
{
    if (_vbo)
    {
        glDeleteBuffers(1, &_vbo);
        _vbo = 0;
    }
}

void
OGLVertexBuffer::update(VertexBufferDataPtr vb) noexcept
{
    assert(_vbo);

    _vertexUsage = OGLTypes::asOGLVertexUsage(vb->getVertexUsage());
    _vertexCount = vb->getVertexCount();
    _vertexSize = vb->getVertexSize() * _vertexCount;

    glBindBuffer(GL_ARRAY_BUFFER, _vbo);
    glBufferData(GL_ARRAY_BUFFER, _vertexSize, vb->data(), _vertexUsage);
}

void
OGLVertexBuffer::bindLayout() const noexcept
{
    GLuint offset = 0;

    auto& layout = this->getVertexLayout();
    auto& components = layout.getVertexComponents();
    for (auto& it : components)
    {
        glEnableVertexAttribArray((GLuint)it.getVertexAttrib());
        glVertexAttribFormat(it.getVertexAttrib(), it.getVertexSize(), GL_FLOAT, GL_FALSE, 0);
        glVertexAttribBinding(it.getVertexAttrib(), it.getVertexAttrib());

        glBindVertexBuffer(it.getVertexAttrib(), _vbo, offset, layout.getVertexByteSize());

        offset += it.getVertexByteSize();
    }
}

GLenum
OGLVertexBuffer::getVertexUsage() const noexcept
{
    return _vertexUsage;
}

GLuint
OGLVertexBuffer::getVertexCount() const noexcept
{
    return _vertexCount;
}

GLsizeiptr
OGLVertexBuffer::getVertexSize() const noexcept
{
    return _vertexSize;
}

OGLIndexBuffer::OGLIndexBuffer() noexcept
    : _vbo(GL_NONE)
    , _indexType(GL_UNSIGNED_SHORT)
    , _indexUsage(GL_STATIC_DRAW)
    , _indexSize(0)
{
}

OGLIndexBuffer::~OGLIndexBuffer() noexcept
{
    this->close();
}

void
OGLIndexBuffer::setup(IndexBufferDataPtr ib) noexcept
{
    assert(!_vbo);

    _indexType = OGLTypes::asOGLIndexType(ib->getIndexType());
    _indexUsage = OGLTypes::asOGLVertexUsage(ib->getIndexUsage());
    _indexCount = ib->getIndexCount();

    _indexSize = 0;
    if (_indexType == GL_UNSIGNED_SHORT)
    {
        _indexSize = sizeof(std::int16_t) * ib->getIndexCount();
    }
    else if (_indexType == GL_UNSIGNED_INT)
    {
        _indexSize = sizeof(std::int32_t) * ib->getIndexCount();
    }
    else
    {
        assert(false);
    }

    glGenBuffers(1, &_vbo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _vbo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, _indexSize, ib->data(), _indexUsage);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void
OGLIndexBuffer::close() noexcept
{
    if (_vbo)
    {
        glDeleteBuffers(1, &_vbo);
        _vbo = 0;
    }
}

void
OGLIndexBuffer::update(IndexBufferDataPtr ib) noexcept
{
    _indexType = OGLTypes::asOGLIndexType(ib->getIndexType());
    _indexUsage = OGLTypes::asOGLVertexUsage(ib->getIndexUsage());
    _indexCount = ib->getIndexCount();

    _indexSize = 0;
    if (_indexType == GL_UNSIGNED_SHORT)
    {
        _indexSize = sizeof(std::int16_t) * ib->getIndexCount();
    }
    else if (_indexType == GL_UNSIGNED_INT)
    {
        _indexSize = sizeof(std::int32_t) * ib->getIndexCount();
    }
    else
    {
        assert(false);
    }

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _vbo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, _indexSize, ib->data(), _indexUsage);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void
OGLIndexBuffer::bind() const noexcept
{
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _vbo);
}

GLuint
OGLIndexBuffer::getIndexCount() const noexcept
{
    return _indexCount;
}

GLenum
OGLIndexBuffer::getIndexType() const noexcept
{
    return _indexType;
}

GLenum
OGLIndexBuffer::getIndexUsage() const noexcept
{
    return _indexUsage;
}

GLsizeiptr
OGLIndexBuffer::getIndexSize() const noexcept
{
    return _indexSize;
}

OGLConstantBuffer::OGLConstantBuffer() noexcept
    : _ubo(0)
    , _size(0)
{
    glGenBuffers(1, &_ubo);
}

OGLConstantBuffer::OGLConstantBuffer(std::ptrdiff_t size)
    : _ubo(0)
    , _size(size)
{
    glGenBuffers(1, &_ubo);
}

OGLConstantBuffer::~OGLConstantBuffer() noexcept
{
    if (_ubo)
    {
        glDeleteBuffers(1, &_ubo);
        _ubo = 0;
    }
}

void
OGLConstantBuffer::resize(std::ptrdiff_t size) noexcept
{
    assert(size > 0);

    glBindBuffer(GL_UNIFORM_BUFFER, _ubo);
    glBufferData(GL_UNIFORM_BUFFER, size, 0, GL_DYNAMIC_DRAW);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);

    _size = size;
}

void
OGLConstantBuffer::setData(std::ptrdiff_t offset, std::ptrdiff_t size, const void* buffer) noexcept
{
    assert((offset + size) <= _size);
    glBufferSubData(GL_UNIFORM_BUFFER, offset, size, buffer);
}

void
OGLConstantBuffer::bind(GLuint program, GLuint location) noexcept
{
    glBindBuffer(GL_UNIFORM_BUFFER, _ubo);
    glBindBufferBase(GL_UNIFORM_BUFFER, location, _ubo);
    glUniformBlockBinding(program, location, _ubo);
}

void
OGLConstantBuffer::unbind() noexcept
{
    glBindBuffer(GL_UNIFORM_BUFFER, GL_NONE);
}

OGLVertexArray::OGLVertexArray() noexcept
    : _vao(GL_NONE)
{
}

OGLVertexArray::~OGLVertexArray() noexcept
{
    this->close();
}

void
OGLVertexArray::setup(VertexBufferDataPtr vb, const IndexBufferDataPtr ib) noexcept
{
    assert(!_vao);

    glGenVertexArrays(1, &_vao);
    glBindVertexArray(_vao);

    if (vb)
    {
        _vb = std::make_shared<OGLVertexBuffer>();
        _vb->setup(vb);
        _vb->bindLayout();
    }

    if (ib)
    {
        _ib = std::make_shared<OGLIndexBuffer>();
        _ib->setup(ib);
        _ib->bind();
    }

    glBindVertexArray(0);
}

void
OGLVertexArray::close() noexcept
{
    if (_vao)
    {
        glDeleteVertexArrays(1, &_vao);
        _vao = 0;
    }

    _vb.reset();
    _ib.reset();
}

void
OGLVertexArray::update(VertexBufferDataPtr vb, IndexBufferDataPtr ib) noexcept
{
    if (vb && _vb)
    {
        _vb->update(vb);
    }

    if (ib && _ib)
    {
        _ib->update(ib);
    }
}

void
OGLVertexArray::bind() const noexcept
{
    glBindVertexArray(_vao);
}

void
OGLVertexArray::unbind() const noexcept
{
    glBindVertexArray(0);
}

OGLVertexBufferPtr
OGLVertexArray::getVertexBuffer() noexcept
{
    return _vb;
}

OGLIndexBufferPtr
OGLVertexArray::getIndexBuffer() noexcept
{
    return _ib;
}

_NAME_END