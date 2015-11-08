// +----------------------------------------------------------------------
// | Project : ray.
// | All rights reserved.
// +----------------------------------------------------------------------
// | Copyright (c) 2013-2015.
// +----------------------------------------------------------------------
// | * Redistribution and use of _renderThread software in source and binary forms,
// |   with or without modification, are permitted provided that the following
// |   conditions are met:
// |
// | * Redistributions of source code must retain the above
// |   copyright notice, _renderThread list of conditions and the
// |   following disclaimer.
// |
// | * Redistributions in binary form must reproduce the above
// |   copyright notice, _renderThread list of conditions and the
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
#if _BUILD_PHYSIC
#include <ray/physics_features.h>
#include <ray/physics_system.h>
#include <ray/physics_body_component.h>
#include <ray/physics_box_component.h>
#include <ray/physics_character_component.h>
#include <ray/game_scene.h>
#include <ray/game_server.h>

_NAME_BEGIN

PhysicFeatures::PhysicFeatures() noexcept
	: _physics(nullptr)
{
	this->setName(typeid(PhysicFeatures).name());
}

PhysicFeatures::~PhysicFeatures() noexcept
{
}

void
PhysicFeatures::onActivate() noexcept
{
	_physics = std::make_shared<PhysicsSystem>();
}

void
PhysicFeatures::onDeactivate() noexcept
{
	if (_physics)
	{
		_physics.reset();
		_physics = nullptr;
	}
}

void
PhysicFeatures::onFrameBegin() noexcept
{
	if (_physics)
		_physics->simulation(this->getGameServer()->getTimer()->delta());
}

void
PhysicFeatures::onOpenScene(GameScenePtr scene) except
{
	auto instance = scene->getInstanceID();

	if (_physicsScenes.size() < instance)
	{
		_physicsScenes.resize(instance * 2);
	}

	auto environment = scene->getEnvironment();

	PhysicsScene::Setting setting;
	setting.aabb = environment.aabb;
	setting.gravity = environment.gravity;
	setting.length = environment.length;
	setting.mass = environment.mass;
	setting.skinWidth = environment.skinWidth;
	setting.speed = environment.speed;

	_physicsScenes[instance] = std::make_shared<PhysicsScene>();
	_physicsScenes[instance]->setup(setting);

	_physics->addPhysicsScene(_physicsScenes[instance]);
}

void
PhysicFeatures::onCloseScene(GameScenePtr scene) except
{
	_physics->removePhysicsScene(this->getPhysicsScene(scene));

	_physicsScenes[scene->getInstanceID()] = nullptr;
}

PhysicsScenePtr
PhysicFeatures::getPhysicsScene(GameScene* scene) const noexcept
{
	return _physicsScenes[scene->getInstanceID()];
}

PhysicsScenePtr
PhysicFeatures::getPhysicsScene(GameScenePtr scene) const noexcept
{
	return _physicsScenes[scene->getInstanceID()];
}

GameFeaturePtr
PhysicFeatures::clone() const noexcept
{
	return std::make_shared<PhysicFeatures>();
}

GameComponentPtr
PhysicFeatures::instanceRigidbody(iarchive& reader) except
{
	if (reader.setToFirstChild())
	{
		auto body = std::make_shared<PhysicsBodyComponent>();

		do
		{
			auto key = reader.getCurrentNodeName();
			if (key == "attribute")
			{
				auto attributes = reader.getAttrs();
				for (auto& it : attributes)
				{
					std::string value = reader.getString(it);
					if (it == "sleep")
					{
						body->sleep(reader.getBoolean(it.c_str()));
					}
					else if (it == "mass")
					{
						body->setMass(reader.getFloat(it.c_str()));
					}
				}
			}
		} while (reader.setToNextChild());

		return body;
	}

	return nullptr;
}

GameComponentPtr
PhysicFeatures::instanceShapeBox(iarchive& reader) except
{
	if (reader.setToFirstChild())
	{
		auto box = std::make_shared<PhysicsBoxComponent>();

		do
		{
			auto key = reader.getCurrentNodeName();
			if (key == "attribute")
			{
				auto attributes = reader.getAttrs();
				for (auto& it : attributes)
				{
					std::string value = reader.getString(it);
					if (it == "size")
					{
						box->setSize(reader.getFloat3(it.c_str()));
					}
				}
			}
		} while (reader.setToNextChild());

		return box;
	}

	return nullptr;
}

_NAME_END
#endif