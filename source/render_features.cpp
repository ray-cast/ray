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
#include <ray/render_features.h>
#include <ray/render_factory.h>
#include <ray/render_scene.h>
#include <ray/game_scene.h>

_NAME_BEGIN

__ImplementSubClass(RenderFeatures, GameFeature, "Graphics")

RenderFeatures::RenderFeatures() noexcept
	: _hwnd(nullptr)
	, _width(0)
	, _height(0)
{
	_renderSystem = RenderFactory::createRenderSystem();
}

RenderFeatures::RenderFeatures(WindHandle hwnd, std::size_t w, std::size_t h) noexcept
	: _hwnd(hwnd)
	, _width(w)
	, _height(h)
{
	_renderSystem = RenderFactory::createRenderSystem();
}

RenderFeatures::~RenderFeatures() noexcept
{
}

void
RenderFeatures::setRenderWindow(WindHandle hwnd, std::size_t w, std::size_t h) noexcept
{
	_width = w;
	_height = h;
	_hwnd = hwnd;
}

WindHandle
RenderFeatures::getRenderWindow() const noexcept
{
	return _hwnd;
}

void
RenderFeatures::setRenderSetting(const RenderSetting& setting) noexcept
{
	_renderSetting = setting;
}

const RenderSetting&
RenderFeatures::getRenderSetting() const noexcept
{
	return _renderSetting;
}

void
RenderFeatures::setRenderSystem(RenderSystemPtr renderSystem) noexcept
{
	_renderSystem = renderSystem;
}

RenderSystemPtr
RenderFeatures::getRenderSystem() const noexcept
{
	return _renderSystem;
}

RenderScenePtr
RenderFeatures::getRenderScene(GameScene* scene) noexcept
{
	return _renderScenes[scene->getInstanceID()];
}

RenderScenePtr
RenderFeatures::getRenderScene(GameScenePtr scene) noexcept
{
	return _renderScenes[scene->getInstanceID()];
}

GameFeaturePtr
RenderFeatures::clone() const noexcept
{
	return std::make_shared<RenderFeatures>();
}

void
RenderFeatures::onActivate() except
{
	_renderSystem->open(_hwnd, _width, _height);
	_renderSystem->setRenderSetting(_renderSetting);

	for (auto& it : _renderScenes)
		_renderSystem->addRenderScene(it.second);
}

void
RenderFeatures::onDeactivate() except
{
	if (_renderSystem)
	{
		_renderSystem.reset();
		_renderSystem = nullptr;
	}
}

void
RenderFeatures::onOpenScene(GameScenePtr scene) except
{
	if (!_renderScenes[scene->getInstanceID()])
	{
		auto renderScene = std::make_shared<RenderScene>();
		_renderScenes[scene->getInstanceID()] = renderScene;

		if (_renderSystem)
			_renderSystem->addRenderScene(renderScene);
	}
}

void
RenderFeatures::onCloseScene(GameScenePtr scene) except
{
	if (_renderSystem)
	{
		auto renderScene = _renderScenes[scene->getInstanceID()];
		if (renderScene)
		{
			_renderSystem->removeRenderScene(renderScene);
			_renderScenes[scene->getInstanceID()] = nullptr;
		}
	}
}

void
RenderFeatures::onFrameBegin() except
{
}

void
RenderFeatures::onFrame() except
{
}

void
RenderFeatures::onFrameEnd() except
{
	if (_renderSystem)
	{
		_renderSystem->renderBegin();
		_renderSystem->renderScene();
		_renderSystem->renderEnd();
	}
}

_NAME_END