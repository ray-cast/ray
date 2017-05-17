// +----------------------------------------------------------------------
// | Project : ray.
// | All rights reserved.
// +----------------------------------------------------------------------
// | Copyright (c) 2013-2014.
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
#include <ray/game_server.h>
#include <ray/game_application.h>
#include <ray/game_scene.h>
#include <ray/game_event.h>
#include <ray/game_component.h>
#include <ray/xmlreader.h>
#include <ray/mstream.h>
#include <ray/ioserver.h>

_NAME_BEGIN

__ImplementSingleton(GameServer)
__ImplementSubClass(GameServer, rtti::Interface, "GameServer")

GameServer::GameServer() noexcept
	: _isActive(false)
	, _isQuitRequest(false)
	, _gameApp(nullptr)
{
}

GameServer::~GameServer() noexcept
{
	this->close();
}

bool
GameServer::open() noexcept
{
	_timer = std::make_shared<Timer>();
	_timer->open();
	return true;
}

void
GameServer::close() noexcept
{
	_isActive = false;
	_isQuitRequest = true;

	for (auto& it : _scenes)
		it->setActive(false);

	_scenes.clear();

	auto it = _features.rbegin();
	auto end = _features.rend();

	for (; it != end; ++it)
		(*it)->setActive(false);

	_features.clear();
}

void 
GameServer::setGameListener(GameListenerPtr listener) noexcept
{
	if (_gameListener != listener)
	{
		for (auto& it : _features)
			it->setGameListener(listener);

		for (auto& it : _scenes)
			it->setGameListener(listener);

		_gameListener = listener;
	}
}

GameListenerPtr 
GameServer::getGameListener() const noexcept
{
	return _gameListener;
}

bool
GameServer::isQuitRequest() const noexcept
{
	return _isQuitRequest;
}

bool
GameServer::start() noexcept
{
	try
	{
		if (!_isActive)
		{
			for (auto& it : _features)
				it->setActive(true);

			for (auto& it : _scenes)
				it->setActive(true);

			_timer->reset();

			_isActive = true;
		}

		return true;
	}
	catch (const exception& e)
	{
		this->print(e.what());

		_isQuitRequest = true;
		return false;
	}
}

void
GameServer::stop() noexcept
{
	if (_isActive)
	{
		for (auto& it : _scenes)
			it->setActive(false);

		for (auto& it : _features)
			it->setActive(false);

		_isActive = false;
	}
}

bool 
GameServer::active() const noexcept
{
	return _isActive;
}

void
GameServer::setTimer(TimerPtr timer) noexcept
{
	assert(timer);
	_timer = timer;
}

TimerPtr
GameServer::getTimer() const noexcept
{
	return _timer;
}

bool
GameServer::openScene(const std::string& filename) noexcept
{
	StreamReaderPtr stream;
	if (!IoServer::instance()->openFile(stream, filename, ios_base::in))
		return false;

	try
	{
		XMLReader xml;
		if (!xml.open(*stream))
			return false;

		auto scene = std::make_shared<GameScene>();
		scene->setGameListener(_gameListener);
		scene->load(xml);

		this->addScene(scene);
		return true;
	}
	catch (const exception& e)
	{
		this->print(e.what());
		return false;
	}
}

void
GameServer::closeScene(const std::string& sceneName) noexcept
{
	auto scene = this->findScene(sceneName);
	if (scene)
	{
		scene->setActive(false);

		for (auto& feature : _features)
			feature->onCloseScene(scene);
	}
}

GameScenePtr
GameServer::findScene(const std::string& sceneName) noexcept
{
	for (auto& it : _scenes)
	{
		if (it->getName() == sceneName)
		{
			return it;
		}
	}

	return nullptr;
}

const GameScenes&
GameServer::getScenes() const noexcept
{
	return _scenes;
}

bool
GameServer::addScene(GameScenePtr& scene) noexcept
{
	assert(std::find(_scenes.begin(), _scenes.end(), scene) == _scenes.end());

	try
	{
		for (auto& feature : _features)
			feature->onOpenScene(scene);

		if (this->active())
			scene->setActive(true);

		_scenes.push_back(scene);

		return true;
	}
	catch (const exception& e)
	{
		this->print(e.what());
		return false;
	}
}

bool
GameServer::addScene(GameScenePtr&& scene) noexcept
{
	return this->addScene(scene);
}

void
GameServer::removeScene(GameScenePtr& scene) noexcept
{
	auto it = std::find(_scenes.begin(), _scenes.end(), scene);
	if (it != _scenes.end())
	{
		for (auto& feature : _features)
		{
			feature->onCloseScene(*it);
		}

		(*it)->setActive(false);

		_scenes.erase(it);
	}
}

void
GameServer::removeScene(GameScenePtr&& scene) noexcept
{
	this->removeScene(scene);
}

bool
GameServer::addFeature(GameFeaturePtr& features) noexcept
{
	assert(features);

	try
	{
		auto it = std::find_if(_features.begin(), _features.end(), [features](GameFeaturePtr it) { return features->isInstanceOf(it->rtti()); });
		if (it == _features.end())
		{
			features->_setGameServer(this);
			features->setGameListener(_gameListener);

			if (this->active())
				features->onActivate();

			_features.push_back(features);
		}

		return true;
	}
	catch (const exception& e)
	{
		this->print(e.what());
		return false;
	}
}

void
GameServer::removeFeature(GameFeaturePtr& features) noexcept
{
	assert(features);
	auto it = std::find(_features.begin(), _features.end(), features);
	if (it != _features.end())
	{
		(*it)->onDeactivate();
		(*it)->_setGameServer(nullptr);

		_features.erase(it);
	}
}

GameFeaturePtr
GameServer::getFeature(const rtti::Rtti& rtti) const noexcept
{
	for (auto& it : _features)
	{
		if (it->isInstanceOf(rtti))
			return it;
	}

	assert(false);
	return nullptr;
}

GameFeaturePtr
GameServer::getFeature(const rtti::Rtti* rtti) const noexcept
{
	for (auto& it : _features)
	{
		if (it->isInstanceOf(rtti))
			return it;
	}

	assert(false);
	return nullptr;
}

const GameFeatures&
GameServer::getGameFeatures() const noexcept
{
	return _features;
}

void
GameServer::_setGameApp(GameApplication* app) noexcept
{
	_gameApp = app;
}

GameApplication*
GameServer::getGameApp() noexcept
{
	return _gameApp;
}

bool
GameServer::sendMessage(const MessagePtr& message) noexcept
{
	if (_isQuitRequest)
		return false;

	try
	{
		for (auto& it : _features)
			it->onMessage(message);

		for (auto& it : _scenes)
			it->sendMessage(message);

		_dispatcher.sendMessage(message);

		return true;
	}
	catch (const exception& e)
	{
		this->print(e.what());
		return false;
	}
}

bool
GameServer::postMessage(const MessagePtr& event) noexcept
{
	_dispatcher.postMessage(event);
	return true;
}

void 
GameServer::print(const std::string& name) noexcept
{
	this->getGameApp()->print(name);
}

void
GameServer::update() noexcept
{
	try
	{
		_timer->update();

		MessagePtr event;
		while (_dispatcher.pollMessages(event))
		{
			if (!this->sendMessage(event))
				_isQuitRequest = true;
		}			

		if (!_isQuitRequest)
		{
			for (auto& it : _features)
				it->onFrameBegin();

			for (auto& it : _features)
				it->onFrame();

			for (auto& it : _features)
				it->onFrameEnd();
		}
	}
	catch (const exception& e)
	{
		this->print(e.what());

		_isQuitRequest = true;
	}
}

_NAME_END