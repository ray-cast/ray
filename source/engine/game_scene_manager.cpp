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
#include <ray/game_scene_manager.h>
#include <ray/game_scene.h>

_NAME_BEGIN

__ImplementSingleton(GameSceneManager)

GameSceneManager::GameSceneManager() noexcept
{
}

GameSceneManager::~GameSceneManager() noexcept
{
}

bool
GameSceneManager::open() noexcept
{
	return true;
}

void 
GameSceneManager::close() noexcept
{
}

void
GameSceneManager::_instanceScene(GameScene* entity, std::size_t& instanceID) noexcept
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
GameSceneManager::_unsetScene(GameScene* entity) noexcept
{
	auto instanceID = entity->getInstanceID();
	_instanceLists[instanceID - 1] = nullptr;
	_emptyLists.push_back(instanceID);
	this->_activeScene(entity, false);
}

void
GameSceneManager::_activeScene(GameScene* entity, bool active) noexcept
{
	if (active)
	{
		_activeActors.push_back(entity);
	}
	else
	{
		std::size_t size = _activeActors.size();
		for (std::size_t i = 0; i < size; i++)
		{
			if (_activeActors[i] == entity)
			{
				_activeActors[i] = nullptr;
				_hasEmptyActors = true;
				break;
			}
		}
	}
}

GameScenePtr
GameSceneManager::findScene(const std::string& name) noexcept
{
	for (auto& it : _instanceLists)
	{
		if (!it)
			continue;

		if (it->getName() == name)
			return it->downcast_pointer<GameScene>();
	}

	return nullptr;
}

GameScenePtr
GameSceneManager::findActiveScene(const std::string& name) noexcept
{
	for (auto& it : _activeActors)
	{
		if (!it)
			continue;

		if (it->getName() == name && it->getActive())
			return it->downcast_pointer<GameScene>();
	}

	return nullptr;
}

GameScenePtr
GameSceneManager::instantiate(const std::string& name) except
{
	auto scene = this->findScene(name);
	if (scene)
		return scene->clone();
	return nullptr;
}

bool
GameSceneManager::activeScene(const std::string& name) noexcept
{
	for (auto& it : _instanceLists)
	{
		if (!it)
			continue;

		if (it->getName() == name)
		{
			it->setActive(true);
			return true;
		}
	}

	return false;
}

void
GameSceneManager::onFrameBegin() noexcept
{
}

void
GameSceneManager::onFrame() noexcept
{
}

void
GameSceneManager::onFrameEnd() noexcept
{
}

_NAME_END