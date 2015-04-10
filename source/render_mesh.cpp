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
#include <ray/render_mesh.h>
#include <ray/render_impl.h>

_NAME_BEGIN

RenderMesh::RenderMesh() noexcept
    : _bufferVertex(nullptr)
    , _bufferIndex(nullptr)
{
}

RenderMesh::~RenderMesh() noexcept
{
    this->close();
}

void
RenderMesh::setup(const MeshProperty& mesh) noexcept
{
    auto numVertex = mesh.getNumVertices();
    auto numIndex = mesh.getNumIndices();

    std::vector<VertexComponent> components;

    auto& vertices = mesh.getVertexArray();
    if (!vertices.empty())
    {
        if (vertices.size() == numVertex)
            components.push_back(VertexComponent(VertexAttrib::GPU_ATTRIB_POSITION, VertexFormat::GPU_VERTEX_FLOAT3));
    }

    auto& normals = mesh.getNormalArray();
    if (!normals.empty())
    {
        if (normals.size() == numVertex)
            components.push_back(VertexComponent(VertexAttrib::GPU_ATTRIB_NORMAL, VertexFormat::GPU_VERTEX_FLOAT3));
    }

    auto& texcoords = mesh.getTexcoordArray();
    if (!texcoords.empty())
    {
        if (texcoords.size() == numVertex)
            components.push_back(VertexComponent(VertexAttrib::GPU_ATTRIB_TEXCOORD, VertexFormat::GPU_VERTEX_FLOAT2));
    }

    auto& faces = mesh.getFaceArray();

    auto vb = std::make_shared<VertexBufferData>();
    auto ib = std::make_shared<IndexBufferData>();

    vb->setVertexComponents(components);
    vb->setup(numVertex, vb->getVertexByteSize(), VertexUsage::GPU_USAGE_STATIC);
    ib->setup(numIndex, IndexType::GPU_UINT32, VertexUsage::GPU_USAGE_STATIC);

    std::size_t offset = 0;
    std::size_t stride = vb->getVertexByteSize();
    if (vertices.size() == numVertex)
    {
        char* data = (char*)vb->data();
        for (auto& it : vertices)
        {
            *(float3*)data = it;
            data += stride;
        }

        offset += sizeof(float3);
    }

    if (normals.size() == numVertex)
    {
        char* data = (char*)vb->data() + offset;
        for (auto& it : normals)
        {
            *(float3*)data = it;
            data += stride;
        }

        offset += sizeof(float3);
    }

    if (texcoords.size() == numVertex)
    {
        char* data = (char*)vb->data() + offset;
        for (auto& it : texcoords)
        {
            *(float2*)data = it;
            data += stride;
        }

        offset += sizeof(float2);
    }

    if (!faces.empty())
    {
        std::uint32_t* indices = (std::uint32_t*)ib->data();
        for (auto& face : faces)
        {
            *indices++ = face;
        }
    }

    this->setup(vb, ib);
}

void
RenderMesh::setup(VertexBufferDataPtr vb, IndexBufferDataPtr ib) noexcept
{
    assert(vb || ib);

    _bufferVertex = vb;
    _bufferIndex = ib;
    _bufferObject = RenderImpl::instance()->createRenderBuffer(vb, ib);
}

void
RenderMesh::close() noexcept
{
    if (_bufferVertex)
    {
        _bufferVertex.reset();
        _bufferVertex = nullptr;
    }

    if (_bufferIndex)
    {
        _bufferIndex.reset();
        _bufferIndex = nullptr;
    }

    if (_bufferObject)
    {
        RenderImpl::instance()->destroyRenderBuffer(_bufferObject);
        _bufferObject.reset();
        _bufferObject = 0;
    }
}

RenderBufferPtr
RenderMesh::getRenderBuffer() const noexcept
{
    return _bufferObject;
}

const VertexBufferDataPtr
RenderMesh::getVertexBuffer() const noexcept
{
    return _bufferVertex;
}

const IndexBufferDataPtr
RenderMesh::getIndexBuffer() const noexcept
{
    return _bufferIndex;
}

std::size_t
RenderMesh::getNumVertices() const noexcept
{
    if (_bufferVertex)
        return _bufferVertex->getVertexCount();
    return 0;
}

std::size_t
RenderMesh::getNumIndices() const noexcept
{
    if (_bufferIndex)
        return _bufferIndex->getIndexCount();
    return 0;
}

bool
RenderMesh::hasIndices() const noexcept
{
    return nullptr != _bufferIndex;
}

_NAME_END