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
#ifndef _H_GAME_SCENE_H_
#define _H_GAME_SCENE_H_

#include <ray/game_object.h>

_NAME_BEGIN

class EXPORT GameScene final : public GameListener
{
	__DeclareSubClass(GameScene, GameListener)
public:
	struct Setting
	{
		float length;
		float mass;
		float speed;
		float skinWidth;
		Vector3 gravity;

		AABB aabb;

		Setting() noexcept;
	};

public:
	GameScene() noexcept;
	~GameScene() noexcept;

	void setActive(bool active) noexcept;
	bool getActive() const noexcept;

	void setEnvironment(const Setting& setting) noexcept;
	const Setting& getEnvironment() const noexcept;

	void addGameObject(GameObjectPtr obj) noexcept;
	void removeGameObject(GameObjectPtr obj) noexcept;

	GameObjectPtr getGameObject(const std::string& name) noexcept;
	GameObjectPtr getRootObject() noexcept;

	GameScenePtr clone() const noexcept;

	GameServer* getGameServer() noexcept;

private:

	friend GameServer;
	void _setGameServer(GameServer* server) noexcept;

	void _onFrameBegin() noexcept;
	void _onFrame() noexcept;
	void _onFrameEnd() noexcept;

	void onMessage(const GameMessage& message) except;

private:

	class RootObject : public GameObject
	{
	public:
		RootObject(GameScene* scene) noexcept;
		virtual ~RootObject() noexcept;

		virtual GameServer* getGameServer() noexcept;
		virtual GameScene* getGameScene() noexcept;

	private:

		GameScene* _scene;
	};

	bool _isActive;

	Setting _setting;

	std::string _name;

	GameObjectPtr  _root;
	GameServer* _gameServer;
};

_NAME_END

#endif