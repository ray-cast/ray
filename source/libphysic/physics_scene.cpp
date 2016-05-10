// +---------------------------------------------------------------------
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
#include <ray/physics_scene.h>
#include <ray/physics_system.h>
#include "bullet_types.h"

_NAME_BEGIN

PhysicsScene::Setting::Setting() noexcept
	: length(1.0f)
	, mass(1000.0f)
	, speed(10.0f)
	, skinWidth(0.0001f)
	, gravity(0.0f, -9.81f, 0.0f)
{
	aabb.min = Vector3(-1000, -1000, -1000);
	aabb.max = Vector3(1000, 1000, 1000);
}

PhysicsScene::PhysicsScene() noexcept
	: _collisionConfiguration(nullptr)
	, _dispatcher(nullptr)
	, _broadphase(nullptr)
	, _solver(nullptr)
	, _dynamicsWorld(nullptr)
	, _isFetchResult(false)
{
}

PhysicsScene::~PhysicsScene() noexcept
{
	this->close();
}

void
PhysicsScene::setup(const Setting& setting) noexcept
{
	_setting = setting;

	_collisionConfiguration = new btDefaultCollisionConfiguration();
	_dispatcher = new btCollisionDispatcher(_collisionConfiguration);

	btVector3 min(setting.aabb.min.x, setting.aabb.min.y, setting.aabb.min.z);
	btVector3 max(setting.aabb.max.x, setting.aabb.max.y, setting.aabb.max.z);

	_broadphase = new btAxisSweep3(min, max);
	_broadphase->getOverlappingPairCache()->setInternalGhostPairCallback(new btGhostPairCallback());

	_solver = new btSequentialImpulseConstraintSolver;

	_dynamicsWorld = new btDiscreteDynamicsWorld(_dispatcher, _broadphase, _solver, _collisionConfiguration);
	_dynamicsWorld->getDispatchInfo().m_allowedCcdPenetration = setting.skinWidth;

	this->setGravity(setting.gravity);
}

void
PhysicsScene::close() noexcept
{
	if (_collisionConfiguration)
	{
		delete _collisionConfiguration;
		_collisionConfiguration = nullptr;
	}

	if (_dispatcher)
	{
		delete _dispatcher;
		_dispatcher = nullptr;
	}

	if (_solver)
	{
		delete _solver;
		_solver = nullptr;
	}

	if (_broadphase)
	{
		delete _broadphase;
		_broadphase = nullptr;
	}

	if (_dynamicsWorld)
	{
		delete _dynamicsWorld;
		_dynamicsWorld = nullptr;
	}
}

void
PhysicsScene::setLength(float length) noexcept
{
	_setting.length = length;
}

void
PhysicsScene::setMass(float mass) noexcept
{
	_setting.mass = mass;
}

void
PhysicsScene::setSpeed(float speed) noexcept
{
	_setting.speed = speed;
}

void
PhysicsScene::setSkitWindow(float width) noexcept
{
	_setting.skinWidth = width;
}

void
PhysicsScene::setGravity(const Vector3& v) noexcept
{
	if (_dynamicsWorld)
		_dynamicsWorld->setGravity(btVector3(v.x, v.y, v.z));

	_setting.gravity = v;
}

float
PhysicsScene::getLength() const noexcept
{
	return _setting.length;
}

float
PhysicsScene::getMass() const noexcept
{
	return _setting.mass;
}

float
PhysicsScene::getSpeed() const noexcept
{
	return _setting.speed;
}

float
PhysicsScene::getSkitWindow() const noexcept
{
	return _setting.skinWidth;
}

const Vector3&
PhysicsScene::getGravity() const noexcept
{
	return _setting.gravity;
}

bool 
PhysicsScene::isFetchResult() const noexcept
{
	return _isFetchResult;
}

/*int
PhysicsScene::raycast(const Vector3& rayFromWorld, const Vector3& rayToWorld, RaycastHit& hit)
{
	btVector3 from(rayFromWorld.x, rayFromWorld.y, rayFromWorld.z);
	btVector3 to(rayToWorld.x, rayToWorld.y, rayToWorld.z);

	btCollisionWorld::ClosestRayResultCallback callback(from, to);
	_dynamicsWorld->rayTest(from, to, callback);
	if (callback.hasHit())
	{
		const btRigidBody* body = btRigidBody::upcast(callback.m_collisionObject);
		if (body)
		{
			hit.point.x = callback.m_hitPointWorld.x();
			hit.point.y = callback.m_hitPointWorld.y();
			hit.point.z = callback.m_hitPointWorld.z();

			hit.normal.x = callback.m_hitNormalWorld.x();
			hit.normal.y = callback.m_hitNormalWorld.y();
			hit.normal.z = callback.m_hitNormalWorld.z();

			hit.distance = callback.m_closestHitFraction;

			auto rigidbody = Rigidbody::find((InstanceID)body->getUserIndex());
			assert(rigidbody);

			hit.collider = rigidbody->getComponent<Collider>();
			hit.rigidbody = rigidbody->getComponent<Rigidbody>();
			hit.transform = rigidbody->getComponent<Transform>();

			return 1;
		}
	}

	return 0;
}

int
PhysicsScene::raycastAll(const Vector3& rayFromWorld, const Vector3& rayToWorld, RaycastHits& hits)
{
	btVector3 from(rayFromWorld.x, rayFromWorld.y, rayFromWorld.z);
	btVector3 to(rayToWorld.x, rayToWorld.y, rayToWorld.z);

	btCollisionWorld::AllHitsRayResultCallback callback(from, to);
	_dynamicsWorld->rayTest(from, to, callback);
	if (callback.hasHit())
	{
		for (int i = 0; i < callback.m_collisionObjects.size(); i++)
		{
			const btCollisionObject* collision = callback.m_collisionObjects[i];
			const btRigidBody* body = btRigidBody::upcast(collision);
			if (body)
			{
				RaycastHit hit;

				hit.point.x = callback.m_hitPointWorld[i].x();
				hit.point.y = callback.m_hitPointWorld[i].y();
				hit.point.z = callback.m_hitPointWorld[i].z();

				hit.normal.x = callback.m_hitNormalWorld[i].x();
				hit.normal.y = callback.m_hitNormalWorld[i].y();
				hit.normal.z = callback.m_hitNormalWorld[i].z();

				hit.distance = callback.m_hitFractions[i];

				auto gameobj = Entity::find(body->getUserIndex());
				assert(gameobj);

				hit.collider = gameobj->getComponent<Collider>();
				hit.rigidbody = gameobj->getComponent<Rigidbody>();
				hit.transform = gameobj->getComponent<Transform>();

				hits.push_back(hit);
			}
		}

		return hits.size();
	}

	return 0;
}
*/

void 
PhysicsScene::addJoint(btTypedConstraint* joint) noexcept
{
	_dynamicsWorld->addConstraint(joint);
}

void 
PhysicsScene::removeJoint(btTypedConstraint* joint) noexcept
{
	_dynamicsWorld->removeConstraint(joint);
}

void
PhysicsScene::addRigidbody(PhysicsBody* body) noexcept
{
	_dynamicsWorld->addRigidBody(body->getRigidbody(), 1 << body->getLayer(), body->getLayerMask());
	_rigidbodys.push_back(body);
}

void
PhysicsScene::removeRigidbody(PhysicsBody* body) noexcept
{
	_dynamicsWorld->removeRigidBody(body->getRigidbody());

	auto it = std::find(_rigidbodys.begin(), _rigidbodys.end(), body);
	if (it != _rigidbodys.end())
	{
		_rigidbodys.erase(it);
	}
}

void
PhysicsScene::addCharacter(btCollisionObject* object) noexcept
{
	_dynamicsWorld->addCollisionObject(object, btBroadphaseProxy::CharacterFilter, btBroadphaseProxy::StaticFilter | btBroadphaseProxy::DefaultFilter);
}

void
PhysicsScene::removeCharacter(btCollisionObject* object) noexcept
{
	_dynamicsWorld->removeCollisionObject(object);
}

void
PhysicsScene::addAction(btActionInterface* action) noexcept
{
	_dynamicsWorld->addAction(action);
}

void
PhysicsScene::removeAction(btActionInterface* action) noexcept
{
	_dynamicsWorld->removeAction(action);
}

void
PhysicsScene::simulation(float delta) noexcept
{
	_isFetchResult = true;

	/*if (_isEnableForce)
	{
		_body->addForce(_constantForce);
		_body->addTorque(_constantTorque);
		_body->setLinearVelocity(_body->getLinearVelocity() + _constantVelocity);
		_body->setAngularVelocity(_body->getAngularVelocity() + _constantAngularVelocity);
	}*/

	_dynamicsWorld->stepSimulation(delta);

	auto dispatcher = _dynamicsWorld->getDispatcher();
	if (dispatcher)
	{
		auto numManifolds = _dispatcher->getNumManifolds();
		for (int i = 0; i < numManifolds; i++)
		{
			auto contactManifold = _dispatcher->getManifoldByIndexInternal(i);
			if (contactManifold->getNumContacts() == 0)
				continue;

			auto objA = contactManifold->getBody0();
			auto objB = contactManifold->getBody1();

			auto objAPointer = objA->getUserPointer();
			auto objBPointer = objB->getUserPointer();

			if (objAPointer)
			{
				PhysicsBody* bodyA = (PhysicsBody*)objAPointer;
				auto listenerA = bodyA->getRigidbodyListener();
				if (listenerA)
					listenerA->onCollisionStay();
			}

			if (objBPointer)
			{
				PhysicsBody* bodyB = (PhysicsBody*)objBPointer;
				auto listenerB = bodyB->getRigidbodyListener();
				if (listenerB)
					listenerB->onCollisionStay();
			}
		}
	}

	for (auto& it : _rigidbodys)
	{
		if (math::equals(it->getMass(), 0.0f))
			continue;

		auto listener = it->getRigidbodyListener();
		if (listener)
			listener->onFetchResult();
	}

	_isFetchResult = false;
}

_NAME_END