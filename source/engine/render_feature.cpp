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
#include <ray/render_feature.h>
#include <ray/render_scene.h>
#include <ray/render_system.h>

#include <ray/game_scene.h>
#include <ray/game_server.h>
#include <ray/game_application.h>
#include <ray/input_feature.h>

_NAME_BEGIN

__ImplementSubClass(RenderFeature, GameFeature, "RenderFeature")

RenderFeature::RenderFeature() noexcept
{
}

RenderFeature::RenderFeature(const RenderSetting& setting) noexcept
	: _renderSetting(setting)
{
}

RenderFeature::RenderFeature(WindHandle window, std::uint32_t w, std::uint32_t h, std::uint32_t dpi_w, std::uint32_t dpi_h) noexcept
{
	_renderSetting.window = window;
	_renderSetting.width = w;
	_renderSetting.height = h;
	_renderSetting.dpi_w = dpi_w;
	_renderSetting.dpi_h = dpi_h;
}

RenderFeature::~RenderFeature() noexcept
{
}

void
RenderFeature::setRenderSetting(const RenderSetting& setting) noexcept
{
	_renderSetting = setting;
}

const RenderSetting&
RenderFeature::getRenderSetting() const noexcept
{
	return _renderSetting;
}

RenderScenePtr
RenderFeature::getRenderScene() noexcept
{
	return _renderScene;
}

GameFeaturePtr
RenderFeature::clone() const noexcept
{
	return std::make_shared<RenderFeature>();
}

void
RenderFeature::onActivate() except
{
	if (!RenderSystem::instance()->setup(_renderSetting))
		throw failure("RenderSystem::instance() fail.");

	_renderScene = RenderSystem::instance()->createRenderScene();
	if (!_renderScene)
		throw failure("RenderSystem::createRenderScene() fail.");
}

void
RenderFeature::onDeactivate() noexcept
{
	RenderSystem::instance()->removeRenderScene(std::move(_renderScene));
	RenderSystem::instance()->close();
}

void 
RenderFeature::onMessage(const MessagePtr& message) except
{
	if (message->isInstanceOf<InputMessage>())
	{
		auto inputMessage = message->downcast<InputMessage>();
		auto& inputEvent = inputMessage->getEvent();

		if (InputEvent::SizeChange == inputEvent.event)
			RenderSystem::instance()->setWindowResolution(inputEvent.change.w, inputEvent.change.h);
	}
}

void
RenderFeature::onFrameBegin() noexcept
{
	RenderSystem::instance()->renderBegin();
}

void
RenderFeature::onFrameEnd() noexcept
{
	RenderSystem::instance()->render();
	RenderSystem::instance()->renderEnd();
}

_NAME_END