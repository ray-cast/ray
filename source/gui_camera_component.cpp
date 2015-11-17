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
#include <ray/material_maker.h>
#include <ray/render_factory.h>
#include <ray/render_buffer.h>
#include <ray/game_scene.h>
#include <ray/camera_component.h>

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
GUICameraComponent::onAttach() noexcept
{
}

void
GUICameraComponent::onDeactivate() noexcept
{
}

void 
GUICameraComponent::onFrameEnd() noexcept
{
	auto& children = this->getGameObject()->getChildren();
	for (auto& child : children)
	{
		if (!child->getActive())
			continue;

		auto control = child->getComponent<GUIBehaviourComponent>();
		if (control)
		{
			if (control->needUpdate())
				control->buildUIControl(nullptr);
		}
	}
}

GameComponentPtr 
GUICameraComponent::_hitTest(const Vector3& raycast) noexcept
{
	auto& children = this->getGameObject()->getChildren();
	for (auto& child : children)
	{
		if (!child->getActive())
			continue;

		auto control = child->getComponent<GUIBehaviourComponent>();
		if (control)
		{
			auto focus = control->hitTest(raycast);
			if (focus)
				return focus;
		}
	}

	return nullptr;
}

void 
GUICameraComponent::onMouseMotion(float x, float y) noexcept
{
	auto camera = this->getComponent<CameraComponent>();
	if (camera)
	{
		auto raycast = camera->screenToWorld(Vector3(x, y, 1.0));

		MouseMotion event;
		event.mouse.x = raycast.x;
		event.mouse.y = raycast.y;
		event.mouse.xrel = x;
		event.mouse.yrel = y;

		if (_mouseFocus)
		{
			_mouseFocus->getGameObject()->sendMessage(&event);

			MouseDrag event;
			event.mouse.x = raycast.x;
			event.mouse.y = raycast.y;
			event.mouse.xrel = x;
			event.mouse.yrel = y;

			_mouseFocus->getGameObject()->sendMessage(&event);
		}
		else
		{
			auto focus = this->_hitTest(raycast);
			if (_motionFocus != focus)
			{
				if (_motionFocus)
				{
					MouseOver event;
					event.mouse.x = raycast.x;
					event.mouse.y = raycast.y;
					event.mouse.xrel = x;
					event.mouse.yrel = y;

					_motionFocus->getGameObject()->sendMessage(&event);
				}

				if (focus)
				{
					focus->getGameObject()->sendMessage(&event);

					MouseEnter event;
					event.mouse.x = raycast.x;
					event.mouse.y = raycast.y;
					event.mouse.xrel = x;
					event.mouse.yrel = y;

					focus->getGameObject()->sendMessage(&event);
				}

				_motionFocus = focus;
			}
		}
	}
}

void 
GUICameraComponent::onMouseButtonDown(int button, float x, float y) noexcept
{
	auto camera = this->getComponent<CameraComponent>();
	if (camera)
	{
		auto raycast = camera->screenToWorld(Vector3(x, y, 1.0));

		auto focus = this->_hitTest(raycast);
		if (_mouseFocus && focus != _mouseFocus)
		{
				MouseButtonUp event;
				event.mouse.button = button;
				event.mouse.x = raycast.x;
				event.mouse.y = raycast.y;
				event.mouse.xrel = x;
				event.mouse.xrel = y;

				_mouseFocus->getGameObject()->sendMessage(&event);
		}
		else if (focus)
		{
			MouseButtonDown event;
			event.mouse.button = button;
			event.mouse.x = raycast.x;
			event.mouse.y = raycast.y;
			event.mouse.xrel = x;
			event.mouse.xrel = y;

			focus->getGameObject()->sendMessage(&event);

			_mouseFocus = focus;
		}
	}
}

void 
GUICameraComponent::onMouseButtonUp(int button, float x, float y) noexcept
{
	auto camera = this->getComponent<CameraComponent>();
	if (camera)
	{
		auto raycast = camera->screenToWorld(Vector3(x, y, 1.0));
		if (_mouseFocus)
		{
			MouseButtonUp event;
			event.mouse.button = button;
			event.mouse.x = raycast.x;
			event.mouse.y = raycast.y;
			event.mouse.xrel = x;
			event.mouse.xrel = y;

			_mouseFocus->getGameObject()->sendMessage(&event);
			_mouseFocus = nullptr;
		}
	}
}

void
GUICameraComponent::onRenderPre() noexcept
{
	WillRenderObjectEvent event;
	this->sendMessageDownwards(&event);
}

void
GUICameraComponent::onRenderPost() noexcept
{
	RenderObjectEvent event;
	this->sendMessageDownwards(&event);
}

_NAME_END
#endif