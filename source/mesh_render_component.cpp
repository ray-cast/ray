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
#include <ray/mesh_render_component.h>
#include <ray/mesh_component.h>
#include <ray/material_maker.h>
#include <ray/game_server.h>
#include <ray/render_features.h>

_NAME_BEGIN

__ImplementSubClass(MeshRenderComponent, GameComponent)

MeshRenderComponent::MeshRenderComponent() noexcept
{
}

MeshRenderComponent::~MeshRenderComponent() noexcept
{
}

GameComponentPtr
MeshRenderComponent::clone() const noexcept
{
    auto result = std::make_shared<MeshRenderComponent>();
    result->setMaterial(this->getMaterial());
    result->setCastShadow(this->getCastShadow());
    result->setReceiveShadow(this->getReceiveShadow());
    result->setName(this->getName());
    result->setVisible(this->getVisible());
    return result;
}

void
MeshRenderComponent::onActivate() except
{
    RenderComponent::onActivate();

    if (!_renderObject)
        _buildRenderObject();

    _attacRenderObject();
}

void
MeshRenderComponent::onDeactivate() noexcept
{
    RenderComponent::onDeactivate();

    if (_renderObject)
        _dttachRenderhObject();
}

void
MeshRenderComponent::onMoveAfter() noexcept
{
    RenderComponent::onMoveAfter();

    if (_renderObject)
    {
        _renderObject->setTransform(this->getGameObject()->getTransform());
        _renderObject->setTransformInverse(this->getGameObject()->getTransformInverse());
        _renderObject->setTransformInverseTranspose(this->getGameObject()->getTransformInverseTranspose());
    }
}

void
MeshRenderComponent::onLayerChangeAfter() noexcept
{
    RenderComponent::onLayerChangeAfter();

    if (_renderObject)
        _renderObject->setLayer(this->getGameObject()->getLayer());
}

void
MeshRenderComponent::_attacRenderObject() noexcept
{
    if (_renderObject)
    {
        auto renderer = this->getGameObject()->getGameServer()->getFeature<RenderFeatures>();
        if (renderer)
        {
            auto renderScene = renderer->getRenderScene(this->getGameObject()->getGameScene());
            if (renderScene)
            {
                _renderObject->setRenderScene(renderScene);
            }
        }
    }
}

void
MeshRenderComponent::_dttachRenderhObject() noexcept
{
    if (_renderObject)
    {
        _renderObject->setRenderScene(nullptr);
        _renderObject = nullptr;
    }
}

void
MeshRenderComponent::_buildRenderObject() noexcept
{
    auto mesh = this->getGameObject()->getComponent<MeshComponent>();
    if (mesh)
    {
        if (!mesh->getRenderBuffer())
        {
            mesh->buildRenderBuffer();
        }

        if (mesh->getRenderBuffer())
        {
            _renderObject = std::make_unique<Geometry>();
            _renderObject->setRenderBuffer(mesh->getRenderBuffer(), mesh->getVertexType());
            _renderObject->setBoundingBox(mesh->getBoundingBox());

            _renderObject->setMaterial(this->getMaterial());

            _renderObject->setCastShadow(this->getCastShadow());
            _renderObject->setReceiveShadow(this->getReceiveShadow());

            _renderObject->setLayer(this->getGameObject()->getLayer());

            _renderObject->setTransform(this->getGameObject()->getTransform());
            _renderObject->setTransformInverse(this->getGameObject()->getTransformInverse());
            _renderObject->setTransformInverseTranspose(this->getGameObject()->getTransformInverseTranspose());
        }
    }
}

_NAME_END