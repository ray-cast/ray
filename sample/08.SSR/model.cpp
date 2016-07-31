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
#include "model.h"
#include <ray/mesh_render_component.h>
#include <ray/mesh_component.h>
#include <ray/res_manager.h>

__ImplementSubClass(ModelComponent, GameComponent, "Model")

ModelComponent::ModelComponent() noexcept
{
}

ModelComponent::~ModelComponent() noexcept
{
}

void
ModelComponent::onActivate() noexcept
{
	auto stage = ray::ResManager::instance()->createGameObject("dlc:SSR/models/sibenik/sibenik.pmx");
	if (!stage)
		return;

	stage->setActive(true);
	stage->setScale(ray::float3(0.1, 0.1, 0.1));
	stage->setTranslate(ray::float3(0, 0, 5));
	stage->setQuaternion(ray::Quaternion(ray::float3::UnitY, -90));

	_objects.push_back(stage);
}

void
ModelComponent::onDeactivate() noexcept
{
	_objects.clear();
}

ray::GameComponentPtr
ModelComponent::clone() const noexcept
{
	return std::make_shared<ModelComponent>();
}