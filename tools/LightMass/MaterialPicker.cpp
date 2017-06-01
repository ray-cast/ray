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
#include "MaterialPicker.h"
#include <ray/input.h>
#include <ray/input_feature.h>
#include <ray/game_server.h>
#include <ray/camera_component.h>

_NAME_BEGIN

MaterialPicker::MaterialPicker() noexcept
{
}

MaterialPicker::~MaterialPicker() noexcept
{
}

void
MaterialPicker::onMessage(const MessagePtr& message) noexcept
{
	if (message->isInstanceOf<ray::InputMessage>())
	{
		auto inputFeature = ray::GameServer::instance()->getFeature<ray::InputFeature>();
		if (inputFeature)
		{
			auto input = inputFeature->getInput();
			if (!input)
				return;

			if (input->getButtonDown(ray::InputButton::LEFT))
			{
				if (input->getKey(ray::InputKey::LeftControl) && !input->isLockedCursor())
				{
					float x;
					float y;
					input->getMousePos(x, y);
				}
			}
		}
	}
}

void
MaterialPicker::onMaterialPicker(float x, float y) noexcept
{
	ray::Vector3 pos(x, y, 1);

	auto translate = this->getGameObject()->getTranslate();

	auto world = this->getComponent<ray::CameraComponent>()->screenToWorld(pos);
	auto view = ray::math::normalize(world - translate);
}

_NAME_END