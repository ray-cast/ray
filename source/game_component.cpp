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

__ImplementSubInterface(GameComponent, MessageListener, "Component")

GameComponent::GameComponent() noexcept
	: _active(false)
{
}

GameComponent::~GameComponent() noexcept
{
}

GameComponentPtr 
GameComponent::getComponent(const rtti::Rtti* type) const noexcept
{
	assert(this->rtti() != type);
	return this->getGameObject()->getComponent(type);
}

GameComponentPtr
GameComponent::getComponent(const rtti::Rtti& type) const noexcept
{
	assert(this->rtti() != &type);
	return this->getGameObject()->getComponent(type);
}

const GameComponents& 
GameComponent::getComponents() const noexcept
{
	return this->getGameObject()->getComponents();
}

void
GameComponent::_setGameObject(GameObject* gameobj) noexcept
{
	_gameObject = gameobj;
}

GameObjectPtr
GameComponent::getGameObject() noexcept
{
	return std::dynamic_pointer_cast<GameObject>(_gameObject->shared_from_this());
}

const GameObjectPtr
GameComponent::getGameObject() const noexcept
{
	return std::dynamic_pointer_cast<GameObject>(_gameObject->shared_from_this());
}

GameServer*
GameComponent::getGameServer() noexcept
{
	return this->getGameObject()->getGameServer();
}

const GameServer*
GameComponent::getGameServer() const noexcept
{
	return this->getGameObject()->getGameServer();
}

void 
GameComponent::setActive(bool active) except
{
	if (_active != active)
	{
		if (active)
			this->onActivate();
		else
			this->onDeactivate();

		_active = active;
	}
}

bool 
GameComponent::getActive() const noexcept
{
	return _active;
}

void
GameComponent::setName(const std::string& name) noexcept
{
	_name = name;
}

const std::string&
GameComponent::getName() const noexcept
{
	return _name;
}

void
GameComponent::load(iarchive& reader) noexcept
{
	std::string name;
	reader >> make_alias(name, "name");
	this->setName(name);
}

void
GameComponent::save(oarchive& write) noexcept
{
	auto& name = this->getName();
	write << make_alias(name, "name");
}

void
GameComponent::sendMessage(const MessagePtr& message) except
{
	auto& components = this->getGameObject()->getComponents();
	for (auto& it : components)
	{
		if (it.get() != this)
			it->onMessage(message);
	}
}

void
GameComponent::sendMessageUpwards(const MessagePtr& message) except
{
	auto& components = this->getGameObject()->getComponents();
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
GameComponent::sendMessageDownwards(const MessagePtr& message) except
{
	auto& components = this->getGameObject()->getComponents();
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
GameComponent::onAttachComponent() except
{
}

void 
GameComponent::onDetachComponent() except
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