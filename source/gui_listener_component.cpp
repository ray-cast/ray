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
#include <ray/gui_listener_component.h>

_NAME_BEGIN

__ImplementSubInterface(GUIListenerComponent, GameComponent, "GUIListener")

GUIListenerComponent::GUIListenerComponent() noexcept
{
}

GUIListenerComponent::~GUIListenerComponent() noexcept
{
}

void 
GUIListenerComponent::onMessage(const MessagePtr& message) except
{
	auto event = message.event();
	if (event->code() == typeid(MouseMotion).hash_code())
	{
		MouseEvent motion = dynamic_cast<MouseMotion*>(event)->mouse;
		this->onMouseMotion(motion.x, motion.y);
	}
	else if (event->code() == typeid(MouseDrag).hash_code())
	{
		MouseEvent motion = dynamic_cast<MouseDrag*>(event)->mouse;
		this->onMouseDrag(motion.x, motion.y);
	}
	else if (event->code() == typeid(MouseEnter).hash_code())
	{
		MouseEvent motion = dynamic_cast<MouseEnter*>(event)->mouse;
		this->onMouseEnter(motion.x, motion.y);
	}
	else if (event->code() == typeid(MouseOver).hash_code())
	{
		MouseEvent motion = dynamic_cast<MouseOver*>(event)->mouse;
		this->onMouseOver(motion.x, motion.y);
	}
	else if (event->code() == typeid(MouseButtonDown).hash_code())
	{
		MouseEvent motion = dynamic_cast<MouseButtonDown*>(event)->mouse;
		this->onMouseButtonDown(motion.button, motion.x, motion.y);
	}
	else if (event->code() == typeid(MouseButtonUp).hash_code())
	{
		MouseEvent motion = dynamic_cast<MouseButtonUp*>(event)->mouse;
		this->onMouseButtonUp(motion.button, motion.x, motion.y);
	}
	else if (event->code() == typeid(WillRenderObjectEvent).hash_code())
	{
		this->onRenderPre();
	}
	else if (event->code() == typeid(RenderObjectEvent).hash_code())
	{
		this->onRenderPost();
	}
}

void 
GUIListenerComponent::onMouseDrag(float x, float y) noexcept
{
}

void 
GUIListenerComponent::onMouseEnter(float x, float y) noexcept
{
}

void 
GUIListenerComponent::onMouseOver(float x, float y) noexcept
{
}

void 
GUIListenerComponent::onMouseMotion(float x, float y) noexcept
{
}

void 
GUIListenerComponent::onMouseButtonDown(int button, float x, float y) noexcept
{
}

void 
GUIListenerComponent::onMouseButtonUp(int button, float x, float y) noexcept
{
}

void 
GUIListenerComponent::onRenderPre() noexcept
{
}

void 
GUIListenerComponent::onRenderPost() noexcept
{
}

_NAME_END
#endif