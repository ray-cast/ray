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
#include <ray/game_object.h>
#include <ray/game_object_manager.h>
#include <ray/game_component.h>

_NAME_BEGIN

__ImplementSubClass(GameObject, rtti::Interface, "Object")

GameObject::GameObject() noexcept
	: _active(false)
	, _layer(0)
	, _needUpdates(true)
	, _scaling(float3::One)
	, _translate(float3::Zero)
	, _quat(Quaternion::Zero)
	, _euler(float3::Zero)
	, _dispatchComponents(GameDispatchType::GameDispatchTypeRangeSize)
{
	GameObjectManager::instance()->_instanceObject(this, _instanceID);
}

GameObject::~GameObject() noexcept
{
	this->cleanupChildren();
	this->cleanupComponents();

	GameObjectManager::instance()->_unsetObject(this);
}

void
GameObject::setName(const std::string& name) noexcept
{
	_name = name;
}

void
GameObject::setName(std::string&& name) noexcept
{
	_name = std::move(name);
}

const std::string&
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
			it->setActiveDownwards(true);

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

std::uint32_t
GameObject::getInstanceID() const noexcept
{
	return _instanceID;
}

void
GameObject::setParent(GameObjectPtr& parent) noexcept
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
			parent->_children.push_back(this->cast<GameObject>());

		this->_onMoveAfter();
	}
}

void
GameObject::setParent(GameObjectPtr&& parent) noexcept
{
	this->setParent(parent);
}

GameObjectPtr
GameObject::getParent() const noexcept
{
	return _parent.lock();
}

void
GameObject::addChild(GameObjectPtr& entity) noexcept
{
	assert(entity);
	entity->setParent(std::dynamic_pointer_cast<GameObject>(this->shared_from_this()));
}

void
GameObject::addChild(GameObjectPtr&& entity) noexcept
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
GameObject::removeChild(GameObjectPtr&& entity) noexcept
{
	this->removeChild(entity);
}

void
GameObject::cleanupChildren() noexcept
{
	for (auto& it : _children)
		it.reset();

	_children.clear();
}

GameObjectPtr
GameObject::findChild(const std::string& name, bool recuse) noexcept
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

std::uint32_t
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
	if (_translate != pos)
	{
		this->_onMoveBefore();

		_translate = pos;
		_needUpdates = true;

		this->_onMoveAfter();
	}
}

void
GameObject::setTranslateAccum(const float3& v) noexcept
{
	this->setTranslate(_translate + v);
}

const float3&
GameObject::getTranslate() const noexcept
{
	return _translate;
}

void
GameObject::setScale(const float3& pos) noexcept
{
	if (_scaling != pos)
	{
		this->_onMoveBefore();

		_scaling = pos;
		_needUpdates = true;

		this->_onMoveAfter();
	}
}

void
GameObject::setScaleAccum(const float3& scale) noexcept
{
	this->setScale(_scaling + scale);
}

const float3&
GameObject::getScale() const noexcept
{
	return _scaling;
}

void
GameObject::setQuaternion(const Quaternion& quat) noexcept
{
	if (_quat != quat)
	{
		this->_onMoveBefore();

		_quat = quat;
		_euler.makeRotate(_quat);
		_needUpdates = true;

		this->_onMoveAfter();
	}
}

void
GameObject::setQuaternionAccum(const Quaternion& quat) noexcept
{
	this->setQuaternion(math::cross(quat, _quat));
}

const Quaternion&
GameObject::getQuaternion() const noexcept
{
	return _quat;
}

void
GameObject::setEulerAngles(const EulerAngles& euler) noexcept
{
	if (_euler != euler)
	{
		this->_onMoveBefore();

		_euler = euler;
		_quat.makeRotate(euler);
		_needUpdates = true;

		this->_onMoveAfter();
	}
}

void
GameObject::setEulerAnglesAccum(const EulerAngles& euler) noexcept
{
	this->setEulerAngles(_euler + euler);
}

const EulerAngles&
GameObject::getEulerAngles() const noexcept
{
	return _euler;
}

const float3&
GameObject::getRight() const noexcept
{
	_updateTransform();
	return _transform.getRight();
}

const float3&
GameObject::getUpVector() const noexcept
{
	_updateTransform();
	return _transform.getUpVector();
}

const float3&
GameObject::getForward() const noexcept
{
	_updateTransform();
	return _transform.getForward();
}

void 
GameObject::setTransform(const float4x4& transform) noexcept
{
	this->_onMoveBefore();

	_transform = transform.getTransform(_translate, _quat, _scaling);
	_transformInverse = math::transformInverse(_transform);
	_transformInverseTranspose = math::transpose(_transformInverse);

	_euler.makeRotate(_quat);

	_needUpdates = false;

	this->_onMoveAfter();
}

void 
GameObject::setTransformOnlyRotate(const float4x4& transform) noexcept
{
	this->_onMoveBefore();

	_scaling = float3::One;
	_transform = transform.getTransformNoScale(_translate, _quat);
	_transformInverse = math::transformInverse(_transform);
	_transformInverseTranspose = math::transpose(_transformInverse);

	_euler.makeRotate(_quat);

	_needUpdates = false;

	this->_onMoveAfter();
}

const Matrix4x4&
GameObject::getTransform() const noexcept
{
	this->_updateTransform();
	return _transform;
}

const Matrix4x4&
GameObject::getTransformInverse() const noexcept
{
	this->_updateTransform();
	return _transformInverse;
}

const Matrix4x4&
GameObject::getTransformInverseTranspose() const noexcept
{
	this->_updateTransform();
	return _transformInverseTranspose;
}

void
GameObject::addComponent(GameComponentPtr& gameComponent) except
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
GameObject::removeComponent(GameComponentPtr& gameComponent) noexcept
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

	for (auto& it : _components)
	{
		if (it->isA(type))
			return it;
	}

	for (auto& it : _children)
	{
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
	for (auto& it : _components)
	{
		if (it->isA(type))
		{
			components.push_back(it);
			break;
		}			
	}

	for (auto& it : _children)
		it->getComponentsInChildren(type, components);
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
	assert(std::find(_dispatchComponents[type].begin(), _dispatchComponents[type].end(), component) == _dispatchComponents[type].end());
	
	if (this->getActive())
	{
		if (type == GameDispatchType::GameDispatchTypeFrame ||
			type == GameDispatchType::GameDispatchTypeFrameBegin ||
			type == GameDispatchType::GameDispatchTypeFrameEnd)
		{
			if (_dispatchComponents[GameDispatchTypeFrame].empty() &&
				_dispatchComponents[GameDispatchTypeFrameBegin].empty() &&
				_dispatchComponents[GameDispatchTypeFrameEnd].empty())
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

	auto it = std::find(_dispatchComponents[type].begin(), _dispatchComponents[type].end(), component);
	if (it != _dispatchComponents[type].end())
		_dispatchComponents[type].erase(it);

	if (this->getActive())
	{
		if (type == GameDispatchType::GameDispatchTypeFrame ||
			type == GameDispatchType::GameDispatchTypeFrameBegin ||
			type == GameDispatchType::GameDispatchTypeFrameEnd)
		{
			if (_dispatchComponents[GameDispatchTypeFrame].empty() &&
				_dispatchComponents[GameDispatchTypeFrameBegin].empty() &&
				_dispatchComponents[GameDispatchTypeFrameEnd].empty())
			{
				GameObjectManager::instance()->_activeObject(this, false);
			}
		}
	}
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

	for (auto& it : _components)
	{
		if (it->getActive())
			it->onMessage(message);
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
GameObject::load(iarchive& reader) noexcept
{
	std::string name;
	bool active;
	int layer;
	float3 position;
	float3 scale;
	float3 rotate;

	if (reader.getValue("name", name))
		this->setName(std::move(name));

	if (reader.getValue("active", active))
		this->setActive(active);

	if (reader.getValue("layer", layer))
		this->setLayer(layer);

	if (reader.getValue("position", position))
		this->setTranslate(position);

	if (reader.getValue("scale", scale))
		this->setScale(scale);

	if (reader.getValue("rotate", rotate))
		this->setEulerAngles(EulerAngles(rotate));
}

void
GameObject::save(oarchive& write) noexcept
{
}

GameObjectPtr
GameObject::clone() const noexcept
{
	auto instance = std::make_shared<GameObject>();
	instance->setParent(this->getParent());
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
	auto& components = _dispatchComponents[GameDispatchType::GameDispatchTypeFrameBegin];
	for (auto& it : components)
		it->onFrameBegin();
}

void
GameObject::_onFrame() except
{
	auto& components = _dispatchComponents[GameDispatchType::GameDispatchTypeFrame];
	for (auto& it : components)
		it->onFrame();
}

void
GameObject::_onFrameEnd() except
{
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

	if (!_dispatchComponents[GameDispatchTypeFrame].empty() ||
		!_dispatchComponents[GameDispatchTypeFrameBegin].empty() ||
		!_dispatchComponents[GameDispatchTypeFrameEnd].empty())
	{
		GameObjectManager::instance()->_activeObject(this, true);
	}
}

void 
GameObject::_onDeactivate() except
{
	if (!_dispatchComponents[GameDispatchTypeFrame].empty() ||
		!_dispatchComponents[GameDispatchTypeFrameBegin].empty() ||
		!_dispatchComponents[GameDispatchTypeFrameEnd].empty())
	{
		GameObjectManager::instance()->_activeObject(this, false);
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
	if (this->getActive())
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
	if (this->getActive())
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
GameObject::_updateTransform() const noexcept
{
	if (_needUpdates)
	{
		_transform.makeTransform(_translate, _quat, _scaling);
		_transformInverse = math::transformInverse(_transform);
		_transformInverseTranspose = math::transpose(_transformInverse);

		_needUpdates = false;
	}
}

_NAME_END