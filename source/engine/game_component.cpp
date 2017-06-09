// +----------------------------------------------------------------------
// | Project : ray.
// | All rights reserved.
// +----------------------------------------------------------------------
// | Copyright (c) 2013-2017.
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
#include <ray/utf8.h>

_NAME_BEGIN

__ImplementSubInterface(GameComponent, MessageListener, "Component")

GameComponent::GameComponent() noexcept
	: _active(true)
	, _gameObject(nullptr)
{
}

GameComponent::GameComponent(const archivebuf& reader) noexcept
{
	this->load(reader);
}

GameComponent::~GameComponent() noexcept
{
}

GameComponentPtr
GameComponent::getComponent(const rtti::Rtti* type) const noexcept
{
	assert(this->rtti() != type);
	return _gameObject->getComponent(type);
}

GameComponentPtr
GameComponent::getComponent(const rtti::Rtti& type) const noexcept
{
	assert(this->rtti() != &type);
	return _gameObject->getComponent(type);
}

const GameComponents&
GameComponent::getComponents() const noexcept
{
	return _gameObject->getComponents();
}

void
GameComponent::addComponentDispatch(GameDispatchType type, const GameComponentPtr& component) noexcept
{
	assert(_gameObject && component);
	_gameObject->addComponentDispatch(type, component);
}

void
GameComponent::removeComponentDispatch(GameDispatchType type, const GameComponentPtr& component) noexcept
{
	assert(_gameObject && component);
	_gameObject->removeComponentDispatch(type, component);
}

void
GameComponent::addComponentDispatch(GameDispatchType type, GameComponent* component) noexcept
{
	assert(_gameObject && component);
	_gameObject->addComponentDispatch(type, component->cast_pointer<GameComponent>());
}

void
GameComponent::removeComponentDispatch(GameDispatchType type, GameComponent* component) noexcept
{
	assert(_gameObject && component);
	_gameObject->removeComponentDispatch(type, component->cast_pointer<GameComponent>());
}

void
GameComponent::_setGameObject(GameObject* gameobj) noexcept
{
	_gameObject = gameobj;
}

GameObjectPtr
GameComponent::getGameObject() const noexcept
{
	if (_gameObject)
		return _gameObject->cast_pointer<GameObject>();
	return nullptr;
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
GameComponent::setName(const util::string& name) noexcept
{
	_name = name;
}

void
GameComponent::setName(util::string&& name) noexcept
{
	_name = std::move(name);
}

const util::string&
GameComponent::getName() const noexcept
{
	return _name;
}

void
GameComponent::load(const archivebuf& reader) noexcept
{
	reader["name"] >> _name;
	reader["active"] >> _active;
}

void
GameComponent::save(archivebuf& write) noexcept
{
	write["name"] << _name;
	write["active"] << _active;
}

void
GameComponent::sendMessage(const MessagePtr& message) except
{
	auto parent = this->getGameObject()->getParent();
	while (parent)
	{
		auto temp = parent->getParent();
		if (!temp)
			break;
		else
			parent = temp;
	}

	GameComponent* ignore[1] = { this };

	auto raw_parent = parent;
	if (raw_parent)
		raw_parent->sendMessageDownwards(message, ignore, 1);
	else
	{
		auto& components = this->getGameObject()->getComponents();
		for (auto& it : components)
		{
			if (it.get() != this)
				it->onMessage(message);
		}
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
GameComponent::onAttach() except
{
}

void
GameComponent::onDetach() noexcept
{
}

void
GameComponent::onActivate() except
{
}

void
GameComponent::onDeactivate() noexcept
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

void
GameComponent::onAttachComponent(const GameComponentPtr& component) except
{
}

void
GameComponent::onDetachComponent(const GameComponentPtr& component) noexcept
{
}

_NAME_END