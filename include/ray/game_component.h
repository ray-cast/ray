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
#ifndef _H_GAME_COMPONENT_H_
#define _H_GAME_COMPONENT_H_

#include <ray/game_object.h>

_NAME_BEGIN

class EXPORT GameComponent : public MessageListener
{
	__DeclareSubInterface(GameComponent, MessageListener)
public:
	GameComponent() noexcept;
	virtual ~GameComponent() noexcept;

	void setActive(bool active) except;
	bool getActive() const noexcept;

	void setName(const std::string& name) noexcept;
	const std::string& getName() const noexcept;

	GameObjectPtr getGameObject() noexcept;
	const GameObjectPtr getGameObject() const noexcept;

	GameServer* getGameServer() noexcept;
	const GameServer* getGameServer() const noexcept;

	template<typename T>
	std::shared_ptr<T> getComponent() const noexcept 
		{ return std::dynamic_pointer_cast<T>(this->getComponent(T::RTTI)); }

	GameComponentPtr getComponent(const rtti::Rtti* type) const noexcept;
	GameComponentPtr getComponent(const rtti::Rtti& type) const noexcept;
	const GameComponents& getComponents() const noexcept;

	virtual void load(iarchive& reader) noexcept;
	virtual void save(oarchive& write) noexcept;

	virtual GameComponentPtr clone() const except = 0;

protected:

	virtual void sendMessage(const MessagePtr& message) except;
	virtual void sendMessageUpwards(const MessagePtr& message) except;
	virtual void sendMessageDownwards(const MessagePtr& message) except;

	virtual void onAttach() except;
	virtual void onDetach() except;

	virtual void onAttachComponent(GameComponentPtr& component) except;
	virtual void onDetachComponent(GameComponentPtr& component) except;

	virtual void onActivate() except;
	virtual void onDeactivate() except;

	virtual void onFrameBegin() except;
	virtual void onFrame() except;
	virtual void onFrameEnd() except;

	virtual void onMoveBefore() except;
	virtual void onMoveAfter() except;

	virtual void onParentChangeBefore() except;
	virtual void onParentChangeAfter() except;

	virtual void onLayerChangeBefore() except;
	virtual void onLayerChangeAfter() except;

private:
	friend GameObject;
	void _setGameObject(GameObject* gameobj) noexcept;

private:

	bool _active;

	std::string _name;

	GameObject* _gameObject;

	GameComponents _components;
};

_NAME_END

#endif