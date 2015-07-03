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
#include <ray/game_component.h>

_NAME_BEGIN

__ImplementInterface(GameComponent)

GameComponent::GameComponent() noexcept
    : _gameObject(nullptr)
{
}

GameComponent::~GameComponent() noexcept
{
}

void
GameComponent::_setGameObject(GameObject* gameobj) noexcept
{
    _gameObject = gameobj;
}

GameObject*
GameComponent::getGameObject() noexcept
{
    return _gameObject;
}

const GameObject*
GameComponent::getGameObject() const noexcept
{
    return _gameObject;
}

GameServer*
GameComponent::getGameServer() noexcept
{
    return _gameObject->getGameServer();
}

const GameServer*
GameComponent::getGameServer() const noexcept
{
    return _gameObject->getGameServer();
}

void
GameComponent::load(iarchive& reader) noexcept
{
    reader >> static_cast<GameListener*>(this);
}

void
GameComponent::save(oarchive& write) noexcept
{
}

void
GameComponent::sendMessage(const GameMessage& message) except
{
    if (!_gameObject->getActive())
        return;

    auto& components = _gameObject->getComponents();
    for (auto& it : components)
    {
        if (it.get() != this)
            it->onMessage(message);
    }
}

void
GameComponent::sendMessageUpwards(const GameMessage& message) except
{
    if (!_gameObject->getActive())
        return;

    auto& components = _gameObject->getComponents();
    for (auto& it : components)
    {
        if (it.get() != this)
            it->onMessage(message);
    }

    auto parent = this->getGameObject()->getParent();
    if (parent)
    {
        parent->sendMessageUpwards(message);
    }
}

void
GameComponent::sendMessageDownwards(const GameMessage& message) except
{
    if (!_gameObject->getActive())
        return;

    auto& components = _gameObject->getComponents();
    for (auto& it : components)
    {
        if (it.get() != this)
            it->onMessage(message);
    }

    auto& children = this->getGameObject()->getChildren();
    for (auto& it : children)
    {
        it->sendMessageDownwards(message);
    }
}

void
GameComponent::onAttach() except
{
}

void
GameComponent::onRemove() except
{
}

void
GameComponent::onActivate() except
{
}

void
GameComponent::onDeactivate() except
{
}

void
GameComponent::onFrameBegin() except
{
}

void
GameComponent::onFrame() except
{
}

void
GameComponent::onFrameEnd() except
{
}

void
GameComponent::onMoveBefore() except
{
}

void
GameComponent::onMoveAfter() except
{
}

void
GameComponent::onLayerChangeBefore() except
{
}

void
GameComponent::onLayerChangeAfter() except
{
}

_NAME_END