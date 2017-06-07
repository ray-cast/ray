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
#include <ray/game_object_manager.h>
#include <ray/game_object.h>

#include <ray/res_loader.h>
#include <ray/ioserver.h>
#include <ray/mstream.h>
#include <ray/mesh_component.h>

_NAME_BEGIN

__ImplementSingleton(GameObjectManager)

RaycastHit::RaycastHit() noexcept
	: object(0)
	, mesh(0)
	, distance(FLT_MAX)
{
}

GameObjectManager::GameObjectManager() noexcept
{
}

GameObjectManager::~GameObjectManager() noexcept
{
}

void
GameObjectManager::_instanceObject(GameObject* entity, std::size_t& instanceID) noexcept
{
	assert(entity);

	if (_emptyLists.empty())
	{
		_instanceLists.push_back(entity);
		instanceID = _instanceLists.size();
	}
	else
	{
		std::size_t _instanceID = _emptyLists.top();
		_emptyLists.pop();
		_instanceLists[_instanceID - 1] = entity;
		instanceID = _instanceID;
	}
}

void
GameObjectManager::_unsetObject(GameObject* entity) noexcept
{
	assert(entity);

	auto instanceID = entity->getInstanceID();
	_instanceLists[instanceID - 1] = nullptr;
	_emptyLists.push(instanceID);
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
GameObjectManager::findObject(const util::string& name) noexcept
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
GameObjectManager::findActiveObject(const util::string& name) noexcept
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
GameObjectManager::instantiate(const util::string& name) noexcept
{
	auto object = this->findObject(name);
	if (object)
		return object->clone();
	return nullptr;
}

bool
GameObjectManager::activeObject(const util::string& name) noexcept
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

std::size_t
GameObjectManager::raycastHit(const Raycast3& ray, RaycastHit& hit, std::function<bool(GameObject*)> comp) noexcept
{
	std::size_t result = 0;

	for (auto& object : _instanceLists)
	{
		if (!object)
			continue;

		if (!object->getActive())
			continue;

		auto component = object->getComponent<ray::MeshComponent>();
		if (!component)
			continue;

		auto mesh = component->getMesh();
		if (!mesh)
			continue;

		if (comp)
		{
			if (!comp(object))
				continue;
		}

		auto boundingBox = mesh->getBoundingBox();
		boundingBox.transform(object->getTransform());

		if (!boundingBox.intersects(ray))
			continue;

		auto& subsets = mesh->getMeshSubsets();
		for (std::size_t i = 0; i < subsets.size(); i++)
		{
			auto boundingBox = subsets[i].boundingBox;
			boundingBox.transform(object->getTransform());

			if (!boundingBox.intersects(ray))
				continue;

			float distance = ray::math::sqrDistance(boundingBox.center(), ray.origin);
			if (distance < hit.distance)
			{
				hit.object = object;
				hit.mesh = i;
				hit.distance = distance;

				result++;
			}
		}
	}

	return result;
}

std::size_t
GameObjectManager::raycastHit(const Vector3& orgin, const Vector3& end, RaycastHit& hit, std::function<bool(GameObject*)> comp) noexcept
{
	return this->raycastHit(Raycast3(orgin, end), hit, comp);
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