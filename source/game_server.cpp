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
#include <ray/game_scene.h>
#include <ray/xmlreader.h>
#include <ray/mstream.h>

_NAME_BEGIN

GameServer::GameServer() noexcept
	: _gameApp(nullptr)
	, _isActive(false)
	, _isQuitRequest(false)
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
	return _timer->open();
}

void
GameServer::close() noexcept
{
	_isQuitRequest = true;

	_scenes.clear();
	_features.clear();
}

bool
GameServer::isQuitRequest() const noexcept
{
	return _isQuitRequest;
}

void
GameServer::setActive(bool active) except
{
	if (_isActive != active)
	{
		if (active)
		{
			for (auto& it : _features)
				it->setActive(true);

			for (auto& it : _scenes)
				it->setActive(true);
		}
		else
		{
			for (auto& it : _scenes)
				it->setActive(false);

			for (auto& it : _features)
				it->setActive(false);
		}

		_isActive = active;
	}
}

bool
GameServer::getActive() const noexcept
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
GameServer::openScene(const std::string& sceneName) except
{
	auto it = _scenes.begin();
	for (; it != _scenes.end(); ++it)
	{
		if ((*it)->getName() == sceneName)
		{
			break;
		}
	}

	if (it == _scenes.end())
	{
		MemoryStream stream;

		IoServer::instance()->openFile(sceneName, stream);
		if (!stream.is_open())
			return false;

		XMLReader xml;
		if (xml.open(stream))
		{
			auto scene = std::make_shared<GameScene>();
			scene->setName(sceneName);
			scene->_setGameServer(this);

			for (auto& feature : _features)
			{
				feature->onOpenScene(scene);
			}

			if (this->load(xml, scene))
			{
				this->addScene(scene);
				return true;
			}
		}
	}

	return false;
}

void
GameServer::closeScene(const std::string& sceneName) noexcept
{
	auto scene = this->findScene(sceneName);
	if (scene)
	{
		scene->setActive(false);

		for (auto& feature : _features)
		{
			feature->onCloseScene(scene);
		}
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
GameServer::addScene(GameScenePtr scene) noexcept
{
	auto it = std::find(_scenes.begin(), _scenes.end(), scene);
	if (it == _scenes.end())
	{
		scene->_setGameServer(this);
		if (this->getActive())
		{
			scene->setActive(true);
		}

		_scenes.push_back(scene);
		return true;
	}

	return false;
}

void
GameServer::removeScene(GameScenePtr scene) noexcept
{
	auto it = std::find(_scenes.begin(), _scenes.end(), scene);
	if (it != _scenes.end())
	{
		for (auto& feature : _features)
		{
			feature->onCloseScene((*it));
		}

		(*it)->setActive(false);

		_scenes.erase(it);
	}
}

void
GameServer::addFeature(GameFeaturePtr features) except
{
	assert(features);

	auto it = std::find(_features.begin(), _features.end(), features);
	if (it == _features.end())
	{
		features->_setGameServer(this);

		if (this->getActive())
			features->onActivate();

		_features.push_back(features);
	}
}

void
GameServer::removeFeature(GameFeaturePtr features) noexcept
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
GameServer::getFeature(const std::string& name) const noexcept
{
	for (auto& it : _features)
	{
		if (it->getName() == name)
		{
			return it;
		}
	}

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

void
GameServer::sendMessage(const GameMessage& message) except
{
	if (!_isQuitRequest)
	{
		if (message.event == GameEvent::AppQuit)
			_isQuitRequest = true;

		for (auto& it : _features)
			it->onMessage(message);

		auto scenes = this->getScenes();
		for (auto& it : scenes)
			it->sendMessage(message);
	}
}

void
GameServer::postMessage(const GameMessage& event) except
{
	GameDispatcher::postMessage(event);
}

void
GameServer::update() except
{
	if (!_isQuitRequest)
	{
		GameMessage event;

		while (this->pollMessages(event))
		{
			this->sendMessage(event);
		}

		this->onFrameBegin();
		this->onFrame();
		this->onFrameEnd();
	}
}

GameObjectPtr
GameServer::instanceObject(iarchive& reader, GameScenePtr scene) except
{
	std::string name = reader.getCurrentNodeName();
	if (name == "object")
	{
		auto actor = std::make_shared<GameObject>();
		scene->addGameObject(actor);
		reader.setToFirstChild();

		bool active = false;

		do
		{
			auto key = reader.getCurrentNodeName();
			if (key == "attribute")
			{
				auto attributes = reader.getAttrs();
				for (auto& it : attributes)
				{
					if (it == "name")
					{
						actor->setName(reader.getString(it));
					}
					else if (it == "active")
					{
						active = reader.getBoolean(it);
					}
					else if (it == "position")
					{
						actor->setTranslate(reader.getFloat3(it));
					}
					else if (it == "scale")
					{
						actor->setScale(reader.getFloat3(it));
					}
					else if (it == "lookat")
					{
						actor->setLookAt(reader.getFloat3(it));
					}
					else if (it == "up")
					{
						actor->setUpVector(reader.getFloat3(it));
					}
					else if (it == "rotate")
					{
						float3 value = reader.getFloat3(it);

						Quaternion quat;
						quat.x = value.x;
						quat.y = value.y;
						quat.z = value.z;

						actor->setRotate(quat);
					}
					else if (it == "layer")
					{
						actor->setLayer(reader.getInteger(it));
					}
				}
			}
			else if (key == "component")
			{
				GameComponentPtr component = nullptr;

				for (auto& it : _features)
				{
					component = it->onSerialization(reader);
					if (component)
						break;
				}

				if (component)
				{
					actor->addComponent(component);
				}
				else
				{
					throw failure("Unknown component name : " + reader.getString("name"));
				}
			}
		} while (reader.setToNextChild());

		actor->setActive(active);
		return actor;
	}

	return nullptr;
}

bool
GameServer::load(iarchive& reader, GameScenePtr scene) except
{
	std::string nodeName;
	nodeName = reader.getCurrentNodeName();
	if (nodeName == "scene")
	{
		reader.setToFirstChild();

		do
		{
			nodeName = reader.getCurrentNodeName();
			if (nodeName == "attribute")
			{
				auto attributes = reader.getAttrs();
				for (auto& it : attributes)
				{
					if (it == "name")
					{
						scene->setName(reader.getString(it));
					}
				}
			}
			else if (nodeName == "object")
			{
				instanceObject(reader, scene);
			}
		} while (reader.setToNextChild());
	}

	return true;
}

void
GameServer::onFrameBegin() except
{
	_timer->update();

	for (auto& it : _scenes)
	{
		it->_onFrameBegin();
	}

	for (auto& it : _features)
	{
		it->onFrameBegin();
	}
}

void
GameServer::onFrame() except
{
	for (auto& it : _scenes)
	{
		it->_onFrame();
	}

	for (auto& it : _features)
	{
		it->onFrame();
	}
}

void
GameServer::onFrameEnd() except
{
	for (auto& it : _scenes)
	{
		it->_onFrameEnd();
	}

	for (auto& it : _features)
	{
		it->onFrameEnd();
	}
}

_NAME_END