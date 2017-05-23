// +----------------------------------------------------------------------
// | Project : ray.
// | All rights reserved.
// +----------------------------------------------------------------------
// | Copyright (c) 2013-2017.
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
#include <ray/game_application.h>
#include <ray/game_server.h>
#include <ray/game_listener.h>

#include <ray/iolistener.h>
#include <ray/rtti_factory.h>

#if defined(_BUILD_INPUT)
#	include <ray/input_feature.h>
#endif

#if defined(_BUILD_SCRIPT)
#	include <ray/script_features.h>
#endif

#if defined(_BUILD_BASEGAME)
#	include <ray/game_base_features.h>
#endif

#if defined(_BUILD_PHYSIC)
#	include <ray/physics_features.h>
#endif

#if defined(_BUILD_SOUND)
#	include <ray/sound_feature.h>
#endif

#if defined(_BUILD_RENDERER)
#	include <ray/render_feature.h>
#endif

#if defined(_BUILD_GUI)
#	include <ray/gui_feature.h>
#endif

_NAME_BEGIN

class GameAppListener final : public GameListener
{
public:
	virtual void onMessage(const util::string& message) noexcept
	{
		std::cout << message << std::endl;
	}
};

class GameIoListener final : public IoListener
{
public:
	virtual void onMessage(const util::string& message) noexcept
	{
		std::cout << message << std::endl;
	}
};

GameApplication::GameApplication() noexcept
	: _isInitialize(false)
	, _gameServer(nullptr)
	, _gameListener(std::make_shared<GameAppListener>())
	, _ioListener(std::make_shared<GameIoListener>())
	, _workDir("")
	, _engineDir("../../engine/")
	, _resourceBaseDir("../../dlc/")
{
	_ioServer = IoServer::instance();
	_ioInterface = IoInterface::instance();
}

GameApplication::~GameApplication() noexcept
{
	this->close();
}

bool
GameApplication::open(WindHandle hwnd, std::uint32_t w, std::uint32_t h, std::uint32_t dpi_w, std::uint32_t dpi_h) noexcept
{
	if (_isInitialize)
	{
		if (_gameListener)
			_gameListener->onMessage("Game Application has already opened.");

		return false;
	}

	if (_gameListener)
		_gameListener->onMessage("Initializing : Game Application.");

	if (_gameListener)
		_gameListener->onMessage("Initializing : RTTI.");

	if (!rtti::Factory::instance()->open())
	{
		if (_gameListener)
			_gameListener->onMessage("Could not initialize with RTTI.");

		return false;
	}

	if (_gameListener)
		_gameListener->onMessage("Initializing : IO Server.");

	if (!_ioInterface->open())
	{
		if (_gameListener)
			_gameListener->onMessage("Could not initialize with IO Server.");
	}

	if (_gameListener)
		_gameListener->onMessage("Initializing : Game Server.");

	_gameServer = GameServer::instance();
	_gameServer->_setGameApp(this);
	_gameServer->setGameListener(_gameListener);

	if (!_gameServer->open())
		return false;

#if defined(_BUILD_INPUT)
	_inputFeature = std::make_shared<InputFeature>((CaptureObject)hwnd);
#endif
#if defined(_BUILD_SCRIPT)
	_scriptFeature = std::make_shared<ScriptFeatures>();
#endif
#if defined(_BUILD_BASEGAME)
	_gameBaseFeature = std::make_shared<GameBaseFeatures>();
#endif
#if defined(_BUILD_PHYSIC)
	_physicFeature = std::make_shared<PhysicFeatures>();
#endif
#if defined(_BUILD_SOUND)
	_soundFeature = std::make_shared<SoundFeature>();
#endif
#if defined(_BUILD_RENDERER)
	_renderFeature = std::make_shared<RenderFeature>(hwnd, w, h, dpi_w, dpi_h);
#endif
#if defined(_BUILD_GUI)
	_guiFeature = std::make_shared<GuiFeature>(w, h, dpi_w, dpi_h);
#endif

#if defined(_BUILD_INPUT)
	this->addFeatures(_inputFeature);
#endif
#if defined(_BUILD_SCRIPT)
	this->addFeatures(_scriptFeature);
#endif
#if defined(_BUILD_BASEGAME)
	this->addFeatures(_gameBaseFeature);
#endif
#if defined(_BUILD_PHYSIC)
	this->addFeatures(_physicFeature);
#endif
#if defined(_BUILD_SOUND)
	this->addFeatures(_soundFeature);
#endif
#if defined(_BUILD_RENDERER)
	this->addFeatures(_renderFeature);
#endif
#if defined(_BUILD_GUI)
	this->addFeatures(_guiFeature);
#endif

	_isInitialize = this->start();
	return _isInitialize;
}

void
GameApplication::close() noexcept
{
	if (_gameListener)
		_gameListener->onMessage("Shutdown : Game Server.");

	if (_gameServer)
	{
		_gameServer->close();
		_gameServer = nullptr;
	}

	if (_gameListener)
		_gameListener->onMessage("Shutdown : IO Server.");

	if (_ioInterface)
	{
		_ioInterface->close();
		_ioInterface = nullptr;
	}
}

void
GameApplication::setGameListener(const GameListenerPtr& listener) noexcept
{
	_gameServer->setGameListener(listener);
}

const GameListenerPtr&
GameApplication::getGameListener() const noexcept
{
	return _gameServer->getGameListener();
}

bool
GameApplication::start() noexcept
{
	if (_gameServer)
		return _gameServer->start();
	return false;
}

void
GameApplication::stop() noexcept
{
	if (_gameServer)
		_gameServer->stop();
}

bool
GameApplication::isQuitRequest() const noexcept
{
	assert(_gameServer);
	return _gameServer->isQuitRequest();
}

bool
GameApplication::openScene(const GameScenePtr& scene) noexcept
{
	assert(_gameServer);
	return _gameServer->addScene(scene);
}

bool
GameApplication::openScene(const util::string& name) noexcept
{
	assert(_gameServer);
	return _gameServer->openScene(name);
}

void
GameApplication::closeScene(const GameScenePtr& name) noexcept
{
	assert(_gameServer);
	return _gameServer->closeScene(name);
}

void
GameApplication::closeScene(const util::string& name) noexcept
{
	assert(_gameServer);
	_gameServer->closeScene(name);
}

GameScenePtr
GameApplication::findScene(const util::string& name) noexcept
{
	assert(_gameServer);
	return _gameServer->findScene(name);
}

bool
GameApplication::addFeatures(GameFeaturePtr& feature) noexcept
{
	assert(_gameServer);
	return _gameServer->addFeature(feature);
}

void
GameApplication::removeFeatures(GameFeaturePtr& feature) noexcept
{
	assert(_gameServer);
	_gameServer->removeFeature(feature);
}

void
GameApplication::setFileService(bool enable) noexcept
{
	assert(_ioServer);

	if (enable)
		_ioServer->mountArchives();
	else
		_ioServer->unmountArchives();
}

void
GameApplication::setFileServicePath(const util::string& path) noexcept
{
	assert(_ioServer);

	auto tmp = path;
	if (*tmp.rbegin() != '\\' &&
		*tmp.rbegin() != '/')
	{
		tmp += SEPARATOR;
	}

	_workDir = path;

	auto engineDir = tmp + _engineDir;
	auto resourceBaseDir = tmp + _resourceBaseDir;

	_ioServer->addAssign({ "sys", engineDir });
	_ioServer->addAssign({ "sys:media/UI/", engineDir + "media/UI/" });
	_ioServer->addAssign({ "dlc", resourceBaseDir });
}

void
GameApplication::setFileServiceListener(bool enable) noexcept
{
	if (enable)
		_ioServer->addIoListener(_ioListener);
	else
		_ioServer->removeIoListener(_ioListener);
}

void
GameApplication::setResDownloadURL(const util::string& path) noexcept
{
	assert(_ioServer);
	if (_downloadURL != path)
	{
		_ioServer->addAssign({ "url", path });
		_downloadURL = path;
	}
}

bool
GameApplication::sendMessage(const MessagePtr& message) noexcept
{
	assert(_gameServer);
	return _gameServer->sendMessage(message);
}

bool
GameApplication::postMessage(const MessagePtr& message) noexcept
{
	assert(_gameServer);
	return _gameServer->postMessage(message);
}

bool
GameApplication::sendInputEvent(const InputEvent& event) noexcept
{
	if (_inputFeature)
		return _inputFeature->downcast<InputFeature>()->sendInputEvent(event);
	return false;
}

bool
GameApplication::postInputEvent(const InputEvent& event) noexcept
{
	if (_inputFeature)
		return _inputFeature->downcast<InputFeature>()->postInputEvent(event);
	return false;
}

void
GameApplication::update() noexcept
{
	assert(_gameServer);
	_gameServer->update();
}

_NAME_END