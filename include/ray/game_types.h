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
#ifndef _H_GAME_TYPES_H_
#define _H_GAME_TYPES_H_

#include <ray/ioserver.h>
#include <ray/iointerface.h>
#include <ray/instances.h>
#include <ray/variant.h>
#include <ray/archive.h>
#include <ray/except.h>
#include <ray/rtti.h>

_NAME_BEGIN

typedef std::shared_ptr<class Window> WindowPtr;

typedef std::shared_ptr<class GameListener> GameListenerPtr;
typedef std::shared_ptr<class GameScene> GameScenePtr;
typedef std::shared_ptr<class GameObject> GameObjectPtr;
typedef std::shared_ptr<class GameComponent> GameComponentPtr;
typedef std::shared_ptr<class GameFeature> GameFeaturePtr;
typedef std::shared_ptr<class GameServer> GameServerPtr;
typedef std::shared_ptr<class GameApplication> GameApplicationPtr;

typedef std::vector<GameListenerPtr> GameListeners;
typedef std::vector<GameScenePtr> GameScenes;
typedef std::vector<GameObjectPtr> GameObjects;
typedef std::vector<GameComponentPtr> GameComponents;
typedef std::vector<GameFeaturePtr> GameFeatures;

_NAME_END

#endif