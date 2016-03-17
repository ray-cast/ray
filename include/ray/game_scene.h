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

class EXPORT GameScene final : public rtti::Interface
{
	__DeclareSubClass(GameScene, rtti::Interface)
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
	GameScene(const std::string& name) noexcept;
	~GameScene() noexcept;

	void setActive(bool active) except;
	bool getActive() const noexcept;

	void setName(const std::string& name) noexcept;
	const std::string& getName() const noexcept;

	void setEnvironment(const Setting& setting) noexcept;
	const Setting& getEnvironment() const noexcept;

	std::uint32_t getInstanceID() const noexcept;

	GameObjectPtr getRootObject() noexcept;

	void setGameServer(GameServerPtr server) noexcept;
	GameServerPtr getGameServer() noexcept;

	void sendMessage(const MessagePtr& message) except;

	void load(iarchive& reader) except;
	void save(oarchive& reader) except;

	GameScenePtr clone() const noexcept;

	GameObjectPtr instanceObject(iarchive& reader, GameObjectPtr parent) except;

private:

	class RootObject : public GameObject
	{
	public:
		RootObject(GameScene* scene) noexcept;
		virtual ~RootObject() noexcept;

		virtual GameServerPtr getGameServer() noexcept;
		virtual GameScenePtr getGameScene() noexcept;

	private:

		GameScene* _scene;
	};

	std::string _name;

	Setting _setting;
	GameObjectPtr  _root;
	GameServerWeakPtr _gameServer;

	std::uint32_t _instanceID;
	static std::uint32_t _instanceCount;
};

_NAME_END

#endif