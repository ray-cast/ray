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
#include <ray/game_object_manager.h>
#include <ray/game_object.h>

#include <ray/res_loader.h>
#include <ray/ioserver.h>
#include <ray/mstream.h>
#include <ray/mesh_component.h>

_NAME_BEGIN

__ImplementSingleton(GameObjectManager)

GameObjectManager::GameObjectManager() noexcept
{
}

GameObjectManager::~GameObjectManager() noexcept
{
}

void
GameObjectManager::_instanceObject(GameObject* entity, std::uint32_t& instanceID) noexcept
{
	if (_emptyLists.empty())
		_instanceLists.push_back(entity);
	else
	{
		std::size_t _instanceID = _emptyLists.back();
		_emptyLists.pop_back();
		_instanceLists[_instanceID - 1] = entity;
	}

	instanceID = _instanceLists.size();
}

void
GameObjectManager::_unsetObject(GameObject* entity) noexcept
{
	auto instanceID = entity->getInstanceID();
	_instanceLists[instanceID - 1] = nullptr;
	_emptyLists.push_back(instanceID);
	this->_activeObject(entity, false);
}

void
GameObjectManager::_activeObject(GameObject* entity, bool active) noexcept
{
	if (active)
	{
		_activeActors.push_back(entity);
	}
	else
	{
		for (auto& actor : _activeActors)
		{
			if (actor == entity)
			{
				actor = nullptr;
				_hasEmptyActors = true;
				break;
			}
		}
	}
}

GameObjectPtr
GameObjectManager::findObject(const std::string& name) noexcept
{
	for (auto& it : _instanceLists)
	{
		if (!it)
			continue;

		if (it->getName() == name)
			return it->downcast_pointer<GameObject>();
	}

	return nullptr;
}

GameObjectPtr
GameObjectManager::findActiveObject(const std::string& name) noexcept
{
	for (auto& it : _activeActors)
	{
		if (!it)
			continue;

		if (it->getName() == name && it->getActive())
			return it->downcast_pointer<GameObject>();
	}

	return nullptr;
}

GameObjectPtr
GameObjectManager::instantiate(const std::string& name) noexcept
{
	auto object = this->findObject(name);
	if (object)
		return object->clone();
	return nullptr;
}

bool
GameObjectManager::activeObject(const std::string& name) noexcept
{
	for (auto& it : _instanceLists)
	{
		if (it)
		{
			if (it->getName() == name)
			{
				it->setActive(true);
				return true;
			}
		}
	}

	return false;
}

void
GameObjectManager::onFrameBegin() noexcept
{
	for (std::size_t i = 0; i < _activeActors.size(); i++)
	{
		if (_activeActors[i])
			_activeActors[i]->_onFrameBegin();
	}
}

void
GameObjectManager::onFrame() noexcept
{
	for (std::size_t i = 0; i < _activeActors.size(); i++)
	{
		if (_activeActors[i])
			_activeActors[i]->_onFrame();
	}
}

void
GameObjectManager::onFrameEnd() noexcept
{
	for (std::size_t i = 0; i < _activeActors.size(); i++)
	{
		if (_activeActors[i])
			_activeActors[i]->_onFrameEnd();
	}

	if (_hasEmptyActors)
	{
		for (auto it = _activeActors.begin(); it != _activeActors.end();)
		{
			if (!(*it))
				it = _activeActors.erase(it);
			else
				++it;
		}

		_hasEmptyActors = false;
	}
}

_NAME_END