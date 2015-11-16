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
#include <ray/game_application.h>
#include <ray/game_server.h>
#include <ray/game_base_features.h>

#include <ray/render_features.h>
#include <ray/input_features.h>

#include <ray/rtti_factory.h>

#if defined(_BUILD_SCRIPT)
#	include <ray/script_features.h>
#endif

#if defined(_BUILD_PHYSIC)
#	include <ray/physics_features.h>
#endif

_NAME_BEGIN

GameApplication::GameApplication() noexcept
	: _ioServer(nullptr)
	, _ioInterface(nullptr)
	, _isQuitRequest(false)
	, _isInitialize(false)
	, _workDir("")
	, _engineDir("..\\..\\engine\\")
	, _resourceBaseDir("..\\..\\dlc\\")
{
}

GameApplication::~GameApplication() noexcept
{
	this->close();
}

bool
GameApplication::initialize(int argc, char *argv[]) except
{
	if (argc != 0)
	{
		if (access(argv[0], 0) != 0)
			return false;
		_workDir = directory(argv[0]);
	}

	if (!_isInitialize)
	{
		_ioServer = IoServer::instance();
		_ioServer->addAssign(IoAssign("sys", _workDir + _engineDir));
		_ioServer->addAssign(IoAssign("dlc", _workDir + _resourceBaseDir));
		_ioServer->mountArchives();

		_ioInterface = IoInterface::instance();
		_ioInterface->open();

		rtti::Factory::instance()->open();

		_isInitialize = true;
	}

	return _isInitialize;
}

bool
GameApplication::open(WindHandle hwnd, std::size_t width, std::size_t height) except
{
	if (_isInitialize)
	{
		_gameServer = std::make_shared<GameServer>();
		_gameServer->_setGameApp(this);

		if (_gameServer->open())
		{
			this->addFeatures(std::make_shared<InputFeatures>());

#if defined(_BUILD_SCRIPT)
			this->addFeatures(std::make_shared<ScriptFeatures>());
#endif

			this->addFeatures(std::make_shared<GameBaseFeatures>());

#if defined(_BUILD_PHYSIC)
			this->addFeatures(std::make_shared<PhysicFeatures>());
#endif

			this->addFeatures(std::make_shared<RenderFeatures>(hwnd, width, height));

			this->start();
		}
	}

	return _isInitialize;
}

void
GameApplication::close() noexcept
{
	if (_gameServer)
	{
		_gameServer.reset();
		_gameServer = nullptr;
	}

	if (_ioInterface)
	{
		_ioInterface->close();
		_ioInterface = nullptr;
	}
}

void
GameApplication::start() except
{
	assert(_gameServer);
	_gameServer->setActive(true);
}

void
GameApplication::stop() noexcept
{
	assert(_gameServer);
	_gameServer->setActive(false);
}

bool
GameApplication::isQuitRequest() const noexcept
{
	assert(_gameServer);
	return _gameServer->isQuitRequest();
}

bool
GameApplication::openScene(GameScenePtr scene) except
{
	assert(_gameServer);
	return _gameServer->addScene(scene);
}

bool
GameApplication::openScene(const std::string& name) except
{
	assert(_gameServer);
	return _gameServer->openScene(name);
}

void
GameApplication::closeScene(GameScenePtr name) noexcept
{
	assert(_gameServer);
	return _gameServer->removeScene(name);
}

void
GameApplication::closeScene(const std::string& name) noexcept
{
	assert(_gameServer);
	_gameServer->closeScene(name);
}

GameScenePtr
GameApplication::findScene(const std::string& name) noexcept
{
	assert(_gameServer);
	return _gameServer->findScene(name);
}

void
GameApplication::addFeatures(GameFeaturePtr feature) except
{
	assert(_gameServer);
	_gameServer->addFeature(feature);
}

void
GameApplication::removeFeatures(GameFeaturePtr feature) noexcept
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
GameApplication::setFileServicePath(const std::string& path) noexcept
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

	_ioServer->addAssign(IoAssign("sys", engineDir));
	_ioServer->addAssign(IoAssign("dlc", resourceBaseDir));
}

void
GameApplication::setResDownloadURL(const std::string& path) noexcept
{
	assert(_ioServer);
	if (_downloadURL != path)
	{
		_ioServer->addAssign(IoAssign("url", path));
		_downloadURL = path;
	}
}

void
GameApplication::sendMessage(const GameMessage& message) noexcept
{
	assert(_gameServer);
	_gameServer->sendMessage(message);
}

void
GameApplication::postMessage(const GameMessage& message) noexcept
{
	assert(_gameServer);
	_gameServer->postMessage(message);
}

void
GameApplication::update() except
{
	assert(_gameServer);
	_gameServer->update();
}

_NAME_END