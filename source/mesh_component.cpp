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
#include <ray/model.h>
#include <ray/render_factory.h>

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
    _bound = _mesh->getBoundingBox();
}

void
MeshComponent::setCombieInstnace(const CombineInstance& instance) noexcept
{
    if (!_mesh)
        _mesh = std::make_shared<MeshProperty>();

    _mesh->setCombieInstnace(instance);
    _bound = _mesh->getBoundingBox();
}

void
MeshComponent::clear() noexcept
{
    _mesh->clear();
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

void
MeshComponent::load(iarchive& reader) noexcept
{
    reader >> static_cast<GameComponent*>(this);

    Model model;
    if (model.load(this->getName()))
    {
        if (model.hasMeshes())
        {
            this->setMesh(model.getMeshsList()[0]);
        }
    }
}

void
MeshComponent::save(oarchive& write) noexcept
{
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
    if (_mesh)
    {
        _mesh.reset();
        _mesh = nullptr;
    }

    if (_renderMesh)
    {
        _renderMesh.reset();
        _renderMesh = nullptr;
    }
}

void
MeshComponent::buildRenderBuffer() noexcept
{
    if (!_renderMesh)
    {
        _renderMesh = RenderFactory::createRenderBuffer(*_mesh);
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