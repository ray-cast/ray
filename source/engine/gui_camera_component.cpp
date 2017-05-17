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
#include <ray/gui_camera_component.h>

#include <ray/game_server.h>
#include <ray/camera_component.h>

#include <ray/imgui.h>
#include <ray/imgui_system.h>

_NAME_BEGIN

__ImplementSubClass(GuiCameraComponent, GameComponent, "GuiCamera")

GuiCameraComponent::GuiCameraComponent() noexcept
	: _onPostRender(std::bind(&GuiCameraComponent::onPostRender, this, std::placeholders::_1))
	, _guiMessage(std::make_shared<GuiMessage>())
{
}

GuiCameraComponent::~GuiCameraComponent() noexcept
{
}

GameComponentPtr
GuiCameraComponent::clone() const noexcept
{
	return std::make_shared<GuiCameraComponent>();
}

void 
GuiCameraComponent::onActivate() noexcept
{
}

void 
GuiCameraComponent::onDeactivate() noexcept
{
}

void
GuiCameraComponent::onAttachComponent(GameComponentPtr& component) noexcept
{
	if (component->isInstanceOf<CameraComponent>())
		component->downcast<CameraComponent>()->addPostRenderListener(&_onPostRender);
}

void
GuiCameraComponent::onDetachComponent(GameComponentPtr& component) noexcept
{
	if (component->isInstanceOf<CameraComponent>())
		component->downcast<CameraComponent>()->removePostRenderListener(&_onPostRender);
}

void
GuiCameraComponent::onPostRender(const Camera& camera) noexcept
{
	IMGUI::newFrame();

	this->sendMessage(_guiMessage);

	IMGUI::render();

	IMGUISystem::instance()->render(GameServer::instance()->getTimer()->delta());
}

_NAME_END