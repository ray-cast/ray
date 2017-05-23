// +----------------------------------------------------------------------
// | Project : ray.
// | All rights reserved.
// +----------------------------------------------------------------------
// | Copyright (c) 2013-2016.
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
#include <ray/physics_features.h>
#include <ray/physics_system.h>

_NAME_BEGIN

__ImplementSubClass(PhysicFeatures, GameFeature, "PhysicFeatures")

PhysicFeatures::PhysicFeatures() noexcept
{
}

PhysicFeatures::~PhysicFeatures() noexcept
{
}

void
PhysicFeatures::onActivate() except
{
	if (this->getGameListener())
		this->getGameListener()->onMessage("GameServer : Starting : PhysicFeatures.");

	if (!PhysicsSystem::instance()->open())
		throw failure("PhysicsSystem::instance() fail.");

	if (this->getGameListener())
		this->getGameListener()->onMessage("GameServer : Started : PhysicFeatures.");
}

void
PhysicFeatures::onDeactivate() noexcept
{
	if (this->getGameListener())
		this->getGameListener()->onMessage("GameServer : Stopping : PhysicFeatures.");

	PhysicsSystem::instance()->close();

	if (this->getGameListener())
		this->getGameListener()->onMessage("GameServer : Stopped : PhysicFeatures.");
}

void
PhysicFeatures::onFrameEnd() noexcept
{
	PhysicsSystem::instance()->simulation(this->getGameServer()->getTimer()->delta());
}

GameFeaturePtr
PhysicFeatures::clone() const noexcept
{
	return std::make_shared<PhysicFeatures>();
}

_NAME_END