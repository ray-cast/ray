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
#ifndef _H_GAME_SERVER_H_
#define _H_GAME_SERVER_H_

#include <ray/game_features.h>

_NAME_BEGIN

class EXPORT GameServer final
{
public:
    GameServer() noexcept;
    ~GameServer() noexcept;

    bool open() noexcept;
    void close() noexcept;

    void setActive(bool active) noexcept;
    bool getActive() const noexcept;

    void setTimer(TimerPtr timer) noexcept;
    TimerPtr getTimer() const noexcept;

    bool openScene(const std::string& sceneName);
    void closeScene(const std::string& sceneName);
    bool addScene(GameScenePtr scene) noexcept;
    void removeScene(GameScenePtr scene) noexcept;
    GameScenePtr findScene(const std::string& sceneName) noexcept;
    const GameScenes& getScenes() const noexcept;

    void addFeature(GameFeaturePtr features) noexcept;
    void removeFeature(GameFeaturePtr features) noexcept;
    GameFeaturePtr getFeature(const std::string& feature) const noexcept;
    template<typename T>
    std::shared_ptr<T> getFeature() const noexcept
    {
        return std::dynamic_pointer_cast<T>(this->getFeature(typeid(T).name()));
    }

    const GameFeatures& getGameFeatures() const noexcept;

    GameApplication* getGameApp() noexcept;

    void sendMessage(const std::string& method, const Variant* data...) except;

    void onFrameBegin() except;
    void onFrame() except;
    void onFrameEnd() except;

    void onEvent(const AppEvent& event) except;

    void onMessage(const std::string& method, const Variant* data...) except;

private:

    bool load(XMLReader* reader, GameScenePtr scene) except;

    GameObjectPtr instanceObject(XMLReader* reader, GameScenePtr scene) except;

private:
    friend GameApplication;
    void _setGameApp(GameApplication* app) noexcept;

private:
    GameServer(const GameServer&) noexcept = delete;
    GameServer& operator=(const GameServer&) noexcept = delete;

private:

    bool _isActive;

    TimerPtr _timer;

    GameScenes _scenes;
    GameFeatures _features;

    GameApplication* _gameApp;
};

_NAME_END

#endif