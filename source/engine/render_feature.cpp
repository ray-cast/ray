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
#if defined(_BUILD_RENDERER)
#include <ray/render_feature.h>
#include <ray/render_scene.h>
#include <ray/render_system.h>

#include <ray/game_scene.h>
#include <ray/game_server.h>
#include <ray/game_application.h>

_NAME_BEGIN

__ImplementSubClass(RenderFeature, GameFeature, "Graphics")

RenderFeature::RenderFeature() noexcept
{
}

RenderFeature::RenderFeature(const RenderSetting& setting) noexcept
	: _renderSetting(setting)
{
}

RenderFeature::RenderFeature(WindHandle window, std::uint32_t w, std::uint32_t h) noexcept
{
	_renderSetting.window = window;
	_renderSetting.width = w;
	_renderSetting.height = h;
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
RenderFeature::getRenderScene(GameScene* scene) noexcept
{
	return _renderScenes[scene->getInstanceID()];
}

RenderScenePtr
RenderFeature::getRenderScene(GameScenePtr scene) noexcept
{
	return _renderScenes[scene->getInstanceID()];
}

GameFeaturePtr
RenderFeature::clone() const noexcept
{
	return std::make_shared<RenderFeature>();
}

void
RenderFeature::onActivate() except
{
	RenderSystem::instance()->setup(_renderSetting);
}

void
RenderFeature::onDeactivate() except
{
	RenderSystem::instance()->close();
}

void
RenderFeature::onOpenScene(GameScenePtr scene) except
{
	if (!_renderScenes[scene->getInstanceID()])
	{
		auto renderScene = std::make_shared<RenderScene>();
		_renderScenes[scene->getInstanceID()] = renderScene;

		RenderSystem::instance()->addRenderScene(renderScene);
	}
}

void
RenderFeature::onCloseScene(GameScenePtr scene) except
{
	auto renderScene = _renderScenes[scene->getInstanceID()];
	if (renderScene)
	{
		RenderSystem::instance()->removeRenderScene(renderScene);
		_renderScenes[scene->getInstanceID()] = nullptr;
	}
}

void
RenderFeature::onWindowSizeChange() except
{
	std::uint32_t w, h;
	this->getGameServer()->getGameApp()->getWindowResolution(w, h);
	RenderSystem::instance()->setWindowResolution(w, h);
}

void
RenderFeature::onFrameBegin() except
{
	RenderSystem::instance()->renderBegin();
}

void
RenderFeature::onFrame() except
{
}

void
RenderFeature::onFrameEnd() except
{
	RenderSystem::instance()->render();
	RenderSystem::instance()->renderEnd();
}

_NAME_END

#endif