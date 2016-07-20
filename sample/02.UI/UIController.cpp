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
#include "uicontroller.h"
#include <ray/gui_feature.h>
#include <ray/game_server.h>

__ImplementSubClass(GuiControllerComponent, ray::GameComponent, "GuiController")

GuiControllerComponent::GuiControllerComponent() noexcept
{
	_f = 0.0f;
	_clearColor = ray::float4(114.f / 255.f, 144.f / 255.f, 154.f / 255.f);
	_showTestWindow = true;
	_showAnotherWindow = false;
}

GuiControllerComponent::~GuiControllerComponent() noexcept
{
}

ray::GameComponentPtr
GuiControllerComponent::clone() const noexcept
{
	return std::make_shared<GuiControllerComponent>();
}

void
GuiControllerComponent::onMessage(const ray::MessagePtr& message) noexcept
{
	if (message->isInstanceOf<ray::GuiMessage>())
	{
		float delta = ray::GameServer::instance()->getTimer()->delta();
		float fps = ray::GameServer::instance()->getTimer()->averageFps();

		auto& Gui = message->downcast<ray::GuiMessage>()->getGui();

		Gui.text("hello, world!");
		Gui.sliderFloat("float", &_f, 0.0f, 1.0f);
		Gui.colorEdit3("clear color", (float*)&_clearColor);
		if (Gui.button("Test Window")) _showTestWindow ^= 1;
		if (Gui.button("Another Window")) _showAnotherWindow ^= 1;
		Gui.text("Application average %f ms/frame (%f FPS)", std::abs(delta), fps);

		if (_showAnotherWindow)
		{
			Gui.setNextWindowSize(ray::float2(200, 100), ray::GuiSetCondFlagBits::GuiSetCondFlagFirstUseEverBit);
			Gui.begin("Another Window", &_showAnotherWindow);
			Gui.text("Hello");
			Gui.end();
		}

		if (_showTestWindow)
		{
			Gui.setNextWindowPos(ray::float2(650, 20), ray::GuiSetCondFlagBits::GuiSetCondFlagFirstUseEverBit);
			Gui.showTestWindow(&_showTestWindow);
		}
	}
}