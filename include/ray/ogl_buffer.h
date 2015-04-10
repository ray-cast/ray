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
#ifndef _H_OGL_GEOMETRY_H_
#define _H_OGL_GEOMETRY_H_

#include <ray/ogl_types.h>

_NAME_BEGIN

class OGLVertexBuffer final : public VertexBuffer
{
public:
    OGLVertexBuffer() noexcept;
    ~OGLVertexBuffer() noexcept;

    void setup(VertexBufferDataPtr vb) noexcept;
    void close() noexcept;

    void update(VertexBufferDataPtr vb) noexcept;

    void bindLayout() const noexcept;

    GLenum getVertexUsage() const noexcept;
    GLuint getVertexCount() const noexcept;
    GLsizeiptr getVertexSize() const noexcept;

private:

    GLuint _vbo;
    GLuint _vertexCount;
    GLenum _vertexUsage;
    GLsizeiptr _vertexSize;
};

class OGLIndexBuffer final : public IndexBuffer
{
public:
    OGLIndexBuffer() noexcept;
    ~OGLIndexBuffer() noexcept;

    void setup(IndexBufferDataPtr ib) noexcept;
    void close() noexcept;

    void update(IndexBufferDataPtr vb) noexcept;

    void bind() const noexcept;

    GLuint getIndexCount() const noexcept;
    GLenum getIndexType() const noexcept;
    GLenum getIndexUsage() const noexcept;
    GLsizeiptr getIndexSize() const noexcept;

private:

    GLuint _vbo;
    GLuint _indexCount;
    GLenum _indexType;
    GLenum _indexUsage;
    GLsizeiptr _indexSize;
};

class OGLConstantBuffer final
{
public:
    friend class OGLShaderParameter;
    friend class OGLShaderProgram;

public:
    OGLConstantBuffer() noexcept;
    OGLConstantBuffer(std::ptrdiff_t size);
    ~OGLConstantBuffer() noexcept;

    void resize(std::ptrdiff_t size) noexcept;

    void setData(std::ptrdiff_t offset, std::ptrdiff_t size, const void* buffer) noexcept;

    void bind(GLuint program, GLuint location) noexcept;
    void unbind() noexcept;

private:
    OGLConstantBuffer(const OGLConstantBuffer&) = delete;
    OGLConstantBuffer& operator=(const OGLConstantBuffer&) = delete;

private:

    GLuint _ubo;
    GLsizeiptr _size;
};

class OGLVertexArray final : public RenderBuffer
{
public:
    OGLVertexArray() noexcept;
    ~OGLVertexArray() noexcept;

    void setup(VertexBufferDataPtr vb, IndexBufferDataPtr ib) noexcept;
    void close() noexcept;

    void update(VertexBufferDataPtr vb, IndexBufferDataPtr ib) noexcept;

    void bind() const noexcept;
    void unbind() const noexcept;

    OGLIndexBufferPtr getIndexBuffer() noexcept;
    OGLVertexBufferPtr getVertexBuffer() noexcept;

private:

    mutable GLuint _vao;

    OGLVertexBufferPtr _vb;
    OGLIndexBufferPtr _ib;
};

_NAME_END

#endif