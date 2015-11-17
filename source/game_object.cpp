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
#include <ray/game_object.h>
#include <ray/game_object_manager.h>
#include <ray/game_component.h>

_NAME_BEGIN

__ImplementSubClass(GameObject, rtti::Interface, "Object")

GameObject::GameObject() noexcept
	: _active(false)
	, _layer(0)
	, _needUpdates(true)
	, _rotation(0, 0, 0, 1)
	, _scaling(1, 1, 1)
	, _translate(0, 0, 0)
	, _lookAt(0, 0, 0)
	, _upVector(0, 1, 0)
{
	_worldTransform.loadIdentity();
	_transform.loadIdentity();
	_transformInverse.loadIdentity();
	_transformInverseTranspose.loadIdentity();
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
		GameObjectManager::instance()->_activeObject(this, active);

		for (auto& it : _components)
			it->setActive(active);

		_active = active;
	}
}

void 
GameObject::setActiveUpwards(bool active) except
{
	if (_active != active)
	{
		for (auto& it : _components)
			it->setActive(active);

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
		for (auto& it : _components)
			it->setActive(active);

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
		for (auto& it : _components)
			it->onLayerChangeBefore();

		_layer = layer;

		for (auto& it : _components)
			it->onLayerChangeAfter();
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
GameObject::setParent(GameObjectPtr parent) noexcept
{
	assert(this != parent.get());

	auto _weak = _parent.lock();
	if (_weak != parent)
	{
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
			parent->_children.push_back(std::dynamic_pointer_cast<GameObject>(this->shared_from_this()));
	}
}

GameObjectPtr
GameObject::getParent() const noexcept
{
	return _parent.lock();
}

void
GameObject::addChild(GameObjectPtr entity) noexcept
{
	assert(entity);
	entity->setParent(std::dynamic_pointer_cast<GameObject>(this->shared_from_this()));
}

void
GameObject::removeChild(GameObjectPtr entity) noexcept
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
GameObject::setTranslateX(float dist) noexcept
{
	auto translate = this->getTranslate();
	translate.x = dist;
	this->setTranslate(translate);
}

void
GameObject::setTranslateY(float dist) noexcept
{
	auto translate = this->getTranslate();
	translate.y = dist;
	this->setTranslate(translate);
}

void
GameObject::setTranslateZ(float dist) noexcept
{
	auto translate = this->getTranslate();
	translate.z = dist;
	this->setTranslate(translate);
}

void
GameObject::setTranslateOnAxis(float dist, const Vector3& axis) noexcept
{
	if (axis.x) this->setTranslateX(dist);
	if (axis.y) this->setTranslateY(dist);
	if (axis.z) this->setTranslateZ(dist);
}

void
GameObject::setTranslateAccum(float v) noexcept
{
	this->setTranslateAccum(v, v, v);
}

void
GameObject::setTranslateAccum(float x, float y, float z) noexcept
{
	this->setTranslateAccum(Vector3(x, y, z));
}

void
GameObject::setTranslateAccumX(float dist) noexcept
{
	auto translate = this->getTranslate();
	translate.z += dist;
	this->setTranslate(translate);
}

void
GameObject::setTranslateAccumY(float dist) noexcept
{
	auto translate = this->getTranslate();
	translate.z += dist;
	this->setTranslate(translate);
}

void
GameObject::setTranslateAccumZ(float dist) noexcept
{
	auto translate = this->getTranslate();
	translate.z += dist;
	this->setTranslate(translate);
}

void
GameObject::setTranslateAccumOnAxis(float dist, const Vector3& axis) noexcept
{
	if (axis.x) this->setTranslateAccumX(dist);
	if (axis.y) this->setTranslateAccumY(dist);
	if (axis.z) this->setTranslateAccumZ(dist);
}

void
GameObject::setTranslateAccum(const Vector3& pos) noexcept
{
	this->setTranslate(this->getTranslate() + pos);
}

void
GameObject::setTranslate(const Vector3& pos) noexcept
{
	if (_translate != pos)
	{
		if (this->getActive())
		{
			this->_onMoveBefore();
		}

		_translate = pos;
		_needUpdates = true;

		if (this->getActive())
		{
			this->_onMoveAfter();
		}
	}
}

const Vector3&
GameObject::getTranslate() const noexcept
{
	return _translate;
}

void
GameObject::setScaleX(float dist) noexcept
{
	auto scale = this->getScale();
	scale.x = dist;
	this->setScale(scale);
}

void
GameObject::setScaleY(float dist) noexcept
{
	auto scale = this->getScale();
	scale.y = dist;
	this->setScale(scale);
}

void
GameObject::setScaleZ(float dist) noexcept
{
	auto scale = this->getScale();
	scale.z = dist;
	this->setScale(scale);
}

void
GameObject::setScaleOnAxis(float dist, const Vector3& axis) noexcept
{
	if (axis.x) this->setScaleX(dist);
	if (axis.y) this->setScaleY(dist);
	if (axis.z) this->setScaleZ(dist);
}

void
GameObject::setScaleAccum(float v) noexcept
{
	this->setScaleAccum(v, v, v);
}

void
GameObject::setScaleAccum(float x, float y, float z) noexcept
{
	this->setScaleAccum(Vector3(x, y, z));
}

void
GameObject::setScaleAccumX(float dist) noexcept
{
	auto scale = this->getScale();
	scale.z += dist;
	this->setScale(scale);
}

void
GameObject::setScaleAccumY(float dist) noexcept
{
	auto scale = this->getScale();
	scale.z += dist;
	this->setScale(scale);
}

void
GameObject::setScaleAccumZ(float dist) noexcept
{
	auto scale = this->getScale();
	scale.z += dist;
	this->setScale(scale);
}

void
GameObject::setScaleAccumOnAxis(float dist, const Vector3& axis) noexcept
{
	if (axis.x) this->setScaleAccumX(dist);
	if (axis.y) this->setScaleAccumY(dist);
	if (axis.z) this->setScaleAccumZ(dist);
}

void
GameObject::setScaleAccum(const Vector3& pos) noexcept
{
	this->setScale(this->getScale() + pos);
}

void
GameObject::setScale(const Vector3& pos) noexcept
{
	if (_scaling != pos)
	{
		if (this->getActive())
		{
			this->_onMoveBefore();
		}

		_scaling = pos;
		_needUpdates = true;

		if (this->getActive())
		{
			this->_onMoveAfter();
		}
	}
}

const Vector3&
GameObject::getScale() const noexcept
{
	return _scaling;
}

void
GameObject::setRotateX(float dist) noexcept
{
	auto rotate = this->getRotate();
	rotate.x = dist;
	this->setRotate(rotate);
}

void
GameObject::setRotateY(float dist) noexcept
{
	auto rotate = this->getRotate();
	rotate.y = dist;
	this->setRotate(rotate);
}

void
GameObject::setRotateZ(float dist) noexcept
{
	auto rotate = this->getRotate();
	rotate.z = dist;
	this->setRotate(rotate);
}

void
GameObject::setRotateOnAxis(float dist, const Quaternion& axis) noexcept
{
	if (axis.x) this->setRotateX(dist);
	if (axis.y) this->setRotateY(dist);
	if (axis.z) this->setRotateZ(dist);
}

void
GameObject::setRotateAccum(float v) noexcept
{
	this->setRotateAccum(v, v, v);
}

void
GameObject::setRotateAccum(float x, float y, float z) noexcept
{
	this->setRotateAccum(Quaternion(x, y, z));
}

void
GameObject::setRotateAccumX(float dist) noexcept
{
	auto rotate = this->getRotate();
	rotate.z += dist;
	this->setRotate(rotate);
}

void
GameObject::setRotateAccumY(float dist) noexcept
{
	auto rotate = this->getRotate();
	rotate.z += dist;
	this->setRotate(rotate);
}

void
GameObject::setRotateAccumZ(float dist) noexcept
{
	auto rotate = this->getRotate();
	rotate.z += dist;
	this->setRotate(rotate);
}

void
GameObject::setRotateAccumOnAxis(float dist, const Quaternion& axis) noexcept
{
	if (axis.x) this->setRotateAccumX(dist);
	if (axis.y) this->setRotateAccumY(dist);
	if (axis.z) this->setRotateAccumZ(dist);
}

void
GameObject::setRotateAccum(const Quaternion& rotate) noexcept
{
	this->setRotate(this->getRotate() + rotate);
}

void
GameObject::setRotate(const Quaternion& rotate) noexcept
{
	if (_rotation != rotate)
	{
		if (this->getActive())
		{
			this->_onMoveBefore();
		}

		_rotation = rotate;
		_needUpdates = true;

		if (this->getActive())
		{
			this->_onMoveAfter();
		}
	}
}

const Quaternion&
GameObject::getRotate() const noexcept
{
	return _rotation;
}

void
GameObject::setLookAtX(float dist) noexcept
{
	auto lookat = this->getLookAt();
	lookat.x = dist;
	this->setLookAt(lookat);
}

void
GameObject::setLookAtY(float dist) noexcept
{
	auto lookat = this->getLookAt();
	lookat.y = dist;
	this->setLookAt(lookat);
}

void
GameObject::setLookAtZ(float dist) noexcept
{
	auto lookat = this->getLookAt();
	lookat.z = dist;
	this->setLookAt(lookat);
}

void
GameObject::setLookAtOnAxis(float dist, const Vector3& axis) noexcept
{
	if (axis.x) this->setLookAtX(dist);
	if (axis.y) this->setLookAtY(dist);
	if (axis.z) this->setLookAtZ(dist);
}

void
GameObject::setLookAtAccum(float v) noexcept
{
	this->setLookAtAccum(v, v, v);
}

void
GameObject::setLookAtAccum(float x, float y, float z) noexcept
{
	this->setLookAtAccum(Vector3(x, y, z));
}

void
GameObject::setLookAtAccumX(float dist) noexcept
{
	auto lookat = this->getLookAt();
	lookat.z += dist;
	this->setLookAt(lookat);
}

void
GameObject::setLookAtAccumY(float dist) noexcept
{
	auto lookat = this->getLookAt();
	lookat.z += dist;
	this->setLookAt(lookat);
}

void
GameObject::setLookAtAccumZ(float dist) noexcept
{
	auto lookat = this->getLookAt();
	lookat.z += dist;
	this->setLookAt(lookat);
}

void
GameObject::setLookAtAccumOnAxis(float dist, const Vector3& axis) noexcept
{
	if (axis.x) this->setLookAtAccumX(dist);
	if (axis.y) this->setLookAtAccumY(dist);
	if (axis.z) this->setLookAtAccumZ(dist);
}

void
GameObject::setLookAtAccum(const Vector3& pos) noexcept
{
	this->setLookAt(this->getLookAt() + pos);
}

void
GameObject::setLookAt(const Vector3& pos) noexcept
{
	if (_lookAt != pos)
	{
		if (this->getActive())
		{
			this->_onMoveBefore();
		}

		_lookAt = pos;
		_needUpdates = true;

		if (this->getActive())
		{
			this->_onMoveAfter();
		}
	}
}

const Vector3&
GameObject::getLookAt() const noexcept
{
	return _lookAt;
}

void
GameObject::setUpVectorX(float dist) noexcept
{
	auto up = this->getUpVector();
	up.x = dist;
	this->setUpVector(up);
}

void
GameObject::setUpVectorY(float dist) noexcept
{
	auto up = this->getUpVector();
	up.y = dist;
	this->setUpVector(up);
}

void
GameObject::setUpVectorZ(float dist) noexcept
{
	auto up = this->getUpVector();
	up.z = dist;
	this->setUpVector(up);
}

void
GameObject::setUpVectorOnAxis(float dist, const Vector3& axis) noexcept
{
	if (axis.x) this->setUpVectorX(dist);
	if (axis.y) this->setUpVectorY(dist);
	if (axis.z) this->setUpVectorZ(dist);
}

void
GameObject::setUpVectorAccum(float v) noexcept
{
	this->setUpVectorAccum(v, v, v);
}

void
GameObject::setUpVectorAccum(float x, float y, float z) noexcept
{
	this->setUpVectorAccum(Vector3(x, y, z));
}

void
GameObject::setUpVectorAccumX(float dist) noexcept
{
	auto up = this->getUpVector();
	up.z += dist;
	this->setUpVector(up);
}

void
GameObject::setUpVectorAccumY(float dist) noexcept
{
	auto up = this->getUpVector();
	up.z += dist;
	this->setUpVector(up);
}

void
GameObject::setUpVectorAccumZ(float dist) noexcept
{
	auto up = this->getUpVector();
	up.z += dist;
	this->setUpVector(up);
}

void
GameObject::setUpVectorAccumOnAxis(float dist, const Vector3& axis) noexcept
{
	if (axis.x) this->setUpVectorAccumX(dist);
	if (axis.y) this->setUpVectorAccumY(dist);
	if (axis.z) this->setUpVectorAccumZ(dist);
}

void
GameObject::setUpVectorAccum(const Vector3& pos) noexcept
{
	this->setUpVector(this->getUpVector() + pos);
}

void
GameObject::setUpVector(const Vector3& up) noexcept
{
	if (_upVector != up)
	{
		if (this->getActive())
		{
			this->_onMoveBefore();
		}

		_upVector = up;
		_needUpdates = true;

		if (this->getActive())
		{
			this->_onMoveBefore();
		}
	}
}

const Vector3&
GameObject::getUpVector() const noexcept
{
	return _upVector;
}

void
GameObject::setTransform(const Matrix4x4& m) noexcept
{
	_transform = m;
	_transformInverse = m;
	_transformInverse.inverse();
	_transformInverseTranspose = Matrix4x4(_transformInverse).transpose();
}

void
GameObject::setTransformInverse(const Matrix4x4& m) noexcept
{
	_transform = m;
	_transformInverse = m;
	_transformInverse.inverse();
	_transformInverseTranspose = Matrix4x4(_transformInverse).transpose();
}

void
GameObject::setTransform(const Vector3& translate, const Quaternion& rotate, const Vector3& scale) noexcept
{
	this->setTranslate(translate);
	this->setRotate(rotate);
	this->setScale(scale);
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

const Matrix4x4&
GameObject::getWorldTransform() const noexcept
{
	this->_updateTransform();
	return _worldTransform;
}

void
GameObject::_updateTransform() const noexcept
{
	if (_needUpdates)
	{
		Matrix4x4 rotate;
		rotate.makeRotate(_rotation);

		_worldTransform = rotate;
		_worldTransform.setTranslate(_translate);

		_transform = _worldTransform;
		_transform.scale(_scaling);

		_transformInverse = _transform;
		_transformInverse.inverse();
		_transformInverseTranspose = rotate;

		_needUpdates = false;
	}
}

void
GameObject::addComponent(GameComponentPtr component) except
{
	assert(component);

	auto it = std::find_if(_components.begin(), _components.end(), [component](GameComponentPtr it) { return component->isInstanceOf(it->rtti()); });
	if (it == _components.end())
	{
		component->_setGameObject(this);
		if (this->getActive())
			component->onActivate();

		_components.push_back(component);

		for (auto& it : _components)
			it->onAttachComponent();
	}
}

void
GameObject::removeComponent(GameComponentPtr component) noexcept
{
	assert(component);

	auto it = std::find(_components.begin(), _components.end(), component);
	if (it != _components.end())
	{
		for (auto& it : _components)
			it->onDetachComponent();

		(*it)->_setGameObject(nullptr);
		_components.erase(it);
	}
}

void
GameObject::destroyComponent(GameComponentPtr component) noexcept
{
	assert(component);

	auto it = std::find(_components.begin(), _components.end(), component);
	if (it != _components.end())
	{
		for (auto& it : _components)
			it->onDetachComponent();

		(*it)->onDeactivate();
		(*it)->_setGameObject(nullptr);

		_components.erase(it);
	}
}

void
GameObject::cleanupComponents() noexcept
{
	if (this->getActive())
	{
		this->setActive(false);

		for (auto& it : _components)
			it->onDetachComponent();
	}

	_components.clear();
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
	for (auto& it : _components)
	{
		if (it->isA(type))
			return it;
	}

	return nullptr;
}

const GameComponents&
GameObject::getComponents() const noexcept
{
	return _components;
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
	if (!this->getActive())
		return;

	for (auto& it : _components)
		it->onMessage(message);
}

void
GameObject::sendMessageUpwards(const MessagePtr& message) noexcept
{
	if (!this->getActive())
		return;

	for (auto& it : _components)
	{
		it->onMessage(message);
	}

	auto parent = _parent.lock();
	if (parent)
		parent->sendMessageDownwards(message);
}

void
GameObject::sendMessageDownwards(const MessagePtr& message) noexcept
{
	if (!this->getActive())
		return;

	for (auto& it : _components)
	{
		it->onMessage(message);
	}

	for (auto& it : _children)
	{
		it->sendMessageDownwards(message);
	}
}

GameObjectPtr
GameObject::clone() const except
{
	auto instance = std::make_shared<GameObject>();
	instance->setParent(this->getParent());
	instance->setName(this->getName());
	instance->setLayer(this->getLayer());
	instance->setLookAt(this->getLookAt());
	instance->setUpVector(this->getUpVector());
	instance->setRotate(this->getRotate());
	instance->setScale(this->getScale());
	instance->setTranslate(this->getTranslate());

	for (auto& it : _components)
	{
		instance->addComponent(it->clone());
	}

	return instance;
}

void
GameObject::_onFrameBegin() except
{
	assert(this->getActive());

	for (auto& it : _components)
		it->onFrameBegin();
}

void
GameObject::_onFrame() except
{
	assert(this->getActive());

	for (auto& it : _components)
		it->onFrame();
}

void
GameObject::_onFrameEnd() except
{
	assert(this->getActive());

	for (auto& it : _components)
		it->onFrameEnd();
}

void
GameObject::_onMoveBefore() except
{
	for (auto& it : _components)
		it->onMoveBefore();

	for (auto& it : _children)
		it->_onMoveBefore();
}

void
GameObject::_onMoveAfter() except
{
	for (auto& it : _components)
		it->onMoveAfter();

	for (auto& it : _children)
		it->_onMoveAfter();
}

GameServer*
GameObject::getGameServer() noexcept
{
	auto parent = _parent.lock();
	if (parent)
		return parent->getGameServer();
	return nullptr;
}

GameScene*
GameObject::getGameScene() noexcept
{
	auto parent = _parent.lock();
	if (parent)
		return parent->getGameScene();
	return nullptr;
}

_NAME_END