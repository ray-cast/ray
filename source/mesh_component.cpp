// +----------------------------------------------------------------------
// | Project : ray.
// | All rights reserved.
// +----------------------------------------------------------------------
// | Copyright (c) 2013-2014.
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
#include <ray/mesh_component.h>

_NAME_BEGIN

__ImplementSubClass(MeshComponent, GameComponent)

MeshComponent::MeshComponent() noexcept
    : _type(VertexType::GPU_TRIANGLE)
{
}

MeshComponent::~MeshComponent() noexcept
{
}

void
MeshComponent::setMesh(MeshPropertyPtr mesh) noexcept
{
    _mesh = mesh;
}

void
MeshComponent::setBufferData(VertexBufferDataPtr vb, IndexBufferDataPtr ib) noexcept
{
    _vb = vb;
    _ib = ib;
    _mesh->clear();
}

void
MeshComponent::setCombieInstnace(const CombineInstance& instance) noexcept
{
    if (!_mesh)
        _mesh = std::make_shared<MeshProperty>();

    _mesh->setCombieInstnace(instance);
}

void
MeshComponent::clear() noexcept
{
    _mesh->clear();

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

MeshPropertyPtr
MeshComponent::getMesh() const noexcept
{
    return _mesh;
}

void
MeshComponent::setSharedMesh(MeshPropertyPtr mesh) noexcept
{
    _sharedMesh = mesh;
}

MeshPropertyPtr
MeshComponent::getSharedMesh() const noexcept
{
    return _sharedMesh;
}

std::size_t
MeshComponent::getNumVertices() const noexcept
{
    return _mesh->getNumVertices();
}

std::size_t
MeshComponent::getNumIndices() const noexcept
{
    return _mesh->getNumIndices();
}

void
MeshComponent::setRenderBuffer(RenderBufferPtr mesh) noexcept
{
    _renderMesh = mesh;
}

RenderBufferPtr
MeshComponent::getRenderBuffer() const noexcept
{
    return _renderMesh;
}

GameComponentPtr
MeshComponent::clone() const noexcept
{
    auto instance = std::make_shared<MeshComponent>();
    instance->setName(this->getName());
    instance->setVisible(this->getVisible());
    if (_mesh)
    {
        instance->setMesh(_mesh->clone());
        instance->setSharedMesh(_mesh);
    }

    return instance;
}

void
MeshComponent::onActivate() noexcept
{
    if (_mesh)
        buildRenderBuffer();
}

void
MeshComponent::onDeactivate() noexcept
{
    if (_renderMesh)
    {
        _renderMesh.reset();
        _renderMesh = nullptr;
    }
}

void
MeshComponent::buildMesh() noexcept
{
    auto numVertex = _mesh->getNumVertices();
    auto numIndex = _mesh->getNumIndices();

    if (numVertex == 0 || numIndex == 0)
        return;

    std::vector<VertexComponent> components;

    auto& vertices = _mesh->getVertexArray();
    if (!vertices.empty())
    {
        if (vertices.size() == numVertex)
            components.push_back(VertexComponent(VertexAttrib::GPU_ATTRIB_POSITION, VertexFormat::GPU_VERTEX_FLOAT3));
        else
            vertices.clear();
    }

    auto& normals = _mesh->getNormalArray();
    if (!normals.empty())
    {
        if (normals.size() == numVertex)
            components.push_back(VertexComponent(VertexAttrib::GPU_ATTRIB_NORMAL, VertexFormat::GPU_VERTEX_FLOAT3));
        else
            normals.clear();
    }

    auto& texcoords = _mesh->getTexcoordArray();
    if (!texcoords.empty())
    {
        if (texcoords.size() == numVertex)
            components.push_back(VertexComponent(VertexAttrib::GPU_ATTRIB_TEXCOORD, VertexFormat::GPU_VERTEX_FLOAT2));
        else
            texcoords.clear();
    }

    auto& faces = _mesh->getFaceArray();

    auto vb = std::make_shared<VertexBufferData>();
    auto ib = std::make_shared<IndexBufferData>();

    vb->setVertexComponents(components);
    vb->setup(numVertex, vb->getVertexByteSize(), VertexUsage::GPU_USAGE_STATIC);
    ib->setup(numIndex, IndexType::GPU_UINT32, VertexUsage::GPU_USAGE_STATIC);

    std::size_t offset = 0;
    std::size_t stride = vb->getVertexByteSize();
    if (!vertices.empty())
    {
        char* data = (char*)vb->data();
        for (auto& it : vertices)
        {
            std::memcpy(data, &it, sizeof(it));
            data += stride;
        }

        offset += sizeof(float3);
    }

    if (!normals.empty())
    {
        char* data = (char*)vb->data() + offset;
        for (auto& it : normals)
        {
            std::memcpy(data, &it, sizeof(it));
            data += stride;
        }

        offset += sizeof(float3);
    }

    if (!texcoords.empty())
    {
        char* data = (char*)vb->data() + offset;
        for (auto& it : texcoords)
        {
            std::memcpy(data, &it, sizeof(it));
            data += stride;
        }

        offset += sizeof(float2);
    }

    if (!faces.empty())
    {
        std::uint32_t* indices = (std::uint32_t*)ib->data();
        std::memcpy(indices, faces.data(), faces.size() * sizeof(std::uint32_t));
    }

    _bound = _mesh->getBoundingBox();

    _vb = vb;
    _ib = ib;
}

void
MeshComponent::buildRenderBuffer() noexcept
{
    if (!_vb && !_ib)
    {
        this->buildMesh();
    }

    if (_vb || _ib)
    {
        _renderMesh = std::make_unique<RenderBuffer>();
        _renderMesh->setup(_vb, _ib);
    }
}

const Bound&
MeshComponent::getBoundingBox() const noexcept
{
    return _bound;
}

VertexType
MeshComponent::getVertexType() const noexcept
{
    return _type;
}

_NAME_END