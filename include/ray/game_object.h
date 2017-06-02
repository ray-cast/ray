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
#ifndef _H_GAME_OBJECT_H_
#define _H_GAME_OBJECT_H_

#include <ray/game_types.h>

_NAME_BEGIN

class EXPORT GameObject : public rtti::Interface
{
	__DeclareSubClass(GameObject, rtti::Interface)
public:
	GameObject() noexcept;
	GameObject(const archivebuf& reader) noexcept;
	virtual ~GameObject() noexcept;

	void setName(const util::string& name) noexcept;
	void setName(util::string&& name) noexcept;
	const util::string& getName() const noexcept;

	void setActive(bool active) except;
	void setActiveUpwards(bool active) except;
	void setActiveDownwards(bool active) except;
	bool getActive() const noexcept;

	void setLayer(std::uint8_t layer) noexcept;
	std::uint8_t getLayer() const noexcept;

	std::size_t getInstanceID() const noexcept;

	void setParent(const GameObjectPtr& parent) noexcept;
	GameObject* getParent() const noexcept;

	virtual GameScene* getGameScene() noexcept;
	virtual const GameScene* getGameScene() const noexcept;

	void addChild(GameObjectPtr& child) noexcept;
	void removeChild(GameObjectPtr& child) noexcept;
	void cleanupChildren() noexcept;
	GameObjectPtr findChild(const util::string& name, bool recurse = true) noexcept;

	std::size_t getChildCount() const noexcept;
	GameObjects& getChildren() noexcept;
	const GameObjects& getChildren() const noexcept;

	void setTranslate(const float3& v) noexcept;
	void setTranslateAccum(const float3& v) noexcept;
	const float3& getTranslate() const noexcept;

	void setScale(const float3& v) noexcept;
	void setScaleAll(const float v) noexcept;
	void setScaleAccum(const float3& v) noexcept;
	const float3& getScale() const noexcept;

	void setQuaternion(const Quaternion& quat) noexcept;
	void setQuaternionAccum(const Quaternion& quat) noexcept;
	const Quaternion& getQuaternion() const noexcept;

	void setTransform(const float4x4& transform) noexcept;
	void setTransformOnlyRotate(const float4x4& transform) noexcept;
	const float4x4& getTransform() const noexcept;
	const float4x4& getTransformInverse() const noexcept;

	void setWorldTranslate(const float3& v) noexcept;
	void setWorldTranslateAccum(const float3& v) noexcept;
	const float3& getWorldTranslate() const noexcept;

	void setWorldScale(const float3& v) noexcept;
	void setWorldScaleAccum(const float3& v) noexcept;
	const float3& getWorldScale() const noexcept;

	void setWorldQuaternion(const Quaternion& quat) noexcept;
	void setWorldQuaternionAccum(const Quaternion& quat) noexcept;
	const Quaternion& getWorldQuaternion() const noexcept;

	const float3& getRight() const noexcept;
	const float3& getUpVector() const noexcept;
	const float3& getForward() const noexcept;

	void setWorldTransform(const float4x4& transform) noexcept;
	void setWorldTransformOnlyRotate(const float4x4& transform) noexcept;
	const float4x4& getWorldTransform() const noexcept;
	const float4x4& getWorldTransformInverse() const noexcept;

	void addComponent(const GameComponentPtr& component) except;
	void addComponent(GameComponentPtr&& component) except;
	void removeComponent(const GameComponentPtr& component) noexcept;
	void removeComponent(GameComponentPtr&& component) noexcept;
	void cleanupComponents() noexcept;

	template<typename T>
	std::shared_ptr<T> getComponent() const noexcept { return std::dynamic_pointer_cast<T>(this->getComponent(T::RTTI)); }
	GameComponentPtr getComponent(const rtti::Rtti* type) const noexcept;
	GameComponentPtr getComponent(const rtti::Rtti& type) const noexcept;

	template<typename T>
	std::shared_ptr<T> getComponentInChildren() const noexcept { return std::dynamic_pointer_cast<T>(this->getComponentInChildren(T::RTTI)); }
	GameComponentPtr getComponentInChildren(const rtti::Rtti* type) const noexcept;
	GameComponentPtr getComponentInChildren(const rtti::Rtti& type) const noexcept;

	template<typename T>
	void getComponentsInChildren(GameComponents& components) const noexcept { this->getComponentsInChildren(T::RTTI, components); }
	void getComponentsInChildren(const rtti::Rtti* type, GameComponents& components) const noexcept;
	void getComponentsInChildren(const rtti::Rtti& type, GameComponents& components) const noexcept;

	const GameComponents& getComponents() const noexcept;

	void addComponentDispatch(GameDispatchType type, GameComponentPtr component) noexcept;
	void removeComponentDispatch(GameDispatchType type, GameComponentPtr component) noexcept;
	void removeComponentDispatchs(GameComponentPtr component) noexcept;

	void sendMessage(const MessagePtr& message) noexcept;
	void sendMessage(const MessagePtr& message, GameComponent* ignores[], std::size_t n) noexcept;

	void sendMessageUpwards(const MessagePtr& message) noexcept;
	void sendMessageUpwards(const MessagePtr& message, GameComponent* ignores[], std::size_t n) noexcept;

	void sendMessageDownwards(const MessagePtr& message) noexcept;
	void sendMessageDownwards(const MessagePtr& message, GameComponent* ignores[], std::size_t n) noexcept;

	void destroy() noexcept;

	void load(const archivebuf& reader) except;
	void save(archivebuf& write) except;

	GameObjectPtr clone() const noexcept;

private:
	friend GameObjectManager;

	void _onFrameBegin() except;
	void _onFrame() except;
	void _onFrameEnd() except;

	void _onActivate() except;
	void _onDeactivate() except;

	void _onMoveBefore() except;
	void _onMoveAfter() except;

	void _onLayerChangeBefore() except;
	void _onLayerChangeAfter() except;

private:
	void _updateLocalChildren() const noexcept;
	void _updateWorldChildren() const noexcept;
	void _updateLocalTransform() const noexcept;
	void _updateWorldTransform() const noexcept;
	void _updateParentTransform() const noexcept;

private:
	GameObject(const GameObject& copy) noexcept = delete;
	GameObject& operator=(const GameObject& copy) noexcept = delete;

private:
	bool _active;

	std::uint8_t _layer;
	std::size_t _instanceID;

	util::string _name;

	mutable float3 _localTranslate;
	mutable float3 _localScaling;
	mutable Quaternion _localRotation;

	mutable float3 _worldTranslate;
	mutable float3 _worldScaling;
	mutable Quaternion _worldRotation;

	mutable float4x4 _localTransform;
	mutable float4x4 _localTransformInverse;

	mutable float4x4 _worldTransform;
	mutable float4x4 _worldTransformInverse;

	mutable bool _localNeedUpdates;
	mutable bool _worldNeedUpdates;

	GameObjects _children;
	GameObjectWeakPtr _parent;

	GameComponents _components;
	std::vector<GameComponents> _dispatchComponents;
};

_NAME_END

#endif