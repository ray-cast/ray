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

_NAME_BEGIN

GameApplication::GameApplication() noexcept
    : _ioServer(nullptr)
    , _ioInterface(nullptr)
    , _isQuitRequest(false)
    , _isInitialize(false)
    , _engineDir("..\\..\\engine\\")
    , _resourceBaseDir("..\\..\\dlc\\")
{
}

GameApplication::~GameApplication() noexcept
{
    this->close();
}

bool
GameApplication::open() except
{
    assert(!_isInitialize);

    _ioServer = IoServer::instance();
    _ioServer->addAssign(IoAssign("sys", _engineDir));
    _ioServer->addAssign(IoAssign("dlc", _resourceBaseDir));
    _ioServer->mountArchives();

    _ioInterface = IoInterface::instance();
    _ioInterface->open();

    _gameServer = std::make_shared<GameServer>();
    _gameServer->_setGameApp(this);
    _gameServer->open();

    return (_isInitialize = true);
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
    return _isQuitRequest;
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

GameFeaturePtr
GameApplication::getFeature(const std::string& name) noexcept
{
    assert(_gameServer);
    return _gameServer->getFeature(name);
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

    _engineDir = tmp + "engine\\";
    _resourceBaseDir = tmp + "dlc\\";

    _ioServer->addAssign(IoAssign("sys", _engineDir));
    _ioServer->addAssign(IoAssign("dlc", _resourceBaseDir));
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
GameApplication::sendEvent(const AppEvent& event) noexcept
{
    if (!_isQuitRequest)
    {
        switch (event.event)
        {
        case AppEvent::AppQuit:
        {
            _isQuitRequest = true;
        }
        break;
        }

        _gameServer->onEvent(event);
    }
}

void
GameApplication::update() except
{
    assert(_gameServer);

    if (!_isQuitRequest)
    {
        AppEvent event;

        while (this->pollEvents(event))
        {
            this->sendEvent(event);
        }

        _gameServer->onFrameBegin();
        _gameServer->onFrame();
        _gameServer->onFrameEnd();
    }
}

_NAME_END