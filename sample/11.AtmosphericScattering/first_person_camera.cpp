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
#include "first_person_camera.h"
#include <ray/game_server.h>
#include <ray/input_feature.h>
#include <ray/physics_character_component.h>
#include <ray/camera_component.h>

__ImplementSubClass(FirstPersonCameraComponent, GameComponent, "FirstPersonCamera")

FirstPersonCameraComponent::FirstPersonCameraComponent() noexcept
	: _speed(100.0)
	, _gravity(15)
	, _maxVelocityChange(1.0)
	, _jumpHeight(10)
	, _sensitivityX(150)
	, _sensitivityY(150)
{
}

FirstPersonCameraComponent::~FirstPersonCameraComponent() noexcept
{
}

void
FirstPersonCameraComponent::onActivate() noexcept
{
	auto cameraComponent = this->getComponent<ray::CameraComponent>();
	if (cameraComponent)
		_sensitivityX = _sensitivityY * cameraComponent->getRatio();

	this->addComponentDispatch(ray::GameDispatchType::GameDispatchTypeFrame, this);
}

void
FirstPersonCameraComponent::onDectivate() noexcept
{
	auto inputFeature = ray::GameServer::instance()->getFeature<ray::InputFeature>();
	if (inputFeature)
		inputFeature->getInput()->lockCursor(false);

	this->removeComponentDispatch(ray::GameDispatchType::GameDispatchTypeFrame, this);
}

void
FirstPersonCameraComponent::onFrame() noexcept
{
	float delta = ray::GameServer::instance()->getTimer()->delta();
	float step = _speed * delta;

	auto inputFeature = ray::GameServer::instance()->getFeature<ray::InputFeature>();
	if (inputFeature)
	{
		auto input = inputFeature->getInput();
		if (!input)
			return;

		if (input->getKey(ray::InputKey::Code::LSHIFT))
			step *= 3;

		auto character = this->getGameObject()->getComponent<ray::PhysicsCharacterComponent>();
		if (!character)
		{
			if (input->getKey(ray::InputKey::Code::W))
				moveCamera(step);

			if (input->getKey(ray::InputKey::Code::A))
				yawCamera(-step);

			if (input->getKey(ray::InputKey::Code::S))
				moveCamera(-step);

			if (input->getKey(ray::InputKey::Code::D))
				yawCamera(step);
		}
		else
		{
			const ray::float3& forward = this->getGameObject()->getForward();
			const ray::float3& right = this->getGameObject()->getRight();

			ray::float3 walkDirection(0, 0, 0);
			if (input->getKey(ray::InputKey::Code::W))
			{
				ray::float3 mov = forward;
				walkDirection += mov;
			}
			if (input->getKey(ray::InputKey::Code::S))
			{
				ray::float3 mov = forward;
				walkDirection -= mov;
			}
			if (input->getKey(ray::InputKey::Code::A))
			{
				ray::float3 mov = right;
				walkDirection -= mov;
			}
			if (input->getKey(ray::InputKey::Code::D))
			{
				ray::float3 mov = right;
				walkDirection += mov;
			}

			character->setWalkDirection(walkDirection * step);

			if (input->getKey(ray::InputKey::SPACE))
			{
				if (character->canJumping())
					character->jump(_jumpHeight);
			}
		}

		if (input->getKey(ray::InputKey::ESCAPE))
		{
			input->lockCursor(false);
		}

		if (input->isLockedCursor())
		{
			rotateCamera(input->getAxisX(), input->getAxisY());
		}
		else
		{
			if (input->getButtonDown(ray::InputButton::Code::LEFT) &&
				!input->getKey(ray::InputKey::Code::LCTRL))
			{
				input->lockCursor(true);
			}
		}
	}
}

void
FirstPersonCameraComponent::yawCamera(float speed) noexcept
{
	const ray::Vector3& right = this->getGameObject()->getRight();
	this->getGameObject()->setTranslateAccum(right * speed);
}

void
FirstPersonCameraComponent::moveCamera(float speed) noexcept
{
	const ray::Vector3& forward = this->getGameObject()->getForward();
	this->getGameObject()->setTranslateAccum(forward * speed);
}

void
FirstPersonCameraComponent::rotateCamera(float angle, const ray::float3& axis) noexcept
{
	ray::Quaternion quat(axis, angle);
	this->getGameObject()->setQuaternionAccum(quat);
}

void
FirstPersonCameraComponent::rotateCamera(float axisX, float axisY) noexcept
{
	float angleY = axisX * _sensitivityX;
	float angleX = axisY * _sensitivityY;

	ray::EulerAngles euler(this->getGameObject()->getQuaternion());

	float angle = angleX + euler.x;
	if (angle > -89.0f && angle < 89.0f && !std::isinf(angle))
		rotateCamera(angleX, this->getGameObject()->getRight());

	rotateCamera(angleY, ray::float3::UnitY);
}

ray::GameComponentPtr
FirstPersonCameraComponent::clone() const noexcept
{
	return std::make_shared<FirstPersonCameraComponent>();
}