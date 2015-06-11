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
#ifndef _H_RENDER_BUFFER_H_
#define _H_RENDER_BUFFER_H_

#include <ray/render_types.h>
#include <ray/model.h>

_NAME_BEGIN

enum VertexType
{
    GPU_POINT,
    GPU_LINE,
    GPU_TRIANGLE,
    GPU_FAN,
};

enum VertexUsage
{
    GPU_USAGE_STATIC,
    GPU_USAGE_DYNAMIC
};

enum VertexAttrib
{
    GPU_ATTRIB_POSITION,
    GPU_ATTRIB_NORMAL,
    GPU_ATTRIB_TEXCOORD,
    GPU_ATTRIB_DIFFUSE,
    GPU_ATTRIB_SPECULAR,
    GPU_ATTRIB_WEIGHT,
    GPU_ATTRIB_TANGENT,
    GPU_ATTRIB_BITANGENT,
    GPU_ATTRIB_NUMS,
};

enum VertexFormat
{
    GPU_VERTEX_FLOAT,
    GPU_VERTEX_FLOAT2,
    GPU_VERTEX_FLOAT3,
    GPU_VERTEX_FLOAT4,
};

enum VertexDataType
{
    GPU_DATATYPE_BYTE,
    GPU_DATATYPE_SHORT,
    GPU_DATATYPE_INT,
    GPU_DATATYPE_UNSIGNED_BYTE,
    GPU_DATATYPE_UNSIGNED_SHORT,
    GPU_DATATYPE_UNSIGNED_INT,
    GPU_DATATYPE_FLOAT,
};

enum IndexType
{
    GPU_UINT16,
    GPU_UINT32
};

class Renderable
{
public:
    VertexType type;

    std::size_t startVertice;
    std::size_t numVertices;
    std::size_t startIndice;
    std::size_t numIndices;
    std::size_t numInstances;

    Renderable() noexcept
        : type(VertexType::GPU_TRIANGLE)
        , startVertice(0)
        , numVertices(0)
        , startIndice(0)
        , numIndices(0)
        , numInstances(0)
    {
    }
};

class EXPORT VertexStreams final
{
public:
    VertexStreams() noexcept;
    ~VertexStreams() noexcept;

    void resize(std::size_t length) noexcept;
    void release() noexcept;

    char* data() const noexcept;
private:

    char* _data;
    std::size_t _length;
};

class EXPORT VertexComponent final
{
public:
    VertexComponent() noexcept;
    VertexComponent(VertexAttrib attrib, VertexFormat format, VertexDataType type = VertexDataType::GPU_DATATYPE_FLOAT) noexcept;
    ~VertexComponent() noexcept;

    VertexAttrib getVertexAttrib() const noexcept;
    VertexFormat getVertexFormat() const noexcept;
    VertexDataType getVertexDataType() const noexcept;

    int getVertexSize() const noexcept;
    int getVertexByteSize() const noexcept;

public:

    VertexAttrib _attrib;
    VertexFormat _format;
    VertexDataType _dataType;
};

class EXPORT VertexLayout final
{
public:
    void setVertexComponents(const VertexComponents& component) noexcept;
    const VertexComponents& getVertexComponents() const noexcept;

    std::size_t getVertexByteSize() const noexcept;

private:
    std::size_t _byteSize;
    VertexComponents _components;
};

class EXPORT VertexBuffer
{
public:
    VertexBuffer() noexcept;
    virtual ~VertexBuffer() noexcept;

    VertexLayout& getVertexLayout() noexcept;
    const VertexLayout& getVertexLayout() const noexcept;

private:

    VertexLayout _vertexLayout;
};

class EXPORT IndexBuffer
{
public:
    IndexBuffer() noexcept;
    virtual ~IndexBuffer() noexcept;

    void setIndexType(IndexType type) noexcept;
    IndexType getIndexType() const noexcept;

private:

    IndexType _type;
};

class EXPORT VertexBufferData final
{
public:
    VertexBufferData() noexcept;
    ~VertexBufferData() noexcept;

    void setup(std::size_t count, std::size_t size, VertexUsage usage) noexcept;
    void close() noexcept;

    void resize(std::size_t count);

    std::size_t getVertexCount() const noexcept;
    std::size_t getVertexSize() const noexcept;
    std::size_t getVertexByteSize() const noexcept;

    VertexUsage getVertexUsage() const noexcept;

    void* data() noexcept;
    const void* data() const noexcept;

    void setVertexComponents(const VertexComponents& components) noexcept;
    const VertexComponents& getVertexComponents() const noexcept;

private:
    std::size_t   _vertexCount;
    std::size_t   _vertexSize;
    VertexUsage   _vertexUsage;
    VertexStreams _vertexStreams;
    VertexLayout  _vertexLayout;
};

class EXPORT IndexBufferData final
{
public:
    IndexBufferData() noexcept;
    ~IndexBufferData() noexcept;

    void setup(std::size_t num, IndexType type, VertexUsage uages) noexcept;
    void close() noexcept;

    std::size_t getIndexCount() const noexcept;
    IndexType getIndexType() const noexcept;
    VertexUsage getIndexUsage() const noexcept;

    void* data() noexcept;
    const void* data() const noexcept;

private:
    std::size_t   _indexCount;
    IndexType     _indexType;
    VertexUsage   _indexUsage;
    VertexStreams _indexStreams;
};

class EXPORT RenderBuffer : public Object<RenderBuffer>
{
public:
    RenderBuffer() noexcept;
    ~RenderBuffer() noexcept;

    void setup(const MeshProperty& mesh) noexcept;
    void setup(VertexBufferDataPtr vb, IndexBufferDataPtr ib) noexcept;
    void close() noexcept;

    std::size_t getNumVertices() const noexcept;
    std::size_t getNumIndices() const noexcept;

    bool hasIndices() const noexcept;

    const VertexBufferDataPtr getVertexBuffer() const noexcept;
    const IndexBufferDataPtr getIndexBuffer() const noexcept;

    RenderBufferPtr clone() const noexcept;

private:
    RenderBuffer(const RenderBuffer& copy) noexcept = delete;
    RenderBuffer& operator=(const RenderBuffer&) noexcept = delete;

private:

    VertexBufferDataPtr _bufferVertex;
    IndexBufferDataPtr  _bufferIndex;
};

_NAME_END

#endif