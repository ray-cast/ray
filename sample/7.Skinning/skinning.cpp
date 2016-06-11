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
#include "skinning.h"
#include <ray/mesh_render_component.h>
#include <ray/mesh_component.h>
#include <ray/res_manager.h>

__ImplementSubClass(SkinningComponent, GameComponent, "Skinning")

SkinningComponent::SkinningComponent() noexcept
{
}

SkinningComponent::~SkinningComponent() noexcept
{
}

void
SkinningComponent::onActivate() except
{
	auto stage = ray::ResManager::instance()->createGameObject("dlc:skinning/models/PDF 活动小丑/stage.pmx");
	if (!stage)
		return;

	stage->setTranslate(ray::float3(0, 0, -2.5));
	stage->setActive(true);

	//auto gameObject = ray::ResManager::instance()->createGameObject("dlc:skinning/models/miku/miku.pmd", "dlc:skinning/animotion/愛言葉.vmd");
	auto gameObject = ray::ResManager::instance()->createGameObject("dlc:skinning/models/PDF 活动小丑/活动小丑.pmx", "dlc:skinning/animotion/愛言葉.vmd");
	gameObject->setActive(true);

	_objects.push_back(stage);
	_objects.push_back(gameObject);
}

void
SkinningComponent::onDeactivate() noexcept
{
	_objects.clear();
}

ray::GameComponentPtr
SkinningComponent::clone() const noexcept
{
	return std::make_shared<SkinningComponent>();
}