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
#include <ray/physics_system.h>

_NAME_BEGIN

__ImplementSingleton(PhysicsSystem)

PhysicsSystem::PhysicsSystem() noexcept
{
}

PhysicsSystem::~PhysicsSystem() noexcept
{
}

bool
PhysicsSystem::open() noexcept
{
	PhysicsScene::Setting setting;
	setting.aabb.min = Vector3(-1000, -1000, -1000);
	setting.aabb.max = Vector3(1000, 1000, 1000);
	setting.gravity = Vector3(0.0f, 9.81f, 0.0f);
	setting.length = 1.0f;
	setting.mass = 1000.0f;
	setting.skinWidth = 0.0001f;
	setting.speed = 10.0f;

	_scene = std::make_shared<PhysicsScene>();
	_scene->setup(setting);
	return true;
}

void
PhysicsSystem::close() noexcept
{
}

PhysicsScenePtr
PhysicsSystem::getPhysicsScene() noexcept
{
	return _scene;
}

void
PhysicsSystem::simulation(float delta) noexcept
{
	if (_scene)
		_scene->simulation(delta);
}

_NAME_END