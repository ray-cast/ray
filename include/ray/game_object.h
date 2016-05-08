// +----------------------------------------------------------------------
// | Project : ray.
// | All rights reserved.
// +----------------------------------------------------------------------
// | Copyright (c) 2013-2016.
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
	virtual ~GameObject() noexcept;

	void setName(const std::string& name) noexcept;
	void setName(std::string&& name) noexcept;
	const std::string& getName() const noexcept;

	void setActive(bool active) except;
	void setActiveUpwards(bool active) except;
	void setActiveDownwards(bool active) except;
	bool getActive() const noexcept;

	void setLayer(std::uint8_t layer) noexcept;
	std::uint8_t getLayer() const noexcept;

	std::uint32_t getInstanceID() const noexcept;

	void setParent(GameObjectPtr& parent) noexcept;
	void setParent(GameObjectPtr&& parent) noexcept;
	GameObjectPtr getParent() const noexcept;

	void addChild(GameObjectPtr& child) noexcept;
	void addChild(GameObjectPtr&& child) noexcept;
	void removeChild(GameObjectPtr& child) noexcept;
	void removeChild(GameObjectPtr&& child) noexcept;
	void cleanupChildren() noexcept;
	GameObjectPtr findChild(const std::string& name, bool recurse = true) noexcept;

	std::uint32_t getChildCount() const noexcept;
	GameObjects& getChildren() noexcept;
	const GameObjects& getChildren() const noexcept;

	void setTranslate(const float3& v) noexcept;
	void setTranslateAccum(const float3& v) noexcept;
	const float3& getTranslate() const noexcept;

	void setScale(const float3& v) noexcept;
	void setScaleAccum(const float3& v) noexcept;
	const float3& getScale() const noexcept;

	void setQuaternion(const Quaternion& quat) noexcept;
	void setQuaternionAccum(const Quaternion& quat) noexcept;
	const Quaternion& getQuaternion() const noexcept;

	void setEulerAngles(const EulerAngles& euler) noexcept;
	void setEulerAnglesAccum(const EulerAngles& euler) noexcept;
	const EulerAngles& getEulerAngles() const noexcept;

	const float3& getRight() const noexcept;
	const float3& getUpVector() const noexcept;
	const float3& getForward() const noexcept;

	void setTransform(const float4x4& transform) noexcept;
	const float4x4& getTransform() const noexcept;
	const float4x4& getTransformInverse() const noexcept;
	const float4x4& getTransformInverseTranspose() const noexcept;

	void addComponent(GameComponentPtr& component) except;
	void addComponent(GameComponentPtr&& component) except;
	void removeComponent(GameComponentPtr& component) noexcept;
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

	void sendMessage(const MessagePtr& message) noexcept;
	void sendMessageUpwards(const MessagePtr& message) noexcept;
	void sendMessageDownwards(const MessagePtr& message) noexcept;

	void destroy() noexcept;

	void load(iarchive& reader) noexcept;
	void save(oarchive& write) noexcept;

	GameObjectPtr clone() const noexcept;

private:

	friend class GameObjectManager;

	void _onFrameBegin() except;
	void _onFrame() except;
	void _onFrameEnd() except;

	void _onMoveBefore() except;
	void _onMoveAfter() except;

	void _updateTransform() const noexcept;

private:
	GameObject(const GameObject& copy) noexcept = delete;
	GameObject& operator=(const GameObject& copy) noexcept = delete;

private:

	bool _active;

	std::uint8_t _layer;
	std::uint32_t _instanceID;

	std::string _name;

	float3 _translate;
	float3 _scaling;
	Quaternion _quat;
	EulerAngles _euler;

	mutable bool _needUpdates;

	mutable float3 _right;
	mutable float3 _up;
	mutable float3 _forward;

	mutable float4x4 _transform;
	mutable float4x4 _transformInverse;
	mutable float4x4 _transformInverseTranspose;

	GameObjects _children;
	GameObjectWeakPtr _parent;

	GameComponents _components;
};

_NAME_END

#endif