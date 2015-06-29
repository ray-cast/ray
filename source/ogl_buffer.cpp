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
{
}

OGLVertexBuffer::~OGLVertexBuffer() noexcept
{
    this->close();
}

void
OGLVertexBuffer::setup() except
{
    auto vertexUsage = OGLTypes::asOGLVertexUsage(this->getVertexUsage());
    auto vertexDataSize = this->getVertexDataSize();
    auto vertexByteSize = this->getVertexSize();

    glGenBuffers(1, &_vbo);
#if !defined(EGLAPI)
    if (OGLExtenstion::isSupport(OGLFeatures::ARB_direct_state_access))
    {
        GLbitfield flags = GL_MAP_READ_BIT | GL_MAP_PERSISTENT_BIT | GL_MAP_COHERENT_BIT;
        if (vertexUsage == GL_DYNAMIC_DRAW)
        {
            flags |= GL_MAP_WRITE_BIT;
            flags |= GL_DYNAMIC_STORAGE_BIT;
        }

        glNamedBufferStorageEXT(_vbo, vertexDataSize, this->data(), flags);
    }
    else
#endif
    {
        glBindBuffer(GL_ARRAY_BUFFER, _vbo);
        glBufferData(GL_ARRAY_BUFFER, vertexDataSize, this->data(), vertexUsage);
    }

    GLuint offset = 0;

    auto& components = this->getVertexComponents();
    for (auto& it : components)
    {
        auto type = OGLTypes::asOGLVertexFormat(it.getVertexFormat());

        if (OGLExtenstion::isSupport(NV_vertex_buffer_unified_memory))
        {
            glBindVertexBuffer(it.getVertexAttrib(), _vbo, offset, vertexByteSize);
        }
        else if (OGLExtenstion::isSupport(ARB_vertex_attrib_binding))
        {
            glEnableVertexAttribArray((GLuint)it.getVertexAttrib());
            glVertexAttribFormat(it.getVertexAttrib(), it.getVertexCount(), type, GL_FALSE, 0);
            glVertexAttribBinding(it.getVertexAttrib(), it.getVertexAttrib());

            glBindVertexBuffer(it.getVertexAttrib(), _vbo, offset, vertexByteSize);
        }
        else
        {
            if (OGLExtenstion::isSupport(ARB_vertex_array_object))
            {
                glEnableVertexAttribArray((GLuint)it.getVertexAttrib());
                glVertexAttribPointer(it.getVertexAttrib(), it.getVertexCount(), type, GL_FALSE, vertexByteSize, (const char*)nullptr + offset);
            }
        }

        offset += it.getVertexSize();
    }

    if (OGLExtenstion::isSupport(NV_vertex_buffer_unified_memory))
    {
        glGetNamedBufferParameterui64vNV(_vbo, GL_BUFFER_GPU_ADDRESS_NV, &_bindlessVbo);
        glMakeNamedBufferResidentNV(_vbo, GL_READ_ONLY);
    }
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

OGLIndexBuffer::OGLIndexBuffer() noexcept
    : _ibo(GL_NONE)
    , _bindlessIbo(GL_NONE)
{
}

OGLIndexBuffer::~OGLIndexBuffer() noexcept
{
    this->close();
}

void
OGLIndexBuffer::setup() noexcept
{
    auto indexType = OGLTypes::asOGLIndexType(this->getIndexType());
    auto indexUsage = OGLTypes::asOGLVertexUsage(this->getIndexUsage());
    auto indexCount = this->getIndexCount();
    auto indexDataSize = this->getIndexDataSize();

    glGenBuffers(1, &_ibo);
#if !defined(EGLAPI)
    if (OGLExtenstion::isSupport(OGLFeatures::ARB_direct_state_access))
    {
        GLbitfield flags = GL_MAP_READ_BIT | GL_MAP_PERSISTENT_BIT | GL_MAP_COHERENT_BIT;
        if (indexUsage == GL_DYNAMIC_DRAW)
        {
            flags |= GL_MAP_WRITE_BIT;
            flags |= GL_DYNAMIC_STORAGE_BIT;
        }

        glNamedBufferStorageEXT(_ibo, indexDataSize, this->data(), flags);
    }
    else
#endif
    {
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _ibo);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexDataSize, this->data(), indexUsage);
    }

    if (OGLExtenstion::isSupport(NV_vertex_buffer_unified_memory))
    {
        glGetNamedBufferParameterui64vNV(_ibo, GL_BUFFER_GPU_ADDRESS_NV, &_bindlessIbo);
        glMakeNamedBufferResidentNV(_ibo, GL_READ_ONLY);
    }
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

    if (!OGLExtenstion::isSupport(NV_vertex_buffer_unified_memory))
    {
        if (OGLExtenstion::isSupport(ARB_vertex_array_object))
        {
            glGenVertexArrays(1, &_vao);
            glBindVertexArray(_vao);
        }
    }

    if (vb)
    {
        std::dynamic_pointer_cast<OGLVertexBuffer>(vb)->setup();
    }

    if (ib)
    {
        std::dynamic_pointer_cast<OGLIndexBuffer>(ib)->setup();
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

_NAME_END