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
#if defined(_BUILD_RENDERER)
#include <ray/sprite_component.h>

_NAME_BEGIN

__ImplementSubClass(ParticleEmitterComponent, GameComponent, "ParticleEmitter")

ParticleEmitterComponent::ParticleEmitterComponent() noexcept
	: _enable(true)
	, _mode(Billboard)
	, _time(0)
	, _emission(0)
	, _emission_min(0)
	, _emission_max(1000)
	, _emission_speed(100)
	, _emission_velocity_scale(1.0)
	, _emission_direction(Vector3::Zero)
	, _particle_count(0)
	, _age_min(0)
	, _age_max(10)
	, _position_start(-Vector3::One * 10)
	, _position_end(Vector3::One * 10)
	, _position_variance(Vector3::One)
	, _size_start(Vector3::One)
	, _size_end(Vector3::One)
	, _size_variance(Vector3::One)
	, _spin_start(-Vector3::One)
	, _spin_end(Vector3::One)
	, _spin_variance(Vector3::One)
	, _color_start(Vector3::Zero)
	, _color_end(Vector3::One)
	, _color_variance(Vector3::One)
	, _speed_min(-Vector3::One)
	, _speed_max(Vector3::One)
	, _speed_variance(Vector3::One)
	, _gravity_min(-Vector3::One)
	, _gravity_max(Vector3::One)
	, _gravity_variance(Vector3::One)
	, _radial_min(-Vector3::One)
	, _radial_max(Vector3::One)
	, _radial_variance(Vector3::One)
	, _tangential_min(-Vector3::One)
	, _tangential_max(Vector3::One)
	, _tangential_variance(Vector3::One)
{
}

ParticleEmitterComponent::~ParticleEmitterComponent() noexcept
{
}

void
ParticleEmitterComponent::emitter()
{
	Particle* dot = new Particle;

	this->emitter(dot);

	_particles.push_back(dot);
}

void
ParticleEmitterComponent::emitter(Particle* it) noexcept
{
	math::randomize();

	auto& position = this->getGameObject()->getTranslate();

	it->age = math::random(_age_min, _age_max);

	it->position = math::random(position + _position_start, position + math::lerp(_position_start, _position_end, _position_variance));

	it->size = math::random(_size_start, math::lerp(_size_start, _size_end, _size_variance));
	it->sizeDelta = math::random(_size_start, math::lerp(_size_start, _size_end, it->age));

	it->spin = math::random(_spin_start, math::lerp(_spin_start, _spin_end, _spin_variance));
	it->spinDelta = math::random(_spin_start, math::lerp(_spin_start, _spin_end, it->age));

	it->color = math::random(_color_start, math::lerp(_color_start, _color_end, _color_variance));
	it->colorDelta = math::random(_color_start, math::lerp(_color_start, _color_end, it->age));

	it->velocity = math::random(_speed_min, math::lerp(_speed_min, _speed_max, _speed_variance));

	it->gravity = math::random(_gravity_min, math::lerp(_gravity_min, _gravity_max, _gravity_variance));

	it->radialAccel = math::random(_radial_min, math::lerp(_radial_min, _radial_max, _radial_variance));

	it->tangentialAccel = math::random(_tangential_min, math::lerp(_tangential_min, _tangential_max, _tangential_variance));
}

void
ParticleEmitterComponent::simulate(float delta) noexcept
{
	std::size_t emissionCount = 0;

	std::size_t count = (std::size_t)floor(_emission_speed * delta);
	std::size_t maxCount = (std::size_t)floor(_emission_max - _emission);

	if (_emission < _emission_max)
	{
		if (count > maxCount)
			count = maxCount;

		for (emissionCount = 0; emissionCount < count; emissionCount++)
		{
			this->emitter();
		}
	}

	for (auto& it : _particles)
	{
		it->age -= delta;

		if (it->age > 0.0)
		{
			Vector3 accel1, accel2;

			accel1 = it->position - this->getGameObject()->getTranslate();
			accel1 *= math::rsqrt(math::length2(accel1));
			accel2 = accel1;
			accel1 *= it->radialAccel;

			float ang = accel2.x;
			accel2.x -= accel2.y;
			accel2.y = ang;

			accel2 *= it->tangentialAccel;

			it->velocity += (accel1 + accel2)* delta;
			it->velocity += it->gravity * delta;

			it->position = it->position + it->velocity * delta;
			it->spin = it->spin + it->spinDelta * delta;
			it->size = it->size + it->sizeDelta * delta;
			it->color = it->color + it->colorDelta * delta;
		}
		else
		{
			if (emissionCount < count)
			{
				this->emitter(it);
			}
		}
	}
}

GameComponentPtr
ParticleEmitterComponent::clone() const noexcept
{
	return std::make_shared<ParticleEmitterComponent>();
}

_NAME_END
#endif