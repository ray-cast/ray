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
#if defined(_BUILD_GUI)
#include <ray/gui_camera_component.h>
#include <ray/gui_behaviour_component.h>
#include <ray/gui_feature.h>
#include <ray/camera_component.h>
#include <ray/game_server.h>

_NAME_BEGIN

__ImplementSubClass(GUICameraComponent, GUIListenerComponent, "GUICamera")

GUICameraComponent::GUICameraComponent() noexcept
{
}

GUICameraComponent::~GUICameraComponent() noexcept
{
}

GameComponentPtr 
GUICameraComponent::clone() const except
{
	return std::make_shared<GUICameraComponent>();
}

void
GUICameraComponent::onAttachComponent() noexcept
{
	auto camera = this->getComponent<CameraComponent>();
	if (camera)
	{
		camera->addRenderListener(this);
	}
}

void
GUICameraComponent::onRemoveComponent() noexcept
{
}

void
GUICameraComponent::onActivate() noexcept
{
	auto camera = this->getComponent<CameraComponent>();
	if (camera)
	{
		camera->addRenderListener(this);
	}
}

void
GUICameraComponent::onDectivate() noexcept
{
}

void 
GUICameraComponent::onWillRenderObject(const Camera& camera) noexcept
{
}

void 
GUICameraComponent::onRenderObject(const Camera& camera) noexcept
{
	auto platform = this->getGameServer()->getFeature<GuiFeature>();
	if (platform)
	{
		platform->render();
	}
}

_NAME_END
#endif