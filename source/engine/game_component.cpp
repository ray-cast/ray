// +----------------------------------------------------------------------
// | Project : ray.
// | All rights reserved.
// +----------------------------------------------------------------------
// | Copyright (c) 2013-2016.
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
GameComponent::_setGameObject(GameObject* gameobj) noexcept
{
	_gameObject = gameobj;
}

GameObjectPtr
GameComponent::getGameObject() const noexcept
{
	if (_gameObject)
		return _gameObject->downcast<GameObject>();
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
GameComponent::setName(const std::string& name) noexcept
{
	_name = name;
}

void
GameComponent::setName(std::string&& name) noexcept
{
	_name = std::move(name);
}

const std::string&
GameComponent::getName() const noexcept
{
	return _name;
}

void
GameComponent::load(iarchive& reader) noexcept
{
	bool active;

	if (reader.getValue("name", _name))
	{
		char buffer[4096];
		auto size = UTF8toGBK(buffer, 4096, _name.c_str(), _name.size());
		this->setName(std::string(buffer, size));
	}

	if (reader.getValue("active", active))
		this->setActive(active);
}

void
GameComponent::save(oarchive& write) noexcept
{
	write << make_archive(_name, "name");
	write << make_archive(_active, "name");
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
GameComponent::onParentChangeBefore() except
{
}

void
GameComponent::onParentChangeAfter() except
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
GameComponent::onAttachComponent(GameComponentPtr& component) except
{
}

void
GameComponent::onDetachComponent(GameComponentPtr& component) noexcept
{
}

_NAME_END