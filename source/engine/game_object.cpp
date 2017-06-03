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
#include <ray/game_object.h>
#include <ray/game_object_manager.h>
#include <ray/game_component.h>

_NAME_BEGIN

__ImplementSubClass(GameObject, rtti::Interface, "Object")

GameObject::GameObject() noexcept
	: _active(false)
	, _layer(0)
	, _localScaling(float3::One)
	, _localTranslate(float3::Zero)
	, _localRotation(Quaternion::Zero)
	, _worldScaling(float3::One)
	, _worldTranslate(float3::Zero)
	, _worldRotation(Quaternion::Zero)
	, _localNeedUpdates(true)
	, _worldNeedUpdates(true)
{
	GameObjectManager::instance()->_instanceObject(this, _instanceID);
}

GameObject::GameObject(const archivebuf& reader) except
	: GameObject()
{
	this->load(reader);
}

GameObject::~GameObject() noexcept
{
	this->cleanupChildren();
	this->cleanupComponents();

	GameObjectManager::instance()->_unsetObject(this);
}

void
GameObject::setName(const util::string& name) noexcept
{
	_name = name;
}

void
GameObject::setName(util::string&& name) noexcept
{
	_name = std::move(name);
}

const util::string&
GameObject::getName() const noexcept
{
	return _name;
}

void
GameObject::setActive(bool active) except
{
	if (_active != active)
	{
		if (active)
			this->_onActivate();
		else
			this->_onDeactivate();

		_active = active;
	}
}

void
GameObject::setActiveUpwards(bool active) except
{
	if (_active != active)
	{
		if (active)
			this->_onActivate();
		else
			this->_onDeactivate();

		auto parent = this->getParent();
		if (parent)
			parent->setActiveUpwards(active);

		_active = active;
	}
}

void
GameObject::setActiveDownwards(bool active) except
{
	if (_active != active)
	{
		if (active)
			this->_onActivate();
		else
			this->_onDeactivate();

		for (auto& it : _children)
			it->setActiveDownwards(active);

		_active = active;
	}
}

bool
GameObject::getActive() const noexcept
{
	return _active;
}

void
GameObject::setLayer(std::uint8_t layer) noexcept
{
	if (_layer != layer)
	{
		this->_onLayerChangeBefore();

		_layer = layer;

		this->_onLayerChangeAfter();
	}
}

std::uint8_t
GameObject::getLayer() const noexcept
{
	return _layer;
}

std::size_t
GameObject::getInstanceID() const noexcept
{
	return _instanceID;
}

void
GameObject::setParent(const GameObjectPtr& parent) noexcept
{
	assert(this != parent.get());

	auto _weak = _parent.lock();
	if (_weak != parent)
	{
		this->_onMoveBefore();

		if (_weak)
		{
			auto it = _weak->_children.begin();
			auto end = _weak->_children.end();

			for (; it != end; ++it)
			{
				if ((*it).get() == this)
				{
					_weak->_children.erase(it);
					break;
				}
			}
		}

		_parent = parent;
		if (parent)
			parent->_children.push_back(this->downcast_pointer<GameObject>());

		this->_updateWorldChildren();
		this->_onMoveAfter();
	}
}

GameObject*
GameObject::getParent() const noexcept
{
	return _parent.lock().get();
}

GameScene*
GameObject::getGameScene() noexcept
{
	auto parent = this->getParent();
	if (parent)
		return parent->getGameScene();
	return nullptr;
}

const GameScene*
GameObject::getGameScene() const noexcept
{
	auto parent = this->getParent();
	if (parent)
		return parent->getGameScene();
	return nullptr;
}

void
GameObject::addChild(GameObjectPtr& entity) noexcept
{
	assert(entity);
	entity->setParent(std::dynamic_pointer_cast<GameObject>(this->shared_from_this()));
}

void
GameObject::removeChild(GameObjectPtr& entity) noexcept
{
	assert(entity);

	auto it = _children.begin();
	auto end = _children.end();
	for (; it != end; ++it)
	{
		if ((*it) == entity)
		{
			(*it)->setParent(nullptr);
			break;
		}
	}

	if (it != end)
	{
		_children.erase(it);
	}
}

void
GameObject::cleanupChildren() noexcept
{
	for (auto& it : _children)
		it.reset();

	_children.clear();
}

GameObjectPtr
GameObject::findChild(const util::string& name, bool recuse) noexcept
{
	for (auto& it : _children)
	{
		if (it->getName() == name)
		{
			return it;
		}
	}

	if (recuse)
	{
		for (auto& it : _children)
		{
			auto result = it->findChild(name, recuse);
			if (result)
			{
				return result;
			}
		}
	}

	return nullptr;
}

std::size_t
GameObject::getChildCount() const noexcept
{
	return _children.size();
}

GameObjects&
GameObject::getChildren() noexcept
{
	return _children;
}

const GameObjects&
GameObject::getChildren() const noexcept
{
	return _children;
}

void
GameObject::setTranslate(const float3& pos) noexcept
{
	if (_localTranslate != pos)
	{
		this->_onMoveBefore();

		_localTranslate = pos;
		_localNeedUpdates = true;

		this->_updateLocalChildren();
		this->_onMoveAfter();
	}
}

void
GameObject::setTranslateAccum(const float3& v) noexcept
{
	this->setTranslate(_localTranslate + v);
}

const float3&
GameObject::getTranslate() const noexcept
{
	_updateLocalTransform();
	return _localTranslate;
}

void
GameObject::setScale(const float3& scale) noexcept
{
	if (_localScaling != scale)
	{
		this->_onMoveBefore();

		_localScaling = scale;
		_localNeedUpdates = true;

		this->_updateLocalChildren();
		this->_onMoveAfter();
	}
}

void
GameObject::setScaleAll(const float scale) noexcept
{
	this->setScale(float3(scale, scale, scale));
}

void
GameObject::setScaleAccum(const float3& scale) noexcept
{
	this->setScale(_localScaling + scale);
}

const float3&
GameObject::getScale() const noexcept
{
	_updateLocalTransform();
	return _localScaling;
}

void
GameObject::setQuaternion(const Quaternion& quat) noexcept
{
	if (_localRotation != quat)
	{
		this->_onMoveBefore();

		_localRotation = quat;
		_localNeedUpdates = true;

		this->_updateLocalChildren();
		this->_onMoveAfter();
	}
}

void
GameObject::setQuaternionAccum(const Quaternion& quat) noexcept
{
	this->setQuaternion(math::cross(quat, _localRotation));
}

const Quaternion&
GameObject::getQuaternion() const noexcept
{
	_updateLocalTransform();
	return _localRotation;
}

const float3&
GameObject::getRight() const noexcept
{
	_updateLocalTransform();
	return _localTransform.getRight();
}

const float3&
GameObject::getUpVector() const noexcept
{
	_updateLocalTransform();
	return _localTransform.getUpVector();
}

const float3&
GameObject::getForward() const noexcept
{
	_updateLocalTransform();
	return _localTransform.getForward();
}

void
GameObject::setTransform(const float4x4& transform) noexcept
{
	this->_onMoveBefore();

	_localTransform = transform.getTransform(_localTranslate, _localRotation, _localScaling);
	_localNeedUpdates = false;

	this->_updateLocalChildren();
	this->_onMoveAfter();
}

void
GameObject::setTransformOnlyRotate(const float4x4& transform) noexcept
{
	this->_onMoveBefore();

	_localTransform = transform.getTransformOnlyRotation(_localTranslate, _localRotation);
	_localTransform.scale(_localScaling);
	_localNeedUpdates = false;

	this->_updateLocalChildren();
	this->_onMoveAfter();
}

const float4x4&
GameObject::getTransform() const noexcept
{
	this->_updateLocalTransform();
	return _localTransform;
}

const float4x4&
GameObject::getTransformInverse() const noexcept
{
	this->_updateLocalTransform();
	return _localTransformInverse;
}

void
GameObject::setWorldTranslate(const float3& pos) noexcept
{
	if (_worldTranslate != pos)
	{
		this->_onMoveBefore();

		_worldTranslate = pos;
		_worldNeedUpdates = true;

		this->_updateWorldChildren();
		this->_onMoveAfter();
	}
}

void
GameObject::setWorldTranslateAccum(const float3& v) noexcept
{
	this->setWorldTranslate(_worldTranslate + v);
}

const float3&
GameObject::getWorldTranslate() const noexcept
{
	_updateWorldTransform();
	return _worldTranslate;
}

void
GameObject::setWorldScale(const float3& pos) noexcept
{
	if (_worldScaling != pos)
	{
		this->_onMoveBefore();

		_worldScaling = pos;
		_worldNeedUpdates = true;

		this->_updateWorldChildren();
		this->_onMoveAfter();
	}
}

void
GameObject::setWorldScaleAccum(const float3& scale) noexcept
{
	this->setWorldScale(_worldScaling + scale);
}

const float3&
GameObject::getWorldScale() const noexcept
{
	_updateWorldTransform();
	return _worldScaling;
}

void
GameObject::setWorldQuaternion(const Quaternion& quat) noexcept
{
	if (_worldRotation != quat)
	{
		this->_onMoveBefore();

		_worldRotation = quat;
		_worldNeedUpdates = true;

		this->_updateWorldChildren();
		this->_onMoveAfter();
	}
}

void
GameObject::setWorldQuaternionAccum(const Quaternion& quat) noexcept
{
	this->setQuaternion(math::cross(quat, _worldRotation));
}

const Quaternion&
GameObject::getWorldQuaternion() const noexcept
{
	_updateWorldTransform();
	return _worldRotation;
}

void
GameObject::setWorldTransform(const float4x4& transform) noexcept
{
	this->_onMoveBefore();

	_worldTransform = transform.getTransform(_worldTranslate, _worldRotation, _worldScaling);
	_worldTransformInverse = math::transformInverse(_worldTransform);
	_worldNeedUpdates = false;

	this->_updateWorldChildren();
	this->_onMoveAfter();
}

void
GameObject::setWorldTransformOnlyRotate(const float4x4& transform) noexcept
{
	this->_onMoveBefore();

	_worldTransform = transform.getTransformOnlyRotation(_worldTranslate, _worldRotation);
	_worldTransform.scale(_worldScaling);
	_worldTransformInverse = math::transformInverse(_worldTransform);

	_worldNeedUpdates = false;

	this->_updateWorldChildren();
	this->_onMoveAfter();
}

const float4x4&
GameObject::getWorldTransform() const noexcept
{
	this->_updateWorldTransform();
	return _worldTransform;
}

const float4x4&
GameObject::getWorldTransformInverse() const noexcept
{
	this->_updateWorldTransform();
	return _worldTransformInverse;
}

void
GameObject::addComponent(const GameComponentPtr& gameComponent) except
{
	assert(gameComponent);
	assert(gameComponent->_gameObject == nullptr);

	auto it = std::find(_components.begin(), _components.end(), gameComponent);
	if (it == _components.end())
	{
		gameComponent->_setGameObject(this);
		gameComponent->onAttach();

		if (this->getActive() && gameComponent->getActive())
			gameComponent->onActivate();

		for (auto& component : _components)
			gameComponent->onAttachComponent(component);

		for (auto& component : _components)
			component->onAttachComponent(gameComponent);

		_components.push_back(gameComponent);
	}
}

void
GameObject::addComponent(GameComponentPtr&& component) except
{
	this->addComponent(component);
}

void
GameObject::removeComponent(const GameComponentPtr& gameComponent) noexcept
{
	assert(gameComponent);
	assert(gameComponent->_gameObject == this);

	auto it = std::find(_components.begin(), _components.end(), gameComponent);
	if (it != _components.end())
	{
		_components.erase(it);

		for (auto& compoent : _components)
			compoent->onDetachComponent(gameComponent);

		for (auto& component : _components)
			gameComponent->onDetachComponent(component);

		if (this->getActive() && gameComponent->getActive())
			gameComponent->onDeactivate();

		gameComponent->onDetach();
		gameComponent->_setGameObject(nullptr);

		this->removeComponentDispatchs(gameComponent);
	}
}

void
GameObject::removeComponent(GameComponentPtr&& component) noexcept
{
	this->removeComponent(component);
}

void
GameObject::cleanupComponents() noexcept
{
	for (auto it = _components.begin(); it != _components.end();)
	{
		auto gameComponent = *it;
		auto nextComponent = _components.erase(it);

		for (auto& compoent : _components)
			compoent->onDetachComponent(gameComponent);

		for (auto& component : _components)
			gameComponent->onDetachComponent(component);

		if (this->getActive() && (gameComponent)->getActive())
			gameComponent->onDeactivate();

		gameComponent->onDetach();
		gameComponent->_setGameObject(nullptr);

		this->removeComponentDispatchs(gameComponent);

		it = nextComponent;
	}
}

GameComponentPtr
GameObject::getComponent(const rtti::Rtti* type) const noexcept
{
	assert(type);

	for (auto& it : _components)
	{
		if (it->isA(type))
			return it;
	}

	return nullptr;
}

GameComponentPtr
GameObject::getComponent(const rtti::Rtti& type) const noexcept
{
	return this->getComponent(&type);
}

GameComponentPtr
GameObject::getComponentInChildren(const rtti::Rtti* type) const noexcept
{
	assert(type);

	for (auto& it : _children)
	{
		for (auto& component : it->_components)
		{
			if (component->isA(type))
				return component;
		}

		auto component = it->getComponentInChildren(type);
		if (component)
			return component;
	}

	return nullptr;
}

GameComponentPtr
GameObject::getComponentInChildren(const rtti::Rtti& type) const noexcept
{
	return this->getComponentInChildren(&type);
}

void
GameObject::getComponentsInChildren(const rtti::Rtti* type, GameComponents& components) const noexcept
{
	assert(type);

	for (auto& it : _children)
	{
		for (auto& component : it->_components)
		{
			if (component->isA(type))
				components.push_back(component);
		}

		it->getComponentInChildren(type);
	}
}

void
GameObject::getComponentsInChildren(const rtti::Rtti& type, GameComponents& components) const noexcept
{
	return this->getComponentsInChildren(&type, components);
}

const GameComponents&
GameObject::getComponents() const noexcept
{
	return _components;
}

void
GameObject::addComponentDispatch(GameDispatchType type, GameComponentPtr component) noexcept
{
	assert(component);

	if (_dispatchComponents.empty())
		_dispatchComponents.resize(GameDispatchType::GameDispatchTypeRangeSize);

	if (std::find(_dispatchComponents[type].begin(), _dispatchComponents[type].end(), component) != _dispatchComponents[type].end())
		return;

	if (this->getActive())
	{
		if (type == GameDispatchType::GameDispatchTypeFrame ||
			type == GameDispatchType::GameDispatchTypeFrameBegin ||
			type == GameDispatchType::GameDispatchTypeFrameEnd)
		{
			if (_dispatchComponents[GameDispatchType::GameDispatchTypeFrame].empty() &&
				_dispatchComponents[GameDispatchType::GameDispatchTypeFrameBegin].empty() &&
				_dispatchComponents[GameDispatchType::GameDispatchTypeFrameEnd].empty())
			{
				GameObjectManager::instance()->_activeObject(this, true);
			}
		}
	}

	_dispatchComponents[type].push_back(component);
}

void
GameObject::removeComponentDispatch(GameDispatchType type, GameComponentPtr component) noexcept
{
	assert(component);

	if (_dispatchComponents.empty())
		return;

	auto it = std::find(_dispatchComponents[type].begin(), _dispatchComponents[type].end(), component);
	if (it == _dispatchComponents[type].end())
		return;

	if (this->getActive())
	{
		if (type == GameDispatchType::GameDispatchTypeFrame ||
			type == GameDispatchType::GameDispatchTypeFrameBegin ||
			type == GameDispatchType::GameDispatchTypeFrameEnd)
		{
			if (_dispatchComponents[GameDispatchType::GameDispatchTypeFrame].empty() &&
				_dispatchComponents[GameDispatchType::GameDispatchTypeFrameBegin].empty() &&
				_dispatchComponents[GameDispatchType::GameDispatchTypeFrameEnd].empty())
			{
				GameObjectManager::instance()->_activeObject(this, false);
			}
		}
	}

	_dispatchComponents[type].erase(it);
}

void
GameObject::removeComponentDispatchs(GameComponentPtr component) noexcept
{
	assert(component);

	for (auto& dispatch : _dispatchComponents)
	{
		auto it = std::find(dispatch.begin(), dispatch.end(), component);
		if (it != dispatch.end())
			dispatch.erase(it);
	}
}

void
GameObject::destroy() noexcept
{
	this->cleanupChildren();
	this->cleanupComponents();
	this->setParent(nullptr);
}

void
GameObject::sendMessage(const MessagePtr& message) noexcept
{
	assert(message);

	if (!this->getActive())
		return;

	auto parent = this->getParent();
	while (parent)
	{
		auto temp = parent->getParent();
		if (!temp)
			break;
		else
			parent = temp;
	}

	if (parent)
		parent->sendMessageDownwards(message);
	else
	{
		auto& components = this->getComponents();
		for (auto& it : components)
			it->onMessage(message);
	}
}

void
GameObject::sendMessage(const MessagePtr& message, GameComponent* ignores[], std::size_t n) noexcept
{
	assert(message);

	if (!this->getActive())
		return;

	auto parent = this->getParent();
	while (parent)
	{
		auto temp = parent->getParent();
		if (!temp)
			break;
		else
			parent = temp;
	}

	if (parent)
		parent->sendMessageDownwards(message, ignores, n);
	else
	{
		auto& components = this->getComponents();
		for (auto& it : components)
		{
			bool ignore = false;
			for (std::size_t i = 0; i < n; i++)
			{
				if (ignores[i] == it.get())
				{
					ignore = true;
					break;
				}
			}

			if (!ignore)
			{
				if (it->getActive())
					it->onMessage(message);
			}
		}
	}
}

void
GameObject::sendMessageUpwards(const MessagePtr& message) noexcept
{
	assert(message);

	if (!this->getActive())
		return;

	for (auto& it : _components)
	{
		if (it->getActive())
			it->onMessage(message);
	}

	auto parent = _parent.lock();
	if (parent)
		parent->sendMessageDownwards(message);
}

void
GameObject::sendMessageUpwards(const MessagePtr& message, GameComponent* ignores[], std::size_t n) noexcept
{
	assert(message);

	if (!this->getActive())
		return;

	for (auto& it : _components)
	{
		bool ignore = false;
		for (std::size_t i = 0; i < n; i++)
		{
			if (ignores[i] == it.get())
			{
				ignore = true;
				break;
			}
		}

		if (!ignore)
		{
			if (it->getActive())
				it->onMessage(message);
		}
	}

	auto parent = _parent.lock();
	if (parent)
		parent->sendMessageDownwards(message, ignores, n);
}

void
GameObject::sendMessageDownwards(const MessagePtr& message) noexcept
{
	assert(message);

	if (!this->getActive())
		return;

	for (auto& it : _components)
	{
		if (it->getActive())
			it->onMessage(message);
	}

	for (auto& it : _children)
		it->sendMessageDownwards(message);
}

void
GameObject::sendMessageDownwards(const MessagePtr& message, GameComponent* ignores[], std::size_t n) noexcept
{
	assert(message);

	if (!this->getActive())
		return;

	for (auto& it : _components)
	{
		bool ignore = false;
		for (std::size_t i = 0; i < n; i++)
		{
			if (ignores[i] == it.get())
			{
				ignore = true;
				break;
			}
		}

		if (!ignore)
		{
			if (it->getActive())
				it->onMessage(message);
		}
	}

	for (auto& it : _children)
		it->sendMessageDownwards(message, ignores, n);
}

void
GameObject::load(const archivebuf& reader) except
{
	bool active = false;;

	reader["name"] >> _name;
	reader["active"] >> active;
	reader["layer"] >> _layer;
	reader["position"] >> this->_localTranslate;
	reader["scale"] >> this->_localScaling;

	float3 euler = float3::Zero;
	reader["rotate"] >> euler;

	this->setActive(active);
	this->setQuaternion((Quaternion)euler);
}

void
GameObject::save(archivebuf& write) except
{
	write["name"] << _name;
	write["active"] << _active;
	write["layer"] << _layer;
	write["position"] << this->_localTranslate;
	write["scale"] << this->_localScaling;
	write["rotate"] << math::eulerAngles(this->_localRotation);
}

GameObjectPtr
GameObject::clone() const except
{
	auto instance = std::make_shared<GameObject>();
	instance->setParent(_parent.lock());
	instance->setName(this->getName());
	instance->setLayer(this->getLayer());
	instance->setQuaternion(this->getQuaternion());
	instance->setScale(this->getScale());
	instance->setTranslate(this->getTranslate());

	for (auto& it : _components)
		instance->addComponent(it->clone());

	for (auto& it : this->getChildren())
		instance->addChild(it->clone());

	return instance;
}

void
GameObject::_onFrameBegin() except
{
	assert(!_dispatchComponents.empty());

	auto& components = _dispatchComponents[GameDispatchType::GameDispatchTypeFrameBegin];
	for (auto& it : components)
		it->onFrameBegin();
}

void
GameObject::_onFrame() except
{
	assert(!_dispatchComponents.empty());

	auto& components = _dispatchComponents[GameDispatchType::GameDispatchTypeFrame];
	for (auto& it : components)
		it->onFrame();
}

void
GameObject::_onFrameEnd() except
{
	assert(!_dispatchComponents.empty());

	auto& components = _dispatchComponents[GameDispatchType::GameDispatchTypeFrameEnd];
	for (auto& it : components)
		it->onFrameEnd();
}

void
GameObject::_onActivate() except
{
	for (auto& it : _components)
	{
		if (it->getActive())
			it->onActivate();
	}

	if (!_dispatchComponents.empty())
	{
		if (!_dispatchComponents[GameDispatchType::GameDispatchTypeFrame].empty() ||
			!_dispatchComponents[GameDispatchType::GameDispatchTypeFrameBegin].empty() ||
			!_dispatchComponents[GameDispatchType::GameDispatchTypeFrameEnd].empty())
		{
			GameObjectManager::instance()->_activeObject(this, true);
		}
	}
}

void
GameObject::_onDeactivate() noexcept
{
	if (!_dispatchComponents.empty())
	{
		if (!_dispatchComponents[GameDispatchType::GameDispatchTypeFrame].empty() ||
			!_dispatchComponents[GameDispatchType::GameDispatchTypeFrameBegin].empty() ||
			!_dispatchComponents[GameDispatchType::GameDispatchTypeFrameEnd].empty())
		{
			GameObjectManager::instance()->_activeObject(this, false);
		}
	}

	for (auto& it : _components)
	{
		if (it->getActive())
			it->onDeactivate();
	}
}

void
GameObject::_onMoveBefore() except
{
	if (!this->getActive())
		return;

	if (!_dispatchComponents.empty())
	{
		auto& components = _dispatchComponents[GameDispatchType::GameDispatchTypeMoveBefore];
		for (auto& it : components)
		{
			if (it->getActive())
				it->onMoveBefore();
		}
	}

	for (auto& it : _children)
	{
		if (it->getActive())
			it->_onMoveBefore();
	}
}

void
GameObject::_onMoveAfter() except
{
	if (!this->getActive())
		return;

	if (!_dispatchComponents.empty())
	{
		auto& components = _dispatchComponents[GameDispatchType::GameDispatchTypeMoveAfter];
		for (auto& it : components)
		{
			if (it->getActive())
				it->onMoveAfter();
		}
	}

	for (auto& it : _children)
	{
		if (it->getActive())
			it->_onMoveAfter();
	}
}

void
GameObject::_onLayerChangeBefore() except
{
	if (this->getActive())
	{
		for (auto& it : _components)
		{
			if (it->getActive())
				it->onLayerChangeBefore();
		}
	}
}

void
GameObject::_onLayerChangeAfter() except
{
	if (this->getActive())
	{
		for (auto& it : _components)
		{
			if (it->getActive())
				it->onLayerChangeAfter();
		}
	}
}

void
GameObject::_updateLocalChildren() const noexcept
{
	_worldNeedUpdates = true;

	for (auto& it : _children)
		it->_updateLocalChildren();
}

void
GameObject::_updateWorldChildren() const noexcept
{
	this->_updateParentTransform();
	this->_updateLocalChildren();
}

void
GameObject::_updateLocalTransform() const noexcept
{
	if (_localNeedUpdates)
	{
		_localTransform.makeTransform(_localTranslate, _localRotation, _localScaling);
		_localTransformInverse = math::transformInverse(_localTransform);

		_localNeedUpdates = false;
	}
}

void
GameObject::_updateWorldTransform() const noexcept
{
	if (_worldNeedUpdates)
	{
		if (_parent.lock())
		{
			auto& baseTransform = _parent.lock()->getWorldTransform();
			_worldTransform = math::transformMultiply(baseTransform, this->getTransform());
			_worldTransform.getTransform(_worldTranslate, _worldRotation, _worldScaling);
			_worldTransformInverse = math::transformInverse(_worldTransform);
		}
		else
		{
			_worldTranslate = _localTranslate;
			_worldScaling = _localScaling;
			_worldRotation = _localRotation;
			_worldTransform.makeTransform(_worldTranslate, _worldRotation, _worldScaling);
			_worldTransformInverse = math::transformInverse(_worldTransform);
		}

		_worldNeedUpdates = false;
	}
}

void
GameObject::_updateParentTransform() const noexcept
{
	if (_worldNeedUpdates)
	{
		_worldTransform.makeTransform(_worldTranslate, _worldRotation, _worldScaling);
		_worldNeedUpdates = false;
	}

	if (_parent.lock())
	{
		auto& baseTransformInverse = _parent.lock()->getWorldTransformInverse();
		_localTransform = math::transformMultiply(baseTransformInverse, _worldTransform);
		_localTransform.getTransform(_localTranslate, _localRotation, _localScaling);
	}
	else
	{
		_localScaling = _worldScaling;
		_localRotation = _worldRotation;
		_localTranslate = _worldTranslate;
	}
}

_NAME_END