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
#include <ray/game_scene.h>

_NAME_BEGIN

__ImplementSubClass(GameScene, GameMessage)

GameScene::Setting::Setting() noexcept
    : gravity(0.0f, 9.81f, 0.0f)
    , length(1.0f)
    , mass(1000.0f)
    , speed(10.0f)
    , skinWidth(0.0001f)
{
    aabb.min = Vector3(-1000, -1000, -1000);
    aabb.max = Vector3(1000, 1000, 1000);
}

GameScene::RootObject::RootObject(GameScene* scene) noexcept
    : _scene(scene)
{
}

GameScene::RootObject::~RootObject() noexcept
{
}

GameServer*
GameScene::RootObject::getGameServer() noexcept
{
    return _scene->getGameServer();
}

GameScene*
GameScene::RootObject::getGameScene() noexcept
{
    return _scene;
}

GameScene::GameScene() noexcept
    : _gameServer(nullptr)
    , _isActive(false)
{
    _root = std::make_unique<RootObject>(this);
}

GameScene::~GameScene() noexcept
{
    this->setActive(false);
}

void
GameScene::setActive(bool active) noexcept
{
    if (_isActive != active)
    {
        _isActive = active;
        _root->setActive(active);
    }
}

bool
GameScene::getActive() const noexcept
{
    return _isActive;
}

void
GameScene::setEnvironment(const Setting& setting) noexcept
{
    _setting = setting;
}

const GameScene::Setting&
GameScene::getEnvironment() const noexcept
{
    return _setting;
}

void
GameScene::addGameObject(GameObjectPtr gameobj) noexcept
{
    assert(gameobj);
    assert(!gameobj->getParent());

    _root->addChild(gameobj);
}

void
GameScene::removeGameObject(GameObjectPtr gameobj) noexcept
{
    if (gameobj)
    {
        gameobj->setActive(false);

        _root->removeChild(gameobj->getInstanceID());
    }
}

GameObjectPtr
GameScene::getGameObject(const std::string& name) noexcept
{
    return _root->findChild(name);
}

GameObjectPtr
GameScene::getRootObject() noexcept
{
    return _root;
}

GameScenePtr
GameScene::clone() const noexcept
{
    return nullptr;
}

void
GameScene::_setGameServer(GameServer* manager) noexcept
{
    _gameServer = manager;
}

GameServer*
GameScene::getGameServer() noexcept
{
    return _gameServer;
}

void
GameScene::_onFrameBegin() noexcept
{
    _root->_onFrameBegin();
}

void
GameScene::_onFrame() noexcept
{
    _root->_onFrame();
}

void
GameScene::_onFrameEnd() noexcept
{
    _root->_onFrameEnd();
}

_NAME_END